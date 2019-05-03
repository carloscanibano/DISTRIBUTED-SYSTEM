import java.io.*;
import java.net.Socket;
import gnu.getopt.Getopt;
import java.net.ServerSocket;
import java.util.concurrent.atomic.AtomicBoolean;


public class ClientThread extends Thread {
  	private ServerSocket sv;
  	private BufferedReader bufferedReader;
    private AtomicBoolean running = new AtomicBoolean(true);
    private DataInputStream inputStream = null;
    int num[] ; // peticion
    int tema;
    int texto;

  	public ClientThread(ServerSocket sv) {
  		this.sv = sv;
      /*try {
          bufferedReader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
          outputWriter = new PrintWriter(socket.getOutputStream(), true);
      } catch (IOException e) {
          System.out.println(e.getMessage());
      }*/
  	}


    public void interrupt(){
      running.set(false);

    }


    public void run(){
      while(running.get()){
        try{
          //System.out.println("Thread lanzado y escuchando\n");
          //while(running.get()){
          Socket sc = sv.accept();
          inputStream = new DataInputStream(sc.getInputStream());
          //this.bufferedReader = new BufferedReader(new
          //InputStreamReader(sc.getInputStream()));
          //boolean bandera = true;
          //System.out.println("Nuevo texto\n");
          /*
          ObjectInput in = new ObjectInputStream(inputStream);

          num = (int[]) in.readObject();
          tema = num[0] + num[1] + num[2];
          DataOutputStream ostream = new DataOutputStream(sc.getOutputStream());
          ostream.writeInt(tema);
          ostream.flush();
          sc.close();*/


          byte[] aux = null;
          int b = 0;
        	aux = new byte[1024];
        	//inputStream.readFully(aux);
          b = inputStream.read(aux);
        	//String s = new String(aux);
          System.out.println(b);
          String s = new String(aux, 0, b);
        	System.out.println(s);
          /*
          while((b = inputStream.read(aux)) > 0){
            String s = new String(aux, 0, b);
            System.out.println(s);
          }*/
          //System.out.println();
          //String reply = bufferedReader.readLine();
          //if(reply != null) System.out.println(reply);
          System.out.println("bien");
          //System.out.println("valor i:" + i);
          inputStream.close();
          sc.close();
          System.out.println("close");
          //}
          //System.out.println("valor i:" + i);
          /*
          String receivedMessage = bufferedReader.readLine();
          System.out.println(receivedMessage);
          */
        } catch(Exception e){
          Thread.currentThread().interrupt();
          e.printStackTrace();
        }
      }
    }
}
