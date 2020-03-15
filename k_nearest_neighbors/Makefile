# NOTE : Change -I directory to location of Harmony lib & -L to location of Harmony static libs

k-nearest: k-nearest.o k_nearest_function.o k_near_gui.o
	g++ k-nearest.o -o k-nearest k_nearest_function.o k_near_gui.o -pthread -lX11 -lsfml-graphics -lsfml-window -lsfml-system -O3 -std=c++1z -L ~/workspace/Harmony -lharmony-sfml

k-nearest.o: k-nearest.cpp
	g++ -c k-nearest.cpp -O3 -std=c++1z -I ~/workspace/

k_near_gui.o: k_near_gui.cpp
	g++ -c k_near_gui.cpp -O3 -std=c++1z -I ~/workspace/

k_nearest_function.o: k_nearest_function.cpp
	g++ -c k_nearest_function.cpp -O3

clean:
	rm k_nearest_function.o k_near_gui.o k-nearest k-nearest.o
