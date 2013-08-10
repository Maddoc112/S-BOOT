
.equ UndefStack,        0x32FFF000
.equ AbortStack,        0x32FFE000
.equ IRQStack,          0x32FFD000
.equ FIQStack,          0x32FFC000
.equ UserStack,         0x32FFB000
.equ SVCStack,          0x32FFA000

 
.text
.global _start
_start:
    b   Reset                   @ 0x00: 发生复位异常时从地址零处开始运行
    b   HandleUndef             @ 0x04: 未定义指令中止模式的向量地址
    b   HandleSWI               @ 0x08: 管理模式的向量地址，通过SWI指令进入此模式
    b   HandlePrefetchAbort     @ 0x0C: 指令预取终止导致的异常的向量地址
    b   HandleDataAbort         @ 0x10: 数据访问终止导致的异常的向量地址
    b   HandleNotUsed           @ 0x14: 保留
    b   HandleIRQ               @ 0x18: 中断模式的向量地址
    b   HandleFIQ               @ 0x1C: 快中断模式的向量地址
/*********************************************************************/

Reset:
        mrs r0,cpsr                 @set cpu to SVC32 mode
	bic r0,r0,#0x1F
	orr r0,r0,#0xD3
	msr cpsr,r0                 @cpsr=11x10011, IRQ/FIQ disabled        

        ldr r0, =0x53000000
        mov r1, #0x0
        str r1, [r0]                @disable watch dog

        ldr r0, =0x4C000014         @CLKDIVN register
        mov r1, #0x05               @FCLK:HCLK:PCLK = 1:4:8
        str r1, [r0]

        mrc p15,0,r0,c1,c0,0        @if HDIVN Not 0, must asynchronous bus mode
        orr r0,r0,#0xC0000000       @see S3C2440A manual P7-9
        mcr p15,0,r0,c1,c0,0 

        ldr r0, =0x4C000004         @MPLLCON register
        ldr r1, =0x0005C011         @((92<<12)|(1<<4)|(1))
        str r1, [r0]                @FCLK is 400 MHz !

        @ SDRAM Init
        mov  r1, #0x48000000        @MEM_CTL_BASE
        adrl r2, mem_cfg_val
        add  r3, r1, #52
1:
        ldr r4, [r2], #4            @ 读取设置值，并让r2加4
        str r4, [r1], #4            @ 将此值写入寄存器，并让r1加4
        cmp r1, r3                  @ 判断是否设置完所有13个寄存器
        bne 1b                      @ 若没有写成，继续

/********* Setup Stack ************/
/*        mrs r0, cpsr
        bic r0, r0, #0x1F
        orr r0, r0, #0xDB
        msr cpsr, r0
        ldr sp, =UndefStack         @设置Undef模式下的堆栈指针
    
        mrs r0, cpsr
        bic r0, r0, #0x1F
        orr r0, r0, #0xD7
        msr cpsr, r0
        ldr sp, =AbortStack         @设置Abort模式下的堆栈指针

        mrs r0, cpsr
        bic r0, r0, #0x1F
        orr r0, r0, #0xD2
        msr cpsr, r0
        ldr sp, =IRQStack           @设置IRQ模式下的堆栈指针
        
        mrs r0, cpsr
        bic r0, r0, #0x1F
        orr r0, r0, #0xD1
        msr cpsr, r0
        ldr sp, =FIQStack           @设置FIQ模式下的堆栈指针
    
        mrs r0, cpsr
        bic r0, r0, #0x1F
        orr r0, r0, #0xD0
        msr cpsr, r0
        ldr sp, =UserStack          @设置User模式下的堆栈指针

        mrs r0,cpsr                 @set cpu to SVC32 mode
	bic r0,r0,#0x1F
	orr r0,r0,#0xD3
	msr cpsr,r0                 @cpsr=11x10011, IRQ/FIQ disabled        
  */      ldr sp, =SVCStack           @设置SVC模式下的栈指针 
        
        @ disable MMU, close CPU ICache and DCache
        @ They are disabled by default 

       @ ldr sp, =4096               @设置堆栈 
        bl  nand_init               @初始化NAND Flash
                                    @将NAND Flash中地址4096开始的1024字节代码(main.c编译得到)复制到SDRAM中
                                    @nand_read_ll函数需要3个参数：
        ldr r0, =0x33000000         @1. 目标地址=0x30000000，这是SDRAM的起始地址
        mov r1, #0               @2. 源地址  =4096，连接的时候，stage2代码都存在NAND地址4096开始处
        mov r2, #102400             @3. 复制长度=1024(bytes)，according to size of stage2
        bl  nand_read               @调用C函数nand_read


        @ LED2
        ldr r0, =0x56000050         @ GPFCON register
        mov r1, #(1<<(5*2))         @ GPF5 controls LED5 
        str r1, [r0]
        ldr r0, =0x56000054         @ GPFDAT register
        mov r1, #(~(1<<5))             
        str r1, [r0]                @ Light LED2 
        
        ldr lr, =halt_loop          @设置返回地址
        ldr pc, =main               @b指令和bl指令只能前后跳转32M的范围，所以这里使用向pc赋值的方法进行跳转

halt_loop:
        b   halt_loop

.align 4
mem_cfg_val:
        @ 存储控制器13个寄存器的设置值
        .long   0x2201D110      @ BWSCON 设置BANK3位宽16,使能nWait,使能UB/LB
        .long   0x00000700      @ BANKCON0
        .long   0x00000700      @ BANKCON1
        .long   0x00000700      @ BANKCON2
        .long   0x00001F7C      @ BANKCON3  
        .long   0x00000700      @ BANKCON4
        .long   0x00000700      @ BANKCON5
        .long   0x00018005      @ BANKCON6
        .long   0x00018005      @ BANKCON7
        .long   0x008C04F4      @ REFRESH跟HCLK有关，见手册P5-18
        .long   0x000000B1      @ BANKSIZE
        .long   0x00000030      @ MRSRB6
        .long   0x00000030      @ MRSRB7



/************** Interrupt Handle ******************/

HandleUndef:
        b HandleUndef        
HandleSWI:
        b HandleSWI        
HandlePrefetchAbort:
        b HandlePrefetchAbort
HandleDataAbort:
        b HandleDataAbort 
HandleNotUsed:
        b HandleNotUsed        
HandleFIQ:
        b HandleFIQ

HandleIRQ:
    sub lr, lr, #4                  @ 计算返回地址
    stmdb   sp!,    { r0-r12,lr }   @ 保存使用到的寄存器
                                    @ 注意，此时的sp是中断模式的sp
                                    @ 初始值是上面设置的3072    
   @ ldr lr, =int_return             @ 设置调用ISR即EINT_Handle函数后的返回地址  
   @ ldr pc, =IRQ_Handle            @ 调用中断服务函数，在interrupt.c中
int_return:
    ldmia   sp!,    { r0-r12,pc }^  @ 中断返回, ^表示将spsr的值复制到cpsr
