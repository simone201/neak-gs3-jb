/* drivers/misc/custom_voltage.c
 *
 * Copyright 2011 Ezekeel
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/device.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>

#define CUSTOMVOLTAGE_VERSION 1

extern void customvoltage_updateintvolt(unsigned long * int_voltages);
extern int customvoltage_numfreqs(void);
extern void customvoltage_freqvolt(unsigned long * freqs, unsigned long * int_voltages);

static int num_freqs;

static int * int_voltages = NULL;
static int * freqs = NULL;

static ssize_t customvoltage_intvolt_read(struct device * dev, struct device_attribute * attr, char * buf) {
    
    int i, j = 0;

    for (i = 0; i < num_freqs; i++) {
		j += sprintf(buf + j, "%dmhz: %d mV\n", freqs[i] / 1000, int_voltages[i] / 1000);
	}

    return j;
}

static ssize_t customvoltage_intvolt_write(struct device * dev, struct device_attribute * attr, const char * buf, size_t size) {
    unsigned int ret = -EINVAL;
    int i = 0;

    ret = sscanf(buf, "%d %d %d %d %d %d %d", &int_voltages[0], &int_voltages[1], &int_voltages[2], &int_voltages[3], &int_voltages[4], &int_voltages[5], &int_voltages[6]);
    
    if(ret != 7)
		return -EINVAL;
	else
		customvoltage_updateintvolt(int_voltages);

    return size;
}

static ssize_t customvoltage_version(struct device * dev, struct device_attribute * attr, char * buf) {
    return sprintf(buf, "%u\n", CUSTOMVOLTAGE_VERSION);
}

void customvoltage_cpu_init(void) {

	num_freqs = customvoltage_numfreqs();

    int_voltages = kzalloc(num_freqs * sizeof(unsigned long), GFP_KERNEL);
    freqs = kzalloc(num_freqs * sizeof(unsigned long), GFP_KERNEL);

    customvoltage_freqvolt(freqs, int_voltages);
}

static DEVICE_ATTR(int_volt, S_IRUGO | S_IWUGO, customvoltage_intvolt_read, customvoltage_intvolt_write);
static DEVICE_ATTR(version, S_IRUGO , customvoltage_version, NULL);

static struct attribute *customvoltage_attributes[] = {
	&dev_attr_int_volt.attr,
	&dev_attr_version.attr,
	NULL
};

static struct attribute_group customvoltage_group = {
	.attrs = customvoltage_attributes,
};

static struct miscdevice customvoltage_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "customvoltage",
};

static int __init customvoltage_init(void) {
    int ret;

    pr_info("%s misc_register(%s)\n", __FUNCTION__, customvoltage_device.name);

    ret = misc_register(&customvoltage_device);

    if (ret) {
		pr_err("%s misc_register(%s) fail\n", __FUNCTION__, customvoltage_device.name);

		return 1;
	}

    if (sysfs_create_group(&customvoltage_device.this_device->kobj, &customvoltage_group) < 0) {
		pr_err("%s sysfs_create_group fail\n", __FUNCTION__);
		pr_err("Failed to create sysfs group for device (%s)!\n", customvoltage_device.name);
	}

    return 0;
}

device_initcall(customvoltage_init);
