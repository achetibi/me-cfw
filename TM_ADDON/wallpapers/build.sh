#!/bin/bash


echo "* Building wallpapers.bmp"
for wallpaper in $(ls | grep -iE "[.]bmp" | sort | uniq)
do
	echo "  - Converting $wallpaper"
	cat $wallpaper >> wallpapers.bmp
done;