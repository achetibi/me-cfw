#ifndef __TRANS_H__
#define __TRANS_H__

#define STRING (char *)g_messages

enum {
	/* Menu & Titles */
	STR_UNIVERSAL_UNBRICKER = 0,
	STR_CUSTOM_FIRMWARES,
	STR_OFFICIAL_FIRMWARES,
	STR_NAND_OPERATION,
	STR_HARDWARE_INFO,
	STR_TEST_M33,
	STR_SHUTDOWN,
	STR_REBOOT,
	STR_INSTALL_OFW_VER,
	STR_INSTALL_CFW_VER,
	STR_DUMP_NAND,
	STR_RESTORE_NAND,
	STR_CHECK_NAND,
	STR_FORMAT_FLASH,
	STR_IDSTORAGE_TOOLS,
	STR_NEW_IDSTORAGE,
	STR_CHANGE_REGION,
	STR_FIX_MAC_ADDRESS,
	STR_BACKUP_IDSTORAGE,
	STR_RESTORE_IDSTORAGE,

	/* Firmware install */
	STR_FLASHING_FW_VERSION,
	STR_PBP_NOT_FOUND_AT_ROOT,
	STR_INVALID_PBP_FILE_SIZE,
	STR_CFW_NOT_SUPPORTED_BY_PSP,
	STR_CUSTOM_FIRMWARE,
	STR_OFFICIAL_FIRMWARE,
	STR_VERIFYING_PBP_VER,
	STR_INVALID_OR_CORRUPTED_PBP,
	STR_EXTRACTING_UPDATER_MODULES,
	STR_CANNOT_DECRYPT_DATA_PSP,
	STR_ERROR_EXTRACTING_MODULE,
	STR_LOADING_UPDATER_MODULE,
	STR_ERROR_LOADING_UPDATER_MODULE,
	STR_UNASSIGN_FLASH,
	STR_FORMAT_FLASH_PARTITION,
	STR_FORMAT_FLASH_FAILED,
	STR_ASSIGN_FLASH,
	STR_ASSIGN_FLASH_FAILED,
	STR_CREATING_FLASH_DIRECTORIES,
	STR_ERROR_CREATING_FLASH_DIRECTORIES,
	STR_ERROR_OPENING_PBP,
	STR_LOADING_PSAR,
	STR_ERROR_READING_PBP,
	STR_PSPPSARINIT_FAILED,
	STR_PSAR_DECODE_ERROR,
	STR_CANNOT_FIND_PATH_OF,
	STR_FLASHING_FILE,
	STR_ERROR_FLASHING_FILE,
	STR_CANNOT_DECRYPT_PSP_SLIM_IPL,
	STR_ERROR_IN_FUNCTION,
	STR_CANNOT_DECRYPT_COMMON_TABLE,
	STR_CANNOT_DECRYPT_GENERATION_TABLE,
	STR_RESTORING_REGISTRY,
	STR_INSTALL_COMPLETE_SHUTDOWN_REQUIRED,
	STR_INSTALL_COMPLETE_SHUTDOWN_OR_REBOOT_REQUIRED,

