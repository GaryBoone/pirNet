# pirNet

_A distributed home activity display system based on Passive InfraRed sensors and LED displays_

pirNet is an ESP8266 microcontroller project that displays activity information on an LED matrix display. It uses Passive InfraRed (PIR) sensors to detect activity, then sends its location to other pirNet devices on the network. The display shows activity of its sensors and the activity received from other sensors. A group of up to eight of these sensors/displays can be placed throughout a house to create a distributed display of movement.

The display allocates a 2x2 square of LEDs to a given sensor. You can configure eight sensors, filling the 4 by 8 LED display. You set the floor, room, and color of each sensor through a web page. They're organized into two floors of four rooms each, but that's arbitrary: you have eight positions on the display.

## Image
![pirNet Assembled](imgs/pirNet.jpg)

## Hardware

* [Adafruit Feather HUZZAH with ESP8266 WiFi](https://www.adafruit.com/products/2821)
* [Adafruit NeoPixel FeatherWing - 4x8 RGB LED Add-on For All Feather Boards](https://www.adafruit.com/products/2945)
* [Adafruit PIR (motion) sensor](https://www.adafruit.com/products/189)
* [Feather Header Kit - 12-pin and 16-pin Female Header Set](https://www.adafruit.com/products/2886)


## Assembly
Solder the PIR sensor to the NeoPixel as follows:

* NeoPixel FeatherWing pin USB --> PIR RED wire, +5V on PIR board
* NeoPixel FeatherWing pin 12 --> PIR YELLOW wire, OUT on PIR board
* NeoPixel FeatherWing pin GND --> PIR BLACK wire, GND on PIR board

As you can see in the photo, I soldered the wires to the LED board and secured them with zipties to prevent stress on the solder joints.

## Software
* [PlatformIO](http://platformio.org/)
* The software in this repository ([github.com/GaryBoone/pirNet](https://github.com/GaryBoone/pirNet))
* The following libraries should be installed into PlatformIO:
  * [ 13  ] Adafruit-GFX
  * [ 28  ] Adafruit-NeoPixel
  * [ 64  ] Json
  * [ 135 ] Adafruit-SSD1306
  * [ 567 ] WifiManager

## Setup

#### Load software

Attach the Huzzah to your computer with a USB cable. Install the software with 

    $ pio run -t install

You can run

    $ screen /dev/cu.SLAB_USBtoUART 115200

while the device is connected via USB to see the serial output. Among the first lines, you'll see the IP address of the device:

    OTA updates enabled
    Configuration server started.
    Software version: 20160503214912
    Local IP: 192.168.1.147
    Status: Connected
    Mode: Station
    ESP8266 Location: floor=1, room=4, color=0x00f33f17

Once you know the IP address, you can update the code over the air (OTA):

    $ pio run -t upload --upload-port=192.168.1.147
    
#### Bulk uploads

Because pirNet is intended to work with a number of devices, there's a Bash script included that will compile and upload code to a number of devices. Just list the last octets of their IP addresses:

    $ ./upload.sh 119 124 147 154

Run the script without arguments to see more details.

#### Joining your network

When you first power up the device, it will create a network called "PenumbralCogitation". _With your computer_, join that network using password "objectsmean". A captive portal web page will appear asking you to choose a local network. After entering credentials, hit the reset button on the pirNet device. If you have the pirNet device connected via USB and are using _screen_ as shown above, you'll see the device restart and join your network.

_With your computer_, rejoin your normal network. 

#### Configuring pirNet devices

Once the device is attached to your network, you can configure its location and color via a webpage. Just enter the device's IP address into your browser's address bar and hit return.

Example:

    http://192.168.1.147

The configuration web page allows you to set the floor, room, and color of each device. Floors may be 1 or 2. Rooms may be 1 through 4. Each room will be assigned a square of 4 pixels on the 4 by 8 LED display. 


#### Available Web Pages

* __/__ to configure the device.
* __/reset__ to reset the device.



## License ##
The code is available at github [GaryBoone/pirNet](https://github.com/GaryBoone/pirNet) under the [MIT license](http://opensource.org/licenses/mit-license.php).