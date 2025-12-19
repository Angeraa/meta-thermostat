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

V0.1 is now in development and the main focus of this version will be to add a functioning UI and allow for runtime configuration of HVAC control values like the heat and cool setpoints. Having persistant storage of these values will be in V0.2
