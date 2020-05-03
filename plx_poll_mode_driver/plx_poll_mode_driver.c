
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "serial_reg.h"

#include <PlxApi.h>


/* The number of 16C950 UARTs on the Sealevel COMM+2.LPCIe board (7205e) */
#define NUM_UARTS 2


/* Structure to access one 16C950 UART, as a 8-bit wide device on the local bus of a PEX8311.
 * Each UART is mapped as one bar in memory space. */
typedef struct
{
    /** The open PLX device in which the UART is mapped */
    PLX_DEVICE_OBJECT *device;
    /** The index of the PCI BAR to which the UART is mapped */
    U8 bar_index;
    /** If non-NULL the virtual address which is mapped to the PCI BAR to allow direct access to the UART registers */
    void *bar_mapping;
    /** Tracks Additional Control Register */
    U8 acr;
} uart_port_t;


/**
 * @brief Write to a UART register
 * @details If the port has it's BAR mapped into the process virtual address space the mapping is used
 *          to write to the UART register, otherwise a PlxApi function is used.
 * @param[in] port Which UART to write to
 * @param[in] offset The register offset to write to
 * @param[in] value The register value to write
 */
static void serial_out (uart_port_t *const port, const U32 offset, const U8 value)
{
    PLX_STATUS status;
    U8 data = value;

    if (port->bar_mapping != NULL)
    {
        volatile U8 *const port_base = port->bar_mapping;

        port_base[offset] = data;
    }
    else
    {
        status = PlxPci_PciBarSpaceWrite (port->device, port->bar_index, offset, &data, 1, BitSize8, FALSE);
        if (status != ApiSuccess)
        {
            printf ("PlxPci_PciBarSpaceWrite failed : %d : bar_index=%u offset=%u\n", status, port->bar_index, offset);
            exit (EXIT_FAILURE);
        }
    }
}


/**
 * @brief Read from a UART register
 * @details If the port has it's BAR mapped into the process virtual address space the mapping is used
 *          to read from the UART register, otherwise a PlxApi function is used.
 * @param[in] port Whuch UART to read from
 * @param[in] offset The register offset to read from
 * @return The register value
 */
static U8 serial_in (uart_port_t *const port, const U32 offset)
{
    PLX_STATUS status;
    U8 data;

    if (port->bar_mapping != NULL)
    {
        const volatile U8 *const port_base = port->bar_mapping;

        data = port_base[offset];
    }
    else
    {
        status = PlxPci_PciBarSpaceRead (port->device, port->bar_index, offset, &data, 1, BitSize8, FALSE);
        if (status != ApiSuccess)
        {
            printf ("PlxPci_PciBarSpaceRead failed : %d : bar_index=%u offset=%u\n", status, port->bar_index, offset);
            exit (EXIT_FAILURE);
        }
    }

    return data;
}


/*
 * For the 16C950
 */
static void serial_icr_write (uart_port_t *const port, const U32 offset, const U8 value)
{
    serial_out (port, UART_SCR, offset);
    serial_out (port, UART_ICR, value);
}

static unsigned int serial_icr_read (uart_port_t *const port, const U32 offset)
{
    unsigned int value;

    serial_icr_write (port, UART_ACR, port->acr | UART_ACR_ICRRD);
    serial_out (port, UART_SCR, offset);
    value = serial_in (port, UART_ICR);
    serial_icr_write (port, UART_ACR, port->acr);

    return value;
}


/**
 * @brief Read the ID bytes of the UART checking that find a 16C950
 * @param[in] port Which UART to auto-detect
 */
static void check_16c950_id (uart_port_t *const port)
{
    /*
     * The 16C950 requires 0xbf to be written to the LCR to read the ID.
     */
    serial_out (port, UART_LCR, UART_LCR_CONF_MODE_B);
    if (serial_in (port, UART_EFR) == 0)
    {
        U8 id1, id2, id3, rev;

        /*
         * Check for Oxford Semiconductor 16C950.
         */
        port->acr = 0;
        serial_out (port, UART_LCR, UART_LCR_CONF_MODE_B);
        serial_out (port, UART_EFR, UART_EFR_ECB);
        serial_out (port, UART_LCR, 0x00);
        id1 = serial_icr_read (port, UART_ID1);
        id2 = serial_icr_read (port, UART_ID2);
        id3 = serial_icr_read (port, UART_ID3);
        rev = serial_icr_read (port, UART_REV);

        if ((id1 == 0x16) && (id2 == 0xC9) && (id3 == 0x50) && (rev == 0x03))
        {
            printf ("Detected 16C950 rev B on bar_index %u\n", port->bar_index);
        }
        else
        {
            printf ("Unknown EFR device on bar_index=%u : id1=%x id2=%x id3=%x rec=%x\n",
                    port->bar_index, id1, id2, id3, rev);
            exit (EXIT_FAILURE);
        }
    }
    else
    {
        printf ("Unknown EFR trying to read ID on bar_index %u\n", port->bar_index);
        exit (EXIT_FAILURE);
    }
}


