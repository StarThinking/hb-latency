#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h> 
#include <linux/moduleparam.h>

#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <uapi/linux/netfilter_ipv4.h>
#include <net/ip.h>
#include <linux/inet.h>
#include <linux/ktime.h>
#include <linux/skbuff.h>

#include <linux/debugfs.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#include "../../../include/hb_common.h"

//#define ONE_TO_ONE
#define TWO_CPUS

MODULE_LICENSE("GPL");

static struct nf_hook_ops nfho0;

static struct dentry *clear;
static u32 queue_index;
static struct dentry *queue_index_entry;
static struct dentry *dir;
static u32 sports[IRQ_NUM];
static struct dentry *files[IRQ_NUM];

static int write_op(void *data, u64 value) {
        int i;
        for(i=0; i<IRQ_NUM; i++)
                sports[i] = 0;
        return 0;
}

DEFINE_SIMPLE_ATTRIBUTE(clear_fops, NULL, write_op, "%llu\n");

/*hook function*/
//unsigned int hook_func(const struct nf_hook_ops *ops, struct sk_buff *skb, 
//        const struct net_device *in, const struct net_device *out, 
//        int (*okfn)(struct sk_buff *)) {
unsigned int hook_func(void *priv, struct sk_buff *skb, const struct nf_hook_state *state) {
        struct iphdr *ip;
        struct tcphdr *tcp;
        unsigned int sport, dport, saddr, daddr;
        unsigned int irq_vec, queue_mapping, proto;
        const char *in_name, *out_name;
        char str[INET_ADDRSTRLEN];
        int cpu = get_cpu();

        ip = (struct iphdr *) skb_network_header(skb);
        saddr = (unsigned int) ip->saddr;
        daddr = (unsigned int) ip->daddr;
        sport = dport = 0;

        irq_vec = skb->napi_id; // this value is modified to be irq in driver
        queue_mapping = skb->queue_mapping; // it's always 0
        proto = ip->protocol;
        in_name = state->in->name;
        out_name = state->out->name;

        if (ip->protocol == IPPROTO_TCP) { 
                tcp = (struct tcphdr *) skb_transport_header(skb);
                sport = (size_t) ntohs(tcp->source);
                dport = (size_t) ntohs(tcp->dest);

                sprintf(str, "%pI4", &saddr);
                
                if (dport == BARRIER_SERVER_PORT && strcmp(str, BARRIER_CLIENT_ADDR) == 0) {
                        printk(KERN_DEBUG "[msx] hooknum %u, %pI4:%u --> %pI4:%u, irq_vec = %u,"
                                " cpu = %d, prot = %u, in = %s, out = %s\n", state->hook, &saddr, 
                                sport, &daddr, dport, irq_vec, cpu, proto, in_name, out_name);
                     
#ifdef ONE_TO_ONE                        
                        if (cpu < 0 || cpu > 4)
                                printk("[msx] Error: cpu id out of bound!\n");
                        else {
                                if (sports[cpu] == 0) { // no tcp connection before, record the sport 
                                        printk("[msx] The sport of queue with cpu %d is %u\n", cpu, sport);
                                        sports[cpu] = sport;
                                }
                        }
#endif

#ifdef TWO_CPUS
                        if (cpu != 0 && cpu != 1)
                                printk("[msx] Error: cpu id out of bound!\n");
                        else {
                                if (cpu == 1 && sports[queue_index] == 0) {
                                        printk("[msx] The sport of queue %d with cpu %d is %u\n", 
                                                BASE_IRQ + queue_index, cpu, sport);
                                        sports[queue_index] = sport;
                                }
                        }
#endif
                }
        } 
        return NF_ACCEPT; 
}

int init_module() {
        int i;

        nfho0.hook = hook_func;        
        nfho0.hooknum = NF_INET_PRE_ROUTING; 
        nfho0.pf = PF_INET; // IPV4 packets
        nf_register_hook(&nfho0);  
       
        dir = debugfs_create_dir(HB_SERVER_ADDR, NULL);
        for (i=0; i<IRQ_NUM; i++) {
                char irq_str[8];
                sprintf(irq_str, "%d", BASE_IRQ + i);
                files[i] = debugfs_create_u32(irq_str, 0644, dir, &sports[i]);
        }

        queue_index_entry = debugfs_create_u32("queue_index", 0644, dir, &queue_index);
        if (!queue_index_entry) {
                // Abort module load.
                printk(KERN_ALERT "[msx] Failed to create queue_index_entry\n");
                return -1;
        }
        
        clear = debugfs_create_file("clear", 0222, dir, NULL, &clear_fops);
        if (!clear) {
                // Abort module load.
                printk(KERN_ALERT "[msx] Failed to create clear\n");
                return -1;
        }
        
        return 0; 
}

void cleanup_module() {
        int i;

        nf_unregister_hook(&nfho0);   
        
        for (i=0; i<IRQ_NUM; i++) {
                debugfs_remove(files[i]);
        }
        debugfs_remove(clear);
        debugfs_remove(queue_index_entry);
        debugfs_remove(dir);
}
