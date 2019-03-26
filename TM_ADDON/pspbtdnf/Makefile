TARGET_01G = pspbtdnf
TARGET_02G = pspbtdnf_02g

all: clean
	bash build.sh
	bin2c $(TARGET_01G).bin $(TARGET_01G).h $(TARGET_01G)_DC500
	bin2c $(TARGET_02G).bin $(TARGET_02G).h $(TARGET_02G)_DC500
	

clean:
	@rm -f *.bin *.h