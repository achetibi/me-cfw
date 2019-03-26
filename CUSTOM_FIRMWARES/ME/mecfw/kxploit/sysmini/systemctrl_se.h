#ifndef __SCTRLLIBRARY_SE_H__
#define __SCTRLLIBRARY_SE_H__

enum SEUmdModes
{
	MODE_UMD = 0,
	MODE_OE_LEGACY = 1,
	MODE_MARCH33 = 2,
	MODE_NP9660 = 3,
};

enum MEUmdModes
{
	ME_MODE_UMD = 0,
	ME_MODE_OE_LEGACY = 1,
	ME_MODE_PIL = 2,
	ME_MODE_NP9660 = 3,
};

typedef struct
{
	int magic; /* 0x47434553 *///ABEC
	int hidecorrupt;//abf0
	int	skiplogo;//
	int umdactivatedplaincheck;
	int gamekernel150;
	int executebootbin;//datac00
	int startupprog;
	int umdmode;
	int useisofsonumdinserted;
	int	vshcpuspeed; //dataAC10
	int	vshbusspeed; 
	int	umdisocpuspeed; 
	int	umdisobusspeed; 
	int fakeregion;//dataAC20
	int freeumdregion;
	int	hardresetHB; //data
	int usbdevice;
	int novshmenu;//dataAC30
	int usbcharge;
	int netupdate;
	int hidepng;
	int plugvsh;//
	int pluggame;
	int plugpop;
	int versiontxt;
	int fastms;//
	int uncc;
	int unccc;
	int nuc;
} SEConfig;


void sctrlSESetUmdFile(char *umd);	
char *sctrlSEGetUmdFile(void);	

//SystemCtrlForKernel_2F157BAF
void sctrlSEApplyConfig(SEConfig *config);	

void sctrlSESetBootConfFileIndex(int type);

/**
 * Gets the SE/OE/M33 version
 *
 * @returns the SE version
*/
int sctrlSEGetVersion();

/**
 * Gets the SE configuration
 *
 * @param config - pointer to a SEConfig structure that receives the SE configuration
 * @returns 0 on success
*/
int sctrlSEGetConfig(SEConfig *config);

/**
 * Sets the SE configuration
 *
 * @param config - pointer to a SEConfig structure that has the SE configuration to set
 * @returns 0 on success
*/
int sctrlSESetConfig(SEConfig *config);

/**
 * Initiates the emulation of a disc from an ISO9660/CSO file.
 *
 * @param file - The path of the 
 * @param noumd - Wether use noumd or not
 * @param isofs - Wether use the custom SE isofs driver or not
 * 
 * @returns 0 on success
 *
 * @Note - When setting noumd to 1, isofs should also be set to 1,
 * otherwise the umd would be still required.
 *
 * @Note 2 - The function doesn't check if the file is valid or even if it exists
 * and it may return success on those cases
 *
 * @Note 3 - This function is not available in SE for devhook
 * @Example:
 *
 * SEConfig config;
 *
 * sctrlSEGetConfig(&config);
 *
 * if (config.usenoumd)
 * {
 *		sctrlSEMountUmdFromFile("ms0:/ISO/mydisc.iso", 1, 1);
 * }
 * else
 * {
 *		sctrlSEMountUmdFromFile("ms0:/ISO/mydisc.iso", 0, config.useisofsonumdinserted);
 * }
*/
int sctrlSEMountUmdFromFile(char *file, int noumd, int isofs);

/**
 * Umounts an iso.
 *
 * @returns 0 on success
*/
int sctrlSEUmountUmd(void);

/**
 * Forces the umd disc out state
 *
 * @param out - non-zero for disc out, 0 otherwise
 *
*/
void sctrlSESetDiscOut(int out);

/**
 * Sets the disctype.
 *
 * @param type - the disctype (0x10=game, 0x20=video, 0x40=audio)
*/
void sctrlSESetDiscType(int type);

#endif
