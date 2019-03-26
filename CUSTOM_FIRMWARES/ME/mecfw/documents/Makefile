all:
	python2.7 readme.py $(VER) $(TARGET_FW)

clean:
	rm -f *_lme_readme_*.txt *_me_readme_*.txt

ifndef $(VER)
VER = lite
endif

ifndef $(TARGET_FW)
TARGET_FW = 150
endif

PSPSDK = $(shell psp-config --pspsdk-path)
