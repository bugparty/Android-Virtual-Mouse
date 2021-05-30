//
// Created by fancy on 2021/5/29.
//

#ifndef MY_APPLICATION_VIRTUALMOUSE_H
#define MY_APPLICATION_VIRTUALMOUSE_H


class VirtualMouse {
public:
    int setupMouse();
    int destroy();
    int move(int x, int y);
    int click(int status, int keycode);
    void test();

    VirtualMouse():fd_mouse(-1){};
    ~ VirtualMouse(){
        destroy();
    }
private:
    int fd_mouse;
};


#endif //MY_APPLICATION_VIRTUALMOUSE_H
