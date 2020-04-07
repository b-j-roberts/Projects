COMPILER := clang++
CLANG_WARNS := #-Weverything -Wextra -Wno-c++98-compat -Wno-padded -Wno-global-constructors -Wno-switch-enum -Wno-shorten-64-to-32

VERSION := -std=c++1z
FLAGS := -O3
INC := -I ~/workspace/

LINKS := -pthread -lX11 -L ~/workspace/Harmony/ -lharmony-sfml -lsfml-graphics -lsfml-window -lsfml-system

k-nearest: k-nearest.o k_nearest_function.o k_near_gui.o
	${COMPILER} k-nearest.o -o k-nearest k_nearest_function.o k_near_gui.o ${FLAGS} ${LINKS}

k-nearest.o: k-nearest.cpp
	${COMPILER} -c k-nearest.cpp ${VERSION} ${FLAGS} ${INC} ${CLANG_WARNS}

k_near_gui.o: k_near_gui.cpp
	${COMPILER} -c k_near_gui.cpp ${VERSION} ${FLAGS} ${INC} ${CLANG_WARNS}

k_nearest_function.o: k_nearest_function.cpp
	${COMPILER} -c k_nearest_function.cpp ${VERSION} ${FLAGS} ${INC} ${CLANG_WARNS}

clean:
	rm k_nearest_function.o k_near_gui.o k-nearest k-nearest.o
