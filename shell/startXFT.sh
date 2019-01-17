
cd

    psx=`ps -ef|grep XFT|grep -v grep|grep -v '\.'`
    if [ -z "$psx" ]
    then
        bin/XFT -mT -s./SignonDB/XFT/ -S./Settings/XFT
    fi
