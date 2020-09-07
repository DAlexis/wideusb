import serial
import json

ser = serial.Serial('/dev/ttyACM0', baudrate=921600, timeout=0.0001)

nope = {
    "module": "core",
    "msg_id": "my_message",
    "action": "nope",
}

status = {
    "module": "core",
    "msg_id": "status_request",
    "action": "status",
}

bad_module = {
    "module": "baaad",
    "msg_id": "baaad_message",
    "action": "nope",
}

test_message = json.dumps(status)

ser.write(test_message.encode("utf-8"))
while True:
    text = ser.readline().decode("utf-8")
    if text != "":
        print(text)


