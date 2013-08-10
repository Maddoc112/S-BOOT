/*
 * =====================================================================================
 *
 *       Filename:  tftp.c
 *
 *    Description:  tftp client application
 *
 *        Version:  1.0
 *        Created:  2009年05月12日 18时28分35秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Li Hongwang (mn), hoakee@gmail.com
 *        Company:  University of Science and Technology of China
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <string.h>
#include <net.h>
//#include <configs.h>
#include <sboot.h>

// Global Pointer to tftp Packet
struct tftp_package *pGtftp = NULL;
unsigned int iGLen = 0;
unsigned short TftpServerPort = 0;

static void tftp_send_request(unsigned char *buf, const char *filename)
{
    int len = 0;
    buf[0] = 0x00;
    buf[1] = 0x01;
    len += 2;
    sprintf(&buf[len], "%s", filename);
    len += strlen(filename);
    buf[len] = '\0';
    len += 1;
    sprintf(&buf[len], "%s", "octet");
    len += strlen("octet");
    buf[len] = '\0';
    len += 1;
    udp_send(buf, len, 69);
}

static void tftp_send_ack(unsigned char *buf, unsigned short blocknum)
{
    struct tftp_package *A = (struct tftp_package *)buf;
    A->opcode = htons(0x04);
    A->u.blocknum = htons(blocknum);
    udp_send(buf, sizeof(struct tftp_package), TftpServerPort);
}

void tftp_handle( unsigned char *buf, unsigned int len, unsigned short port )
{
    //int i;
    pGtftp = (struct tftp_package *)buf; 
    iGLen = len; 
    TftpServerPort = port;

//    printf("\n\r Get tftp Packet:\n\r");
   /* for(i=0; i<len; i++)
    {
        printf("%02X ", buf[i]);
    }
    */
    /* struct tftp_package *p;
    p = (struct tftp_package *)buf;

    switch ( ntohs(p->opcode) )
    {
        case DATA:
            
            break;
        case ERROR:
            break;
        default:
            break;
    }
*/
}

int tftp_download(unsigned char *addr, const char *filename)
{  
    int i=0;
    unsigned short curblock = 1;

    tftp_send_request( &TxBuf[256], filename );

    msdelay(100);

    while (1)
    {
       // len = udp_receive( &buf ); //接收到的tftp数据包起始于buf,长度为len
        eth_rx();
      
        if( pGtftp == NULL )
            continue;
        
        if ( ntohs(pGtftp->opcode) == TFTP_DATA )
        {
  //          printf("\n\r TFTP Data Packet.\n");
            if (ntohs(pGtftp->u.blocknum) == curblock)
            {
                //addr -= (curblock - ntohs(pGtftp->u.blocknum)) * SEGSIZE;
                printf("\r Current Block Number = %d", curblock);
                for (i=0; i<iGLen-4; i++)
                {
                    *(addr++) = *(pGtftp->data+i);
                }
                tftp_send_ack( &TxBuf[256], curblock);
                
                if (iGLen < TFTP_DATASIZE+4)
                {
                    break;
                }
                curblock += 1;
            }
            else if (ntohs(pGtftp->u.blocknum) < curblock)
            {
                tftp_send_ack( &TxBuf[256], ntohs(pGtftp->u.blocknum)); 
            }
            else
            {
                printf("\n\rBlock Number Not Match.");
                printf("Block Number = %d, curblock = %d\n", ntohs(pGtftp->u.blocknum), curblock);
                
            }
        }
        else if ( ntohs(pGtftp->opcode) == TFTP_ERROR ) 
        {
            switch( ntohs(pGtftp->u.errcode) )
            {
                case ENOTFOUND:
                    printf(" Error: File NOT Found.\n\r");
                    break;
                case EBADOP:
                    printf(" Error: Bad TFTP Operation.\n\r");
                    break;
                case EBADID:
                    printf(" Error: Unknown Transfer ID.\n\r");
                    break;
                default:
                    printf(" Error: ..... \n\r");
                    break;
            } 
        }
        else if ( ntohs(pGtftp->opcode) == TFTP_RRQ ) 
        {
            printf(" Get TFTP RRQ Packet.\n\r");
        }
        else if ( ntohs(pGtftp->opcode) == TFTP_WRQ ) 
        {
            printf(" Get TFTP WRQ Packet.\n\r");
        }
        else if ( ntohs(pGtftp->opcode) == TFTP_ACK ) 
        {
            printf(" Get TFTP ACK Packet.\n\r");
        }
        else 
        {
            printf(" Unknown TFTP Packet.");
            printf(" Opcode = %d.\n\r", ntohs(pGtftp->opcode));
        }

        pGtftp = NULL;
        iGLen = 0;
    }
    
    printf("\n\rTransfer complete: %d Bytes.\n\r", (curblock-1)*TFTP_DATASIZE + iGLen-4 );
    
    return 0;

}

