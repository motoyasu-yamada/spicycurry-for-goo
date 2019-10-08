#!/bin/bash

#
#   check arg
#
if [ $# -eq 0 ] ; then
    echo "SPICY web application installer"
    echo "SYNTAX: commit <application-home-directory>"
    exit 0
fi

#
#   make backup & rollback.
#   copy debug current
#
pushd  $1

TODAY=$(date +%Y%m%d)

#   make backup & rollback.
cp -rf current current.bak.$TODAY
cp -rf current rollback

#   copy debug current
rm -rf current
cp -rf debug   current

popd

echo "commit done."