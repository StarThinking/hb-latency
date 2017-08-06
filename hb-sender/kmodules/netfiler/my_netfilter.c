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

MODULE_LICENSE("GPL");

static struct nf_hook_ops nfhos[5];
static const int hooknums[5] = {NF_INET_PRE_ROUTING, NF_INET_LOCAL_IN, NF_INET_FORWARD, NF_INET_POST_ROUTING, NF_INET_LOCAL_OUT};

static unsigned int hook_func(const struct nf_hook_ops *ops, struct sk_buff *skb, 
        const struct net_device *in, const struct net_device *out, 
        int (*okfn)(struct sk_buff *)) {
        
        struct iphdr *iph;
        unsigned int saddr, daddr;
        char saddr_ip[INET_ADDRSTRLEN];
        char daddr_ip[INET_ADDRSTRLEN];

        iph = (struct iphdr *) skb_network_header(skb);
        saddr = (unsigned int) iph->saddr;
        daddr = (unsigned int) iph->daddr;
        sprintf(saddr_ip, "%pI4", &saddr);
        sprintf(daddr_ip, "%pI4", &daddr);

        if(strcmp(saddr_ip, "10.0.0.1") != 0 && strcmp(daddr_ip, "10.0.0.1") != 0) 
                printk(KERN_DEBUG "[msx] hooknum = %d, %pI4 --> %pI4\n",ops->hooknum,  &saddr, &daddr);

        return NF_ACCEPT; 
}

int init_module() {
        int i = 0;
        for(; i<=4; i++) {
                nfhos[i].hook = hook_func;        
                nfhos[i].hooknum = hooknums[i]; 
                nfhos[i].pf = PF_INET; // IPV4 packets
                nf_register_hook(&nfhos[i]);  
        }

        return 0; 
}

void cleanup_module() {
        int i = 0;
        for(; i<=4; i++) {
                nf_unregister_hook(&nfhos[i]);   
        }
}