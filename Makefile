CXXFLAGS = -std=c++20
LDFLAGS = -lnixstore -lnixutil -lnixmain

inputDrvs: inputDrvs.cc

install: inputDrvs
	mkdir -p ${PREFIX}/bin
	cp $^ ${PREFIX}/bin/
