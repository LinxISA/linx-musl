#include <features.h>
#include "libc.h"

#define START "_start"

#include "crt_arch.h"

#if defined(__linx)
/*
 * Normal musl process startup is a single execution context, not a four-PE
 * kernel launch. A dedicated PE launcher may override this weak hook with a
 * strong definition returning 4 after establishing all four PE contexts.
 */
weak unsigned __pto_runtime_pe_context_count(void)
{
	return 1; /* PTO_RUNTIME_SINGLE_CONTEXTS */
}
#endif

int main();
weak void _init();
weak void _fini();
int __libc_start_main(int (*)(), int, char **,
	void (*)(), void(*)(), void(*)());

hidden void _start_c(long *p)
{
	int argc = p[0];
	char **argv = (void *)(p+1);
	__libc_start_main(main, argc, argv, _init, _fini, 0);
}
