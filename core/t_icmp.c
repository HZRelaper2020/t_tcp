#include "t_common.h"

int t_icmp_reply(struct t_netif* inp,struct t_pbuf*p)
{
	int ret = 0;
	

	return ret;
}

int t_icmp_input(struct t_pbuf* p,struct t_netif* inp)
{
	int ret = 0;

	struct t_ip_hdr* iphdr = (struct t_ip_hdr*)p->payload;
	uint16_t hlen = T_IPH_HL(iphdr)*4;
	struct t_ip_addr tmpaddr;
	
	if (hlen < 100){
		t_pbuf_header(p,hlen);
		uint8_t type = *(uint8_t*)p->payload;
		struct t_icmp_echo_hdr* echo = (struct t_icmp_echo_hdr*)p->payload;
		switch(type){
			case T_ICMP_ECHO_REQUEST:
			// ip layer	
				tmpaddr.addr = iphdr->src.addr;
				iphdr->src.addr = iphdr->dst.addr;
				iphdr->dst.addr = tmpaddr.addr;
			// icmp layer
				echo->_type_code = (htons(echo->_type_code) & 0xff00); 
				if (echo->chksum > (htons(0xffff - (T_ICMP_ECHO_REQUEST <<8)))){
					echo->chksum += htons(T_ICMP_ECHO_REQUEST<<8) +1;
				}else{
					echo->chksum += htons(T_ICMP_ECHO_REQUEST<<8);
				}
				t_ip_output(inp,p,NULL,&iphdr->src,T_IPH_TTL(iphdr),0,T_IP_PROTO_ICMP);
				break;
		}
	}else{
		ERROR(("t_icmp_input: hlen is too big"));
	}

	return ret;
}
