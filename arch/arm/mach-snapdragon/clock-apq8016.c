/*
 * Clock drivers for Qualcomm APQ8016
 *
 * (C) Copyright 2015 Mateusz Kulikowski <mateusz.kulikowski@gmail.com>
 *
 * Based on Little Kernel driver, simplified
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#include <common.h>
#include <clk-uclass.h>
#include <dm.h>
#include <errno.h>
#include <asm/io.h>
#include <linux/bitops.h>
#include "clock-snapdragon.h"

/* GPLL0 clock control registers */
#define GPLL0_STATUS_ACTIVE BIT(17)
#define APCS_GPLL_ENA_VOTE_GPLL0 BIT(10)

static const struct bcr_regs sdc_regs[] = {
	{
	.cfg_rcgr = SDCC_CFG_RCGR(1),
	.cmd_rcgr = SDCC_CMD_RCGR(1),
	.M = SDCC_M(1),
	.N = SDCC_N(1),
	.D = SDCC_D(1),
	},
	{
	.cfg_rcgr = SDCC_CFG_RCGR(2),
	.cmd_rcgr = SDCC_CMD_RCGR(2),
	.M = SDCC_M(2),
	.N = SDCC_N(2),
	.D = SDCC_D(2),
	}
};

static struct gpll0_ctrl gpll0_ctrl = {
	.cbcr_reg = BLSP1_AHB_CBCR,
	.ena_vote = APCS_CLOCK_BRANCH_ENA_VOTE,
	.vote_bit = APCS_GPLL_ENA_VOTE_GPLL0,
};

/* SDHCI */
static int clk_init_sdc(struct msm_clk_priv *priv, int slot, uint rate)
{
	int div = 8; /* 100MHz default */

	if (rate == 200000000)
		div = 4;

	clk_enable_cbc(priv->base + SDCC_AHB_CBCR(slot));
	/* 800Mhz/div, gpll0 */
	clk_rcg_set_rate_mnd(priv->base, &sdc_regs[slot], div, 0, 0,
			     CFG_CLK_SRC_GPLL0);
	clk_enable_gpll0(priv->base, &gpll0_ctrl);
	clk_enable_cbc(priv->base + SDCC_APPS_CBCR(slot));

	return rate;
}

static const struct bcr_regs uart2_regs = {
	.cfg_rcgr = BLSP1_UART2_APPS_CFG_RCGR,
	.cmd_rcgr = BLSP1_UART2_APPS_CMD_RCGR,
	.M = BLSP1_UART2_APPS_M,
	.N = BLSP1_UART2_APPS_N,
	.D = BLSP1_UART2_APPS_D,
};

#ifndef GPIO_INPUT
#define GPIO_INPUT	0x0000
#endif
#ifndef GPIO_OUTPUT
#define GPIO_OUTPUT	0x0001
#endif

#define GPIO_LEVEL	0x0000
#define GPIO_EDGE	0x0010

#define GPIO_RISING	0x0020
#define GPIO_FALLING	0x0040

#define GPIO_HIGH	0x0020
#define GPIO_LOW	0x0040

#define GPIO_PULLUP	0x0100
#define GPIO_PULLDOWN	0x0200
#define GPIO_NO_PULL    0

#define GPIO_ENABLE     0
#define GPIO_DISABLE    1
#define GPIO_8MA        3

#define TLMM_BASE_ADDR              0x1000000
#define GPIO_CONFIG_ADDR(x)         (TLMM_BASE_ADDR + (x)*0x1000)

void gpio_tlmm_config(uint32_t gpio, uint8_t func,
			uint8_t dir, uint8_t pull,
			uint8_t drvstr, uint32_t enable)
{
	uint32_t val = 0;
	val |= pull;
	val |= func << 2;
	val |= drvstr << 6;
	val |= enable << 9;
	writel(val, (uint32_t *)GPIO_CONFIG_ADDR(gpio));
	return;
}


/* Configure gpio for blsp uart 2 */
void gpio_config_uart_dm()
{
	/* configure rx gpio */
	gpio_tlmm_config(5, 2, GPIO_INPUT, GPIO_NO_PULL,
				GPIO_8MA, GPIO_DISABLE);

	/* configure tx gpio */
	gpio_tlmm_config(4, 2, GPIO_OUTPUT, GPIO_NO_PULL,
				GPIO_8MA, GPIO_DISABLE);
}


/* UART: 115200 */
static int clk_init_uart(struct msm_clk_priv *priv)
{
	/* Enable iface clk */
	clk_enable_gpll0(priv->base, &gpll0_ctrl);

	/* 7372800 uart block clock @ GPLL0 */
	clk_rcg_set_rate_mnd(priv->base, &uart2_regs, 1, 144, 15625,
			     CFG_CLK_SRC_GPLL0);

	/* Enable core clk */

	//clk_enable_cbc(priv->base + BLSP1_AHB_CBCR);

	clk_enable_cbc(priv->base + BLSP1_UART2_APPS_CBCR);

	dsb();
	gpio_config_uart_dm();
	dsb();
	return 0;
}

/* UART: 115200 */
int _clk_init_uart()
{
	/* Enable iface clk */
	clk_enable_gpll0(0x1800000 , &gpll0_ctrl);

	/* 7372800 uart block clock @ GPLL0 */
	clk_rcg_set_rate_mnd(0x1800000 , &uart2_regs, 1, 144, 15625,
			     CFG_CLK_SRC_GPLL0);

	/* Enable core clk */

	//clk_enable_cbc(priv->base + BLSP1_AHB_CBCR);

	clk_enable_cbc(0x1800000 + BLSP1_UART2_APPS_CBCR);

	dsb();
	gpio_config_uart_dm();
	dsb();
	return 0;
}
ulong msm_set_rate(struct clk *clk, ulong rate)
{
	struct msm_clk_priv *priv = dev_get_priv(clk->dev);

	switch (clk->id) {
	case 0: /* SDC1 */
		return clk_init_sdc(priv, 0, rate);
		break;
	case 1: /* SDC2 */
		return clk_init_sdc(priv, 1, rate);
		break;
	case 4: /* UART2 */
		return clk_init_uart(priv);
		break;
	default:
		return 0;
	}
}
