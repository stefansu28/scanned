CC=g++
vpath %.cpp src
vpath %.h include

BUILDDIRS=build

all: build/scanned

build/scanned: build/main.o build/scanned.o | $(BUILDDIRS)
	$(CC) -lm build/*.o -o build/$(@F)

build/scanned.o: scanned.cpp scanned.h | $(BUILDDIRS)
	$(CC) -c $< -o build/$(@F)

build/main.o: main.cpp scanned.h | $(BUILDDIRS)
	$(CC) -c $< -o build/$(@F)

$(BUILDDIRS):
	mkdir $@

clean:
	rm -rf ./build/*
