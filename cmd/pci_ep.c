// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2019 Ramon Fried <rfried.dev@gmail.com>
 */

/*
 * PCI routines
 */

#include <common.h>
#include <bootretry.h>
#include <cli.h>
#include <command.h>
#include <console.h>
#include <dm.h>
#include <asm/processor.h>
#include <asm/io.h>
#include <pci_ep.h>

static int do_pci_ep(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	struct udevice *dev;
	int ret;

	if (argc < 2)
		return CMD_RET_USAGE;

	for (ret = uclass_first_device(UCLASS_PCI_EP, &dev);
	     dev;
	     ret = uclass_next_device(&dev)) {
		printf("1: %s\n", dev->name);
	}

	return ret;
}

#ifdef CONFIG_SYS_LONGHELP
static char pci_ep_help_text[] =
	"device				- list available devices\n"
	"pci_ep header <devname> <function>	- show header of PCI endpoint\n"
	"pci_ep bars <devname> <function>	- show BARs configuration'\n";
#endif

U_BOOT_CMD(
	pci_ep,	2,	1,	do_pci_ep,
	"Show PCI endpoint Configuration", pci_ep_help_text
);
