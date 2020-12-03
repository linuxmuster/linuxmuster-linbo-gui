#!/bin/bash
for i in $(cat libs.txt); do
    #echo $i
    rsync -avz root@10.9.0.2:$i ./
done
