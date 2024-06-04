CXXFLAGS = -std=c++20 -I${NIX_SRC}/src/libstore -I${NIX_SRC}/src/libutil -I${NIX_SRC}/src/libmain
LDFLAGS = -L${NIX_OUT}/lib/ -lnixstore -lnixutil -lnixmain

test: test.cc

install: test
	mkdir -p ${PREFIX}/bin
	cp test ${PREFIX}/bin/inputDrvs
