#!/usr/bin/env bash

# Installs ROOT on OSX

brew install --ignore-dependencies homebrew/science/root6

echo "copying .bashrc to .bashrc-backup for safe keeping"
cp ~/.bashrc ~/.bashrc-backup
echo '. $(brew --prefix root6)/libexec/thisroot.sh' >> ~/.bashrc