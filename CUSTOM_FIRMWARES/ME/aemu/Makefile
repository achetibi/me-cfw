VERMAX	= 1
VERMIN	= 0
PREV	= ..

PSPNET_ADHOC_AUTH		= pspnet_adhoc_auth
PSPNET_ADHOC			= pspnet_adhoc
PSPNET_ADHOCCTL			= pspnet_adhocctl
PSPNET_ADHOC_MATCHING	= pspnet_adhoc_matching
PSPNET_ADHOC_DOWNLOAD	= pspnet_adhoc_download
PSPNET_ADHOC_DISCOVER	= pspnet_adhoc_discover
PSPNET_ADHOCCTL_SERVER	= pspnet_adhocctl_server
PSPNET_MINIUPNC			= pspnet_miniupnc
#CONTRIBUTIONS			= contributions

DIST		= dist
ATPRO		= atpro
MEMAB		= memab
RELFL		= release_XXXonline

CFWSRC		= $(PREV)/mecfw
DOCS		= $(DIST)/docs
SEPLUGINS	= $(DIST)/seplugins
KD			= $(DIST)/kd
SERVER		= $(DIST)/server
RELDIR		= $(DIST)/$(RELFL)

ifeq ($(CONFIG_620), 1)
FLAGS = CONFIG_620=1
RELFL = release_620online
endif

ifeq ($(CONFIG_63X), 1)
FLAGS = CONFIG_63X=1
RELFL = release_639online
endif

ifeq ($(CONFIG_660), 1)
FLAGS = CONFIG_660=1
RELFL = release_660online
endif

ifeq ($(RELEASE), 1)
FLAGS += RELEASE=1
else
FLAGS += DEBUG=1

ifeq ($(ENABLE_LOGGER), 1)
FLAGS += ENABLE_LOGGER=1
endif

ifeq ($(TRACE), 1)
FLAGS += TRACE=1
endif

endif

ifeq ($(ENABLE_PEERLOCK), 1)
FLAGS += ENABLE_PEERLOCK=1
endif

ifeq ($(ENABLE_NETLOCK), 1)
FLAGS += ENABLE_NETLOCK=1
endif

ifeq ($(PDP_DIRTY_MAGIC), 1)
FLAGS += PDP_DIRTY_MAGIC=1
endif

ifeq ($(BROADCAST_TO_LOCALHOST), 1)
FLAGS += BROADCAST_TO_LOCALHOST=1
endif

ifeq ($(LOCALHOST_AS_PEER), 1)
FLAGS += LOCALHOST_AS_PEER=1
endif

all:
	@mkdir $(DIST) || true
	@mkdir $(RELDIR) || true
	@mkdir $(DOCS) || true
	@mkdir $(SEPLUGINS) || true
	@mkdir $(KD) || true
	@mkdir $(SERVER) || true
	@touch $(SEPLUGINS)/hotspot.txt || true
#	@cp -r $(CONTRIBUTIONS)/PSP $(DIST)
	@echo pro.coldbird.net > $(SEPLUGINS)/server.txt
#	@echo home.coldbird.uk.to > $(SEPLUGINS)/server.txt
#	@echo 192.168.1.103 > $(SEPLUGINS)/server.txt
#	@touch $(SEPLUGINS)/server.txt || true

	@cd $(CFWSRC); make build_lib
	@cd $(ATPRO); make $(FLAGS)
	@cd $(MEMAB); make $(FLAGS)
	@cd $(PSPNET_ADHOC_AUTH); make $(FLAGS)
	@cd $(PSPNET_ADHOC); make $(FLAGS)
	@cd $(PSPNET_ADHOCCTL); make $(FLAGS)
	@cd $(PSPNET_ADHOC_MATCHING); make $(FLAGS)
#	@cd $(PSPNET_ADHOC_DOWNLOAD); make $(FLAGS)
#	@cd $(PSPNET_ADHOC_DISCOVER); make $(FLAGS)
	@cd $(PSPNET_ADHOCCTL_SERVER); make $(FLAGS)
	@cd $(PSPNET_MINIUPNC); make $(FLAGS)
	@mv $(DOCS) $(RELDIR)
	@mv $(SEPLUGINS) $(RELDIR)
	@mv $(KD) $(RELDIR)
	@mv $(SERVER) $(RELDIR)
	@cd $(DIST); zip -r -9 "../../$(RELFL)$(VERMAX).$(VERMIN).zip" "$(RELFL)";

clean:
	@cd $(ATPRO); make clean $(FLAGS)
	@cd $(MEMAB); make clean $(FLAGS)
	@cd $(PSPNET_ADHOC_AUTH); make clean $(FLAGS)
	@cd $(PSPNET_ADHOC); make clean $(FLAGS)
	@cd $(PSPNET_ADHOCCTL); make clean $(FLAGS)
	@cd $(PSPNET_ADHOC_MATCHING); make clean $(FLAGS)
#	@cd $(PSPNET_ADHOC_DOWNLOAD); make clean $(FLAGS)
#	@cd $(PSPNET_ADHOC_DISCOVER); make clean $(FLAGS)
	@cd $(PSPNET_ADHOCCTL_SERVER); make clean $(FLAGS)
	@cd $(PSPNET_MINIUPNC); make clean $(FLAGS)
	@rm -rf $(DIST) || true
