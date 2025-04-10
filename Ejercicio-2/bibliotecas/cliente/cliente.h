#ifndef CLIENTE_H_INCLUDED
#define CLIENTE_H_INCLUDED
#include <iostream>
#include <exception>

#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <sys/socket.h> // socket(), AF_INET
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif

//#include <string.h> // memset()
#include <cerrno>   // errno
#include <cstring>  // biblioteca de string

/// ERRORES
#define ERROR_ABRIR_CANAL_COMUNICACION_TCP -1
#define ID_INVALIDO -1
#define ERROR_CONECTAR_AL_SERVIDOR -1

/// PASO 1
#define TIPO_DE_IP_IPv4 AF_INET
#define TIPO_DE_SOCKET_TCP SOCK_STREAM
#define PROTOCOLO_TCP IPPROTO_TCP

///SOCKET ORIENTADO A CONEXIÓN(SOCKET STREAM) => TCP
#define IP_SERVIDOR "127.0.0.1"
#define PUERTO_SERVIDOR 8080

using namespace std;

#define BITS_DE_RELLENO 8
typedef struct {
    short tipoDeIP;
    uint32_t direccionIPServidor;
    uint16_t puertoServidor;
    char rellenoParaCompatibilidad[BITS_DE_RELLENO];
} tInfoDireccionSocket;

void inicializarIPPuerto(tInfoDireccionSocket* infoDireccionSocket);

class Cliente {
    private:
        int fileDescriptor;
        int idAsignadoPorServidor;

    public:
        Cliente();
        int getFileDescriptor() const;
        void setIDAsignadoPorServidor(int idAsignadoPorServidor);
        ~Cliente();

        void abrirCanalDeComunicacionTCP();

        void conectarAlServidor(const tInfoDireccionSocket* infoDireccionSocket) const;

        void cerrarConexion();
};

/**
    cd /mnt/c/Users/PC/Desktop/Sistemas-Operativos/Ejercicio-2
    cambiar directorio a una carpeta específica en disco C: de Windows, pero vista desde Ubuntu (WSL).
    /mnt/c/: En WSL, así se accede al disco C: de Windows.
*/

#endif // CLIENTE_H_INCLUDED
