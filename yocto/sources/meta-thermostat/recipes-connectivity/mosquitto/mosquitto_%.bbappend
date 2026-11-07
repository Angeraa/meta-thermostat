# The broker only serves the local thermostat app; don't block boot on network-online.target.
do_install:append() {
    sed -i -e 's/^After=network-online.target$/After=network.target/' \
           -e '/^Wants=network-online.target$/d' \
           ${D}${systemd_system_unitdir}/mosquitto.service
}
