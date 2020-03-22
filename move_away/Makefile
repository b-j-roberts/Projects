COMPILER := clang++
CLANG_WARNS := -Weverything -Wextra -Wno-c++98-compat -Wno-padded -Wno-global-constructors -Wno-switch-enum -Wno-shorten-64-to-32

VERSION := -std=c++1z
FLAGS := -O3 -fPIC -ggdb
INC := -I ~/workspace/

LINKS := -pthread -lX11 -L ~/workspace/Harmony/ -lharmony-sfml -lsfml-graphics -lsfml-window -lsfml-system

move_away: move_away.o Components.o move_away_gui.o
	${COMPILER} move_away.o -o move_away Components.o move_away_gui.o ${FLAGS} ${LINKS}

move_away.o: move_away.cpp
	${COMPILER} -c move_away.cpp ${VERSION} ${FLAGS} ${INC} ${CLANG_WARNS}

move_away_gui.o: move_away_gui.cpp
	${COMPILER} -c move_away_gui.cpp  ${VERSION} ${FLAGS} ${INC} ${CLANG_WARNS}

Components.o: Components.cpp
	${COMPILER} -c Components.cpp ${VERSION} ${FLAGS} ${INC} ${CLANG_WARNS}

clean:
	rm move_away move_away.o move_away_gui.o Components.o
