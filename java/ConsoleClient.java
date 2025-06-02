import java.io.*;
import java.net.*;

public class ConsoleClient {
    private static final String SERVER_HOST = "localhost";
    private static final int SERVER_PORT = 12345;

    public static void main(String[] args) {
        try (
            Socket socket = new Socket(SERVER_HOST, SERVER_PORT);
            PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
            BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            BufferedReader consoleIn = new BufferedReader(new InputStreamReader(System.in));
        ) {
            // Thread para recibir respuestas del servidor
            Thread receiverThread = new Thread(() -> {
                try {
                    String serverResponse;
                    while ((serverResponse = in.readLine()) != null) {
                        System.out.println(serverResponse);
                        if ("Servidor cerrándose. Gracias por usar el servicio.".equalsIgnoreCase(serverResponse)) {
                            break;
                        }
                    }
                    // Si el servidor cierra la conexión abruptamente
                    System.exit(0);
                } catch (IOException e) {
                    if (!socket.isClosed()) {
                        System.out.println("Conexión con el servidor perdida.");
                        System.exit(0);
                    }
                }
            });
            receiverThread.start();

            // Loop principal para enviar mensajes
            String userInput;
            while ((userInput = consoleIn.readLine()) != null) {
                out.println(userInput);
                if ("exit".equalsIgnoreCase(userInput)) {
                    break;
                }
            }

        } catch (UnknownHostException e) {
            System.err.println("No se puede encontrar el servidor: " + SERVER_HOST);
        } catch (IOException e) {
            System.err.println("Error de conexión: " + e.getMessage());
        }
    }
}
