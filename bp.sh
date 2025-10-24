#! /bin/bash

echo "testing branch predictor"


#compile code

g++ -o sim_bp sim_bp.cc

#Comparing misprediction rates for different bimodal predictor sizes
# for i in gcc_trace.txt jpeg_trace.txt perl_trace.txt
# do
#     echo $i
#     for j in 7 8 9 10 11 12 13 14 15 16 17 18 19 20
#     do
#         ./sim_bp bimodal $j $i
#     done
# done

#comparing misprediction rates for different bimodal predictor sizes

#comparing misprediction rates for differng gbh sizes in gshare
for i in 7 8 9 10 11 12 13 14 15 16 17 18 19 20         #m
do
    echo "m = $i"
    for j in 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20       #n
    do
        if [ $j -le $i ]; then
            ./sim_bp gshare $i $j gcc_trace.txt
        fi
    done
done
#comparing misprediction rates for different gbh sizes in gshare
