#ifndef SOURCE_T_INET__H
#define SOURCE_T_INET__H

u16_t  
t_inet_chksum(void *dataptr, u16_t len);

u16_t
t_inet_chksum_pseudo(struct t_pbuf *p,
       struct t_ip_addr *src, struct t_ip_addr *dest,
       u8_t proto, u16_t proto_len);

#endif
