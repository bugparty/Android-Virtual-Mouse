package com.example.MousePad;

import android.content.Context;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class ExtractFile {
    public static String extract(Context context){
        String path = null;
        try{
            final String FILENAME = "virtualMouse";
            InputStream is = context.getAssets().open("arm64-v8a/virtualMouse");
            byte [] buf = new byte [is.available()] ;
            OutputStream os = context.openFileOutput(FILENAME, Context.MODE_PRIVATE);
            os.write(buf);
            os.close();
            File file = context.getFileStreamPath (FILENAME);
            file.setExecutable(true);
            path = file.getAbsolutePath();
        }catch (IOException e) {
            e.printStackTrace();
        }
        return path;
    }
}

