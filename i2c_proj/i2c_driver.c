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

#include "i2c_driver.h"

static int abcd_probe(struct i2c_client *client, const struct i2c_device_id *id) {
    
    printk(KERN_INFO "abcd_probe successful!\r\n");
    return 0;
}

static int abcd_remove(struct i2c_client *client) {

    printk(KERN_INFO "abcd_remove successful!\r\n");
    return 0;
}

static const struct i2c_device_id abcd_id[] = {
    {"alientek, abcd",0},
    {}
};

static const struct of_device_id abcd_of_match[] = {
    { .compatible = "alientek, abcd" },
    {}
};

static struct i2c_driver abcd_driver = {
    .probe = abcd_probe,
    .remove = abcd_remove,
    .driver = {
        .name = "abcd",
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(abcd_of_match),
    },
    .id_table = abcd_id,
};
 
int __init i2c_drv_init(void)
{
    int ret = 0;

    printk(KERN_INFO "%s", __FUNCTION__);
    ret = i2c_add_driver(&abcd_driver);
    return ret;
}

void __exit i2c_drv_exit(void)
{
    printk(KERN_INFO "%s", __FUNCTION__);
    i2c_del_driver(&abcd_driver);
    return;
}

module_init(i2c_drv_init);
module_exit(i2c_drv_exit);

MODULE_LICENSE("GPL");