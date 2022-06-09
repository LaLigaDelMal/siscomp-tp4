#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/math64.h>
#include <linux/module.h>
#include <linux/timekeeping.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <asm/errno.h>

#define TIMEOUT 1000  // Milliseconds
#define BUFFER_SIZE 256

//// Define GPIOs for pins
static struct gpio pins[] = {
    {17, GPIOF_OUT_INIT_LOW, "TRIGGER"},  // Trigger for ultrasonic sensor
    {18, GPIOF_IN, "ECHO_RISING"},        // Echo for ultrasonic sensor
    {27, GPIOF_IN, "LIGHT"},              // Pin for photovoltaic sensor
    {22, GPIOF_IN, "ECHO_FALLING"}
};

static char input_buffer[BUFFER_SIZE];
static uint8_t output_value;
int selected_sensor = 0;  // 0 -> Light sensor, 1 -> Proximity sensor
static struct timer_list timer;
static int echo_rising_irq;
static int echo_falling_irq;
struct cdev char_dev;  // Estructura que almacena informacion del device (ej.:
                       // file_operations, owner)
static struct class* dev_class;
static dev_t dev_id;

// Declaracion de funciones
void timer_callback(struct timer_list*);
static int chardev_open(struct inode* i, struct file* f);
static int chardev_close(struct inode* i, struct file* f);
static ssize_t chardev_read(struct file* f, char __user* buf, size_t len, loff_t* offset);
static ssize_t chardev_write(struct file* f, const char __user* buf, size_t len, loff_t* off);
int get_light_reading(void);
void get_proximity_reading(void);
static irqreturn_t echo_rising_isr(int irq, void *data);
static irqreturn_t echo_falling_isr(int irq, void *data);

static struct file_operations chardev_fops = {.owner = THIS_MODULE,
                                              .open = chardev_open,
                                              .release = chardev_close,
                                              .read = chardev_read,
                                              .write = chardev_write};

void timer_callback(struct timer_list* d) {
    // TODO: read sensors and process raw data

    if (selected_sensor) {
        get_proximity_reading();
        printk(KERN_INFO "PROXIMITY READING %d.\n", output_value);

    } else {
        output_value = (uint8_t)get_light_reading();
        printk(KERN_INFO "LIGHT READING %d.\n", output_value);
    }

    // Set Timer again
    mod_timer(&timer, jiffies + msecs_to_jiffies(TIMEOUT));
}

static int __init gpio_prxl_init(void) {
    int r;

    printk(KERN_INFO "Initializing module gpio_prxl.\n");

    if ((r = alloc_chrdev_region(&dev_id, 0, 1, "gpio_prxl")) < 0) {
        return r;
    }

    if (IS_ERR(dev_class = class_create(THIS_MODULE, "gpio_prxl"))) {
        unregister_chrdev_region(dev_id, 1);

        return PTR_ERR(dev_class);
    }

    if (IS_ERR(r = device_create(dev_class, NULL, dev_id, NULL, "gpio_prxl"))) {
        class_destroy(dev_class);
        unregister_chrdev_region(dev_id, 1);

        return PTR_ERR(r);
    }

    cdev_init(&char_dev, &chardev_fops);

    if ((r = cdev_add(&char_dev, dev_id, 1)) < 0) {
        device_destroy(dev_class, dev_id);
        class_destroy(dev_class);
        unregister_chrdev_region(dev_id, 1);

        return r;
    }

    //// Register GPIOs
    if (r = gpio_request_array(pins, ARRAY_SIZE(pins))) {
        printk(KERN_ERR "Failed to allocate GPIOs\n");

        return r;
    }

    // Configure Echo IRQ
    echo_rising_irq = gpio_to_irq(pins[1].gpio);
    echo_falling_irq = gpio_to_irq(pins[3].gpio);

    request_irq(echo_rising_irq, echo_rising_isr, IRQF_TRIGGER_RISING /* | IRQF_DISABLED */, "echo-rising", NULL);
    request_irq(echo_falling_irq, echo_falling_isr, IRQF_TRIGGER_FALLING /* | IRQF_DISABLED */, "echo-falling", NULL);


    // Set Timer
    timer_setup(&timer, timer_callback, 0);
    mod_timer(&timer, jiffies + msecs_to_jiffies(TIMEOUT));

    printk(KERN_INFO "Successfully initialized module gpio_prxl.\n");
    return 0;
}

static void __exit gpio_prxl_exit(void) {

    free_irq(echo_rising_irq, NULL);
    free_irq(echo_falling_irq, NULL);
    del_timer(&timer);
    gpio_free_array(pins, ARRAY_SIZE(pins));
    cdev_del(&char_dev);
    device_destroy(dev_class, dev_id);
    class_destroy(dev_class);
    unregister_chrdev_region(dev_id, 1);

    printk(KERN_INFO "Module gpio_prxl unloaded.\n");
}

static int chardev_open(struct inode* i, struct file* f) {
    printk(KERN_INFO "gpio_prxl: open() called.\n");
    return 0;
}

static int chardev_close(struct inode* i, struct file* f) {
    printk(KERN_INFO "gpio_prxl: close() called.\n");
    return 0;
}

static ssize_t chardev_read(struct file* f,
                            char __user* buf,
                            size_t len,
                            loff_t* offset) {
    printk(KERN_INFO "gpio_prxl: read() called.\n");

    if (*offset == 0) {
        copy_to_user(buf, &output_value, 1);
        (*offset)++;

        return 1;
    } else {
        return 0;
    }
}

static ssize_t chardev_write(struct file* f,
                             const char __user* buf,
                             size_t len,
                             loff_t* off) {
    printk(KERN_INFO "gpio_prxl: write() called.\n");

    copy_from_user(input_buffer, buf, len);

    if (!strncmp(input_buffer, "1", 1)) {
        printk(KERN_INFO "gpio_prxl: Light sensor selected. \n");
        selected_sensor = 0;
    } else {
        printk(KERN_INFO "gpio_prxl: Proximity sensor selected. \n");
        selected_sensor = 1;
    }

    return len;
}

int get_light_reading(void) {
    int light_value = gpio_get_value(pins[2].gpio);
    return light_value;
}

static time64_t time_aux1 = 0, time_aux2 = 0, echo_time;
void get_proximity_reading(void) {
    // Send trigger pulse
    gpio_set_value(pins[0].gpio, 1);
    udelay(10);
    gpio_set_value(pins[0].gpio, 0);
}

static irqreturn_t echo_rising_isr(int irq, void *data)
{
	if(irq == echo_rising_irq){

        time_aux1 = ktime_get_ns();	
    }

	return IRQ_HANDLED;
}

static irqreturn_t echo_falling_isr(int irq, void *data)
{
	if(irq == echo_falling_irq){
        
        time_aux2 = ktime_get_ns();
        echo_time = time_aux2 - time_aux1;
        output_value = (uint8_t) (echo_time >> 12);
    }

	return IRQ_HANDLED;
}

module_init(gpio_prxl_init);
module_exit(gpio_prxl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adriel-Gaspar");
MODULE_DESCRIPTION("Proximity and light sensing for rpi");