	.set noreorder

#include "pspstub.s"

	STUB_START "RecoveryBridge",0x40090000,0x00030005
	STUB_FUNC  0x8F664575,CheckModel
	STUB_FUNC  0x7328C6EC,ReadBatterySerial
	STUB_FUNC  0x1037EFD6,WriteBatterySerial
	STUB_END
