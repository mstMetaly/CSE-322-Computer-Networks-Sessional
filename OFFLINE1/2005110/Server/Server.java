package Server;

import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;


public class Server {
    static final int PORT = 3000;

    public static void main(String[] args) throws IOException {

        ServerSocket serverSocket = new ServerSocket(PORT);
        System.out.println("Server started.\nListening for connections on port : " + PORT + " ...\n");

        while (true)
        {
            Socket socket = serverSocket.accept();
            System.out.println("Client connected.");

            Thread worker = new Worker(socket);
            worker.start();

            //socket.close();--extra
        }

    }

}
