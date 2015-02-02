from __future__ import print_function
import serial

def printf(str, *args):
    print(str%args,end="")

gate=0
def init():
    s_port=3
    s_baud=19200
    stargate=serial.Serial()
    print("Welcome to pressure collector setup program")
    stargate.port=s_port
    stargate.baudrate=s_baud
    try:
        stargate.open()
    except:
        stargate.port+=1
    
    
    print("connection established.")
    return stargate

    
    
stargate=init()
while True:
    printf(stargate.readline())


		
