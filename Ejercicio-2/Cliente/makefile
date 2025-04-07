CLIENTE = cliente

CLIENTE_SRCS = cliente.cpp funcionesYMetodosCliente.cpp

CLIENTE_OBJS = $(CLIENTE_SRCS:.cpp=.o)

CXX = g++
CXXFLAGS = -Wall -pthread

all: $(CLIENTE)

$(CLIENTE): $(CLIENTE_OBJS)
	$(CXX) $(CXXFLAGS) -o $(CLIENTE) $(CLIENTE_OBJS)
	@echo "Se han compilado los archivos: $(CLIENTE_SRCS)."

clean:
	rm -f $(CLIENTE)$(CLIENTE_OBJS)
	@echo "Se han eliminado los archivos: $(CLIENTE) $(CLIENTE_OBJS)."

