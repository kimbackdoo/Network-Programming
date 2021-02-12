// 파일명 : CountByte.java
// 기능 : 파일의 바이트 수를 센다.
// 컴파일 : javac CountByte.java
// 사용법 : java CountByte CountByte.java

import java.io.*;
class CountByte{
	public static void main(String [] args) throws IOException {
		try{
			// �Է� ���� ��Ʈ�� ����
			InputStream is = new FileInputStream(args[0]);
			int total = 0;
			
			while(is.read() != -1) total++;		// read()�޼���� 1����Ʈ ����
			
			System.out.println(args[0]+ " has " + total + " bytes.");
		} catch(FileNotFoundException e) {
			// ������ ���� �� ȭ�鿡 ���� ���
			System.out.println("File Open Error!");
		}
		// �����߻��� ������� ȭ�鿡 ���
		System.out.println("End of main.");
	}
}