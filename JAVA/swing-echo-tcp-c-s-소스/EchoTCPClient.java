import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.net.*;
import java.awt.*;
import javax.swing.*;

class HelloFrame extends JFrame {
	public HelloFrame (String title){
		super(title);
	}
	public Dimension getPreferredSize(){
		return new Dimension(460,300);
	}
}

class ExitListener implements WindowListener{
	public void windowClosing(WindowEvent e){
		System.out.println("bye~");
		System.exit(0);
	}
	
	public void windowActivated(WindowEvent e) {}
	public void windowClosed(WindowEvent e) {}
	public void windowDeactivated(WindowEvent e) {}
	public void windowDeiconified(WindowEvent e) {}
	public void windowIconified(WindowEvent e) {}
	public void windowOpened(WindowEvent e) {}
}

public class EchoTCPClient{
	Socket s;
	DataInputStream dis;
	DataOutputStream dos;
	JTextArea memo;
	JTextField field;
	JLabel label;
	JScrollPane scrollPane;
	
	final int PORT = 5000;
	
	String ServerAddr;
	
	public static void main(String [] args){
		EchoTCPClient client = new EchoTCPClient();
		try{
			client.ServerAddr = args[0];
			
		} catch(ArrayIndexOutOfBoundsException e){
			System.out.println("Java Echo Server needs port number.");
			System.exit(0);
		}
		client.run();
	}
	
	public void run(){													
		try{
			s = new Socket(ServerAddr, PORT);
			dis = new DataInputStream(s.getInputStream());
			dos = new DataOutputStream(s.getOutputStream());
		} catch(IOException e){
			memo.append("Can't open Socket\n");
			return;
		}
		
		JFrame MainFrame = new HelloFrame("Echo Client");
		MainFrame.addWindowListener(new ExitListener());
		MainFrame.pack();
		
		label = new JLabel("Connection completed!");
		memo = new JTextArea();
		memo.setEditable(false);
		
		JScrollPane scrollPane = new JScrollPane(memo); 
		scrollPane.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS);
		
		field = new JTextField();	
		field.addActionListener(new SendListener());
		field.setColumns(50);
		
		MainFrame.getContentPane().add(scrollPane, BorderLayout.CENTER);
		MainFrame.getContentPane().add(label, BorderLayout.NORTH);
		//MainFrame.getContentPane().add(memo, BorderLayout.CENTER);
		MainFrame.getContentPane().add(field, BorderLayout.SOUTH);
		
		MainFrame.setVisible(true);
	}

	class SendListener implements ActionListener{
			public void actionPerformed(ActionEvent ae){
				String recvMsg;
				String sendMsg = field.getText();
				try {
					dos.writeUTF(sendMsg);
					dos.flush();
					
				} catch(IOException e){
					memo.append("Sending error\n");
				}
				memo.append("Client Messsage :");
				memo.append(sendMsg + "\n");
				field.setText("");
				
				try{
					recvMsg = dis.readUTF() + '\n';
					memo.append("Server Message :");
					memo.append(recvMsg + "\n");
				} catch(IOException e)
				{
					memo.append("read error");
				}
		}
	}
}