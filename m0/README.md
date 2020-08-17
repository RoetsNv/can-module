# Software for M0 CAN modules

## Installation

Install the [platformio package](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide)

At first compilation: platform io takes care of downloading and installing all necessary compilers and dependencies

## Change the mcu configuration:

- Goto [Atmel Start](https://start.atmel.com/)
- upload the existing `m0.atstart` config file in this folder
- change the config in the online configurator
- download the new config file again and overwrite the existing `m0.atstart` file
- recompile (will take care of downloading new dependencies)

due to a bug in the atstart download API: the `peripheral_clk_config.h` file is corrupt

- goto `View Code` in the online Atmel start tool
- copy the full content of the `config/peripheral_clk_config.h` file
- paste it in the downloaded package content in `./pio/libdeps/m0/AtmelStart_PlatformIO/packages/[uuid]/config/peripheral_clk_config.h`

## Upload code

- Connect the m0 CAN module with a JLink debugger to your computer
- use the upload functionality of platformio

## Serial monitor

by including `#include "SEGGER_RTT.h"` in the `main.cpp` file, all `printf` statements are sent to the Segger Jlink Serial monitor.
On your computer

- open a Jlink server: `JLinkExe -device ATSAMC21E18 -speed 4000 -if SWD -autoconnect 1` or use the vscode task
- open a RTT Client: `JLinkRTTClient`

## MCU config

### CAN settings

Can bitrate is determined by following formula:

bitrate = CAN Clock / Baudrate prescaler / (1 + Time segment before sample point + Time segment after sample point)

sample point = (1 + Time segment before sample point) / (1 + Time segment before sample point + Time segment after sample point)

See http://www.bittiming.can-wiki.info/
