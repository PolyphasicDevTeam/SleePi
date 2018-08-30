#!/bin/bash
~/eeg/setuptty.sh
echo "START" &>> ~/eeg.log
~/eeg/processor.py &>> ~/eeg.log
