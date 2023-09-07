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

struct i2c_client *client_1, *client_2, *client_3, *client_4;

static struct i2c_board_info i2c1_devs __initdata = {
    I2C_BOARD_INFO("abcd1", 0x50)
}; 

static struct i2c_board_info i2c2_devs __initdata = {
    I2C_BOARD_INFO("abcd2", 0x1b)
}; 

static struct i2c_board_info i2c3_devs __initdata = {
    I2C_BOARD_INFO("abcd3", 0x34)
}; 

static struct i2c_board_info i2c4_devs __initdata = {
    I2C_BOARD_INFO("abcd4", 0x1a)
}; 

int __init i2c_drv_init(void)
{
    int ret = 0;
    struct i2c_adapter* adap;

    printk(KERN_INFO "%s", __FUNCTION__);

    adap = i2c_get_adapter(0);
    client_1 = i2c_new_client_device(adap, &i2c1_devs);
    client_2 = i2c_new_client_device(adap, &i2c2_devs);
    client_3 = i2c_new_client_device(adap, &i2c3_devs);
    client_4 = i2c_new_client_device(adap, &i2c4_devs);

    i2c_put_adapter(adap);
    return ret;
}

void __exit i2c_drv_exit(void)
{
    printk(KERN_INFO "%s", __FUNCTION__);
    return;
}

module_init(i2c_drv_init);
module_exit(i2c_drv_exit);

MODULE_LICENSE("GPL");