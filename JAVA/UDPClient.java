/*
	delete package_name
	javac -encoding UTF-8 UDPClient.java
	java UDPClient ip_address port
*/

package network_programming;

import java.net.*;
import java.io.*;

public class UDPClient {
	final static int MAXBUFFER = 512;
	
	public static void main(String[] args) {
		if(args.length != 2) {
			System.out.println("Usage : java file_name localhost port");
			System.exit(0);
		}
		
		byte buffer[] = new byte[MAXBUFFER];
		int bufferLength = 0;
		int port = Integer.parseInt(args[1]);
		
		try {
			InetAddress inetaddr = InetAddress.getByName(args[0]);
			DatagramSocket socket = new DatagramSocket();
			DatagramPacket send_packet, recv_packet;
			
			while(true) {
				System.out.print("Input Data (bye to quit) : ");
				bufferLength = System.in.read(buffer);
				send_packet = new DatagramPacket(buffer, buffer.length, inetaddr, port);
				socket.send(send_packet);
				recv_packet = new DatagramPacket(buffer, buffer.length);
				socket.receive(recv_packet);
				
				String result = new String(buffer, 0, bufferLength);
				System.out.println("Echo Data : " + result);
				if(result.indexOf("BYE") >= 0) break;
			}
		}
		catch(UnknownHostException ex) {
			System.out.println("Error int the host address");		
		}
		catch(IOException e) {
			System.out.println(e);
		}
	}
}