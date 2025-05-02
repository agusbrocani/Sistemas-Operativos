import java.io.*;
import java.net.*;
import java.util.concurrent.Semaphore;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.CopyOnWriteArrayList;

public class Servidor {
    private static final int PUERTO = 4000;
    private static final int MAX_CLIENTES = 4;
    private static final Semaphore conexiones = new Semaphore(MAX_CLIENTES);
    private static final AtomicInteger contadorClientes = new AtomicInteger(1);
    private static final CopyOnWriteArrayList<PrintWriter> clientesActivos = new CopyOnWriteArrayList<>();

    public static void main(String[] args) {
        Runtime.getRuntime().addShutdownHook(new Thread(() -> {
            System.out.println("🛑 Servidor finalizando. Notificando clientes...");
            for (PrintWriter out : clientesActivos) {
                try {
                    out.println("🚨 El servidor se cerró inesperadamente. Adiós.");
                } catch (Exception ignored) {}
            }
        }));

        while (true) {
            try {
                iniciarServidor();
            } catch (Exception e) {
                System.err.println("💥 Error en el servidor: " + e.getMessage());
                e.printStackTrace();
                System.out.println("Reintentando en 1 segundo...");
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException ignored) {}
            }
        }
    }

    private static void iniciarServidor() throws IOException {
        try (ServerSocket servidor = new ServerSocket(PUERTO)) {
            System.out.println("🟢 Servidor escuchando en el puerto " + PUERTO);

            while (true) {
                aceptarCliente(servidor);
            }
        }
    }

    private static void aceptarCliente(ServerSocket servidor) {
        try {
            Socket cliente = servidor.accept();
            conexiones.acquire();

            int clienteId = contadorClientes.getAndIncrement();
            System.out.println("👤 Cliente #" + clienteId + " conectado desde " + cliente.getInetAddress()
                    + ". Slots disponibles: " + conexiones.availablePermits());

            manejarCliente(cliente, clienteId);

        } catch (IOException e) {
            System.err.println("⚠️ Error al aceptar cliente: " + e.getMessage());
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
            System.err.println("⛔ Interrupción del semáforo");
        }
    }

    private static void manejarCliente(Socket cliente, int id) {
        new Thread(() -> {
            try (
                BufferedReader in = new BufferedReader(new InputStreamReader(cliente.getInputStream()));
                PrintWriter out = new PrintWriter(cliente.getOutputStream(), true)
            ) {
                clientesActivos.add(out);

                out.println("Bienvenido. Enviá un número. Enviá -1 para salir.");
                out.println("Sos el cliente #" + id);

                procesarMensajes(in, out, id);

            } catch (IOException e) {
                System.err.println("❌ Error con cliente #" + id + ": " + e.getMessage());
            } finally {
                clientesActivos.removeIf(p -> p.checkError());
                try { cliente.close(); } catch (IOException ignored) {}
                conexiones.release();
                System.out.println("🔁 Cliente #" + id + " desconectado. Slots disponibles: " + conexiones.availablePermits());
            }
        }).start();
    }

    private static void procesarMensajes(BufferedReader in, PrintWriter out, int id) throws IOException {
        String msg;
        while ((msg = in.readLine()) != null) {
            System.out.println("📨 Cliente #" + id + " dijo: " + msg);

            if (msg.trim().equals("-1")) {
                out.println("👋 Hasta luego.");
                break;
            }

            try {
                int numero = Integer.parseInt(msg.trim());
                out.println("✅ Respuesta: " + (numero * 2));
            } catch (NumberFormatException e) {
                out.println("⚠️ Debés ingresar un número.");
            }
        }
    }
}
