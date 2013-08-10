//
#pragma once													//保证头文件只被编译一次

#define ATAG_NONE	0x00000000					//这个必须的，放在最后，而且只能有taghead

struct TagHeader {
	unsigned int nSize;
	unsigned int ulTag;
};

#define ATAG_CORE	0x54410001					//这个也是必须的，放在第一个

struct TagCore {
	unsigned int ulFlags;								//一般设为0
	unsigned int nPageSize;							//分页内存管理中每一页的大小，一般为4096字节
	unsigned int ulRootDev;							//系统启动的设备号，设为0即可，后面CMDLINE的内容会覆盖这个
};

#define ATAG_MEM	0x54410002					//描述系统内存的物理地址空间

struct TagMem32 {	
	unsigned int	nSize;								//系统内存的大小
	unsigned int	ulStart;							//系统内存的起始物理地址
};

#define ATAG_VIDEOTEXT	0x54410003

struct TagVideoText {
	unsigned char		bX;
	unsigned char		bY;
	unsigned short		usVideoPage;
	unsigned char		bVideoMode;
	unsigned char		bVideoCols;
	unsigned short		usVideoEgaBx;
	unsigned char		bVideoLines;
	unsigned char		bVideoIsVGA;
	unsigned short		usVideoPoints;
};

#define ATAG_RAMDISK	0x54410004

struct TagRamDisk {
	unsigned int dwFlags;
	unsigned int nSize;
	unsigned int nStart;
};
#define ATAG_INITRD	0x54410005

#define ATAG_INITRD2	0x54420005

struct TagInitrd {
	unsigned int ulStart;
	unsigned int nSize;
};

#define ATAG_SERIAL	0x54410006

struct TagSerialnr {
	unsigned int ulLow;
	unsigned int ulHigh;
};

#define ATAG_REVISION	0x54410007

struct TagRevision {
	unsigned int ulRev;
};

#define ATAG_VIDEOLFB	0x54410008

struct TagVideolfb {
	unsigned short		usLfbWidth;
	unsigned short		usLfbHeight;
	unsigned short		usLfbDepth;
	unsigned short		usLfbLineLength;
	unsigned int		ulLfbBase;
	unsigned int		nLfbSize;
	unsigned char		bRedSize;
	unsigned char		bRedPos;
	unsigned char		bGreenSize;
	unsigned char		bGreenPos;
	unsigned char		bBlueSize;
	unsigned char		bBluePos;
	unsigned char		bRsvdSize;
	unsigned char		bRsvdPos;
};

#define ATAG_CMDLINE	0x54410009												//命令行参数

struct TagCmdline {
	char	cCmdLine[1];																		//
};


#define ATAG_MEMCLK	0x41000402

struct TagMemClk {
	unsigned int ulFMemClk;
};

struct Atag {
	struct TagHeader stHdr;

	union {
		struct TagCore		stCore;
		struct TagMem32		stMem;
		struct TagVideoText	stVideoText;
		struct TagRamDisk	stRamDisk;
		struct TagInitrd	stInitRd;
		struct TagSerialnr	stSerialNr;
		struct TagRevision	stRevision;
		struct TagVideolfb	stVideoLfb;
		struct TagCmdline	stCmdLine;
	}u;
};


#define TAG_NEXT(t)	((struct Atag *)((unsigned int *)(t) + (t)->stHdr.nSize))
#define TAG_SIZE(type)	((sizeof(struct TagHeader) + sizeof(struct type)) >> 2)

#if 1
#define FOR_EACH_TAG(t, base) \
	for (t = base; t->stHdr.nSize; t = TAG_NEXT(t))
#endif


#define CONF_RAM_BANK_NUM 		1
#define DEFAULT_KCMDLINE_LEN		1024

