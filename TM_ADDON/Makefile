INCLUDES		= includes
IMPORTS			= imports
WALLPAPERS		= wallpapers
MODULES			= modules
KPSPIDENT		= kpspident
PSPBTDNF		= pspbtdnf
VUNBRICKER		= vunbricker
VRManager		= vrmanager
DC500			= DC500
DC660			= DC660
DOC				= doc
RELEASE			= VResurrection_Manager

all:
	@mkdir $(INCLUDES) || true
	@mkdir $(INCLUDES)/$(WALLPAPERS) || true
	@mkdir $(INCLUDES)/$(MODULES) || true
	@mkdir $(INCLUDES)/$(KPSPIDENT) || true

	#build wallpapers
	@cd $(WALLPAPERS); make
	@mv $(WALLPAPERS)/wallpapers.h $(INCLUDES)/$(WALLPAPERS)

	#build modules
	@cd $(MODULES); make

	#build kpspident
	@cd $(KPSPIDENT); make
	@mv $(KPSPIDENT)/kpspident.h $(INCLUDES)/$(KPSPIDENT)

	#build pspbtdnf
	@cd $(PSPBTDNF); make
	@mv $(PSPBTDNF)/pspbtdnf.h $(INCLUDES)/$(MODULES)/$(DC500)
	@mv $(PSPBTDNF)/pspbtdnf_02g.h $(INCLUDES)/$(MODULES)/$(DC500)

	#build resurrection
	@cd $(VUNBRICKER); make
	@mv $(VUNBRICKER)/resurrection.h $(INCLUDES)/$(MODULES)/$(DC500)
	@mv $(VUNBRICKER)/vunbricker_en.h $(INCLUDES)/$(MODULES)/$(DC500)

	#build installer
	@cd $(VRManager); make

	@mkdir $(RELEASE) || true
	@mkdir $(RELEASE)/doc || true
	@mkdir $(RELEASE)/lang || true
	@mkdir $(RELEASE)/modules || true

	@cp $(DOC)/* $(RELEASE)/doc
	@cp $(VRManager)/lang/vrmanager_en.txt $(RELEASE)/lang
	@cp $(VRManager)/modules/* $(RELEASE)/modules
	@mv $(VRManager)/EBOOT.PBP $(RELEASE)/EBOOT.PBP
	
	@zip -r9 ./$(RELEASE).zip ./$(RELEASE)

clean:
	#clean includes
	@rm -rf $(INCLUDES)
	@rm -rf $(RELEASE)
	@rm -f $(RELEASE).zip

	#clean wallpapers
	make -C $(WALLPAPERS) clean;

	#clean kpspident
	make -C $(KPSPIDENT) clean;
	
	#clean pspbtdnf
	make -C $(PSPBTDNF) clean;

	#clean resurrection
	make -C $(VUNBRICKER) clean;

	#clean installer
	make -C $(VRManager) clean;