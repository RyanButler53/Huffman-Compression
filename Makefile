CXXFLAGS = -std=c++2a -Wall -Wextra -pedantic -g -O3
CXX = clang++

TARGETS = compress uncompress

all: $(TARGETS)

compress: huffmanNode.o encoder.o compress.o
	$(CXX) -o $@ $^

uncompress: huffmanNode.o decoder.o uncompress.o
	$(CXX) -o $@ $^

compress.o: compress.cpp huffmanNode.hpp encoder.hpp
	$(CXX) $(CXXFLAGS) -c $<

uncompress.o: uncompress.cpp huffmanNode.hpp decoder.hpp
	$(CXX) $(CXXFLAGS) -c $<

huffmanNode.o: $@(.o=.*pp)
	$(CXX) $(CXXFLAGS) -c $@(.o=.cpp)

decoder.o: decoder.cpp decoder.hpp huffmanNode.hpp
	$(CXX) $(CXXFLAGS) -c $<

encoder.o: encoder.cpp encoder.hpp huffmanNode.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -rf *.o $(TARGETS)

cleancomp:
	rm *.compress.codes
	rm *.compress
	rm *.uncompress
	