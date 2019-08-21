#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H

#include "kern/pci.h"

#define E1000_VENDOR_ID_82540EM 0x8086
#define E1000_DEVICE_ID_82540EM 0x100e

int e1000_attachfn(struct pci_func *pcif);

#endif  // SOL >= 6

