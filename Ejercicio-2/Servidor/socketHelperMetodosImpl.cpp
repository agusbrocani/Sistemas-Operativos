#include "../bibliotecas/servidor/socketHelper.h"


void SocketHelper::inicializarIPPuerto(tInfoDireccionSocket* infoDireccionSocket) {
    infoDireccionSocket->tipoDeIP = TIPO_DE_IP_IPv4;
    infoDireccionSocket->direccionIP = TODA_INTERFAZ_LOCAL_DISPONIBLE;
    infoDireccionSocket->puerto = htons(PUERTO_POR_DEFECTO);
    memset(infoDireccionSocket->rellenoParaCompatibilidad, 0, BITS_DE_RELLENO);
}

string SocketHelper::aCadena(int numero){
    ostringstream oss;
    oss << numero;
    return oss.str();
}

string SocketHelper::convertirIPBinariaACadena(const struct in_addr& direccionBinaria) {
    #ifdef _WIN32
        return convertirIPBinariaACadenaWin(direccionBinaria);
    #else
        char buffer[CANTIDAD_CARACTERES_PARA_IPV_4];
        const char* resultado = inet_ntop(AF_INET, &(direccionBinaria), buffer, CANTIDAD_CARACTERES_PARA_IPV_4);
        return (resultado != nullptr) ? string(buffer) : "IP inválida";
    #endif
}

string SocketHelper::convertirPuertoACadena(uint16_t puertoRed) {
    uint16_t puertoHost = ntohs(puertoRed);
    return to_string(puertoHost);
}

string SocketHelper::convertirIPBinariaACadenaWin(const struct in_addr& direccionBinaria) {
    return string(inet_ntoa(direccionBinaria));
}

string SocketHelper::obtenerHoraActual() {
    time_t tiempoActual = time(nullptr);               // Obtener tiempo actual
    tm* horaLocal = localtime(&tiempoActual);          // Convertir a hora local

    char buffer[64];
    if (strftime(buffer, sizeof(buffer), "%H:%M:%S", horaLocal)) {
        return string(buffer) + " ART - ";  // O podés usar %Z si en tu sistema lo soporta
    }

    return "";
}
