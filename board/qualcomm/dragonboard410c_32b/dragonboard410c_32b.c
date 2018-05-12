// SPDX-License-Identifier: GPL-2.0+
/*
 * Board init file for Dragonboard 410C 32bit
 *
 * (C) Copyright 2018 Ramon Fried <ramon.fried@gmail.com>
 *
 */

#include <common.h>
#include <dm.h>
#include <usb.h>
#include <asm/gpio.h>
#include <fdt_support.h>
#include <power/pmic.h>
#include <asm/io.h>
#include <linux/arm-smccc.h>

#define SCM_SVC_PWR                     0x9
#define SCM_IO_DISABLE_PMIC_ARBITER     0x1
#define SCM_CMD_MILESTONE_32_64 0x200010f
#define SCM_CMD_ARGS 0x12

struct el1_system_param {
	u64 el1_x0;
	u64 el1_x1;
	u64 el1_x2;
	u64 el1_x3;
	u64 el1_x4;
	u64 el1_x5;
	u64 el1_x6;
	u64 el1_x7;
	u64 el1_x8;
	u64 el1_elr;
};

DECLARE_GLOBAL_DATA_PTR;

void scm_elexec_call(u32 kernel_entry, u32 dtb_offset)
{
	static struct el1_system_param param = {0};

	param.el1_x0 = dtb_offset;
	param.el1_elr = kernel_entry;

	arm_smccc_smc(SCM_CMD_MILESTONE_32_64, SCM_CMD_ARGS,
					(unsigned long)&param,
					sizeof(struct el1_system_param),
					0, 0, 0, 0, 0);

	puts("Failed to jump to kernel\n");
}

int dram_init(void)
{
	gd->ram_size = PHYS_SDRAM_1_SIZE;

	return 0;
}

int dram_init_banksize(void)
{
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;

	return 0;
}

int board_prepare_usb(enum usb_init_type type)
{
	static struct udevice *pmic_gpio;
	static struct gpio_desc hub_reset, usb_sel;
	int ret = 0, node;

	if (!pmic_gpio) {
		ret = uclass_get_device_by_name(UCLASS_GPIO,
						"pm8916_gpios@c000",
						&pmic_gpio);
		if (ret < 0) {
			printf("Failed to find pm8916_gpios@c000 node.\n");
			return ret;
		}
	}

	/* Try to request gpios needed to start usb host on dragonboard */
	if (!dm_gpio_is_valid(&hub_reset)) {
		node = fdt_subnode_offset(gd->fdt_blob,
					  dev_of_offset(pmic_gpio),
					  "usb_hub_reset_pm");
		if (node < 0) {
			printf("Failed to find usb_hub_reset_pm dt node.\n");
			return node;
		}
		ret = gpio_request_by_name_nodev(offset_to_ofnode(node),
						 "gpios", 0, &hub_reset, 0);
		if (ret < 0) {
			printf("Failed to request usb_hub_reset_pm gpio.\n");
			return ret;
		}
	}

	if (!dm_gpio_is_valid(&usb_sel)) {
		node = fdt_subnode_offset(gd->fdt_blob,
					  dev_of_offset(pmic_gpio),
					  "usb_sw_sel_pm");
		if (node < 0) {
			printf("Failed to find usb_sw_sel_pm dt node.\n");
			return 0;
		}
		ret = gpio_request_by_name_nodev(offset_to_ofnode(node),
						 "gpios", 0, &usb_sel, 0);
		if (ret < 0) {
			printf("Failed to request usb_sw_sel_pm gpio.\n");
			return ret;
		}
	}

	if (type == USB_INIT_HOST) {
		/* Start USB Hub */
		dm_gpio_set_dir_flags(&hub_reset,
				      GPIOD_IS_OUT | GPIOD_IS_OUT_ACTIVE);
		mdelay(100);
		/* Switch usb to host connectors */
		dm_gpio_set_dir_flags(&usb_sel,
				      GPIOD_IS_OUT | GPIOD_IS_OUT_ACTIVE);
		mdelay(100);
	} else { /* Device */
		/* Disable hub */
		dm_gpio_set_dir_flags(&hub_reset, GPIOD_IS_OUT);
		/* Switch back to device connector */
		dm_gpio_set_dir_flags(&usb_sel, GPIOD_IS_OUT);
	}

	return 0;
}

/* Check for vol- button - if pressed - stop autoboot */
int misc_init_r(void)
{
	struct udevice *pon;
	struct gpio_desc resin;
	int node, ret;

	ret = uclass_get_device_by_name(UCLASS_GPIO, "pm8916_pon@800", &pon);
	if (ret < 0) {
		printf("Failed to find PMIC pon node. Check device tree\n");
		return 0;
	}

	node = fdt_subnode_offset(gd->fdt_blob, dev_of_offset(pon),
				  "key_vol_down");
	if (node < 0) {
		printf("Failed to find key_vol_down node. Check device tree\n");
		return 0;
	}

	if (gpio_request_by_name_nodev(offset_to_ofnode(node), "gpios", 0,
				       &resin, 0)) {
		printf("Failed to request key_vol_down button.\n");
		return 0;
	}

	if (dm_gpio_get_value(&resin)) {
		env_set("bootdelay", "-1");
		printf("Power button pressed - dropping to console.\n");
	}

	return 0;
}

int board_init(void)
{
	return 0;
}

int ft_board_setup(void *blob, bd_t *bd)
{
	int offset, len, i;
	const char *mac;
	struct {
		const char *compatible;
		const char *property;
	} fix[] = {
		[0] = {
			/* update the kernel's dtb with wlan mac */
			.compatible = "qcom,wcnss-wlan",
			.property = "local-mac-address",
		},
		[1] = {
			/* update the kernel's dtb with bt mac */
			.compatible = "qcom,wcnss-bt",
			.property = "local-bd-address",
		},
	};

	for (i = 0; i < ARRAY_SIZE(fix); i++) {
		offset = fdt_node_offset_by_compatible(gd->fdt_blob, -1,
						       fix[i].compatible);
		if (offset < 0)
			continue;

		mac = fdt_getprop(gd->fdt_blob, offset, fix[i].property, &len);
		if (mac)
			do_fixup_by_compat(blob, fix[i].compatible,
					   fix[i].property, mac, ARP_HLEN, 1);
	}

	return 0;
}

void reset_cpu(ulong addr)
{
	debug("in reset_cpu\n");
}
