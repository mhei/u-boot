/*
 * Copyright (C) 2013 Michael Heimpold <mhei@heimpold.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#ifndef __DUCKBILL_H
#define __DUCKBILL_H

#define CONFIG_DUCKBILL

/* SoC configurations */
#define CONFIG_MX28				/* i.MX28 SoC */

#define CONFIG_MXS_GPIO				/* GPIO control */
#define CONFIG_SYS_HZ			1000	/* Ticks per second */

/* This should be removed after it's added into mach-types.h */
#ifndef MACH_TYPE_DUCKBILL
#define MACH_TYPE_DUCKBILL	4754
#endif

#define CONFIG_MACH_TYPE		MACH_TYPE_DUCKBILL

#include <asm/arch/regs-base.h>

#define CONFIG_SYS_NO_FLASH
#define CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_ARCH_MISC_INIT
#define CONFIG_MISC_INIT_R

/* SPL */
#define CONFIG_SPL
#define CONFIG_SPL_NO_CPU_SUPPORT_CODE
#define CONFIG_SPL_START_S_PATH	"arch/arm/cpu/arm926ejs/mxs"
#define CONFIG_SPL_LDSCRIPT	"arch/arm/cpu/arm926ejs/mxs/u-boot-spl.lds"
#define CONFIG_SPL_LIBCOMMON_SUPPORT
#define CONFIG_SPL_LIBGENERIC_SUPPORT
#define CONFIG_SPL_GPIO_SUPPORT
#define CONFIG_SPL_MXS_NO_DCDC_BATT_SOURCE

/* U-Boot Commands */
#include <config_cmd_default.h>
#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DOS_PARTITION

#define CONFIG_CMD_CACHE
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_GPIO
#define CONFIG_CMD_I2C
#define CONFIG_CMD_MII
#define CONFIG_CMD_MMC
#define CONFIG_CMD_NET
#define CONFIG_CMD_NFS
#define CONFIG_CMD_PING
#define CONFIG_CMD_SETEXPR
#define CONFIG_CMD_BOOTZ
#define CONFIG_CMD_ITEST
#define CONFIG_CMD_UNZIP
#define CONFIG_CMD_UNXZ
#define CONFIG_XZ

/* Memory configurations */
#define CONFIG_NR_DRAM_BANKS		1		/* 1 bank of DRAM */
#define PHYS_SDRAM_1			0x40000000	/* Base address */
#define PHYS_SDRAM_1_SIZE		0x40000000	/* Max 1 GB RAM */
#define CONFIG_SYS_MALLOC_LEN		0x00400000	/* 4 MB for malloc */
#define CONFIG_SYS_MEMTEST_START	0x40000000	/* Memtest start adr */
#define CONFIG_SYS_MEMTEST_END		0x40400000	/* 4 MB RAM test */
#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM_1
/* Point initial SP in SRAM so SPL can use it too. */

#define CONFIG_SYS_INIT_RAM_ADDR	0x00000000
#define CONFIG_SYS_INIT_RAM_SIZE	(128 * 1024)

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

/*
 * We need to sacrifice first 4 bytes of RAM here to avoid triggering some
 * strange BUG in ROM corrupting first 4 bytes of RAM when loading U-Boot
 * binary. In case there was more of this mess, 0x100 bytes are skipped.
 */
#define CONFIG_SYS_TEXT_BASE	0x40000100

