### Temperature, Humidity, and Pressure Sensor

The BME280 is a combined digital temperature, humidity, and pressure sensor. It comes in a small 2.5 mm x 2.5 mm package. A breakout board with the necessary pullup resistors and voltage translator ICs can be purchsed online at various places. I bought mine from Aliexpress.

[Link to buy](https://www.aliexpress.com/item/33052320902.html?spm=a2g0o.productlist.0.0.d96023892YdO9c&algo_pvid=8ee19218-7c1c-4fef-bfe3-3f53120eccb9&algo_expid=8ee19218-7c1c-4fef-bfe3-3f53120eccb9-1&btsid=225280c4-0977-47d0-8cf9-4e644d557fc9&ws_ab_test=searchweb0_0,searchweb201602_5,searchweb201603_52)

The sensor can be communicated via:
- Two Wire I2c
- 3 Wire SPI
- 4 Wire SPI

For this driver, I decided to go with I2C for a couple of reasons. **One**, it requires the least amount of wires and **Two**, even at I2C the device supports 3.4 MHz speed.

<img src="images/bme280_arduino_driver/bme280_back.jpg" width="250px" height="250px"> <img src="images/bme280_arduino_driver/bme280_back.jpg" width="250px" height="250px">

For this example, I will be using I2C to communicate with the sensor. When using I2C, the **CSB** pin is tied to **VCC**. The **SDO** pin is connected to GND, setting the device address to **0x76**.

The table also shows the connection to an Arduino.

<table>
						<thead>
							<tr>
								<th>Pin</th>
								<th>Name</th>
								<th>I/O Type</th>
								<th>Description</th>
								<th>I2C</th>
								<th>Arduino Nano</th>
							</tr>
						</thead>
						<tbody>
							<tr>
								<td>1</td>
								<td>VCC</td>
								<td>In</td>
								<td>Voltage in pin: <b>1.7V to 3.6V</b></td>
								<td>VCC</td>
								<td><b>+3.3V</b></td>
							</tr>
							<tr>
								<td>2</td>
								<td>GND</td>
								<td>In</td>
								<td>Ground pin</td>
								<td>GND</td>
								<td><b>GND</b></td>
							</tr>
							<tr>
								<td>3</td>
								<td>SCL</td>
								<td>In</td>
								<td>Serial Clock Pin</td>
								<td>SCL</td>
								<td><b>A5</b></td>
							</tr>
							<tr>
								<td>4</td>
								<td>SDA</td>
								<td>In/Out</td>
								<td>Serial Data In/Out</td>
								<td>SDA</td>
								<td><b>A4</b></td>
							</tr>
							<tr>
								<td>5</td>
								<td>CSB</td>
								<td>In</td>
								<td>Chip Select</td>
								<td>Tie to VCC</td>
								<td><b>+3.3V</b></td>
							</tr>
							<tr>
								<td>6</td>
								<td>SDO</td>
								<td>Out</td>
								<td>Serial Data Out</td>
								<td>GND: I2C Addr = 0x76<br>
									VCC: I2C Addr = 0x77
								</td>
								<td><b>GND</b></td>
							</tr>
						</tbody>
					</table>

Link to my BME280 drivers for the arduino can be found on my github page: [Here](https://github.com/hshah89/BME280_Arduino_Driver)

The first part is to include the BME280 library.

The next part is to define the baudrate for the serial port and the device address. In this case, the device address is set to 0x76.

After setting up the library, baudrate, and the device address, declare the global variables that will be used throughout the program. At a minimum, you need to declare the object BME280 to use the sensor library.

Next is to write the setup function. Here, the I2C bus and the Serial port are initialized. The BME280 sensor is started up as well. The I2C address is set based on the connections of the **SDO** pin. We look for the Chip ID. It should return back in hex **0x60**. If it does not, check the connections to the arduino.
After that, the oversampling settings for the temperature, pressure, and humidity readings are set.

The device also supports 2 different operating modes: **Normal** and **Forced**. In **Normal** mode, the sensor is always on. In **Forced** mode, when a reading is requested, the device turns on, returns the requested reading and then goes back to sleep. Forced mode is useful when running the sensor from a battery.

The **getData()** arduino function is shown below. It gets the temperature, humidity, and pressure. This function is declared so you don't have to keep repeating same commands over and over.

Finally, the loop function is configured. You call the **getData()** function and then delay it a certain amount.

If everything runs correctly, you should get the following output after programming the arduino board:

That should get you going with the sensor. Now that it's working with the Arduino board, you can use this sensor with any project. Stay tuned for a projected using this sensor.