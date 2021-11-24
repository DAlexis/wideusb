#!/usr/bin/env python3

import numpy as np
import pywideusb as wu

dev = wu.Device()

dac = wu.DAC(dev)

dac.init_sample(buffer_size=50, prescaler=20, period=500, repeat=True)

x = list(np.linspace(0, 1, 10))

dac.send_data(x)

dac.run()
