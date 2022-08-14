# AD9833-DDS-Generator

## Description
***This project will be archived. Please look at the ![new repository](https://github.com/Ivanchenko59/DDS-Generator) with the continuation of the project on another STM32 chip.*** 

A simple signal generator based on the ***AD9833*** chip. 
The project is built on the ***STM32F401*** microcontroller
using a 1.8 inch TFT display based on the ***ST7735*** driver, 
and an ***EC11*** rotary encoder is also used for control.

## About the project
The purpose of this project is to create a small ***educational 
project*** based on the STM32 microcontroller using the HAL library. 
And also, ***writing a library of the AD9833 chip for experience*** 
with technical documentation. I also tried to document my first library. 

In the near future, I will publish the library in a separate repository.

## Demo Video
Here you can watch a video of the demo version.

[![Watch the video](https://img.youtube.com/vi/vuJpcqhaOHE/maxresdefault.jpg)](https://youtu.be/vuJpcqhaOHE)

## Some Photos

<img src="https://user-images.githubusercontent.com/80352225/147411294-d064a6ac-9c03-4451-bf48-b480c6d194d0.png" width="700" height="500">
<img src="https://user-images.githubusercontent.com/80352225/147411301-9e7306ec-843a-4bc4-bdb0-118933c6111e.png" width="700" height="500">
<img src="https://user-images.githubusercontent.com/80352225/147411309-43c8c370-bd6c-4d15-a22b-6f7558e3a7e9.png" width="700" height="500">
<img src="https://user-images.githubusercontent.com/80352225/147411313-8d32928f-dfe4-4d12-9ad8-56a91a9bc07c.png" width="700" height="500">


## Connectivity
### Current device connection. Will change as new features are added and the PCB is designed.

##### STM32CubeIDE connectivity

![image](https://user-images.githubusercontent.com/80352225/147410584-d2469fcf-5edc-48b4-8d60-de47e59898d7.png)

##### SPI1 Config (st7735)

![image](https://user-images.githubusercontent.com/80352225/147410549-49b7d7c5-3afa-4f7c-831b-77b8127cbf8d.png)
##### SPI2 Config (AD9833)
![image](https://user-images.githubusercontent.com/80352225/147410534-360b5c84-5f3d-4516-9f32-1d134b7deae6.png)


## Future Release 
* Sweep mode
* Change signal amplitude
* PCB design
* Development of a case for the device
* Create documentation
* Full github project with a demo of work an oscilloscope
