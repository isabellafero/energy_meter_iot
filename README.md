# energy_meter_iot
My final paper for degree in control engineering and automation.
Paper link: http://www.monografias.ufop.br/handle/35400000/1795

### Abstract
The present work presents the development of a prototype for home automation
capable of monitoring and controlling electrical devices through a cloud application.
Based on the Internet of Things (IoT) system, this device is capable of measuring the
current and alternating voltage of an equipment connected to the electrical grid and
also calculate the approximate of the power consumed, in addition to its activation.
The user interface is performed using the ThingSpeak application, where the device
data is monitored, stored and exported to a worksheet where the energy consumed
is calculated. In the application there are also two On and Off buttons capable of
triggering the load. A remote load trigger test and sensor tests were performed using
a halogen lamp and then a desktop. After the tests, it was verified the viability of
the system allowing its use for remote energy management of devices connected to
the alternating energy grid.

#### Components

* 1x ESP32
* 1x ACS712-05B
* 1x 4n25
* 4x 1n4007
* 1x 390k ohm
* 1x 15k ohm
* 1x 1nF

#### Voltage Circuit

![](/img/voltage_circuit.PNG)

#### Calibration curve

![](/img/voltage_calibration.PNG)

![](/img/current_calibration.PNG)
