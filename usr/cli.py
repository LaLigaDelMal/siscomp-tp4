# -*- coding: utf-8 -*-

import os 
import matplotlib.pyplot as plt
import matplotlib.animation as anim
import time
import numpy as np



DEVICE_FILE = "/dev/gpio_prxl"
sensorName = ""
yName = ""
fig = plt.figure()
ax1 = fig.add_subplot(1,1,1)
signal = "0"
def prompt():
    print("SENSORES:")
    print("[1] Luz")
    print("[2] Proximidad")


def animationFrame(i):

    time.sleep(0.2)

    fd = open(DEVICE_FILE, "rb")
    r = ord(fd.read(1))
    if(signal=="2"):
        r=r * 4096 * 0.000017
    
    fd.close()

    ax1.clear()
    ax1.bar([sensorName], r, align='center', alpha=0.5)
    ax1.set_ylim([0, 20])
    ax1.set_ylabel("Señal")
    ax1.set_xlabel("Tiempo")


while(1):

    prompt()
    signal = input("Ingrese el numero de la seÃ±al a graficar\n")
    fd = open(DEVICE_FILE, "w")
    fd.write(signal)
    fd.close()

    animation = anim.FuncAnimation(fig, func=animationFrame, frames=100, interval=20)
    plt.show()

