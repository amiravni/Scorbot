import serial
import sys, traceback
import struct
import threading
import binascii
import time

COM_BAUDRATE = 9600
PLATFORM_SPEED = 80
CONFIDENCE_FACTOR = 1
class platformCommunicationHandler:

    _ser_con = None
    serial_read_buffer = []
    serial_read_thread = None
    serial_buffer = ""
    
    def __init__(self,comPort):
        try:
            self._ser_con = serial.Serial(comPort,COM_BAUDRATE)
            self._ser_con.timeout = 0.05
            #self.serial_read_thread = threading.Thread(target=self.read_from_serial)
            #self.serial_read_thread.start()
            
        except:
            print 'Couldn\'t use %s port, Try using another one' % comPort
            traceback.print_exc(file=sys.stdout)
            
    def send_data(self,data_to_send):
        if self._ser_con is not None:
            for conf_idx in range(0,CONFIDENCE_FACTOR):
                self._ser_con.write(data_to_send)
         #       print "*"*10,binascii.hexlify(data_to_send),"*"*10
                time.sleep(0.01)

    def give_commands(self,encoder_abs_pos_array):
        if self._ser_con is not None:
            self.send_data(struct.pack('<'+'B'*3+'h'*6,255,254,100,encoder_abs_pos_array[0],encoder_abs_pos_array[1],encoder_abs_pos_array[2],encoder_abs_pos_array[3],encoder_abs_pos_array[4],encoder_abs_pos_array[5]))

    def give_commands_go_home(self,encoder_init_positions_array):
        if self._ser_con is not None:
            self.send_data(struct.pack('<'+'B'*3+'h'*6,255,254,200,encoder_init_positions_array[0],encoder_init_positions_array[1],encoder_init_positions_array[2],encoder_init_positions_array[3],encoder_init_positions_array[4],encoder_init_positions_array[5]))
            
    def give_commands_poly(self,encoder_poly_array):
        if self._ser_con is not None:
            sendVector = struct.pack('<'+'B'*3+'h'*37,255,254,150,encoder_poly_array[0],encoder_poly_array[1],encoder_poly_array[2],encoder_poly_array[3],encoder_poly_array[4],encoder_poly_array[5],encoder_poly_array[6],encoder_poly_array[7],encoder_poly_array[8],encoder_poly_array[9],encoder_poly_array[10],encoder_poly_array[11],encoder_poly_array[12],encoder_poly_array[13],encoder_poly_array[14],encoder_poly_array[15],encoder_poly_array[16],encoder_poly_array[17],encoder_poly_array[18],encoder_poly_array[19],encoder_poly_array[20],encoder_poly_array[21],encoder_poly_array[22],encoder_poly_array[23],encoder_poly_array[24],encoder_poly_array[25],encoder_poly_array[26],encoder_poly_array[27],encoder_poly_array[28],encoder_poly_array[29],encoder_poly_array[30],encoder_poly_array[31],encoder_poly_array[32],encoder_poly_array[33],encoder_poly_array[34],encoder_poly_array[35],encoder_poly_array[36])
           # for sendIdx in range(0,37):
            #S   sendVector += struct.pack('<'+'h',encoder_poly_array[sendIdx])
            self.send_data(sendVector)
        
    def close(self):
        if self._ser_con is not None:
            self._ser_con.close()
            self._ser_con = None
    
    def read_from_serial_thread(self):
        if self._ser_con is not None:
            while True:
                ret = self._ser_con.read(32*1024)
                if len(ret) > 0:
                    self.serial_read_buffer.append(ret)
                    print ret

    def read_from_serial_blocking(self,data_to_read):
        if self._ser_con is not None:
            while True:
                ret = self._ser_con.read(data_to_read)
                if len(ret) > 0:
                    self.serial_buffer  += ret
                    print self.serial_buffer
                    if 'Srl' in self.serial_buffer:
                        self.serial_buffer = ""
                        break

"""        
    def turn_leds_on(self):
        if self._ser_con is not None:
            self._ser_con.write(struct.pack('B'*3+'h'*6,255,254,100,1,6,150,150,0))
            self._ser_con.write(struct.pack('B'*3+'h'*6,255,254,100,1,6,150,150,0))
            self._ser_con.write(struct.pack('B'*3+'h'*6,255,254,100,1,6,150,150,0))

    def move_forward(self):
        if self._ser_con is not None:
            self._ser_con.write(struct.pack('B'*3+'h'*6,255,254,100,1,1,PLATFORM_SPEED,PLATFORM_SPEED,0))
            self._ser_con.write(struct.pack('B'*3+'h'*6,255,254,100,1,1,PLATFORM_SPEED,PLATFORM_SPEED,0))
            self._ser_con.write(struct.pack('B'*3+'h'*6,255,254,100,1,1,PLATFORM_SPEED,PLATFORM_SPEED,0))
"""