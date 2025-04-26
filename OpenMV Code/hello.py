# Untitled - By: dimk2 - Thu Apr 24 2025

import sensor
import time
import image
import math
import ml
# === Initialize camera ===
sensor.reset()
sensor.set_pixformat(sensor.GRAYSCALE)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time=2000)
clock = time.clock()

while True:
    clock.tick()
    img = sensor.snapshot()
    print(clock.fps())
