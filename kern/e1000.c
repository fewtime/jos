#include <kern/e1000.h>
#include <kern/pmap.h>

// LAB 6: Your driver code here

struct e1000_tx_desc tx_desc_arr[TXRING_LEN];
char tx_buffer_arr[TXRING_LEN][TX_PACKET_SIZE];

struct e1000_rx_desc rx_desc_arr[RXRING_LEN];
char rx_buffer_arr[RXRING_LEN][RX_PACKET_SIZE];

int e1000_attachfn(struct pci_func *pcif) {
  pci_func_enable(pcif);
  cprintf("reg_base: %x, reg_size: %x\n", pcif->reg_base[0], pcif->reg_size[0]);

  // memory mapping
  mmio_e1000 =
      mmio_map_region((physaddr_t)pcif->reg_base[0], (size_t)pcif->reg_size[0]);

  init_desc();
  e1000_transmit_init();
  e1000_receive_init();

  return 0;
}

static void init_desc(void) {
  int i = 0;
  for (i = 0; i < TXRING_LEN; ++i) {
    tx_desc_arr[i].addr = PADDR(&tx_buffer_arr[i]);
    tx_desc_arr[i].status = E1000_TXD_STAT_DD;
  }

  for (i = 0; i < RXRING_LEN; ++i) {
    memset(&rx_desc_arr[i], 0, sizeof(struct e1000_rx_desc));
    memset(&rx_buffer_arr[i], 0, RX_PACKET_SIZE);
    rx_desc_arr[i].addr = PADDR(&rx_buffer_arr[i]);
    rx_desc_arr[i].status = E1000_RXD_STAT_DD | E1000_RXD_STAT_EOP;
  }
}

int e1000_transmit_init(void) {
  assert(mmio_e1000[E1000_STATUS] == 0x80080783);
  init_desc();
  mmio_e1000[E1000_TDBAL] = PADDR(tx_desc_arr);
  mmio_e1000[E1000_TDBAH] = 0;
  mmio_e1000[E1000_TDLEN] = VALUEMASK(TXRING_LEN, E1000_TDLEN_LEN);
  mmio_e1000[E1000_TDH] = 0;
  mmio_e1000[E1000_TDT] = 0;
  mmio_e1000[E1000_TCTL] =
      VALUEMASK(1, E1000_TCTL_EN) | VALUEMASK(1, E1000_TCTL_PSP) |
      VALUEMASK(0x10, E1000_TCTL_CT) | VALUEMASK(0x40, E1000_TCTL_COLD);
  mmio_e1000[E1000_TIPG] = VALUEMASK(0x10, E1000_TIPG_IPGT) |
			   VALUEMASK(0x4, E1000_TIPG_IPGR1) |
			   VALUEMASK(0x6, E1000_TIPG_IPGR2);
  return 0;
}

int e1000_transmit(void *data, size_t len) {
  uint32_t current = mmio_e1000[E1000_TDT];

  if ((tx_desc_arr[current].status & E1000_TXD_STAT_DD) != E1000_TXD_STAT_DD) {
    return -E_TRANSMIT_RETRY;
  }

  len = len > TX_PACKET_SIZE ? TX_PACKET_SIZE : len;

  memcpy(&tx_buffer_arr[current], data, len);
  tx_desc_arr[current].length = len;
  tx_desc_arr[current].status &= ~E1000_TXD_STAT_DD;
  tx_desc_arr[current].cmd |= (E1000_TXD_CMD_EOP | E1000_TXD_CMD_RS);

  mmio_e1000[E1000_TDT] = (current + 1) % TXRING_LEN;

  return 0;
}

int e1000_receive_init(void) {
  /* QEMU默认MAC: 52 : 54 : 00 : 12 : 34 : 56 */
  mmio_e1000[E1000_RAL] = 0x12005452;
  mmio_e1000[E1000_RAH] = 0x00005634 | E1000_RAH_AV;
  mmio_e1000[E1000_RDBAL] = PADDR(rx_desc_arr);
  mmio_e1000[E1000_RDBAH] = 0;
  mmio_e1000[E1000_RDLEN] = VALUEMASK(RXRING_LEN, E1000_RDLEN_LEN);
  mmio_e1000[E1000_RDH] = 0;
  mmio_e1000[E1000_RDT] = RXRING_LEN;
  mmio_e1000[E1000_RCTL] = E1000_RCTL_EN | E1000_RCTL_BAM | E1000_RCTL_SECRC;

  return 0;
}
