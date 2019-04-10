TM_IPL=ipl_tm
IPL=ipl_$(TARGET_FW)
DATADIR=../../installer/data



all:
	bin2c ipl_tm.bin $(DATADIR)/ipl_tm_bin.h ipl_tm
	bin2c $(IPL).bin $(DATADIR)/ipl_bin.h ipl
	sed -i "/int/d" $(DATADIR)/$(TM_IPL)_bin.h
	sed -i "/int/d" $(DATADIR)/ipl_bin.h

clean:
	rm -f $(DATADIR)/ipl_tm_bin.h $(DATADIR)/ipl_bin.h