#include "../bibliotecas/servidor/servidor.h"
#include "../bibliotecas/servidor/socketHelper.h"

Servidor::Servidor() {
    this->fileDescriptor = ERROR_ABRIR_CANAL_COMUNICACION_TCP;
    this->conexionesActivas = 0;
}

Servidor::~Servidor() {
    cout << SocketHelper::obtenerHoraActual() <<  "Servidor destruido." << endl;
}

void Servidor::abrirCanalDeComunicacionTCP() {
    #ifdef _WIN32
    WSADATA wsaData;
    int resultado = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (resultado != 0) {
        throw runtime_error("Error al inicializar Winsock");
    }
    #endif
    this->fileDescriptor = socket(TIPO_DE_IP_IPv4, TIPO_DE_SOCKET_TCP, PROTOCOLO_TCP);
    if (ERROR_ABRIR_CANAL_COMUNICACION_TCP == this->fileDescriptor) {
        throw runtime_error(string("Error al abrir canal"));
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


void Servidor::escucharConexionesEntrantes() const {
    int estadoDeConexion = listen(this->fileDescriptor, CANTIDAD_MAXIMA_CONEXIONES);
    if (ERROR_ESCUCHAR_CONEXIONES == estadoDeConexion) {
        throw runtime_error(string("Error al escuchar conexiones: ") + strerror(errno));
    }
    cout << SocketHelper::obtenerHoraActual() << "[Servidor] Escuchando conexiones en el puerto "
    << PUERTO_POR_DEFECTO << endl;
    cout << SocketHelper::obtenerHoraActual() << "[Servidor] Cantidad de conexiones activas: "
    << this->obtenerCantidadConexiones() << endl;
}


// Uso sockaddr_in en vez de tInfoDireccionSocket porque su layout en memoria no es exactamente igual al
// de sockaddr_in y el sist no sabe como llenarla
// accept() espera (bloquea) hasta que un cliente se conecte, y devuelve un nuevo file descriptor
// (un número entero) que representa la conexión con ese cliente
void Servidor::aceptarConexiones() {

    struct sockaddr_in direccionCliente;
    socklen_t tamanioDireccion = sizeof(direccionCliente);
    do {
        int descriptorCliente = this->aceptarConexionConCliente(direccionCliente, tamanioDireccion);
        if (ERROR_CONECTAR_CON_CLIENTE != descriptorCliente) {
            int statusDeAtencion = this->manejarColaDeClientes(descriptorCliente);
            if (CLIENTE_ATENDIDO == statusDeAtencion) {
                this->incrementarConexiones();
            }

            cout << SocketHelper::obtenerHoraActual() << "[Servidor] Cliente conectado desde IP: "
                 << SocketHelper::convertirIPBinariaACadena(direccionCliente.sin_addr)
                 << ", puerto: " << SocketHelper::convertirPuertoACadena(direccionCliente.sin_port) << endl;

            // Lanzar thread para manejar al cliente
            thread([this, descriptorCliente]() {
                this->manejarCliente(descriptorCliente);
            }).detach();
        }

    } while (this->obtenerCantidadConexiones() > VALOR_DE_CIERRE_SERVIDOR);
}

/// Métodos privados

// lock: Clase de RAII que bloquea un mutex automáticamente cuando se crea.
// Y lo desbloquea automáticamente cuando sale del scope (cuando termina la función o bloque {})
// Esto evita olvidarse de llamar a unlock() y es más seguro
void Servidor::incrementarConexiones() {
    lock_guard<mutex> lock(mutexConexiones);
    this->conexionesActivas++;
}

void Servidor::decrementarConexiones() {
    lock_guard<mutex> lock(mutexConexiones);
    this->conexionesActivas--;
}

int Servidor::obtenerCantidadConexiones() const {
    lock_guard<mutex> lock(mutexConexiones);
    return this->conexionesActivas;
}

void Servidor::enviarMensajeACliente(int socket, const string& mensaje) const {
    ssize_t bytesEnviados = send(socket, mensaje.c_str(), mensaje.size(), 0);
    if (bytesEnviados == -1) {
        cerr << SocketHelper::obtenerHoraActual() << "[Servidor] Error al enviar mensaje al cliente: "
        << strerror(errno) << endl;
    }
}

int Servidor::aceptarConexionConCliente(sockaddr_in& direccionCliente, socklen_t& tamanioDireccion) {
    cout << SocketHelper::obtenerHoraActual() << "[Servidor] Esperando conexion entrante..." << endl;
    int descriptorCliente = accept(this->fileDescriptor, (struct sockaddr*)&direccionCliente, &tamanioDireccion);
    if (ERROR_CONECTAR_CON_CLIENTE == descriptorCliente) {
        cerr << SocketHelper::obtenerHoraActual() << "[Servidor] Error al aceptar conexión: " << strerror(errno) << endl;

        return ERROR_CONECTAR_CON_CLIENTE;
    }

    return descriptorCliente;
}

int Servidor::manejarColaDeClientes(int descriptorCliente) {
    lock_guard<mutex> lock(mutexConexiones);
    if (CANTIDAD_MAXIMA_CONEXIONES <= this->conexionesActivas) {
        int clientesEnEspera = this->conexionesActivas - CANTIDAD_MAXIMA_CONEXIONES + 1;
        string mensaje = "Servidor ocupado. Tiene " + SocketHelper::aCadena(clientesEnEspera) + " clientes antes que usted.";
        this->enviarMensajeACliente(descriptorCliente, mensaje);

        return CLIENTE_NO_ATENDIDO;
    }

    return CLIENTE_ATENDIDO;
}

void Servidor::enviarMensajeDeBienvenida(int descriptorCliente) const {
    string mensaje = "Bienvenido al servidor!\nEnvíe -1 para salir.\n";
    this->enviarMensajeACliente(descriptorCliente, mensaje);
}

int Servidor::recibirMensaje(int socket, char* buffer, size_t tamanio) const {
    ssize_t cantidad = recv(socket, buffer, tamanio - 1, 0);
    if (cantidad > 0) {
        buffer[cantidad] = '\0';  // asegurar string válido
        cout << SocketHelper::obtenerHoraActual() << "[Servidor] Cliente dice: " << buffer;
        int valor = atoi(buffer);
        if (valor == VALOR_DE_CIERRE_DE_CONEXION) {
            cout << SocketHelper::obtenerHoraActual() << "[Servidor] Cliente pidió cerrar la conexión." << endl;

            return VALOR_DE_CIERRE_DE_CONEXION;
        }

        return valor;
    } else if (cantidad == 0) {
        cout << SocketHelper::obtenerHoraActual() << "[Servidor] Cliente cerró la conexión." << endl;
    } else {
        cerr << SocketHelper::obtenerHoraActual() << "[Servidor] Error al recibir datos del cliente: "
        << strerror(errno) << endl;
    }

    return VALOR_DE_CIERRE_DE_CONEXION;
}

string Servidor::generarRespuesta(int valor, const char* original) {
    return "Número recibido: " + string(original);
}


void Servidor::manejarCliente(int descriptorCliente) {
    this->enviarMensajeDeBienvenida(descriptorCliente);

    char buffer[1024];
    int valor = this->recibirMensaje(descriptorCliente, buffer, sizeof(buffer));

    while (VALOR_DE_CIERRE_DE_CONEXION != valor){
        string respuesta = this->generarRespuesta(valor, buffer);
        this->enviarMensajeACliente(descriptorCliente, respuesta);

        valor = this->recibirMensaje(descriptorCliente, buffer, sizeof(buffer));
    }

    this->cerrarConexion(descriptorCliente);
    this->decrementarConexiones();

    cout << SocketHelper::obtenerHoraActual() << "[Servidor] Cliente desconectado. Conexiones activas: "
    << obtenerCantidadConexiones() << endl;
}


void Servidor::cerrarConexion(int descriptorCliente) {
    #ifdef _WIN32
        closesocket(descriptorCliente);
    #else
        close(descriptorCliente);
    #endif
}
