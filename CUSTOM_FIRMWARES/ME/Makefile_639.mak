AEMU = aemu
MECFW = mecfw

all:
	make -C $(MECFW) distribute_all TARGET_FW=639
	make -C $(AEMU) CONFIG_63X=1 RELEASE=1

clean:
	make -C $(AEMU) clean
	make -C $(MECFW) clean
