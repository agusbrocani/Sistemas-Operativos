#include "../bibliotecas/cliente/cliente.h"

//Para LINUX todo es un archivo, por ende, le asigna un numero al recurso.
// socket devuelve file descriptor (FD) ó -1
/**
* @brief Abre un canal de comunicacion mediante un socket TCP.
*/

Cliente::Cliente() {
    this->fileDescriptor = ERROR_ABRIR_CANAL_COMUNICACION_TCP;
    this->idAsignadoPorServidor = ID_INVALIDO;
}
Cliente::~Cliente() {
    cout << "Cliente destruido." << endl;
}

int Cliente::getFileDescriptor() const {
    return this->fileDescriptor;
};

void Cliente::setIDAsignadoPorServidor(int idAsignadoPorServidor) {
    this->idAsignadoPorServidor = idAsignadoPorServidor;
}

void Cliente::abrirCanalDeComunicacionTCP() {
    this->fileDescriptor = socket(TIPO_DE_IP_IPv4, TIPO_DE_SOCKET_TCP, PROTOCOLO_TCP);
    if (ERROR_ABRIR_CANAL_COMUNICACION_TCP == this->fileDescriptor) {
        throw runtime_error(string("Error al abrir canal de comunicacion TCP: ") + strerror(errno));
    }
}

void Cliente::conectarAlServidor(const tInfoDireccionSocket* infoDireccionSocket) const {
    int estadoDeEnlace = connect(this->fileDescriptor, (struct sockaddr*)infoDireccionSocket, sizeof(tInfoDireccionSocket));
    if (ERROR_CONECTAR_AL_SERVIDOR == estadoDeEnlace) {
        throw runtime_error(string("Error al conectarse al servidor: ") + strerror(errno));
    }
}

void Cliente::cerrarConexion() {
    #ifdef _WIN32
        closesocket(this->fileDescriptor);
    #else
        close(this->fileDescriptor);
    #endif
}

void inicializarIPPuerto(tInfoDireccionSocket* infoDireccionSocket) {
    infoDireccionSocket->tipoDeIP = TIPO_DE_IP_IPv4;
    infoDireccionSocket->direccionIPServidor = inet_addr(IP_SERVIDOR);
    infoDireccionSocket->puertoServidor = htons(PUERTO_SERVIDOR);
    memset(infoDireccionSocket->rellenoParaCompatibilidad, 0, BITS_DE_RELLENO);
}
