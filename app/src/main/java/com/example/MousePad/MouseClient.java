package com.example.MousePad;


import android.os.Handler;
import android.os.Message;
import android.util.Log;

import androidx.annotation.NonNull;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.UnknownHostException;
import java.nio.ByteBuffer;

public class MouseClient extends Handler {
    private static final String TAG = MouseClient.class.getSimpleName();
    private static final String HOST = "127.0.0.1";
    public static final int PORT = 18887;
    public static final int TYPE_MOVE = 2;
    public static final int TYPE_CLICK = 3;
    public static final int TYPE_CONNECT = 4;
    public static final int BTN_MOUSE  = 0x110;

    public static final int STATUS_SINGLE_CLICK = 3;
    Socket socket = null;
    private void connect() {
        try {
            socket = new Socket(HOST, PORT);
            Log.d(TAG, "connected");
        } catch (UnknownHostException e) {
            Log.e(TAG, e.getLocalizedMessage());
        } catch (IOException e) {
            Log.e(TAG, e.getLocalizedMessage());
        }
    }
    private void sendMove(int x, int y){
        if (null == socket)return;

        try {
            OutputStream output = socket.getOutputStream();

            InputStream input = socket.getInputStream();
            ByteBuffer buf = ByteBuffer.allocate(12);
            buf.putInt(2);
            buf.putInt(x);
            buf.putInt(y);

            output.write(buf.array());
            output.flush();

            int ret = input.read();

            System.out.println(String.format("server return %s",ret));
        } catch (IOException e) {
            e.printStackTrace();
        }

    }
    private void sendClick(int status){
        if (null == socket)return;

        try {
            OutputStream output = socket.getOutputStream();

            InputStream input = socket.getInputStream();
            ByteBuffer buf = ByteBuffer.allocate(12);
            if (status == MouseClient.STATUS_SINGLE_CLICK){
                buf.putInt(3);
                buf.putInt(BTN_MOUSE);
                buf.putInt(1);
                output.write(buf.array());
                buf.clear();
                buf.putInt(3);
                buf.putInt(BTN_MOUSE);
                buf.putInt(0);
                output.write(buf.array());
            }else{
                buf.putInt(3);
                buf.putInt(BTN_MOUSE);
                buf.putInt(status);

            }


            output.flush();

            int ret = input.read();

            System.out.println(String.format("server return %s",ret));
        } catch (IOException e) {
            e.printStackTrace();
        }

    }
    @Override
    public void handleMessage(@NonNull Message msg) {
        switch (msg.what){
            case TYPE_CLICK:
                final int status = msg.arg2;
                post(() -> sendClick(status));
                break;
            case TYPE_MOVE:
                final int x = msg.arg1;
                final int y = msg.arg2;
                post(() -> {
                    //Log.d(TAG, String.format("msg %i %i %i", msg.what, msg.arg1, msg.arg2));
                    sendMove(x, y);
                });
                break;
            case TYPE_CONNECT:
                post(() -> connect());
                break;

        }
    }
}
