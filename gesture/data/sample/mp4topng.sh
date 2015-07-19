#!/bin/bash

# parameter : file name with out extension (.mp4) is assumed

echo $1

if [[ $# == 0 ]]
then
	echo "parameter : file name with out extension (.mp4) is assumed"	
else
	ffmpeg -i $1.mp4 -r 1/1 $1%d.png
	for f in *.png;  
	do  
		ffmpeg -i "$f" -vf scale=640:-1 "${f%}" -y; 
	done
fi
