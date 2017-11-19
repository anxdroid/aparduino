#!/bin/bash
git add .
git commit -a -m "update"
git push
ssh pi@192.168.1.9 'cd /home/pi/aparduino; git pull'

