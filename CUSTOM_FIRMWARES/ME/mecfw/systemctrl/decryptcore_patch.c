#include <pspsdk.h>
#include <pspkernel.h>
#include <pspinit.h>
#include <psploadexec_kernel.h>
#include <psputilsforkernel.h>
#include <psppower.h>
#include <pspreg.h>
#include <pspopenpsid.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"


int (* ExtendDecryption)() = NULL;
int (* MemlmdSigcheck)() = NULL;
int (* MemlmdDecrypt)(u8 *buf, int size, int *ret, int u) = NULL;

//int (* memlmd_FD379991)() = NULL;
int (* memlmd_C071E9CA)() = NULL;


//if unofficial return 0
int MemlmdSigcheckPatched(void *buf,int size,int flag)
{
	PSP_Header *head=(PSP_Header *)buf;
	int i;

	if(head->signature == 0x5053507E)//PSP~
	{
#if _PSP_FW_VERSION == 660
		for(i=0;i<0x30;i++)
#else
		for(i=0;i<0x38;i++)
#endif		
		{
			if(head->scheck[i] != '\0')
			{	
				if(head->reserved2[0] !=0 && head->reserved2[1]!=0 )
					return MemlmdSigcheck(buf, size ,flag);

				break;
			}
		}
	}

	return 0;
}

/*
int MemlmdSigcheckPatched(void *buf,int size,int flag)
{
	PSP_Header *head=(PSP_Header *)buf;
	int i;
	int check = 0;

	if( head != NULL && head->signature == 0x5053507E)//PSP~
	{
		if(head->reserved2[0] == 0 && head->reserved2[1] == 0 )
		{
#if _PSP_FW_VERSION == 639
			for(i=0;i<0x38;i++)
#else
			for(i=0;i<0x30;i++)
#endif
			{
				if(head->scheck[i] != 0 )
				{
					check = 1;
					break;
				}
			}

			if( check == 0)
			{
				return 0;
			}
		}
	}

	return MemlmdSigcheck(buf, size ,flag);
}
*/
static void scramble(unsigned char *target, int size, unsigned char *seed, int seed_size)
{
	int seed_offset = 0;
	unsigned char *end_buffer = target + size;

	while( target < end_buffer )
	{
		if( seed_offset >= seed_size )
			seed_offset = 0;

		*target ^= seed[seed_offset];
		seed_offset++;
		target++;
		
	}

}

static void scramble_simple(unsigned int *target, unsigned int *seed, int size )
{
	unsigned int *end_buffer = target + size/sizeof(int);

	while( target < end_buffer )
	{
		*target ^= *seed;

		seed++;
		target++;
	}
}

int CheckCompress(PSP_Header *header)
{
	u8 *data = ((u8 *)header ) + 336;
	u16 data_top = *(u16 *)data;
	if( data_top == 0x8B1F )
		return 1;

	data_top ^= *((u16 *)(&header->scheck[0x38]));
	if( data_top == 0x8B1F && ( *((u32 *)&data[4]) == ~(*((u32 *)(&header->scheck[0x38 + 4])) )) )
	{
		goto MAIN_SCRAMBLE;
	}

	data_top ^= *((u16 *)header->key_data1);
	if( data_top == 0x8B1F )
	{
		scramble_simple( (u32 *)data , (u32 *)header->key_data1 , 0x10 );

MAIN_SCRAMBLE:
		scramble( data , header->comp_size , &(header->scheck[0x38]) , 0x20 );
		return 1;
	}

	return 0;
}


void *SystemCtrlForKernel_AC0E84D1(int (* func)())
{
	int (* r)() = ExtendDecryption;
	ExtendDecryption = func;
	return r;
}

int MemlmdDecryptPatched(u8 *buf, int size ,int *ret, int flag )
{
	PSP_Header *head=(PSP_Header *)buf;


	if(ExtendDecryption)
	{
		int res;
		if( ( res = ExtendDecryption( buf , size , ret , flag )) >=0)
			return res;
	}

	if(buf && ret)
	{
		if(head->oe_tag ==0xC6BA41D3 || head->oe_tag == 0x55668D96 || head->oe_tag == 0xC01DB15D)
		{
			if( CheckCompress( head ) )
			{
				*ret=head->comp_size;
				memmove(buf ,buf+336 ,*ret);
				return 0;
			}
		}
	}

	int i =MemlmdDecrypt( buf, size, ret, flag);
	if(i>=0)
		return i;

	if( MemlmdSigcheckPatched(buf ,size ,flag) < 0 )
		return i;

	memlmd_C071E9CA(0 ,0xBFC00200);

	return MemlmdDecrypt(buf, size, ret, flag);
}


