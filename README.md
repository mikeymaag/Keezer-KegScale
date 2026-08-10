This is a KegScale system to measure the volume of 5 gallon Cornelius kegs inside of a keezer or kegerator. This project uses Arduino compatible microcontroller, MQTT and Node-Red.

KegScale.ino - is the c++ code for the micro-controller.  I used an ESP32 DevBoard.
KegScaleObj.cpp - is the c++ code for the KegScale object.  One of these are instantiated for each scale in your system.
KegScaleObj.h - is the complementary H file for the KegScaleObj.cpp file
config.h - is the configuration file that describes which pins were used on the microcontroller to connect the scales.  There are two pins utilized per scale, one for the clock and one for the data.
secret.h - is the file that contains the UserID and passwords for the WiFi and the MQTT Broker.
KegScale-NodeRed.Flows.json - is the Node-Red flow export that presents the keg levels.
KegScalePresentation.pdf - is a pdf version of a powerpoint presentation describing the system.
