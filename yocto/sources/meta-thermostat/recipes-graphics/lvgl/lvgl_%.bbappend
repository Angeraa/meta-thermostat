FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
SRC_URI += "file://lv_conf.h"

# Tell CMake to skip using a custom path flag and use include paths.
EXTRA_OECMAKE += "-DCMAKE_C_FLAGS='-DLV_CONF_INCLUDE_SIMPLE -I${STAGING_INCDIR} -I${STAGING_INCDIR}/libdrm'"

PR:append = ".1"

DEPENDS += "libdrm"

# Apply the application configuration after the base recipe generates its
# configuration from lv_conf_template.h.
do_configure:append() {
    cp -f ${WORKDIR}/lv_conf.h ${S}/lv_conf.h
}

do_install:append() {
    install -d ${D}${includedir}
    install -d ${D}${includedir}/lvgl
    install -m 0644 ${WORKDIR}/lv_conf.h ${D}${includedir}/lv_conf.h
    install -m 0644 ${WORKDIR}/lv_conf.h ${D}${includedir}/lvgl/lv_conf.h
}