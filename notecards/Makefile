COMPILER := gcc
CLANG_WARNS := -Wextra -Wno-c++98-compat -Wno-padded -Wno-switch-enum -Wno-shorten-64-to-32 -Wno-weak-vtables -Wno-global-constructors -Wno-missing-prototypes -Wno-exit-time-destructors

VERSION := -std=c++2a
FLAGS := -O3 -ggdb
INC := -I ~/workspace/

LINKS := -pthread -lX11 -L ~/workspace/Harmony -lharmony-sfml -lharmony-basic -lsfml-graphics -lsfml-window -lsfml-system -lpthread -lstdc++

notecards: notecards.o notecards_gui.o components.o notecards_gui.h
	${COMPILER} notecards.o notecards_gui.o components.o -o notecards ${FLAGS} ${LINKS}

notecards.o: notecards.cpp
	${COMPILER} -c notecards.cpp -o notecards.o ${INC} ${VERSION} ${CLANG_WARNS}

notecards_gui.o: notecards_gui.cpp components.h
	${COMPILER} -c notecards_gui.cpp -o notecards_gui.o ${INC} ${VERSION} ${CLANG_WARNS}

components.o: components.cpp
	${COMPILER} -c components.cpp -o components.o ${INC} ${VERSION} ${CLANG_WARNS}

clean:
	rm notecards notecards.o notecards_gui.o components.o
