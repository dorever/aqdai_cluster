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

int __init i2c_dev_init(void)
{
    int ret = 0;

    printk(KERN_INFO "%s", __FUNCTION__);
    return ret;
}

void __exit i2c_dev_exit(void)
{
    printk(KERN_INFO "%s", __FUNCTION__);
    return;
}

module_init(i2c_dev_init);
module_exit(i2c_dev_exit);

MODULE_LICENSE("GPL");