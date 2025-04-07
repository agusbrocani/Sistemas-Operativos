SERVIDOR = servidor

SERVIDOR_SRCS = servidor.cpp funcionesYMetodosServidor.cpp

SERVIDOR_OBJS = $(SERVIDOR_SRCS:.cpp=.o)

CXX = g++
CXXFLAGS = -Wall -pthread

all: $(SERVIDOR)

$(SERVIDOR): $(SERVIDOR_OBJS)
	$(CXX) $(CXXFLAGS) -o $(SERVIDOR) $(SERVIDOR_OBJS)
	@echo "Se han compilado los archivos: $(SERVIDOR_SRCS)."

clean:
	rm -f $(SERVIDOR) $(SERVIDOR_OBJS)
	@echo "Se han eliminado los archivos: $(SERVIDOR) $(SERVIDOR_OBJS)."
