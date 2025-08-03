package Server;

import java.io.*;
import java.net.Socket;
import java.net.URLDecoder;
import java.nio.channels.FileChannel;
import java.nio.charset.StandardCharsets;
import java.util.Date;

public class HandleClient{

    public HandleClient()
    {

    }

    public static void sendErrorResponse(OutputStream os , int statusCode , String statusMessage , String errorMessage) throws IOException {
        String s = "<html><body>" + "<h1>" + statusCode + " " + statusMessage + "</h1>" +
                "<p>" + errorMessage + "</p>" + "</body></html>";
        StringBuilder responseBody = new StringBuilder(s);

        os.write(("HTTP/1.1 " + statusCode + " " + statusMessage + "\r\n").getBytes());
        os.write("Server: Java HTTP Server: 1.0\r\n".getBytes());
        os.write(("Date: " + new Date() + "\r\n").getBytes());
        os.write(("Content-Type: text/html"+"\r\n").getBytes());
        os.write(("Content-Length: " + responseBody.length() + "\r\n").getBytes());
        os.write("\r\n".getBytes());
        os.write(String.valueOf(responseBody).getBytes());
       // os.flush();
    }

    public static void sendDirectoryResponse(StringBuilder responseBody, OutputStream os,String contentType) throws IOException {
        os.write("HTTP/1.1 200 OK\r\n".getBytes());
        os.write("Server: Java HTTP Server: 1.0\r\n".getBytes());
        os.write(("Date: " + new Date() + "\r\n").getBytes());
        os.write(("Content-Type: "+contentType +"\r\n").getBytes());
        os.write(("Content-Length: " + responseBody.length() + "\r\n").getBytes());
        os.write("\r\n".getBytes());
        os.write(String.valueOf(responseBody).getBytes());
       // os.flush();

    }

    public static void sendFileResponse( OutputStream outputStream , File file , String contentType ) throws IOException {
        outputStream.write("HTTP/1.1 200 OK\r\n".getBytes());
        outputStream.write(("Content-Type: " + contentType + "\r\n").getBytes());
        outputStream.write(("Content-Length: " + file.length() + "\r\n").getBytes());
        outputStream.write("\r\n".getBytes()); // End of headers
        //outputStream.flush();

        FileInputStream fileInputStream = new FileInputStream(file);
        byte[] buffer = new byte[512];
        int bytesRead;
        while((bytesRead = fileInputStream.read(buffer)) != -1)
        {
            outputStream.write(buffer , 0 , bytesRead);
        }
        //outputStream.flush();
    }


    public static String extractPath(String input) throws UnsupportedEncodingException {
        String path = "";
        if(input == null)
            return "";
        else if(input.equalsIgnoreCase("GET /favicon.ico HTTP/1.1"))
            return "";
        else if(input.startsWith("GET")){
            String[] requestParts = input.split(" /");
            String tempPath = requestParts[1];
            String[] temp = tempPath.split(" HTTP/1.1");
            path = temp[0];
        }

        String decodedPath = URLDecoder.decode(path, StandardCharsets.UTF_8.toString());
        return decodedPath;

    }

    public static String getContentType(File file) {
        String fileName = file.getName().toLowerCase();
        if(fileName.endsWith("txt"))
            return "text/html";
        if (fileName.endsWith(".jpg") || fileName.endsWith(".jpeg")) {
            return "image/jpeg";
        } else if (fileName.endsWith(".png")) {
            return "image/png";
        } else if (fileName.endsWith(".gif")) {
            return "image/gif";
        } else {
            return "application/octet-stream";
        }
    }



    //handle file
    public static void  handleFile(String path , StringBuilder responseBody , OutputStream outputStream) throws IOException {

        File file = new File(path);
        if(file.exists())
        {
            String contentType = getContentType(file);
            sendFileResponse(outputStream,file,contentType);

        }
        else{
            sendErrorResponse(outputStream,404,"NOT FOUND", "File Not Found");
        }

    }


    //list all files for decoded path
    public static void handlePath(String decodedPath , StringBuilder responseBody , OutputStream outputStream) throws IOException {
        File directory = new File(decodedPath);
        File[] files = null;

        if(directory.isDirectory())
        {
            files = directory.listFiles();
            for(File file : files)
            {
                String fullPath = decodedPath + "/" + file.getName();  // Ensure full relative path
                fullPath = fullPath.replace(" ", "%20");
                if (!fullPath.startsWith("/")) {
                    fullPath = "/" + fullPath;
                }
                System.out.println("Full path: "+ fullPath);
                responseBody.append("<li><a href=\"").append(fullPath).append("\">").append(file.getName()).append("</a></li>");
            }

            responseBody.append("</body></html>");
            sendDirectoryResponse(responseBody , outputStream , "text/html");//??
        }
        else
        {
            handleFile(decodedPath , responseBody ,outputStream);
        }

    }


