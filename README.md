
# STM32 with Wifi connection using ESP and MQTT protocol.
This project uses as STM32F446 MCU and a ESP8266 for wifi connection, to implement the MQTT protocol 
  
# Target
Cortex-M4 STM32F446RE, but the drivers can be easily converted to any other STM32 F4 family just correcting the memory mapping.
If you intend to change the target, but sure to create a new project for your MCU, and import the libraries. 
Compile usign arm-none-eabi-gcc 


## IDE
This project uses a STM32CubeIDE, which is a Eclipse based IDE. So any other Eclipse version is capable of convert the .project file. 

## Instructions

1. Clone the project repository: `git clone https://github.com/Fernandoks/MQTT_ESP8266.git `
2. Using STM32CubeIDE (https://www.st.com/en/development-tools/stm32cubeide.html) and import the project
3. Compile using arm-none-eabi-gcc 
