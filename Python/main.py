import serial
import time
from tkinter import *
import struct
import functools
import operator
from scipy.spatial.distance import euclidean
from fastdtw import fastdtw
import matplotlib.pyplot as plt


class UartCommunication:
    def __init__(self):
        self.uartPort = serial.Serial()
        self.x = 0.0
        self.y = 0.0
        self.z = 0.0

    def openPort(self, portName, baudRate):
        if self.uartPort.is_open == False:
            self.uartPort.port = portName
            self.uartPort.baudrate = baudRate
            try:
                self.uartPort.open()
            except:
                print("Failed to open port")

    def isPortOpen(self):
        if self.uartPort.is_open == False:
            return False
        else:
            return True

    def closePort(self):
        if self.uartPort.is_open == True:
            try:
                self.uartPort.close()
            except:
                print("Failed to close port")

    def readPort(self):
        #line = self.uartPort.readline() # unicode znaki np b'1'
        #tmp = str(line, 'utf-8') # conversion from byte to string
        #KL46
        if self.uartPort.is_open == True:
            tmpX = self.uartPort.read(4)
            tmpY = self.uartPort.read(4)
            tmpZ = self.uartPort.read(4)
            tmpX = struct.unpack('f', tmpX) #byte to tuple
            tmpY = struct.unpack('f', tmpY)
            tmpZ = struct.unpack('f', tmpZ)
            self.x = functools.reduce(operator.add, tmpX) #tuple to float
            self.y = functools.reduce(operator.add, tmpY)
            self.z = functools.reduce(operator.add, tmpZ)
        #try:
            #x, y, z = tmp.split() #jesli nie uda sie podzielic tmp to wykonuje sie except czyli nic
            #self.x = float(x)
            #self.y = float(y)
            #self.z = float(z)
        #except:
            #pass

    def getX(self):
        return self.x
    def getY(self):
        return self.y
    def getZ(self):
        return self.z


# obj UartCommunication
uart = UartCommunication()

def liveData():
    button1.configure(text="Zamknij wykres", command=closeAll)
    uart.openPort("COM7", 9600)  # nazwa portu, predkosc transmisji
    plt.style.use('seaborn-bright')
    fig = plt.figure(figsize = (9,7.5))
    ax = fig.add_subplot(211, projection='3d')  # 3d chart
    ax2 = fig.add_subplot(212)  # x, y, z depending on time
    ax2.set_ylim(-2, 2)
    ax2.set_xlabel("Time (s)")
    ax2.set_ylabel("Amplitudes x,y,z")
    xValues, yValues, zValues, timeValues = [], [], [], []
    toc = 0
    while uart.isPortOpen() == True:
        tic = time.time() # time axis tmp
        timeValues.append(toc)
        uart.readPort()
        x = uart.getX()
        xValues.append(x)
        y = uart.getY()
        yValues.append(y)
        z = uart.getZ()
        zValues.append(z)
    # x, y, z 3d chart
        ax.set_title("Real-Time data from accelerometer") #te parametry musza byc ustawiane w petli poniewaz cla() je kasuje
        ax.set_xlim(-2, 2)  # granice osi x
        ax.set_xlabel('X axis')  # nazwa osi x
        ax.set_ylim(-2, 2)
        ax.set_ylabel('Y axis')
        ax.set_zlim(-2, 2)
        ax.set_zlabel('Z axis')
        ax.scatter3D(x, y, z, c = 'r', marker = '^')
    ## x, y z depending on time chart
        ax2.set_xlim(left = max(0.01, toc-5), right = toc)
        ax2.plot(timeValues, xValues, 'b-',
                timeValues, yValues, 'g-',
                timeValues, zValues, 'r-',)
        plt.show(block=False) # block na false zeby dane wciaz byly wyswietlane na wykresie
        plt.pause(0.000001) #czas odswiezania wykresu
        ax.cla()    # clear axes
        toc += time.time()-tic

