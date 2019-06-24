CC=g++
CFLAGS=-I.
DEPS = Big_Int.hpp
OBJ =  main.o Big_Int.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY : clean
clean :
	     -rm $(OBJ)  
