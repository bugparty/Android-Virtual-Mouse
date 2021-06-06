adb root
adb remount
adb push app/build/intermediates/cmake/debug/obj/arm64-v8a/virtualMouse /data/data/com.example.myapplication/files
adb shell chmod +x /data/data/com.example.myapplication/files/virtualMouse
adb shell /data/data/com.example.myapplication/files/virtualMouse