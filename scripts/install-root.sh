#!/usr/bin/env bash

# Installs ROOT on NIX
echo "copying .bashrc to .bashrc-backup-root for safe keeping"
cp ~/.bashrc ~/.bashrc-backup-root

# Install Pythia8

mkdir -p /opt

pushd /opt

wget -O root.tgz \
    https://root.cern.ch/download/root_v6.08.00.Linux-ubuntu16-x86_64-gcc5.4.tar.gz && \
    tar -xzf root.tgz 

if [[ ! -z $USER  ]]; then
	echo "running a chown op on /opt/root "
	chown -R $USER /opt/root 
fi

cat >> ~/.bashrc <<-'EOF'

# -- ROOT 6 install

export ROOTSYS=/opt/root
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ROOTSYS/lib
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$ROOTSYS/lib
export PYTHONPATH=$PYTHONPATH:$ROOTSYS/lib
export PATH=$PATH:$ROOTSYS/bin
export DISPLAY=""

EOF
