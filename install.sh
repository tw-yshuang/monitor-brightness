#!/bin/bash

set -e

sudo apt update
sudo apt -y install x11-xserver-utils
sudo apt -y install make build-essential
sudo make
monitor-brightness $(xrandr | grep ' connected ' | awk '{ print$1 }') &
