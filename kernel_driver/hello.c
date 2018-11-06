#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "hello"
#define CLASS_NAME "hello"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Daniel Crowe");
MODULE_DESCRIPTION("Flappy Bird");
MODULE_VERSION("0.1");

static char* message = "Hello Flappy\n";
module_param(message, charp, S_IRUGO);
MODULE_PARM_DESC(message, "the message to display");

static struct class* helloClass = NULL;
static struct device* helloDevice = NULL;
static int majorNumber = 0;

static int dev_open(struct inode *inodep, struct file *filep) {
	printk(KERN_INFO "Hello: Open\n");
	return 0;
}

static int dev_release(struct inode *inodep, struct file *filep) {
	printk(KERN_INFO "Hello: Release\n");
	return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
	int error = 0;
	ssize_t messageLen = strlen(message);
	ssize_t copyLen = len > messageLen ? messageLen : len;

	error = copy_to_user(buffer, message, copyLen);
	
	if (error == 0) {
		printk(KERN_INFO "Hello: Read %d chars\n", (int)copyLen);
		return copyLen;
	} else {
		printk(KERN_WARNING "Hello: Error reading: %d\n", error);
		return error;
	}
}

static struct file_operations fops = {
	.open = dev_open,
	.release = dev_release,
	.read = dev_read
};

static int dev_uevent(struct device *devicep, struct kobj_uevent_env *env) {
	printk(KERN_INFO "Hello: dev_uvent");
	add_uevent_var(env, "DEVMODE=%#o", S_IRUGO);
	return 0;
}

static int __init helloFlappy_init(void) {
	printk(KERN_INFO "Hello: Initing\n");
	
	majorNumber = register_chrdev(0, DEVICE_NAME, &fops);

	if (majorNumber < 0) {
		printk(KERN_ALERT "Hello: register_chrdev failed: %d\n", majorNumber);
		return majorNumber;
	}

	helloClass = class_create(THIS_MODULE, CLASS_NAME);
	helloClass->dev_uevent = dev_uevent;

	if (IS_ERR(helloClass)) {
		printk(KERN_ALERT "Hello: class_create failed: %ld\n", PTR_ERR(helloClass));
		unregister_chrdev(majorNumber, DEVICE_NAME);
		return PTR_ERR(helloClass);
	}

	helloDevice = device_create(helloClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);

	if (IS_ERR(helloDevice)) {
		printk(KERN_ALERT "Hello: device_create failed: %ld\n", PTR_ERR(helloDevice));
		class_destroy(helloClass);
		unregister_chrdev(majorNumber, DEVICE_NAME);
		return PTR_ERR(helloDevice);
	}

	return 0;
}

module_init(helloFlappy_init);

static void __exit helloFlappy_exit(void) {
	printk(KERN_INFO "Hello: Exiting\n");
	device_destroy(helloClass, MKDEV(majorNumber, 0));
	class_destroy(helloClass);
	unregister_chrdev(majorNumber, DEVICE_NAME);
}

module_exit(helloFlappy_exit);


