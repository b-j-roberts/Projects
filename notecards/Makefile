notecards: notecards.o
	clang++-9 notecards.o -o notecards -lsfml-graphics -lsfml-window -lsfml-system -pthread -lX11 -L ~/workspace/Harmony -lharmony-sfml -lharmony-basic -std=c++2a	

notecards.o: notecards.cpp
	clang++-9 -c notecards.cpp -o notecards.o -I ~/workspace/ -std=c++2a

clean:
	rm notecards notecards.o
