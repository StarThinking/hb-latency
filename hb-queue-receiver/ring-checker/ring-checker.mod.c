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
	{ 0x9ac8e80d, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xb6b46a7c, __VMLINUX_SYMBOL_STR(param_ops_int) },
	{ 0xc97cdeae, __VMLINUX_SYMBOL_STR(debugfs_remove) },
	{ 0xbadbb8df, __VMLINUX_SYMBOL_STR(nf_unregister_hook) },
	{ 0xad24f741, __VMLINUX_SYMBOL_STR(debugfs_create_u8) },
	{ 0xb2c803a1, __VMLINUX_SYMBOL_STR(debugfs_create_dir) },
	{ 0xd6060e6e, __VMLINUX_SYMBOL_STR(nf_register_hook) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "D40CBF8AEBFB570AC911641");
