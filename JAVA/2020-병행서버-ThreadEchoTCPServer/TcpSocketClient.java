import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.net.*;

class HelloFrame extends Frame {	
	public HelloFrame (String title){ 
	    super(title);
	}
	public Dimension getPreferredSize(){
	    return new Dimension(460,300);
	}
}

class ExitListener implements WindowListener {
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

public class TcpSocketClient {
	Socket s;
	DataInputStream dis;
	DataOutputStream dos;
	TextArea memo;
	TextField field;
	Label label;
	String ServerAddr;
	int PORT;
	
	public static void main(String [] args){
		TcpSocketClient client = new TcpSocketClient();
		
		try{	
			client.ServerAddr = args[0];
			client.PORT = Integer.parseInt(args[1]);	
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
		
		Frame MainFrame = new HelloFrame("Echo Client");
		MainFrame.addWindowListener(new ExitListener());
		MainFrame.pack();
		
		label = new Label("Connection completed!");
		memo = new TextArea();
		memo.setEditable(false);
		field = new TextField();	
		field.addActionListener(new SendListener()); 
		field.setColumns(50);
		MainFrame.add(label, BorderLayout.NORTH);			
		MainFrame.add(memo, BorderLayout.CENTER);		
		MainFrame.add(field, BorderLayout.SOUTH);		
		MainFrame.setVisible(true);
	}

	class SendListener implements ActionListener {
		public void actionPerformed(ActionEvent ae) {
			String recvMsg;
			String sendMsg = field.getText();
			try {
				dos.writeUTF(sendMsg);				
				dos.flush();	
				memo.append("Client Messsage :");
				memo.append(sendMsg + "\n");
			} catch(IOException e){
				memo.append("Sending error\n");
			}
			field.setText("");
			try {
				recvMsg = dis.readUTF() + '\n';
				memo.append("Server Message :");
				memo.append(recvMsg + "\n");
				Thread.sleep(20000); 
			} catch(IOException e){
				memo.append("Sending error\n");
			} catch(InterruptedException e) {
				e.printStackTrace();
			}
			System.exit(0);
		}
	}
}