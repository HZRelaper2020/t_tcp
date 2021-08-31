#ifndef SOURCE_T_PBUF__H
#define SOURCE_T_PBUF__H

#include <stdint.h>
#include <stdio.h>

#define T_PBUF_FLAG_POOL 	0x1
#define T_PBUF_FALG_RAM		0x4

#define T_PBUF_TRANSPORT	(14+20+20)
#define T_PBUF_IP		(14+20)

#define T_PBUF_RAM		0
#define T_PBUF_REF		0

struct t_pbuf{
	struct t_pbuf *next;
	void* payload;

	uint16_t tot_len;
	uint16_t len;
	uint16_t flags;
	uint16_t ref;
	uint16_t reserved;
};

void t_pbuf_init();

struct t_pbuf* t_pbuf_alloc(uint16_t offset,uint16_t length,uint16_t flags);

int t_pbuf_free(struct t_pbuf* p);

int t_pbuf_copy_data(struct t_pbuf*p,uint8_t* data,int size);

int t_pbuf_header(struct t_pbuf *p,int increment);

int t_pbuf_cat(struct t_pbuf*p,struct t_pbuf*p1);

#endif