def saveGesture():
    fileName = ""

    def setFileName(text):
        global fileName # global variable
        fileName = text

    def saveGestureFile(fileN,txt):
        if(fileN != ""):
            with open("gestures/{}.log".format(fileN), 'w') as file:
                file = file.write(''.join(txt))

    def gesturesBase(event):
        import os
        x = os.listdir("gestures")
        print("Gesty zapisane w bazie :")
        if not x: # if list is empty
            print("Brak")
        else:
            for i, gesture in enumerate(x):
                gesture = gesture.replace(".log", "")
                i = i + 1
                print(str(i) + ". " +gesture)

    def gesturesDel(event):
        import os
        import glob
        for filename in glob.glob(os.path.join("gestures", '*.log')):
            os.remove(filename)
        print("Wszystkie gesty zostały usuniete z bazy")

    def start(event):
        global fileName
        if(fileName != ""):
            xValues, yValues, zValues, timeValues, dataList = [], [], [], [], []
            del xValues[:]
            del yValues[:]
            del zValues[:]
            del timeValues[:]
            ax.cla()
            ax.set_ylim(-2, 2)
            ax.set_xlabel("Time (s)")
            ax.set_ylabel("Amplitudes x,y,z")
            ax.set_xlim(left=0, right=2)
            toc = 0
            while toc < 3 and uart.isPortOpen() == True:
                tic = time.time()  # time axis tmp
                timeValues.append(toc)
                uart.readPort()
                x = uart.getX()
                xValues.append(x)
                y = uart.getY()
                yValues.append(y)
                z = uart.getZ()
                zValues.append(z)
                data = str(x) + " " + str(y) + " " + str(z) + " " + str(toc) + "\n"
                dataList.append(data)
                ## x, y z depending on time chart
                ax.plot(timeValues, xValues, 'b-',
                        timeValues, yValues, 'g-',
                        timeValues, zValues, 'r-', )
                plt.show(block = False)
                plt.pause(0.000001)  # czas odswiezania wykresu
                toc += time.time() - tic
            saveGestureFile(fileName, dataList)
            print("Dodano nowy gest do bazy")

    from matplotlib.widgets import Button
    from matplotlib.widgets import TextBox
    button2.configure(text="Zamknij wykres", command=closeAll)
    plt.style.use('seaborn-bright')
    fig = plt.figure()
    ax = fig.add_subplot(111)
    ax.set_ylim(-2, 2)
    ax.set_xlabel("Time (s)")
    ax.set_ylabel("Amplitudes x,y,z")
    ax.set_xlim(left=0, right=2)
    uart.openPort("COM7", 9600)  # nazwa portu, predkosc transmisji
    axbox = plt.axes([0.20, 0.90, 0.25, 0.05])
    inputBox = TextBox(axbox, 'Nazwa gestu')
    inputBox.on_submit(setFileName)
    axButtonStart = plt.axes([0.45, 0.90, 0.1, 0.05]) #poczatek, wys, dl, szer
    buttonStart = Button(axButtonStart, "Zapisz")
    buttonStart.on_clicked(start)
    axButtonList = plt.axes([0.55, 0.90, 0.20, 0.05])
    buttonList = Button(axButtonList, "Zapisane gesty")
    buttonList.on_clicked(gesturesBase)
    axButtonDel = plt.axes([0.75, 0.90, 0.15, 0.05])
    buttonDel = Button(axButtonDel, "Usun gesty")
    buttonDel.on_clicked(gesturesDel)
    plt.show()

