#include <common.h>
#include <mmc.h>

#define UNSTUFF_BITS(resp,start,size)                   \
({                              \
        const int __size = size;                \
        const u32 __mask = (__size < 32 ? 1 << __size : 0) - 1; \
        const int __off = 3 - ((start) / 32);           \
        const int __shft = (start) & 31;            \
        u32 __res;                      \
                                    \
        __res = resp[__off] >> __shft;              \
        if (__size + __shft > 32)               \
            __res |= resp[__off-1] << ((32 - __shft) % 32); \
        __res & __mask;                     \
})

u32 msm_board_serial(void)
{
	struct mmc *mmc_dev;

	mmc_dev = find_mmc_device(0);
	if (!mmc_dev)
		return 0;

	return UNSTUFF_BITS(mmc_dev->cid, 16, 32);
}


/* generate a unique locally administrated MAC */
int msm_generate_mac_addr(char *mac)
{
	int i;
	char sn[9];

	snprintf(sn, 8, "%08x", msm_board_serial());

	/* fill in the mac with serialno, use locally adminstrated pool */
	mac[0] = 0x02;
	mac[1] = 00;
	for (i = 3; i >= 0; i--) {
		mac[i+2] = strtoul(&sn[2*i], NULL, 16);
		sn[2*i] = 0;
	}

	return 0;
}
