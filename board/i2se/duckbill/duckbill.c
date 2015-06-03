/*
 * I2SE Duckbill board
 *
 * (C) Copyright 2014-2015 Michael Heimpold <mhei@heimpold.de>
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

#define GPIO_PHY_RESET MX28_PAD_SSP0_DATA7__GPIO_2_7
#define GPIO_LED_GREEN MX28_PAD_AUART1_TX__GPIO_3_5
#define GPIO_LED_RED   MX28_PAD_AUART1_RX__GPIO_3_4

DECLARE_GLOBAL_DATA_PTR;

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

#ifdef	CONFIG_CMD_MMC
int board_mmc_init(bd_t *bis)
{
	return mxsmmc_initialize(bis, 0, NULL, NULL);
}
#endif

#ifdef	CONFIG_CMD_NET
int board_eth_init(bd_t *bis)
{
	int ret;

	ret = cpu_eth_init(bis);

	/* Reset PHY */
	gpio_direction_output(GPIO_PHY_RESET, 0);
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

void mx28_adjust_mac(int dev_id, unsigned char *mac)
{
	mac[0] = 0x00;
	mac[1] = 0x01;

	if (dev_id == 1) /* Let MAC1 be MAC0 + 1 by default */
		mac[5] += 1;
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
