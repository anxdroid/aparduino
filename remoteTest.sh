#!/bin/bash
git add .
git commit -a -m "update"
git push
echo "uploading $1..."
commands="cd /home/pi/aparduino; git pull; cd $1; make;"
echo "sending commands: $commands"
ssh pi@192.168.1.9 '$commands'

