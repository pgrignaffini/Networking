CXX=g++
CXXFLAGS= -std=c++11
DEPS = Generator.hpp
OBJ =  main.o Generator.o

%.c: %.o $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

main: $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS)

.PHONY : clean
clean :
	     -rm $(OBJ)
