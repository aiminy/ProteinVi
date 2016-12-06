CC	=g++
CFLAGS 	=-Wall -ggdb3

DIR	=/root/fltk-1.1.6
INC	=-I${DIR}
LIB	=-L${DIR}/lib ${LINKFLTKGL} ${LDLIBS} ${GLDLIBS} ${IMAGELIBS} -L/usr/lib -lglut ${LINKFLTKIMG}

LDLIBS		=  -L/usr/X11R6/lib  -lm  -lXext -lX11  -lsupc++
GLDLIBS		=  -L/usr/X11R6/lib  -lGLU -lGL -lm  -lXext -lX11  -lsupc++
LINKFLTK	= -L${DIR}/lib -lfltk
LINKFLTKGL	= -L${DIR}/lib -lfltk_gl -lfltk
LINKFLTKFORMS	= -L${DIR}/lib -lfltk_forms -lfltk
LINKFLTKIMG	= -L${DIR}/lib -lfltk_images -lfltk $(IMAGELIBS)
LINKSHARED	=  -L${DIR}/lib -lfltk_images -lfltk_forms -lfltk $(IMAGELIBS)
IMAGELIBS	= -lpng -lz -ljpeg 

clean:
	rm -f *~ *.o main main1

.cpp.o:
	${CC} ${CFLAGS} ${INC} -c -o $@ $<


main1:main1.o MyWindow.o draw.o pdb_parser.o imageio.o
	${CC} -o $@ $< MyWindow.o draw.o pdb_parser.o imageio.o ${LIB}
