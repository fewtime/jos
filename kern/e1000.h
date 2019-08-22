#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H

#include "kern/pci.h"

#define E1000_VENDOR_ID_82540EM 0x8086
#define E1000_DEVICE_ID_82540EM 0x100e

/* Register Set. (82543, 82544) */
#define E1000_STATUS 0x00008 / 4 /* Device Status - RO */

int e1000_attachfn(struct pci_func *pcif);

volatile uint32_t *mmio_e1000;

#endif  // SOL >= 6

