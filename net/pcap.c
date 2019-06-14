// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright 2019 Ramon Fried <rfried.dev@gmail.com>
 */

#include <common.h>
#include <net.h>
#include <time.h>
#include <asm/io.h>

#define LINKTYPE_ETHERNET	1

static void *buf;
static unsigned int pos;

struct pcap_header {
	u32 magic;
	u16 version_major;
	u16 version_minor;
	s32 thiszone;
	u32 sigfigs;
	u32 snaplen;
	u32 network;
};

struct pcap_packet_header {
	u32 ts_sec;
	u32 ts_usec;
	u32 incl_len;
	u32 orig_len;
};

static struct pcap_header file_header = {
	.magic = 0xa1b2c3d4,
	.version_major = 2,
	.version_minor = 4,
	.snaplen = 65535,
	.network = LINKTYPE_ETHERNET,
};

int pcap_init(void)
{
	buf = map_physmem(CONFIG_NET_PCAP_ADDR, CONFIG_NET_PCAP_SIZE, 0);
	if (!buf) {
		printf("Failed mapping PCAP memory\n");
		return -ENOMEM;
	}

	printf("PCAP capture initialized: addr: 0x%lx max length: 0x%x\n",
	       (unsigned long)buf, CONFIG_NET_PCAP_SIZE);

	memcpy(buf, &file_header, sizeof(file_header));
	pos += sizeof(file_header);
	return 0;
}

int pcap_post(const void *packet, size_t len)
{
	struct pcap_packet_header header;
	u64 cur_time = timer_get_us();

	if (!buf)
		return -ENODEV;
	if ((pos + len + sizeof(header)) >= CONFIG_NET_PCAP_SIZE)
		return -ENOMEM;

	header.ts_sec = cur_time / 1000000;
	header.ts_sec = cur_time % 1000000;
	header.incl_len = len;
	header.orig_len = len;

	memcpy(buf + pos, &header, sizeof(header));
	pos += sizeof(header);
	memcpy(buf + pos, packet, len);
	pos += len;

	return 0;
}

unsigned int pcap_size(void)
{
	return pos;
}
