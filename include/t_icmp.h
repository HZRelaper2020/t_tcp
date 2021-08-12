#ifndef SOURCE_T_ICMP__H
#define SOURCE_T_ICMP__H

struct t_icmp_echo_hdr{
	uint16_t _type_code;
	uint16_t chksum;
	uint16_t id;
	uint16_t seqno;
};

int t_icmp_input(struct t_pbuf* p,struct t_netif* inp);

#endif
