dict1: dict1.o dictionary.o read.o bit.o
	gcc -Wall -o dict1 dict1.o dictionary.o read.o bit.o -g

dict1.o: dict1.c dictionary.h read.h
	gcc -Wall -o dict1.o dict1.c -g -c

# Note that because we #include record_struct.c we need it in our dependencies.
dictionary.o: dictionary.c dictionary.h record_struct.c record_struct.h bit.h
	gcc -Wall -o dictionary.o dictionary.c -g -c

read.o: read.c read.h record_struct.c record_struct.h
	gcc -Wall -o read.o read.c -g -c

bit.o: bit.c bit.h
	gcc -Wall -o bit.o bit.c -g -c
