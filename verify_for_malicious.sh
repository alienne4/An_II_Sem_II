#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: $0 <filename>"
    exit 1
fi

filename=$1

if [ ! -e "$filename" ]; then
    echo "File $filename does not exist."
    exit 1
fi

permissions=$(ls -l "$filename" | awk '{print $1}')


empty_permissions="----------"

if [[ $permissions != $empty_permissions ]]; then
    exit 0
fi

words_to_check=("corrupted" "dangerous" "risk" "attack" "malware" "malicious")

for ((i = 0; i < 6; i++))
do
    if sudo grep -q "${words_to_check[$i]}" "$filename"; then
        echo "The word '${words_to_check[$i]}' is found in the file $filename."
        
        directory="isolated_space_dir"
        
        if [ ! -d "$directory" ]; then
            mkdir -p "$directory"
            echo "Directory $directory created successfully."
        fi
        
        mv "$filename" "$directory"
        
        exit 0
    fi
done
