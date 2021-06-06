package com.example.MousePad;

import android.os.Bundle;
import android.os.Message;
import android.os.SystemClock;
import android.util.Log;
import android.view.MotionEvent;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import com.example.MousePad.databinding.ActivityMainBinding;

import static android.view.MotionEvent.ACTION_DOWN;
import static android.view.MotionEvent.ACTION_MOVE;
import static android.view.MotionEvent.ACTION_UP;
import static java.lang.Math.abs;

public class MainActivity extends AppCompatActivity {
    private final static String TAG = MainActivity.class.getSimpleName();

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    private ActivityMainBinding binding;
    private MouseClient mouseClient = null;

    void initialMouse() {
        if (mouseClient == null) {
            synchronized (MouseClient.class) {
                if (mouseClient == null) {
                    mouseClient = new MouseClient();
                }
            }
        }
        Message msg = Message.obtain();
        msg.what = MouseClient.TYPE_CONNECT;
        mouseClient.sendMessage(msg);
    }

    boolean firstTouch = true;
    float lastX;
    float lastY;
    boolean isPressing = false;
    float offX = 0, offY = 0;
    long lastClick = 0;
    long doubleClickTime = 1000l;
    @Override
    public boolean onTouchEvent(MotionEvent event) {
        int index = event.getActionIndex();

        float x = event.getX();
        float y = event.getY();


        offX = x - lastX;
        offY = y - lastY;
        lastX = x;
        lastY = y;

        if (event.getAction() == ACTION_UP) {
            Log.d(TAG, "up");
            if (SystemClock.elapsedRealtime()- lastClick < doubleClickTime){
                lastClick = 0;//重置状态,需要两次点击才能启动
                Log.d(TAG, "double click");
                //click
                if (mouseClient != null) {

                    Message msg = Message.obtain();
                    msg.what = MouseClient.TYPE_CLICK;
                    msg.arg1 = MouseClient.BTN_MOUSE;
                    msg.arg2 = MouseClient.STATUS_SINGLE_CLICK;
                    mouseClient.sendMessage(msg);

                }
            }else{
                //跟踪最近一次点击时间
                lastClick = SystemClock.elapsedRealtime();
            }


            isPressing = false;
        } else if (event.getAction() == ACTION_DOWN) {
            isPressing = true;
            lastX = event.getX();
            lastY = event.getY();
            //click
            Log.d(TAG, "down");

        } else if (event.getAction() == ACTION_MOVE) {
            if (isPressing == true && abs(offX) > 0.5 && abs(offY) > 0.5) {
                //mouse move offset are big enough to move
                int iOffX = (int) offX * 2;
                int iOffY = (int) offY * 2;
                Log.d(TAG, String.format("move %d %d", iOffX, iOffY));
                if (mouseClient != null) {
                    Message msg = Message.obtain();
                    msg.what = MouseClient.TYPE_MOVE;
                    msg.arg1 = iOffX;
                    msg.arg2 = iOffY;
                    mouseClient.sendMessage(msg);
                }
            }
        }


        Log.v("motion dispatch",
                String.format("motion_type: %d, x: %f, y: %f, %f, %f", event.getAction(), x, y,
                        offX, offY)
        );
        return super.onTouchEvent(event);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        Log.d(TAG, String.format("path %s", getExecuteLocation()));
        // Example of a call to a native method
        TextView tv = binding.sampleText;
        tv.setText(stringFromJNI());
        binding.buttonStart.setOnClickListener((v) -> {
            VirualMousePilot pilot = new VirualMousePilot(this);
            pilot.startServer();
        });
        binding.buttonConnect.setOnClickListener((v) -> {
            initialMouse();
        });
        binding.buttonLeft.setOnClickListener((v) -> {
            if (mouseClient == null) return;
            Message msg = Message.obtain();
            msg.what = MouseClient.TYPE_MOVE;
            msg.arg1 = -5;
            msg.arg2 = 0;
            mouseClient.sendMessage(msg);
        });
        binding.buttonRight.setOnClickListener((v) -> {
            if (mouseClient == null) return;
            Message msg = Message.obtain();
            msg.what = MouseClient.TYPE_MOVE;
            msg.arg1 = 5;
            msg.arg2 = 0;
            mouseClient.sendMessage(msg);
        });
        binding.buttonUp.setOnClickListener((v) -> {
            if (mouseClient == null) return;
            Message msg = Message.obtain();
            msg.what = MouseClient.TYPE_MOVE;
            msg.arg1 = 0;
            msg.arg2 = -5;
            mouseClient.sendMessage(msg);
        });
        binding.buttonDown.setOnClickListener((v) -> {
            if (mouseClient == null) return;
            Message msg = Message.obtain();
            msg.what = MouseClient.TYPE_MOVE;
            msg.arg1 = 0;
            msg.arg2 = 5;
            mouseClient.sendMessage(msg);
        });
    }

    private String getExecuteLocation() {
        return ExtractFile.extract(getApplicationContext());
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    public native void mouseTest();
}