/*
 * Freescale MX28EVK board
 *
 * (C) Copyright 2011 Freescale Semiconductor, Inc.
 *
 * Author: Fabio Estevam <fabio.estevam@freescale.com>
 *
 * Based on m28evk.c:
 * Copyright (C) 2011 Marek Vasut <marek.vasut@gmail.com>
 * on behalf of DENX Software Engineering GmbH
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <common.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/iomux-mx28.h>
#include <asm/imx-common/gpio.h>
#include <asm/arch/clock.h>
#include <asm/arch/sys_proto.h>
#include <linux/mii.h>
#include <miiphy.h>
#include <netdev.h>
#include <errno.h>
#include <mmc.h>

#define SYSTEM_REV_OFFSET    0x8
#define HW3REV0100 0x100
#define HW3REV0200 0x200
#define HW3REV0300 0x300

DECLARE_GLOBAL_DATA_PTR;

#define MUX_CONFIG_REV (MXS_PAD_3V3 | MXS_PAD_4MA | MXS_PAD_NOPULL)
#define MUX_CONFIG_LED (MXS_PAD_3V3 | MXS_PAD_4MA | MXS_PAD_NOPULL)


/* LEDs on HW3 Rev 0100 */
static const iomux_cfg_t leds_hw0100_pads[] = {
	MX28_PAD_GPMI_RDY0__GPIO_0_20 | MUX_CONFIG_LED,
	MX28_PAD_GPMI_RDN__GPIO_0_24 | MUX_CONFIG_LED,
	MX28_PAD_GPMI_ALE__GPIO_0_26 | MUX_CONFIG_LED,
};

/* LEDs on HW3 Rev 0200 */
static const iomux_cfg_t leds_hw0200_pads[] = {
	MX28_PAD_LCD_D18__GPIO_1_18 | MUX_CONFIG_LED,
	MX28_PAD_LCD_D19__GPIO_1_19 | MUX_CONFIG_LED,
	MX28_PAD_LCD_D20__GPIO_1_20 | MUX_CONFIG_LED,
	MX28_PAD_LCD_D21__GPIO_1_21 | MUX_CONFIG_LED,
	MX28_PAD_LCD_D22__GPIO_1_22 | MUX_CONFIG_LED,
	MX28_PAD_LCD_D23__GPIO_1_23 | MUX_CONFIG_LED,
};

static const iomux_cfg_t revision_pads[] = {
	MX28_PAD_LCD_D05__GPIO_1_5 | MUX_CONFIG_REV,
	MX28_PAD_LCD_D06__GPIO_1_6 | MUX_CONFIG_REV,
	MX28_PAD_LCD_D07__GPIO_1_7 | MUX_CONFIG_REV,
};

/*
 * Functions
 */
int board_early_init_f(void)
{
	/* IO0 clock at 480MHz */
	mxs_set_ioclk(MXC_IOCLK0, 480000);
	/* IO1 clock at 480MHz */
	mxs_set_ioclk(MXC_IOCLK1, 480000);


	/* SSP0 clock at 96MHz */
	mxs_set_sspclk(MXC_SSPCLK0, 96000, 0);
	/* SSP1 clock at 160MHz */
	mxs_set_sspclk(MXC_SSPCLK2, 160000, 0);

	/* the pads seem to have a long setup time, so do it early */
	mxs_iomux_setup_multiple_pads(
		revision_pads, ARRAY_SIZE(revision_pads));

	return 0;
}

int dram_init(void)
{
	return mxs_dram_init();
}

