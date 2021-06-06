package com.example.MousePad;

import android.content.Context;
import android.util.Log;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;

public class VirualMousePilot {
    private final static String TAG = VirualMousePilot.class.getSimpleName();
    private final Context ctx;
    private String path;
    public VirualMousePilot(Context context){
        ctx = context;
    }
    private void runAsRoot(){
        new ExecuteAsRootBase() {
            @Override
            protected ArrayList<String> getCommandsToExecute() {
                ArrayList<String> l = new ArrayList<String>();
                l.add(String.format("chmod +x %s", path));
                l.add(path);

                return l;
            }
        }.execute();
    }
    public void startServer(){
        path = extractBinary();
        if (null != path){

            try{
                Process process = Runtime.getRuntime().exec(path);
                InputStream is = process.getInputStream();

                //sleep a while for result
                Thread.sleep(1000);
                if (is.available() > 0){
                    Log.d(TAG, "exec has Output");
                }
            } catch (IOException | InterruptedException e) {
                e.printStackTrace();
                Log.e(TAG, "exec failed",e );
            }

        }
    }
    private String extractBinary(){
        String path = null;
        try{
            final String FILENAME = "virtualMouse";
            InputStream is = ctx.getAssets().open("arm64-v8a/virtualMouse");
            byte [] buf = new byte [is.available()] ;
            is.read(buf);
            OutputStream os = ctx.openFileOutput(FILENAME, Context.MODE_PRIVATE);
            os.write(buf);
            os.flush();
            os.close();
            File file = ctx.getFileStreamPath (FILENAME);
            file.setExecutable(true);
            path = file.getAbsolutePath();
            Log.d(TAG, "file extracted to "+path);
        }catch (IOException e) {

            Log.e(TAG, "create file error",e);
        }
        return path;
    }
}
