//
// Created by fancy on 2021/5/29.
//

#include "virtualMouse.h"
#include <linux/uinput.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <time.h>
#include <cmath>

static int uinp_fd;
struct uinput_user_dev uinp;
struct input_event event;

void register_device() {

    int i = 50;

    uinp_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (NULL == uinp_fd) {
        printf("/dev/uinput not accessable\n");
        return;
    }
    // configure touch device event properties
    memset(&uinp, 0, sizeof(uinp));
    strncpy(uinp.name, "Bowman Touch", UINPUT_MAX_NAME_SIZE);
    uinp.id.version = 4;
    uinp.id.bustype = BUS_USB;
    uinp.absmin[ABS_MT_SLOT] = 0;
    uinp.absmax[ABS_MT_SLOT] = 9; // track up to 9 fingers
    uinp.absmin[ABS_MT_TOUCH_MAJOR] = 0;
    uinp.absmax[ABS_MT_TOUCH_MAJOR] = 15;
    uinp.absmin[ABS_MT_POSITION_X] = 0; // screen dimension
    uinp.absmax[ABS_MT_POSITION_X] = 1079; // screen dimension
    uinp.absmin[ABS_MT_POSITION_Y] = 0; // screen dimension
    uinp.absmax[ABS_MT_POSITION_Y] = 1919; // screen dimension
    uinp.absmin[ABS_MT_TRACKING_ID] = 0;
    uinp.absmax[ABS_MT_TRACKING_ID] = 65535;
    uinp.absmin[ABS_MT_PRESSURE] = 0;
    uinp.absmax[ABS_MT_PRESSURE] = 255;

// Setup the uinput device
    ioctl(uinp_fd, UI_SET_EVBIT, EV_KEY);
    ioctl(uinp_fd, UI_SET_EVBIT, EV_REL);

// Touch
    ioctl(uinp_fd, UI_SET_EVBIT, EV_ABS);
    ioctl(uinp_fd, UI_SET_ABSBIT, ABS_MT_SLOT);
    ioctl(uinp_fd, UI_SET_ABSBIT, ABS_MT_TOUCH_MAJOR);
    ioctl(uinp_fd, UI_SET_ABSBIT, ABS_MT_POSITION_X);
    ioctl(uinp_fd, UI_SET_ABSBIT, ABS_MT_POSITION_Y);
    ioctl(uinp_fd, UI_SET_ABSBIT, ABS_MT_TRACKING_ID);
    ioctl(uinp_fd, UI_SET_ABSBIT, ABS_MT_PRESSURE);
    ioctl(uinp_fd, UI_SET_PROPBIT, INPUT_PROP_DIRECT);


/* Create input device into input sub-system */
    write(uinp_fd, &uinp, sizeof(uinp));
    printf("registered mouse\n");
}

void send_event(int type, int code, int value) {
    memset(&event, 0, sizeof(event));
    gettimeofday(&event.time, NULL);
    event.type = type;
    event.code = code;
    event.value = value;
    write(uinp_fd, &event, sizeof(event));
    usleep(500);
}

int VirtualMouse::setupMouse() {
    struct uinput_user_dev uinp;
    fd_mouse = open("/dev/uinput", O_WRONLY | O_NDELAY);
    if (fd_mouse <= 0) {
        return -1;
    }
    memset(&uinp, 0x00, sizeof(uinp));
    strncpy(uinp.name, "myMouse", strlen("myMouse"));
    uinp.id.version = 1;
    uinp.id.bustype = BUS_USB;

    ioctl(fd_mouse, UI_SET_EVBIT, EV_KEY);
    ioctl(fd_mouse, UI_SET_EVBIT, EV_REL);
    ioctl(fd_mouse, UI_SET_KEYBIT, BTN_MIDDLE);
    ioctl(fd_mouse, UI_SET_KEYBIT, BTN_LEFT);
    ioctl(fd_mouse, UI_SET_KEYBIT, BTN_RIGHT);
    ioctl(fd_mouse, UI_SET_RELBIT, REL_X);
    ioctl(fd_mouse, UI_SET_RELBIT, REL_Y);

    if (write(fd_mouse, &uinp, sizeof(uinp)) != sizeof(uinp)) {
        close(fd_mouse);
        fd_mouse = -1;
        return fd_mouse;
    }

    if (ioctl(fd_mouse, UI_DEV_CREATE)) {
        close(fd_mouse);
        fd_mouse = -1;
        return fd_mouse;
    }
    return fd_mouse;

}

int VirtualMouse::move(int x, int y) {
    if (fd_mouse < 0) {
        printf("invalid fd\n");
        return -1;

    }
    int ret = -1;
    static struct input_event ievent[4];
    static struct timespec now;
    float tv_nsec = 0;
    clock_gettime(CLOCK_MONOTONIC, &now);
    tv_nsec = now.tv_nsec / 1000;

    ievent[0].time.tv_sec = now.tv_sec;
    ievent[0].time.tv_usec = tv_nsec;
    ievent[0].type = EV_REL;
    ievent[0].code = REL_X;
    ievent[0].value = x;

    ievent[1].time.tv_sec = now.tv_sec;
    ievent[1].time.tv_usec = tv_nsec;
    ievent[1].type = EV_REL;
    ievent[1].code = REL_Y;
    ievent[1].value = y;

    ievent[2].time.tv_sec = now.tv_sec;
    ievent[2].time.tv_usec = tv_nsec;
    ievent[2].type = EV_SYN;
    ievent[2].code = SYN_REPORT;
    ievent[2].value = 0;

    ret = write(fd_mouse, &ievent[0], sizeof(ievent[0]));
    ret = write(fd_mouse, &ievent[1], sizeof(ievent[1]));
    ret = write(fd_mouse, &ievent[2], sizeof(ievent[2]));

    return ret;
}

int VirtualMouse::destroy() {
    printf("onDestory\n");
    if (fd_mouse > 0){
        ioctl(fd_mouse, UI_DEV_DESTROY);
        close(fd_mouse);
        fd_mouse = -1;
    }

}


void VirtualMouse::test() {
    static struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    srand(now.tv_nsec / 1000000);
    int ret = setupMouse();
    printf("mouse fd %i\n", ret);
    sleep(1);
    printf("begin mouse move\n");
    int rx = rand() % 2 == 0 ? 1 : -1;
    int ry = rand() % 2 == 0 ? 1 : -1;

    for (auto i = 0; i < 500; i++) {
        move(rx, ry);
        usleep(1500);
    }

    printf("\nvirtualMouse quit\n");
}

int VirtualMouse::click(int keycode,int status) {
    if (fd_mouse < 0) {
        return -1;
    }
    int ret = -1;
    struct input_event event;
    memset(&event, 0, sizeof(event));
    gettimeofday(&event.time, NULL);

    event.type = EV_KEY;
    event.code = keycode;
    event.value = status;
    write(fd_mouse, &event, sizeof(event));

    event.type = EV_SYN;
    event.code = SYN_REPORT;
    event.value = 0;
    ret = write(fd_mouse, &event, sizeof(event));
    printf("click key %i status %i\n", keycode, status);
    return ret;
}