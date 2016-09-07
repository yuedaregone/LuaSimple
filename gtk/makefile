include config
FILENAME= animated-revealing

blur-performance.exe: blur-performance.o
	$(CC) blur-performance.o gtkcairoblur.o -o blur-performance.exe $(CLIBS)

flicker.exe: flicker.o
	$(CC) flicker.o -o flicker.exe $(CLIBS)
	
animated-resizing.exe: animated-resizing.o
	$(CC) animated-resizing.o frame-stats.o variable.o -o animated-resizing.exe $(CLIBS)

animated-revealing.exe: animated-revealing.o
	$(CC) animated-revealing.o frame-stats.o variable.o -o animated-revealing.exe $(CLIBS)

	
clean:
	rm -f *.o

animated-revealing.o: animated-revealing.c frame-stats.h frame-stats.o
animated-resizing.o: animated-resizing.c frame-stats.h frame-stats.o
frame-stats.o: variable.o variable.h frame-stats.c
variable.o: variable.h variable.c
flicker.o: flicker.c
blur-performance.o: gtkcairoblur.o blur-performance.c gtkcairoblurprivate.h
gtkcairoblur.o: gtkcairoblur.c gtkcairoblurprivate.h