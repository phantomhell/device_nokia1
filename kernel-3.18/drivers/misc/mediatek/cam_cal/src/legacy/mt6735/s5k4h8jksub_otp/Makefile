#include $(srctree)/drivers/misc/mediatek/Makefile.custom
#obj-y += s5k4h8jksubmipi_Sensor.o
ifeq ($(TARGET_BUILD_VARIANT),user)
#$(error yujinyun $(TARGET_BUILD_VARIANT))
ccflags-y += -D_LOG_USER_
endif
obj-y += s5k4h8jksub_otp.o


