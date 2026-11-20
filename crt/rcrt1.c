#define START "_start"
#define LINX_PTO_STATIC_PIE 1
#define _dlstart_c _start_c
#include "../ldso/dlstart.c"

#if defined(__linx)
#include <pto_runtime.h>
#include "pto_startup.h"
/* Static-PIE startup has the same single-context launch contract as crt1. */
weak unsigned __pto_runtime_pe_context_count(void)
{
	return 1; /* PTO_RUNTIME_SINGLE_CONTEXTS */
}

weak const struct pto_runtime_group_context *
__pto_runtime_group_context_get(void)
{
	return 0;
}

#endif

int main();
weak void _init();
weak void _fini();
int __libc_start_main(int (*)(), int, char **,
	void (*)(), void(*)(), void(*)());

hidden void __dls2(unsigned char *base, size_t *sp)
{
#if defined(__linx)
	const struct pto_runtime_group_context *group_context =
		__pto_runtime_group_context_get ?
		__pto_runtime_group_context_get() : 0;
	/* rcrt1 self-relocates before __dls2; it cannot admit four contexts.
	 * Workers are rejected before relocation in dlstart.c as well. Use a
	 * fixed-address static image for the group CRT until a loader owns PIE. */
	if (group_context || __pto_runtime_pe_context_count() != 1)
		__pto_startup_fault();
#endif
	__libc_start_main(main, *sp, (void *)(sp+1), _init, _fini, 0);
}
