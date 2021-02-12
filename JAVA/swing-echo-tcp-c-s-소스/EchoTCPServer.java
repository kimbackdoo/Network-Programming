import java.io.*;
import java.net.*;
public class EchoTCPServer{

	public static void main(String [] args){
		EchoTCPServer server = new EchoTCPServer(args);
		server.run();
	}
	ServerSocket ss = null;
	Socket s = null;
	DataInputStream dis;
	DataOutputStream dos;
	
	final int PORT = 5000;		

	EchoTCPServer(String[] args){
		try{
			ss = new ServerSocket(PORT);
		}catch(IOException e){
			System.out.println("Can't make ServerSocket");
			System.exit(0);
		}
		System.out.println("Java	Server started. Accepting the client...");
	}
	
	public void run(){
		String msg;
		try{
			s = ss.accept();							
			dis = new DataInputStream(s.getInputStream());
			dos = new DataOutputStream(s.getOutputStream());
			System.out.println("New Client!");
			while(true){
				msg = dis.readUTF();
				System.out.println("Client message: " + msg);
				dos.writeUTF(msg);
			}
		}catch(IOException e){
			System.out.println("IO Exception :" + e.getMessage());
		}
	}
}