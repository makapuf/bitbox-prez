#testkernel makefile
NAME = prez

GAME_C_FILES = main.c 
GAME_C_OPTS = -DVGAMODE_640_OVERCLOCK

USE_ENGINE=1

include $(BITBOX)/lib/bitbox.mk
all: slides.btc

slides.btc: prez.pdf
	mkdir -p build/slides
	convert -density 150 -limit memory 1024 $< build/slides/slides-%03d.png
	mogrify -filter Cosine -resize 640x -unsharp 0x1 build/slides/slides-???.png
	#convert -density 150  -limit memory 1024 -adaptive-resize 640x480  $< build/slides/slides-%03d.png
	for i in build/slides/slides-*?png; do python $(BITBOX)/scripts/btc4.py -d $$i ; done
	cat build/slides/slides-???.btc > slides.btc # ordre 


