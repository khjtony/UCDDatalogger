import serial

for port in range(1,10):
    try:
        s=serial.Serial(port,19200)
        s.close()
    except:
        continue
