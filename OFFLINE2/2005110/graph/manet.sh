#!/bin/bash

# List of combinations
speeds=(5 10 15 20)
packets=(100 200 300 400)
nodes=(20 40 70 100)
##

# Loop over all combinations
for speed in "${speeds[@]}"
do
    for packet in "${packets[@]}"
    do    
        for node in "${nodes[@]}"
            do 
                echo "Running simulation with nodes=$node, packets=$packet, speed=$speed"
            
                # Run NS-3 simulation for each combination
                ./ns3 run "scratch/manet-routing-compare --nodes=$node --packets=$packet --speed=$speed"
            done
        ##for 4 diff node, need to plot graph nodes vs throughput, end-to-end-delay,packet-delivary-ratio, packet-drop-ratio
    done
done
