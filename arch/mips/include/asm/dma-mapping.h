/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2019
 * Ramon Fried <rfried.dev@gmail.com>
 */

#ifndef __ASM_MIPS_DMA_MAPPING_H
#define __ASM_MIPS_DMA_MAPPING_H

#include <linux/dma-direction.h>
#include <asm/addrspace.h>
#define	dma_mapping_error(x, y)	0

static inline void *dma_alloc_coherent(size_t len, unsigned long *handle)
{
	void *vaddr = memalign(ARCH_DMA_MINALIGN,
			       ROUND(len, ARCH_DMA_MINALIGN));

	*handle = CPHYSADDR((unsigned long)vaddr);
	return (void *)(CKSEG1ADDR((unsigned long)vaddr));
}

static inline void dma_free_coherent(void *addr)
{
	free((void *)CPHYSADDR((unsigned long)addr));
}

static inline unsigned long dma_map_single(volatile void *vaddr, size_t len,
					   enum dma_data_direction dir)
{
	unsigned long dma_addr = CPHYSADDR((unsigned long)vaddr);

	if (dir == DMA_TO_DEVICE)
		flush_dcache_range((unsigned long)vaddr,
				   (unsigned long)vaddr + len);
	if (dir == DMA_FROM_DEVICE)
		invalidate_dcache_range((unsigned long)vaddr,
					(unsigned long)vaddr + len);

	return dma_addr;
}

static inline void dma_unmap_single(volatile void *vaddr, size_t len,
				    unsigned long paddr)
{
}

#endif /* __ASM_MIPS_DMA_MAPPING_H */
