#define sceSysregGetFuseId sceSysreg_driver_4F46EEDE
#define sceSysregGetFuseConfig sceSysreg_driver_8F4F4E96
#define sceSysregGetTachyonVersion sceSysreg_driver_E2A5D1EE
#define sceSysconGetBaryonVersion sceSyscon_driver_7EC5A957
#define sceSysconGetPommelVersion sceSyscon_driver_E7E87741

u32 sceSysconCmdExec(void *param, int unk);
u64 sceSysregGetFuseId(void);
u32 sceSysregGetFuseConfig(void);
u32 sceSysregGetTachyonVersion(void);
u32 sceSysconGetBaryonVersion(u32 *val);
u32 sceSysconGetPommelVersion(u32 *val);
int sceNandSetScramble(u32 magic);
int sceSysregKirkBusClockEnable(void);
int sceSysregAtaBusClockEnable(void);
void *sceUmdManGetUmdDrive(int driveNum);
int sceUmdExecInquiryCmd(void *drive, u8 *param, u8 *buf);

typedef struct
{
	unsigned long		maxclusters;
	unsigned long		freeclusters;
	int					unk1;
	unsigned int		sectorsize;
	u64					sectorcount;
	
} SystemDevCtl;

typedef struct
{
	SystemDevCtl *devinf;
	
} SystemDevCommand;

int pspReadSerial(u16* serial);
int pspWriteSerial(u16* serial);
int pspReadEEPROM(int address, u16* pdata);
int pspWriteEEPROM(int address, u16* pdata);
u32 pspGetBatterySerial();

int pspIdStorageLookup(u16 key, u32 offset, void *buf, u32 len);
int pspIdStorageReadLeaf(u16 key, void *buf);
int pspIdStorageWriteLeaf(u16 key, void *buf);
int pspIdStorageCreateLeaf(u16 key);
int pspIdStorageDeleteLeaf(u16 key);

u32 pspGetTachyonVersion();
u32 pspGetBaryonVersion();
u32 pspGetPommelVersion();
u64 pspGetFuseId();
u32 pspGetFuseConfig();
u32 pspGetKirkVersion();
u32 pspGetSpockVersion();
int pspKernelGetModel();

int pspNandGetPageSize(void);
int pspNandGetPagesPerBlock(void);
int pspNandGetTotalBlocks(void);
int pspNandIsBadBlock(u32 ppn);
int pspNandLock(int writeflag);
int pspNandReadBlockWithRetry(u32 ppn, void *buf, void *buf2);
int pspNandReadId(void *buf, SceSize size);
int pspNandReadPages(u32 ppn, void *buf, void *buf2, u32 count);
int pspNandReadStatus(void);
int pspNandReset(int flag);
int pspNandSetWriteProtect(int protectFlag);
void pspNandUnlock(void);
int pspNandSetScramble(u32 magic);
u32 pspNandGetScramble(void);
int pspGetHardwareInfo();
int pspGetRegionInfo();
char *pspGetShippedFirmware(char *buf);
u8 *pspGetMACAddress(u8 *buf);
u32 pspGetMsTotalSize();
u32 pspGetMsFreeSpace();
int pspUmdExecInquiryCmd(void *drive, u8 *param, u8 *buf);
void *pspUmdManGetUmdDrive(int driveNum);
char *pspGetUMDFirmware(char *buf);
int pspGetKeyPress(int wait, int milisecs);

int module_start(SceSize args, void *argp);
int module_stop();
