// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>

// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW		0x800

//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//
static void
pgfault(struct UTrapframe *utf)
{
	void *addr = (void *) utf->utf_fault_va;
	uint32_t err = utf->utf_err;
	int r;

	// Check that the faulting access was (1) a write, and (2) to a
	// copy-on-write page.  If not, panic.
	// Hint:
	//   Use the read-only page table mappings at uvpt
	//   (see <inc/memlayout.h>).

	// LAB 4: Your code here.
	// 只处理写入copy-on-write地址情况
	if (!(err & FEC_WR) &&
	    (uvpt[PGNUM(addr)] & PTE_COW)) {
		panic("pgfault(): write to non-copy-on-write page.");
	}

	// Allocate a new page, map it at a temporary location (PFTEMP),
	// copy the data from the old page to the new page, then move the new
	// page to the old page's address.
	// Hint:
	//   You should make three system calls.

	// LAB 4: Your code here.
	addr = ROUNDDOWN(addr, PGSIZE);

        // 当前进程PFTEMP分配新物理页
	if ((r = sys_page_alloc(0, (void *)PFTEMP, PTE_P | PTE_U | PTE_W)) < 0) {
		panic("pgfault(): alloc error %e.\n", r);
	}
	// 将addr物理页拷贝到新PFTEMP页中
	memmove((void *)PFTEMP, addr, PGSIZE);

        // 将PFTEMP映射到addr中
        if ((r = sys_page_map(0, (void *)PFTEMP,
			      0, addr,
			      PTE_P | PTE_U | PTE_W)) < 0) {
          panic("pgfault(): map error %e.\n", r);
        }

        // 解除当前进程与PFTEMP的映射
	if ((r = sys_page_unmap(0, (void *)PFTEMP)) < 0) {
		panic("pgfault(): unmap error %e.\n", r);
	}

}

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)
//
// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
//
static int
duppage(envid_t envid, unsigned pn)
{
	int r;

	// LAB 4: Your code here.
	uintptr_t va = pn * PGSIZE;

	// 可写或写时拷贝页
        if ((uvpt[pn] & PTE_W) || (uvpt[pn] & PTE_COW)) {
		if ((r = sys_page_map(thisenv->env_id, (void *)va,
				      envid, (void *)va,
				      PTE_P | PTE_U | PTE_COW)) < 0) {
			panic("duppage(): map from parent to child env error: %e.\n", r);
		}
		if ((r = sys_page_map(thisenv->env_id, (void *)va,
				      thisenv->env_id, (void *)va,
				      PTE_P | PTE_U | PTE_COW)) < 0) {
			panic("duppage(): remap from parent to parent env error: %e.\n", r);
                }
        } else {  // 只读页
		if ((r = sys_page_map(thisenv->env_id, (void *)va,
				      envid, (void *)va,
				      PTE_U | PTE_P)) < 0) {
			panic("duppage(): read-only from parent to child env error: %e.\n", r);
		}
	}

	return 0;
}

//
// User-level fork with copy-on-write.
// Set up our page fault handler appropriately.
// Create a child.
// Copy our address space and page fault handler setup to the child.
// Then mark the child as runnable and return.
//
// Returns: child's envid to the parent, 0 to the child, < 0 on error.
// It is also OK to panic on error.
//
// Hint:
//   Use uvpd, uvpt, and duppage.
//   Remember to fix "thisenv" in the child process.
//   Neither user exception stack should ever be marked copy-on-write,
//   so you must allocate a new page for the child's user exception stack.
//
envid_t
fork(void)
{
	// LAB 4: Your code here.
	// 设置page fault handler
	set_pgfault_handler(pgfault);
	// 创建子进程，复制当前进程寄存器状态
	envid_t envid = sys_exofork();

	if (envid < 0) {  // 错误
		panic("fork(): sys_exofork: %e\n", envid);
	} else if (envid == 0) {  // 子进程
		thisenv = &envs[ENVX(sys_getenvid())];
		return 0;
	}

	// 页复制
	uintptr_t va;

	for (va = 0; va < USTACKTOP; va += PGSIZE) {
		if ((uvpd[PDX(va)] & PTE_P) &&
		    (uvpt[PGNUM(va)] & (PTE_P | PTE_U))) {
			duppage(envid, PGNUM(va));
                }
	}

	int ret = 0;
	// 分配异常栈
	if ((ret = sys_page_alloc(envid, (void *)(UXSTACKTOP - PGSIZE), PTE_P | PTE_W | PTE_U)) < 0) {
		panic("fork(): sys_page_alloc: %e\n", ret);
	}

	// 设置 _pgfault_upcall
	extern void _pgfault_upcall(void);
	sys_env_set_pgfault_upcall(envid, _pgfault_upcall);

	// 设置 ENV_RUNNABLE
	if ((ret = sys_env_set_status(envid, ENV_RUNNABLE)) < 0) {
		panic("fork(): sys_env_set_status: %e\n", ret);
	}

	return envid;
}

// Challenge!
int
sfork(void)
{
	panic("sfork not implemented");
	return -E_INVAL;
}
