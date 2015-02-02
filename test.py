import serial
import time
def s_connect(port=2,baud=19200):
    gate=serial.Serial(port,baud,timeout=1)
    time.sleep(2)
    gate.write("H")
    line=gate.readline()
    if line[:1]=="H":
        print("Connecting established...")
        return gate
    else:
        return 0
        
gate=s_connect()
gate.write('C')
value=str(55)
for i in value:
    gate.write(i)
   # print(gate.readline())
gate.write('H')
for i in value:
    gate.write(i)
    #print(gate.readline())
line=gate.readline()
print line
gate.close()