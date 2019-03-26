#ifndef _CFW_RECOVERY_TEXT_
#define _CFW_RECOVERY_TEXT_

struct RegistryHack {
	char *Button_o;
	char *Button_x;
	char *Wma;
	char *Wma_on;
	char *Wma_already;
	char *FPlayer;
	char *FPlayer_on;
	char *FPlayer_already;
};

/*
struct FolderName {
	char *Folder3xx;
	char *Folder150;
};
*/

struct UmdModeList {
	char *Normal;
	char *OE;
	char *M33;
	char *NP9660;
	char *ME;
	char *INFERNO;
};

struct Region {
	char *Japan;
	char *America;
	char *Europe;
	char *Korea;
	char *UnitedKingdom;
	char *Mexico;
	char *Australia;
	char *East;
	char *Taiwan;
	char *Russia;
	char *China;
	char *DebugI;
	char *DebugII;
};

struct Color {
	char *green;
	char *red;
	char *blue;
	char *grey;
	char *pink;
	char *purple;
	char *turquoise;
	char *orange;
	char *yellow;
	char *white;
	char *black;
};

typedef struct _MenuText {
	char*  Enabled;
	char*  Disabled;
	char*  Default;
	char*  Formatting;
	char*  Currently;
	char*  Back;
	char*  Exit;
	char*  Exitting;
	char*  UsbEnabled;
	char*  UsbDisabled;
	char*  Always;
	char*  Never;
	char*  UmdDisc;
	char*  MemoryStick;
	char*  HiddenStorage;
	char*  RunProgram;
	char*  Plugins;
	char*  MainMenu;
	char*  SlimColor;
	char*  UmdVideoPatch;
	char*  Unknown;
	char*  Wait;
	char*  Done;
	char*  Failed;
	char*  Saving;
	char*  Setting;
	char*  Reloading;
	char*  Writing;
	char*  AutoRun;
	char*  UsbCharge;
	struct Region region_list;
//	struct FolderName folder_name_list;
	struct UmdModeList umd_mode_list;
	struct RegistryHack registry_list;
	struct Color color_list;
} MenuText;

extern MenuText recovery_str;
extern char **TextAddrList[];

void init_language_file();

#endif
