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

#define KOBJ_ATTR(_name, _mode, _show, _store) \
    struct kobj_attribute kobj_attr_##_name = __ATTR(_name, _mode, _show, _store)

static struct kobject *my_kobj;

static ssize_t vpd_flip_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    printk(KERN_INFO "%s\r\n", __FUNCTION__);
    return sprintf(buf, "%dn", 855);
}

static ssize_t vpd_flip_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    dev_info(dev, "%s: %d enter string:%sn", __func__, __LINE__,buf);
    return count;
}

static ssize_t vpd_ss_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    printk(KERN_INFO "%s\r\n", __FUNCTION__);
    return sprintf(buf, "%dn", 350);
}

static ssize_t vpd_ss_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    printk(KERN_INFO "%s\r\n", __FUNCTION__);
    dev_info(dev, "%s: %d enter string:%sn", __func__, __LINE__,buf);
    return count;
}

static ssize_t vpd_mode_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    printk(KERN_INFO "%s\r\n", __FUNCTION__);
    dev_info(dev, "%s: %d enter string:%sn", __func__, __LINE__,buf);
    return sprintf(buf, "%dn", 120);
}

static ssize_t vpd_mode_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    printk(KERN_INFO "%s\r\n", __FUNCTION__);
    dev_info(dev, "%s: %d enter string:%sn", __func__, __LINE__,buf);
    return count;
}

static ssize_t vpd_enable_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    printk(KERN_INFO "%s\r\n", __FUNCTION__);
    return sprintf(buf, "%dn", 520);
}

static ssize_t vpd_enable_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    printk(KERN_INFO "%s\r\n", __FUNCTION__);
    dev_info(dev, "%s: %d enter string:%sn", __func__, __LINE__,buf);
    return count;
}

static DEVICE_ATTR(flip,   S_IWUSR | S_IRUSR, vpd_flip_show, vpd_flip_store);
static DEVICE_ATTR(ss,     S_IWUSR | S_IRUSR, vpd_ss_show, vpd_ss_store);
static DEVICE_ATTR(mode,   S_IWUSR | S_IRUSR, vpd_mode_show, vpd_mode_store);
static DEVICE_ATTR(enable, S_IWUSR | S_IRUSR, vpd_enable_show, vpd_enable_store);

static struct attribute *vpd_attributes[] = {
    &dev_attr_flip.attr,
    &dev_attr_ss.attr,
    &dev_attr_mode.attr,
    &dev_attr_enable.attr,
    NULL////属性结构体数组最后一项必须以NULL结尾。
};

static const struct attribute_group vpd_attr_group = {
    .attrs = vpd_attributes,
};

static ssize_t kobj_show_function(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    int ret = 0;

    printk(KERN_INFO "%s\r\n", __FUNCTION__);
    sprintf(buf, "%s", __FUNCTION__);
    return ret;
}

static ssize_t kobj_store_function(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    printk(KERN_INFO "%s\r\n", __FUNCTION__);
    return count;
}

static KOBJ_ATTR(name, S_IWUSR | S_IRUSR, kobj_show_function, kobj_store_function);
static KOBJ_ATTR(type, S_IWUSR | S_IRUSR, kobj_show_function, kobj_store_function);
static KOBJ_ATTR(addr, S_IWUSR | S_IRUSR, kobj_show_function, kobj_store_function);
static KOBJ_ATTR(offset, S_IWUSR | S_IRUSR, kobj_show_function, kobj_store_function);

static struct attribute *kobj_attrs[] = {
    &kobj_attr_name.attr,
    &kobj_attr_type.attr,
    &kobj_attr_addr.attr,
    &kobj_attr_offset.attr,
    NULL,
};

static struct attribute_group kobj_attr_group = {
    .attrs = kobj_attrs,
};

int __init i2c_drv_init(void)
{
    int ret = 0;

    printk(KERN_INFO "%s", __FUNCTION__);

    my_kobj = kobject_create_and_add("kobject_test", NULL); // 创建kobject对象
    if(!my_kobj) 
    {
        printk(KERN_ERR "Failed to create object.\n");
        return -ENOMEM;
    }
 
    ret |= sysfs_create_group(my_kobj, &vpd_attr_group);  // 创建属性文件组
    ret |= sysfs_create_group(my_kobj, &kobj_attr_group); // 创建属性文件组
    if(ret) 
    {
        printk(KERN_ERR "Failed to create attribute group.\n");
        kobject_put(my_kobj);
        return ret;
    }

    return ret;
}

void __exit i2c_drv_exit(void)
{
    printk(KERN_INFO "%s", __FUNCTION__);
    
    sysfs_remove_group(my_kobj, &kobj_attr_group); // 移除属性文件组
    sysfs_remove_group(my_kobj, &vpd_attr_group); // 移除属性文件组
    kobject_put(my_kobj); // 销毁kobject
    return;
}

module_init(i2c_drv_init);
module_exit(i2c_drv_exit);

MODULE_LICENSE("GPL");