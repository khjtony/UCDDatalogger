from __future__ import print_function
import serial
import time
port=4
baud=19200
def printf(str, *args):
    print(str%args,end="")

def s_connect(port=4,baud=19200):
    gate=serial.Serial(port,baud,timeout=1)
    time.sleep(2)
    gate.write("H")
    line=gate.readline()
    if line[:1]=="H":
        print("Connecting established...")
        return gate
    else:
        return 0

def printTitle(option):
    if option=="intro":
        print("Welcome to use pressure collector control program")
    elif option=="option":
        print("Please select an option:")
        print("1.Check current setup")
        print("2.Setup time schedule")
        print("3.Live pressure monitor")
        print("4.Download data")
        print("5.Change SN")
        print("0.Exit")
    elif option=="err":
        print("Error input. Please select again!")
        printTitle("option")
    return

def op_check():
    gate=s_connect()
    while True:    #read HA1, which is S
        gate.write('A')
        thisSn=gate.readline()
        if thisSn[:1]=="A" and thisSn[-3]=="H":
            thisSn=thisSn[1:-3]
            print("SN series: %s."%thisSn)
            break
        else:
            continue
    while True:    #read HA2,which is time period setting
        gate.write('B')
        thisTime=gate.readline()
        if thisTime[:1]=="B" and thisTime[-3]=="H":
            thisTime=int(thisTime[1:-3])
            print("Collecting period is every %d hours."%thisTime)
            break
        else:
            continue
    
    
    gate.close()
    return 0
    
def op_time():
    gate=s_connect()
    
    time=int(raw_input("Please input a int time period: "))
    gate.write('C')
    time=str(time)
    for i in time:
        gate.write(i)
    gate.write('H')
    print(gate.readline())
    return 0
    
def op_monitor():
    gate=s_connect()
    gate.write('D')
    line=gate.readline()
    while (line!='H'):
        print(line)
        line=gate.readline()
    return 0
    


printTitle("intro")
printTitle("option")
while (True):
    try:
        option=int(raw_input("Selection: "))
        if (option==0):
            break
        elif (option==1):    #check current setup
            op_check()
        elif (option==2):    #setup time schedule
            op_time()
        elif (option==3):    #live monitor
            op_monitor()
      #  elif (option==4):    #Download data
        else:
            printTitle("err")
            continue
    except:
        print("Unexpected error happend, please select again")
        gate=s_connect()
        gate.close()
        printTitle("option")


    


