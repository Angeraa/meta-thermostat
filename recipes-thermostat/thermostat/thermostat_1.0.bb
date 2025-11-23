SUMMARY = "THermostat recipe"
DESCRIPTION = "Recipe for the thermostat application"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://lib/bme68x/LICENSE;md5=8deb9dfcdd5806cbcd2de80ef36d1b12"

python do_display_banner() {
    bb.plain("*******************************************");
    bb.plain("*                                         *");
    bb.plain("*  Recipe for the thermostat application  *");
    bb.plain("*                                         *");
    bb.plain("*******************************************");
}

addtask display_banner before do_build

SRC_URI = "file://CMakeLists.txt \
           file://thermostat.service \
           file://src/ \
           file://include/ \
           git://github.com/boschsensortec/BME68x_SensorAPI.git;branch=master;protocol=https;destsuffix=lib/bme68x \
"

S = "${WORKDIR}"

inherit cmake systemd

SYSTEMD_SERVICE:${PN} = "thermostat.service"
SYSTEMD_AUTO_ENABLE = "enable"

RPI_EXTRA_CONFIG:append = " dtoverlay=msensor-i2c,i2c_addr=0x76"

do_install() {
    install -d ${D}${bindir}
    install -m 0755 Thermostat ${D}${bindir}/Thermostat

    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/thermostat.service ${D}${systemd_system_unitdir}
}
