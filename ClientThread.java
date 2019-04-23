import java.io.*;
import java.net.Socket;
import gnu.getopt.Getopt;
import java.net.ServerSocket;

public class ClientThread extends Thread {
  	private ServerSocket sv;
  	private BufferedReader bufferedReader;

  	public ClientThread(ServerSocket sv) {
  		this.sv = sv;
  	}

    public void run(){
      try{
        System.out.println("Thread lanzado y escuchando\n");
        Socket sc = sv.accept();
        this.bufferedReader = new BufferedReader(new 
        InputStreamReader(sc.getInputStream()));
        String reply = bufferedReader.readLine();
        System.out.format("%s\n", reply);

      } catch(Exception e){
        e.printStackTrace();
      }
    }
}