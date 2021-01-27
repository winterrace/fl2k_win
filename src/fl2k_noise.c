/*
 * osmo-fl2k, turns FL2000-based USB 3.0 to VGA adapters into
 * low cost DACs
 *
 * Copyright (C) 2016-2018 by Steve Markgraf <steve@steve-m.de>
 *
 * SPDX-License-Identifier: GPL-2.0+
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef _WIN32
#include <unistd.h>
#define sleep_ms(ms)	usleep(ms*1000)
#else
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include "getopt/getopt.h"
#define sleep_ms(ms)	Sleep(ms)
#endif

#include "osmo-fl2k.h"

static fl2k_dev_t *dev = NULL;

static volatile int do_exit = 0;
char *txbuf = NULL;

void usage(void)
{
	fprintf(stderr,
		"fl2k_noise, a noise generator for FL2K VGA dongles\n\n"
		"Usage:\n"
		"\t[-d device_index (default: 0)]\n"
		"\t[-s samplerate (default: 64 MS/s)]\n\n"
	);
	exit(1);
}

#ifdef _WIN32
BOOL WINAPI
sighandler(int signum)
{
	if (CTRL_C_EVENT == signum) {
		fprintf(stderr, "Signal caught, exiting!\n");
		fl2k_stop_tx(dev);
		do_exit = 1;
		return TRUE;
	}
	return FALSE;
}
#else
static void sighandler(int signum)
{
	fprintf(stderr, "Signal caught, exiting!\n");
	fl2k_stop_tx(dev);
	do_exit = 1;
}
#endif

int r = 0;
void fl2k_callback(fl2k_data_info_t *data_info)
{
	if (data_info->device_error) {
		fprintf(stderr, "Device error, exiting.\n");
		do_exit = 1;
		return;
	}
	int g, b;
	g = (r + 1) % 3;
	b = (r + 2) % 3;

	data_info->sampletype_signed = 1;
	data_info->r_buf = txbuf + r * FL2K_BUF_LEN;
	data_info->g_buf = txbuf + r * FL2K_BUF_LEN;
	data_info->g_buf = txbuf + r * FL2K_BUF_LEN;

	if (++r > 2) r = 0;
}

uint64_t lfbsr(uint64_t* i, uint64_t* fb) 
{
	if (*i & 1) { *i = (*i >> 1) ^ *fb; }
	else { *i = (*i >> 1); }
	return *i;
}

uint64_t r0 =  0x1234567890123456ULL;
uint64_t fb0 = 0x800000000000057CULL;

int main(int argc, char **argv)
{
#ifndef _WIN32
	struct sigaction sigact, sigign;
#endif
	int r, opt, i;
	uint32_t samp_rate = 64000000;
	uint32_t buf_num = 0;
	int dev_index = 0;
	void *status;

	while ((opt = getopt(argc, argv, "d:s:")) != -1) {
		switch (opt) {
		case 'd':
			dev_index = (uint32_t)atoi(optarg);
			break;
		case 's':
			samp_rate = (uint32_t)atof(optarg);
			break;
		default:
			usage();
			break;
		}
	}

	txbuf = malloc(FL2K_XFER_LEN); 
	if (!txbuf) {
		fprintf(stderr, "malloc error!\n");
		goto out;
	}

	
	for (i = 0; i < FL2K_XFER_LEN; i++)
	{
		txbuf[i] = lfbsr(&r0, &fb0); 
	}

	fl2k_open(&dev, (uint32_t)dev_index);
	if (NULL == dev) {
		fprintf(stderr, "Failed to open fl2k device #%d.\n", dev_index);
		goto out;
	}

	r = fl2k_start_tx(dev, fl2k_callback, NULL, 0);

	/* Set the sample rate */
	r = fl2k_set_sample_rate(dev, samp_rate);
	if (r < 0)
		fprintf(stderr, "WARNING: Failed to set sample rate.\n");


#ifndef _WIN32
	sigact.sa_handler = sighandler;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;
	sigign.sa_handler = SIG_IGN;
	sigaction(SIGINT, &sigact, NULL);
	sigaction(SIGTERM, &sigact, NULL);
	sigaction(SIGQUIT, &sigact, NULL);
	sigaction(SIGPIPE, &sigign, NULL);
#else
	SetConsoleCtrlHandler( (PHANDLER_ROUTINE) sighandler, TRUE );
#endif

	while (!do_exit)
	{
		sleep_ms(100);
		
		for (i = 0; i < FL2K_XFER_LEN; i++)
		{
			txbuf[i] = lfbsr(&r0, &fb0);
		}
	}
	fl2k_close(dev);

out:
	if (txbuf)
		free(txbuf);

	return 0;
}
