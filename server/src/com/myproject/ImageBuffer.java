package com.myproject;

public class ImageBuffer {
    // Python에서 받은 최신 이미지를 저장하는 공간
    public static volatile byte[] currentFrame = null;
}