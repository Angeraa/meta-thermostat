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
           file://lib/ \
"

S = "${UNPACKDIR}"

inherit cmake systemd

EXTRA_OECMAKE += "-DCMAKE_INSTALL_PREFIX=${prefix}"

SYSTEMD_SERVICE:${PN} = "thermostat.service"
SYSTEMD_AUTO_ENABLE = "enable"

RPI_EXTRA_CONFIG:append = " dtoverlay=msensor-i2c,i2c_addr=0x76"

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${B}/Thermostat ${D}${bindir}/Thermostat

    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${UNPACKDIR}/thermostat.service ${D}${systemd_system_unitdir}
}
