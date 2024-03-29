CXX = gcc
CXXFLAGS = -std=c99 -g -Wall -MMD
EXEC = server
OBJECTS = main.o logging.o safesockets.o
DEPENDS = ${OBJECTS:.o=.d}

${EXEC}: ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} -lm -o ${EXEC}

-include ${DEPENDS}

.PHONY: clean

clean:
	rm ${OBJECTS} ${EXEC} ${DEPENDS}
