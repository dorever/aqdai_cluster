#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/jiffies.h>
#include <linux/i2c.h>
#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>
#include <linux/err.h>
#include <linux/mutex.h>
#include <linux/sysfs.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/dmi.h>
#include <linux/kobject.h>

#include "i2c_device.h"


/* 设备结构体 */
struct abcd_char_dev_dev {
    dev_t devid;            /* 设备号 */
    int major;              /* 主设备号 */
    int minor;              /* 主设备号 */
    int count;              /* 设备个数 */
    char* name;             /* 设备名字 */
    struct cdev cdev;       /* 注册设备结构体 */
    struct class *class;    /* 类 */
    struct device *device;  /* 设备 */
};
static struct abcd_char_dev_dev abcd_char_dev;  /* 实例abcd_char_dev_dev结构体 */

static int abcd_char_dev_open(struct inode *inde, struct file *filp) 
{
    filp->private_data = &abcd_char_dev;
    printk(KERN_INFO "abcd_char_dev_open\r\n");
    return 0;
}

static int abcd_char_dev_release(struct inode *inde, struct file *filp) 
{
    struct abcd_char_dev_dev *dev = (struct abcd_char_dev_dev *)filp->private_data;
    printk(KERN_INFO "abcd_char_dev_release\r\n");
    return 0;
}

static ssize_t abcd_char_dev_read(struct file *filp, char __user *buf, size_t count, loff_t *ppos) 
{
    printk(KERN_INFO "abcd_char_dev_read\r\n");
    return 0;
}

static ssize_t abcd_char_dev_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos) 
{
    struct abcd_char_dev_dev *dev = (struct abcd_char_dev_dev *)filp->private_data;
    printk(KERN_INFO "abcd_char_dev_write\r\n");
    return 0;
}

static const struct file_operations abcd_char_dev_fops = {
    .owner      =   THIS_MODULE,
    .open       =   abcd_char_dev_open,
    .release    =   abcd_char_dev_release,
    .read       =   abcd_char_dev_read,
    .write      =   abcd_char_dev_write,
};


static int abcd_char_dev_probe(struct i2c_client *client, const struct i2c_device_id *id) 
{
    int ret = 0;

    printk(KERN_INFO "abcd_char_dev_probe successful!\r\n");
    abcd_char_dev.name = "abcd_char_dev";   
    abcd_char_dev.major = 0;   
    abcd_char_dev.count = 1;

    if(abcd_char_dev.major) 
    {
        abcd_char_dev.devid = MKDEV(abcd_char_dev.major, 0); 
        ret = register_chrdev_region(abcd_char_dev.devid, abcd_char_dev.count, abcd_char_dev.name);   
    } 
    else 
    {
        alloc_chrdev_region(&abcd_char_dev.devid, 0, abcd_char_dev.count, abcd_char_dev.name);   
        abcd_char_dev.major = MAJOR(abcd_char_dev.devid);   
        abcd_char_dev.minor = MINOR(abcd_char_dev.devid);   
    }    

    if (ret < 0) 
    {
        printk(KERN_INFO "abcd_char_dev chrdev region failed!\r\n");
        goto fail_devid;    
    }
    printk(KERN_INFO "abcd_char_dev major = %d, minor = %d \r\n", abcd_char_dev.major, abcd_char_dev.minor);  

    cdev_init(&abcd_char_dev.cdev, &abcd_char_dev_fops);
    
    ret = cdev_add(&abcd_char_dev.cdev, abcd_char_dev.devid, abcd_char_dev.count);
    if(ret < 0) 
    {
        goto fail_cdev;
    }

    abcd_char_dev.class = class_create(THIS_MODULE, abcd_char_dev.name); 
    if(IS_ERR(abcd_char_dev.class)) 
    {
        ret = PTR_ERR(abcd_char_dev.class);
        goto fail_class;
    }

    abcd_char_dev.device = device_create(abcd_char_dev.class, NULL, abcd_char_dev.devid, NULL, abcd_char_dev.name); 
    if(IS_ERR(abcd_char_dev.device)) 
    {
        ret = PTR_ERR(abcd_char_dev.device);
        goto fail_device;
    }

    return 0;

fail_device:    
    class_destroy(abcd_char_dev.class); 
fail_class: 
    cdev_del(&abcd_char_dev.cdev);
fail_cdev:   
    unregister_chrdev_region(abcd_char_dev.devid, abcd_char_dev.count);  
fail_devid:  
    return ret;
}

static int abcd_char_dev_remove(struct i2c_client *client) 
{
    device_destroy(abcd_char_dev.class,abcd_char_dev.devid);    
    class_destroy(abcd_char_dev.class);  
    cdev_del(&abcd_char_dev.cdev);    
    unregister_chrdev_region(abcd_char_dev.devid, abcd_char_dev.count);  
    return 0;
}

static const struct i2c_device_id abcd_char_dev_id[] = {
    {"alientek,abcd_char_dev", 0},
    {}
};

static const struct of_device_id abcd_char_dev_of_match[] = {
    { .compatible = "alientek, abcd_char_dev" },
    {}
};

static struct i2c_driver abcd_char_dev_driver = {
    .probe = abcd_char_dev_probe,
    .remove = abcd_char_dev_remove,
    .driver = {
        .name = "abcd_char_dev",
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(abcd_char_dev_of_match),
    },
    .id_table = abcd_char_dev_id,
};

int __init i2c_dev_init(void)
{
    int ret = 0;

    ret = i2c_add_driver(&abcd_char_dev_driver);
    printk(KERN_INFO "%s", __FUNCTION__);
    return ret;
}

void __exit i2c_dev_exit(void)
{
    i2c_del_driver(&abcd_char_dev_driver);
    printk(KERN_INFO "%s", __FUNCTION__);
    return;
}

module_init(i2c_dev_init);
module_exit(i2c_dev_exit);

MODULE_LICENSE("GPL");