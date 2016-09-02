LFLAGS+=-lpng
LFLAGS+=-lwindow



CORE_O= MImage.o DotMatrix.o
EXECUTE= DotMatrix

all: $(EXECUTE)

clean:
	rm -f $(CORE_O) $(EXECUTE)

MImage.o: MImage.cpp MImage.h png.h
DotMatrix.o: DotMatrix.cpp MImage.h

.cpp.o:
	$(CC) -Iinclude -c $*.cpp -o $*.o -Wall $(CFLAGS)

$(EXECUTE): DotMatrix.o MImage.o
	$(CC) -Iinclude *.o -o $(EXECUTE) $(LFLAGS) -Iinclude -lpng -lz