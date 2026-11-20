#if defined(__linx)
#include <pto_runtime.h>
#include "pto_startup.h"
#include "libc.h"

extern void __pto_musl_init_libc(char **, char *);
extern void __libc_start_init(void);
extern int main(int, char **, char **) PTO_RUNTIME_INTRA;

/* PE0-only process lifetime state. Failure stays poisoned until image reload. */
static volatile unsigned process_state;
static int process_argc;
static char **process_argv;

int __linx_group_pe_main(unsigned pe, const struct pto_runtime_group_context *ctx)
{
	(void)pe;
	(void)ctx;
	return main(process_argc, process_argv, process_argv + process_argc + 1);
}

void __pto_musl_group_start(long *sp)
{
	if (__builtin_linx_get_thread_idx() == 0) {
		if (process_state != 2) {
			if (process_state) __pto_startup_fault();
			process_state = 1;
			process_argc = sp[0];
			process_argv = (void *)(sp + 1);
			__pto_musl_init_libc(process_argv + process_argc + 1,
				process_argv[0]);
			__libc_start_init();
			process_state = 2;
		}
	}
	/* Workers do not read process_state/argc/argv before generation zero. */
	__linx_group_worker_start();
}
#endif