int board_init(void)
{
	/* Adress of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM_1 + 0x100;

	return 0;
}

static u32 system_rev;

void board_rev_init(void)
{
	/*
	 * REV0100: all pins high     -> 111 -> 000 -> 0x0100
	 * REV0200: PINID_LCD_D06 low -> 101 -> 010 -> 0x0200
	 */
	system_rev =
		((!gpio_get_value(MX28_PAD_LCD_D05__GPIO_1_5)) << 2) |
		((!gpio_get_value(MX28_PAD_LCD_D06__GPIO_1_6)) << 1) |
		(!gpio_get_value(MX28_PAD_LCD_D07__GPIO_1_7));

	/* FIXME: gpio_get_value do not always detect correct revision */
	system_rev = 2;

	if (system_rev == 0)
		system_rev = 1;

	system_rev <<= SYSTEM_REV_OFFSET;

	/* Disable LEDs after reset ... */
	switch (system_rev) {
	case HW3REV0100:
		mxs_iomux_setup_multiple_pads(
			leds_hw0100_pads, ARRAY_SIZE(leds_hw0100_pads));
		gpio_direction_output(MX28_PAD_GPMI_RDY0__GPIO_0_20, 0);
		gpio_direction_output(MX28_PAD_GPMI_RDN__GPIO_0_24, 0);
		gpio_direction_output(MX28_PAD_GPMI_ALE__GPIO_0_26, 0);
		break;
	case HW3REV0200:
	default:
		mxs_iomux_setup_multiple_pads(
			leds_hw0200_pads, ARRAY_SIZE(leds_hw0200_pads));
		gpio_direction_output(MX28_PAD_LCD_D18__GPIO_1_18, 0);
		gpio_direction_output(MX28_PAD_LCD_D19__GPIO_1_19, 0);
		gpio_direction_output(MX28_PAD_LCD_D20__GPIO_1_20, 0);
		gpio_direction_output(MX28_PAD_LCD_D21__GPIO_1_21, 0);
		gpio_direction_output(MX28_PAD_LCD_D22__GPIO_1_22, 0);
		gpio_direction_output(MX28_PAD_LCD_D23__GPIO_1_23, 0);
		break;
	}

	/* ... but switch on one to indicate a running bootloader */
	switch (system_rev) {
	case HW3REV0100:
		gpio_set_value(MX28_PAD_GPMI_ALE__GPIO_0_26, 1);
		break;
	case HW3REV0200:
	default:
		gpio_set_value(MX28_PAD_LCD_D18__GPIO_1_18, 1);
		gpio_set_value(MX28_PAD_LCD_D19__GPIO_1_19, 1);
		break;
   }
}

u32 get_board_rev(void)
{
	return system_rev;
}

#ifdef	CONFIG_CMD_MMC
int board_mmc_init(bd_t *bis)
{
	return mxsmmc_initialize(bis, 1, NULL, NULL);
}
#endif

#ifdef	CONFIG_CMD_NET

int board_eth_init(bd_t *bis)
{
	struct mxs_clkctrl_regs *clkctrl_regs =
		(struct mxs_clkctrl_regs *)MXS_CLKCTRL_BASE;
	struct eth_device *dev;
	int ret;

	ret = cpu_eth_init(bis);

	/* MX28EVK uses ENET_CLK PAD to drive FEC clock */
	writel(CLKCTRL_ENET_TIME_SEL_RMII_CLK | CLKCTRL_ENET_CLK_OUT_EN,
					&clkctrl_regs->hw_clkctrl_enet);

	/* Reset PHY */
	gpio_direction_output(MX28_PAD_LCD_D00__GPIO_1_0, 0);
	udelay(10000);
	gpio_set_value(MX28_PAD_LCD_D00__GPIO_1_0, 1);
	udelay(10000);

	ret = fecmxc_initialize(bis);
	if (ret) {
		puts("FEC MXS: Unable to init FEC\n");
		return ret;
	}

	dev = eth_get_dev_by_name("FEC");
	if (!dev) {
		puts("FEC MXS: Unable to get FEC device entry\n");
		return -EINVAL;
	}

	return ret;
}

#endif

int misc_init_r(void)
{
	puts("Board: Home-Box\n");
	char *s = getenv("serial");

	if (s && s[0]) {
		puts("Serial: ");
		puts(s);
		putc('\n');
	}
	return 0;
}

int board_late_init(void)
{
	board_rev_init();

	printf("Revision: %04x\n", system_rev);

	return 0;
}
