#!/bin/csh
setenv LD_LIBRARY_PATH ./lib:$LD_LIBRARY_PATH
#pkill -9 XSim
ps -ef|grep XSim |grep -v grep |awk '{print $2}'|xargs kill -9
bin/XSim -d -Snsconfig 
