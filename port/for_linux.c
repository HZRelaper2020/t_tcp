#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>

#include <pcap.h>

#define BUFSIZE 1024*10

#define ERROR(a) printf a
#define PRINT(a) printf a

static uint8_t xC;
static pcap_t * handle;
static void (*recv_callback)(const uint8_t* data,int len);

static void print_hex(const uint8_t* data,int len)
{
	for (int i=0;i<len;i++){
		printf("%02x ",*(data+i));
		if (i%16 == 15) printf("\n");
	}
	printf("\n\n");
}

static void pcap_recv_callback( unsigned char * user,
                           const struct pcap_pkthdr * pkt_header,
                           const u_char * pkt_data )
{
	if (recv_callback){
		recv_callback(pkt_data,pkt_header->len);
	}
//	print_hex(pkt_data,pkt_header->len);
}

static void* pcap_recv_thread(void* param)
{
	int ret;
	pcap_t* handle = (pcap_t*) param;

#if 0
	FILE* fd = fopen("recv.bin","rb");

	uint8_t buf[2048];
	int len =0;
	int times = 0;
	while (1){
		int readlen = fread(&len,1,4,fd);
		if (readlen < 1) break;
		readlen = fread(buf,1,len,fd);
		if (readlen < 1) break;

		times += 1;
		
		if (times == 200000){
			break;
		}
		recv_callback(buf,len);
	//	printf("times:%d,len%d\n",times,len);
	}

	fclose(fd);
	return NULL;
#endif
	while(1){
		ret=pcap_dispatch(handle,1,pcap_recv_callback,NULL);
		if (ret == -1){
			ERROR(("pcap_dispatch failed %s",pcap_geterr(handle)));
			break;
		}
	}
	return NULL;
}

static void* pcap_send_thread(void* param)
{
	pcap_t* handle = (pcap_t*) param;

	uint8_t buf[1624]={0xf8,0x63,0x3f,0x6b,0x2c,0xbd,0xb0,0x6e,0xbf,0x37,0x50,0xbd,0x08,0x00,0x45,0x00,0x00,0x54,0xe9,0x63,0x40,0x00,0x40,0x01,0xcd,0x2a,0xc0,0xa8,0x01,0x64,0xc0,0xa8,0x01,0x66,0x08,0x00,0xa1,0x1f,0x00,0x03,0x00,0x01,0x24,0x53,0x0b,0x61,0x00,0x00,0x00,0x00,0x66,0x55,0x02,0x00,0x00,0x00,0x00,0x00,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37};
	while(1){
		if (xC == '1'){
			pcap_sendpacket(handle,buf,82);
			xC = 0;
		}
	}

	return NULL;
}

int linux_send_data(uint8_t* data,int len)
{
	pcap_sendpacket(handle,data,len);
	return len;
}

int linux_set_recv_callback(void (*callback)(const uint8_t* data,int size))
{
	recv_callback = callback;
	return 0;
}


int linux_ether_init()
{
	uint8_t buf[BUFSIZE];
	uint8_t* dev;
	int num;
	int ret;
	pcap_if_t *it;
	struct bpf_program fp;
	pthread_t tid;

	do{
		if (!(handle=pcap_open_live("enp2s0",BUFSIZE,1,1000,buf))){
			ERROR(("pcap_open_live failed %s\n",buf));
			break;
		}

		if (pthread_create(&tid,NULL,pcap_recv_thread,(void*)handle)){
			perror("pthread_create");
			break;
		}
		/*
		if (pthread_create(&tid,NULL,pcap_send_thread,(void*)handle)){
			perror("pthread_create");
			break;
		} */

		
	}while(0);

#if 0
	while(1){
		xC = getchar();		
	}
	if (handle)
		pcap_close(handle);
#endif

	return 0;
}
