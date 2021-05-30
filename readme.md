# 测试流程

adb root
adb remount

点击build
adb push build/intermediates/cmake/debug/obj/arm64-v8a/virtualMouse /data/data/com.example.myapplication/files/
进入adb shell
cd /data/data/com.example.myapplication/files/
chmod +x virtualMouse && virtualMouse
此时服务器已经开始运行
如果出现bind failed,可能之前的server没有退出,kill后等一分钟即可
之后运行apk点击connect server就能测试乐
