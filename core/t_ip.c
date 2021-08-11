#include "t_common.h"

t_err_t t_ip_input(struct t_pbuf *p,struct t_netif* inp)
{
	static int times = 0;
	times += 1;
	printf("it's ip\n");
	return 0;
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

	switch(header->frame_type){
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
			print_hex(p->payload,p->tot_len);
				break;
	}

	t_pbuf_free(p);
	return ret;
}
