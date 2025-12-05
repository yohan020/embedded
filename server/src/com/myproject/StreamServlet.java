package com.myproject;

import jakarta.servlet.ServletException;
import jakarta.servlet.annotation.WebServlet;
import jakarta.servlet.http.HttpServlet;
import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;
import java.io.IOException;
import java.io.OutputStream;

@WebServlet("/stream")
public class StreamServlet extends HttpServlet {
    @Override
    protected void doGet(HttpServletRequest req, HttpServletResponse resp) throws ServletException, IOException {
        resp.setContentType("multipart/x-mixed-replace; boundary=--frame");
        OutputStream out = resp.getOutputStream();

        while (true) {
            byte[] frame = ImageBuffer.currentFrame;
            if (frame != null) {
                try {
                    out.write(("--frame\r\n").getBytes());
                    out.write(("Content-Type: image/jpeg\r\n").getBytes());
                    out.write(("Content-Length: " + frame.length + "\r\n\r\n").getBytes());
                    out.write(frame);
                    out.write(("\r\n").getBytes());
                    out.flush();
                    Thread.sleep(50); // 약 20FPS 조절
                } catch (Exception e) {
                    break; // 브라우저 종료 시 루프 탈출
                }
            }
        }
    }
}