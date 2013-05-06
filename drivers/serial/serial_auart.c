/*
 * (c) 2011 Wolfgang Ocker <weo@reccoware.de>
 *
 * Based on the standard DUART serial driver:
 *
 * (c) 2007 Sascha Hauer <s.hauer@pengutronix.de>
 *
 * (C) Copyright 2009-2010 Freescale Semiconductor, Inc.
 *
 * Further based on the Linux mxs-auart.c driver:
 *
 * Freescale STMP37XX/STMP378X Application UART driver
 *
 * Author: dmitry pervushin <dimka@embeddedalley.com>
 *
 * Copyright 2008-2010 Freescale Semiconductor, Inc.
 * Copyright 2008 Embedded Alley Solutions, Inc All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#define REG_RD(base, reg) \
	(*(volatile unsigned int *)((base) + (reg)))
#define REG_WR(base, reg, value) \
	((*(volatile unsigned int *)((base) + (reg))) = (value))


#include <common.h>
#include <asm/arch/imx-regs.h>
#include <serial.h>

DECLARE_GLOBAL_DATA_PTR;

static void mxs_auart_reset(void)
{
	int i;

	REG_WR(MXS_UARTAPP_BASE, HW_UARTAPP_CTRL0_CLR,
		BM_UARTAPP_CTRL0_SFTRST);

	for (i = 0; i < 10000; i++) {
		unsigned int reg = REG_RD(MXS_UARTAPP_BASE, HW_UARTAPP_CTRL0);
		if (!(reg & BM_UARTAPP_CTRL0_SFTRST))
			break;
		udelay(3);
	}

	REG_WR(MXS_UARTAPP_BASE, HW_UARTAPP_CTRL0_CLR,
		BM_UARTAPP_CTRL0_CLKGATE);
}

/*
 * Set baud rate. The settings are always 8n1:
 * 8 data bits, no parity, 1 stop bit
 */
void mxs_serial_setbrg(void)
{
	u32 div;
	u32 linectrl = 0;

	/* Calculate and set baudrate */
	div = (CONFIG_UARTAPP_CLK * 32) / gd->baudrate;
	linectrl |= BF_UARTAPP_LINECTRL_BAUD_DIVFRAC(div & 0x3F);
	linectrl |= BF_UARTAPP_LINECTRL_BAUD_DIVINT(div >> 6);
	linectrl |= BF_UARTAPP_LINECTRL_WLEN(3);	/* 8 bit */
	linectrl |= BM_UARTAPP_LINECTRL_FEN;		/* enable fifo */

	REG_WR(MXS_UARTAPP_BASE, HW_UARTAPP_LINECTRL, linectrl);
}

int mxs_serial_init(void)
{

	/* Reset AURT */
	mxs_auart_reset();

	/* Mask interrupts */
	REG_WR(MXS_UARTAPP_BASE, HW_UARTAPP_INTR, 0);

	/* Set default baudrate */
	mxs_serial_setbrg();

	/*
	 * Disable RTS/CTS, enable Rx, Tx, UART */
	REG_WR(MXS_UARTAPP_BASE, HW_UARTAPP_CTRL2_CLR,
		BM_UARTAPP_CTRL2_RTSEN | BM_UARTAPP_CTRL2_CTSEN |
		BM_UARTAPP_CTRL2_USE_LCR2);
	REG_WR(MXS_UARTAPP_BASE, HW_UARTAPP_CTRL2_SET,
		BM_UARTAPP_CTRL2_RXE | BM_UARTAPP_CTRL2_TXE |
		BM_UARTAPP_CTRL2_UARTEN);

	return 0;
}

/* Send a character */
void mxs_serial_putc(const char c)
{
	/* Wait for room in TX FIFO */
	while (REG_RD(MXS_UARTAPP_BASE, HW_UARTAPP_STAT) &
	       BM_UARTAPP_STAT_TXFF)
		;

	/* Write the data byte */
	REG_WR(MXS_UARTAPP_BASE, HW_UARTAPP_DATA, c);

	if (c == '\n')
		mxs_serial_putc('\r');
}

void mxs_serial_puts(const char *s)
{
	while (*s)
		mxs_serial_putc(*s++);
}

/* Test whether a character is in TX buffer */
int mxs_serial_tstc(void)
{
	/* Check if RX FIFO is not empty */
	return !(REG_RD(MXS_UARTAPP_BASE, HW_UARTAPP_STAT) &
		 BM_UARTAPP_STAT_RXFE);
}

/* Receive character */
int mxs_serial_getc(void)
{
	/* Wait while RX FIFO is empty */
	while (!mxs_serial_tstc())
		;

	/* Read data byte */
	return REG_RD(MXS_UARTAPP_BASE, HW_UARTAPP_DATA) & 0xff;
}


static struct serial_device mxs_serial_drv = {
	.name	= "mxs_serial",
	.start	= mxs_serial_init,
	.stop	= NULL,
	.setbrg	= mxs_serial_setbrg,
	.putc	= mxs_serial_putc,
	.puts	= default_serial_puts,
	.getc	= mxs_serial_getc,
	.tstc	= mxs_serial_tstc,
};

void mxs_serial_initialize(void)
{
	serial_register(&mxs_serial_drv);
}

__weak struct serial_device *default_serial_console(void)
{
	return &mxs_serial_drv;
}
