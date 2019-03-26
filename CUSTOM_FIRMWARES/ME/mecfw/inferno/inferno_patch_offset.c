/*
 * This file is part of PRO CFW.

 * PRO CFW is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * PRO CFW is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PRO CFW. If not, see <http://www.gnu.org/licenses/ .
 */

#include <pspsdk.h>

#include "inferno_patch_offset.h"

#if _PSP_FW_VERSION == 660
PatchOffset g_offsets = {
	.patches = {
		0x00003FEC,
		0x00004024,
		0x000040D8,
		0x000042B4,
	},
};

#elif _PSP_FW_VERSION == 639
PatchOffset g_offsets = {
	.patches = {
		0x00004020,
		0x00004058,
		0x0000410C,
		0x000042E8,
	},
};

#elif _PSP_FW_VERSION == 620
PatchOffset g_offsets = {
	.patches = {
		0x00004020,
		0x00004058,
		0x0000410C,
		0x000042E8,
	},
};
#endif
