SUMMARY = "Minimal image for the personal thermostat"
DESCRIPTION = "Small systemd image containing the thermostat application, MQTT broker, GPIO, DRM, and touchscreen support"
LICENSE = "MIT"

inherit core-image features_check

# The image must be built with systemd selected by the distro or local.conf.
# This prevents silently producing an image whose init system cannot manage
# the thermostat and mosquitto service units.
REQUIRED_DISTRO_FEATURES = "systemd usrmerge"

IMAGE_LINGUAS = " "
IMAGE_FEATURES = ""

# packagegroup-core-boot provides the boot-essential userspace and device
# manager. The application recipe supplies its shared-library dependencies;
# these explicit entries document the hardware/runtime pieces of this image.
# LVGL uses the kernel evdev interface directly, so libinput is not required.
IMAGE_INSTALL = " \
    packagegroup-core-boot \
    libdrm \
    libgpiod \
    libgpiodcxx \
    lvgl \
    mosquitto \
    thermostat \
"