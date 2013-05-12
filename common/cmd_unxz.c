/*
 * (C) Copyright 2013
 * Michael Heimpold, mhei@heimpold.de.
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <xz.h>
#include <unxz.h>

static int do_unxz(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned long src, src_len, dst, dst_len = ~0UL;
	int rv;

	if (argc != 4)
		return CMD_RET_USAGE;

	src = simple_strtoul(argv[1], NULL, 16);
	src_len = simple_strtoul(argv[2], NULL, 16);
	dst = simple_strtoul(argv[3], NULL, 16);

	rv = unxz((unsigned char *) src, src_len, (unsigned char *) dst, &dst_len);
	if (rv != 0) {
		switch (rv) {
			case XZ_FORMAT_ERROR:
				printf("Error: file format was not recognized (wrong magic bytes).\n");
				break;
			case XZ_OPTIONS_ERROR:
				printf("Error: unsupported compression options (e.g. integrity check type).\n");
				break;
			case XZ_DATA_ERROR:
				printf("Error: compressed data is corrupt.\n");
				break;
			default:
				printf("Error: unxz returned %d\n", rv);
		}
		return 1;
	}

	printf("Uncompressed size: %ld = 0x%lX\n", dst_len, dst_len);
	setenv_hex("filesize", dst_len);

	return 0;
}

U_BOOT_CMD(
	unxz,	4,	1,	do_unxz,
	"decompress a memory region using xz",
	"srcaddr srcsize dstaddr"
);
