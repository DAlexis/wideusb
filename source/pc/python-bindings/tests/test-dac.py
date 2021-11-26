#!/usr/bin/env python3

import pywideusb as wu
import numpy as np
import time

runner = wu.IOServiceRunner()
serial = wu.PhysicalLayerSerialPort(runner.io_service(), "/dev/ttyACM0", 921600)

net_srv = wu.NetService(serial)
discovery = wu.DeviceDiscovery(net_srv, 123)
discovery.run()
time.sleep(1)
print("Discovered devices on serial port: %s" % str(discovery.devices()))
addr = discovery.devices()[0]

dac = wu.DAC(net_srv, 123, addr)

x = np.linspace(0, 100, 10000)

y = list((np.sin(x) + 1.0) * 10.0)

v = list(np.linspace(0, 1, 10))

dac.play_continious(buffer_size=1000, prescaler=200, period=500, dma_chunk_size=100, data=x)


#!/usr/bin/env python3

import numpy as np
import pywideusb as wu

dev = wu.Device()

dac = wu.DAC(dev)

dac.init_sample(buffer_size=50, prescaler=20, period=500, repeat=True)

x = list(np.linspace(0, 1, 10))

dac.send_data(x)

dac.run()