void PatchMemlmd()
{
#if _PSP_FW_VERSION == 620
#if PSP_MODEL == 0
//#define MemlmdTagPatchAddr	0x00001290
#define MemlmdSetSeedAddr	0x00001158//OK
#define MemlmdSigcheckAddr	0x00000F10//OK
#define MemlmdPatchAddr1	0x000010D8//OK
#define MemlmdPatchAddr2	0x0000112C//OK
#define MemlmdDecryptAddr	0x00000134//OK
#define MemlmdPatchAddr3	0x00000E10//OK
#define MemlmdPatchAddr4	0x00000E74//OK

#else
//#define MemlmdTagPatchAddr	0x00001360
#define MemlmdSetSeedAddr	0x000011F0//OK
#define MemlmdSigcheckAddr	0x00000FA8//OK
#define MemlmdPatchAddr1	0x00001170//OK
#define MemlmdPatchAddr2	0x000011C4//OK
#define MemlmdDecryptAddr	0x00000134//OK
#define MemlmdPatchAddr3	0x00000EA8//OK
#define MemlmdPatchAddr4	0x00000F0C//OK
#endif

#elif _PSP_FW_VERSION == 639
#if PSP_MODEL == 0
//#define MemlmdTagPatchAddr	0x00001308
#define MemlmdSetSeedAddr	0x000011D0
#define MemlmdSigcheckAddr	0x00000F88
#define MemlmdPatchAddr1	0x00001150
#define MemlmdPatchAddr2	0x000011A4
#define MemlmdDecryptAddr	0x00000134
#define MemlmdPatchAddr3	0x00000E88
#define MemlmdPatchAddr4	0x00000EEC

#else
//#define MemlmdTagPatchAddr	0x00001430
#define MemlmdSetSeedAddr	0x000012C0
#define MemlmdSigcheckAddr	0x00001078
#define MemlmdPatchAddr1	0x00001240
#define MemlmdPatchAddr2	0x00001294
#define MemlmdDecryptAddr	0x00000134
#define MemlmdPatchAddr3	0x00000F78
#define MemlmdPatchAddr4	0x00000FDC
#endif

#elif _PSP_FW_VERSION == 660
#if PSP_MODEL == 0
//#define MemlmdTagPatchAddr	0x000013F0
#define MemlmdSetSeedAddr	0x000012B8
#define MemlmdSigcheckAddr	0x00001070
#define MemlmdPatchAddr1	0x00001238
#define MemlmdPatchAddr2	0x0000128C
#define MemlmdDecryptAddr	0x0000020C
#define MemlmdPatchAddr3	0x00000F70
#define MemlmdPatchAddr4	0x00000FD4

#else
//#define MemlmdTagPatchAddr	0x000014B0
#define MemlmdSetSeedAddr	0x00001340
#define MemlmdSigcheckAddr	0x000010F8
#define MemlmdPatchAddr1	0x000012C0
#define MemlmdPatchAddr2	0x00001314
#define MemlmdDecryptAddr	0x0000020C
#define MemlmdPatchAddr3	0x00000FF8
#define MemlmdPatchAddr4	0x0000105C
#endif

#else
#error decryptcore_memlmd
#endif

	SceModule2 *mod = sceKernelFindModuleByName("sceMemlmd");
	u32 text_addr = mod->text_addr;

	//tag check
//	_sh( 0xF005 , text_addr +  MemlmdTagPatchAddr );

	//set seed key
	memlmd_C071E9CA = (void *)(text_addr+ MemlmdSetSeedAddr );

	//MemlmdSigcheck
	MemlmdSigcheck = (void *)(text_addr+ MemlmdSigcheckAddr );
	//memlmd_3F2AC9C6(a0,a1,a2)
	MAKE_CALL(text_addr + MemlmdPatchAddr1 , MemlmdSigcheckPatched);
	//memlmd_97DA82BC(a0,a1,a2)
	MAKE_CALL(text_addr + MemlmdPatchAddr2 , MemlmdSigcheckPatched);

	//MemlmdDecrypt(buf, size, ret, flag);
	MemlmdDecrypt = (void *)(text_addr+ MemlmdDecryptAddr );
	//memlmd_E42AFE2E
	MAKE_CALL(text_addr+ MemlmdPatchAddr3 , MemlmdDecryptPatched );
	//memlmd_D56F8AEC
	MAKE_CALL(text_addr+ MemlmdPatchAddr4 , MemlmdDecryptPatched );

//	ClearCaches();
}


