# Created by Khai Ly on 3/12/2019

#!/bin/bash

# Use cat to print out three lines instead of using echo many times.
# Cores: use grep to count all lines start with "processor" in proc/cpuinfo.
# Memories: use free -m to find memory usage and pipe it to awk and print out the 3rd and 2nd column on 2nd row.
# Storage: use df -h to find disk info and print out the 3rd and 2nd column on the 4th row (sda1).

cat << EOF
cores: $(grep -c ^processor /proc/cpuinfo)
memories: $(free -m | awk 'NR==2{printf "%sMB / %sMB (%.2f%%)\n", $3,$2,$3*100/$2 }')
primary partition storage: $(df -h | grep -h sda | awk '{printf "%sB / %sB\n", $3, $2}')
EOF

