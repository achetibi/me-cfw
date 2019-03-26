#!/bin/bash

for pspbtcnf in $(ls | grep -iE "[.]txt")
do
	echo "* Building $pspbtcnf"
	psp-btcnf -b $pspbtcnf
	echo ""
done;