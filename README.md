Here's the translated markdown:

# Virtual Mouse
## Testing Process

### Through Compilation

Click `build` in Android Studio

### Obtain Permissions
```shell script
adb root

adb remount
```

### Install Program
```shell script
adb push build/intermediates/cmake/debug/obj/arm64-v8a/virtualMouse /data/data/com.example.myapplication/files/
```

### Upgrade Permissions and Start Service
Enter adb shell
```shell script
cd /data/data/com.example.myapplication/files/
# Upgrade permissions
chmod +x virtualMouse && ./virtualMouse
```

The server should now be running.

If you encounter a `bind failed` error, it might be because a previous `server` has not exited. `kill` it and wait a minute before trying again.

### Run the Application on the Mobile Device
Run the apk and click `connect server` to test.
