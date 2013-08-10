/*
 * =====================================================================================
 *
 *       Filename:  net.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2008年12月02日 10时13分38秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Li Hongwang (mn), hoakee@gmail.com
 *        Company:  University of Science and Technology of China
 *
 * =====================================================================================
 */

#ifndef NET_H
#define NET_H

//Ethernet header
struct mac_header{
	unsigned char		dest[6];	/* Destination node		*/
	unsigned char		src[6];	        /* Source node			*/
        unsigned short		proto;	        /* Protocol of upper layer      */
}__attribute__ ((packed));
//#define MAC_HEADER_SIZE         (14)

//ARP header
struct arp_header{
	unsigned short		ar_hrd;		/* Format of hardware address	*/
	unsigned short		ar_pro;		/* Format of protocol address	*/
	unsigned char		ar_hln;	        /* Length of hardware address	*/
	unsigned char		ar_pln;	        /* Length of protocol address	*/
	unsigned short		ar_op;		/* Operation			*/

	unsigned char		ar_sha[6];	/* Sender hardware address	*/
	unsigned long		ar_spa;	        /* Sender protocol address	*/
	unsigned char		ar_tha[6];	/* Target hardware address	*/
	unsigned long		ar_tpa;	        /* Target protocol address	*/
}__attribute__ ((packed));
//#define ARP_HEADER_SIZE         (28)

struct ip_header {
        unsigned char           ver_ihl;        /* Version 4b header length 4b  */
        unsigned char           tos;            /* Type of service              */
        unsigned short          tlen;           /* Total length of the packet   */
        unsigned short          identification; /* Identification               */
        unsigned short          flags_fo;       /* Flags 3b,Fragment offset 13b */
        unsigned char           ttl;            /* Time to live                 */             
        unsigned char           proto;          /* Protocol num of upper layer  */
        unsigned short          crc;            /* Checksum of the header       */
        unsigned long           ip_src;         /* Source IP address            */
        unsigned long           ip_dest;        /* Destination IP address       */
}__attribute__ ((packed));
//#define IP_HEADER_SIZE          (20)

//UDP header
struct udp_header{
	unsigned short          port_src;       /* Source Port                  */
	unsigned short	        port_dest;	/* Destination Port             */
        unsigned short	        tlen;	        /* Total length of the datagram */
        unsigned short	        crc;	        /* Checksum of entire datagram  */
}__attribute__ ((packed));
//#define UDP_HEADER_SIZE         (8)

//tftp packet 
struct tftp_package
{
    unsigned short opcode;                      /* packet type */
    union
    {
        unsigned short blocknum;                /* block # */
        unsigned short errcode;                 /* error code */
    } u;
    unsigned char  data[0];                         /* data or error string */
} __attribute__ ((__packed__));

#define PROTO_TCP       0x06
#define PROTO_UDP       0x11

#define PROTO_IP	0x0800		        /* IP protocol			*/
#define PROTO_ARP	0x0806		        /* IP ARP protocol		*/
#define PROTO_RARP	0x8035		        /* IP ARP protocol		*/

#define ARP_REQUEST     0x01            //1 for request
#define ARP_REPLY       0x02            //2 for reply
#define RARP_REQUEST    0x03            //3 for RARP request
#define RARP_REPLY      0x04            //4 for RARP reply.

#define TFTP_DATASIZE         512             /* data segment size */

#define TFTP_RRQ     0x01                              /* read request */
#define TFTP_WRQ     0x02                              /* write request */
#define TFTP_DATA    0x03                              /* data packet */
#define TFTP_ACK     0x04                              /* acknowledgement */
#define TFTP_ERROR   0x05                              /* error code */

// Tftp Error Code
#define EUNDEF          0               /* not defined */
#define ENOTFOUND       1               /* file not found */
#define EACCESS         2               /* access violation */
#define ENOSPACE        3               /* disk full or allocation exceeded */
#define EBADOP          4               /* illegal TFTP operation */
#define EBADID          5               /* unknown transfer ID */
#define EEXISTS         6               /* file already exists */
#define ENOUSER         7               /* no such user */

/********************** Network Configurations **********************************/
extern unsigned char	NetOurEther[6];	        /* Our ethernet address		*/
extern unsigned char	NetServerEther[6];      /* Boot server enet address	*/
extern unsigned char	NetBcastAddr[6];        /* Ethernet bcast address	*/

extern unsigned long	NetOurIP;		/* Our IP addr 192.168.1.252	*/
extern unsigned long	NetServerIP;            /* Server IP   192.168.1.249	*/

extern unsigned char    TxBuf[1024];
extern unsigned char    RxBuf[1024];

//把2字节的Little Endian转换为Big Endian
#define htons(x) (unsigned short)( \
((((unsigned short)(x) & (unsigned short)0x00FF)) << 8) | \
((((unsigned short)(x) & (unsigned short)0xFF00)) >> 8) )

//把4字节的Little Endian转换为Big Endian
#define htonl(x) (unsigned long)( \
((((unsigned long)(x) & (unsigned long)0x000000FF)) << 24) | \
((((unsigned long)(x) & (unsigned long)0x0000FF00)) <<  8) | \
((((unsigned long)(x) & (unsigned long)0x00FF0000)) >>  8) | \
((((unsigned long)(x) & (unsigned long)0xFF000000)) >> 24) )

#define ntohs htons
#define ntohl htohl

#endif