/**
 * @brief Perform an auto-detection sequence, on which should be an OX16C950 UART.
 * @details This is a cutdown sequence from the Linux Kernel 8250_core.c, excluding tests not applicable
 *          to the expected UART.
 * @param[in] port Which UART to auto-detect
 */
static void autoconfig (uart_port_t *const port)
{
    U8 status1, scratch, scratch2, scratch3;
    U8 save_lcr, save_mcr;

    /* Do a simple existence test first, on the Interrupt Enable Register */
    scratch = serial_in (port, UART_IER);
    serial_out (port, UART_IER, 0);

    /*
     * Mask out IER[7:4] bits for test as some UARTs (e.g. TL
     * 16C754B) allow only to modify them if an EFR bit is set.
     */
    scratch2 = serial_in (port, UART_IER) & 0x0f;
    serial_out (port, UART_IER, 0x0F);

    scratch3 = serial_in (port, UART_IER) & 0x0f;
    serial_out (port, UART_IER, scratch);
    if ((scratch2 != 0) || (scratch3 != 0x0F))
    {
        printf ("IER test failed (%02x, %02x)\n", scratch2, scratch3);
        exit (EXIT_FAILURE);
    }

    save_mcr = serial_in (port, UART_MCR);
    save_lcr = serial_in (port, UART_LCR);

    /* Check to see if a UART is really there, by performing a loopback test on the modem status bits */
    serial_out (port, UART_MCR, UART_MCR_LOOP | 0x0A);
    status1 = serial_in (port, UART_MSR) & 0xF0;
    serial_out (port, UART_MCR, save_mcr);
    if (status1 != 0x90)
    {
        printf ("LOOP test failed (%02x)\n", status1);
    }

    /*
     * We're pretty sure there's a port here.  Lets find out what
     * type of port it is.  The IIR top two bits allows us to find
     * out if it's 8250 or 16450, 16550, 16550A or later.  This
     * determines what we test for next.
     *
     * We also initialise the EFR (if any) to zero for later.  The
     * EFR occupies the same register location as the FCR and IIR.
     */
    serial_out (port, UART_LCR, UART_LCR_CONF_MODE_B);
    serial_out (port, UART_EFR, 0);
    serial_out (port, UART_LCR, 0);

    serial_out (port, UART_FCR, UART_FCR_ENABLE_FIFO);

    scratch = serial_in (port, UART_IIR) >> 6;
    switch (scratch)
    {
    case 0:
        printf ("IIR Unexpected ID of 8250\n");
        exit (EXIT_FAILURE);
        break;
    case 1:
        printf ("IIR Unknown ID\n");
        exit (EXIT_FAILURE);
        break;
    case 2:
        printf ("IIR Unexpected ID of 16550\n");
        exit (EXIT_FAILURE);
        break;
    case 3:
        check_16c950_id (port);
        break;
    }
}


/**
 * @brief Sequence the UART tests, which demonstrate using the PlxApi
 * @param[in] device The open PLX device in which the UARTs are mapped
 * @param[in] use_bar_mapping Controls how the UART registers are accessed for the tests:
 *            - When false PlxApi functions are used for each register access, involving user/Kernel space
 *              transitions.
 *            - When true the UART registers are mapped into the application virtual address space.
 */
