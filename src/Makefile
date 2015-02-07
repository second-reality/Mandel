CC=gcc
CFLAGS=-Wall -O3 
LDFLAGS=-lm -lSDL -lpthread  
EXEC=mandel
OBJS=args.o gfx.o main.o mandelbrot.o options.o

all: $(EXEC)

mandel: $(OBJS) 
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c %.h
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o 

mrproper: 
	rm -rf $(EXEC) *.bmp *.o 
