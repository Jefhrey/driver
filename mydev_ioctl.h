#ifndef MYDEV_IOCTL_H
#define MYDEV_IOCTL_H

#include <linux/ioctl.h>
#include <linux/types.h>

#define MYDEV_IOC_MAGIC     'M'

#define MYDEV_IOC_SET_VALUE \
    _IOW(MYDEV_IOC_MAGIC, 1, __u32)

#define MYDEV_IOC_GET_VALUE \
    _IOR(MYDEV_IOC_MAGIC, 2, __u32)

#endif
