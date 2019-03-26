	.set noreorder

#include "pspstub.s"

	STUB_START "DcManager",0x40090000,0x001B0005
		STUB_FUNC  0xA22190CE,dcGetHardwareInfo
		STUB_FUNC  0x3A3EA355,dcPatchModule
		STUB_FUNC  0xD298BA53,dcPatchModuleString
		STUB_FUNC  0x9C87A1C7,dcGetCancelMode
		STUB_FUNC  0xF5161120,dcSetCancelMode
		STUB_FUNC  0x39D91E83,dcLflashStartFatfmt
		STUB_FUNC  0xA899D713,dcLflashStartFDisk
		STUB_FUNC  0x741FFCFD,dcGetNandInfo
		STUB_FUNC  0xEA3F4DE5,dcLockNand
		STUB_FUNC  0x4A5FCC2F,dcUnlockNand
		STUB_FUNC  0x7EB5614A,dcReadNandBlock
		STUB_FUNC  0x504C82B4,dcWriteNandBlock
		STUB_FUNC  0x7E9128B0,dcEraseNandBlock
		STUB_FUNC  0xAD3509E5,dcRegisterPhysicalFormatCallback
		STUB_FUNC  0x1A718513,dcUnregisterPhysicalFormatCallback
		STUB_FUNC  0x48DDEA84,dcQueryRealMacAddress
		STUB_FUNC  0x767A728E,dcIdStorageUnformat
		STUB_FUNC  0xFE8EA2A0,dcIdStorageFormat
		STUB_FUNC  0xFFAAC913,dcIdStorageCreateLeaf
		STUB_FUNC  0xE5F8523D,dcIdStorageCreateAtomicLeaves
		STUB_FUNC  0x127323A6,dcIdStorageReadLeaf
		STUB_FUNC  0x549329DF,dcIdStorageWriteLeaf
		STUB_FUNC  0x34F3F7FC,dcIdStorageFlush
		STUB_FUNC  0xA6C4148A,dcSysconReceiveSetParam
		STUB_FUNC  0x3534B9C1,dcKirkCmd
		STUB_FUNC  0x109A36FC,SW
		STUB_FUNC  0x4914B8D1,LW
	STUB_END

	STUB_START "idsRegeneration",0x40090000,0x00130005
		STUB_FUNC  0xBDE13E76,idsRegenerationSetup
		STUB_FUNC  0xFEE3C55B,idsRegenerationGetIndex
		STUB_FUNC  0x0B33639A,idsRegenerationGetHwConfigKeys
		STUB_FUNC  0xFEA979A6,idsRegenerationGetMGKeys
		STUB_FUNC  0xEE1AD992,idsRegenerationGetFactoryBadBlocksKey
		STUB_FUNC  0x87A30D3A,idsRegenerationGetSerialKey
		STUB_FUNC  0xE37CC2E6,idsRegenerationGetWlanKey
		STUB_FUNC  0xC44FE956,idsRegenerationGetAudioVolumeSetupKey
		STUB_FUNC  0xBC42FEED,idsRegenerationGetUsbKeys
		STUB_FUNC  0x3F014928,idsRegenerationGetUnkKey140
		STUB_FUNC  0x8F7EE9C0,idsRegenerationGetMGKey40
		STUB_FUNC  0xB9156F27,idsRegenerationGetUnkKeys3X
		STUB_FUNC  0xB417BCF0,idsRegenerationGetParentalLockKey
		STUB_FUNC  0xFA2368E8,idsRegenerationGenerateFactoryFirmwareKey
		STUB_FUNC  0x0EF8731D,idsRegenerationGetLCDKey
		STUB_FUNC  0x4E95D36F,idsRegenerationGenerateCallibrationKey
		STUB_FUNC  0xD7603D82,idsRegenerationGetUnkKeys5253
		STUB_FUNC  0x6015A7CD,idsRegenerationGetDefaultXMBColorKey
		STUB_FUNC  0xB79A6C46,idsRegenerationCreateCertificatesAndUMDKeys
	STUB_END

	STUB_START "pspIplUpdate",0x40090000,0x00030005
		STUB_FUNC  0xAD923E2F,pspIplUpdateGetIpl
		STUB_FUNC  0xF7EF8E3F,pspIplUpdateSetIpl
		STUB_FUNC  0x282EDFEC,pspIplUpdateClearIpl
	STUB_END
