# Cliente-Servidor Chat

Este proyecto implementa un sistema de chat cliente-servidor en Java.

## Estructura del Proyecto

El proyecto contiene dos implementaciones del mismo sistema:

### Implementación Java
- `Server.java`: Servidor principal que maneja las conexiones
- `ClientHandler.java`: Manejador de clientes individuales
- `ConsoleClient.java`: Cliente de consola

## Características

- Máximo N clientes activos simultáneamente
- Cola de espera para M clientes adicionales
- Reintento automático para clientes rechazados
- Mensajes con emojis para indicar estado
- Salida fácil con ESC o comando "exit"

## Compilación y Ejecución

### Java
```bash
# Compilar
javac Server.java ClientHandler.java ConsoleClient.java

# Ejecutar servidor
java Server

# Ejecutar cliente (en otra terminal)
java ConsoleClient
```