	.set noreorder

#include "pspstub.s"

#if _PSP_FW_VERSION == 620
	STUB_START "scePaf",0x00010011,0x000A0005 	
	STUB_FUNC 0x58189108, scePaf_strlen
	STUB_FUNC 0x6F64AB9B, scePaf_memset
	STUB_FUNC 0x16789955, scePaf_memcmp
	STUB_FUNC 0xE8473E80, scePaf_sprintf
	STUB_FUNC 0x5E7610DF, scePaf_snprintf
	STUB_FUNC 0x23C8DAB5, scePaf_memcpy
	STUB_FUNC 0xE5A74996, scePaf_strcpy
	STUB_FUNC 0xA34473F7, scePaf_strtok_r
	STUB_FUNC 0xF0D98BD1, scePaf_malloc
	STUB_FUNC 0xE0E8820F, scePaf_free
	STUB_END

#elif _PSP_FW_VERSION == 639
	STUB_START "scePaf",0x00010011,0x000A0005 
	STUB_FUNC 0x49A72E5D, scePaf_strlen
	STUB_FUNC 0x43B6A88A, scePaf_memset
	STUB_FUNC 0x861C4627, scePaf_memcmp
	STUB_FUNC 0x4C386F3C, scePaf_sprintf
	STUB_FUNC 0x726776D7, scePaf_snprintf
	STUB_FUNC 0xE281261E, scePaf_memcpy
	STUB_FUNC 0x8F95CC01, scePaf_strcpy
	STUB_FUNC 0x38D7AF98, scePaf_strtok_r
	STUB_FUNC 0x9E9FFBFB, scePaf_malloc
	STUB_FUNC 0xB0363C2E, scePaf_free
	STUB_END

#elif _PSP_FW_VERSION == 660
	STUB_START "scePaf",0x00010011,0x000A0005 
	STUB_FUNC 0xD7DCB972, scePaf_strlen
	STUB_FUNC 0xBB89C9EA, scePaf_memset
	STUB_FUNC 0xB05D9677, scePaf_memcmp
	STUB_FUNC 0xA138A376, scePaf_sprintf
	STUB_FUNC 0x7B7133D5, scePaf_snprintf
	STUB_FUNC 0xF7C46E37, scePaf_memcpy
	STUB_FUNC 0x726DFBA9, scePaf_strcpy
	STUB_FUNC 0x7E600551, scePaf_strtok_r
	STUB_FUNC 0x5E909060, scePaf_malloc
	STUB_FUNC 0xDB1612F1, scePaf_free
	STUB_END

#elif _PSP_FW_VERSION == 661
	STUB_START "scePaf",0x00010011,0x000A0005 
	STUB_FUNC 0xD7DCB972, scePaf_strlen
	STUB_FUNC 0xBB89C9EA, scePaf_memset
	STUB_FUNC 0xB05D9677, scePaf_memcmp
	STUB_FUNC 0xA138A376, scePaf_sprintf
	STUB_FUNC 0x7B7133D5, scePaf_snprintf
	STUB_FUNC 0xF7C46E37, scePaf_memcpy
	STUB_FUNC 0x726DFBA9, scePaf_strcpy
	STUB_FUNC 0x7E600551, scePaf_strtok_r
	STUB_FUNC 0x5E909060, scePaf_malloc
	STUB_FUNC 0xDB1612F1, scePaf_free
	STUB_END

#else
#error scePaf.S
#endif