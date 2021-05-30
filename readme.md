# 虚拟鼠标
## 测试流程

### 通过编译

点击AS中的`build`
![](http://mark-vue-oss.oss-cn-hangzhou.aliyuncs.com/pasteimageintomarkdown/2021-05-31/37740718193901.png?Expires=4776002037&OSSAccessKeyId=LTAI4G8kArj75ch3irL8mUUJ&Signature=yfIydhQVB4bH%2BNxGc%2FXgPOL9mhw%3D)

### 获取权限
```shell script
adb root

adb remount
```

### 安装程序
```shell script
adb push build/intermediates/cmake/debug/obj/arm64-v8a/virtualMouse /data/data/com.example.myapplication/files/
```

### 升级权限并启动服务
进入adb shell
```shell script
cd /data/data/com.example.myapplication/files/
# 提升权限
chmod +x virtualMouse && virtualMouse
```

此时服务器已经开始运行。


如果出现`bind failed`，可能之前的`server`没有退出，`kill`后等一分钟即可。

### 在手机端运行软件
运行apk，点击`connect server`就能测试了。

