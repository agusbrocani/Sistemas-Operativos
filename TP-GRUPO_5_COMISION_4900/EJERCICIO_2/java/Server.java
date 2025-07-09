import java.io.*;
import java.net.*;
import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.atomic.AtomicInteger;

public class Server {
    private static final int PORT = 12345;
    private static final int MAX_ACTIVE_CLIENTS = 2;  // Conexiones activas máximas
    private static final int CONNECTION_BACKLOG = 3; // Cola de espera
    private static List<ClientHandler> activeClients = Collections.synchronizedList(new ArrayList<>());
    private static Queue<ClientHandler> waitingClients = new ConcurrentLinkedQueue<>();
    private static ServerSocket serverSocket;
    private static volatile boolean running = true;
    private static final AtomicInteger clientId = new AtomicInteger(1);
    private static volatile boolean closedServer = false;
    private static volatile boolean clientServed = false;

    public static void main(String[] args) {
        try {
            // Configurar el servidor con la cola de espera
            serverSocket = new ServerSocket(PORT, CONNECTION_BACKLOG);
            System.out.println("Servidor iniciado en el puerto " + PORT);
            System.out.println("Máximo de clientes activos: " + MAX_ACTIVE_CLIENTS);
            System.out.println("Tamaño de la cola de espera: " + CONNECTION_BACKLOG);

            // Thread para manejar el cierre del servidor
            Runtime.getRuntime().addShutdownHook(new Thread(() -> {
                running = false;
                closedServer = true;
                closeServer();
            }));

            // Thread para procesar la cola de espera
            Thread queueProcessor = new Thread(() -> processWaitingQueue());
            queueProcessor.start();

            // Thread de monitoreo para cerrar servidor si ya no quedan clientes
            new Thread(() -> {
                while (running) {
                    synchronized (Server.class) { // Lock común para ver ambos: active y waiting
                        if (clientServed && activeClients.isEmpty() && waitingClients.isEmpty()) {
                            System.out.println("Todos los clientes fueron atendidos. Cerrando servidor...");
                            running = false;
                            try {
                                serverSocket.close(); // Esto forza el cierre del accept()
                            } catch (IOException e) {
                                System.out.println("Error cerrando serverSocket en cierre automático.");
                            }
                        }
                    }

                    try {
                        Thread.sleep(1000); // Revisa cada segundo
                    } catch (InterruptedException e) {
                        break;
                    }
                }
            }).start();

            while (running) {
                try {
                    Socket clientSocket = serverSocket.accept();
                    handleNewConnection(clientSocket);
                } catch (IOException e) {
                    if (running) {
                        System.out.println("Error aceptando cliente: " + e.getMessage());
                    }
                }
            }
        } catch (IOException e) {
            System.out.println("Error iniciando el servidor: " + e.getMessage());
        } finally {
            if (!closedServer) {
                closeServer();
            }
        }
    }

    private static void removeInactiveClients() {
        synchronized(activeClients) {
            activeClients.removeIf(client -> !client.isActive());
        }
    }

    private static void handleNewConnection(Socket clientSocket) {
        clientServed = true;
        // Limpiar clientes desconectados
        removeInactiveClients();

        int assignedClientId = clientId.getAndIncrement();

        synchronized (activeClients) {
            if (activeClients.size() < MAX_ACTIVE_CLIENTS) {
                ClientHandler clientHandler = new ClientHandler(clientSocket, assignedClientId, false);
                activeClients.add(clientHandler);
                new Thread(clientHandler).start();
                System.out.println("Nuevo cliente conectado con ID: " + clientHandler.getClientId() + ". Total activos: " + activeClients.size());
                return;
            }
        }

        // No hay espacio, añadir a la cola de espera
        if (waitingClients.size() < CONNECTION_BACKLOG) {
            ClientHandler waitingClient = new ClientHandler(clientSocket, assignedClientId, true);
            waitingClients.offer(waitingClient);
            new Thread(waitingClient).start();
            System.out.println("Cliente en cola con ID: " + waitingClient.getClientId() + ". Posición: " + waitingClients.size());
        } else {
            // Cola llena
            try {
                PrintWriter out = new PrintWriter(clientSocket.getOutputStream(), true);
                out.println("Servidor y cola de espera llenos. Por favor, intente más tarde.");
                clientSocket.close();
            } catch (IOException e) {
                System.out.println("Error rechazando cliente: " + e.getMessage());
            }
        }
    }

    private static void processWaitingQueue() {
        while (running) {
            try {
                Thread.sleep(1000); // Revisar la cola cada segundo

                synchronized (Server.class) { 
                    // Limpiar clientes desconectados
                    removeInactiveClients();

                    // Procesar clientes en espera si hay espacio
                    while (activeClients.size() < MAX_ACTIVE_CLIENTS && !waitingClients.isEmpty()) {
                        ClientHandler nextClient = waitingClients.poll();
                        if (nextClient != null && nextClient.isActive()) {
                            nextClient.setWaiting(false);
                            activeClients.add(nextClient);
                            System.out.println("Cliente-" + nextClient.getClientId() + " movido de la cola a activo. Total activos: " + activeClients.size());
                        }
                    }
                }

            } catch (InterruptedException e) {
                if (running) {
                    System.out.println("Error en el procesador de cola: " + e.getMessage());
                }
            }
        }
    }

    private static void closeServer() {
        try {
            if (serverSocket != null && !serverSocket.isClosed()) {
                serverSocket.close();
                System.out.println("Servidor cerrado.");
            }
            // Cerrar todas las conexiones
            synchronized (activeClients) {
                for (ClientHandler client : activeClients) {
                    String serverMessage = closedServer ? 
                    "Se cayo el servidor. Finalizando la conexión."
                    : "Servidor cerrándose. Gracias por usar el servicio.";
                    if (closedServer) {
                        client.sendMessage(serverMessage);
                    } else {
                        System.out.println(serverMessage);
                    }
                } 
            }
            // No es necesario synchronized porque se usa ConcurrentLinkedQueue(Estructura de datos thread-safe)
            for (ClientHandler client : waitingClients) {
                String serverMessage = closedServer ? 
                "Se cayo el servidor. Finalizando la conexión."
                : "Servidor cerrándose. Gracias por usar el servicio.";
                if (closedServer) {
                    client.sendMessage(serverMessage);
                } else {
                    System.out.println(serverMessage);
                }
            }
        } catch (IOException e) {
            System.out.println("Error cerrando el servidor: " + e.getMessage());
        }
    }

    public static void processMessage(String message, ClientHandler sender) {
        String response = "Respuesta del servidor:\n-Longitud del mensaje: " + "[" + message.length() + "]\n" + "-Mensaje: " + message;
        sender.sendMessage(response);
    }
}
