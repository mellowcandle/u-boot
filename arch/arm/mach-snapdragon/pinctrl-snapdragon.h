/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Qualcomm Pin control
 *
 * (C) Copyright 2018 Ramon Fried <ramon.fried@gmail.com>
 *
 */
#ifndef _PINCTRL_SNAPDRAGON_H
#define _PINCTRL_SNAPDRAGON_H

struct pinctrl_function {
	const char *name;
	int val;
};

extern const char * const msm_pinctrl_pins[];
extern const struct pinctrl_function msm_pinctrl_functions[];
extern const int msm_functions_count;
extern const int msm_pins_count;

#endif
