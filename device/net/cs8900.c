#include <stdio.h>
#include <cs8900.h>
#include <net.h>
#include <sboot.h>

/* the PWM TImer 4 uses a counter of 15625 for 10 ms, so we need */
/* it to wrap 100 times (total 1562500) to get 1 sec. */
//#define CONFIG_SYS_HZ  1562500

/* packet page register access functions */

static unsigned short get_reg_init_bus (int regno)
{
	/* force 16 bit busmode */
	volatile unsigned char c;

	c = CS8900_BUS16_0;
	c = CS8900_BUS16_1;
	c = CS8900_BUS16_0;
	c = CS8900_BUS16_1;
	c = CS8900_BUS16_0;

	CS8900_PPTR = regno;
	return CS8900_PDATA;
}

static unsigned short get_reg (int regno)
{
	CS8900_PPTR = regno;
	return CS8900_PDATA;
}


static void put_reg (int regno, unsigned short val)
{
	CS8900_PPTR = regno;
	CS8900_PDATA = val;
}

static void eth_reset (void)
{
	int i;

	/* reset NIC */
	put_reg (PP_SelfCTL, get_reg (PP_SelfCTL) | PP_SelfCTL_Reset);

	/* wait for 200ms */
	msdelay(200);
        /* Wait until the chip is reset */
	//tmo = get_timer (0) + 1 * CONFIG_SYS_HZ;
	for (i=0; i<5; i++)
        {
            if( ((get_reg_init_bus (PP_SelfSTAT)) & PP_SelfSTAT_InitD) == 0 )
                msdelay(200);
            else
                break;
        }
}

static void eth_reginit (void)
{
	/* receive only error free packets addressed to this card */
	put_reg (PP_RxCTL, PP_RxCTL_IA | PP_RxCTL_Broadcast | PP_RxCTL_RxOK);
	/* do not generate any interrupts on receive operations */
	put_reg (PP_RxCFG, 0);
	/* do not generate any interrupts on transmit operations */
	put_reg (PP_TxCFG, 0);
	/* do not generate any interrupts on buffer operations */
	put_reg (PP_BufCFG, 0);
	/* enable transmitter/receiver mode */
	put_reg (PP_LineCTL, PP_LineCTL_Rx | PP_LineCTL_Tx);
}

//int eth_init (bd_t * bd)
int eth_init ()
{
	/* verify chip id */
	if (get_reg_init_bus (PP_ChipID) != 0x630e) {
		printf ("CS8900 Ethernet chip not found?!\n");
		return 0;
	}

	eth_reset ();
	/* set the ethernet address */
	//put_reg (PP_IA + 0, 0x09 | 0x00 << 8);
	//put_reg (PP_IA + 2, 0xD8 | 0x58 << 8);
	//put_reg (PP_IA + 4, 0x22 | 0x11 << 8);

	put_reg (PP_IA + 0, 0x00 | 0x09 << 8);
	put_reg (PP_IA + 2, 0x58 | 0xD8 << 8);
	put_reg (PP_IA + 4, 0x11 | 0x22 << 8);
	eth_reginit ();
	return 0;
}

void eth_halt (void)
{
	/* disable transmitter/receiver mode */
	put_reg (PP_LineCTL, 0);

	/* "shutdown" to show ChipID or kernel wouldn't find he cs8900 ... */
	get_reg_init_bus (PP_ChipID);
}

/* Send a data block via Ethernet. */
int eth_send (volatile void *packet, int length)
{
	volatile unsigned short *addr;
	//int tmo;
	unsigned short s;

retry:
	/* initiate a transmit sequence */
	CS8900_TxCMD = PP_TxCmd_TxStart_Full;
	CS8900_TxLEN = length;

	/* Test to see if the chip has allocated memory for the packet */
	if ((get_reg (PP_BusSTAT) & PP_BusSTAT_TxRDY) == 0) {
		/* Oops... this should not happen! */
		printf ("cs: unable to send packet; retrying...\n");
	//	for (tmo = get_timer (0) + 5 * CONFIG_SYS_HZ; get_timer (0) < tmo;)
			/*NOP*/;
		eth_reset ();
		eth_reginit ();
		goto retry;
	}

	/* Write the contents of the packet */
	/* assume even number of bytes */
	for (addr = packet; length > 0; length -= 2)
        {
            //printf("%4X ", *addr);
            CS8900_RTDATA = *addr++;    
        }
	/* wait for transfer to succeed */
	//tmo = get_timer (0) + 5 * CONFIG_SYS_HZ;
	while ((s = get_reg (PP_TER) & ~0x1F) == 0) {
		//if (get_timer (0) >= tmo)
		//	break;
	}

	/* nothing */ ;
	if ((s & (PP_TER_CRS | PP_TER_TxOK)) != PP_TER_TxOK) {
		printf ("\ntransmission error %#x\n", s);
	}

	return 0;
}

/* Get a data block via Ethernet */
int eth_rx (void)
{
	int i;
	unsigned short rxlen;
	unsigned short *addr;
	unsigned short status;

	status = get_reg (PP_RER);

	if ((status & PP_RER_RxOK) == 0)
		return 0;

	status = CS8900_RTDATA;		/* stat */
	rxlen = CS8900_RTDATA;		/* len */

#ifdef DEBUG
	if (rxlen > PKTSIZE_ALIGN + PKTALIGN)
		printf ("packet too big!\n");
#endif
	for (addr = (unsigned short *) &RxBuf[0], i = rxlen >> 1; i > 0;
		 i--)
		*addr++ = CS8900_RTDATA;
	if (rxlen & 1)
		*addr++ = CS8900_RTDATA;

	/* Pass the packet up to the protocol layers. */
	//NetReceive (NetRxPackets[0], rxlen);
        net_receive( &RxBuf[0], rxlen );

	return rxlen;
}

