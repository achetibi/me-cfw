
ifndef $(TARGET_FW)
TARGET_FW = 150
endif

all: 
	make -C payloadex PSP_MODEL=0 TARGET_FW=$(TARGET_FW)
	make -C payloadex PSP_MODEL=1 TARGET_FW=$(TARGET_FW)
	make -C mainbinex PSP_MODEL=0 TARGET_FW=$(TARGET_FW)
	make -C mainbinex PSP_MODEL=1 TARGET_FW=$(TARGET_FW)
	make -C combine

clean:
	make clean -C payloadex
	make clean -C mainbinex
	make clean -C combine
