# Speakers
Hardware/software for a bluetooth splitter + iphone app, and custom bluetooth speaker

- [Project Directory](#project-directory)
- [Getting Started](#getting-started)
- [Resources](#resources)
	- [Hardware](#hardware)
	- [Software](#software)
- [Bluetooth splitter](#bluetooth-splitter)
- [Bluetooth speaker](#bluetooth-speaker)
- [References](#references--kudos)

Have questions? [Feel free to make a new issue](https://github.com/BarakBinyamin/speakers/issues)

<p align="center">
<img  width="34%" src="img/splitter.jpg"/><img width="65%" src="img/speaker.jpg"/>
</p>
<video controls src="img/demo.mov"></video>

## Project Directory
| Name                                   					| Purpose                                             | 
| :--                                    					| :--                                                 |
|[app](app)                             					| Custom app for selecting speakers to connect to     |
|[imgs](imgs)                            					| Images, docs and old source code                    |
|[include](include)                      					| PlatormIO Folder for maunually adding dependencies  |
|[lib](lib)                              					| PlatormIO Folder for maunually adding dependencies  |                    
|[src](src)                               					| Firmware and docs for a bluetooth splitter & speaker|
|[partition-more-pgrm-mem.csv](partition-more-pgrm-mem.csv) | Memory configuration file for esp32                 |
|[platformio.ini](platformio.ini)                           | PlatformIO configuration file                       |

## Getting Started
1. `git clone https://github.com/BarakBinyamin/speakers.git`
2. [Buy the parts](#hardware), [Download and setup an VScode](#software)  

### Speaker
1. [Build the speaker](src)

### Splitter
In order to build the app you need macOS with Xcode & some other dependencies
1. [Build the splitter](src)
2. [Build the app](app)

## Resources
### Hardware
Custom Speaker Parts
- [Two or more ESP32 dev board's](https://www.ebay.com/sch/i.html?_nkw=esp32+wroom)
- [Max98357 i2s DAC Amplifier](https://www.amazon.com/s?k=max98357+i2s+amplifier)
- [Breadboard's](https://www.amazon.com/s?k=breadboard&crid=3SAWQUGI374BK&sprefix=breadboard+%2Caps%2C331&ref=nb_sb_noss_2)
- [Breadboard Jumper Wires](https://www.amazon.com/s?k=breadboard+jumper+kit)
- [Soldering Iron](https://www.amazon.com/s?k=soldering+iron)
- [Dayton Audio Transducers With Sticky Frame](https://www.amazon.com/s?k=Dayton+Audio+DAEX25+Audio+Exciter+Pair)

[Check out the difference between chipsets](https://www.espressif.com/en/products/socs), basically s3's have 2 cores, s2's have 1 core, S2 WROVERS's have extra ram (accessible with extra work) vs S2 WROOM's 

### Software
App Setup
- [Nodejs](https://nodejs.org/en)

VScode + PlatformIO setup
- [VScode](https://code.visualstudio.com/)
- [PlatformIO for VScode](https://platformio.org/install/ide?install=vscode)



## Some other useful resources
- [Rui & Sarah Santos ESP32 Tutorials](https://randomnerdtutorials.com/getting-started-with-esp32/)
- [Simple ESP32 bluetooth/i2s library by @pschatzmann](https://github.com/pschatzmann/ESP32-A2DP)
- [Fritzing](https://fritzing.org/) Arduino diagram/pcb tool 
- [Fritzing esp32 board](https://forum.fritzing.org/t/doit-esp32-devkit-v1/6158/8), [DOIT Esp32 DevKit v1 improved.fzpz](https://forum.fritzing.org/uploads/default/original/2X/5/52c6aaad54a039b8412a393cc22f929288fa2ac3.fzpz)
- [PlatformIO with multiple main](https://community.platformio.org/t/here-is-a-simple-way-to-have-many-small-main-apps/26009)

## References & Kudos
- [Professor Yang's](https://www.rit.edu/directory/sjyeec-shanchieh-yang) guidance on data communications
- [Dr. Kwasinski's](https://www.rit.edu/directory/axkeec-andres-kwasinski) unpublished notes data communications
