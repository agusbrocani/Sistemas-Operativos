import java.io.*;
import java.net.*;
import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.atomic.AtomicInteger;

public class Server {
    private static final int PORT = 12345;
    private static final int MAX_ACTIVE_CLIENTS = 1;  // Conexiones activas máximas
    private static final int CONNECTION_BACKLOG = 3; // Cola de espera
    private static List<ClientHandler> activeClients = new ArrayList<>();
    private static Queue<ClientHandler> waitingClients = new LinkedList<>();
    private static ServerSocket serverSocket;
    private static boolean running = true;
    private static final AtomicInteger clientId = new AtomicInteger(1);

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
                closeServer();
            }));

            // Thread para procesar la cola de espera
            Thread queueProcessor = new Thread(() -> processWaitingQueue());
            queueProcessor.start();

            while (running) {
                try {
                    Socket clientSocket = serverSocket.accept();
                    handleNewConnection(clientSocket);
                    //AGREGAR BANDERA DE QUE ATENDIO AL MENOS 1 CLIENTE => BREAK
                } catch (IOException e) {
                    if (running) {
                        System.out.println("Error aceptando cliente: " + e.getMessage());
                    }
                }
            }
        } catch (IOException e) {
            System.out.println("Error iniciando el servidor: " + e.getMessage());
        } finally {
            closeServer();
        }
    }

    private static void handleNewConnection(Socket clientSocket) {
        // Limpiar clientes desconectados
        activeClients.removeIf(client -> !client.isActive());

        if (activeClients.size() < MAX_ACTIVE_CLIENTS) {
            int assignedClientId = clientId.getAndIncrement();
            // Hay espacio para un cliente activo
            ClientHandler clientHandler = new ClientHandler(clientSocket, assignedClientId, false);
            activeClients.add(clientHandler);
            new Thread(clientHandler).start();
            System.out.println("Nuevo cliente conectado con ID: " + clientHandler.getClientId() + ". Total activos: " + activeClients.size());
        } else {
            int assignedClientId = clientId.getAndIncrement();
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
                    out.println("❌ Servidor y cola de espera llenos. Por favor, intente más tarde.");
                    clientSocket.close();
                } catch (IOException e) {
                    System.out.println("Error rechazando cliente: " + e.getMessage());
                }
            }
        }
    }

    private static void processWaitingQueue() {
        while (running) {
            try {
                Thread.sleep(1000); // Revisar la cola cada segundo
                
                // Limpiar clientes desconectados
                activeClients.removeIf(client -> !client.isActive());

                // Procesar clientes en espera si hay espacio
                while (activeClients.size() < MAX_ACTIVE_CLIENTS && !waitingClients.isEmpty()) {
                    ClientHandler nextClient = waitingClients.poll();
                    if (nextClient != null && nextClient.isActive()) {
                        nextClient.setWaiting(false);
                        activeClients.add(nextClient);
                        System.out.println("Cliente-" + nextClient.getClientId() + " movido de la cola a activo. Total activos: " + activeClients.size());
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
            for (ClientHandler client : activeClients) {
                client.sendMessage("⚠️ Servidor cerrándose. Gracias por usar el servicio.");
            }
            for (ClientHandler client : waitingClients) {
                client.sendMessage("⚠️ Servidor cerrándose. Gracias por usar el servicio.");
            }
        } catch (IOException e) {
            System.out.println("Error cerrando el servidor: " + e.getMessage());
        }
    }

    public static void processMessage(String message, ClientHandler sender) {
        String response = "Respuesta del servidor:\n-Longitud del mensaje: " + "[" + message.length() + "]\n" + "-Mensaje: " + message;
        sender.sendMessage(response);
        
        // Notificar a otros clientes activos   => BROADCAST
        // for (ClientHandler client : activeClients) {
        //     if (client != sender && client.isActive()) {
        //         client.sendMessage("Cliente-" + sender.getClientId() + " dice: " + message);
        //     }
        // }
    }
} 
