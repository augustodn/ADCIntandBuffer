import serial
import time

SERIAL_PORT = '/dev/ttyUSB0'
SPEED = 500000
TIMEOUT = 1
NUM = '1'

# Open Port
ser = serial.Serial(SERIAL_PORT, SPEED, timeout=TIMEOUT)

while (True):
    num = NUM
    # Send command to arduino
    print("Give me data ------->")
    ser.write(num.encode())
    inbuf = ser.inWaiting()
    if (inbuf):
        print("----------> Reading buffer")
        newData = ser.read(inbuf)

        for i in range(len(newData)):
            if newData[i] == 0x7c and i != (len(newData) - 1):
                print(newData[i+2], newData[i+1])


    time.sleep(1.5)
