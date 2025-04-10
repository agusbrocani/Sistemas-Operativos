#ifndef SOCKETHELPER_H
#define SOCKETHELPER_H

#include "servidor.h"

using namespace std;
class SocketHelper
{
    public:
        SocketHelper() = delete;
        ~SocketHelper() = delete;

        static void inicializarIPPuerto(tInfoDireccionSocket* infoDireccionSocket);
        static string convertirIPBinariaACadena(const struct in_addr& direccionBinaria);
        static string convertirPuertoACadena(uint16_t puertoRed);
        static string aCadena(int numero);
        static string obtenerHoraActual();

    private:
        static string convertirIPBinariaACadenaWin(const struct in_addr& direccionBinaria);

};

#endif // SOCKETHELPER_H
