.PHONY: clean

target: main.o tpl_algorithm.o tpl_circuit.o
	g++ -o target -lboost_system -lboost_filesystem main.o tpl_algorithm.o tpl_circuit.o \
	../bookshelf/bookshelf_net.o ../bookshelf/bookshelf_node.o ../bookshelf/bookshelf_pl.o

main.o:
	g++ -c main.cpp

tpl_algorithm.o: tpl_algorithm.h tpl_algorithm.cpp
	g++ -c tpl_algorithm.cpp

tpl_circuit.o: tpl_circuit.h tpl_circuit.cpp
	g++ -c tpl_circuit.cpp

clean:
	rm *.o