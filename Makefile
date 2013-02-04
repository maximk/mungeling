
.PHONY: default

default: mungeling

mungeling.o: mungeling.c
	$(CC) -c $< -o $@

sha1.o: sha1.c
	$(CC) -c $< -o $@

mungeling: mungeling.o sha1.o
	gcc -o mungeling $^

