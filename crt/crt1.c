#include <features.h>
#include "libc.h"

#define START "_start"

#include "crt_arch.h"

#if defined(__linx)
#include <pto_runtime.h>
/*
 * Normal musl process startup is a single execution context, not a four-PE
 * kernel launch. A dedicated PE launcher may override this weak hook with a
 * strong definition returning 4 after establishing all four PE contexts.
 */
weak unsigned __pto_runtime_pe_context_count(void)
{
	return 1; /* PTO_RUNTIME_SINGLE_CONTEXTS */
}

/* A launcher may provide the group descriptor through this weak hook. The
 * default is null, so accidental multi-PE entry fails closed rather than
 * repeating libc initialization on PE1..PE3. */
weak const struct pto_runtime_group_context *
__pto_runtime_group_context_get(void)
{
	return 0;
}

weak void __linx_group_worker_start(void) __attribute__((noreturn));
#endif

int main();
weak void _init();
weak void _fini();
int __libc_start_main(int (*)(), int, char **,
	void (*)(), void(*)(), void(*)());

hidden void _start_c(long *p)
{
	const struct pto_runtime_group_context *group_context =
		__pto_runtime_group_context_get ?
		__pto_runtime_group_context_get() : 0;
	if (group_context) {
		if (__linx_group_worker_start)
			__linx_group_worker_start();
		__builtin_trap();
	}
	int argc = p[0];
	char **argv = (void *)(p+1);
	__libc_start_main(main, argc, argv, _init, _fini, 0);
}
