
#include <common.h>

#ifndef CONFIG_SYS_DCACHE_OFF
void enable_caches(void)
{
	dcache_enable();
}

void disable_caches(void)
{
	dcache_disable();
}
#endif


