import serial
import json

ser = serial.Serial('/dev/ttyACM0', baudrate=921600, timeout=0.0001)

status_request = {
    "module": "core",
    "msg_id": "my_message",
    "action": "status_request",
}

test_message = json.dumps(status_request)

ser.write(test_message.encode("utf-8"))
while True:
    text = ser.readline().decode("utf-8")
    if text != "":
        print(text)


