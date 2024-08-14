#!/bin/bash

# Could add these as cli arguments
folder=.
max=100

df_res=$(du -s $folder/*) 

while read -r line
do
    split=( $line )
    size=${split[0]}
    folder=${split[1]}

    if test $size -lt $max
    then
        echo "$folder with size $size KB is less than $max KB"
    fi
done <<< "$df_res"
