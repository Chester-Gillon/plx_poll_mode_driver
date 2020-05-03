/*
 * @file serial_reg.h
 * @date May 2, 2020
 * @author Chester Gillon
 * @details
 *  These are the UART port assignments, expressed as offsets from the base
 *  register.  These assignments should hold for any serial port based on
 *  a 8250, 16450, or 16550(A).
 *
 *  A sub-set of the definitions in the Linux Kernel include/linux/serial_reg.h
 */

#ifndef SERIAL_REG_H_
#define SERIAL_REG_H_

/*
 * DLAB=0
 */

#define UART_IER    1   /* Out: Interrupt Enable Register */

#define UART_IIR    2   /* In:  Interrupt ID Register */

#define UART_FCR    2   /* Out: FIFO Control Register */
#define UART_FCR_ENABLE_FIFO    0x01 /* Enable the FIFO */

#define UART_LCR    3   /* Out: Line Control Register */

/*
 * Access to some registers depends on register access / configuration
 * mode.
 */
#define UART_LCR_CONF_MODE_A    UART_LCR_DLAB   /* Configutation mode A */
#define UART_LCR_CONF_MODE_B    0xBF        /* Configutation mode B */

#define UART_MCR    4   /* Out: Modem Control Register */
#define UART_MCR_LOOP       0x10 /* Enable loopback test mode */

#define UART_MSR    6   /* In:  Modem Status Register */

#define UART_SCR    7   /* I/O: Scratch Register */

/*
 * LCR=0xBF (or DLAB=1 for 16C660)
 */
#define UART_EFR    2   /* I/O: Extended Features Register */
#define UART_EFR_ECB        0x10 /* Enhanced control bit */

/*
 * These register definitions are for the 16C950
 */
#define UART_ICR    0x05    /* Index Control Register */

/* The 16950 ICR registers */
#define UART_ACR    0x00    /* Additional Control Register */
#define UART_ID1    0x08    /* ID #1 */
#define UART_ID2    0x09    /* ID #2 */
#define UART_ID3    0x0A    /* ID #3 */
#define UART_REV    0x0B    /* Revision */

/*
 * The 16C950 Additional Control Register
 */
#define UART_ACR_ICRRD  0x40    /* ICR Read enable */

#endif /* SERIAL_REG_H_ */
