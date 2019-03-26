AEMU = aemu
MECFW = mecfw

all:
	make -C $(MECFW) distribute_all TARGET_FW=620
	make -C $(AEMU) CONFIG_620=1 RELEASE=1

clean:
	make -C $(AEMU) clean
	make -C $(MECFW) clean
