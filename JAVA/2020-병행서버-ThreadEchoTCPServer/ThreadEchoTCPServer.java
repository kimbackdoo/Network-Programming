import java.io.*;
import java.net.*;
import java.util.Random;

public class ThreadEchoTCPServer extends Thread {
	Socket client_s = null;

 	public ThreadEchoTCPServer(Socket c_soc) {
		this.client_s = c_soc; 
	}

	public void run() {
		String msg;
		DataInputStream dis;
		DataOutputStream dos;
		
		System.out.println(client_s + " Client Socket Thread Start...");

		try{
			dis = new DataInputStream(client_s.getInputStream());
			dos = new DataOutputStream(client_s.getOutputStream());	
			msg = dis.readUTF();
			System.out.println("Client Message: " + msg);

			Random r = new Random(System.currentTimeMillis());
			long s = r.nextInt(20000);
			Thread.sleep(s);
 			String capitalizedMsg = msg.toUpperCase(); 
			dos.writeUTF(capitalizedMsg);					
		} catch(Exception e) {
			e.printStackTrace();
		}
		System.out.println(client_s + "Client Socket Thread End...");
	}
 
	public static void main(String [] args) {
		Socket c_socket;
		ServerSocket serevr_s=null;	
		if(args.length != 1) {
			System.out.println("Usage : java EchoTCPServerThread port number");
			System.exit(0);
    	}

     	int port = Integer.parseInt(args[0]);

		try{
			serevr_s  = new ServerSocket(port);
		} catch(IOException e) {
		    System.out.println("Can't make ServerSocket");
			System.exit(0);
		}
		System.out.println("Main of ThreadEchoTCPServer has Started.");

		while(true) {
		    try{
		    	c_socket= serevr_s.accept();
		     	System.out.println(c_socket + "New Client Connected!");
		     	ThreadEchoTCPServer server = new ThreadEchoTCPServer(c_socket);
		    	server.start();
		    } catch(IOException e){ 
		     	System.out.println("IO Exception :" + e.getMessage());
		    }
		} 	
	}
}