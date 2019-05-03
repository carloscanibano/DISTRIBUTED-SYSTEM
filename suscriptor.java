import java.io.*;
import java.net.Socket;
import gnu.getopt.Getopt;
import java.net.ServerSocket;


class suscriptor {

	/********************* TYPES **********************/


	/******************* ATTRIBUTES *******************/

	private static int port = -1;
	private static String _server   = null;
	private static short _port = -1;
	private static Socket clientSocket = null;
  private static OutputStream outputStream = null;
  private static DataInputStream inputStream = null;
  private static BufferedReader bufferedReader = null;
  private static ServerSocket svSocket = null;
	private static ClientThread thread = null;

	/********************* METHODS ********************/

	static int subscribe(String topic) throws Exception
	{
		if (svSocket == null) {
			svSocket = new ServerSocket(0);
			port = svSocket.getLocalPort();
			thread = new ClientThread(svSocket);
			thread.start();
		}
		System.out.println("El valor puerto: " + port);
		if (topic.length() > 128) return 1;
		// Write your code here
		outputStream.write(("SUBSCRIBE" + "\0").getBytes());
	  outputStream.write((topic + "\0").getBytes());
		//Listener port
		outputStream.write((String.valueOf(port) + "\0").getBytes());
		//MIO
		/*
		int num[] ;
		int res;
		ObjectInput in = new ObjectInputStream(inputStream);

		num = (int[]) in.readObject();
		res = num[0] + num[1];
		DataOutputStream ostream = new DataOutputStream(clientSocket.getOutputStream());
		ostream.writeInt(res);
		ostream.flush();*/

		byte[] aux = null;
		int b = 0;
		aux = new byte[1];
	  byte res = inputStream.readByte();
	  String s = Byte.toString(res);
		//String s = new String(new byte[] { res });
	  System.out.println("El valor recibido es: " + s);
		String s1 = "0";

		if (s.equals(s1)){
			System.out.println("SUBSCRIBE OK");
		}else{
			System.out.println("SUSCRIBE FAIL");
		}
		//MIO


		/*
		DataInputStream  in  = new DataInputStream(clientSocket.getInputStream());
		byte[] aux = null;
		aux = new byte[256];
	   	in.read(aux);
	   	String s = new String(aux);
	   	System.out.println("El valor recibido es: " + s);
		if (s.equals("0")) System.out.println("Subscribe to: " + topic);
		*/
		return 1;
	}

	static int unsubscribe(String topic) throws Exception
	{
		if (topic.length() > 128) return 1;
		// Write your code here
		outputStream.write(("UNSUBSCRIBE" + "\0").getBytes());
	   	outputStream.write((topic + "\0").getBytes());
		//Listener port
		if (port == -1) {
			System.out.println("Can't unsubscribe, no port was listening");
			return -1;
		}
		outputStream.write((String.valueOf(port) + "\0").getBytes());
		//MIO
		//System.out.println("esperando");
		byte[] aux = null;
		aux = new byte[1];
	  inputStream.read(aux);
		//System.out.println("esperando");
	  String s = new String(aux);
		//System.out.println("esperando");
	  System.out.println("El valor recibido es: " + s);
		String s1 = "0";
		if (s.equals(s1)){
			System.out.println("UNSUBSCRIBE OK");
		}else{
			System.out.println("UNSUSCRIBE FAIL");
		}
		//MIO

		//if (!reply.equals("0")) System.out.println("Unsubscribe to: " + topic);

		return 1;
	}

	static int quit() throws Exception
	{
		outputStream.write(("QUIT" + "\0").getBytes());
		outputStream.write((String.valueOf(port) + "\0").getBytes());
		return 1;
	}

	/**
	 * @brief Command interpreter for the suscriptor. It calls the protocol functions.
	 */
	static void shell() throws Exception
	{
		boolean exit = false;
		String input;
		String [] line;
		BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
		inputStream  = new DataInputStream(clientSocket.getInputStream());

		while (!exit) {
			try {
				System.out.print("c> ");
				input = in.readLine();
				line = input.split("\\s");

				if (line.length > 0) {
					/*********** SUBSCRIBE *************/
					if (line[0].equals("SUBSCRIBE")) {
						if  (line.length == 2) {
							subscribe(line[1]); // topic = line[1]
						} else {
							System.out.println("Syntax error. Usage: SUBSCRIBE <topic>");
						}
					}

					/********** UNSUBSCRIBE ************/
					else if (line[0].equals("UNSUBSCRIBE")) {
						if  (line.length == 2) {
							unsubscribe(line[1]); // topic = line[1]
						} else {
							System.out.println("Syntax error. Usage: UNSUBSCRIBE <topic>");
						}
                    }

          /************** QUIT **************/
          else if (line[0].equals("QUIT")){
						if (line.length == 1) {
							exit = true;
							quit();
							if(thread != null) thread.interrupt();
							if(svSocket != null) svSocket.close();
						} else {
							System.out.println("Syntax error. Use: QUIT");
						}
					}

					/************* UNKNOWN ************/
					else {
						System.out.println("Error: command '" + line[0] + "' not valid.");
					}
				}
			} catch (java.io.IOException e) {
				System.out.println("Exception: " + e);
				e.printStackTrace();
			}
		}
	}

	/**
	 * @brief Prints program usage
	 */
	static void usage()
	{
		System.out.println("Usage: java -cp . suscriptor -s <server> -p <port>");
	}

	/**
	 * @brief Parses program execution arguments
	 */
	static boolean parseArguments(String [] argv)
	{
		Getopt g = new Getopt("suscriptor", argv, "ds:p:");

		int c;
		String arg;

		while ((c = g.getopt()) != -1) {
			switch(c) {
				//case 'd':
				//	_debug = true;
				//	break;
				case 's':
					_server = g.getOptarg();
					break;
				case 'p':
					arg = g.getOptarg();
					_port = Short.parseShort(arg);
					break;
				case '?':
					System.out.print("getopt() returned " + c + "\n");
					break; // getopt() already printed an error
				default:
					System.out.print("getopt() returned " + c + "\n");
			}
		}

		if (_server == null)
			return false;

		if ((_port < 1024) || (_port > 65535)) {
			System.out.println("Error: Port must be in the range 1024 <= port <= 65535");
			return false;
		}

		return true;
	}



	/********************* MAIN **********************/

	public static void main(String[] argv)
	{
		if(!parseArguments(argv)) {
			usage();
			return;
		}


		// Write code here
	    try {
	    	//CREACION DEL SOCKET
	    	clientSocket = new Socket(_server, _port);
	    	outputStream = clientSocket.getOutputStream();
	    	inputStream = new DataInputStream(clientSocket.getInputStream());
	    	bufferedReader = new BufferedReader(new
	    	InputStreamReader(clientSocket.getInputStream()));
	    	//INVOCACION DE CONSOLA
	    	shell();
	    	/*
	    	String reply;
	    	while (true) {
		      	reply = bufferedReader.readLine();
		      	if (reply.equalsIgnoreCase("End")) {
		        	break;
		       	}
		      	if(reply.length()!= 0){
		       		System.out.println("you got reply ---"+ reply);
		       	}
	    	}
	    	*/
	    	//CERRAR STREAMS
	    	outputStream.close();
	   		bufferedReader.close();
	   		clientSocket.close();
	  	} catch (Exception e) {
	    	e.printStackTrace();
	  	}

		System.out.println("+++ FINISHED +++");
	}
}
