# 测试流程

adb root
adb remount

点击build
adb push build/intermediates/cmake/debug/obj/arm64-v8a/virtualMouse /data/data/com.example.myapplication/files/
进入adb shell
cd /data/data/com.example.myapplication/files/
chmod +x virtualMouse && virtualMouse
