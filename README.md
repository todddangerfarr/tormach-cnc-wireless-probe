# Tormach CNC Wireless Probe Project

A Microprocessor project to convert the Tormach CNC Passive Probe [PN 39295](https://tormach.com/passive-probe-kit-bt30-1.html) to a Wireless option. The solution uses two [Adafruit Feather 32u4](https://www.adafruit.com/product/3078) (sometimes referred to as RadioFruits) boards with the RFM69 Radio Modules running at 915.0MHz. One board is setup as the Transmitter (Attached to the probe with a battery pack) TX and the other is setup as the Receiver (powered through the machines ACC1 Port) RX. The Receiver has an [OLED feather wing](https://www.adafruit.com/product/4650) to display the status and signal strength of the Connected Probe.

If at anytime the RX stops receiving data from the TX probe for more than 2 seconds it goes into a default state and locks the ACC1 port to ON.  Path pilot will not allow any probing routines to take place if this signal is tried. 
