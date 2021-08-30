#ifndef LWIP_CHKSUM
#define LWIP_CHKSUM lwip_standard_chksum
#endif
#include "t_common.h"

static uint16_t
lwip_standard_chksum(void *dataptr, int len)
{
  uint32_t acc;

  for(acc = 0; len > 1; len -= 2) {
      /*    acc = acc + *((u16_t *)dataptr)++;*/
    acc += *(uint16_t *)dataptr;
    dataptr = (void *)((u16_t *)dataptr + 1);
  }

  /* add up any odd byte */
  if (len == 1) {
    acc += htons((u16_t)((*(u8_t *)dataptr) & 0xff) << 8);
  } else {
  }
  acc = (acc >> 16) + (acc & 0xffffUL);

  if ((acc & 0xffff0000) != 0) {
    acc = (acc >> 16) + (acc & 0xffffUL);
  }

  return (u16_t)acc;
}

u16_t
t_inet_chksum(void *dataptr, u16_t len)
{
  u32_t acc;

  acc = LWIP_CHKSUM(dataptr, len);
  while (acc >> 16) {
    acc = (acc & 0xffff) + (acc >> 16);
  }
  return (u16_t)~(acc & 0xffff);
}

u16_t
t_inet_chksum_pseudo(struct t_pbuf *p,
       struct t_ip_addr *src, struct t_ip_addr *dest,
       u8_t proto, u16_t proto_len)
{
  u32_t acc;
  struct t_pbuf *q;
  u8_t swapped;

  acc = 0;
  swapped = 0;
  /* iterate through all pbuf in chain */
  for(q = p; q != NULL; q = q->next) {
    acc += LWIP_CHKSUM(q->payload, q->len);
    /*LWIP_DEBUGF(INET_DEBUG, ("inet_chksum_pseudo(): unwrapped lwip_chksum()=%lx \n", acc));*/
    while (acc >> 16) {
      acc = (acc & 0xffffUL) + (acc >> 16);
    }
    if (q->len % 2 != 0) {
      swapped = 1 - swapped;
      acc = ((acc & 0xff) << 8) | ((acc & 0xff00UL) >> 8);
    }
    /*LWIP_DEBUGF(INET_DEBUG, ("inet_chksum_pseudo(): wrapped lwip_chksum()=%lx \n", acc));*/
  }

  if (swapped) {
    acc = ((acc & 0xff) << 8) | ((acc & 0xff00UL) >> 8);
  }
  acc += (src->addr & 0xffffUL);
  acc += ((src->addr >> 16) & 0xffffUL);
  acc += (dest->addr & 0xffffUL);
  acc += ((dest->addr >> 16) & 0xffffUL);
  acc += (u32_t)htons((u16_t)proto);
  acc += (u32_t)htons(proto_len);

  while (acc >> 16) {
    acc = (acc & 0xffffUL) + (acc >> 16);
  }
  return (u16_t)~(acc & 0xffffUL);
}
