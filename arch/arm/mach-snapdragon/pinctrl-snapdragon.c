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
#include "pinctrl-snapdragon.h"

struct msm_pinctrl_priv {
	phys_addr_t base;
};

#define GPIO_CONFIG_OFFSET(x)         ((x)*0x1000)
#define TLMM_GPIO_PULL_MASK GENMASK(1,0)
#define TLMM_FUNC_SEL_MASK GENMASK(5, 2)
#define TLMM_DRV_STRENGTH_MASK GENMASK(8, 6)

extern const int msm_functions_count;
extern const int msm_pins_count;


static const struct pinconf_param msm_conf_params[] = {
	{ "drive-strength", PIN_CONFIG_DRIVE_STRENGTH, 3 },
};

static int msm_get_functions_count(struct udevice *dev)
{
	printf("+%s\n", __PRETTY_FUNCTION__);
	return msm_functions_count;
}

static int msm_get_pins_count(struct udevice *dev)
{
	return msm_pins_count;
}


static const char *msm_get_function_name(struct udevice *dev,
					     unsigned selector)
{
	return msm_pinctrl_functions[selector];
}

static int msm_pinctrl_probe(struct udevice *dev)
{
	printf("+%s\n", __PRETTY_FUNCTION__);
	struct msm_pinctrl_priv *priv = dev_get_priv(dev);

	priv->base = devfdt_get_addr(dev);

	return priv->base == FDT_ADDR_T_NONE ? -EINVAL : 0;
}

static const char *msm_get_pin_name(struct udevice *dev, unsigned selector)
{
	printf("+%s\n", __PRETTY_FUNCTION__);
	return msm_pinctrl_pins[selector];
}

static int msm_pinmux_set(struct udevice *dev, unsigned pin_selector,
			  unsigned func_selector)
{
	printf("+%s\n", __PRETTY_FUNCTION__);
	struct msm_pinctrl_priv *priv = dev_get_priv(dev);
	uint32_t val;

	printf("sandbox pinmux: pin = %d (%s), function = %d (%s)\n",
	      pin_selector, msm_get_pin_name(dev, pin_selector),
	      func_selector, msm_get_function_name(dev, func_selector));

	val = readl(priv->base + GPIO_CONFIG_OFFSET(pin_selector));
	val &= ~TLMM_FUNC_SEL_MASK;
	val |= func_selector << 2;
	writel(val, priv->base + GPIO_CONFIG_OFFSET(pin_selector));

	return 0;
}


static int msm_pinconf_set(struct udevice *dev, unsigned pin_selector,
                   unsigned param, unsigned argument)
{
   	struct msm_pinctrl_priv *priv = dev_get_priv(dev);
	uint32_t val;

	printf("sandbox pinconf: pin = %d (%s), param = %d, arg = %d\n",
          pin_selector, msm_get_pin_name(dev, pin_selector),
          param, argument);

	val = readl(priv->base + GPIO_CONFIG_OFFSET(pin_selector));

	switch (param) {
	case PIN_CONFIG_DRIVE_STRENGTH:
		val &= ~TLMM_DRV_STRENGTH_MASK;
		val |= argument << 6;
		break;
	default:
		dev_err(dev, "Unknown pin configuration\n");
		return 0;
	}

	writel(val, priv->base + GPIO_CONFIG_OFFSET(pin_selector));
    return 0;
}



static struct pinctrl_ops msm_pinctrl_ops = {
	.get_pins_count = msm_get_pins_count,
	.get_pin_name = msm_get_pin_name,
	.set_state = pinctrl_generic_set_state,
	.pinmux_set = msm_pinmux_set,
	.pinconf_num_params = ARRAY_SIZE(msm_conf_params),
	.pinconf_params = msm_conf_params,
	.pinconf_set = msm_pinconf_set,
	.get_functions_count = msm_get_functions_count,
	.get_function_name = msm_get_function_name,
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
