package com.myproject;

import jakarta.servlet.ServletContextEvent;
import jakarta.servlet.ServletContextListener;
import jakarta.servlet.annotation.WebListener;
import java.io.DataInputStream;
import java.net.ServerSocket;
import java.net.Socket;

@WebListener
public class CameraReceiver implements ServletContextListener {
    
    private Thread worker;

    @Override
    public void contextInitialized(ServletContextEvent sce) {
        worker = new Thread(() -> {
            try (ServerSocket serverSocket = new ServerSocket(9999)) { // 9999포트 개방
                System.out.println("=== Camera Socket Server Started on Port 9999 ===");
                while (true) {
                    try {
                        Socket socket = serverSocket.accept();
                        DataInputStream dis = new DataInputStream(socket.getInputStream());
                        
                        while (true) {
                            // 1. 데이터 길이 읽기 (Python struct.pack(">L") 대응)
                            int len = dis.readInt();
                            byte[] buffer = new byte[len];
                            
                            // 2. 이미지 데이터 읽기
                            dis.readFully(buffer);
                            
                            // 3. 버퍼 업데이트
                            ImageBuffer.currentFrame = buffer;
                        }
                    } catch (Exception e) {
                        System.out.println("Camera Disconnected: " + e.getMessage());
                    }
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        });
        worker.start();
    }

    @Override
    public void contextDestroyed(ServletContextEvent sce) {
        if (worker != null && worker.isAlive()) worker.interrupt();
    }
}