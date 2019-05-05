import java.io.*;
import java.net.Socket;
import gnu.getopt.Getopt;
import java.net.ServerSocket;
import java.util.concurrent.atomic.AtomicBoolean;
import client.FormatServiceService;
import client.FormatService;
import java.util.*;
import java.net.URL;
import java.net.SocketException;

public class ClientThread extends Thread {
    private ServerSocket sv;
    private BufferedReader bufferedReader;
    //Sirve para parar el bucle while
    private AtomicBoolean running = new AtomicBoolean(true);
    private DataInputStream inputStream = null;
    int num[];  //Peticion
    int tema;
    int texto;

    public ClientThread(ServerSocket sv) {
        this.sv = sv;
    }

    //Cambia el booleano atomico a falso
    public void interrupt() {
        running.set(false);
    }

    public void run() {
        //Bucle infinito de escucha de peticiones mientras no se reciba una señal
        while (running.get()) {
            try {
                //Creamos la URL para conectarse con el servicio web
                URL url = new URL("http://localhost:2000/rs?wsdl");
                FormatServiceService service = new FormatServiceService(url);
                FormatService port = service.getFormatServicePort();
                //Esperamos por conexiones externas para recibir mensajes
                Socket sc = sv.accept();
                inputStream = new DataInputStream(sc.getInputStream());
                //Leemos byte a byte todo el mensaje completo (tema + texto)
                byte[] aux = null;
                int b = 0;
                aux = new byte[1200];
                b = inputStream.read(aux);
                String s = new String(aux, 0, b);
                //Dividimos el mensaje separando por "/"
                String[] msg = s.split("/");
                //Invocamos el servicio web eliminando "\0" y espacios laterales para formatear
                String answer = port.format(msg[1].trim().replaceAll("[\\000]*", ""));
                //Imprimo por pantalla el mensaje ya formateado
                System.out.println("MESSAGE FROM " + msg[0] + " : " + answer);
                //Cerramos los recursos (stream + socket)
                inputStream.close();
                sc.close();
            } catch(SocketException e) {
                //Si el socket es cerrado desde el programa suscriptor creado, paramos todo
                System.out.println("Thread finalizado");
                //Interrupcion para parar el bucle while
                Thread.currentThread().interrupt();
            } catch(Exception f) {
                f.printStackTrace();
            }
        }
    }
}