int (* MesgLedDecrypt)() = NULL;
int (* MesgLedDecryptEX)() = NULL;

void *SystemCtrlForKernel_1F3037FB(int (* func)())
{
	int (* r)() = (void *)MesgLedDecryptEX;
	MesgLedDecryptEX = (void *)func;
	return (void *)r;
}

int MesgLedDecryptPatched(u32* a0/*tag*/,u8* a1/*key*/,int a2,u8 *buff,int t0/*size*/,int *t1/*ret*/,int t2/*flag*/,char *t3 ,
						  int sp0 , int sp1/* type*/ , int sp2 , int sp3)//decrypt patch
{

	if(buff && a0 && t1)
	{

		if( MesgLedDecryptEX )
		{
			int ret = MesgLedDecryptEX( buff , t0 , t1 , t2 );
			if( ret >= 0 )
				return ret;
		}

		PSP_Header *head=(PSP_Header *)buff;
		int tag=head->oe_tag;

		if(tag==0x28796DAA || tag==0x7316308C ||
			tag==0x3EAD0AEE || tag==0x8555ABF2)
		{
			if( CheckCompress( head ) )
			{
				*t1=head->comp_size;
				memmove(buff ,buff + 336 , head->comp_size );
				return 0;
			}
		}
	}

	int i= MesgLedDecrypt( a0, a1, a2, buff, t0, t1,t2,t3, sp0 , sp1 , sp2 , sp3);
	if(i  >= 0 )
		return i;

	if( MemlmdSigcheckPatched( buff, t0, t2) < 0 )
		return i;

	return MesgLedDecrypt( a0, a1, a2, buff, t0, t1,t2,t3, sp0 , sp1 , sp2 , sp3);
}

