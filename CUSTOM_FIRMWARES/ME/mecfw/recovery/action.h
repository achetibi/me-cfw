#ifndef __ACTION_H__
#define __ACTION_H__

char *iso_cache_policy(int type, int direction);
char *iso_cache_num(int type, int direction);
char *iso_cache_total_size(int type, int direction);
char *dummy_change(int type);

char *vsh_clock(int type, int direction);
char *game_clock(int type, int direction);

const char *folder_change(int type, int direction);
const char *umdmode_change(int type, int direction);
const char *region_change(int type, int direction);
char *usb_change(int type, int direction);
char *ms_speed_change(int type, int direction);
const char *color_change(int type, int direction);
const char *vsh_color_change(int type, int direction);

int make_JigkickBattery();
int make_NormalBattery();
int make_AutobootBattery();
char *backup_eeprom(int type );

void* plugin_manager(int type);

char *autorun_umdvideo_change(int type);
char *usbcharge_slimcolor_change(int type);


const char *swap_key(int type);
void activate_wma();
void activate_flash();

int toggle_usb_ms();
void* toggle_usb_flash0();
void* toggle_usb_flash1();
void* toggle_usb_flash2();
void* toggle_usb_flash3();
int disable_usb();

void* run_game();
int format_flash1();
int format_flash2();

int netcnf_backup();
int netcnf_restore();


#endif
