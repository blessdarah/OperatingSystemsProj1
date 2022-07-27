p1: prog1.cpp my_const.h
	clang++ --std=gnu++17 -g prog1.cpp -o p1

p2: prog2.cpp my_const.h
	clang++ --std=gnu++17 -g prog2.cpp -o p2

matrix: matrix.cpp
	clang++ --std=gnu++17 -g matrix.cpp -o matrix

clean:
	rm p1 p2 matrix

zip:
	zip named_pipe.zip *.cpp *.h Makefile
