

#include "pops_patch_list.h"
#include "inline.h"

#define JAL_OPCODE	0x0C000000
#define J_OPCODE	0x08000000
#define SC_OPCODE	0x0000000C
#define JR_RA		0x03e00008

#define NOP	0x00000000

#define MAKE_SYSCALL(a, n) _sw(SC_OPCODE | (n << 6), a);
#define JUMP_TARGET(x) (0x80000000 | ((x & 0x03FFFFFF) << 2))

/*
#define MAKE_JUMP(a, f) _sw(J_OPCODE | (((u32)(f) & 0x0ffffffc) >> 2), a);
#define MAKE_CALL(a, f) _sw(JAL_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a);
#define REDIRECT_FUNCTION(a, f) _sw(J_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a);  _sw(NOP, a+4);
#define MAKE_DUMMY_FUNCTION0(a) _sw(0x03e00008, a); _sw(0x00001021, a+4);
#define MAKE_DUMMY_FUNCTION1(a) _sw(0x03e00008, a); _sw(0x24020001, a+4);
*/

static inline u32 _lwl( void *f )
{
	u32 func;
	asm volatile ("lwr %0, 0(%1)\n"
		"lwl %0, 3(%1)\n" 
		: "=r"(func)	/*o—Í*/
		: "r" ((u32)f)	/*“ü—Í*/
	);
	return func;
}

static inline void _swl( u32 value, void *f )
{
	asm volatile (
		"swr %0, 0(%1)\n"
		"swl %0, 3(%1)\n" 
		::"r"(value) ,"r" ((u32)f)	/*“ü—Í*/
	);
	return;
}

typedef struct { 
	u32 signature; 
	int version; 
	int offset[8]; 
} HEADER; 

/*
#define DEBUG_PATH "ms0:/idcange_log.txt"
void init_debug()
{
        SceUID fd;
        
        if ((fd = sceIoOpen(DEBUG_PATH, PSP_O_CREAT | PSP_O_WRONLY | PSP_O_TRUNC, 0777)) >= 0)
        {
                sceIoClose(fd);
        }
}

// Debugging log function
void write_debug(const char* description, void* value, unsigned int size)
{
        SceUID fd;
        
        if ((fd = sceIoOpen(DEBUG_PATH, PSP_O_CREAT | PSP_O_WRONLY | PSP_O_APPEND, 0777)) >= 0)
        {
                if (description != NULL)
                {       
                        sceIoWrite(fd, description, strlen(description));
                }
                if (value != NULL) 
                {       
                        sceIoWrite(fd, value, size);
                }
                sceIoClose(fd);
        }
}

void log_u32( char *fmt, u32 cnt)
{
	char msg[256];	

	sprintf(msg,"%s :0x%08X\n", fmt, cnt);

	write_debug(msg ,NULL , 0);	

}
void log_txt( char *msg)
{

	write_debug(msg ,NULL , 0);	

}
*/