g++-6 -I/usr/local/include -I/usr/include/python2.7 -fpic -o eeg.o -c eeg_reader.cpp
g++-6  -std=c++11 -shared -Wl,-soname,"libeeg.so" -L/usr/local/lib eeg.o -lboost_python -fpic -o openeeg.so
