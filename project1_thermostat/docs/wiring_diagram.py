# -*- coding: utf-8 -*-
"""
生成 实战项目1 桌面温控时钟 接线图 (SVG + PNG)

引脚以代码为准：
  PA0  DHT11 DATA        (Hardware/Wire-1.c)
  PB8/PB9  OLED SCL/SDA  (Hardware/OLED.c, 软 I2C, 地址 0x78)
  PA4..PA7 W25Q64 CS/CLK/MISO/MOSI (Hardware/MySPI.c, SPI1)
  PA1  TIM2_CH2 PWM 风扇 (Hardware/PWM.c, PSC=719 ARR=99 -> 1kHz)
  PB12/PB15 按键 K1/K2   (Hardware/Key.c, 下拉输入, 按下为高)

改图只改本文件，然后在 docs/ 目录下运行 `py wiring_diagram.py`，会覆盖同目录的 wiring.svg / wiring.png。
"""
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib import font_manager
from matplotlib.patches import FancyBboxPatch, Circle
import os

for name in ("Microsoft YaHei", "SimHei", "DengXian"):
    try:
        font_manager.findfont(name, fallback_to_default=False)
        plt.rcParams["font.sans-serif"] = [name]
        break
    except Exception:
        continue
plt.rcParams["axes.unicode_minus"] = False
plt.rcParams["svg.fonttype"] = "path"

C_SIG = "#2b6cb0"
C_PWR = "#c53030"
C_3V3 = "#dd6b20"
C_GND = "#1a202c"
C_KEY = "#6b46c1"
C_MOT = "#2f855a"
C_BOX = "#edf2f7"
C_EDGE = "#2d3748"
C_TXT = "#1a202c"
C_GRAY = "#718096"

W, H = 1700, 1150
fig = plt.figure(figsize=(17, 11.5), dpi=100)
ax = fig.add_axes([0, 0, 1, 1])
ax.set_xlim(0, W)
ax.set_ylim(H, 0)
ax.axis("off")
fig.patch.set_facecolor("white")


def box(x0, y0, x1, y1, title, sub=None, fc=C_BOX):
    ax.add_patch(FancyBboxPatch((x0, y0), x1 - x0, y1 - y0,
                                boxstyle="round,pad=0,rounding_size=12",
                                linewidth=1.6, edgecolor=C_EDGE, facecolor=fc, zorder=2))
    ax.text((x0 + x1) / 2, y0 + 26, title, ha="center", va="center",
            fontsize=11.5, weight="bold", color=C_TXT, zorder=3)
    if sub:
        ax.text((x0 + x1) / 2, y0 + 48, sub, ha="center", va="center",
                fontsize=8.5, color=C_GRAY, zorder=3)


def pin(x, y, label, side):
    ax.add_patch(Circle((x, y), 4.5, facecolor="white", edgecolor=C_EDGE,
                        linewidth=1.3, zorder=4))
    if side == "right":
        ax.text(x - 10, y, label, ha="right", va="center", fontsize=9, color=C_TXT, zorder=4)
    elif side == "left":
        ax.text(x + 10, y, label, ha="left", va="center", fontsize=9, color=C_TXT, zorder=4)


def wire(pts, color=C_SIG, lw=2.2):
    ax.plot([p[0] for p in pts], [p[1] for p in pts], color=color, linewidth=lw,
            solid_capstyle="round", solid_joinstyle="round", zorder=1)


def pwr_tag(x, y, label, color, up=False, length=24):
    d = -1 if up else 1
    ax.plot([x, x], [y, y + d * length], color=color, lw=1.8, zorder=1)
    ax.plot([x - 9, x + 9], [y + d * length, y + d * length], color=color, lw=2.0, zorder=3)
    ax.text(x, y + d * (length + 13), label, ha="center", va="center",
            fontsize=8.5, weight="bold", color=color, zorder=3)


