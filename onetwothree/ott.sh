#!/bin/bash
read n
p=(0 0 2 1 1 0 2 2 1)
c=30331;
for ((i=1;i<$n;i++))
do
((c=c+19+${p[$i-1]}))
done
printf "%s\n" `hexdump -s $c -n $((${p[$n-1]}+3)) /bin/dumpkeys -e '"%c"'`