	/* Hardware Informations */
	/* TAB 01 */
	STR_PSP_INFORMATIONS,
	STR_PSP_INFORMATIONS_MODEL,
	STR_PSP_INFORMATIONS_GENERATION,
	STR_PSP_INFORMATIONS_CPU_SPEED,
	STR_PSP_INFORMATIONS_BUS_SPEED,
	STR_PSP_INFORMATIONS_FREE_RAM,
	STR_PSP_INFORMATIONS_NAME,
	STR_PSP_INFORMATIONS_PSP_VERSION,
	STR_PSP_INFORMATIONS_REGION,
	STR_PSP_INFORMATIONS_ORIGINAL_FW,
	STR_PSP_INFORMATIONS_MOTHERBOARD,
	STR_PSP_INFORMATIONS_WLAN_STATUS,
	STR_PSP_INFORMATIONS_MAC_ADDRESS,
	/* TAB 02 */
	STR_SYSTEM_INFORMATIONS,
	STR_SYSTEM_INFORMATIONS_TACHYON,
	STR_SYSTEM_INFORMATIONS_BARYON,
	STR_SYSTEM_INFORMATIONS_POMMEL,
	STR_SYSTEM_INFORMATIONS_KIRK_VER,
	STR_SYSTEM_INFORMATIONS_SPOCK_VER,
	STR_SYSTEM_INFORMATIONS_EEPROM_ACCESS,
	STR_SYSTEM_INFORMATIONS_FUSE_ID,
	STR_SYSTEM_INFORMATIONS_FUSE_CONFIG,
	STR_SYSTEM_INFORMATIONS_NAND_SEED,
	STR_SYSTEM_INFORMATIONS_NAND_SIZE,
	STR_SYSTEM_INFORMATIONS_UMD_FIRMWARE,
	STR_SYSTEM_INFORMATIONS_TIME_MACHINE,
	/* TAB 03 */
	STR_MS_INFORMATIONS,
	STR_MS_INFORMATIONS_BOOT_STATUS,
	STR_MS_INFORMATIONS_SIGNATURE,
	STR_MS_INFORMATIONS_PARTITION_TYPE,
	STR_MS_INFORMATIONS_STARTING_HEAD,
	STR_MS_INFORMATIONS_LAST_HEAD,
	STR_MS_INFORMATIONS_MS_SIZE,
	STR_MS_INFORMATIONS_IPL_SPACE,
	STR_MS_INFORMATIONS_ABS_SECTOR,
	STR_MS_INFORMATIONS_TOTAL_SECTOR,
	STR_MS_INFORMATIONS_START_SEC_CLU,
	STR_MS_INFORMATIONS_LAST_SEC_CLU,
	STR_MS_INFORMATIONS_FREE_MS_SIZE,
	/* TAB 04 */
	STR_BATTERY_INFORMATIONS,
	STR_BATTERY_INFORMATIONS_STATUS,
	STR_BATTERY_INFORMATIONS_CHARGE,
	STR_BATTERY_INFORMATIONS_IN_USE,
	STR_BATTERY_INFORMATIONS_SOURCE,
	STR_BATTERY_INFORMATIONS_BATTERY,
	STR_BATTERY_INFORMATIONS_EXTERNAL,
	STR_BATTERY_INFORMATIONS_LEVEL,
	STR_BATTERY_INFORMATIONS_HOURE_LEFT,
	STR_BATTERY_INFORMATIONS_MODE,
	STR_BATTERY_INFORMATIONS_MODE_PANDORA,
	STR_BATTERY_INFORMATIONS_MODE_AUTOBOOT,
	STR_BATTERY_INFORMATIONS_MODE_NORMAL,
	STR_BATTERY_INFORMATIONS_MODE_UNSUPPORTED,
	STR_BATTERY_INFORMATIONS_REMAIN_CAPACITY,
	STR_BATTERY_INFORMATIONS_TOTAL_CAPACITY,
	STR_BATTERY_INFORMATIONS_TEMPERATURE,
	STR_BATTERY_INFORMATIONS_VOLTAGE,
	STR_BATTERY_INFORMATIONS_SERIAL,

	/* Nand Dump */
	STR_AN_EXISTING_NAND_DUMP_WAS_FOUND,
	STR_INSUFFICIENT_FREE_MS_SPACE,
	STR_DUMPING_NAND,
	STR_ERROR_CREATING_NAND_DUMP_BIN,
	STR_DUMP_IS_COMPLETE,
	STR_BAD_BLOCK_WERE_FOUND,
	STR_NO_BAD_BLOCK_WERE_FOUND,

	/* Nand Restore */
	STR_NAND_DUMP_NOT_FOUND_AT_ROOT,
	STR_NAND_DUMP_HAS_NOT_THE_CORRECT_SIZE,
	STR_PHYSICAL_NAND_RESTORE_WARNING,
	STR_RESTORING_NAND,
	STR_NAND_INFO_NOT_EXPECTED,
	STR_ERROR_OPENING_NAND_DUMP_BIN,
	STR_RESTORE_BREAK,
	STR_THERE_ARE_BEING_TOO_MANY_ERRORS,
	STR_NAND_RESTORE_IS_COMPLETE,

