#include "t_common.h"

struct t_arp_entry{
	struct t_ip_addr ip;
	uint8_t mac[6];
	uint8_t status;
};

static struct t_arp_entry t_arp_table[T_ARP_TABLE_SIZE];
static uint16_t table_count = 0;

void t_arp_init()
{
	table_count = 0;
}

void t_print_arp_table()
{
	for (int i = 0;i<table_count;i++){
		uint8_t* mac = t_arp_table[i].mac;
		printf("%d %08x %02x:%02x:%02x:%02x:%02x:%02x\n",
				i,t_arp_table[i].ip.addr,
				mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	}
}

static void t_update_arp_entry(struct t_netif* inp,struct t_ip_addr* ip,uint8_t* mac)
{
	if (!ip_addr_isany(ip) &&  !ip_addr_isbroadcast(ip)){
		uint8_t find = 0;
		for (int i=0;i<table_count;i++){
			if (!memcmp(mac,t_arp_table[i].mac,6)){
				t_arp_table[i].ip.addr = ip->addr;
				find = 1;
				break;
			}
		}

		if (!find){
			if (table_count < T_ARP_TABLE_SIZE){
				t_arp_table[table_count].ip.addr = ip->addr;
				memcpy(t_arp_table[table_count].mac,mac,6);
				table_count += 1;
			}else{
				ERROR(("t_update_arp_entry:table_count is too big"));
			}
		}
	}
}

int t_arp_request(struct t_netif* inp,struct t_ip_addr* ip)
{
	int ret = 0;
	struct t_pbuf* p;
	struct t_etharp_hdr *hdr;

	p= t_pbuf_alloc(0,sizeof(struct t_etharp_hdr),T_PBUF_FLAG_POOL);

	if (p == NULL){
		ret = -1;
	}else{
		hdr = (struct t_etharp_hdr*)p->payload;

		hdr->hwtype = htons(0x1);
		hdr->proto = htons(T_IPV4_FRAME_TYPE);
		hdr->hwlen_protolen = htons(0x0604);
		hdr->opcode = htons(T_ARP_REQUEST);

		memcpy(hdr->sender_mac,inp->hwaddr,6);
		hdr->sender_ip.addr = inp->ip_addr.addr;

		memset(hdr->target_mac,0xff,6);
		hdr->target_ip.addr = ip->addr;

		memset(hdr->ethhdr.dst,0xff,6);
		memcpy(hdr->ethhdr.src,inp->hwaddr,6);
		hdr->ethhdr.frame_type = htons(T_ARP_FRAME_TYPE);

		inp->output(inp,p);

		t_pbuf_free(p);
	}
	return ret;
}

t_err_t t_arp_input(struct t_pbuf *p,struct t_netif* inp)
{
	if (p->tot_len >= sizeof(struct t_etharp_hdr)){ 
		struct  t_etharp_hdr* hdr = (struct t_etharp_hdr*)p->payload;

		int for_us = !t_ip_addr_cmp(&hdr->target_ip,&inp->ip_addr);
		if (for_us){
			switch(htons(hdr->opcode)){
				case T_ARP_REQUEST: 
					hdr->opcode = htons(T_ARP_REPLY);

					hdr->target_ip.addr = hdr->sender_ip.addr;
					hdr->sender_ip.addr = inp->ip_addr.addr;

					for (int i=0;i<6;i++){
						hdr->ethhdr.dst[i] = hdr->ethhdr.src[i];
						hdr->target_mac[i] = hdr->ethhdr.src[i];

						hdr->ethhdr.src[i] = inp->hwaddr[i];
						hdr->sender_mac[i] = inp->hwaddr[i];

					} 
					inp->output(inp,p);
					break;
				case T_ARP_REPLY: 
					t_update_arp_entry(inp,&hdr->sender_ip,hdr->sender_mac);
					break;
				default:
					ERROR(("t_arp_input not supported opcde"));
					break;
			}
		}
	}

        return 0;
}

#ifdef T_TEST_ARP
int t_arp_test(struct t_netif* netif)
{
	int times = 0;
	while(1){
		char c = getchar();
		if (c == 0x31){ // 1
			struct t_ip_addr ip;
			ip.addr = 0x6401A8C0 + (times<<24);
			times += 1;
			PRINT(("send request %08x\n",ip.addr));
			t_arp_request(netif,&ip);
		}else if (c== 0x32){ // 2
			PRINT(("print arp table\n"));
			t_print_arp_table();
		}
	}
	return 0;
}
#endif
