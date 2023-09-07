#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/jiffies.h>
#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>
#include <linux/err.h>
#include <linux/mutex.h>
#include <linux/sysfs.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/dmi.h>
#include <linux/kobject.h>
#include <linux/cdev.h>
#include <linux/fs.h>

#include "i2c_dev_driver.h"

static struct ax_i2c_dev ax_i2c_ch_dev;  /* 实例ax_i2c_dev结构体 */

static int ax_i2c_read_regs(struct ax_i2c_dev *dev, u8 reg, void *val, int len)
{
    int ret;    
    struct i2c_msg msg[2];  /* 构建msg, 读取时一般使用一个至少两个元素的msg数组第一个元素用于发送目标数据地址(写), 第二个元素发送buffer地址(读) */
    struct i2c_client *client = (struct i2c_client *)dev->private_data; /* 从设备结构体变量中获取client数据 */
 
    msg[0].addr = client->addr;    //构造msg,设置设备地址
    msg[0].flags = 0;              //构造msg,标记为写, 先给eeprom发送读取数据的所在地址
    msg[0].buf = &reg;             //构造msg,读取数据的所在地址
    msg[0].len = 1;                //构造msg,地址数据长度, 只发送首地址的话长度就为1
	
    msg[1].addr = client->addr;    //构造msg,设置设备地址
    msg[1].flags = I2C_M_RD;       //构造msg,标记为读
    msg[1].buf = val;              //构造msg,数据读出的buffer地址
    msg[1].len = len;              //构造msg,读取数据长度
    
    ret = i2c_transfer(client->adapter, msg, 2);    /* 调用i2c_transfer发送msg */
    if(2 == ret)
    {
        ret = 0;
    }
    else
    {
        printk("i2c read failed %d\r\n", ret);
        ret = -EREMOTEIO;
    }
 
    return ret;
}

static s32 ax_i2c_write_regs(struct ax_i2c_dev *dev, u8 reg, u8 *buf, int len)
{
    int ret;                    /* 数据buffer */
    u8 b[MAX_BYTE_COUNT] = {0}; /* 构建msg */
    struct i2c_msg msg;         /* 从设备结构体变量中获取client数据 */
    struct i2c_client *client = (struct i2c_client *)dev->private_data;
 
    b[0] = reg;                 /* 把写入目标地址放在buffer的第一个元素中首先发送 */    
    memcpy(&b[1], buf, MAX_BYTE_COUNT > len ? len : MAX_BYTE_COUNT);    /* 把需要发送的数据拷贝到随后的地址中 */

    msg.addr = client->addr;    //构建msg, 设置设备地址
    msg.flags = 0;              //构建msg, 标记为写
    msg.buf = b;                //构建msg, 数据写入的buffer地址
    msg.len = len + 1;          //构建msg, 写入的数据长度, 因为除了用户数据外, 还需要发送数据地址所以要+1   
    
    ret = i2c_transfer(client->adapter, &msg, 1);   /* 调用i2c_transfer发送msg */
 
    if(1 == ret)
    {
        ret = 0;
    }
    else
    {
        printk(KERN_INFO "i2c write failed %d\r\n", ret);
        ret = -EREMOTEIO;
    }
    return ret;
}
 
static ssize_t ax_i2c_ch_dev_read(struct file *file, char __user *buf, size_t size, loff_t *offset)
{
    struct ax_i2c_dev *dev = (struct ax_i2c_dev *)file->private_data;   /* 获取私有数据 */
    char b[MAX_BYTE_COUNT] = {0};   /* 读取数据buffer */
    int ret = 0;
    
    printk(KERN_INFO "ax_i2c_ch_dev_read\r\n");
    ax_i2c_read_regs(dev, 0x00, b, MAX_BYTE_COUNT > size ? size : MAX_BYTE_COUNT);  /* 为了实验方便从0地址开始读, 实际应用不应在驱动中使用固定地址, 可以尝试使用iotcl去实现 */
    ret = copy_to_user(buf, b, MAX_BYTE_COUNT > size ? size : MAX_BYTE_COUNT);  /* 把读取到的数据拷贝到用户读取的地址 */
    return 0;
}
 
static ssize_t ax_i2c_ch_dev_write(struct file *file, const char __user *buf, size_t size, loff_t *offset)
{
    int ret = 0;
    struct ax_i2c_dev *dev = (struct ax_i2c_dev *)file->private_data;   /* 获取私有数据 */
    static char user_data[MAX_BYTE_COUNT] = {0};    /* 写入数据的buffer */

    printk(KERN_INFO "ax_i2c_ch_dev_write\r\n");
    ret = copy_from_user(user_data, buf, MAX_BYTE_COUNT > size ? size : MAX_BYTE_COUNT);    /* 获取用户需要发送的数据 */
    if(ret < 0)
    {
        printk(KERN_INFO "copy user data failed\r\n");
        return ret;
    } 
    
    ax_i2c_write_regs(dev, 0x00, user_data, size);  /* 和读对应的从0开始写 */
    
    return 0;
}

static int ax_i2c_ch_dev_open(struct inode *inde, struct file *filp) 
{
    printk(KERN_INFO "ax_i2c_ch_dev_open\r\n");
    filp->private_data = &ax_i2c_ch_dev;
    return 0;
}

