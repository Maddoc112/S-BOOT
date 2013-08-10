/*
 * =====================================================================================
 *
 *       Filename:  net_simple.c
 *
 *    Description:  简化的net.c
 *
 *        Version:  1.0
 *        Created:  2008年11月25日 14时17分53秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Li Hongwang (mn), hoakee@gmail.com
 *        Company:  University of Science and Technology of China
 *
 * =====================================================================================
 */
/* ARP:
 *	Prerequisites:	- own ethernet address
 *			- own IP address
 *			- TFTP server IP address
 *	We want:	- TFTP server ethernet address
 *	Next step:	TFTP
 */
#include <stdio.h>
#include <string.h>
#include <configs.h>
#include <net.h>
#include <sboot.h>

/********************** Network Configurations **********************************/
unsigned char	NetOurEther[6] =	        /* Our ethernet address		*/
        {0x00, 0x09, 0x58, 0xD8, 0x11, 0x22};
unsigned char	NetServerEther[6] =	        /* Boot server enet address	*/
	{0x00, 0x14, 0x2A, 0xA5, 0x50, 0x97};
unsigned char	NetBcastAddr[6] =	        /* Ethernet bcast address	*/
	{0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

unsigned long	NetOurIP = 0xC0A801FC;		/* Our IP addr 192.168.1.252	*/
unsigned long	NetServerIP = 0xC0A801F9;       /* Server IP   192.168.1.249	*/

unsigned char TxBuf[1024];
unsigned char RxBuf[1024];


unsigned short checksum(unsigned char *ptr, int len)
{
    unsigned long sum = 0;
    unsigned short *p = (unsigned short *)ptr;

    while (len > 1)
    {
        sum += *p++;
        len -= 2;
    }
    
    if(len == 1)
        sum += *(unsigned char *)p;
    
    while(sum>>16)
        sum = (sum&0xffff) + (sum>>16);
    
    return (unsigned short)((~sum)&0xffff);
}
//===================================================================================

//第一个参数为要建立帧头的位置，第二个参数为目的MAC地址，第三个参数为协议类型
void mac_send( unsigned char *buf, unsigned int len, unsigned short proto )
{
    buf -= sizeof(struct mac_header);
    len += sizeof(struct mac_header);
    
    struct mac_header *p = (struct mac_header*)(buf);
    
    memcpy (p->dest, NetServerEther, 6);
    memcpy (p->src, NetOurEther, 6);
    
    p->proto = htons(proto);
     
    eth_send( buf, len );
}

void ip_send( unsigned char *buf, unsigned int len )
{
    buf -= sizeof(struct ip_header);
    len += sizeof(struct ip_header);
    
    struct ip_header *p = (struct ip_header*)(buf);
    
    p->ver_ihl = 0x45;                          // 1 Byte
    p->tos = 0x00;                              // 1 Byte
    p->tlen = htons(len);       // 2 Byte
    p->identification = htons(0x00);            // 2 Byte
    p->flags_fo = htons(0x4000);                // 2 Byte
    p->ttl = 0xFF;                              // 1 Byte
    p->proto = 17;                          // 1 Byte, 17 for UDP
    p->ip_src  = htonl(NetOurIP);               // 4 Byte
    p->ip_dest = htonl(NetServerIP);  // 4 Byte
    p->crc = 0x0;                                 // 2 Byte, To be
    p->crc = checksum( buf, sizeof(struct ip_header) );
    
    //printf("\rip_transmit: len=%d\n", len);

    mac_send( buf, len, PROTO_IP );
}

/*
 ************************************************************************************
 * 传输层UDP
 ************************************************************************************
 */

void udp_send( unsigned char *buf, unsigned int len, unsigned short port )
{
    buf -= sizeof(struct udp_header);
    len += sizeof(struct udp_header);

    struct udp_header *P = (struct udp_header*)(buf);
    P->port_src  = htons(0x8DA4);    // 2 Byte
    P->port_dest = htons(port);   // 2 Byte
    P->tlen = htons(len);       // 2 Byte 
    P->crc  = 0x00;     // Do Not Checksum, 2 Byte 

    ip_send( buf, len );
}

static int arp_handle( unsigned char *buf, unsigned int len )
{
    struct arp_header *pRx, *pTx;
    pRx = (struct arp_header *)(buf);
    pTx = (struct arp_header *)&TxBuf[256];

/*    if (pRx->ar_pro != PROTO_IP)
    {
        printf("\r\tARP Packet ar_pro Error !\n");
        return -1;
    }
*/
    switch (htons(pRx->ar_op))
    {
        case ARP_REQUEST:
            if (pRx->ar_tpa == htonl(NetOurIP))
            {
    //            printf("\n\rGot ARP request, return our MAC\n");
                pTx->ar_hrd = htons(0x01);
                pTx->ar_pro = htons(PROTO_IP);
                pTx->ar_hln = 0x06;
                pTx->ar_pln = 0x04;
                pTx->ar_op = htons(ARP_REPLY); 
                memcpy(pTx->ar_sha, NetOurEther, 6);
                pTx->ar_spa = htonl(NetOurIP);
                memcpy (pTx->ar_tha, pRx->ar_sha, 6);		// source ET addr
                pTx->ar_tpa = pRx->ar_spa;
                mac_send( (unsigned char*)pTx, sizeof(struct arp_header), PROTO_ARP);
            }
            break;
        case ARP_REPLY:
            printf("\n\rGot ARP reply\n");
            break;
        default:
            printf("\n\r ar_op Not Support.\n");
            break;
    }

    return 0;
}

static int udp_handle( unsigned char *buf, unsigned int len )
{
    struct udp_header *p;
    p = (struct udp_header *)buf;
    
    //printf("\n\r UDP Packet, len = %d\n", len);
    //printf("\r UDP Header, tlen= %d\n", p->tlen);
    
    buf += sizeof(struct udp_header);
    len -= sizeof(struct udp_header);
   
    tftp_handle( buf, len, htons(p->port_src) );
    //pGtftp = (struct tftp_package *)buf;

    /* P->port_src  = htons(0);    // 2 Byte
    P->port_dest = htons(69);   // 2 Byte
    P->tlen = htons(len);       // 2 Byte 
    P->crc  = 0x00;     // Do Not Checksum, 2 Byte 
*/
    return 0;
}


static int ip_handle( unsigned char *buf, unsigned int len )
{
    struct ip_header *p;
    p = (struct ip_header *)buf;

    if ( sizeof(struct ip_header) != ((p->ver_ihl & 0x0F)<<2))
    {
        printf("\r\tpIpHead len not match!\n");
        return -1;
    }

    //printf("\n\r Get IP Packet.\n\r");
    
    if ( p->ip_dest != htonl(NetOurIP) )
    {
        //printf("\n\r Not Our IP Packet.\n\r");
        return -1;
    }
   
    switch ( p->proto )
    {
        case PROTO_UDP:
            buf += sizeof(struct ip_header);
            len -= sizeof(struct ip_header);
            udp_handle( buf, len );
            break;
        case PROTO_TCP:
            break;
        default:
            break;
    }

    return 0;
}


void net_receive( unsigned char *buf, unsigned int len )
{
    struct mac_header *p = (struct mac_header*)(buf);
   
    switch ( htons(p->proto) )
    {
        case PROTO_ARP:
            buf += sizeof(struct mac_header);
            len -= sizeof(struct mac_header);
            arp_handle( buf, len );
            break;
        case PROTO_IP:
            buf += sizeof(struct mac_header);
            len -= sizeof(struct mac_header);
            ip_handle( buf, len ); 
            
            //IpLayerDeliver( pSockBuff );
            break;
        default:
            printf("\r\tFrame type error (=0x%04X)!\n", p->proto);
            break;
    }
}


