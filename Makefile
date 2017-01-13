CFLAGS = -std=c++11 -DFUSE_USE_VERSION=26 `pkg-config fuse --cflags` -I../../azure-storage-cpp/Microsoft.WindowsAzure.Storage/includes -I../../casablanca/Release/include
LINKFLAGS = -Wall `pkg-config fuse --libs` -L../../azure-storage-cpp/Microsoft.WindowsAzure.Storage/build.release/Binaries -L../../casablanca/Release/build.release/Binaries -lcpprest -lazurestorage -lboost_system -lssl -lcrypto

all: bin/azurefs

clean:
	rm -rf bin obj

bin: 
	mkdir -p bin

bin/azurefs: bin obj/azurefs.o obj/wrap.o obj/main.o
	g++ -g -o bin/azurefs obj/* $(LINKFLAGS)

obj:
	mkdir -p obj

obj/azurefs.o: obj azurefs.cc azurefs.hh
	g++ -g $(CFLAGS) -c azurefs.cc -o obj/azurefs.o

obj/main.o: obj main.c wrap.hh
	gcc -g $(CFLAGS) -c main.c -o obj/main.o

obj/wrap.o: obj wrap.cc wrap.hh azurefs.hh
	g++ -g $(CFLAGS) -c wrap.cc -o obj/wrap.o

