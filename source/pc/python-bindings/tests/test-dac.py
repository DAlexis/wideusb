#!/usr/bin/env python3

import pywideusb as wu
import numpy as np
import time

runner = wu.IOServiceRunner()
serial = wu.PhysicalLayerSerialPort(runner.io_service(), "/dev/ttyACM0", 921600)

net_srv = wu.NetService(runner, serial)
discovery = wu.DeviceDiscovery(net_srv, 123)
discovery.run()
time.sleep(1)
print("Discovered devices on serial port: %s" % str(discovery.devices()))
addr = discovery.devices()[0]

dac = wu.DAC(net_srv, 123, addr)

x = np.linspace(0, 100, 10000)
y = list((np.sin(x) + 1.0) / 2.0)
v = list(np.linspace(0, 1, 1000))

dac.play_continious(prescaler=200, period=500, dma_chunk_size=100, data=y)
