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
//			ERROR(("t_raw_input:not supported frame type 0x%x",header->frame_type));
//			print_hex(p->payload,p->tot_len);
				break;
	}

	t_pbuf_free(p);
	return ret;
}

int t_ip_output(struct t_netif* inp,struct t_pbuf* p,struct t_ip_addr *src,
		struct t_ip_addr * dst,
		uint8_t ttl,uint8_t tos, uint8_t proto)
{
	int ret =0;
	if (src == NULL){
		src = & inp->ip_addr;
	}
	t_pbuf_header(p,-T_IP_HLEN);

	struct t_ip_hdr* iphdr =(struct t_ip_hdr*)p->payload;

	T_IPH_TTL_SET(iphdr,ttl);
	T_IPH_PROTO_SET(iphdr,proto);

	iphdr->dst.addr = dst->addr;

	T_IPH_VHLTOS_SET(iphdr,4,T_IP_HLEN/4,tos);
	T_IPH_LEN_SET(iphdr,htons(p->tot_len));
	T_IPH_OFFSET_SET(iphdr,htons(T_IP_DF));

	t_ip_addr_set(&(iphdr->src), src);
	T_IPH_CHKSUM_SET(iphdr, t_inet_chksum(iphdr, T_IP_HLEN));


	if (t_pbuf_header(p,-14)){
		ERROR(("t_bpbuf_header failed"));
	}else{
		struct t_ether_hdr *ethhdr =(struct t_ether_hdr*)p->payload;
		uint8_t buf[6] = {0xf8,0x63,0x3f,0x6b,0x2c,0xbd};
		//uint8_t buf[6] = {0xb0,0x25,0xaa,0x26,0xca,0x7c};
		// ether mac
		memcpy(ethhdr->dst,buf,6);
		memcpy(ethhdr->src,inp->hwaddr,6);
		ethhdr->frame_type = htons(0x800);
		// 
		inp->output(inp,p);
	}


	return ret;
}
