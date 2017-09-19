#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x710e0d3b, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x619cb7dd, __VMLINUX_SYMBOL_STR(simple_read_from_buffer) },
	{ 0x68e2f221, __VMLINUX_SYMBOL_STR(_raw_spin_unlock) },
	{ 0x359ee64f, __VMLINUX_SYMBOL_STR(generic_file_llseek) },
	{ 0xb7ce9e9b, __VMLINUX_SYMBOL_STR(debugfs_create_dir) },
	{ 0x779a18af, __VMLINUX_SYMBOL_STR(kstrtoll) },
	{ 0xc0241fac, __VMLINUX_SYMBOL_STR(register_kretprobe) },
	{ 0xa4eb4eff, __VMLINUX_SYMBOL_STR(_raw_spin_lock_bh) },
	{ 0xdcdefc8d, __VMLINUX_SYMBOL_STR(nf_register_hook) },
	{ 0x2447533c, __VMLINUX_SYMBOL_STR(ktime_get_real) },
	{ 0x8b35cde9, __VMLINUX_SYMBOL_STR(debugfs_create_file) },
	{ 0xc9aca5d0, __VMLINUX_SYMBOL_STR(debugfs_remove_recursive) },
	{ 0xecc32caa, __VMLINUX_SYMBOL_STR(simple_attr_read) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xe1ba164b, __VMLINUX_SYMBOL_STR(unregister_kretprobe) },
	{ 0x32740957, __VMLINUX_SYMBOL_STR(simple_attr_release) },
	{ 0x8bf826c, __VMLINUX_SYMBOL_STR(_raw_spin_unlock_bh) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
	{ 0x67f7403e, __VMLINUX_SYMBOL_STR(_raw_spin_lock) },
	{ 0xbb4f4766, __VMLINUX_SYMBOL_STR(simple_write_to_buffer) },
	{ 0x2a35968d, __VMLINUX_SYMBOL_STR(nf_unregister_hook) },
	{ 0x68466925, __VMLINUX_SYMBOL_STR(simple_attr_open) },
	{ 0x8de200ba, __VMLINUX_SYMBOL_STR(simple_attr_write) },
	{ 0x4cdb3178, __VMLINUX_SYMBOL_STR(ns_to_timeval) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "071E09696EEDDD12443AEA4");
