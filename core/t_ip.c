#include "t_common.h"

t_err_t t_ip_input(struct t_pbuf *p,struct t_netif* inp)
{
	int ret = 0;
	t_pbuf_header(p,14);

	struct  t_ip_hdr* iphdr = (struct t_ip_hdr*)p->payload;

	if (T_IPH_V(iphdr) != 4){
		ERROR(("t_ip_input: not supported ip version"));
		ret = -1;
	}else{
		uint16_t iphdrlen = T_IPH_HL(iphdr) * 4;
		if (p->len > iphdrlen){
			switch(T_IPH_PROTO(iphdr)){
				case T_IP_PROTO_ICMP:
					t_icmp_input(p,inp);
					break;
				default:
					break;
			}
		}else{
			ret = -1;
			ERROR(("t_ip_input:p->len < iphdrlen"));
		}
	}

	return ret;
}

/*
 *
 *@return  0 if need to process
 */
int t_ether_is_mac_to_process(struct t_ether_hdr* header,struct t_netif* inp)
{
	int ret = 0;
//	uint8_t* dst = header->dst;
	return 0;
}

t_err_t t_raw_input(struct t_pbuf *p,struct t_netif* inp)
{
	int ret = 0;

	struct t_ether_hdr* header=(struct t_ether_hdr*)p->payload;

	switch(htons(header->frame_type)){
		case T_ARP_FRAME_TYPE:
			ret = t_arp_input(p,inp);
			break;
		case T_IPV4_FRAME_TYPE:
			//if (!t_ether_is_mac_to_process(header,inp)){
				ret = t_ip_input(p,inp);
			//}
			break;
		default:
			ret = -1;
			ERROR(("t_raw_input:not supported frame type 0x%x",header->frame_type));
//			print_hex(p->payload,p->tot_len);
				break;
	}

	t_pbuf_free(p);
	return ret;
}
