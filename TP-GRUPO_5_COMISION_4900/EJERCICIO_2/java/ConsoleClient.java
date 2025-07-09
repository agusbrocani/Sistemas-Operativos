import java.io.*;
import java.net.*;

public class ConsoleClient {
    public static void main(String[] args) {
        try (
                BufferedReader input = new BufferedReader(new InputStreamReader(System.in))) {
            System.out.print("Ingrese IP del servidor: ");
            final String serverHost = input.readLine();

            System.out.print("Ingrese el puerto: ");
            final int serverPort = Integer.parseInt(input.readLine());

            try (
                    Socket socket = new Socket(serverHost, serverPort);
                    PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
                    BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                    BufferedReader consoleIn = new BufferedReader(new InputStreamReader(System.in));) {
                // Thread para recibir respuestas del servidor
                Thread receiverThread = new Thread(() -> {
                    try {
                        String serverResponse;
                        System.out.println("Puerto local asignado al cliente: " + socket.getLocalPort());
                        while ((serverResponse = in.readLine()) != null) {
                            System.out.println(serverResponse);
                            if ("Servidor cerrándose. Gracias por usar el servicio.".equalsIgnoreCase(serverResponse)) {
                                break;
                            }
                        }
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
                System.err.println("No se puede encontrar el servidor: " + serverHost);
            } catch (IOException e) {
                System.err.println("Error de conexión: " + e.getMessage());
            }

        } catch (IOException e) {
            System.err.println("Error leyendo IP o puerto: " + e.getMessage());
        }
    }
}
