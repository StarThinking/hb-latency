#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/errno.h>
#include <linux/ktime.h>
#include <linux/delay.h>

#include <linux/kprobes.h>

static struct kprobe kp;
static int count;

static struct kprobe kp = {
    .symbol_name    = "dev_queue_xmit",
};

static int handler_pre(struct kprobe *p, struct pt_regs *regs) {
    printk(KERN_INFO "handler_pre: delay_dev_queue_xmit\n");
    
    if (count++ % 20 == 0) {
            printk("delay_dev_queue_xmit: let's add delay of 2500ms.\n");
            mdelay(2500);
    }

    return 0;
}

static void handler_post(struct kprobe *p, struct pt_regs *regs,
                                          unsigned long flags) {
    printk(KERN_INFO "handler_post: delay_dev_queue_xmit\n");
}

static int __init delay_dev_queue_xmit_init(void) {
    int ret;

    printk(KERN_INFO "delay_dev_queue_xmit_init\n");
    
//    kp.addr = (kprobe_opcode_t *) foobar;
    kp.pre_handler = handler_pre;
    kp.post_handler = handler_post;
    ret = register_kprobe(&kp);

    if (ret < 0) {
        printk(KERN_INFO "register_kprobe failed, returned %d\n", ret);
        return ret;
    }
    
    printk(KERN_INFO "Planted kprobe at %p\n", kp.addr);
    
    return 0;
}
 
static void __exit delay_dev_queue_xmit_exit(void) {
    unregister_kprobe(&kp);
    printk(KERN_INFO "delay_dev_queue_xmit_exit\n");
}
 
module_init(delay_dev_queue_xmit_init);
module_exit(delay_dev_queue_xmit_exit);
 
MODULE_LICENSE("GPL");
