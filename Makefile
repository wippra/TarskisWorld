PROF=-pg
all: Makefile Tarski

Tarski: Makefile Tarskis\ World\ Version\ 2.c
	gcc -o3 $(PROF) -o Tarski Tarskis\ World\ Version\ 2.c
