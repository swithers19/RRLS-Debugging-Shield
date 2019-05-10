# RRLS Debugging Shield

The debugging shield is a simple board that collects analog readings of learning device pins and extracts meaning from how they vary. It can then command the learning board to operate in a particular fashion to identify probable issues. It also acts as the beacon to the rest of the system and controls how the learning device sends information. 


#Hardware
The hardware element is relatively simple, with external ADC's, level shifting between 5V and 3.3V devices and connections for I2C and serial communication that connect to an ESP32 microcontroller. The ESP32 is a low cost board with WiFi functionality and plenty of pins. Below is the schematic for the debugging shield:

![Debugging Shield Schematic](https://raw.githubusercontent.com/swithers19/RRLS-Debugging-Shield/master/docs/Board-Schematic.png)

This resulted in the following PCB, with the pcb design and resulting board seen below:

Resulting Debugging Shield:
![PCB Design and Board](https://raw.githubusercontent.com/swithers19/RRLS-Debugging-Shield/master/docs/PCB-Board.png)


# Software
The code running on the ESP32 was written in C utiling the ESP-IDF. This provided an FreeRTOS to better organise the flow of tasks and minimise downtime. There were four main pieces of functionality:
- Serial Communication with Learning Device
- SPI Communication with external ADC
- Wifi Connectivity & MQTT Communication
- Task management and optimisation using an RTOS

The code was challenging to implement due to the non-linear event sequences and multiple triggers. There are two key sequences, debugging and configuration seen below:
![Task Scheduling](https://raw.githubusercontent.com/swithers19/RRLS-Debugging-Shield/master/docs/Task-Scheduling-and-Transition.jpg)

The following example demonstrates the potential for intertwined tasks running which optimises the speed at which the microcontroller can achieve both tasks:

![Task Scheduling example](https://raw.githubusercontent.com/swithers19/RRLS-Debugging-Shield/master/docs/Task-Scheduling-and-Transition-Example.jpg)