#ifndef SERVIDOR_H_INCLUDED
#define SERVIDOR_H_INCLUDED
#include <iostream>

#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <sys/socket.h> // socket(), bind(), AF_INET
    #include <netinet/in.h>
    #include <arpa/inet.h>  // htons(), INADDR_ANY
    #include <unistd.h>
#endif

#include <string.h> // memset()
#include <cerrno>   // errno
#include <cstring>  // biblioteca de string

using namespace std;

/// ERRORES
    // errores paso 1
#define ERROR_ABRIR_CANAL_COMUNICACION_TCP -1
#define ID_INVALIDO -1
    // errores paso 2
#define ERROR_ENLAZAR_SOCKET -1

/// PASO 1
#define TIPO_DE_IP_IPv4 AF_INET
#define TIPO_DE_SOCKET_TCP SOCK_STREAM
#define PROTOCOLO_TCP IPPROTO_TCP
/// PASO 2
#define TODA_INTERFAZ_LOCAL_DISPONIBLE INADDR_ANY
#define PUERTO_POR_DEFECTO 8080

#define BITS_DE_RELLENO 8
typedef struct {
    short tipoDeIP;
    uint32_t direccionIP;
    uint16_t puerto;
    char rellenoParaCompatibilidad[BITS_DE_RELLENO];
} tInfoDireccionSocket;

void inicializarIPPuerto(tInfoDireccionSocket* infoDireccionSocket);

class Servidor {
    private:
        int fileDescriptor;
    public:
        Servidor();
        void enlazarSocketAIPYPuerto(const tInfoDireccionSocket* infoDireccionSocket) const;
        ~Servidor();
        void abrirCanalDeComunicacionTCP();
};

#endif // SERVIDOR_H_INCLUDED
