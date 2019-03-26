all:
	psp-packer -d $(TARGET_FW) -i $(TARGET)_$(TARGET_FW).prx -o $(TARGET).prx
	bin2c $(TARGET).prx $(TARGET).h $(TARGET)
	sed -i "s/static unsigned int size/\/\/static unsigned int size/g" $(TARGET).h
	rm -f $(TARGET).prx

clean: reset
reset:
	rm -f $(TARGET).h

TARGET = dax9660

ifndef $(TARGET_FW)
TARGET_FW = 150
endif

PSPSDK = $(shell psp-config --pspsdk-path)
