#!/bin/bash

# scale down positive images to 100x width aspect ratio
cd positive
for f in *.png; 
do 
	ffmpeg -i "$f" -vf scale=100:-1 "${f%}" -y
done

cd ..

# scale down negative video to 640x width
cd negative
for f in *.MOV; 
do 
	ffmpeg -i "$f" -vf scale=640:-1 "${f%}".avi -y
done

# extract images every 2 seconds from video
counter=1
for f in *.avi; 
do 
	echo "$counter"
	ffmpeg -i "$f" -r 1/2 pic"$counter"%"$counter"d.png -y
	counter=$((counter+1));
done

cd ..

# negatives file list
find ./negative -iname "*.png" > negatives.txt


####opencv_createsamples -info ./samples/info.dat -img ./positive/positive.png -bg ./negatives.txt maxxangle 0.1 -maxyangle 0.1 -maxzangle 0.1

#opencv_createsamples -vec samples/samples1.vec -img positive/positive1.png -bg negatives.txt maxxangle 0.1 -maxyangle 0.1 -maxzangle 0.1 -w 100 -h 75


# opencv_createsamples -bgcolor 0 -bgthresh 0 -maxxangle 1.1  -maxyangle 1.1 maxzangle 0.5 -maxidev 40 -w 80 -h 40 -img ./positive_images/test1436675381.png -bg tmp -vec samples/test1436675381.png.vec -num 37

#opencv_createsamples -info ./samples/info.dat -vec samples.vec

#opencv_createsamples -info ./samples/info.dat -img ./positive/positive.png -bg ./negatives.txt maxxangle 0.1 -maxyangle 0.1 -maxzangle 0.1 -w 100 -h 75

#opencv_traincascade -data classifier -vec samples.vec -bg negatives.txt -numStages 2 -minHitRate 0.999 -maxFalseAlarmRate 0.5 -numPos 392 -numNeg 392 -w 100 -h 75 -mode ALL -precalcValBufSize 1024 -precalcIdxBufSize 1024


opencv_createsamples -vec samples/samples1.vec -img positive/positive1.png -bg negatives.txt maxxangle 0.1 -maxyangle 0.1 -maxzangle 0.1 -w 100 -h 75

python mergevec.py -v samples -o samples.vec

opencv_traincascade -data classifier -vec samples.vec -bg negatives.txt -numStages 2 -minHitRate 0.999 -maxFalseAlarmRate 0.5 -numPos 1000 -numNeg 300 -w 100 -h 75 -mode ALL -precalcValBufSize 1024 -precalcIdxBufSize 1024
