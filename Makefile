CFLAGS += -I components/common/thirdparty/100ask_r128_demos/

obj-$(CONFIG_USE_100ASK_MODULE_DRIVER) += module_driver/
obj-$(CONFIG_LV_100ASK_DESKTOP) += lv_100ask_desktop/
obj-$(CONFIG_NWATCH_100ASK) += nwatch/
