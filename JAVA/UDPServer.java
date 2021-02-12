/*
	delete package_name
	javac -encoding UTF-8 UDPServer.java
	java UDPServer port
*/

package network_programming;

import java.net.*;
import java.io.*;

public class UDPServer {
	final int MAXBUFFER = 512;
	
	public static void main(String[] args) {
		if(args.length != 1) {
			System.out.println("Usage : java file_name port");
			System.exit(0);
		}
		
		int arg_port = Integer.parseInt(args[0]);
		new UDPServer().work(arg_port);
	}
	
	public void work(int arg_port) {
		int port = arg_port;
		byte buffer[] = new byte[MAXBUFFER];
		char ch;
		
		try {
			DatagramSocket socket = new DatagramSocket(port);
			DatagramPacket recv_packet;
			System.out.println("Running the UDP Echo Server...");
			
			while(true) {
				recv_packet = new DatagramPacket(buffer, buffer.length);
				socket.receive(recv_packet);
				
				String sentence = new String(recv_packet.getData());
				String echo_sentence = "";
				/* Client에서 받은 데이터를 대문자를 소문자로 소문자는 대문자로 변화, 만약 숫자일 경우에는 바로 출력*/
				for(int i=0; i<sentence.length(); i++) {
					ch = sentence.charAt(i);
					if('A'<=ch && ch<='Z')
						echo_sentence += String.valueOf(ch).toLowerCase();
					else if('a'<=ch && ch<='z')
						echo_sentence += String.valueOf(ch).toUpperCase();
					else
						echo_sentence += String.valueOf(ch);
				}
				
				byte tx_buffer[] = new byte[MAXBUFFER];
				/* 받은 데이터를 인코딩하지 않으면 데이터가 손상될 수 있으므로
				 클라이언트로부터 받은 데이터를 인코딩하여 tx_buffer에 저장 */
				/* 현재 getBytes 메소드의 인자가 없으므로 자바의 default charset을 이용하여 바이트 배열로 변환*/
				tx_buffer = echo_sentence.getBytes();
				
				DatagramPacket send_packet = new DatagramPacket(
						tx_buffer, recv_packet.getLength(),
						recv_packet.getAddress(), recv_packet.getPort());
				
				socket.send(send_packet);
			}
		}
		catch(IOException e) {
			System.out.println(e);
		}
	}
}