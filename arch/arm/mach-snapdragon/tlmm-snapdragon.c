/*
 * TLMM driver for Qualcomm APQ8016, APQ8096
 *
 * (C) Copyright 2018 Ramon Fried <ramon.fried@linaro.org>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#define DEBUG

#include <common.h>
#include <dm.h>
#include <errno.h>
#include <asm/io.h>
#include <dm/pinctrl.h>

struct msm_pinctrl_priv {
	phys_addr_t base;
};

static int msm_pinctrl_probe(struct udevice *dev)
{
	struct msm_pinctrl_priv *priv = dev_get_priv(dev);

	priv->base = devfdt_get_addr(dev);

	return priv->base == FDT_ADDR_T_NONE ? -EINVAL : 0;
}

int msm_pinctrl_set_state(struct udevice *dev, struct udevice *config) {

	debug("entered %s\n", __PRETTY_FUNCTION__);
	return 0;
}
static int msm_pinmux_set(struct udevice *dev, unsigned pin_selector,
			  unsigned func_selector)
{
	return 0;
}


static struct pinctrl_ops msm_pinctrl_ops = {
	.set_state = pinctrl_generic_set_state,
	.pinmux_set = msm_pinmux_set,
};

static const struct udevice_id msm_pinctrl_ids[] = {
	{ .compatible = "qcom,tlmm-msm8916" },
	{ .compatible = "qcom,tlmm-apq8016" },
	{ }
};

U_BOOT_DRIVER(pinctrl_snapdraon) = {
	.name		= "pinctrl_msm",
	.id		= UCLASS_PINCTRL,
	.of_match	= msm_pinctrl_ids,
	.priv_auto_alloc_size = sizeof(struct msm_pinctrl_priv),
	.ops		= &msm_pinctrl_ops,
	.probe		= msm_pinctrl_probe,
};
