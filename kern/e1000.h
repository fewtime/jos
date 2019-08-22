#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H

#include "inc/string.h"
#include "kern/pci.h"

#define E1000_VENDOR_ID_82540EM 0x8086
#define E1000_DEVICE_ID_82540EM 0x100e

/* Register Set. (82543, 82544) */
#define E1000_STATUS 0x00008 / 4 /* Device Status - RO */
#define E1000_TDBAL 0x03800 / 4  /* TX Descriptor Base Address Low - RW */
#define E1000_TDBAH 0x03804 / 4  /* TX Descriptor Base Address High - RW */
#define E1000_TDLEN 0x03808 / 4  /* TX Descriptor Length - RW */
#define E1000_TDH 0x03810 / 4    /* TX Descriptor Head - RW */
#define E1000_TDT 0x03818 / 4    /* TX Descripotr Tail - RW */
#define E1000_TCTL 0x00400 / 4   /* TX Control - RW */
#define E1000_TIPG 0x00410 / 4   /* TX Inter-packet gap -RW */

/* Transmit Descriptor Length*/
#define E1000_TDLEN_LEN 0x000fff80

/* Transmit Control */
#define E1000_TCTL_EN 0x00000002   /* enable tx */
#define E1000_TCTL_PSP 0x00000008  /* pad short packets */
#define E1000_TCTL_CT 0x00000ff0   /* collision threshold */
#define E1000_TCTL_COLD 0x003ff000 /* collision distance */

/* TX Inter-packet gap bit definitions */
#define E1000_TIPG_IPGT 0x000003ff
#define E1000_TIPG_IPGR1 0x000ffc00
#define E1000_TIPG_IPGR2 0x3ff00000

/* Transmit Descriptor bit definitions */
#define E1000_TXD_STAT_DD 0x00000001 /* Descriptor Done */
#define E1000_TXD_CMD_EOP 0x0000001 /* End of Packet */
#define E1000_TXD_CMD_RS 0x00000008  /* Report Status */

#define TXRING_LEN 32
#define TX_PACKAGE_SIZE 1518

#define VALUEMASK(value, mask) (value) * ((mask) & ~((mask) << 1))

struct e1000_tx_desc {
  uint64_t addr;
  uint16_t length;
  uint8_t cso;
  uint8_t cmd;
  uint8_t status;
  uint8_t css;
  uint16_t special;
} __attribute__((packed));


int e1000_attachfn(struct pci_func *pcif);
static void init_desc(void);
int e1000_transmit_init(void);
int e1000_transmit(void *data, size_t len);

volatile uint32_t *mmio_e1000;

#endif // SOL >= 6
