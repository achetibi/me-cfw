all:
	bin2c bin/$(TARGET_01g).bin ../$(TARGET_01g).h $(TARGET_01g)
	bin2c bin/$(TARGET_02g).bin ../$(TARGET_02g).h $(TARGET_02g)
	bin2c bin/$(TARGET_03g).bin ../$(TARGET_03g).h $(TARGET_03g)
	bin2c bin/$(TARGET_04g).bin ../$(TARGET_04g).h $(TARGET_04g)
	bin2c bin/$(TARGET_05g).bin ../$(TARGET_05g).h $(TARGET_05g)

	bin2c bin/$(TARGET_01g_p).bin ../$(TARGET_01g_p).h $(TARGET_01g_p)
	bin2c bin/$(TARGET_02g_p).bin ../$(TARGET_02g_p).h $(TARGET_02g_p)
	bin2c bin/$(TARGET_03g_p).bin ../$(TARGET_03g_p).h $(TARGET_03g_p)
	bin2c bin/$(TARGET_04g_p).bin ../$(TARGET_04g_p).h $(TARGET_04g_p)
	bin2c bin/$(TARGET_05g_p).bin ../$(TARGET_05g_p).h $(TARGET_05g_p)

	sed -i "s/static unsigned int size/\/\/static unsigned int size/g" ../$(TARGET_01g).h
	sed -i "s/static unsigned int size/\/\/static unsigned int size/g" ../$(TARGET_02g).h
	sed -i "s/static unsigned int size/\/\/static unsigned int size/g" ../$(TARGET_03g).h
	sed -i "s/static unsigned int size/\/\/static unsigned int size/g" ../$(TARGET_04g).h
	sed -i "s/static unsigned int size/\/\/static unsigned int size/g" ../$(TARGET_05g).h

	sed -i "s/static unsigned int size/\/\/static unsigned int size/g" ../$(TARGET_01g_p).h
	sed -i "s/static unsigned int size/\/\/static unsigned int size/g" ../$(TARGET_02g_p).h
	sed -i "s/static unsigned int size/\/\/static unsigned int size/g" ../$(TARGET_03g_p).h
	sed -i "s/static unsigned int size/\/\/static unsigned int size/g" ../$(TARGET_04g_p).h
	sed -i "s/static unsigned int size/\/\/static unsigned int size/g" ../$(TARGET_05g_p).h

TARGET_01g = pspbtjnf_01g
TARGET_02g = pspbtjnf_02g
TARGET_03g = pspbtjnf_03g
TARGET_04g = pspbtjnf_04g
TARGET_05g = pspbtjnf_05g

TARGET_01g_p = pspbtjnf_01g_p
TARGET_02g_p = pspbtjnf_02g_p
TARGET_03g_p = pspbtjnf_03g_p
TARGET_04g_p = pspbtjnf_04g_p
TARGET_05g_p = pspbtjnf_05g_p

PSPSDK = $(shell psp-config --pspsdk-path)