def recognizeGesture():
    from matplotlib.widgets import Button
    button3.configure(text="Zamknij wykres", command=closeAll)
    def saveTestingGesture(txt):
        with open("testingGesture/test.log", 'w') as file:
            file = file.write(''.join(txt))

    def dtwTest(setDistance): #set minimum recognition distance!
        xTList, yTList, zTList, tTList = [], [], [], []
        xGList, yGList, zGList, tGList = [], [], [], []
        def openTestingGesture():
            with open("testingGesture/test.log", 'r') as file:
                testGestureData = file.read()
                testGestureData = testGestureData.split("\n")
                for i in testGestureData:
                    try:
                        xT, yT, zT, tT = i.split()
                        xTList.append(xT)
                        yTList.append(yT)
                        zTList.append(zT)
                        tTList.append(tT)
                    except:
                        pass
        def openGestures():
            openTestingGesture()
            gesturePath = "Tego gestu nie ma w bazie.log"
            import os
            import glob
            for filename in glob.glob(os.path.join("gestures", '*.log')):
                del xGList[:]
                del yGList[:]
                del zGList[:]
                del tGList[:]
                with open(filename, 'r') as file:
                    gesturesData = file.read()
                    gesturesData = gesturesData.split("\n")
                    for i in gesturesData:
                        try:
                            xG, yG, zG, tG = i.split()
                            xGList.append(xG)
                            yGList.append(yG)
                            zGList.append(zG)
                            tGList.append(tG)
                        except:
                            pass
                    distanceX, pathX = fastdtw(xTList, xGList, dist = euclidean)
                    distanceY, pathY = fastdtw(yTList, yGList, dist = euclidean)
                    distanceZ, pathZ = fastdtw(zTList, zGList, dist = euclidean)
                    if distanceX < setDistance and distanceY < setDistance and distanceZ < setDistance:
                        gesturePath = filename
            return gesturePath
        return openGestures()

    def Start(event):
        xValues, yValues, zValues, timeValues, dataList = [], [], [], [], []
        xList, yList, zList, tList = [], [], [], []
        toc = 0
        while toc < 3:
            tic = time.time()  # time axis tmp
            timeValues.append(toc)
            uart.readPort()
            x = uart.getX()
            xValues.append(x)
            y = uart.getY()
            yValues.append(y)
            z = uart.getZ()
            zValues.append(z)
            data = str(x) + " " + str(y) + " " + str(z) + " " + str(toc) + "\n"
            dataList.append(data)
            ## PLOT
            ax.set_xlim(left = 0, right = 2)
            ax.plot(timeValues, xValues, 'b-',
                    timeValues, yValues, 'g-',
                    timeValues, zValues, 'r-')

            plt.show(block=False)
            plt.pause(0.000001)  # czas odswiezania wykresu
            toc += time.time() - tic
        saveTestingGesture(dataList)
        recognizedGesturePath = dtwTest(9)# set distance
        recognizedGestureName = recognizedGesturePath.replace("gestures\\", "")
        recognizedGestureName, fileFormat = recognizedGestureName.split(".")
        uart.closePort()
        if(recognizedGesturePath != "Tego gestu nie ma w bazie.log"):
            with open(recognizedGesturePath, 'r') as file:
                GestureData = file.read()
                GestureData = GestureData.split("\n")
                for i in GestureData:
                    try:
                        x1, y1, z1, t1 = i.split()
                        xList.append(float(x1))
                        yList.append(float(y1))
                        zList.append(float(z1))
                        tList.append(float(t1))
                    except:
                        pass
        ax2.plot(tList, xList, 'b-', label = "X")
        ax2.plot(tList, yList, 'g-', label = "Y")
        ax2.plot(tList, zList, 'r-', label = "Z")
        ax2.legend()
        ax2.text(0.7, 1.75, "Wykryto następujący gest : " + recognizedGestureName, style='italic',
                    bbox={'facecolor': 'green', 'alpha': 0.5, 'pad': 3})
        plt.show(block=False)


    uart.openPort("COM7", 9600)  # nazwa portu, predkosc transmisji
    plt.style.use('seaborn-bright')
    fig = plt.figure(figsize = (9,7.5))
    ax = fig.add_subplot(211)
    ax.set_ylim(-2, 2)
    ax.set_xlabel("Time (s)")
    ax.set_ylabel("X, Y, Z of Tested Signal")
    ax.set_xlim(left=0, right=2)
    ax2 = fig.add_subplot(212)
    ax2.set_ylim(-2, 2)
    ax2.set_xlabel("Time (s)")
    ax2.set_ylabel("X, Y, Z of Recognized Signal")
    ax2.set_xlim(left=0, right=2)
    axButtonStart = plt.axes([0.2, 0.90, 0.6, 0.05])
    buttonStart = Button(axButtonStart, "START")
    buttonStart.on_clicked(Start)
    plt.show()

def closeAll():
    plt.close("all")
    uart.closePort()
    button1.configure(text="Wyswietlaj dane z akcelerometru", command = liveData)
    button2.configure(text="Zapisz gest", command = saveGesture)
    button3.configure(text="Rozpoznaj gest", command = recognizeGesture)

def exitButton():
    plt.close("all")
    uart.closePort()
    window.destroy()


# Main menu
window = Tk()
window.title("KL46 Project")
window.geometry("370x220")
window.resizable(0,0)
window.configure(background = "white")
button1 = Button(window, text = "Wyswietlaj dane z akcelerometru", width = 30, height = 2, font = 'Verdana 13 bold italic', background = "white", command = liveData)
button1.grid(row = 3, column = 1, sticky = W)
button2 = Button(window, text = "Zapisz gest", width = 30, height = 2, font = 'Verdana 13 bold italic', background = "white", command = saveGesture)
button2.grid(row = 4, column = 1, sticky = W)
button3 = Button(window, text = "Rozpoznaj gest", width = 30, height = 2, font = 'Verdana 13 bold italic', background = "white", command = recognizeGesture)
button3.grid(row = 5, column = 1, sticky = W)
Button(window, text = "Wyjdz", width = 30,height = 2, font = 'Verdana 13 bold italic', background = "white", command = exitButton).grid(row = 6, column = 1, sticky = W)
window.mainloop()











