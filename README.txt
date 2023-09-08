# STM32-SID-PLAYER
SID chip and 6502 CPU emulator, with audio output on pin PA8 (PWM) or PA4 (DAC), for STM32 line of microcontrollers, compiled with Arduino IDE
  My personal breadboard project that i'm still having fun with . 
      - Mainly done for STM32F103C8 Blue Pill (PWM) and STM32G431CBU6 (WeAct board) (DAC)
      - Should work with other STM32 microcontrollers that Arduino IDE support.
      - Tested also at STM32F401CCU6, STM32F411CEU6 and STM32F407VET6 (PWM/DAC) dev boards (and as i see, they all have same pin (PA8) for Timer1,channel1) and same pins for SPI.

Demonstration video: 
 - internal flash memory player  https://www.youtube.com/watch?v=pvWagY7ppYg
 - SD Card player                https://www.youtube.com/watch?v=dMKJ6nt50_M
 - IRQinternal version           https://www.youtube.com/watch?v=eym6_oySOKk
 

Much more details are in versions subfolder.
  
HAVE FUN :-)
  
  
PS: Special thanks:

        - Daniel Muszynski for great help on this project. Checkout his YT page for real SID player:
        
        https://www.youtube.com/user/dkjm1978/
        
        
        - cbm80amiga ,  Checkout his YT page for so much great stuff for Blue pill:
        
        https://www.youtube.com/user/cbm80amiga/


PS2: Also thanks for people who got interested into this project:

        - Edu Arana      https://www.youtube.com/watch?v=_bBsGYvaXog               - STM32F401CE PCB board
        - Matteo         https://www.youtube.com/watch?v=mWr3eCfsWOw               - STM32F103C8 + I2C LCD screen on DIY board
        - Noplan         https://www.youtube.com/channel/UCru3FcoymFzAeXwfSxfGPoQ  - STM32F103C8
        - Xad Nightfall  https://www.youtube.com/watch?v=O6zrsIFlbew               - STM32F407VE dev board + LCD

PS3: Forks worth checking:
	- Matteo	https://github.com/matt199394/STM32-SID-PLAYER
	- Christmas SID	https://github.com/texelec/ChristmasSID
        