static int ax_i2c_ch_dev_release(struct inode *inde, struct file *filp) 
{
    struct ax_i2c_dev *dev = (struct ax_i2c_dev *)filp->private_data;
    (void)dev;
    printk(KERN_INFO "ax_i2c_ch_dev_release\r\n");
    return 0;
}

static const struct file_operations ax_i2c_ch_dev_fops = {
    .owner      =   THIS_MODULE,
    .open       =   ax_i2c_ch_dev_open,
    .release    =   ax_i2c_ch_dev_release,
    .read       =   ax_i2c_ch_dev_read,
    .write      =   ax_i2c_ch_dev_write,
};

static int ax_i2c_ch_dev_probe(struct i2c_client *client, const struct i2c_device_id *id) 
{
    int ret = 0;

    printk(KERN_INFO "ax_i2c_ch_dev_probe successful!\r\n");
    ax_i2c_ch_dev.name = "ax_i2c_ch_dev";   
    ax_i2c_ch_dev.major = 0;   
    ax_i2c_ch_dev.count = 1;

    if(ax_i2c_ch_dev.major) 
    {
        ax_i2c_ch_dev.devid = MKDEV(ax_i2c_ch_dev.major, 0); 
        ret = register_chrdev_region(ax_i2c_ch_dev.devid, ax_i2c_ch_dev.count, ax_i2c_ch_dev.name);   
    } 
    else 
    {
        alloc_chrdev_region(&ax_i2c_ch_dev.devid, 0, ax_i2c_ch_dev.count, ax_i2c_ch_dev.name);   
        ax_i2c_ch_dev.major = MAJOR(ax_i2c_ch_dev.devid);   
        ax_i2c_ch_dev.minor = MINOR(ax_i2c_ch_dev.devid);   
    }    

    if (ret < 0) 
    {
        printk(KERN_INFO "ax_i2c_ch_dev chrdev region failed!\r\n");
        goto fail_devid;    
    }
    printk(KERN_INFO "ax_i2c_ch_dev major = %d, minor = %d \r\n", ax_i2c_ch_dev.major, ax_i2c_ch_dev.minor);  

    cdev_init(&ax_i2c_ch_dev.cdev, &ax_i2c_ch_dev_fops);
    ret = cdev_add(&ax_i2c_ch_dev.cdev, ax_i2c_ch_dev.devid, ax_i2c_ch_dev.count);
    if(ret < 0) 
    {
        goto fail_cdev;
    }

    ax_i2c_ch_dev.class = class_create(THIS_MODULE, ax_i2c_ch_dev.name); 
    if(IS_ERR(ax_i2c_ch_dev.class)) 
    {
        ret = PTR_ERR(ax_i2c_ch_dev.class);
        goto fail_class;
    }

    ax_i2c_ch_dev.device = device_create(ax_i2c_ch_dev.class, NULL, ax_i2c_ch_dev.devid, NULL, ax_i2c_ch_dev.name); 
    if(IS_ERR(ax_i2c_ch_dev.device)) 
    {
        ret = PTR_ERR(ax_i2c_ch_dev.device);
        goto fail_device;
    }
    ax_i2c_ch_dev.private_data = client;
    
    return 0;

fail_device:    
    class_destroy(ax_i2c_ch_dev.class); 
fail_class: 
    cdev_del(&ax_i2c_ch_dev.cdev);
fail_cdev:   
    unregister_chrdev_region(ax_i2c_ch_dev.devid, ax_i2c_ch_dev.count);  
fail_devid:  
    return ret;
}

static int ax_i2c_ch_dev_remove(struct i2c_client *client) 
{
    device_destroy(ax_i2c_ch_dev.class,ax_i2c_ch_dev.devid);    
    class_destroy(ax_i2c_ch_dev.class);  
    cdev_del(&ax_i2c_ch_dev.cdev);    
    unregister_chrdev_region(ax_i2c_ch_dev.devid, ax_i2c_ch_dev.count);  
    return 0;
}

static const struct i2c_device_id ax_i2c_ch_dev_id[] = {
    {"ax_i2c_ch_dev", 0},
    {}
};

static const struct of_device_id ax_i2c_ch_dev_of_match[] = {
    { .compatible = "ax_i2c_ch_dev" },
    {}
};

static struct i2c_driver ax_i2c_ch_dev_driver = {
    .probe = ax_i2c_ch_dev_probe,
    .remove = ax_i2c_ch_dev_remove,
    .driver = {
        .name = "ax_i2c_ch_dev",
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(ax_i2c_ch_dev_of_match),
    },
    .id_table = ax_i2c_ch_dev_id,
};

int __init i2c_dev_init(void)
{
    int ret = 0;

    ret = i2c_add_driver(&ax_i2c_ch_dev_driver);
    printk(KERN_INFO "%s", __FUNCTION__);
    return ret;
}

void __exit i2c_dev_exit(void)
{
    i2c_del_driver(&ax_i2c_ch_dev_driver);
    printk(KERN_INFO "%s", __FUNCTION__);
    return;
}

module_init(i2c_dev_init);
module_exit(i2c_dev_exit);

MODULE_LICENSE("GPL");