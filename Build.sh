CURRENT_DIR=`readlink -f .`

echo "CURRENT_DIR = " $CURRENT_DIR

TARGET_DIR=$CURRENT_DIR/bin

rm -rf $TARGET_DIR

mkdir $TARGET_DIR

echo "TARGET_DIR = " $TARGET_DIR

cd src

echo "Building src..."

cc -o $TARGET_DIR/DomoDaemon DomoDaemon.c serial.c -lrt -lpthread -ludev
cc -o $TARGET_DIR/Domo.o -c -Wall -Werror -fPIC Domo.c
cc -shared -o $TARGET_DIR/libDomo.so $TARGET_DIR/Domo.o -lrt
cc -o $TARGET_DIR/Example Example.c $TARGET_DIR/libDomo.so
cp Domo.py $TARGET_DIR/Domo.py

echo "Copying shared object file to /usr/lib/..."

echo "Build End"