    //create response body
    public static void handleGetRequest(String input ,  OutputStream outputStream) throws IOException {

        String decodedPath = extractPath(input);

        if(decodedPath == null)
        {
            sendErrorResponse(outputStream,404, "Not found", "No path exists like this");
            System.out.println("Path not found!");
        }

        String s = "<html><body><h1>Directory listing for " + decodedPath + "</h1><ul>";
        StringBuilder responseBody = new StringBuilder(s);

        //handle the directory/file
        handlePath(decodedPath , responseBody  ,outputStream);

    }


    public static String readLine(InputStream inputStream) throws IOException {
        ByteArrayOutputStream lineBuffer = new ByteArrayOutputStream();
        int ch;
        boolean foundNewline = false;
        while ((ch = inputStream.read()) != -1) {
            if (ch == '\r') {
                // Check for newline character after carriage return
                if (inputStream.read() == '\n') {
                    foundNewline = true;
                    break;
                }
            } else if (ch == '\n') {
                foundNewline = true;
                break;
            }
            lineBuffer.write(ch);
        }
        if (!foundNewline && lineBuffer.size() == 0) {
            // Handle end-of-stream or empty line scenario
            return null; // or throw new IOException("End of stream reached");
        }
        return lineBuffer.toString("UTF-8");
    }



    //handles client request
    public static void handleClientRequest(Socket socket) throws IOException {

        //initializing input stream
        InputStream inputStream = socket.getInputStream();
        //initializing outputStream
        OutputStream outputStream = socket.getOutputStream();

        //for upload
        String input = readLine(inputStream);

        //log.txt
        String currentDir = System.getProperty("user.dir");
        String baseDir = currentDir + "/";
        File logFile = new File(baseDir,"log.txt");
        FileOutputStream logOutputStream = new FileOutputStream(logFile,true);


//        if(input != null)
//            System.out.println("request: " + input);

        if(input == null)
        {
            return;
        }
        if (input != null && input.contains("/favicon.ico")) {
            System.out.println("Ignoring /favicon.ico request");
            outputStream.write("HTTP/1.1 204 No Content\r\n".getBytes());
            outputStream.write("Server: Java HTTP Server: 1.0\r\n".getBytes());
            outputStream.write(("Date: " + new Date() + "\r\n").getBytes());
            outputStream.write("Content-Length: 0\r\n".getBytes());
            outputStream.write("\r\n".getBytes());
            //outputStream.flush();

            return;  // Exit the method to stop processing this request
        }
        if(input.startsWith("GET"))
        {
            logOutputStream.flush();
            logOutputStream.write(("[" + new Date() + "] Request: " + input + "\n").getBytes());
            handleGetRequest(input,  outputStream );
        }
        else if(input.startsWith("UPLOAD"))
        {
            logOutputStream.flush();
            logOutputStream.write(("[" + new Date() + "] Request: " + input + "\n").getBytes() );

            String[] arr = input.split(" ");
            String fileName = arr[1];
            long fileSize = Long.parseLong(readLine(inputStream));
            System.out.println("here before calling handle upload file " + fileName + "  "+fileSize);
            handleUploadfile(fileName, fileSize, inputStream );
        }

        logOutputStream.flush();

    }


    //handle file upload
    public  static  void handleUploadfile(String fileName,long fileSize, InputStream inputStream ) throws IOException {

            String currentDir = System.getProperty("user.dir");
            String baseDir = currentDir + "/Server/uploaded";
            File file = new File(baseDir, fileName);

            // Ensure directory exists
            File directory = file.getParentFile();
            if (!directory.exists()) {
                directory.mkdirs(); // Create directory if it does not exist
            }


            FileOutputStream fileOutputStream = new FileOutputStream(file);

            try{
            //FileChannel fileChannel = fileOutputStream.getChannel();
            byte[] buffer = new byte[4096];
            int bytesRead ;
            int totalBytesRead = 0;
            while(totalBytesRead < fileSize && (bytesRead = inputStream.read(buffer)) != -1)
            {
                System.out.println("in server------:"+bytesRead);
                fileOutputStream.write(buffer , 0 , bytesRead);
               // fileOutputStream.flush();

                // Force the changes to be written to disk
              //  fileChannel.force(true);
                totalBytesRead+=bytesRead;
            }

            fileOutputStream.flush();

            if(totalBytesRead == 0)
            {
                System.out.println("File upload completed. Total bytes read: " + 0);
            }
            else{
                System.out.println("File upload completed. Total bytes read: " + (totalBytesRead-1));
            }

            if (file.exists() && (totalBytesRead-1) == fileSize){
                System.out.println("File received successfully and is fully written.");
            } else {
                System.out.println("File upload incomplete or file is missing.");
            }

        }
        catch (Exception e)
        {
            System.out.println("Error while uploading file to server");
            e.printStackTrace();
        }
        finally {
           fileOutputStream.close();
        }

    }


}
