#!/bin/bash
echo "SPICY web application installer"

#
#   check arg
#
if [ $# -eq 0 ] ; then
    echo "SYNTAX: rollback <application-home-directory>"
    exit 0
fi
echo "commiting"

#
#   make backup & rollback.
#   copy debug current
#

pushd  $1

TODAY=$(date +%Y%m%d)

#   make backup & rollback.
cp -rf current rollback.bak.$TODAY

#   copy debug  current
rm -rf current
cp -rf rollback current

popd

echo "rollback done."