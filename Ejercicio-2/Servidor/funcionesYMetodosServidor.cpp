#include "../bibliotecas/servidor/servidor.h"

Servidor::Servidor() {
    this->fileDescriptor = ERROR_ABRIR_CANAL_COMUNICACION_TCP;
}
Servidor::~Servidor() {
    cout << "Servidor destruido." << endl;
}

void Servidor::abrirCanalDeComunicacionTCP() {
    this->fileDescriptor = socket(TIPO_DE_IP_IPv4, TIPO_DE_SOCKET_TCP, PROTOCOLO_TCP);
    if (ERROR_ABRIR_CANAL_COMUNICACION_TCP == this->fileDescriptor) {
        throw runtime_error(string("Error al abrir canal: ") + strerror(errno));
    }
}
/// file descriptor del socket
/// puntero genérico a una estructura que contiene la dirección y puerto donde voy a vincular el socket
/// tamaño en bytes de la estructura
/// BIND => Le dice al SO: “este socket(fileDescriptor) va a escuchar conexiones o datos en esta IP:PUERTO”.
void Servidor::enlazarSocketAIPYPuerto(const tInfoDireccionSocket* infoDireccionSocket) const{
    // (struct sockaddr*) para infoDireccionSocket
    int estadoDeEnlace = bind(this->fileDescriptor, (struct sockaddr*)infoDireccionSocket, sizeof(tInfoDireccionSocket));
    if (ERROR_ENLAZAR_SOCKET == estadoDeEnlace) {
        throw runtime_error(string("Error al enlazar el socket: ") + strerror(errno));
    }
}

void inicializarIPPuerto(tInfoDireccionSocket* infoDireccionSocket) {
    infoDireccionSocket->tipoDeIP = TIPO_DE_IP_IPv4;
    infoDireccionSocket->direccionIP = TODA_INTERFAZ_LOCAL_DISPONIBLE;
    infoDireccionSocket->puerto = htons(PUERTO_POR_DEFECTO);
    memset(infoDireccionSocket->rellenoParaCompatibilidad, 0, BITS_DE_RELLENO);
}
