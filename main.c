#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

MODULE_LICENSE("GPL");

//LEDs
static unsigned int gpioBlueLED = 16;
static unsigned int gpioRedLED = 20;

//Buttons
static unsigned int gpioBlueButtonON = 13;
static unsigned int gpioBlueButtonOFF = 19;
static unsigned int gpioRedButtonON = 21;
static unsigned int gpioRedButtonOFF = 26;

//Interrupts
static unsigned int irqNumberBlueON;
static unsigned int irqNumberBlueOFF;
static unsigned int irqNumberRedON;
static unsigned int irqNumberRedOFF;  

//Counters
static unsigned int counterBlueButtonON = 0;
static unsigned int counterBlueButtonOFF = 0;
static unsigned int counterRedButtonON = 0;
static unsigned int counterRedButtonOFF = 0;

static irq_handler_t LED_BlueON(unsigned int irq, void *dev_id, struct pt_regs *regs);
static irq_handler_t LED_BlueOFF(unsigned int irq, void *dev_id, struct pt_regs *regs);
static irq_handler_t LED_RedON(unsigned int irq, void *dev_id, struct pt_regs *regs);
static irq_handler_t LED_RedOFF(unsigned int irq, void *dev_id, struct pt_regs *regs);

static irq_handler_t LED_BlueON(unsigned int irq, void *dev_id, struct pt_regs *regs){
    counterBlueButtonON++;
    gpio_set_value(gpioBlueLED, true);          
    return (irq_handler_t) IRQ_HANDLED;      
}

static irq_handler_t LED_BlueOFF(unsigned int irq, void *dev_id, struct pt_regs *regs){
    counterBlueButtonOFF++;
    gpio_set_value(gpioBlueLED, false);          
    return (irq_handler_t) IRQ_HANDLED;      
}

static irq_handler_t LED_RedON(unsigned int irq, void *dev_id, struct pt_regs *regs){
    counterRedButtonON++;
    gpio_set_value(gpioRedLED, true);          
    return (irq_handler_t) IRQ_HANDLED;      
}

static irq_handler_t LED_RedOFF(unsigned int irq, void *dev_id, struct pt_regs *regs){
    counterRedButtonOFF++;
    gpio_set_value(gpioRedLED, false);          
    return (irq_handler_t) IRQ_HANDLED;      
}

static int __init raspberry_init(void){
    int result = 0;

    //Init LEDs (Starts OFF)
    gpio_request(gpioBlueLED, "sysfs");
    gpio_direction_output(gpioBlueLED, false);
    gpio_export(gpioBlueLED, false);
    
    gpio_request(gpioBlueButtonON, "sysfs");
    gpio_direction_input(gpioBlueButtonON);
    gpio_set_debounce(gpioBlueButtonON, 200);
    gpio_export(gpioBlueButtonON, false);

    //Init Buttons
    gpio_request(gpioBlueButtonOFF, "sysfs");
    gpio_direction_input(gpioBlueButtonOFF);
    gpio_set_debounce(gpioBlueButtonOFF, 200);
    gpio_export(gpioBlueButtonOFF, false);

    gpio_request(gpioRedLED, "sysfs");
    gpio_direction_output(gpioRedLED, false);
    gpio_export(gpioRedLED, false);
    
    gpio_request(gpioRedButtonON, "sysfs");
    gpio_direction_input(gpioRedButtonON);
    gpio_set_debounce(gpioRedButtonON, 200);
    gpio_export(gpioRedButtonON, false);

    gpio_request(gpioRedButtonOFF, "sysfs");
    gpio_direction_input(gpioRedButtonOFF);
    gpio_set_debounce(gpioRedButtonOFF, 200);
    gpio_export(gpioRedButtonOFF, false);

    //Interrupts
    irqNumberBlueON = gpio_to_irq(gpioBlueButtonON);
    irqNumberBlueOFF = gpio_to_irq(gpioBlueButtonOFF);
    irqNumberRedON = gpio_to_irq(gpioRedButtonON);
    irqNumberRedOFF = gpio_to_irq(gpioRedButtonOFF);

    result = request_irq(irqNumberBlueON, (irq_handler_t) LED_BlueON, IRQF_TRIGGER_RISING, "LED_Blue_ON", NULL);               
    result = request_irq(irqNumberBlueOFF, (irq_handler_t) LED_BlueOFF, IRQF_TRIGGER_RISING, "LED_Blue_OFF", NULL);    
    result = request_irq(irqNumberRedON, (irq_handler_t) LED_RedON, IRQF_TRIGGER_RISING, "LED_Red_ON", NULL);               
    result = request_irq(irqNumberRedOFF, (irq_handler_t) LED_RedOFF, IRQF_TRIGGER_RISING, "LED_Red_OFF", NULL);        

    return result;
}

static void __exit raspberry_exit(void){

    //Show number of clicks for each button
    printk(KERN_INFO "ASO: The button A (Blue ON) was pressed %d times\n", counterBlueButtonON);
    printk(KERN_INFO "ASO: The button B (Blue OFF) was pressed %d times\n", counterBlueButtonOFF);
    printk(KERN_INFO "ASO: The button C (Red ON) was pressed %d times\n", counterRedButtonON);
    printk(KERN_INFO "ASO: The button D (Red OFF) was pressed %d times\n", counterRedButtonOFF);
    
    gpio_set_value(gpioBlueLED, 0);              
    gpio_unexport(gpioBlueLED);    

    gpio_set_value(gpioRedLED, 0);              
    gpio_unexport(gpioRedLED);   
    
    free_irq(irqNumberBlueON, NULL);   
    free_irq(irqNumberBlueOFF, NULL);      
    free_irq(irqNumberRedON, NULL);   
    free_irq(irqNumberRedOFF, NULL);     

	gpio_unexport(gpioBlueButtonON);
    gpio_unexport(gpioBlueButtonOFF);
    gpio_unexport(gpioRedButtonON);
    gpio_unexport(gpioRedButtonOFF);
    
    gpio_free(gpioBlueLED);
    gpio_free(gpioRedLED);
    gpio_free(gpioBlueButtonON); 
    gpio_free(gpioBlueButtonOFF); 
    gpio_free(gpioRedButtonON); 
    gpio_free(gpioRedButtonOFF); 
}

module_init(raspberry_init);
module_exit(raspberry_exit);