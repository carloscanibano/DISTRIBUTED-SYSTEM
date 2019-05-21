import java.io.*;
import java.net.Socket;
import gnu.getopt.Getopt;
import java.net.ServerSocket;

class suscriptor {
	/******************* ATTRIBUTES *******************/
	private static int port = -1;
	private static String _server = null;
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
		//Si el socket asociado no esta creado, obtenemos el puerto e inicializamos el thread
		if (svSocket == null) {
			svSocket = new ServerSocket(0);
			port = svSocket.getLocalPort();
			thread = new ClientThread(svSocket);
			thread.start();
		}
		//Limite de longitud del topic
		if (topic.length() > 128) return 1;
		//Mandamos el codigo de operacion y el tema
		outputStream.write(("SUBSCRIBE" + "\0").getBytes());
	  	outputStream.write((topic + "\0").getBytes());
		//El broker necesita saber el puerto al que enviar los textos
		outputStream.write((String.valueOf(port) + "\0").getBytes());
		//Leemos byte de resultado de la operacion
		byte[] aux = null;
		int b = 0;
		aux = new byte[2];
	  	b = inputStream.read(aux);
	  	String s = new String(aux, 0, b);

		if (s.equals("0\0")){
			System.out.println("SUBSCRIBE OK");
		}else{
			System.out.println("SUBSCRIBE FAIL");
		}

		return 1;
	}

	static int unsubscribe(String topic) throws Exception
	{	
		//Limite de longitud del topic
		if (topic.length() > 128) return 1;
		//Mandamos el codigo de operacion y el tema
		outputStream.write(("UNSUBSCRIBE" + "\0").getBytes());
	   	outputStream.write((topic + "\0").getBytes());
		//Necesariamente debe existir un puerto de escucha
		if (port == -1) {
			System.out.println("Can't unsubscribe, no port was listening");
			return -1;
		}
		//Enviamos el puerto de escucha
		outputStream.write((String.valueOf(port) + "\0").getBytes());
		//Leemos el byte de respuesta de operacion
		byte[] aux = null;
		int b = 0;
		aux = new byte[2];
	  	b = inputStream.read(aux);
	  	String s = new String(aux, 0, b);

		if (s.equals("0\0")){
			System.out.println("UNSUBSCRIBE OK");
		}else{
			System.out.println("TOPIC NOT SUBSCRIBED");
		}

		return 1;
	}

	static int quit() throws Exception
	{	
		//Necesariamente debe existir un puerto de escucha
		if (port == -1) {
			System.out.println("Can't unsubscribe, no port was listening");
			return -1;
		}
		//Enviamos el codigo de operacion y el puerto de escucha
		outputStream.write(("QUIT" + "\0").getBytes());
		outputStream.write((String.valueOf(port) + "\0").getBytes());
		//Si el socket existe, lo cerramos para parar el thread asociado
		if (svSocket != null) svSocket.close();

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

	    try {
	    	//Creacion del socket utilizado para conectarnos al broker
	    	clientSocket = new Socket(_server, _port);
	    	outputStream = clientSocket.getOutputStream();
	    	inputStream = new DataInputStream(clientSocket.getInputStream());
	    	bufferedReader = new BufferedReader(new
	    	InputStreamReader(clientSocket.getInputStream()));
	    	//Llamamos a la consola
	    	shell();
	    	//Liberamos los recursos asociados al uso del programa
	    	outputStream.close();
	   		bufferedReader.close();
	   		clientSocket.close();
	  	} catch (Exception e) {
	  		System.out.println("Error in the connection to the broker <" + _server + ">:<" + _port + ">");
	    	e.printStackTrace();
	  	}
		System.out.println("+++ FINISHED +++");
	}
}