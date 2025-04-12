
import java.io.*;
import java.net.*;
import java.util.concurrent.Semaphore;

public class Servidor {
    private static final int PUERTO = 4000;
    private static final int MAX_CLIENTES = 4;
    private static final Semaphore conexiones = new Semaphore(MAX_CLIENTES);

    public static void main(String[] args) {
        try (ServerSocket servidor = new ServerSocket(PUERTO)) {
            System.out.println("Servidor escuchando en el puerto " + PUERTO);

            while (true) {
                Socket cliente = servidor.accept();
                System.out.println("Cliente conectado desde " + cliente.getInetAddress());

                conexiones.acquire(); // ↓ ↓ ↓ bloquea si ya hay 4 clientes
                new Thread(() -> {
                    try (
                        BufferedReader in = new BufferedReader(new InputStreamReader(cliente.getInputStream()));
                        PrintWriter out = new PrintWriter(cliente.getOutputStream(), true);
                    ) {
                        out.println("Bienvenido al servidor. Enviá un mensaje:");
                        String msg = in.readLine();
                        System.out.println("Cliente dijo: " + msg);
                        out.println("Recibido: " + msg);
                    } catch (IOException e) {
                        e.printStackTrace();
                    } finally {
                        try { cliente.close(); } catch (IOException ignored) {}
                        conexiones.release(); // libera el slot
                        System.out.println("Cliente desconectado. Slots disponibles: " + conexiones.availablePermits());
                    }
                }).start();
            }

        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }
    }
}
