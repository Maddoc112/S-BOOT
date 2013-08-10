
#include <stdio.h>
#include <configs.h>
#include <sboot.h>

int main(int argc,char **argv)
{
    int Num;
    init_uart();
    
    eth_init();

    while(1)
    {
        printf("\n ******************************* \n\r");
        printf("\n **** Our Simple Bootloader **** \n\r");
        printf("\n ******************************* \n\r");
        printf("1: Boot Linux from Nand Flash.\n\r");
        printf("2: Download Kernel from Kermit Serial.\n\r");
        printf("3: Download Kernel from TFTP Server.\n\r");
        printf("4: Boot Linux form RAM.\n\r");
        printf("5: Test OK2440 Board Hardware.\n\r");
        
        printf("\nSelect Number: ");
        scanf("%d", &Num);
        
        switch (Num)
        {
            case 1:
                printf(" Now Booting Linux form Nand ....\n\r"); 
                boot_linux();                
                break;
            case 2:
                boot_linux_serial();                
                break;
            case 3:
                tftp_download((unsigned char*)DRAM_KERNEL_START, "zImage");
                break;
            case 4:
                boot_linux_ram();                
                break;
            case 5:
                test_hardware();
                break;
            default:
                printf(" Wrong Number! Select Again.\n\r"); 
                break;
        } 
    }
}

int test_hardware()
{
    int Num;
    
    while(1)
    {
        printf("\n=== Test ARM OK2440 Development Board ===\n\r");
        printf("1: LED Test\n\r");
        printf("2: BEEP Test\n\r");
        printf("3: ADC Test\n\r");
        printf("4: LCD Test\n\r");
        printf("5: Touch Screen Test\n\r");
        printf("6: RTC Test\n\r");
        printf("7: KEY Test\n\r");
        printf("8: AC97 Audio Test\n\r");
        printf("9: Timer and Clock Test\n\r");
        printf("0: Quit \n\r");
        
        printf("\nSelect Number: ");
        scanf("%d", &Num);
        
        if (Num == 0) break;

        switch (Num)
        {
            case 1:
            case 2:
            case 3:
            case 5:
                printf(" Sorry, under development.\n\r"); 
                break;
            case 4:
       //         lcd_test();
                break;
            case 6:
      //        rtc_test();
                break;
            case 7:
            case 8:
            case 9:
                printf(" Sorry, under development.\n\r"); 
                break; 
            default:
                printf(" Wrong Number! Select Again.\n\r"); 
                break; 
        }
    }
    return 0;
}

