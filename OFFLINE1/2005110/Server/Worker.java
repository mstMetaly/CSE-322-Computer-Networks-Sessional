package Server;

import java.io.IOException;
import java.net.Socket;

public class Worker extends Thread{
    Socket socket;
    HandleClient handleClient;

    public Worker(Socket socket)
    {
        this.socket = socket;
    }

    public void run()
    {
        try{
            while (true)
            {
                handleClient.handleClientRequest(socket);
            }

        }
        catch (IOException e)
        {
            e.printStackTrace();
        }
    }


}
