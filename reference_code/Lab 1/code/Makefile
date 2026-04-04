#target: dependencies
#	action

all: area perimeter

area: area.o length.o
	gcc area.o length.o -o area

area.o: area.c length.h
	gcc -c area.c

length.o: length.c
	gcc -c length.c

perimeter: perimeter.o length.o
	gcc perimeter.o length.o -o perimeter

perimeter.o: perimeter.c length.h
	gcc -c perimeter.c

clean:
	rm *.o area perimeter
