#ifndef MACROS_H_INCLUDED
#define MACROS_H_INCLUDED

// ========================
// Parámetros de configuración
// ========================

// inicializarIPPuerto
#define TIPO_DE_IP_IPv4 AF_INET
#define TODA_INTERFAZ_LOCAL_DISPONIBLE INADDR_ANY
#define PUERTO_POR_DEFECTO 8080
#define BITS_DE_RELLENO 8

// abrirCanalDeComunicacionTCP
#define TIPO_DE_SOCKET_TCP SOCK_STREAM
#define PROTOCOLO_TCP IPPROTO_TCP

// aceptarConexiones
#define CANTIDAD_MAXIMA_CONEXIONES 5
#define VALOR_DE_CIERRE_SERVIDOR 0
#define CLIENTE_ATENDIDO 1
#define CLIENTE_NO_ATENDIDO 0

// manejarCliente
#define VALOR_DE_CIERRE_DE_CONEXION -1

// convertirIPBinariaACadena
#define CANTIDAD_CARACTERES_PARA_IPV_4 16

#endif // MACROS_H_INCLUDED
