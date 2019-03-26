#ifndef ___INLINE_H___
#define ___INLINE_H___

#define JAL_OPCODE	0x0C000000
#define J_OPCODE	0x08000000
#define SC_OPCODE	0x0000000C
#define JR_RA		0x03e00008

#define NOP	0x00000000
/*
#define MAKE_JUMP(a, f) _sw(J_OPCODE   | (((u32)(f) & 0x0ffffffc) >> 2), a); 
#define MAKE_CALL(a, f) _sw(JAL_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a); 

#define MAKE_JUMP(a, f)					_sw(0x08000000 | (((u32)(f) >> 2)  & 0x03ffffff), a)
#define MAKE_CALL(a, f)					_sw(0x0C000000 | (((u32)(f) >> 2)  & 0x03ffffff), a)

#define MAKE_SYSCALL(a, n) _sw(SC_OPCODE | (n << 6), a);
#define JUMP_TARGET(x) (0x80000000 | ((x & 0x03FFFFFF) << 2))
*/

static inline u32 GENERATE_JUMP( void *f )
{
	u32 func;
	asm volatile ("ext %0, %1, 2, 26\n" 
		: "=r"(func)	/*o—Í*/
		: "r" ((u32)f)	/*“ü—Í*/
	);

	return (func | J_OPCODE );
}

static inline u32 GENERATE_CALL( void *f )
{
	u32 func;
	asm volatile ("ext %0, %1, 2, 26\n" : "=r"(func): "r" ((u32)f));
	return (func | JAL_OPCODE );
}

static inline void REDIRECT_FUNCTION(u32 a, void *f )
{
	_sw( GENERATE_JUMP( f ), a );
//	_sw(J_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a);
	_sw( NOP, a + 4);
}

#define MAKE_JUMP(a, f) _sw( GENERATE_JUMP( (void *)f ), a); 
#define MAKE_CALL(a, f) _sw( GENERATE_CALL( (void *)f ), a); 


static inline void MAKE_DUMMY_FUNCTION0(u32 a)
{
	_sw(0x03e00008, a );_sw(0x00001021, a + 4 );
}

static inline void MAKE_DUMMY_FUNCTION1(u32 a)
{
	_sw(0x03e00008, a); _sw(0x24020001, a + 4 );
}


static inline unsigned int _pspSdkSetK1(unsigned int a0)
{
	u32 ret;
	asm  ("move %0, $k1\n" : "=r" (ret));
	asm volatile ("move $k1, %0\n" :: "r" (a0));
	return ret;
}

#define pspSdkSetK1 _pspSdkSetK1

#endif