KERN_VER := $(shell uname -r)
KDIR ?=/lib/modules/$(KERN_VER)/build
PWD := $(shell pwd)
BUILD_DIR=$(PWD)/build
BUILD_DIR_MAKEFILE = $(BUILD_DIR)/Makefile

all:$(BUILD_DIR_MAKEFILE)
	make -C $(KDIR) M=$(BUILD_DIR) src=$(PWD) modules
	@cp $(BUILD_DIR)/*.ko $(PWD)

$(BUILD_DIR):
	mkdir -p "$@"
 
$(BUILD_DIR_MAKEFILE): $(BUILD_DIR)
	touch "$@"

clean:
	make -C $(KDIR) M=$(BUILD_DIR) src=$(PWD) clean
	@rm -rf $(BUILD_DIR)
	@rm -rf $(PWD)/*.ko

obj-m += i2c_char_dev_driver.o 
obj-m += i2c_board_info_driver.o
obj-m += i2c_base_driver.o
obj-m += i2c_attr_driver.o
