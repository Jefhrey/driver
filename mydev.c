#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/mutex.h>
#include <linux/uaccess.h>

#include "mydev_ioctl.h"

#define DEVICE_NAME "mydev"

struct mydev_device {
    struct cdev cdev;
    struct mutex lock;
    __u32 value;
};

static struct mydev_device mydev;
static dev_t mydev_devno;

/* Called when userspace opens /dev/mydev. */
static int mydev_open(struct inode *inode, struct file *file)
{
    struct mydev_device *dev;

    dev = container_of(inode->i_cdev,
                       struct mydev_device, cdev);

    file->private_data = dev;

    pr_info("mydev: device opened\n");
    return 0;
}

/* Called when the open file description is released. */
static int mydev_release(struct inode *inode, struct file *file)
{
    (void)inode;
    (void)file;

    pr_info("mydev: device released\n");
    return 0;
}

/* Handle commands issued through ioctl(). */
static long mydev_ioctl(struct file *file,
                        unsigned int cmd,
                        unsigned long arg)
{
    struct mydev_device *dev = file->private_data;
    __u32 value;

    switch (cmd) {
    case MYDEV_IOC_SET_VALUE:
        /* Copy the supplied value from userspace. */
        if (copy_from_user(&value,
                           (void __user *)arg,
                           sizeof(value)))
            return -EFAULT;

        mutex_lock(&dev->lock);
        dev->value = value;
        mutex_unlock(&dev->lock);

        pr_info("mydev: value updated\n");
        return 0;

    case MYDEV_IOC_GET_VALUE:
        /* Read the current value under the lock. */
        mutex_lock(&dev->lock);
        value = dev->value;
        mutex_unlock(&dev->lock);

        /* Return the value to userspace. */
        if (copy_to_user((void __user *)arg,
                         &value,
                         sizeof(value)))
            return -EFAULT;

        return 0;

    default:
        return -ENOTTY;
    }
}

static const struct file_operations mydev_fops = {
    .owner          = THIS_MODULE,
    .open           = mydev_open,
    .release        = mydev_release,
    .unlocked_ioctl = mydev_ioctl,
};

static int __init mydev_init(void)
{
    int ret;

    mutex_init(&mydev.lock);
    mydev.value = 0;

    /* Dynamically allocate a device number. */
    ret = alloc_chrdev_region(&mydev_devno, 0, 1,
                              DEVICE_NAME);
    if (ret < 0)
        return ret;

    /* Associate the character device with its operations. */
    cdev_init(&mydev.cdev, &mydev_fops);
    mydev.cdev.owner = THIS_MODULE;

    ret = cdev_add(&mydev.cdev, mydev_devno, 1);
    if (ret < 0) {
        unregister_chrdev_region(mydev_devno, 1);
        return ret;
    }

    pr_info("mydev: registered major=%u minor=%u\n",
            MAJOR(mydev_devno), MINOR(mydev_devno));

    return 0;
}

static void __exit mydev_exit(void)
{
    cdev_del(&mydev.cdev);
    unregister_chrdev_region(mydev_devno, 1);

    pr_info("mydev: unregistered\n");
}

module_init(mydev_init);
module_exit(mydev_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Example character device with ioctl");
MODULE_AUTHOR("Driver Tutorial");
