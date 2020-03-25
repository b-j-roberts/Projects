COMPILER := clang++
CLANG_WARNS := -Weverything -Wextra -Wno-c++98-compat -Wno-padded -Wno-global-constructors -Wno-switch-enum -Wno-shorten-64-to-32

VERSION := -std=c++1z
FLAGS := -O3 -fPIC
INC := -I ~/workspace/

LINKS := -pthread -lX11 -L ~/workspace/Harmony/ -lharmony-sfml -lsfml-graphics -lsfml-window -lsfml-system

charged-particles: charged-particles.o components.o charged_particles_gui.o
	${COMPILER} charged-particles.o -o charged-particles components.o charged_particles_gui.o ${FLAGS} ${LINKS}

charged-particles.o: charged-particles.cpp
	${COMPILER} -c charged-particles.cpp ${VERSION} ${FLAGS} ${INC} ${CLANG_WARNS}

charged_particles_gui.o: charged_particles_gui.cpp
	${COMPILER} -c charged_particles_gui.cpp  ${VERSION} ${FLAGS} ${INC} ${CLANG_WARNS}

components.o: components.cpp
	${COMPILER} -c components.cpp ${VERSION} ${FLAGS} ${INC} ${CLANG_WARNS}

clean:
	rm charged-particles charged-particles.o charged_particles_gui.o components.o
