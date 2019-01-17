cd /kway/rd/shell
cp /kway/rd/xtwse/kwaylib/bin/XCtrl       ../bin/XCtrl.D
cp /kway/rd/xtwse/kwaylib/bin/XFT         ../bin/XFT.D
cp /kway/rd/xtwse/kwaylib/bin/XMonitor ../bin/XMonitor.D
cp /kway/rd/xtwse/kwaylib/bin/XTester   ../bin/XTester.D
cp /kway/rd/xtwse/kwaylib/bin/XTwseC     ../bin/XTwse.D
rm -f ../lib/*.so.0.0.0
cp /kway/rd/xtwse/kwaylib/lib/*.so.0.0.0 ../lib/.
strip ../bin/*
chmod 755 ../lib/*.so.0.0.0
strip ../lib/*.so.0.0.0
