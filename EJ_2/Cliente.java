import java.io.*;
import java.net.Socket;

public class Cliente {
    private static volatile boolean conectado = true;

    public static void main(String[] args) {
        try {
            conectar("localhost", 4000);
        } catch (IOException e) {
            System.err.println("❌ Error de conexión: " + e.getMessage());
        }
    }

    private static void conectar(String host, int puerto) throws IOException {
        try (Socket socket = new Socket(host, puerto);
             BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
             PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
             BufferedReader consola = new BufferedReader(new InputStreamReader(System.in))) {

            System.out.println(in.readLine()); // bienvenida
            System.out.println(in.readLine()); // ID del cliente

            Thread escuchaServidor = new Thread(() -> {
                try {
                    String respuesta;
                    while ((respuesta = in.readLine()) != null) {
                        if (respuesta.startsWith("🚨")) {
                            System.out.println(respuesta);
                            conectado = false;
                            break;
                        }
                    }
                    if (respuesta == null) {
                        System.out.println("❌ Conexión cerrada por el servidor.");
                        conectado = false;
                    }
                } catch (IOException e) {
                    System.out.println("❌ Error escuchando al servidor: " + e.getMessage());
                    conectado = false;
                }
            });

            escuchaServidor.start();

            while (conectado) {
                System.out.print("Ingresá un número (-1 para salir): ");
                String entrada = consola.readLine();
                if (!conectado) break;

                out.println(entrada);

                if (entrada.equals("-1")) {
                    System.out.println("Te desconectaste del servidor.");
                    break;
                }
            }

        } catch (IOException e) {
            throw e;
        }
    }
}