// 파일명 : IPAddress.java
// 기능 : 자신의 IP 주소 및 임의의 호스트의 IP 주소 얻기
// 컴파일 : javac IPAddress.java
// 사용법 : java IPAddress

import java.awt.*;
import java.awt.event.*;
import java.net.*;
import java.io.*;

public class IPAddress {
	TextField inputText;
	TextArea output;
	
	public static void main(String args[]) {
		new IPAddress().work();
	}

	public void work() {
			Frame f = new Frame("IPAddress");
			output = new TextArea();
			inputText = new TextField("<Type Host Name here>");
			inputText.addActionListener(new AddressListener());
			f.add(output, "Center");
			f.add(inputText, "South");
			f.setSize(400, 300);
			f.setVisible(true);
			// �ڽ��� IP �ּ� ã��
			try {
				InetAddress inetaddr = InetAddress.getLocalHost();
				output.append("\nYour Host name is : " + inetaddr.getHostName ());
				output.append("\nYour IP Address is : " + inetaddr.getHostAddress ());
			}catch (UnknownHostException ex){
				output.append ("\nError in getLocalHost()\n");
			}
	}
	
	// ������ ȣ��Ʈ IP �ּ� ã��
	class AddressListener implements ActionListener {
			public void actionPerformed(ActionEvent ae) {
				String h_name = inputText.getText();
				try {
						InetAddress inetaddr = InetAddress.getByName(h_name);
						output.append ("\n\nFor the Host : " + inetaddr.getHostName());
						output.append ("\nIP Address is : " + inetaddr.getHostAddress());
				}catch (UnknownHostException ex) {
					output.append ("\nFailed to find : " + h_name);
			}
		}
	}
	
}