static void perform_uart_tests (PLX_DEVICE_OBJECT *const device, const BOOL use_bar_mapping)
{
    uart_port_t ports[NUM_UARTS];
    unsigned int port_index;
    PLX_STATUS status;

    /* Initialise ports to access both UARTS on the board */
    memset (ports, 0, sizeof (ports));
    ports[0].device = device;
    ports[0].bar_index = 2;
    ports[1].device = device;
    ports[1].bar_index = 3;

    /* Map the BAR of the ports into virtual address when requested */
    if (use_bar_mapping)
    {
        printf ("Performing tests with UART registers mapped into virtual address space\n");
        for (port_index = 0; port_index < NUM_UARTS; port_index++)
        {
            uart_port_t *const port = &ports[port_index];

            status = PlxPci_PciBarMap (device, port->bar_index, &port->bar_mapping);
            if (status != ApiSuccess)
            {
                printf ("PlxPci_PciBarMap failed : %d : bar_index=%u\n", status, port->bar_index);
                exit (EXIT_FAILURE);
            }
        }
    }
    else
    {
        printf ("Performing tests using PlxPci_PciBarSpaceWrite() / PlxPci_PciBarSpaceRead() to access UART registers"
                ", which involves user/Kernel space transitions.\n");
    }

    /* Perform tests which detect the type of UART */
    for (port_index = 0; port_index < NUM_UARTS; port_index++)
    {
        autoconfig (&ports[port_index]);
    }

    /* Unmap the BARs of the ports (if were mapped) */
    for (port_index = 0; port_index < NUM_UARTS; port_index++)
    {
        uart_port_t *const port = &ports[port_index];

        if (port->bar_mapping != NULL)
        {
            status = PlxPci_PciBarUnmap (device, &port->bar_mapping);
            if (status != ApiSuccess)
            {
                printf ("PlxPci_PciBarUnmap failed : %d : bar_index=%u\n", status, port->bar_index);
                exit (EXIT_FAILURE);
            }
        }
    }
}


int main (int argc, char *argv[])
{
    PLX_STATUS status;
    PLX_DEVICE_KEY deviceKey;
    PLX_DEVICE_OBJECT device;
    U8 apiVersionMajor;
    U8 apiVersionMinor;
    U8 apiVersionRevision;
    U16 deviceNumber;
    U8 driverVersionMajor;
    U8 driverVersionMinor;
    U8 driverVersionRevision;
    BOOL use_bar_mapping = FALSE;

    /* Read optional command line argument which specifies if to access UART registers with a BAR mapping.
     * Default is disabled. */
    if (argc >= 2)
    {
        use_bar_mapping = atoi (argv[1]) != 0;
    }

    /* Get the PlxSdk API version this program is linked against, which doesn't require any communication with an actual device */
    status = PlxPci_ApiVersion (&apiVersionMajor, &apiVersionMinor, &apiVersionRevision);
    if (status != ApiSuccess)
    {
        printf ("PlxPci_ApiVersion failed : %d\n", status);
        exit (EXIT_FAILURE);
    }
    printf ("PlxPci_ApiVersion : versionMajor=%u versionMinor=%u versionRevison=%u\n", apiVersionMajor, apiVersionMinor, apiVersionRevision);

    /* Find the first SIO4 board, on the assumption that a maximum of one fitted */
    memset (&deviceKey, PCI_FIELD_IGNORE, sizeof (deviceKey));
    deviceNumber = 0;
    deviceKey.VendorId = 0x10b5;
    deviceKey.DeviceId = 0x9056;
    deviceKey.SubVendorId = 0x10b5;
    deviceKey.SubDeviceId = 0x3198;
    status = PlxPci_DeviceFind (&deviceKey, deviceNumber);
    if (status != ApiSuccess)
    {
        printf ("Unable to find a SIO4 board. PlxPci_DeviceFind returned %d\n", status);
        exit (EXIT_FAILURE);
    }
    printf ("Found SIO4 board at bus=%u slot=%u function=%u\n", deviceKey.bus, deviceKey.slot, deviceKey.function);

    /* Open the device found */
    memset (&device, 0, sizeof (device));
    status = PlxPci_DeviceOpen (&deviceKey, &device);
    if (status != ApiSuccess)
    {
        printf ("PlxPci_DeviceOpen failed : %d\n", status);
        exit (EXIT_FAILURE);
    }

    /* Obtain the driver version, and check that if matches the API version.
       If the versions don't match then abort the program as may lead to problems */
    status = PlxPci_DriverVersion (&device, &driverVersionMajor, &driverVersionMinor, &driverVersionRevision);
    if (status != ApiSuccess)
    {
        printf ("PlxPci_DriverVersion failed : %d\n", status);
        exit (EXIT_FAILURE);
    }
    printf ("PlxPci_DriverVersion : versionMajor=%u versionMinor=%u versionRevison=%u\n", driverVersionMajor, driverVersionMinor, driverVersionRevision);
    if ((apiVersionMajor != driverVersionMajor) || (apiVersionMinor != driverVersionMinor) || (apiVersionRevision != driverVersionRevision))
    {
        printf ("Aborting as the API and driver versions don't match\n");
        exit (EXIT_FAILURE);
    }

    /* Perform the tests */
    perform_uart_tests (&device, use_bar_mapping);

    /* Close the device */
    status = PlxPci_DeviceClose (&device);
    if (status != ApiSuccess)
    {
        printf ("PlxPci_DeviceClose failed : %d\n", status);
        exit (EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