	/* Check Nand */
	STR_VERIFYING_NAND,
	STR_VERIFICATION_IS_COMPLETE,

	/* Format Nand */
	STR_FLASH_PART_SIZE,
	STR_PRESS_BUTTON_TO_BEGIN_FORMAT,
	STR_LOADING_MODULES,
	STR_ERROR_LOADING_MODULES,
	STR_PHYSICAL_FORMAT,
	STR_ERROR_IN_PHYSICAL_FORMAT,
	STR_CREATING_PARTITION,
	STR_ERROR_IN_FDISK,
	STR_LOGICAL_FORMAT_FLASH_PART,
	STR_FORMAT_IS_COMPLETE,

	/* Create IdStorage */
	STR_SELECT_REGION,
	STR_GET_REAL_MAC,
	STR_THE_WLAN_SWITCH_IS_OFF,
	STR_CANNOT_GET_REAL_MAC_ADDRESS,
	STR_SKIP_THIS_STEP_TO_GENERATE_RANDOM_MAC,
	STR_MAC_ADDRESS_RANDOM,
	STR_MAC_RANDOM,
	STR_PRESS_BUTTON_TO_CREATE_IDSTORAGE,
	STR_CREATE_IDSTORAGE_WARNING,
	STR_FORMATTING_IDSTORAGE,
	STR_ERROR_FORMATTING_IDSTORAGE,
	STR_CREATING_IDSTORAGE_INDEX,
	STR_ERROR_CREATING_KEY,
	STR_UGLY_BUG_IN_IDSREGENERATIONGETINDEX,
	STR_ERROR_CREATING_KEYS,
	STR_GENERATING_CERTIFICATES_AND_UMD_KEYS,
	STR_ERROR_GENERATING_CERTIFICATES_AND_UMD_KEYS,
	STR_ERROR_WRITING_KEY,
	STR_SCEIDSTORAGEFLUSH_FAILED,
	STR_VERIFYING_CERTIFICATES,
	STR_CERTIFICATES_VERIFICATION_FAILED,
	STR_CREATING_OTHER_KEYS,
	STR_IDSTORAGE_SUCCESFULLY_CREATED,

	/* Change Region */
	STR_CHANGE_REGION_WARNING,
	STR_CHANGING_REGION,
	STR_ERROR_WRITING_KEYS,
	STR_REGING_CHANGED_SUCCESFULLY,

	/* Fix Mac */
	STR_RESTORING_ORIGINAL_MAC,
	STR_ERROR_WRITING_TO_IDSTORAGE,
	STR_ORIGINAL_MAC_WRITTEN_SUCCESFULLY,

	/* Dump IdStorage */
	STR_AN_EXISTING_IDSTORAGE_WAS_FOUND,
	STR_DUMPING_IDSTORAGE,
	STR_ERROR_CREATING_IDSTORAGE_BIN,
	STR_SAVING_IDSTORAGE_KEY,

	/* Restore IdStorage */
	STR_IDSTORAGE_BIN_NOT_FOUND_AT_ROOT,
	STR_IDSTORAGE_BIN_HAS_NOT_THE_CORRECT_SIZE,
	STR_IDSTORAGE_BIN_HAS_BEEN_MODIFIED,
	STR_IDSTORAGE_RESTORE_WARNING,
	STR_RESTORING_IDSTORAGE,
	STR_RESTORING_IDSTORAGE_KEY,
	STR_RESTORE_IS_COMPLETE,

	/* Regions */
	STR_REGION_JAPAN,
	STR_REGION_AMERICA,
	STR_REGION_AUSTRALIA,
	STR_REGION_UNITED_KINGDOM,
	STR_REGION_EUROPE,
	STR_REGION_KOREA,
	STR_REGION_HONGKONG,
	STR_REGION_TAIWAN,
	STR_REGION_RUSSIA,
	STR_REGION_CHINA,
	STR_REGION_MEXICO,

	/* Others */
	STR_UNKNOWN,
	STR_ON,
	STR_OFF,
	STR_YES,
	STR_NO,
	MSG_END
};

void vpl_init(void);
void vpl_finish(void);
void select_language(void);

#endif

