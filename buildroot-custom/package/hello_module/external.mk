HELLO_MODULE_VERSION = 0.1
HELLO_MODULE_SITE = $(BR2_EXTERNAL_FLAPPY_PATH)/../hello_module
HELLO_MODULE_SITE_METHOD = local

$(eval $(kernel-module))
$(eval $(generic-package))
