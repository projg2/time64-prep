#include <stdio.h>
#include <stdlib.h>

__attribute__((weak))
__attribute__((visibility("default")))
struct {
	int time32;
	int time64;
} __gentoo_time_bits;

__attribute__((constructor))
static void __gentoo_time_check() {
#if _TIME_BITS == 64
	__gentoo_time_bits.time64 = 1;
#else
	__gentoo_time_bits.time32 = 1;
#endif

	if (__gentoo_time_bits.time32 && __gentoo_time_bits.time64) {
		FILE *f;
		fprintf(stderr, "time32 and time64 ABI mixing detected\n");
		/* trigger a sandbox failure for good measure too */
		f = fopen("/time32-time64-mixing", "w");
		if (f)
			fclose(f);
		abort();
	}
}
