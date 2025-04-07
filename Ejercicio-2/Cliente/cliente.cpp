#include "../bibliotecas/cliente/cliente.h"

int main()
{
    try {
        Cliente cliente;
        tInfoDireccionSocket infoDireccionSocket;

        inicializarIPPuerto(&infoDireccionSocket);
        cliente.abrirCanalDeComunicacionTCP();
        cliente.conectarAlServidor(&infoDireccionSocket);
        cliente.cerrarConexion();
    } catch(const exception& ex) {
        cerr << ex.what() << endl;
    }

    return 0;
}
