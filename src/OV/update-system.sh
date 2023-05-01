#!/bin/bash

OPKG_OUT=$(sudo apt list --upgradable)

echo $OPKG_OUT

echo -e "\nUpdating system ...\n\n"
sudo apt upgrade -y
sudo apt autoremove -y
