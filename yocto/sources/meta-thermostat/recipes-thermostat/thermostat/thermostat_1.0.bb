SUMMARY = "Personal thermostat application"
DESCRIPTION = "C++ thermostat application with MQTT, GPIO, BME680, and LVGL touchscreen support"

# The application contains the Bosch BSEC binary, which is distributed under
# Bosch's terms and is not redistributable as an Open Source license. The
# bme68x C source also contains a BSD license, but the combined application
# must be treated as CLOSED while the BSEC artifact is used.
LICENSE = "CLOSED"

# The checked-in BSEC archive is built for Raspberry Pi 5's 64-bit target.
# COMPATIBLE_HOST would test the build workstation and would incorrectly skip
# this recipe on the normal x86-64 Yocto build host.
COMPATIBLE_MACHINE = "raspberrypi5"

# The application source is kept at the repository root rather than copied
# into the layer. FILESEXTRAPATHS makes that source available to the local
# file fetcher while retaining normal BitBake unpack/checksum behavior.
FILESEXTRAPATHS:prepend := "${THISDIR}/../../../../..:"

python do_display_banner() {
    bb.plain("*******************************************");
    bb.plain("*                                         *");
    bb.plain("*  Recipe for the thermostat application  *");
    bb.plain("*                                         *");
    bb.plain("*******************************************");
}

addtask display_banner before do_build

DEPENDS = " \
    boost \
    libdrm \
    libgpiod \
    lvgl \
    paho-mqtt-c \
    paho-mqtt-cpp \
"

# Shared-library dependencies are discovered by the packaging machinery.
# Mosquitto is explicit because the systemd unit requires its broker service.
RDEPENDS:${PN} = "mosquitto"

SRC_URI = " \
    file://thermostat/CMakeLists.txt \
    file://thermostat.service \
    file://thermostat/src/ \
    file://thermostat/include/ \
    file://thermostat/lib/ \
"

S = "${WORKDIR}/thermostat"

inherit cmake systemd

EXTRA_OECMAKE += "-DCMAKE_INSTALL_PREFIX=${prefix} -DCMAKE_INSTALL_LIBDIR=${baselib}"

SYSTEMD_SERVICE:${PN} = "thermostat.service"
SYSTEMD_AUTO_ENABLE = "enable"

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${B}/Thermostat ${D}${bindir}/Thermostat

    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/thermostat.service ${D}${systemd_system_unitdir}/thermostat.service

    # Keep a copy of the target binary in the source tree for direct debugging.
    install -d ${TOPDIR}/../../thermostat/yocto-build
    install -m 0755 ${B}/Thermostat \
        ${TOPDIR}/../../thermostat/yocto-build/Thermostat
}

FILES:${PN} += "${systemd_system_unitdir}/thermostat.service"