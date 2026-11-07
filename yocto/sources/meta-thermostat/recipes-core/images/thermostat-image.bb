SUMMARY = "Minimal image for the personal thermostat"
DESCRIPTION = "Small systemd image containing the thermostat application, MQTT broker, GPIO, DRM, and touchscreen support"
LICENSE = "MIT"

require recipes-core/images/core-image-base.bb

# The image must be built with systemd selected by the distro or local.conf.
# This prevents silently producing an image whose init system cannot manage
# the thermostat and mosquitto service units.
REQUIRED_DISTRO_FEATURES = "systemd usrmerge opengl"

IMAGE_LINGUAS = " "
IMAGE_FEATURES += "empty-root-password serial-autologin-root"
EXTRA_IMAGE_FEATURES:append = " ssh-server-dropbear"

# packagegroup-core-boot provides the boot-essential userspace and device
# manager. The application recipe supplies its shared-library dependencies;
# these explicit entries document the hardware/runtime pieces of this image.
# LVGL uses the kernel evdev interface directly, so libinput is not required.
IMAGE_INSTALL:append = " \
    libdrm \
    libgpiod \
    libgpiodcxx \
    lvgl \
    mosquitto \
    thermostat \
    gdbserver \
"

# The Pi 5 I2C controller (RP1, /dev/i2c-1) is a module that packagegroup-core-boot does not pull in.
IMAGE_INSTALL:append:raspberrypi5 = " kernel-module-i2c-designware-platform"
