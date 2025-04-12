import java.io.*;
import java.net.*;

public class Cliente {
    public static void main(String[] args) {
        try (
            Socket socket = new Socket("localhost", 4000);
            BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
            BufferedReader consola = new BufferedReader(new InputStreamReader(System.in));
        ) {
            System.out.println(in.readLine()); // mensaje del servidor
            System.out.print("Escribí algo: ");
            String entrada = consola.readLine();
            out.println(entrada);
            System.out.println("Respuesta: " + in.readLine());
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
