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
#define E1000_RA 0x05400 / 4         /* Receive Address - RW Array */
#define E1000_RAL 0x05400 / 4 /* Receive Address Low - RW */
#define E1000_RAH 0x05404 / 4 /* Receive Address HIGH - RW */
#define E1000_RDBAL 0x02800 / 4   /* RX Descriptor Base Address Low - RW */
#define E1000_RDBAH 0x02804 / 4   /* RX Descriptor Base Address High - RW */
#define E1000_RDLEN 0x02808 / 4   /* RX Descriptor Length - RW */
#define E1000_RDH 0x02810 / 4     /* RX Descriptor Head - RW */
#define E1000_RDT 0x02818 / 4     /* RX Descriptor Tail - RW */
#define E1000_RCTL 0x00100 / 4        /* RX Control - RW */

/* Transmit Descriptor Length*/
#define E1000_TDLEN_LEN 0x000fff80

/* Receive Descriptor Length*/
#define E1000_RDLEN_LEN 0x000fff80

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

/* Receive Descriptor bit definitions */
#define E1000_RXD_STAT_DD 0x01  /* Descriptor Done */
#define E1000_RXD_STAT_EOP 0x02 /* End of Packet */

/* Receive Address */
#define E1000_RAH_AV 0x80000000 /* Receive descriptor valid */

/* Receive Control */
#define E1000_RCTL_EN 0x00000002  /* enable */
#define E1000_RCTL_BAM 0x00008000 /* broadcast enable */
#define E1000_RCTL_SECRC 0x04000000 /* Strip Ethernet CRC */

#define TXRING_LEN 32
#define TX_PACKET_SIZE 1518
#define RXRING_LEN 128
#define RX_PACKET_SIZE 1518

#define E_TRANSMIT_RETRY 1

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

struct e1000_rx_desc {
  uint64_t addr;
  uint16_t length;
  uint16_t chksum;
  uint8_t status;
  uint8_t errors;
  uint16_t special;
} __attribute__((packed));

int e1000_attachfn(struct pci_func *pcif);
static void init_desc(void);
int e1000_transmit_init(void);
int e1000_transmit(void *data, size_t len);
int e1000_receive_init(void);

volatile uint32_t *mmio_e1000;

#endif // SOL >= 6
