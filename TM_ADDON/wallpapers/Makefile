TARGET = wallpapers

all: clean
	bash build.sh
	bin2c $(TARGET).bmp $(TARGET).h $(TARGET)
	
	
clean:
	@rm -f *.h $(TARGET).bmp