#ifndef SOURCE_IP_ADDR__H
#define SOURCE_IP_ADDR__H

#define t_ip_addr_netcmp(a,b,mask) (!(((a)->addr&(mask)->addr) == ((b)->addr&(mask)->addr)))

#define t_ip_addr_cmp(a,b) (!((a)->addr == (b)->addr))

#define ip_addr_isany(a)	((a)->addr == 0)
#define ip_addr_isbroadcast(a)	((a)->addr == 0xFFFFFFFF)

#define htons(a)  (((a)<<8&0xff00) + ((a)>>8&0x00ff))

#define t_ip_addr_set(dest, src) (dest)->addr = \
                               ((src) == NULL? 0:\
                               (src)->addr)
#define t_ip_addr_set(dest, src) (dest)->addr = \
                               ((src) == NULL? 0:\
                               (src)->addr)

struct t_ip_addr{
	uint32_t addr;
};

struct in_addr{
	u32_t s_addr;
};
#endif

