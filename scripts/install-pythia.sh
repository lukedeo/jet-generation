#!/usr/bin/env bash

# Installs Pythia8 - this should *always* be run as root
# Runs on $1 cores

if [ "$EUID" -ne 0 ]
  then echo "*** Please run as root ***"
  exit 1
fi

if [[ ! -z $1  ]]; then
	echo "setting -j$1 as a flag to make invocations"
	MAKEFLAGS="-j$1"
fi

echo "copying .bashrc to .bashrc-backup-pythia for safe keeping"
cp ~/.bashrc ~/.bashrc-backup-pythia

# Install Pythia8

mkdir -p /opt

pushd /opt

wget -q http://home.thep.lu.se/~torbjorn/pythia8/pythia8219.tgz 
tar -xzf pythia8219.tgz

if [[ ! -z $USER  ]]; then
	echo "running a chown op on /opt/pythia8219 "
	chown -R $USER /opt/pythia8219 
fi

cd pythia8219       && \
    ./configure     && \
    make $MAKEFLAGS && \
    cd /opt
popd


cat >> ~/.bashrc <<-'EOF'

# -- PYTHIA 8 install
export PYTHIA_ROOT=/opt/pythia8219

export C_INCLUDE_PATH=$C_INCLUDE_PATH:$PYTHIA_ROOT/include        
export CPLUS_INCLUDE_PATH=$C_INCLUDE_PATH:$PYTHIA_ROOT/include
export PATH=$PATH:$PYTHIA_ROOT/bin

EOF

