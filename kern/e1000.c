#include <kern/e1000.h>
#include <kern/pmap.h>

// LAB 6: Your driver code here

int
e1000_attachfn(struct pci_func *pcif) {
	pci_func_enable(pcif);
	cprintf("reg_base: %x, reg_size: %x\n", pcif->reg_base[0], pcif->reg_size[0]);

	// memory mapping
	mmio_e1000 = mmio_map_region((physaddr_t)pcif->reg_base[0], (size_t)pcif->reg_size[0]);
	assert(mmio_e1000[E1000_STATUS] == 0x80080783);
	return 0;
}
