AEMU = aemu
MECFW = mecfw

all:
	make -C $(MECFW) distribute_all TARGET_FW=660
	make -C $(AEMU) CONFIG_660=1 RELEASE=1

clean:
	make -C $(AEMU) clean
	make -C $(MECFW) clean
