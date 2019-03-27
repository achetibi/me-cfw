AEMU = aemu
MECFW = mecfw

all:
	make -C $(MECFW) distribute_all TARGET_FW=661
	make -C $(AEMU) CONFIG_661=1 RELEASE=1

clean:
	make -C $(AEMU) clean
	make -C $(MECFW) clean
