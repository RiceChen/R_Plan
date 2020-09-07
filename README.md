R_Plan

[中文页](README.md) |

There is a wide range of embedded development tools for developers, and whenever we start developing, we need to open every tool one by one. For example, serial port, network debugging assistant, etc. Sometimes when multiple tools are opened, we still will waste time switching from different tools. 

To solve this problem, I started writing an Integrated Software Toolkit on Host Computer to integrate all the host computer software I used, so the R-Plan is built, which is the abbreviation for Rice Plan. Each function is like a grain of rice, gradually stacked into a bowl of rice.

## 

- [R plan tool](https://github.com/RiceChen/R_Plan/tree/master/R_plan_tool)

| Item             | Description                                                  |
| ---------------- | ------------------------------------------------------------ |
| 《cmd console》  | Integrated windows cmd function, to synchronize the command line function of windows. |
| 《rtt ota pack》 | Integrated RT-Thread packaging tool for easy packaging of rbl when developing RT-Thread. |
| 《find file》    | Integrated find files tool so that find files could be as quickly as Linux's grep and find commands. |

## R plan Little Box

- [R plan Little Box](https://github.com/RiceChen/R_Plan/tree/master/R_plan_LB)
- Little Box is a small, functional desktop box. The features are shown as follows:

| Module        | Feature                                                      |
| ------------- | ------------------------------------------------------------ |
| BK7252U WIFI  | As the main controller (CPU frequency 180MHZ) and provides WIFI hardware support. |
| BLE           | Assist to distribute the network.                   |
| Camera OV7670 | Image recognition, take pictures.             |
| LCD           | Picture display, time display, ambient temperature and humidity display. |
| SHT3X         | Obtain ambient temperature and humidity. |
| MIC           | Recording, speech recognition control, sound distribution network. |
| Speaker       | Record playback, music playback. |
| SD card       | Save pictures, store music, store data. |