def gnd_tag(x, y, up=False, length=13):
    d = -1 if up else 1
    ax.plot([x, x], [y, y + d * length], color=C_GND, lw=1.8, zorder=1)
    for i, w in enumerate((18, 11, 5)):
        yy = y + d * (length + i * 5)
        ax.plot([x - w / 2, x + w / 2], [yy, yy], color=C_GND, lw=1.8, zorder=3)
    ax.text(x, y + d * (length + 3 * 5 + 13), "GND", ha="center", va="center",
            fontsize=8, color=C_GND, zorder=3)


# ================= 标题 =================
ax.text(W / 2, 50, "实战项目1 · 桌面温控时钟  接线图", ha="center", va="center",
        fontsize=18, weight="bold", color=C_TXT)
ax.text(W / 2, 82, "STM32F103C8T6 + DHT11 + 0.96″ OLED(I²C) + W25Q64(SPI) + TB6612 驱动风扇",
        ha="center", va="center", fontsize=10, color=C_GRAY)

# ================= MCU =================
MX0, MY0, MX1, MY1 = 640, 195, 960, 860
ax.add_patch(FancyBboxPatch((MX0, MY0), MX1 - MX0, MY1 - MY0,
                            boxstyle="round,pad=0,rounding_size=14",
                            linewidth=2.0, edgecolor="#2c5282", facecolor="#e6effa", zorder=2))
ax.text((MX0 + MX1) / 2, 232, "STM32F103C8T6", ha="center", va="center",
        fontsize=12, weight="bold", color="#1a365d", zorder=3)
ax.text((MX0 + MX1) / 2, 254, "Blue Pill 最小系统板 · 72MHz", ha="center", va="center",
        fontsize=8.5, color="#2c5282", zorder=3)

LEFT_PINS = [("PB8", 270), ("PB9", 320), ("PA0", 460), ("PB12", 700), ("PB15", 770)]
RIGHT_PINS = [("PA4", 290), ("PA5", 350), ("PA6", 410), ("PA7", 470), ("PA1", 580)]

for name, y in LEFT_PINS:
    ax.plot([MX0 - 12, MX0], [y, y], color=C_EDGE, lw=1.2, zorder=3)
    ax.text(MX0 + 12, y, name, ha="left", va="center", fontsize=9.5, weight="bold", color=C_TXT, zorder=3)
for name, y in RIGHT_PINS:
    ax.plot([MX1, MX1 + 12], [y, y], color=C_EDGE, lw=1.2, zorder=3)
    ax.text(MX1 - 12, y, name, ha="right", va="center", fontsize=9.5, weight="bold", color=C_TXT, zorder=3)

# ================= 左列 =================
box(60, 200, 520, 380, "0.96″ OLED (SSD1306)", "软 I2C · 地址 0x78", fc="#f0fff4")
pin(520, 270, "SCL", "right")
pin(520, 320, "SDA", "right")
pin(180, 380, "", "left")
pin(340, 380, "", "left")

box(60, 440, 520, 610, "DHT11 温湿度", "单总线 · 数据线上拉 4.7k~10k", fc="#fffaf0")
pin(520, 460, "DATA", "right")
pin(180, 610, "", "left")
pin(340, 610, "", "left")

box(60, 665, 300, 735, "K1 下一页", fc="#faf5ff")
pin(300, 700, "OUT", "right")
box(60, 735, 300, 805, "K2 上一页", fc="#faf5ff")
pin(300, 770, "OUT", "right")
ax.text(60, 828, "按键另一端接 3.3V（下拉输入，按下为高电平）", ha="left", va="center",
        fontsize=8.2, color=C_GRAY)

# ================= 右列 =================
box(1100, 240, 1560, 510, "W25Q64 SPI Flash", "8MB · 只用扇区 0 前 4KB", fc="#faf5ff")
pin(1100, 290, "CS", "left")
pin(1100, 350, "CLK", "left")
pin(1100, 410, "MISO", "left")
pin(1100, 470, "MOSI", "left")

box(1100, 540, 1420, 800, "TB6612FNG", "电机驱动 · 单通道驱动风扇", fc="#fff5f5")
pin(1100, 580, "PWMA", "left")
pin(1100, 620, "STBY", "left")
pin(1100, 660, "AIN1", "left")
pin(1100, 700, "AIN2", "left")
pin(1420, 640, "AO1", "right")
pin(1420, 700, "AO2", "right")

