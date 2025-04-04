
warns = -Wall -Wextra -pedantic
std = -std=c++11
cc = g++
cflags = $(std) $(warns)

all: VaaToCpp

VaaToCpp: VaaToCpp.o tokenizing.o parsing.o
	${cc} ${cflags} $< tokenizing.o parsing.o -o $@

VaaToCpp.o: VaaToCpp.cpp tokenizing.h parsing.h
	${cc} ${cflags} -c $<

tokenizing.o: tokenizing.cpp tokenizing.h
	${cc} ${cflags} -c $<

parsing.o: parsing.cpp parsing.h tokenizing.h
	${cc} ${cflags} -c $<

clean:
	rm -f VaaToCpp.o tokenizing.o parsing.o VaaToCpp

