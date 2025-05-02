import java.io.*;
import java.net.Socket;

public class Cliente {

    public static void main(String[] args) {
        try {
            System.out.println("🟢 Conectando al servidor...");
            conectar("localhost", 4000);
        } catch (IOException e) {
            System.err.println("❌ Error de conexión: " + e.getMessage());
            e.printStackTrace();
        }
    }

    private static void conectar(String host, int puerto) throws IOException {
        try (
            Socket socket = new Socket(host, puerto);
            BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
            BufferedReader consola = new BufferedReader(new InputStreamReader(System.in))
            ){

            System.out.println(leerMensaje(in)); // Bienvenida
            interactuar(in, out, consola);
        }
    }

    private static void interactuar(BufferedReader in, PrintWriter out, BufferedReader consola) throws IOException {
        while (true) {
            System.out.print("Ingresá un número (-1 para salir): ");
            String entrada = consola.readLine();

            out.println(entrada);

            if (entrada.equals("-1")) {
                System.out.println("Te desconectaste del servidor.");
                break;
            }

            String respuesta = leerMensaje(in);
            if (respuesta == null) {
                System.out.println("El servidor cerró la conexión.");
                break;
            }

            System.out.println("Respuesta: " + respuesta);
        }
    }

    private static String leerMensaje(BufferedReader in) throws IOException {
        String mensaje = in.readLine();
        if (mensaje == null) return null;
        return mensaje;
    }
}
