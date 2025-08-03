package Client;

import java.io.File;
import java.io.IOException;
import java.io.OutputStream;
import java.net.Socket;
import java.util.Arrays;
import java.util.List;
import java.util.Scanner;

public class Client2 {

    private static final List<String> allowedExtensions = Arrays.asList("txt", "jpg", "png", "mp4");
    public static String getFileExtension(String fileName) {
        int lastIndexOfDot = fileName.lastIndexOf(".");
        if (lastIndexOfDot == -1) {
            return ""; // No extension found
        }
        return fileName.substring(lastIndexOfDot + 1); // Extract extension
    }

    public static boolean isFileUploadable(String  fileName) {
        String extension = getFileExtension(fileName);
        return allowedExtensions.contains(extension.toLowerCase());
    }

    public static void main(String[] args) throws IOException {
        Socket socket = new Socket("localhost",3000);
        System.out.println("Client connected.");
        Scanner sc = new Scanner(System.in);

        OutputStream outputStream = socket.getOutputStream();

        while (true)
        {
            System.out.print("Enter Command + fileName : ");
            String input = sc.nextLine();
            String[] arr = input.split(" ");
            if(arr.length == 1)
            {
                System.out.println("Please enter fileName. Wrong format input provided");
                continue;
            }

            String fileName = arr[1];

            if(!arr[0].equalsIgnoreCase("UPLOAD"))
            {
                System.out.println("Command not right. use \"UPLOAD\" fileName");
                continue;
            }
            if(!isFileUploadable(fileName))
            {
                System.out.println("This type of format is not allowed to upload!");
                continue;
            }

            String currentDir = System.getProperty("user.dir");
            String baseDir = currentDir + "/Client/upload";
            File file = new File(baseDir,fileName);

            if(file.exists()){
                Thread t = new ClientThread(socket,file);
                t.start();
            }


        }
    }
}