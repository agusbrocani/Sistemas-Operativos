import java.io.*;
import java.net.*;

public class ClientHandler implements Runnable {
    private Socket socket;
    private PrintWriter out;
    private BufferedReader in;
    private int clientId;
    private boolean active;
    private boolean isWaiting;

    public ClientHandler(Socket socket, int clientId, boolean isWaiting) {
        this.socket = socket;
        this.clientId = clientId;
        this.active = true;
        this.isWaiting = isWaiting;
        try {
            out = new PrintWriter(socket.getOutputStream(), true);
            in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            
            if (isWaiting) {
                out.println("Estás en cola de espera. Por favor, espera a que haya espacio disponible.");
                out.println("No podrás enviar mensajes hasta que seas activado.");
            } else {
                out.println("Conexión activa. Puedes empezar a enviar mensajes.");
            }
        } catch (IOException e) {
            System.out.println("Error al crear el manejador del cliente: " + e.getMessage());
        }
    }

    public boolean isActive() {
        return active && !socket.isClosed();
    }

    public void setWaiting(boolean waiting) {
        this.isWaiting = waiting;
        if (!waiting) {
            out.println("¡Cliente ID: " + this.getClientId() + ", ya puedes enviar mensajes! Has sido movido de la cola de espera.");
        }
    }

    @Override
    public void run() {
        try {
            String inputLine;
            while ((inputLine = in.readLine()) != null) {
                if (isWaiting && !inputLine.contains("exit")) {
                    out.println("Estás en cola de espera. No puedes enviar mensajes aún.");
                    continue;
                }
                
                if ("exit".equalsIgnoreCase(inputLine)) {
                    break;
                }

                System.out.println("Cliente-" + clientId + " envió: " + inputLine);
                Server.processMessage(inputLine, this);
            }
        } catch (IOException e) {
            if (isActive()) {
                System.out.println("Error en la comunicación con Cliente-" + clientId + ": " + e.getMessage());
            }
        } finally {
            out.println("Ha sido desconectado del servidor.");
            closeConnection();
        }
    }

    private void closeConnection() {
        try {
            active = false;
            if (socket != null && !socket.isClosed()) {
                socket.close();
                System.out.println("Cliente-" + clientId + " desconectado");
            }
        } catch (IOException e) {
            System.out.println("Error al cerrar la conexión: " + e.getMessage());
        }
    }

    public void sendMessage(String message) {
        if (isActive()) {
            out.println(message);
        }
    }

    public int getClientId() {
        return clientId;
    }
} 
