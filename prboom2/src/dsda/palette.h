//
// Copyright(C) 2020 by Ryan Krafnick
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//	DSDA Palette Management
//

#ifndef __DSDA_PALETTE__
#define __DSDA_PALETTE__

#include "SDL.h"

typedef enum {
  playpal_default,
  playpal_1,
  playpal_2,
  playpal_3,
  playpal_4,
  playpal_5,
  playpal_6,
  playpal_7,
  playpal_8,
  playpal_9,
  playpal_heretic_e2end,
  NUMPALETTES
} dsda_playpal_index_t;

typedef struct playpal_data_s {
  const int index;
  const char* lump_name;
  unsigned char* lump;
  int length;
  // See r_patch.c
  int transparent;
  int duplicate;
  int black;
  int white;

  // Array of SDL_Color structs used for setting the 256-colour palette
  SDL_Color* colours;
} dsda_playpal_t;

double dsda_PaletteEntryLightness(const char *playpal, int i);
void dsda_ColorLabToXYZ(double cL, double ca, double cb, double *x, double *y, double *z);
void dsda_ColorXYZtoLab(double x1, double y1, double z1, double *cL, double *ca, double *cb);
void dsda_PaletteGetColorLab(const char *playpal, int i, double *cL, double *ca, double *cb);
double dsda_XYZDistanceSquared(double x1, double y1, double z1, double x2, double y2, double z2);
void dsda_PaletteGetColorXYZ(const char *playpal, int i, double *x1, double *y1, double *z1);
int dsda_PaletteFindNearestXYZColor(const char *playpal, double x, double y, double z);
dsda_playpal_t* dsda_PlayPalData(void);
void dsda_CyclePlayPal(void);
void dsda_SetPlayPal(int index);
void dsda_FreePlayPal(void);
void dsda_InitPlayPal(void);

#endif
