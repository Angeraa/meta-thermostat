FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
SRC_URI += "file://lv_conf.h"

# Tell CMake to skip using a custom path flag and use include paths.
EXTRA_OECMAKE += "-DLV_CONF_INCLUDE_SIMPLE=ON"

# Physically copy the configuration header into the expected source tree root
do_configure:prepend() {
    # For LVGL v9, copying it directly into the source root allows 
    # it to be found via standard relative checks or simple inclusion.
    cp ${WORKDIR}/lv_conf.h ${S}/lv_conf.h
}

do_install:append() {
    install -d ${D}${includedir}
    install -m 0644 ${WORKDIR}/lv_conf.h ${D}${includedir}/lv_conf.h
}