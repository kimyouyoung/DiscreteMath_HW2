dnf : dnf_converter.o
	gcc -o dnf dnf_converter.o

dnf_converter.o : dnf_converter.c
	gcc -c -o dnf_converter.o dnf_converter.c

clean :
	rm *.o dnf