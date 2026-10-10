# Personal Thermostat Layer

This project is intended to be a crack at creating my own personal smart home thermostat built using the yocto project and running on a Raspberry Pi 5.

## V0

V0 includes the most basic functions of a thermostat being:

- The ability to read temperature
- Acting on temperature data to turn on or off HVAC systems

V0 was built with a focus on modularity. Each individual module has a specific purpose and sends and receives data over MQTT topics. Currently there are only two modules excluding the MQTT module which is the Sensor module and the HVAC control module.

##### Sensor Module

The sensor module includes a hardware interface class meant specifically for the Bosch BME680 sensor which includes temperature, humidity, pressure, and air quality. The sensor module publishes to 5 different topics which includes the four values previously mentioned and also the air quality accuracy.

##### HVAC Control Module

THe HVAC control module wraps an HVAC Controller class which contains the state machine for the HVAC system. The HVAC control module class handles the reception of messages over MQTT and handles the threads for the control loop and the message handling.

THe HVAC Controller class contains a loop function which contains the state machine logic. The class also has various settings like minimum on and off time for heating and cooling as well as the heat and cool setpoints. Currently they are hardcoded but once a UI is made they will be configurable.

## V0.1

V0.1 is now in development and the main focus of this version will be to add a functioning UI and allow for runtime configuration of HVAC control values like the heat and cool setpoints. Having persistant storage of these values will be in V0.2 but currently there is a ConfigModule that owns an in-memory AppState struct that other modules can reference to get access to state.

Changes to the messaging system has also been done in v0.1 to support the UI addition. Now messages are sent to an internal queue that will then handle them appropriately as many of the messages update state so it is mostly just updating the previously mentioned AppState.

## Yocto build

The Yocto layer is in `yocto/sources/meta-thermostat`. The application source
and the layer are intentionally kept in the same repository; the application
recipe fetches the source tree from the repository root.

This layer requires the `meta-oe` and `meta-networking` collections from
`meta-openembedded`:

- `libgpiod`, Boost, LVGL, and Paho MQTT are provided by `meta-oe`.
- Mosquitto is provided by `meta-networking`.
- `meta-raspberrypi` is recommended for the Raspberry Pi 5 machine, display
	overlays, and I2C configuration. It is a layer recommendation rather than a
	hard dependency so the application recipe can still be parsed for another
	aarch64 machine.

The checked-in `lib/bsec/libalgobsec.a` is an aarch64 prebuilt Bosch BSEC
library. The recipe therefore only supports an aarch64 Linux target and the
artifact must be redistributed according to Bosch's license terms.

Configure the build with systemd before building the image:

```text
DISTRO_FEATURES:append = " systemd usrmerge"
DISTRO_FEATURES:remove = " sysvinit"
VIRTUAL-RUNTIME_init_manager = "systemd"
MACHINE = "raspberrypi5"
```

For the Raspberry Pi 5 and the official 800x480 DSI touchscreen, add the
following to `yocto/build/conf/local.conf`:

```text
ENABLE_I2C = "1"
RPI_EXTRA_CONFIG:append = " \\
	dtoverlay=vc4-kms-v3d \\
	dtoverlay=vc4-kms-dsi-7inch \\
"
```

Build `thermostat-image` rather than appending packages to
`core-image-base`:

```text
bitbake thermostat-image
```

The image contains only the core boot package group, systemd-selected boot
support, the thermostat application, Mosquitto, LVGL, libgpiod, and libdrm.
LVGL's DRM and evdev support provides display and touchscreen access; the
kernel/BSP must still expose `/dev/dri`, `/dev/input/event*`,
`/dev/gpiochip0`, and `/dev/i2c-1`.
