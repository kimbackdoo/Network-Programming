//IPAddress.java with Swing 

import java.awt.BorderLayout;
import java.awt.EventQueue;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.ScrollPaneConstants;
import javax.swing.border.EmptyBorder;
import java.awt.*;
import java.awt.event.*;
import java.net.*;
import java.io.*;

public class IPAddress {
	JTextField inputText;
	JTextArea output;
	JScrollPane scrollPane;
	
	public static void main(String args[]) {
		new IPAddress().work();
	}

	public void work() {
			JFrame f = new JFrame("IPAddress");
			output = new JTextArea();
			output.setEditable(false);
			output.setText("NGN \uC5F0\uAD6C\uC2E4");
			inputText = new JTextField("<Type Host Name here>");
			inputText.addActionListener(new AddressListener());
			JScrollPane scrollPane = new JScrollPane(output); 
			scrollPane.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS);
			
			
			f.getContentPane().add(scrollPane, BorderLayout.CENTER);
			f.getContentPane().add(inputText, BorderLayout.SOUTH);
			f.setSize(400, 300);
			f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
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

//end of File 