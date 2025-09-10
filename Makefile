dict1: dict1.o dictionary.o read.o bit.o patricia.o editdist.o metrics.o a2data.o
	gcc -Wall -o dict1 dict1.o dictionary.o read.o bit.o patricia.o editdist.o metrics.o a2data.o -g

dict1.o: dict1.c dictionary.h read.h
	gcc -Wall -o dict1.o dict1.c -g -c

dictionary.o: dictionary.c dictionary.h record_struct.h bit.h patricia.h metrics.h a2data.h
	gcc -Wall -o dictionary.o dictionary.c -g -c

read.o: read.c read.h record_struct.h
	gcc -Wall -o read.o read.c -g -c

bit.o: bit.c bit.h
	gcc -Wall -o bit.o bit.c -g -c

# Stage 2 Patricia
dict2: dict2.o patricia.o editdist.o metrics.o a2data.o read.o bit.o dictionary.o
	gcc -Wall -o dict2 dict2.o patricia.o editdist.o metrics.o a2data.o read.o bit.o dictionary.o -g

dict2.o: dict2.c patricia.h editdist.h metrics.h a2data.h read.h dictionary.h
	gcc -Wall -o dict2.o dict2.c -g -c

patricia.o: patricia.c patricia.h metrics.h a2data.h editdist.h bit.h
	gcc -Wall -o patricia.o patricia.c -g -c

editdist.o: editdist.c editdist.h
	gcc -Wall -o editdist.o editdist.c -g -c

metrics.o: metrics.c metrics.h
	gcc -Wall -o metrics.o metrics.c -g -c

a2data.o: a2data.c a2data.h record_struct.h
	gcc -Wall -o a2data.o a2data.c -g -c

