#ifndef __FRIENDLYELEC_BOARDTYPE_H__
#define __FRIENDLYELEC_BOARDTYPE_H__

#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <stdlib.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <sys/ioctl.h> 
#include <fcntl.h>
#include <linux/fs.h> 
#include <errno.h> 
#include <stdio.h>
#include <sys/utsname.h>

typedef struct {
	char kernelHardware[255];
	int kernelRevision;
	int boardTypeId;
	char boardDisplayName[255];
	char allwinnerBoardID[255];
} BoardHardwareInfo;

#define S3C6410_COMMON (6410)
#define S5PV210_COMMON (210)
#define S5P4412_COMMON (4412)
#define S5P4418_BASE (4418)
#define NanoPi2 (S5P4418_BASE+0)
#define NanoPC_T2 (S5P4418_BASE+1)
#define NanoPi_S2 (S5P4418_BASE+2)
#define Smart4418 (S5P4418_BASE+3)
#define NanoPi2_Fire (S5P4418_BASE+4)
#define NanoPi_M2 (S5P4418_BASE+5)
#define NanoPi_M2A (S5P4418_BASE+7)
#define Smart4418SDK (S5P4418_BASE+0x103)
#define S5P4418_MAX Smart4418SDK

//s5p6818
#define S5P6818_BASE (6818)
#define NanoPC_T3 (S5P6818_BASE+1)
#define NanoPi_M3B (S5P6818_BASE+2)
#define Smart6818 (S5P6818_BASE+3)
#define NanoPC_T3T (S5P6818_BASE+4)
#define NanoPi_Fire3 (S5P6818_BASE+5)
#define NanoPi_M3 (S5P6818_BASE+7)
#define S5P6818_MAX NanoPi_M3

//s3c2451
#define S3C2451_BASE (2451)
#define S3C2451_COMMON (S3C2451_BASE+0)


//allwinner
#define ALLWINNER_BASE (7000)
#define NanoPi_M1 (ALLWINNER_BASE+1)
#define NanoPi_NEO (ALLWINNER_BASE+2)
#define NanoPi_NEO_Air (ALLWINNER_BASE+3)
#define NanoPi_M1_Plus (ALLWINNER_BASE+4)
#define NanoPi_A64 (ALLWINNER_BASE+5)
#define NanoPi_NEO2 (ALLWINNER_BASE+6)
#define NanoPi_M1_Plus2 (ALLWINNER_BASE+7)
#define NanoPi_NEO_Plus2 (ALLWINNER_BASE+8)
#define NanoPi_NEO_Core (ALLWINNER_BASE+9)
#define NanoPi_Duo (ALLWINNER_BASE+10)
#define NanoPi_NEO_Core2 (ALLWINNER_BASE+11)
#define NanoPi_K1 (ALLWINNER_BASE+12)
#define NanoPi_K1_Plus (ALLWINNER_BASE+13)
#define NanoPi_Hero (ALLWINNER_BASE+14)
#define NanoPi_Duo2 (ALLWINNER_BASE+15)
#define NanoPi_R1 (ALLWINNER_BASE+16)
#define ALLWINNER_MAX NanoPi_R1

//amlogic
#define AMLOGIC_BASE (8000)
#define NanoPi_K2 (AMLOGIC_BASE+1)

//rk3399
#define RK3399_BASE (9000)
#define NanoPC_T4 (RK3399_BASE+1)
#define NanoPi_M4 (RK3399_BASE+2)
#define NanoPi_NEO4 (RK3399_BASE+3)

extern int getBoardType(BoardHardwareInfo** retBoardInfo);

#endif
