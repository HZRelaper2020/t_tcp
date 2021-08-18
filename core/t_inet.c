#ifndef LWIP_CHKSUM
#define LWIP_CHKSUM lwip_standard_chksum
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
#endif

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

