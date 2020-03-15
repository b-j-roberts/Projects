move_away: move_away.o Components.o move_away_gui.o
	g++ move_away.o -o move_away Components.o move_away_gui.o -O3 -L ~/workspace/Harmony/ -lharmony-basic -lharmony-sfml -pthread -lX11 -lsfml-graphics -lsfml-window -lsfml-system

move_away.o: move_away.cpp
	g++ -c move_away.cpp -std=c++1z -I ~/workspace/ -O3

move_away_gui.o: move_away_gui.cpp
	g++ -c move_away_gui.cpp -std=c++1z -I ~/workspace/ -O3

Components.o: Components.cpp
	g++ -c Components.cpp -std=c++1z -I ~/workspace/ -O3

clean:
	rm move_away move_away.o move_away_gui.o Components.o
