#!/bin/bash

func () {
    ORIG_DIR=$pwd
    DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
    DEST=$DIR/../gungi/tests/gungi/scenarios.res
    cd $DIR/../gungi/tests/gungi/res
    rm $DEST
    touch $DEST
    for f in `find *.gn`; do
        xxd -i $f >> $DEST
    done
    cd $ORIG_DIR
}

func