fan = Circle((1540, 670), 62, facecolor="#f7fafc", edgecolor=C_EDGE, linewidth=1.6, zorder=2)
ax.add_patch(fan)
ax.plot([1540, 1540], [622, 718], color=C_EDGE, lw=1.2, zorder=3)
ax.plot([1500, 1580], [670, 670], color=C_EDGE, lw=1.2, zorder=3)
ax.text(1540, 748, "风扇 / 电机", ha="center", va="top", fontsize=9.5, color=C_TXT)
ax.text(1540, 764, "5V 单独供电", ha="center", va="top", fontsize=8, color=C_GRAY)

# ================= 信号连线 =================
wire([(520, 270), (640, 270)])                                # SCL  -> PB8
wire([(520, 320), (640, 320)])                                # SDA  -> PB9
wire([(520, 460), (640, 460)])                                # DATA -> PA0
wire([(300, 700), (640, 700)], C_KEY)                         # K1 -> PB12
wire([(300, 770), (640, 770)], C_KEY)                         # K2 -> PB15
wire([(960, 290), (1100, 290)])                               # PA4 -> CS
wire([(960, 350), (1100, 350)])                               # PA5 -> CLK
wire([(960, 410), (1100, 410)])                               # PA6 -> MISO
wire([(960, 470), (1100, 470)])                               # PA7 -> MOSI
wire([(960, 580), (1100, 580)])                               # PA1 -> PWMA
wire([(1420, 640), (1478, 640)], C_MOT)                       # AO1 -> 风扇
wire([(1420, 700), (1478, 700)], C_MOT)                       # AO2 -> 风扇

# STBY / AIN1 / AIN2：代码不控制方向，固定电平
for y, txt, c in [(620, "3.3V", C_3V3), (660, "GND", C_GND), (700, "3.3V", C_3V3)]:
    ax.plot([1100, 1058], [y, y], color=c, lw=1.8, zorder=1)
    ax.text(1050, y, txt, ha="right", va="center", fontsize=8.5, color=c)

# ================= 电源符号 =================
pwr_tag(180, 380, "VCC 3.3V", C_3V3)
gnd_tag(340, 380)
pwr_tag(180, 610, "VCC 3.3V", C_3V3)
gnd_tag(340, 610)
pwr_tag(1180, 240, "3.3V", C_3V3, up=True)
gnd_tag(1400, 240, up=True)
pwr_tag(1180, 800, "VM 5V", C_PWR)
pwr_tag(1260, 800, "VCC 3.3V", C_3V3)
gnd_tag(1340, 800)
pwr_tag(700, 860, "5V", C_PWR)
pwr_tag(750, 860, "3.3V", C_3V3)
gnd_tag(800, 860)

# ================= 底部注释 =================
notes = [
    "注：① 本图按 User/main.c 与 Hardware/ 下驱动的实际引脚绘制，实物以万用表实测为准。",
    "　　② TB6612 的 STBY 接 3.3V、AIN1 接 GND、AIN2 接 3.3V（固定正转，只用 PWM 调速），B 通道未用。",
    "　　③ 风扇与 TB6612 输出端务必并续流二极管（1N4148 / 1N5819）；5V 来自 USB，3.3V 来自板载稳压。",
]
for i, t in enumerate(notes):
    ax.text(60, 1040 + i * 26, t, ha="left", va="center", fontsize=8.2, color=C_GRAY)

# ================= 导出 =================
# 直接在本目录运行：py wiring_diagram.py，会覆盖同目录的 wiring.svg / wiring.png
out_dir = os.path.dirname(os.path.abspath(__file__))
os.makedirs(out_dir, exist_ok=True)
svg = os.path.join(out_dir, "wiring.svg")
png = os.path.join(out_dir, "wiring.png")
fig.savefig(svg, format="svg", facecolor="white", bbox_inches="tight", pad_inches=0.15)
fig.savefig(png, format="png", dpi=160, facecolor="white", bbox_inches="tight", pad_inches=0.15)
print(svg)
print(png)
