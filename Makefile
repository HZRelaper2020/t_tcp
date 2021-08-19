sources := port/for_linux.c 

sources += api/t_tcpip.c 
sources += api/t_sockets.c
sources += api/t_api_lib.c

sources += include/arch/t_sys.c

sources += core/t_ip.c
sources += core/t_netif.c
sources += core/t_pbuf.c
sources += core/t_memp.c
sources += core/t_arp.c
sources += core/t_icmp.c
sources += core/t_inet.c
sources += core/t_udp.c

sources += user/s_main.c

INC := -I./include
INC += -I.
INC += -I./user
INC += -I./include/ipv4

FLAGS := -g
#FLAGS += -Xlinker -Map=file.map
#FLAGS += -DT_TEST_MALLOC
#FLAGS += -DT_TEST_ARP
#FLAGS += -DT_PBUF_TEST
FLAGS += -DUSER_MAIN
FLAGS += -DDEBUG 

main:
	gcc ${sources} ${FLAGS} -lpcap -lpthread ${INC} 
	sudo ./a.out

debug:
	gcc ${sources} ${FLAGS} -lpcap -lpthread ${INC} 
	sudo gdb ./a.out

tt:
	gcc ${sources} ${FLAGS} -lpcap -lpthread ${INC} 
