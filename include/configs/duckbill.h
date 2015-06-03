/*
 * Copyright (C) 2014-2015 Michael Heimpold <mhei@heimpold.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#ifndef __CONFIGS_DUCKBILL_H__
#define __CONFIGS_DUCKBILL_H__

/* System configurations */
#define CONFIG_MX28				/* i.MX28 SoC */
#define CONFIG_MACH_TYPE	MACH_TYPE_DUCKBILL

#define CONFIG_MISC_INIT_R

#define CONFIG_SYS_MXS_VDD5V_ONLY

/* U-Boot Commands */
#define CONFIG_SYS_NO_FLASH
#include <config_cmd_default.h>
#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DOS_PARTITION

#define CONFIG_CMD_BOOTZ
#define CONFIG_CMD_CACHE
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_EXT4
#define CONFIG_CMD_EXT4_WRITE
#define CONFIG_CMD_FAT
#define CONFIG_CMD_FUSE
#define CONFIG_CMD_GPIO
#define CONFIG_CMD_I2C
#define CONFIG_CMD_ITEST
#define CONFIG_CMD_MII
#define CONFIG_CMD_MMC
#define CONFIG_CMD_NET
#define CONFIG_CMD_NFS
#define CONFIG_CMD_PING
#define CONFIG_CMD_SAVEENV
#define CONFIG_CMD_SETEXPR
#define CONFIG_CMD_SPI
#define CONFIG_CMD_UNZIP

/* Memory configuration */
#define CONFIG_NR_DRAM_BANKS		1		/* 1 bank of DRAM */
#define PHYS_SDRAM_1			0x40000000	/* Base address */
#define PHYS_SDRAM_1_SIZE		0x40000000	/* Max 1 GB RAM */
#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM_1

/* Environment is in MMC */
#define CONFIG_ENV_OVERWRITE
#define CONFIG_ENV_IS_IN_MMC		1
#define CONFIG_ENV_SIZE			(128 * 1024)
#define CONFIG_ENV_OFFSET		(128 * 1024)
#define CONFIG_ENV_OFFSET_REDUND	(256 * 1024)
#define CONFIG_SYS_MMC_ENV_DEV		0
#define CONFIG_SYS_REDUNDAND_ENVIRONMENT

/* FEC Ethernet on SoC */
#ifdef	CONFIG_CMD_NET
#define CONFIG_FEC_MXC
#define CONFIG_NET_MULTI
#define CONFIG_MX28_FEC_MAC_IN_OCOTP
#define CONFIG_FEC_MXC_PHYADDR	1
#define IMX_FEC_BASE		MXS_ENET0_BASE
#endif

#define CONFIG_IPADDR		192.168.1.10
#define CONFIG_SERVERIP		192.168.1.1
#define CONFIG_NETMASK		255.255.255.0
#define CONFIG_GATEWAYIP	192.168.1.254

/* BOOTP options */
#define CONFIG_BOOTP_SUBNETMASK
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_HOSTNAME

/* SPI */
#ifdef CONFIG_CMD_SPI
#define CONFIG_DEFAULT_SPI_BUS		2
#define CONFIG_DEFAULT_SPI_MODE		SPI_MODE_0
#endif

/* Boot Linux */
#define CONFIG_BOOTDELAY	1
#define CONFIG_BOOTFILE		"zImage"
#define CONFIG_LOADADDR		0x42000000
#define CONFIG_SYS_LOAD_ADDR	CONFIG_LOADADDR

/* Extra Environment */
#define CONFIG_EXTRA_ENV_SETTINGS \
	"update_sd_firmware_filename=openwrt-mxs-root.ext4\0" \
	"update_sd_firmware=" \
		"if mmc rescan; then " \
		"if tftp ${update_sd_firmware_filename}; then " \
		"setexpr fw_sz ${filesize} / 200; " \
		"setexpr fw_sz ${fw_sz} + 1; " \
		"mmc dev ${mmcdev} 3; " \
		"mmc write ${loadaddr} 0 ${fw_sz}; " \
		"mmc dev ${mmcdev} 2; " \
		"mmc write ${loadaddr} 0 ${fw_sz}; " \
		"mmc dev ${mmcdev}; " \
		"fi; " \
		"fi\0" \
	"erase_mmc=mmc erase 0 2\0" \
	"erase_env1=mmc erase 100 100\0" \
	"erase_env2=mmc erase 200 100\0" \
	"script=boot.scr\0" \
	"image=zImage\0" \
	"console=ttyAMA0\0" \
	"fdt_file=imx28-duckbill.dtb\0" \
	"fdt_addr=0x41000000\0" \
	"boot_fdt=try\0" \
	"ip_dyn=yes\0" \
	"bootsys=1\0" \
	"mmcdev=0\0" \
	"mmcpart=2\0" \
	"mmcroot=/dev/mmcblk0p2\0" \
	"mmcargs=setenv bootargs console=${console},${baudrate} " \
		"root=${mmcroot} " \
		"rootwait bootsys=${bootsys} panic=1\0" \
	"loadbootscript="  \
		"fatload mmc ${mmcdev}:${mmcpart} ${loadaddr} ${script};\0" \
	"bootscript=echo Running bootscript from mmc ...; "	\
		"source\0" \
	"loadimage=ext4load mmc ${mmcdev}:${mmcpart} ${loadaddr} /boot/${image}\0" \
	"loadfdt=ext4load mmc ${mmcdev}:${mmcpart} ${fdt_addr} /boot/${fdt_file}\0" \
	"mmcboot=echo Booting from mmc ...; " \
		"setexpr mmcpart 1 + ${bootsys}; " \
		"setenv mmcroot /dev/mmcblk0p${mmcpart}; " \
		"run mmcargs; " \
		"if test ${boot_fdt} = yes || test ${boot_fdt} = try; then " \
			"if run loadfdt; then " \
				"bootz ${loadaddr} - ${fdt_addr}; " \
			"else " \
				"if test ${boot_fdt} = try; then " \
					"bootz; " \
				"else " \
					"echo WARN: Cannot load the DT; " \
				"fi; " \
			"fi; " \
		"else " \
			"bootz; " \
		"fi;\0" \
	"netargs=setenv bootargs console=${console},${baudrate} " \
		"root=/dev/nfs " \
		"ip=dhcp nfsroot=${serverip}:${nfsroot},v3,tcp\0" \
	"netboot=echo Booting from net ...; " \
		"run netargs; "	\
		"if test ${ip_dyn} = yes; then " \
			"setenv get_cmd dhcp; " \
		"else " \
			"setenv get_cmd tftp; " \
		"fi; " \
		"${get_cmd} ${image}; " \
		"if test ${boot_fdt} = yes; then " \
			"if ${get_cmd} ${fdt_addr} ${fdt_file}; then " \
				"bootz ${loadaddr} - ${fdt_addr}; " \
			"else " \
				"if test ${boot_fdt} = try; then " \
					"bootz; " \
				"else " \
					"echo WARN: Cannot load the DT; " \
				"fi;" \
			"fi; " \
		"else " \
			"bootz; " \
		"fi;\0"

#define CONFIG_BOOTCOMMAND \
	"mmc dev ${mmcdev}; if mmc rescan; then " \
		"if run loadbootscript; then " \
			"run bootscript; " \
		"else " \
			"if run loadimage; then " \
				"run mmcboot; " \
			"else run netboot; " \
			"fi; " \
		"fi; " \
	"else run netboot; fi"

/* The rest of the configuration is shared */
#include <configs/mxs.h>

#endif /* __CONFIGS_DUCKBILL_H__ */
