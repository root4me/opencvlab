#!/bin/bash

# parameter : file name with out extension (.mp4) is assumed

echo $1

if [[ $# == 0 ]]
then
	echo "usage : . ./mp4topng.sh sample/07182015_1"	
	echo "extension (.mp4) is assumed"
else
	ffmpeg -i $1.mp4 -r 1/.5 $1%d.png
	for f in $1*.png;  
	do  
		ffmpeg -i "$f" -vf scale=640:-1 "${f%}" -y; 
	done
fi
