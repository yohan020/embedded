<%@ page language="java" contentType="text/plain; charset=UTF-8" pageEncoding="UTF-8"%>
<%@ page import="java.io.*" %>

<%
    request.setCharactorEncoding("UTF-8");
    String action = request.getParameter("action");

    String recordFile = "/home/pi/embedded/record.txt";
    
    String result = "";

    if (action != null) {
        try {
            BufferedWriter writer = new BufferedWriter(new FileWriter(recordFile, false));
            
            if (action.equals("rec_start")) {
                writer.write("1");
                result = "녹화 시작 명령(1) 기록됨";
            } 
            else if (action.equals("rec_stop")) {
                writer.write("0");
                result = "녹화 중지 명령(0) 기록됨";
            }
            
            writer.close();
            out.print("성공: " + result);
            
        } catch (IOException e) {
            out.print("에러: " + e.getMessage());

        }
    }
    
%>