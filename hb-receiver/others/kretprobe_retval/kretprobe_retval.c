/*
 * kretprobe_example.c
 *
 * Here's a sample kernel module showing the use of return probes to
 * report the return value and total time taken for probed function
 * to run.
 *
 * usage: insmod kretprobe_example.ko func=<func_name>
 *
 * If no func_name is specified, do_fork is instrumented
 *
 * For more information on theory of operation of kretprobes, see
 * Documentation/kprobes.txt
 *
 * Build and insert the kernel module as done in the kprobe example.
 * You will see the trace data in /var/log/messages and on the console
 * whenever the probed function returns. (Some messages may be suppressed
 * if syslogd is configured to eliminate duplicate messages.)
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/ktime.h>
#include <linux/limits.h>
#include <linux/sched.h>

#include <linux/skbuff.h>
#include <net/ip.h>

//static char func_name[NAME_MAX] = "foobar2";
static char func_name[NAME_MAX] = "napi_consume_skb";
module_param_string(func, func_name, NAME_MAX, S_IRUGO);
MODULE_PARM_DESC(func, "Function to kretprobe; this module will report the"
			" function's execution time");
static int entry_handler(struct kretprobe_instance *ri, struct pt_regs *regs) {
//	printk("entry_handler\n");
	struct sk_buff *skb = NULL;
	struct iphdr *iph = NULL;
        struct udphdr *uh = NULL;
        u16 dport;
	
	if (regs == NULL)
        	return 0;

        if ((skb = (struct sk_buff *) regs->di) == NULL)
                return 0;

        if ((iph = ip_hdr(skb)) == NULL)
                return 0;

        if (iph->protocol != IPPROTO_UDP)
                return 0;
	
	uh = (struct udphdr *) skb_transport_header(skb);
	dport = ntohs(uh->dest);
	
	//if (dport == 5001) {
	//}
	
	return 0;
}

/*static int ret_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{
	int retval = regs_return_value(regs);
//	printk(KERN_INFO "%s returned %d\n", func_name, retval);

//        regs->ax = 19;
	return 0;
}*/

static struct kretprobe my_kretprobe = {
	.entry_handler          = entry_handler,
	//.handler		= ret_handler,
	/* Probe up to 20 instances concurrently. */
	.maxactive		= 20,
};

//extern int foobar2(int, int, int);
DEFINE_PER_CPU(long, x);
static int __init kretprobe_init(void)
{
	int ret;
        int ret_foobar;
		
	long z;
	this_cpu_write(x, 2);
	z = this_cpu_read(x);
	printk("z = %ld\n", z);
	my_kretprobe.kp.symbol_name = func_name;
	ret = register_kretprobe(&my_kretprobe);
	if (ret < 0) {
		printk(KERN_INFO "register_kretprobe failed, returned %d\n",
				ret);
		return -1;
	}
	printk(KERN_INFO "Planted return probe at %s: %p\n",
			my_kretprobe.kp.symbol_name, my_kretprobe.kp.addr);
        
 //       ret_foobar = foobar2(3,2,1);
        printk("after invoking, the retval of ret_foobar is %d\n", ret_foobar);
        
         return 0;
}

static void __exit kretprobe_exit(void)
{
	unregister_kretprobe(&my_kretprobe);
	printk(KERN_INFO "kretprobe at %p unregistered\n",
			my_kretprobe.kp.addr);

	/* nmissed > 0 suggests that maxactive was set too low. */
	printk(KERN_INFO "Missed probing %d instances of %s\n",
		my_kretprobe.nmissed, my_kretprobe.kp.symbol_name);
}

module_init(kretprobe_init)
module_exit(kretprobe_exit)
MODULE_LICENSE("GPL");
