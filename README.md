#Tormach Wireless Probe

A Microprocessor project to convert the Tormach CNC Passive Probe PN 39295 to a Wireless option. The solution uses two Adafruit Feather 32u4 boards with the RFM69 Radio Modules running at 915.0MHz. One board is setup as the Transmitter (Attached to the probe with a battery pack) TX and the other is setup as the Receiver (powered through the machines ACC1 Port) RX. The Receiver has an OLED feather wing to display the status and signal strength of the Connected Probe.

If at anytime the Receiver stops receiving data from the Transmitter it goes into a default state and locks the ACC1 port so no more probing can take place.
