#!/bin/bash

rm negative.txt
find ./negative -iname "*.png" > negative.txt

rm positive.vec
opencv_createsamples -info positive.info -vec positive.vec - num $1

rm ./classifier/*.*
opencv_traincascade -data classifier -vec positive.vec -bg negative.txt -numStages $3 -minHitRate 0.999 -maxFalseAlarmRate 0.5 -numPos $1 -numNeg $2 -mode ALL -precalcValBufSize 1024 -precalcIdxBufSize 1024

