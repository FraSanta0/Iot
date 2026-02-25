import paho.mqtt.client as mqtt
import serial
import threading
SERIAL_PORT = '/dev/ttyUSB0' # or 'COM3' on Windows, '/dev/tty.usbmodemXXXX' on macOS
BAUD_RATE = 9600
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
BROKER = 'localhost'
TOPIC_SUB = 'plant/irrigation'
TOPIC_PUB = 'plant/sensor'
def on_connect(client, userdata, flags, rc):
    print("Connected with result code", rc)
    client.subscribe(TOPIC_SUB)
def on_message(client, userdata, msg):
    print(f"Message received on {msg.topic}: {msg.payload.decode()}")
    ser.write((msg.payload.decode() + '\n').encode())
def serial_reader():
    while True:
        if ser.in_waiting:
            line = ser.readline().decode().strip()
            if line:
                print("From Arduino:", line)
                client.publish(TOPIC_PUB, line)
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(BROKER, 1883, 60)
t = threading.Thread(target=serial_reader, daemon=True)
t.start()
print("MQTT Bridge running...")
client.loop_forever()