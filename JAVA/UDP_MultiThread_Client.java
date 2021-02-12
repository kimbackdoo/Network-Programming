/*
delete package_name
javac -encoding UTF-8 UDP_MultiThread_Client.java
java UDP_MultiThread_Client ip_address port
*/

package network_programming;

import java.net.*;
import java.io.*;

public class UDP_MultiThread_Client extends Thread{
	// 정보 은닉을 위해 모든 멤버변수를 private으로 선언
	private final static int MAXBUFFER = 512;
	private DatagramSocket socket;
	private int thread_num;
	/* main에서 아래 변수들을 객체를 생성안하고 사용하기 위해 static으로 선언 */
	private static int port, bufferLength = 0;
	private static String result;
	private static InetAddress inetaddr;
	private static byte buffer[] = new byte[MAXBUFFER];
	
	public UDP_MultiThread_Client(DatagramSocket socket, int thread_num) {
		this.socket = socket;
		this.thread_num = thread_num;
	}
	/* 쓰레드가 생성되고 쓰레드에서 데이터를 송수신 */
	@Override
	public void run() {
		DatagramPacket send_packet, recv_packet;
		try {
			send_packet = new DatagramPacket(buffer, buffer.length, inetaddr, port);
			socket.send(send_packet);
			recv_packet = new DatagramPacket(buffer, buffer.length);
			socket.receive(recv_packet);
			result = new String(buffer, 0, bufferLength);
			System.out.println(" Echo Data : " + result + " ---Thread-" + thread_num + " 동작 중---");
			System.out.println();
		}
		catch(IOException e) {
			System.out.println(e);
		}
	}
	
	public static void main(String[] args) {
		if(args.length != 2) {
			System.out.println("Usage : java file_name localhost port");
			System.exit(0);
		}
		port = Integer.parseInt(args[1]);
		
		try {
			inetaddr = InetAddress.getByName(args[0]);
			DatagramSocket socket = new DatagramSocket();
			
			// 사용자로부터 문자를 입력 받고 차례대로 쓰레드를 생성하므로 쓰레드 동작도 차례대로 동작하게 됩니다.
			for(int thread_num=0; thread_num<5; thread_num++) {
				System.out.print("Input Data (bye to quit) : ");
				bufferLength = System.in.read(buffer);
				// 쓰레드 생성 및 동작
				UDP_MultiThread_Client c = new UDP_MultiThread_Client(socket, thread_num);
				c.start();
				// main 쓰레드와 별도로 만든 쓰레드가 동시다발적으로 동작하지 않게 하기 위해 sleep 메소드를 이용
				Thread.sleep(1000);
			}
		}
		catch(UnknownHostException ex) {
			System.out.println("Error int the host address");		
		}
		catch(IOException e) {
			System.out.println(e);
		}
		catch(InterruptedException e) {
			e.printStackTrace();
		}
	}
}