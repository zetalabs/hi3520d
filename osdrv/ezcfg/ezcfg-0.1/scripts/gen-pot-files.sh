#!/bin/bash

SOURCE_LIST=$1

usage() {
  echo "usage: ./gen-pot-files.sh <list file>"
}

dbg() {
  echo $1
}

if [ "x$SOURCE_LIST" = "x" ] ; then
  usage
  exit 0
fi

DOMAIN=
OUT_FILE=
IN_FILE=

while read LINE
do
  echo $LINE
  if [ "x$LINE" = "x" ] ; then
    echo "LINE is empty."
    continue;
  fi
  FIRST_CHAR=${LINE:0:1}
  if [ $FIRST_CHAR = "#" ] ; then
    echo "It's a comment."
  else
    if [ "x$DOMAIN" = "x" ] ; then
      DOMAIN=$LINE
    elif [ "x$OUT_FILE" = "x" ] ; then
      OUT_FILE=$LINE
    elif [ "x$IN_FILE" = "x" ] ; then
      IN_FILE=$LINE
      dbg "xgettext -d $DOMAIN -o $OUT_FILE -kezcfg_locale_text:2 -s $IN_FILE"
      xgettext -d $DOMAIN -o $OUT_FILE -kezcfg_locale_text:2 -s $IN_FILE
      DOMAIN=
      OUT_FILE=
      IN_FILE=
    fi
  fi
done < $SOURCE_LIST

