	.set noreorder

#include "pspstub.s"

	STUB_START "Plutonium",0x40090000,0x00040005
	STUB_FUNC  0x340AC1E4,PlutoniumGetModel
	STUB_FUNC  0xE4BAB456,PlutoniumColdReset
	STUB_FUNC  0x791E6046,PlutoniumStartUpdater
	STUB_FUNC  0x7E91802A,PlutoniumLoadUranium
	STUB_END
