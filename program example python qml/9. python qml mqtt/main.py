######  PROGRAM MEMANGGIL WINDOWS PYQT5 ##########################

####### memanggil library PyQt5 ##################################
#----------------------------------------------------------------#
from PyQt5.QtCore import * 
from PyQt5.QtGui import * 
from PyQt5.QtQml import * 
from PyQt5.QtWidgets import *
from PyQt5.QtQuick import *  
import sys

import paho.mqtt.client as paho

#broker="127.0.0.1"    #local broker
#broker="broker.emqx.io"   #online broker
broker = "mqtt.ardumeka.com" #broker ardumeka
port = 11219 #1883
topic_test = ""
#----------------------------------------------------------------#


##################################################################
#----------------deklarasi variabel------------------------------#
analog = 0
input1_val = ""
input2_val = ""
input1_color = "#df1c39"
input2_color = "#df1c39"

button1 = ""
button2 = ""
button3 = ""

analog_out = ""
########## mengisi class table dengan instruksi pyqt5#############
#----------------------------------------------------------------#
class table(QObject):    
    def __init__(self, parent = None):
        super().__init__(parent)
        self.app = QApplication(sys.argv)
        self.engine = QQmlApplicationEngine(self)
        self.engine.rootContext().setContextProperty("backend", self)    
        self.engine.load(QUrl("main.qml"))
        sys.exit(self.app.exec_())
    
    #####################TOMBOL QML KE PYTHON###################
    @pyqtSlot(str)
    def button1(self, message):
        global button1
        button1 = message
        print(button1)
        client.publish("button1_send",str(button1))
        
    @pyqtSlot(str)
    def button2(self, message):
        global button2
        button2 = message
        print(button2)
        client.publish("button2_send",str(button2))
        
    @pyqtSlot(str)
    def button3(self, message):
        global button3
        button3 = message
        print(button3)
        client.publish("button3_send",str(button3))
        
    #####################SLIDER QML KE PYTHON###################
    @pyqtSlot(str)
    def analog_output(self, message):
        global analog_output
        analog_output = message
        print(analog_output)
        client.publish("analog_send",str(analog_output))
    
    ######################KIRIM DATA ANALOG KE GAUGE##############
    @pyqtSlot(result=float)
    def get_analog(self):  return analog
    
    ####################KIRIM DATA WARNA STATUS BUTTON#############
    @pyqtSlot(result=str)
    def get_input1_color(self):  return input1_color
    
    @pyqtSlot(result=str)
    def get_input2_color(self):  return input2_color


#----------------------------------------------------------------#
def on_message(client, userdata, message):
    msg = str(message.payload.decode("utf-8"))
    t = str(message.topic)

    if(msg[0] == 'c'):
        val =  1
    else:
        val = (msg)
    
    if (t == "sensor1"):
        global input1_val
        global input1_color
        input1_val = (msg)
        
        if (input1_val == "on"):
            input1_color = "#04f8fa"
        if (input1_val == "off"):
            input1_color = "#df1c39"
        print(input1_val)
        
        
    if (t == "sensor2"):
        global input2_val
        global input2_color
        input2_val = (msg)
        
        if (input2_val == "on"):
            input2_color = "#04f8fa"
        if (input2_val == "off"):
            input2_color = "#df1c39"
        print(input2_val)
        
    if (t == "potensiometer"):
        global analog
        analog = float(msg)
        




########## memanggil class table di mainloop######################
#----------------------------------------------------------------#    
if __name__ == "__main__":
    client= paho.Client("GUI")
    client.on_message=on_message

    print("connecting to broker ",broker)
    client.connect(broker,port)#connect
    print(broker," connected")
    
    client.loop_start()
    print("Subscribing")

    client.subscribe("sensor1")
    client.subscribe("sensor2")
    client.subscribe("potensiometer")
    
    main = table()
    
    
#----------------------------------------------------------------#