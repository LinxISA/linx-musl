#include <features.h>
#include "libc.h"

#define START "_start"

#include "crt_arch.h"

#if defined(__linx)
#include <pto_runtime.h>
#include "pto_startup.h"
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

#endif

int main();
weak void _init();
weak void _fini();
int __libc_start_main(int (*)(), int, char **,
	void (*)(), void(*)(), void(*)());

hidden void _start_c(long *p)
{
#if defined(__linx)
	const struct pto_runtime_group_context *group_context =
		__pto_runtime_group_context_get ?
		__pto_runtime_group_context_get() : 0;
	if (group_context) {
		if (group_context->magic != PTO_RUNTIME_GROUP_CONTEXT_MAGIC ||
		    group_context->version != PTO_RUNTIME_GROUP_CONTEXT_VERSION ||
		    group_context->participant_count != PTO_RUNTIME_GROUP_PARTICIPANTS ||
		    !group_context->state || !group_context->result ||
		    !group_context->error)
			__pto_startup_fault();
		extern void __pto_musl_group_start(long *) __attribute__((noreturn));
		__pto_musl_group_start((long *)p);
		__pto_startup_fault();
	}
	if (__builtin_linx_get_thread_idx() != 0 ||
	    __pto_runtime_pe_context_count() != 1)
		__pto_startup_fault();
#endif
	int argc = p[0];
	char **argv = (void *)(p+1);
	__libc_start_main(main, argc, argv, _init, _fini, 0);
}
