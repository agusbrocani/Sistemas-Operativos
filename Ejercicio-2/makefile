# Nombre de los ejecutables (puedo poner el nombre que quiera, si lo cambio, tengo que volver a compilar make)
CLIENTE = cliente
SERVIDOR = servidor

# Archivos fuente
CLIENTE_SRCS = cliente.cpp
SERVIDOR_SRCS = servidor.cpp

# Archivos objeto

# Convierte automáticamente cada .cpp en su correspondiente .o (objeto)
# Sirve para compilar por partes y después enlazarlos todos en un solo ejecutable.
CLIENTE_OBJS = $(CLIENTE_SRCS:.cpp=.o)
SERVIDOR_OBJS = $(SERVIDOR_SRCS:.cpp=.o)

# Compilador y flags

# CXX define qué compilador usar (en este caso g++)
# CXXFLAGS define opciones de compilación:
# -Wall muestra todos los warnings (buena práctica).
# -pthread permite usar hilos (threads).
CXX = g++
CXXFLAGS = -Wall -pthread

# Regla principal

# Es la regla por defecto cuando corrés simplemente make.
# Hace que se compile el cliente y el servidor.
all: $(CLIENTE) $(SERVIDOR)

# tomar los .o de cliente y compilar el ejecutable cliente
$(CLIENTE): $(CLIENTE_OBJS)
	$(CXX) $(CXXFLAGS) -o $(CLIENTE) $(CLIENTE_OBJS)

# tomar los .o de servidor y compilar el ejecutable servidor
$(SERVIDOR): $(SERVIDOR_OBJS)
	$(CXX) $(CXXFLAGS) -o $(SERVIDOR) $(SERVIDOR_OBJS)

# Regla para limpiar los archivos temporales
# Borra todo lo generado (.o + ejecutables).
clean:
	rm -f $(CLIENTE) $(SERVIDOR) $(CLIENTE_OBJS) $(SERVIDOR_OBJS)

# make => compila todos los archivos
# make clean => elimina todos los .o y ejecutable
