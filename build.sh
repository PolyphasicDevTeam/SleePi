g++ -I/usr/local/include -I/usr/include/python3.6m -fpic -o eeg.o -c eeg_reader.cpp
g++  -std=c++11 -shared -Wl,-soname,"libeeg.so" -L/usr/local/lib eeg.o -lboost_python36 -fpic -o openeeg.so
