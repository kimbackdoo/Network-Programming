/*
delete package_name
javac -encoding UTF-8 UDP_MultiThread_Server.java
java UDP_MultiThread_Server port
*/

package network_programming;

import java.net.*;
import java.io.*;

public class UDP_MultiThread_Server extends Thread {
	DatagramSocket datasoc;
	int num;
	
	public UDP_MultiThread_Server(DatagramSocket datasoc, int num) {
		this.datasoc = datasoc;
		this.num = num;
	}
	
	@Override
	public void run() {
		byte[] receiveData = new byte[1024];
		byte[] sendData = new byte[1024];
		while(true) {
			try {
				DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);
				datasoc.receive(receivePacket);
				String sentence = new String(receivePacket.getData());
				InetAddress IPAddress = receivePacket.getAddress();
				int port = receivePacket.getPort();
				String capitalizedSentence = sentence.toUpperCase();
				sendData = capitalizedSentence.getBytes();
				DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, IPAddress, port);
				datasoc.send(sendPacket);
				System.out.println("Thread-" + num + " 동작 중 ---:");
			}
			catch(IOException e) {
				e.printStackTrace();
			}
		}
	}
	
	public static void main(String[] args) throws Exception {
		if(args.length != 1) {
			System.out.println("사용법 : Java UDP_MultiThread_Server port");
			System.exit(0);
		}
		
		int port = Integer.parseInt(args[0]);
		DatagramSocket datasoc = new DatagramSocket(port);
		
		for(int num=0; num<3; num++) {
			UDP_MultiThread_Server serv = new UDP_MultiThread_Server(datasoc, num);
			serv.start();
		}
	}
}