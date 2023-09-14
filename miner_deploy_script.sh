#!/bin/bash

# Become root (similar to Ansible's become: yes)
if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

# self-upgrade hiveos
selfupgrade || true

# update nvidia-drivers
if nvidia-smi &>/dev/null; then
    nvidia-driver-update || echo "nvidia-driver-update encountered an error, but continuing..."
else
    echo "No NVIDIA GPU detected. Skipping nvidia-driver-update."
fi

# install amd-drivers
if lspci | grep -i --quiet "VGA.*AMD"; then
    amd-ocl-install 22.20 || echo "amd-ocl-install encountered an error, but continuing..."
else
    echo "No AMD GPU detected. Skipping amd-ocl-install."
fi

# install dependencies
apt install -y build-essential cmake mesa-common-dev tmux

# clone quai-gpu-miner
git clone https://github.com/dominant-strategies/quai-gpu-miner /home/user/quai-gpu-miner
cd /home/user/quai-gpu-miner
git submodule update --init --recursive

# create build directory
if [ ! -d "/home/user/quai-gpu-miner/build" ]; then
    mkdir -p '/home/user/quai-gpu-miner/build'
fi

# configure cmake
cd '/home/user/quai-gpu-miner/build'
cmake ..

# build cmake
cmake --build .

# Finish
echo "All tasks completed successfully!"