void PatchMesgLed()
{
#if _PSP_FW_VERSION == 620
#if PSP_MODEL == 0 //01g
#define MesgLedDecryptPatchAddr1	0x00001DB4//OK
#define MesgLedDecryptPatchAddr2	0x00001880//OK
#define MesgLedDecryptPatchAddr3	0x00003808//OK
#define MesgLedDecryptPatchAddr4	0x00003AF4//OK
#define MesgLedDecryptPatchAddr5	0x00002C04//OK
#elif PSP_MODEL == 1 //02g
#define MesgLedDecryptPatchAddr1	0x00001E44//OK
#define MesgLedDecryptPatchAddr2	0x00001880//OK
#define MesgLedDecryptPatchAddr3	0x00003D10//OK
#define MesgLedDecryptPatchAddr4	0x00004044//OK
#define MesgLedDecryptPatchAddr5	0x00002F34//OK
#elif PSP_MODEL == 2 //(PSP_MODEL == 2 || PSP_MODEL == 3 ||  PSP_MODEL == 6 ||  PSP_MODEL == 8)//03g
#define MesgLedDecryptPatchAddr1	0x00001ED4//OK
#define MesgLedDecryptPatchAddr2	0x00001880//OK
#define MesgLedDecryptPatchAddr3	0x000041F0//OK
#define MesgLedDecryptPatchAddr4	0x0000456C//OK
#define MesgLedDecryptPatchAddr5	0x00003244//OK
#elif PSP_MODEL == 4 //05g
#define MesgLedDecryptPatchAddr1	0x00001F64//OK
#define MesgLedDecryptPatchAddr2	0x00001880//OK
#define MesgLedDecryptPatchAddr3	0x00004674//OK
#define MesgLedDecryptPatchAddr4	0x00004A38//OK
#define MesgLedDecryptPatchAddr5	0x00003518//OK
#endif

#elif _PSP_FW_VERSION == 639
#if PSP_MODEL == 0 //01g
#define MesgLedDecryptPatchAddr1	0x00001D20
#define MesgLedDecryptPatchAddr2	0x00001ADC
#define MesgLedDecryptPatchAddr3	0x00003914
#define MesgLedDecryptPatchAddr4	0x00003AA0
#define MesgLedDecryptPatchAddr5	0x00002DB4
#elif PSP_MODEL == 1 //02g
#define MesgLedDecryptPatchAddr1	0x00001DD0
#define MesgLedDecryptPatchAddr2	0x00001AE8
#define MesgLedDecryptPatchAddr3	0x00003E84
#define MesgLedDecryptPatchAddr4	0x00004010
#define MesgLedDecryptPatchAddr5	0x00003194
#elif PSP_MODEL == 2 //(PSP_MODEL == 2 || PSP_MODEL == 3 ||  PSP_MODEL == 6 ||  PSP_MODEL == 8)//03g
#define MesgLedDecryptPatchAddr1	0x00001E60
#define MesgLedDecryptPatchAddr2	0x00001AE8
#define MesgLedDecryptPatchAddr3	0x000043AC
#define MesgLedDecryptPatchAddr4	0x00004538
#define MesgLedDecryptPatchAddr5	0x00003534
#elif PSP_MODEL == 4 //05g
#define MesgLedDecryptPatchAddr1	0x00001EF8
#define MesgLedDecryptPatchAddr2	0x00001AEC
#define MesgLedDecryptPatchAddr3	0x00004880
#define MesgLedDecryptPatchAddr4	0x00004A0C
#define MesgLedDecryptPatchAddr5	0x000038A0
#endif

#elif _PSP_FW_VERSION == 660
#if PSP_MODEL == 0 //01g
#define MesgLedDecryptPatchAddr1	0x00002114
#define MesgLedDecryptPatchAddr2	0x00001ED0
#define MesgLedDecryptPatchAddr3	0x00003FD8
#define MesgLedDecryptPatchAddr4	0x00004164
#define MesgLedDecryptPatchAddr5	0x0000335C
#elif PSP_MODEL == 1 //02g
#define MesgLedDecryptPatchAddr1	0x000021C4
#define MesgLedDecryptPatchAddr2	0x00001EDC
#define MesgLedDecryptPatchAddr3	0x00004548
#define MesgLedDecryptPatchAddr4	0x000046D4
#define MesgLedDecryptPatchAddr5	0x0000373C
#elif PSP_MODEL == 2 //(PSP_MODEL == 2 || PSP_MODEL == 3 ||  PSP_MODEL == 6 ||  PSP_MODEL == 8)//03g
#define MesgLedDecryptPatchAddr1	0x00002254
#define MesgLedDecryptPatchAddr2	0x00001EDC
#define MesgLedDecryptPatchAddr3	0x00004A70
#define MesgLedDecryptPatchAddr4	0x00004BFC
#define MesgLedDecryptPatchAddr5	0x00003ADC
#elif PSP_MODEL == 4 //05g
#define MesgLedDecryptPatchAddr1	0x000022EC
#define MesgLedDecryptPatchAddr2	0x00001EE0
#define MesgLedDecryptPatchAddr3	0x00004F44
#define MesgLedDecryptPatchAddr4	0x000050D0
#define MesgLedDecryptPatchAddr5	0x00003E48
#endif

#else
#error decryptcore_mesgled
#endif

	SceModule2 *mod_ = sceKernelFindModuleByName("sceMesgLed");
	u32 text_addr = mod_ ->text_addr;

	MAKE_CALL(text_addr+ MesgLedDecryptPatchAddr1 , MesgLedDecryptPatched);
	MAKE_CALL(text_addr+ MesgLedDecryptPatchAddr2 , MesgLedDecryptPatched);
	MAKE_CALL(text_addr+ MesgLedDecryptPatchAddr3 , MesgLedDecryptPatched);
	MAKE_CALL(text_addr+ MesgLedDecryptPatchAddr4 , MesgLedDecryptPatched);
	MAKE_CALL(text_addr+ MesgLedDecryptPatchAddr5 , MesgLedDecryptPatched);

	MesgLedDecrypt =(void *)(text_addr+ 0xE0);
}