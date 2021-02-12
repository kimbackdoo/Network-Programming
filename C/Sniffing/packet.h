//64��Ʈ �ü��: unsigned long = 8byte

/*
 * protocol type list (IP ����� Protocol �ʵ��� ���� �ش��Ѵ�.)
 */

#define os_64bits          1
//#undef os_64bits 

#define PROTO_IP        0x800
#define PROTO_ARP       0x806
#define PROTO_ICMP      0x001
#define PROTO_TCP       0x006
#define PROTO_UDP       0x011


/*
 * TCP flags list (TCP ����� flag �ʵ� ���� �ش��Ѵ�.)
 */
#define TCP_FIN      0x01
#define TCP_SYN      0x02
#define TCP_RST      0x04
#define TCP_PSH      0x08
#define TCP_ACK      0x10
#define TCP_URG      0x20
#define TCP_CTL      0x3f

// IP ����� ������ ������ ����ü
struct __attribute__((packed)) ip_header_t {
   unsigned char hlen:4;
   unsigned char version:4;
   unsigned char service;
   unsigned short length;
   unsigned short id;

   unsigned short fl_offset0:4;
   unsigned char flags:3;
   unsigned short fl_offset1:1;
   unsigned short fl_offset2:8;

   unsigned char ttl;
   unsigned char protocol;
   unsigned short checksum;

#ifdef os_64bits  
   unsigned int src_ip;
   unsigned int dst_ip;
#else
   unsigned long src_ip;
   unsigned long dst_ip;
#endif 
};

// TCP ����� ������ ������ ����ü
struct __attribute__((packed)) tcp_header_t {
   unsigned short src_port;
   unsigned short dst_port;
  
#ifdef os_64bits 
   unsigned int seqnum;
   unsigned int acknum;
#else
   unsigned long seqnum;
   unsigned long acknum;
#endif 

   unsigned char reserved2:4;
   unsigned char hlen:4;
   unsigned char flags:6;
   unsigned char reserved1:2;

   unsigned short window_size;
   unsigned short checksum;
   unsigned short urgent_point;
};

// UDP ����� ������ ������ ����ü
struct __attribute__((packed)) udp_header_t {
	unsigned short src_port;
	unsigned short dst_port;
	unsigned short length;
	unsigned short checksum;
};

// ICMP ����� ������ ������ ����ü 
struct __attribute__((packed)) icmp_header_t {
	unsigned char type;
	unsigned char code;
	unsigned short checksum;
	unsigned short id;
	unsigned short seq;
};
