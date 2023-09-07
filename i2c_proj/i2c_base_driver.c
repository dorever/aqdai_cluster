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
#include <linux/init.h>
#include <linux/i2c.h>

static int abcd0_probe(struct i2c_client *client, const struct i2c_device_id *id) {
    
    printk(KERN_INFO "abcd0_probe successful!\r\n");
    return 0;
}

static int abcd0_remove(struct i2c_client *client) {

    printk(KERN_INFO "abcd0_remove successful!\r\n");
    return 0;
}

static const struct i2c_device_id abcd0_id[] = {
    {"alientek, abcd0", 0},
    {}
};

static const struct of_device_id abcd0_of_match[] = {
    { .compatible = "alientek, abcd0" },
    {}
};

static struct i2c_driver abcd0_driver = {
    .probe = abcd0_probe,
    .remove = abcd0_remove,
    .driver = {
        .name = "abcd0",
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(abcd0_of_match),
    },
    .id_table = abcd0_id,
};
 
int __init i2c_drv_init(void)
{
    int ret = 0;

    printk(KERN_INFO "%s", __FUNCTION__);
    ret = i2c_add_driver(&abcd0_driver);
    return ret;
}

void __exit i2c_drv_exit(void)
{
    printk(KERN_INFO "%s", __FUNCTION__);
    i2c_del_driver(&abcd0_driver);
    return;
}

module_init(i2c_drv_init);
module_exit(i2c_drv_exit);

MODULE_LICENSE("GPL");