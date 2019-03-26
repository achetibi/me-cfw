all:
	bin2c bin/$(TARGET_01g).bin ../$(TARGET_01g).h $(TARGET_01g)
	bin2c bin/$(TARGET_02g).bin ../$(TARGET_02g).h $(TARGET_02g)
	bin2c bin/$(TARGET_03g).bin ../$(TARGET_03g).h $(TARGET_03g)
	bin2c bin/$(TARGET_04g).bin ../$(TARGET_04g).h $(TARGET_04g)
	bin2c bin/$(TARGET_05g).bin ../$(TARGET_05g).h $(TARGET_05g)
	bin2c bin/$(TARGET_07g).bin ../$(TARGET_07g).h $(TARGET_07g)
	bin2c bin/$(TARGET_09g).bin ../$(TARGET_09g).h $(TARGET_09g)
	bin2c bin/$(TARGET_11g).bin ../$(TARGET_11g).h $(TARGET_11g)

	sed -i "s/static unsigned int size/\/\/static unsigned int size/g" ../$(TARGET_01g).h
	sed -i "s/static unsigned int size/\/\/static unsigned int size/g" ../$(TARGET_02g).h
	sed -i "s/static unsigned int size/\/\/static unsigned int size/g" ../$(TARGET_03g).h
	sed -i "s/static unsigned int size/\/\/static unsigned int size/g" ../$(TARGET_04g).h
	sed -i "s/static unsigned int size/\/\/static unsigned int size/g" ../$(TARGET_05g).h
	sed -i "s/static unsigned int size/\/\/static unsigned int size/g" ../$(TARGET_07g).h
	sed -i "s/static unsigned int size/\/\/static unsigned int size/g" ../$(TARGET_09g).h
	sed -i "s/static unsigned int size/\/\/static unsigned int size/g" ../$(TARGET_11g).h

TARGET_01g = pspbtjnf_01g
TARGET_02g = pspbtjnf_02g
TARGET_03g = pspbtjnf_03g
TARGET_04g = pspbtjnf_04g
TARGET_05g = pspbtjnf_05g
TARGET_07g = pspbtjnf_07g
TARGET_09g = pspbtjnf_09g
TARGET_11g = pspbtjnf_11g

PSPSDK = $(shell psp-config --pspsdk-path)
