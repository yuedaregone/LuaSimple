LFLAGS+=-lpng
LFLAGS+=-lz

PNG_PATH= /home/yue/workspace/MyLibs/libpng-libpng16
ZLIB_PATH = /home/yue/workspace/MyLibs/zlib-1.2.8/

CORE_O= MImage.o DotMatrix.o
EXECUTE= DotMatrix

$(EXECUTE): DotMatrix.o MImage.o
	g++ -L$(PNG_PATH) -L$(ZLIB_PATH) $(CORE_O) -o $(EXECUTE) $(LFLAGS)

clean:
	rm -f $(CORE_O) $(EXECUTE)

.cpp.o:
	$(CC) -I$(PNG_PATH) -c -o $@ $<

MImage.o: MImage.cpp MImage.h	
DotMatrix.o: DotMatrix.cpp MImage.h

