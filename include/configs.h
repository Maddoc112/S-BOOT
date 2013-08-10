/*
 * =====================================================================================
 *
 *       Filename:  configs.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2009年06月02日 16时01分03秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Li Hongwang (mn), hoakee@gmail.com
 *        Company:  University of Science and Technology of China
 *
 * =====================================================================================
 */

#ifndef CONFIGS_H
#define CONFIGS_H

/********** RAM and Nand Space Allocation ****************/
#define DRAM_TOTAL_SIZE         0x04000000
#define DRAM_ADDR_START         0x30000000
#define DRAM_SBOOT_START        0x33000000
#define DRAM_TAGS_START         0x30000100
#define DRAM_KERNEL_START       0x33800000

#define NAND_KERNEL_START       0x00230000
#define NAND_KERNEL_SZIE        0x001D0000

/*************** Linux Kernel Boot Parameters ***********/
#define S3C2440_MATHINE_TYPE    362

#endif

