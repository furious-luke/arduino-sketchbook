import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
# from dateutil import parser
# import datetime
import numpy as np
# from scipy import interpolate
import numpy.polynomial.polynomial as poly

x = []
y = []
with open('temp.log', 'r') as inf:
    time = 0.0
    for line in inf:
        time += 2.0
        try:
            temp = float(line.strip())
            x.append(time)
            y.append(temp)
        except:
            pass

# f = np.poly1d(poly.polyfit(x, y, 5))
# f = interpolate.UnivariateSpline(x, y)
# g = interpolate.interp1d(x, y)
# xn = np.arange(x[0], x[-1], datetime.timedelta(seconds=0.3))
xn = np.arange(x[0], x[-1], 0.3)

plt.figure(figsize=(12.8,12.8), dpi=80)
plt.plot(x, y)
plt.title('Room Temperatures')
plt.xlabel('Time')
plt.ylabel('Temperature (C)')
plt.savefig('temp.png')
