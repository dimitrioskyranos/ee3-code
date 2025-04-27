import time
import sensor
import image
import math
import ml
from ml.utils import NMS
from pyb import UART

# === Initialize camera ===
sensor.reset()
sensor.set_pixformat(sensor.GRAYSCALE)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time=2000)

# === Initialize UART (P4=TX, P5=RX) ===
uart = UART(3, 921600, timeout_char=1000)

# === Initialize FOMO model ===
min_confidence = 0.4
threshold_list = [(math.ceil(min_confidence * 255), 255)]
model = ml.Model("trained")
print(model)

# Color list for up to 7 classes
colors = [
    (255, 0, 0),
    (0, 255, 0),
    (255, 255, 0),
    (0, 0, 255),
    (255, 0, 255),
    (0, 255, 255),
    (255, 255, 255),
]

# === FOMO post-processing ===
def fomo_post_process(model, inputs, outputs):
    n, oh, ow, oc = model.output_shape[0]
    nms = NMS(ow, oh, inputs[0].roi)
    for i in range(oc):
        img = image.Image(outputs[0][0, :, :, i] * 255)
        blobs = img.find_blobs(
            threshold_list, x_stride=1, area_threshold=1, pixels_threshold=1
        )
        for b in blobs:
            rect = b.rect()
            x, y, w, h = rect
            score = (
                img.get_statistics(thresholds=threshold_list, roi=rect).l_mean() / 255.0
            )
            nms.add_bounding_box(x, y, x + w, y + h, score, i)
    return nms.get_bounding_boxes()

# === Main loop ===
while True:
    print("📷 Taking picture...")

    img = sensor.snapshot()
    if img is None:
        print("❌ Error: Image capture failed!")
        continue

    # === Run detection and draw if objects found ===
    for i, detection_list in enumerate(model.predict([img], callback=fomo_post_process)):
        if i == 0:
            continue  # Skip background class
        if len(detection_list) == 0:
            continue

        print("********** %s **********" % model.labels[i])
        for (x, y, w, h), score in detection_list:
            center_x = math.floor(x + (w / 2))
            center_y = math.floor(y + (h / 2))
            print(f"x {center_x}\ty {center_y}\tscore {score}")
            img.draw_circle((center_x, center_y, 12), color=colors[i])  # Draw detection

    # === Compress and send image to ESP32 ===
    jpg = img.compress(quality=10).bytearray()
    img_size = len(jpg)
    print(f"📦 Image size: {img_size} bytes")

    if img_size == 0:
        print("❌ Error: Image compression failed!")
        continue

    # 🔹 Send start signal
    time.sleep_ms(1)
    uart.write("IMG_START".encode())
    time.sleep_ms(1)

    # 🔹 Send image size
    uart.write(img_size.to_bytes(4, 'little'))
    time.sleep_ms(1)

    # 🔹 Send image data in chunks
    chunk_size = 1024
    for i in range(0, img_size, chunk_size):
        uart.write(jpg[i:i+chunk_size])
        time.sleep_ms(1)

    # 🔹 Send end signal
    time.sleep_ms(1)
    uart.write("IMG_END".encode())
    time.sleep_ms(1)
    print("✅ Image sent successfully.")

    # 🔁 Check response from ESP32 (optional)
    if uart.any():
        response = uart.read()
        print("🔁 Response from ESP32:", response)

    # time.sleep(15)  # Wait 15 seconds before next capture
