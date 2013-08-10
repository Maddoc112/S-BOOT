/*
 * =====================================================================================
 *
 *       Filename:  apps.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2009年04月26日 21时10分04秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Li Hongwang (mn), hoakee@gmail.com
 *        Company:  University of Science and Technology of China
 *
 * =====================================================================================
 */

#ifndef SBOOT_H
#define SBOOT_H

/********** Linux Boot Functions ************/
int  boot_linux(void);
int  test_hardware(void);
int boot_linux_serial();
int boot_linux_ram();

/********** Network Related Functions ************/
int tftp_download(unsigned char *addr, const char *filename);
void udp_send( unsigned char *buf, unsigned int len, unsigned short port );
void net_receive( unsigned char *buf, unsigned int len );
void tftp_handle( unsigned char *buf, unsigned int len, unsigned short port );
int eth_rx (void);


/********** Hardware Test Functions ************/
// Serial
void init_uart();
void putc(unsigned char c);
unsigned char getc(void);
int GtSerialLoad(void *pLoadAddr);

// nand.c
void nand_init(void);
void nand_read(unsigned char *buf, unsigned long start_addr, int size);

// timer.c
int timer_init (void);
void msdelay(unsigned long msec);

//cs8900.c
int eth_init ();
int eth_send (volatile void *packet, int length);

#endif
