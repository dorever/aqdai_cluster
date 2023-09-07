#include <linux/init.h>
#include <linux/i2c.h>

/* 设备结构体 */
struct ax_i2c_dev {
    dev_t devid;            /* 设备号 */
    int major;              /* 主设备号 */
    int minor;              /* 主设备号 */
    int count;              /* 设备个数 */
    char* name;             /* 设备名字 */
    struct cdev cdev;       /* 注册设备结构体 */
    struct class *class;    /* 类 */
    struct device *device;  /* 设备 */
    void *private_data;     /* 用于在probe函数中获取client */
};

#define MAX_BYTE_COUNT 128