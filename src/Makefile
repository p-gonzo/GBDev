CC	= ../libs/gbdk/bin/lcc -Wa-l -Wl-m -Wl-j -Wm-ys

all:
	mkdir -p ../builds/$(dir)
	$(CC) -o ../builds/$(dir)/$(dir).gb ./$(dir)/main.c
	cd ../builds/$(dir) &&\
	bgb $(dir).gb

build:
	mkdir -p ../builds/$(dir)
	$(CC) -o ../builds/$(dir)/$(dir).gb ./$(dir)/main.c

clean:
	rm -f *.o *.lst *.map *.gb *~ *.rel *.cdb *.ihx *.lnk *.sym *.asm *.noi