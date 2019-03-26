#!/bin/bash

for moduleDir in $(ls -1)
do
	if [ -d $moduleDir ];
	then
		echo "* Building $moduleDir"
		cd $moduleDir
			for filePath in $(find . -type f -follow)
			do
				fileName=$(basename -- "$filePath")
				baseName="${fileName%.*}"
				modPath="../../includes/modules/$moduleDir"
				
				echo "  - Converting $fileName"
				if [[ ! -e $modPath ]]; then
					mkdir -p $modPath
				fi
				
				bin2c $filePath $modPath/$baseName.h $baseName"_"$moduleDir
			done;
			
			echo ""
		cd ..
	fi
done;