#ifndef _CFW_VSHMENU_TEXT_
#define _CFW_VSHMENU_TEXT_

struct UmdModeList {
	char *Normal;
	char *OE;
	char *M33;
	char *NP9660;
	char *ME;
	char *INFERNO;
};

struct Colors {
	char *red;
	char *green;
	char *blue;
	char *pink;
	char *purple;
	char *orange;
	char *yellow;
	char *black;
	char *white;
};

typedef struct _MenuText {
	char*  Enabled;
	char*  Disabled;
	char*  Default;
	char*  None;
	char*  Back;
	char*  UmdDisc;
	char*  MemoryStick;
	char*  HiddenStorage;
	struct UmdModeList umd_mode_list;
	struct Colors color_list;
} MenuText;


extern MenuText vshmenu_str;
extern char **TextAddrList[];

void init_font_table();

#endif