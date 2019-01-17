#!/bin/csh
ps -ef | grep XSim | grep `whoami` | grep -v grep | awk '{print $2}' | xargs kill -9
rm -rf Logs
