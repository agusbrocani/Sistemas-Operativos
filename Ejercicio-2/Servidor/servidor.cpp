#include "../bibliotecas/servidor/servidor.h"
#include "../bibliotecas/servidor/socketHelper.h"

int main()
{
    try {
        Servidor servidor;
        tInfoDireccionSocket infoDireccionSocket;

        SocketHelper::inicializarIPPuerto(&infoDireccionSocket);

        servidor.abrirCanalDeComunicacionTCP();
        servidor.enlazarSocketAIPYPuerto(&infoDireccionSocket);
        servidor.escucharConexionesEntrantes();
        servidor.aceptarConexiones();

    } catch(const exception& ex) {
        cerr << ex.what() << endl;
    }

    return 0;
}
