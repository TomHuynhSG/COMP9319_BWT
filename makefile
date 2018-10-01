all: bwtencode bwtsearch 

bwtsearch: bwtsearch.c
	gcc -Wall bwtsearch.c -o bwtsearch

bwtencode: bwtencode.c
	gcc -Wall bwtencode.c -o bwtencode

clean:
	rm bwtencode bwtsearch