#!/usr/bin/python3.6
import datetime
import openeeg
import pyqtgraph as pg
from pyqtgraph.Qt import QtGui, QtCore
import numpy as np
import threading
import pyqtgraph.multiprocess as mp
import time
print("Starting EEG reader")


data1 = np.ones((1024,), dtype=np.int)*512
data2 = np.ones((1024,), dtype=np.int)*512

def qtSlave():
    app = QtGui.QApplication([])
    win = pg.GraphicsWindow(title="My plotting examples")
    win.resize(480,360)
    win.setWindowTitle('EEG')
    plotwin1 = win.addPlot(row=0,col=0,title="Channel 1")
    plotwin2 = win.addPlot(row=1,col=0,title="Channel 2")
    curve1 = plotwin1.plot(pen='y')
    curve2 = plotwin2.plot(pen='y')
    plotwin1.setYRange(0,1024,padding=0)
    plotwin2.setYRange(0,1024,padding=0)
    while True:
        curve1.setData(y=data1, _callSync='off')
        curve2.setData(y=data2, _callSync='off')
        app.processEvents()
        time.sleep(0.1)

t1 = threading.Thread(target = qtSlave, args = ())
t1.start()

openeeg.eeg_init();
fout = open('data/{}.dat'.format(datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")),'w');
freq = 2
while True:
    update = 0
    eeg_vals = openeeg.eeg_get();
    if eeg_vals:
        eeg_vals = filter(None,eeg_vals.split('X'))
        for eeg_val in eeg_vals:
            fout.write(eeg_val);
            fout.write('\n')
            fout.flush();
            ch1 = eeg_val.split(' ')[0];
            ch2 = eeg_val.split(' ')[1];
            data1 = np.roll(data1,-1)
            data2 = np.roll(data2,-1)
            if update % freq == 0:
                data1[1023] = ch1;
                data2[1023] = ch2;
            update += 1
            #print data1
fout.close();
openeeg.eeg_destroy();

