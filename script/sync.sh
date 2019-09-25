#!/bin/bash
time=$(date "+%Y%m%d%H%M%S")
cp /data/res/group.tox "/data/res/back/tox_"${time}
echo "success"