#define CONFIG_ENV_OVERWRITE
/* U-Boot general configurations */
#define CONFIG_SYS_LONGHELP
#define CONFIG_SYS_PROMPT	"> "
#define CONFIG_SYS_CBSIZE	1024	/* Console I/O buffer size */
#define CONFIG_SYS_PBSIZE	\
	(CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
					/* Print buffer size */
#define CONFIG_SYS_MAXARGS	32	/* Max number of command args */
#define CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE
					/* Boot argument buffer size */
#define CONFIG_VERSION_VARIABLE	/* U-BOOT version */
#define CONFIG_AUTO_COMPLETE		/* Command auto complete */
#define CONFIG_CMDLINE_EDITING		/* Command history etc */
#define CONFIG_SYS_HUSH_PARSER

/* Serial Driver */
#define CONFIG_PL011_SERIAL
#define CONFIG_PL011_CLOCK		24000000
#define CONFIG_PL01x_PORTS		{ (void *)MXS_UARTDBG_BASE }
#define CONFIG_CONS_INDEX		0
#define CONFIG_BAUDRATE			115200

/* DMA */
#define CONFIG_APBH_DMA

/* MMC Driver */
#define CONFIG_ENV_IS_IN_MMC
#ifdef CONFIG_ENV_IS_IN_MMC
 #define CONFIG_ENV_OFFSET		(0x20000)		/* 128 KiB */
 #define CONFIG_ENV_SIZE		(0x20000)		/* 128 KiB */
 #define CONFIG_SYS_MMC_ENV_DEV		0
 #define CONFIG_ENV_OFFSET_REDUND	(0x40000)		/* 128 KiB */
 #define CONFIG_SYS_REDUNDAND_ENVIRONMENT
#endif
#define CONFIG_CMD_SAVEENV
#ifdef CONFIG_CMD_MMC
#define CONFIG_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_BOUNCE_BUFFER
#define CONFIG_MXS_MMC
#define CONFIG_CMD_EXT2                 /* EXT2 Support */
#define CONFIG_CMD_EXT4                 /* EXT4 Support */
#endif

/* Ethernet on SOC (FEC) */
#ifdef CONFIG_CMD_NET
#define CONFIG_ETHPRIME		"FEC"
#define CONFIG_FEC_MXC
#define CONFIG_MII
#define CONFIG_FEC_XCV_TYPE	RMII
#if 0
#define CONFIG_MX28_FEC_MAC_IN_OCOTP /* use MAC from environment */
#endif
#define CONFIG_PHYLIB
#define CONFIG_FEC_MXC_PHYADDR	1
#define IMX_FEC_BASE		MXS_ENET0_BASE
#define CONFIG_PHY_SMSC
#define CONFIG_NETCONSOLE
#endif

#define CONFIG_IPADDR		192.168.1.10
#define CONFIG_SERVERIP		192.168.1.1
#define CONFIG_NETMASK		255.255.255.0
#define CONFIG_GATEWAYIP	192.168.1.254

/* I2C */
#ifdef CONFIG_CMD_I2C
#define CONFIG_I2C_MXS
#define CONFIG_HARD_I2C
#define CONFIG_SYS_I2C_SPEED	400000
#endif

/* BOOTP options */
#define CONFIG_BOOTP_SUBNETMASK
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_HOSTNAME

/* Boot Linux */
#define CONFIG_CMDLINE_TAG
#define CONFIG_SETUP_MEMORY_TAGS

#define CONFIG_BOOTDELAY		2	/* autoboot after 2s */
#define CONFIG_BOOT_RETRY_TIME		120	/* retry autoboot after 120s */
#define CONFIG_BOOT_RETRY_TIME_MIN	1	/* can go down to 1s */
#define CONFIG_AUTOBOOT_KEYED
#define CONFIG_AUTOBOOT_PROMPT		"Autobooting in %d seconds, " \
		"press <c> to stop\n", bootdelay
#define CONFIG_AUTOBOOT_DELAY_STR	"\x63"	/* allows retry after retry time */
#define CONFIG_AUTOBOOT_STOP_STR	" "	/* stop autoboot with <Space> */
#define CONFIG_RESET_TO_RETRY			/* reset board to retry booting */

#define CONFIG_BOOTFILE		"zImage"
#define CONFIG_BOOTCOMMAND	"run find_bootsys"
#define CONFIG_LOADADDR		0x42000000
#define CONFIG_SYS_LOAD_ADDR	CONFIG_LOADADDR
#define CONFIG_OF_LIBFDT

/* Extra Environment */
#define CONFIG_EXTRA_ENV_SETTINGS \
		"console=ttyAMA0\0"                                                               \
		"ethaddr=02:c0:1d:c0:ff:ee\0"                                                     \
		"bootsys=1\0"                                                                     \
		"mmcpart=2\0"                                                                     \
		"fdt_file=imx28-duckbill.dtb\0"                                            \
		"fdt_addr=0x41000000\0"                                                           \
		"erase_mmc=mmc erase 0 2\0"                                                       \
		"erase_env1=mmc erase 100 100\0"                                                  \
		"erase_env2=mmc erase 200 100\0"                                                  \
		"loadfdt=ext4load mmc 0:${mmcpart} ${fdt_addr} /boot/${fdt_file}\0"               \
		"loadimage=ext4load mmc 0:${mmcpart} ${loadaddr} /boot/${bootfile}\0"             \
		"setmmcpart=setexpr mmcpart 1 + ${bootsys}\0"                                     \
		"addrootfs=setenv bootargs ${bootargs} root=/dev/mmcblk0p${mmcpart} rootwait\0"   \
		"addconsole=setenv bootargs ${bootargs} console=${console},${baudrate}\0"         \
		"addmisc=setenv bootargs ${bootargs} bootsys=${bootsys} panic=1\0"                \
		"mmcboot=run setmmcpart addrootfs addconsole addmisc loadfdt loadimage; "         \
				"bootz ${loadaddr} - ${fdt_addr}\0"                               \
		"find_bootsys=echo starting bootscript; "                                         \
			"if test -n ${bootsys}; then "                                            \
				"if test -n ${checksys}; then "                                   \
					"if test ${checksys} = 0; then "                          \
						"echo Previous boot of the new system failed, "   \
							"trying to boot the old one.; "           \
						"if test ${bootsys} = 1; then "                   \
							"echo Booting sys 2; "                    \
							"setenv bootsys 2; "                      \
							"run mmcboot; "                           \
						"else "                                           \
							"echo Booting sys 1; "                    \
							"setenv bootsys 1; "                      \
							"run mmcboot; "                           \
						"fi; "                                            \
					"else "                                                   \
						"setenv checksys 0; saveenv; "                    \
					"fi; "                                                    \
				"fi; "                                                            \
				"if test ${bootsys} = 1; then "                                   \
					"echo Booting sys 1 && run mmcboot || "                   \
					"echo Booting alt. sys 2 && setenv bootsys 2 && "         \
						"run mmcboot; "                                   \
				"else "                                                           \
					"if test ${bootsys} = 2; then "                           \
						"echo Booting sys 2 && run mmcboot || "           \
						"echo Booting alt. sys 1 && setenv bootsys 1 && " \
							"run mmcboot; "                           \
					"else "                                                   \
						"echo Warning: bootsys does not contain 1 or 2, " \
							"trying to boot sys 1; "                  \
						"setenv bootsys 1; "                              \
						"run mmcboot; "                                   \
					"fi; "                                                    \
				"fi; "                                                            \
			"else "                                                                   \
				"echo Error: bootsys is not set.; "                               \
			"fi\0"                                                                    \
		"update_sd_firmware="                                                             \
			"tftp ${loadaddr} openwrt-mxs-root.ext4.xz; "                             \
			"unxz ${loadaddr} ${filesize} 43000000; "                                 \
			"setexpr fsize ${filesize} + 1ff; "                                       \
			"setexpr fsize ${fsize} / 200; "                                          \
			"mmc dev 0 3; "                                                           \
			"mmc write 43000000 0 ${fsize}; "                                         \
			"mmc dev 0 2; "                                                           \
			"mmc write 43000000 0 ${fsize}; "                                         \
			"mmc dev 0\0"

#endif /* __DUCKBILL_H */
