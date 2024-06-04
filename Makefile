CXXFLAGS = -std=c++20 -DSYSTEM='"${SYSTEM}"'
LDFLAGS = -lnixstore -lnixutil -lnixmain

inputDrvs: inputDrvs.cc

install: inputDrvs
	mkdir -p ${PREFIX}/bin
	cp $^ ${PREFIX}/bin/
