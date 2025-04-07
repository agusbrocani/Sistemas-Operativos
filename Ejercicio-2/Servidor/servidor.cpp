#include "../bibliotecas/servidor/servidor.h"

int main()
{
    try {
        Servidor servidor;
        tInfoDireccionSocket infoDireccionSocket;

        inicializarIPPuerto(&infoDireccionSocket);
        servidor.abrirCanalDeComunicacionTCP();
        servidor.enlazarSocketAIPYPuerto(&infoDireccionSocket);
    } catch(const exception& ex) {
        cerr << ex.what() << endl;
    }

    return 0;
}
