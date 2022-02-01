#!/usr/bin/env python3

import pywideusb as wu
import time

runner = wu.IOServiceRunner()
serial = wu.PhysicalLayerSerialPort(runner, "/dev/ttyACM0", 921600)

net_srv = wu.NetService(serial)
discovery = wu.DeviceDiscovery(net_srv, 123)
discovery.run()
time.sleep(1)
print("Discovered devices on sesrial port: %s" % str(discovery.devices()))
