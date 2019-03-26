#!/usr/bin/python

import os, glob

def main():
	libs = [
		["KUBridge.S"				,"kubridge"			],
		["SystemCtrlForKernel.S"	,"systemctrl_kernel"],
		["SystemCtrlForUser.S"		,"systemctrl_user"	],
		["VshCtrlLib.S"				,"vshctrl"			]
	]

	for lib in libs:
#		target_file = lib[1] + "/" + lib[0]
		target_file = glob.glob(lib[1] + "/*.S")[0]
		output_file = "libpsp" + lib[1] + ".a"
		target_dir = lib[1]

		f = open( target_file )
		lines = f.read().split('\n')
		f.close()

		objects = lines[5][2:-1]

		ret = os.system("make -C %s \"TARGET=%s\" \"OBJS=%s\" "%( target_dir, output_file, objects ))
		assert(ret == 0)

if __name__ == "__main__":
	main()
