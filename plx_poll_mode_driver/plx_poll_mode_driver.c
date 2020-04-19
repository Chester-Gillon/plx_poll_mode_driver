
#include <stdlib.h>
#include <stdio.h>

#include <PlxApi.h>

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

    /* Close the device */
    status = PlxPci_DeviceClose (&device);
    if (status != ApiSuccess)
    {
        printf ("PlxPci_DeviceClose failed : %d\n", status);
        exit (EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
