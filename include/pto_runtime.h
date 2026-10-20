#ifndef PTO_RUNTIME_H
#define PTO_RUNTIME_H

#include <stdint.h>

#define PTO_RUNTIME_GROUP_CONTEXT_MAGIC 0x50544f47u
#define PTO_RUNTIME_GROUP_CONTEXT_VERSION 1u
#define PTO_RUNTIME_GROUP_STATE_READY 0u
#define PTO_RUNTIME_GROUP_STATE_RUNNING 1u
#define PTO_RUNTIME_GROUP_STATE_DONE 2u
#define PTO_RUNTIME_GROUP_STATE_ABORT 3u

struct pto_runtime_group_context {
	uint32_t magic;
	uint16_t version;
	uint16_t participant_count;
	volatile uint32_t *state;
	volatile int32_t *result;
	volatile uint32_t *error;
};

/* state and result each point to a four-element PE-indexed array.  The
 * launcher owns storage and must publish the descriptor before common entry. */

/* Launcher-owned hook; it must not perform libc work. */
const struct pto_runtime_group_context *__pto_runtime_group_context_get(void);
int __linx_group_pe_main(unsigned pe_id,
	const struct pto_runtime_group_context *context);
void __linx_group_worker_start(void) __attribute__((noreturn));
void __pto_runtime_group_complete(
	const struct pto_runtime_group_context *context, int status)
	__attribute__((noreturn));

#endif
