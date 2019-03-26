	.set noreorder

#include "pspstub.s"

	STUB_START "SystemCtrlForUser", 0x40090000, 0x00020005
	STUB_FUNC 0x1DDDAD0C, sctrlSESetConfig
	STUB_FUNC 0x16C3B7EE, sctrlSEGetConfig
	STUB_END

	STUB_START "pspUsbDevice", 0x40090000, 0x00020005
	STUB_FUNC 0xD4D90520, pspUsbDeviceSetDevice
	STUB_FUNC 0xB3CF0329, pspUsbDeviceFinishDevice
	STUB_END
