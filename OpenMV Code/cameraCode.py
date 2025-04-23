import time
import sensor
import image
from pyb import UART

# Initialize camera
sensor.reset()
sensor.set_pixformat(sensor.GRAYSCALE)  # or sensor.RGB565
sensor.set_framesize(sensor.QVGA)       # 320x240 resolution
sensor.skip_frames(time=2000)           # Allow auto-adjustment

# Initialize UART (P4=TX, P5=RX)
uart = UART(3, 115200, timeout_char=1000)

while True:
    print("📷 Taking picture...")

    img = sensor.snapshot()  # Capture an image
    if img is None:
        print("❌ Error: Image capture failed!")
        continue  # Skip this loop iteration

    jpg = img.compress(quality=10).bytearray()
    img_size = len(jpg)
    print(f"📦 Image size: {img_size} bytes")

    if img_size == 0:
        print("❌ Error: Image compression failed!")
        continue

    # 🔹 Send Start Signal
    time.sleep(0.05)

    uart.write("IMG_START".encode())
    time.sleep(0.05)

    # 🔹 Send Image Size
    uart.write(img_size.to_bytes(4, 'little'))
    time.sleep(0.05)

    # 🔹 Send Image in Chunks
    chunk_size = 1024
    for i in range(0, img_size, chunk_size):
        uart.write(jpg[i:i+chunk_size])
        time.sleep(0.05)

    # 🔹 Send End Signal
    time.sleep(0.1)  # longer delay to ensure all data is sent out
    uart.write("IMG_END".encode())
    time.sleep(0.1)
    print("✅ Image sent successfully.")

    # 🔹 Check for Response from ESP32
    if uart.any():
        response = uart.read()
        print("🔁 Response from ESP32:", response)

    time.sleep(15)  # Wait before taking the next image
