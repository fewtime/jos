/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>

#include <kern/picirq.h>


###################################################################
# exceptions/interrupts
###################################################################

/* TRAPHANDLER defines a globally-visible function for handling a trap.
 * It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 *
 * You shouldn't call a TRAPHANDLER function from C, but you may
 * need to _declare_ one in C (for instance, to get a function pointer
 * during IDT setup).  You can declare the function with
 *   void NAME();
 * where NAME is the argument passed to TRAPHANDLER.
 */
#define TRAPHANDLER(name, num)						\
	.globl name;		/* define global symbol for 'name' */	\
	.type name, @function;	/* symbol type is function */		\
	.align 2;		/* align function definition */		\
	name:			/* function starts here */		\
	pushl $(num);							\
	jmp _alltraps

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num)					\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name:								\
	pushl $0;							\
	pushl $(num);							\
	jmp _alltraps

.text

/*
 * Lab 3: Your code here for generating entry points for the different traps.
 */
	TRAPHANDLER_NOEC(divide_entry, T_DIVIDE);
	TRAPHANDLER_NOEC(debug_entry, T_DEBUG);
	TRAPHANDLER_NOEC(nmi_entry, T_NMI);
	TRAPHANDLER_NOEC(brkpt_entry, T_BRKPT);
	TRAPHANDLER_NOEC(oflow_entry, T_OFLOW);
	TRAPHANDLER_NOEC(bound_entry, T_BOUND);
	TRAPHANDLER_NOEC(illop_entry, T_ILLOP);
	TRAPHANDLER_NOEC(device_entry, T_DEVICE);
	TRAPHANDLER(dblflt_entry, T_DBLFLT);

	TRAPHANDLER(tss_entry, T_TSS);
	TRAPHANDLER(segnp_entry, T_SEGNP);
	TRAPHANDLER(stack_entry, T_STACK);
	TRAPHANDLER(gpflt_entry, T_GPFLT);
	TRAPHANDLER(pgflt_entry, T_PGFLT);
	TRAPHANDLER_NOEC(fperr_entry, T_FPERR);

	TRAPHANDLER_NOEC(syscall_entry, T_SYSCALL);

	TRAPHANDLER_NOEC(irq_handler0, IRQ_OFFSET);
	TRAPHANDLER_NOEC(irq_handler1, IRQ_OFFSET + 1);
	TRAPHANDLER_NOEC(irq_handler2, IRQ_OFFSET + 2);
	TRAPHANDLER_NOEC(irq_handler3, IRQ_OFFSET + 3);
	TRAPHANDLER_NOEC(irq_handler4, IRQ_OFFSET + 4);
	TRAPHANDLER_NOEC(irq_handler5, IRQ_OFFSET + 5);
	TRAPHANDLER_NOEC(irq_handler6, IRQ_OFFSET + 6);
	TRAPHANDLER_NOEC(irq_handler7, IRQ_OFFSET + 7);
	TRAPHANDLER_NOEC(irq_handler8, IRQ_OFFSET + 8);
	TRAPHANDLER_NOEC(irq_handler9, IRQ_OFFSET + 9);
	TRAPHANDLER_NOEC(irq_handler10, IRQ_OFFSET + 10);
	TRAPHANDLER_NOEC(irq_handler11, IRQ_OFFSET + 11);
	TRAPHANDLER_NOEC(irq_handler12, IRQ_OFFSET + 12);
	TRAPHANDLER_NOEC(irq_handler13, IRQ_OFFSET + 13);
	TRAPHANDLER_NOEC(irq_handler14, IRQ_OFFSET + 14);
	TRAPHANDLER_NOEC(irq_handler15, IRQ_OFFSET + 15);

/*
 * Lab 3: Your code here for _alltraps
 */

_alltraps:
	// 根据 ~Trapframe~ 进行压栈
	pushl %ds
	pushl %es
	pushal

	// 把 ~GD_KD~ 加载到 ~%ds~ 和 ~%es~
	mov $GD_KD, %ax
	mov %ax, %dx
	mov %ax, %es

	// 将指向 ~Trapframe~ 的指针作为 ~trap()~ 参数
	pushl %esp

	// 调用 ~trap()~
	call trap

