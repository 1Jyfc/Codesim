codesim: main.cpp codesim.cpp codesim.h
	clang++ -g main.cpp codesim.cpp -lclang -o codesim

clean:
	rm codesim
