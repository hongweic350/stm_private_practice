# STM32 实战练习合集

STM32F103 系列练习项目，每个子目录是一个独立工程（Keil MDK + 标准外设库）。

## 项目列表

| 目录 | 项目 | 用到的外设 / 技术点 |
| --- | --- | --- |
| [project1_thermostat](project1_thermostat/) | 桌面温控时钟 | DHT11 单总线、0.96" OLED 软 I2C、W25Q64 SPI Flash、RTC、PWM 风扇、按键翻页 |
| _（下一个）_ | 待添加 | — |

## 目录约定

- 每个项目独占一个子目录，命名 `projectN_英文短名`（避免中文路径，Keil 老版本对中文路径不友好）；
- 每个子目录带自己的 `README.md`，GitHub 进入该目录时会自动渲染；
- 编译产物（`Objects/`、`Listings/`、`*.axf`、`*.hex` 等）由根目录 `.gitignore` 统一忽略，不用逐个项目配；
- 打开某个项目：进入对应目录，取 `Project.uvprojx` 用 Keil 打开即可。

## 添加新项目

```bash
git clone https://github.com/hongweic350/stm_private_practice.git
cd stm_private_practice
cp -r /path/to/新工程 projectN_xxx      # 先清掉 Objects/、Listings/ 等编译产物
git add projectN_xxx
git commit -m "add projectN: xxx"
git push
```
