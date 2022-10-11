.PHONEY: generate clean

generate:
	g++ -c main.cpp
	g++ main.o -o pathfinder -lsfml-graphics -lsfml-window -lsfml-system

clean:
	rm *.o
