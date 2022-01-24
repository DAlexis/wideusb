#!/usr/bin/env python3

import pywideusb as wu
import time

runner = wu.IOServiceRunner()
serial = wu.PhysicalLayerSerialPort(runner.io_service(), "/dev/ttyACM0", 921600)
net_srv = wu.NetService(runner, serial)
discovery = wu.DeviceDiscovery(net_srv, 123)
discovery.run()
time.sleep(1)
print("Discovered devices on serial port: %s" % str(discovery.devices()))
addr = discovery.devices()[0]

mon = wu.Monitor(net_srv, 123, addr)
mon.enable_stdout_print()
