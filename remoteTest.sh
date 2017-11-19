#!/bin/bash
git add .
git commit -a -m "update"
git push
echo "uploading $1..."
commands = "cd /home/pi/aparduino; git pull; echo "entering $1"; cd "$1"; make; sudo make upload"
ssh pi@192.168.1.9 '$commands'

