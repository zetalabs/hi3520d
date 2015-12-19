#!/bin/bash

usage() {
  echo "usage: ./replace-all-strings.sh <directory> <original string> <target string>"
}

dbg() {
  echo $1
}

DPATH="$1"
OLD="$2"
NEW="$3"

BPATH="./bakup.$$"
TFILE="/tmp/out.tmp.$$"

if [ "x$DPATH" = "x" ] ; then
  usage
  exit 0
fi

if [ "x$OLD" = "x" ] ; then
  usage
  exit 0
fi

if [ "x$NEW" = "x" ] ; then
  usage
  exit 0
fi

[ ! -d $BPATH ] && mkdir -p $BPATH || :
cp -af "$DPATH" "$BPATH"

find ${DPATH} -type f -exec grep -l "${OLD}" {} \; | while read file
do
  sed "s/$OLD/$NEW/g" "$file" > $TFILE && mv $TFILE "$file"
done
rm -f $TFILE

