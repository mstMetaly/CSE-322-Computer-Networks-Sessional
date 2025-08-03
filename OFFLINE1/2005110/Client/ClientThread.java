package Client;

import java.io.*;
import java.net.Socket;

public class ClientThread extends Thread{
    Socket socket;
    File file;

    public ClientThread(Socket socket , File file)
    {
        this.socket = socket;
        this.file=file;
    }

    public void run()
    {
        while (true) {
            FileInputStream fileInputStream = null;
            try {
                fileInputStream = new FileInputStream(file);
            } catch (FileNotFoundException e) {
                throw new RuntimeException(e);
            }

            try {
                OutputStream outputStream = socket.getOutputStream();
                outputStream.write(("UPLOAD " + file.getName() + "\n").getBytes());
                //outputStream.flush();
                long fileSize = file.length();
                outputStream.write((fileSize + "\n").getBytes());
                //outputStream.flush();

                byte[] buffer = new byte[512];
                int bytesRead;
                while ((bytesRead = fileInputStream.read(buffer)) != -1) {
                    outputStream.write(buffer, 0, bytesRead);
                    //outputStream.flush();
                }
                outputStream.write("\n".getBytes());

                outputStream.flush(); //////

//                System.out.println("File upload completed");

            } catch (Exception e) {
                e.printStackTrace();
            }

            break;
        }

    }
}
