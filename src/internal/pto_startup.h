#ifndef PTO_STARTUP_H
#define PTO_STARTUP_H

/* Bootstrap failures cannot call abort(): libc may not be initialized. The
 * platform owns the resulting unrecoverable group fault and quiescence. */
static inline __attribute__((noreturn)) void __pto_startup_fault(void)
{
	__asm__ volatile("BSTART.SYS FALL\nEBREAK 0\nBSTOP\n" ::: "memory");
	for (;;) __asm__ volatile("" ::: "memory");
}
#endif
