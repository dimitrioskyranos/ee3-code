import time
import sensor
import image
from pyb import UART

sensor.reset()
sensor.set_pixformat(sensor.GRAYSCALE)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time=2000)

uart = UART(3, 250000, timeout_char=1000)

while True:
    print("📷 Taking picture...")
    img = sensor.snapshot()

    if img is None:
        print("❌ Error: Image capture failed!")
        continue

    jpg = img.compress(quality=10).bytearray()
    img_size = len(jpg)
    print(f"📦 Image size: {img_size} bytes")

    if img_size == 0:
        print("❌ Error: Image compression failed!")
        continue

    uart.write("IMG_START".encode())
    time.sleep(0.005)

    uart.write(img_size.to_bytes(4, 'little'))
    time.sleep(0.005)

    chunk_size = 512
    for i in range(0, img_size, chunk_size):
        uart.write(jpg[i:i+chunk_size])
        time.sleep(0.002)

    time.sleep(0.01)
    uart.write("IMG_END".encode())
    time.sleep(0.01)

    print("✅ Image sent successfully.")

    if uart.any():
        response = uart.read()
        print("🔁 Response from ESP32:", response)

    time.sleep(0.05)  # Wait before taking the next picture
