#ifndef SERVIDOR_H_INCLUDED
#define SERVIDOR_H_INCLUDED

#include "macros.h"
#include "errores.h"
#include "metodos.h"

using namespace std; //mutex

typedef struct {
    short tipoDeIP;
    uint32_t direccionIP;
    uint16_t puerto;
    char rellenoParaCompatibilidad[BITS_DE_RELLENO];
} tInfoDireccionSocket;

class Servidor {
    private:
        int fileDescriptor;
        int conexionesActivas;
        mutable mutex mutexConexiones;

        int aceptarConexionConCliente(sockaddr_in& direccionCliente, socklen_t& tamanioDireccion);
        void incrementarConexiones();
        void decrementarConexiones();
        int obtenerCantidadConexiones() const;
        void manejarCliente(int descriptorCliente);
        void enviarMensajeACliente(int socket, const string& mensaje) const;
        void enviarMensajeDeBienvenida(int descriptorCliente) const;
        int recibirMensaje(int socket, char* buffer, size_t tamanio) const;
        string generarRespuesta(int valor, const char* original);
        int manejarColaDeClientes(int descriptorCliente);
        void cerrarConexion(int descriptorCliente);

    public:
        Servidor();
        void enlazarSocketAIPYPuerto(const tInfoDireccionSocket* infoDireccionSocket) const;
        ~Servidor();
        void abrirCanalDeComunicacionTCP();
        void escucharConexionesEntrantes() const;
        void aceptarConexiones();
};

#endif // SERVIDOR_H_INCLUDED
