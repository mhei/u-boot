/*
 * I2SE Duckbill
 *
 * Copyright (C) 2013 Michael Heimpold <mhei@heimpold.de>
 *
 * Based on m28evk.c:
 * Copyright (C) 2011 Marek Vasut <marek.vasut@gmail.com>
 * on behalf of DENX Software Engineering GmbH
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/iomux-mx28.h>
#include <asm/arch/clock.h>
#include <asm/arch/sys_proto.h>
#include <linux/mii.h>
#include <miiphy.h>
#include <netdev.h>
#include <errno.h>

#define GPIO_PHY_RESET MX28_PAD_ENET0_RX_CLK__GPIO_4_13
#define GPIO_LED_GREEN MX28_PAD_AUART0_RX__GPIO_3_0
#define GPIO_LED_RED   MX28_PAD_AUART0_TX__GPIO_3_1

DECLARE_GLOBAL_DATA_PTR;

int board_early_init_f(void)
{
	/* IO0 clock at 480MHz */
	mxs_set_ioclk(MXC_IOCLK0, 480000);

	/* SSP0 clock at 96MHz */
	mxs_set_sspclk(MXC_SSPCLK0, 96000, 0);

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

#ifdef CONFIG_CMD_MMC
int board_mmc_init(bd_t *bis)
{
	return mxsmmc_initialize(bis, 0, NULL, NULL);
}
#endif

#ifdef CONFIG_CMD_NET
int board_eth_init(bd_t *bis)
{
	struct mxs_clkctrl_regs *clkctrl_regs =
		(struct mxs_clkctrl_regs *)MXS_CLKCTRL_BASE;
	struct eth_device *dev;
	int ret;

	ret = cpu_eth_init(bis);

	/* Reset PHY */
	gpio_direction_output(GPIO_PHY_RESET, 1);
	udelay(50);
	gpio_set_value(GPIO_PHY_RESET, 0);
	udelay(200);
	gpio_set_value(GPIO_PHY_RESET, 1);

	/* give PHY some time to get out of the reset */
	udelay(10000);

	ret = fecmxc_initialize(bis);
	if (ret) {
		puts("FEC MXS: Unable to init FEC\n");
		return ret;
	}

	return ret;
}
#endif

int misc_init_r(void)
{
	char *s = getenv("serial#");

	/* enable red LED to indicate a running bootloader */
	gpio_direction_output(GPIO_LED_RED, 1);

	puts("Board: I2SE Duckbill\n");
	if (s && s[0]) {
		puts("Serial: ");
		puts(s);
		putc('\n');
	}

	return 0;
}
