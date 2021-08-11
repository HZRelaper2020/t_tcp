#ifndef SOURCE_IP_ADDR__H
#define SOURCE_IP_ADDR__H

#define t_ip_addr_netcmp(a,b,mask) (!(((a)->addr&(mask)->addr) == ((b)->addr&(mask)->addr)))

struct t_ip_addr{
	uint32_t addr;
};

#endif

