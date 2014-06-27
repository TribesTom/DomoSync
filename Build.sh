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
cp Domo.py $TARGET_DIR/Domo.py
cd ../example
for F in *.c; do cc -o ${F%.c} $F $TARGET_DIR/libDomo.so -Wall ; done


echo "Build End"
