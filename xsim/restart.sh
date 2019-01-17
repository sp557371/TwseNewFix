#!/bin/csh
cd $HOME/xsim
setenv LD_LIBRARY_PATH ./lib:$LD_LIBRARY_PATH
ps -ef|grep XSim |grep -v grep |awk '{print $2}'|xargs kill -9
#pkill -9 XSim
rm -rf Logs
bin/XSim -d -Snsconfig
