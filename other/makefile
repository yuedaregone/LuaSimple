LFLAGS+=-lm
LFLAGS+=-lpng


CORE_O= MImage.o DotMatrix.o
EXECUTE= DotMatrix

$(EXECUTE): DotMatrix.o MImage.o
	g++ *.o -o $(EXECUTE) $(LFLAGS)

clean:
	rm -f $(CORE_O) $(EXECUTE)

MImage.o: MImage.cpp MImage.h png.h
DotMatrix.o: DotMatrix.cpp MImage.h
