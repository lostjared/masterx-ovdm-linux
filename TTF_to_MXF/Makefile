mxfont_fix: mxfont_fix.cc
	g++ mxfont_fix.cc -o mxfont_fix `sdl-config --cflags --static-libs` -lSDL_ttf -lfreetype -lz -lbz2
all: mxfont_fix

clean: 
	rm -f mxfont_fix
