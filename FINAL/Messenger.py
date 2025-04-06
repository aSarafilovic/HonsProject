import serial
import time

class Messenger():

    def setup(self):
        #check if there's a serial connection and if not the setup fails.
        try:
            self.ser = serial.Serial('/dev/ttyACM0', baudrate = 9600, timeout=1, write_timeout=1)
            self.ser.reset_input_buffer()
            self.commandLine = "07770\n"
            return True
        except:
            return False
        #while True:
        #    ser.write(b"World\n")
        #    time.sleep(1)
        #    line = ser.readline().decode('utf-8').rstrip()
        #    print(line)
        #    ser.write(b"Wide\n")
        #    time.sleep(1)
        #    line = ser.readline().decode('utf-8').rstrip()
        #    print(line)
        #    time.sleep(1)
   
    def motor_char(self,M):
        self.commandLine[0] = M

    def loc_chars(self,L):
        self.commandLine[1] = L[0]
        self.commandLine[2] = L[1]
        self.commandLine[3] = L[2]

    def action_char(A):
        self.commandLine[4] = A

    def send(self):
        selfser.write(self.commandLine.encode('utf-8'))