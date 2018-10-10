BUILD_FOLD=build
PROF=-pg

.PHONY: all

all: Makefile $(BUILD_FOLD) tarski

tarski: Makefile $(BUILD_FOLD)/tarski.o $(BUILD_FOLD)/bn.o
	gcc -o3 $(PROF) -o tarski $(BUILD_FOLD)/tarski.o $(BUILD_FOLD)/bn.o
$(BUILD_FOLD)/tarski.o: Makefile Tarskis\ World\ Version\ 2.c
	gcc -o3 $(PROF) -o $(BUILD_FOLD)/tarski.o -c Tarskis\ World\ Version\ 2.c
$(BUILD_FOLD)/bn.o: Makefile bn.c bn.h
	gcc -o3 $(PROF) -o $(BUILD_FOLD)/bn.o -c bn.c
$(BUILD_FOLD): Makefile
	mkdir -p $(BUILD_FOLD)
