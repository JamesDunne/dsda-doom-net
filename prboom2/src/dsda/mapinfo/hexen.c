//
// Copyright(C) 2022 by Ryan Krafnick
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
//  DSDA MapInfo Hexen
//

#include "doomstat.h"
#include "g_game.h"
#include "lprintf.h"
#include "m_argv.h"
#include "m_misc.h"
#include "p_setup.h"
#include "r_data.h"
#include "s_sound.h"
#include "sc_man.h"
#include "sounds.h"
#include "w_wad.h"

#include "hexen/p_acs.h"
#include "hexen/sv_save.h"

#include "dsda/map_format.h"
#include "dsda/mapinfo.h"

#include "hexen.h"

#define MAPINFO_SCRIPT_NAME "MAPINFO"
#define MCMD_SKY1 1
#define MCMD_SKY2 2
#define MCMD_LIGHTNING 3
#define MCMD_FADETABLE 4
#define MCMD_DOUBLESKY 5
#define MCMD_CLUSTER 6
#define MCMD_WARPTRANS 7
#define MCMD_NEXT 8
#define MCMD_CDTRACK 9
#define MCMD_CD_STARTTRACK 10
#define MCMD_CD_END1TRACK 11
#define MCMD_CD_END2TRACK 12
#define MCMD_CD_END3TRACK 13
#define MCMD_CD_INTERTRACK 14
#define MCMD_CD_TITLETRACK 15

#define UNKNOWN_MAP_NAME "DEVELOPMENT MAP"
#define DEFAULT_SKY_NAME "SKY1"
#define DEFAULT_FADE_TABLE "COLORMAP"

typedef struct mapInfo_s {
  short cluster;
  short warpTrans;
  short nextMap;
  char name[32];
  short sky1Texture;
  short sky2Texture;
  fixed_t sky1ScrollDelta;
  fixed_t sky2ScrollDelta;
  dboolean doubleSky;
  dboolean lightning;
  int fadetable;
  char songLump[10];
} mapInfo_t;

static int MapCount = 98;

static mapInfo_t MapInfo[99];

static const char *MapCmdNames[] = {
  "SKY1",
  "SKY2",
  "DOUBLESKY",
  "LIGHTNING",
  "FADETABLE",
  "CLUSTER",
  "WARPTRANS",
  "NEXT",
  "CDTRACK",
  "CD_START_TRACK",
  "CD_END1_TRACK",
  "CD_END2_TRACK",
  "CD_END3_TRACK",
  "CD_INTERMISSION_TRACK",
  "CD_TITLE_TRACK",
  NULL
};

static int MapCmdIDs[] = {
  MCMD_SKY1,
  MCMD_SKY2,
  MCMD_DOUBLESKY,
  MCMD_LIGHTNING,
  MCMD_FADETABLE,
  MCMD_CLUSTER,
  MCMD_WARPTRANS,
  MCMD_NEXT,
  MCMD_CDTRACK,
  MCMD_CD_STARTTRACK,
  MCMD_CD_END1TRACK,
  MCMD_CD_END2TRACK,
  MCMD_CD_END3TRACK,
  MCMD_CD_INTERTRACK,
  MCMD_CD_TITLETRACK
};

static int QualifyMap(int map) {
  return (map < 1 || map > MapCount) ? 0 : map;
}

static int P_TranslateMap(int map) {
  int i;

  for (i = 1; i < 99; i++)
    if (MapInfo[i].warpTrans == map)
      return i;

  return -1;
}

int P_GetMapSky1Texture(int map) {
  return MapInfo[QualifyMap(map)].sky1Texture;
}

int P_GetMapSky2Texture(int map) {
  return MapInfo[QualifyMap(map)].sky2Texture;
}

fixed_t P_GetMapSky1ScrollDelta(int map) {
  return MapInfo[QualifyMap(map)].sky1ScrollDelta;
}

fixed_t P_GetMapSky2ScrollDelta(int map) {
  return MapInfo[QualifyMap(map)].sky2ScrollDelta;
}

dboolean P_GetMapDoubleSky(int map) {
  return MapInfo[QualifyMap(map)].doubleSky;
}

void P_PutMapSongLump(int map, char *lumpName) {
  if (map < 1 || map > MapCount)
    return;

  M_StringCopy(MapInfo[map].songLump, lumpName,
               sizeof(MapInfo[map].songLump));
}

int dsda_HexenFirstMap(int* episode, int* map) {
  if (!map_format.mapinfo)
    return false;

  *episode = 1;
  *map = P_TranslateMap(1);

  if (*map == -1)
    I_Error("Unable to detect default first map");

  return true;
}

int dsda_HexenNewGameMap(int* episode, int* map) {
  if (!map_format.mapinfo)
    return false;

  *episode = 1;
  *map = P_TranslateMap(*map);

  if (*map == -1)
    *map = 1;

  return true;
}

int dsda_HexenResolveWarp(int arg_p, int* episode, int* map) {
  if (!map_format.mapinfo)
    return false;

  *episode = 1;

  if (arg_p < myargc - 1)
    *map = P_TranslateMap(atoi(myargv[arg_p + 1]));
  else
    *map = P_TranslateMap(1);

  if (*map == -1)
    I_Error("-warp: Invalid map number.\n");

  return true;
}

int dsda_HexenNextMap(int* episode, int* map) {
  if (!map_format.mapinfo)
    return false;

  *episode = 1;
  *map = MapInfo[QualifyMap(gamemap)].nextMap;

  return true;
}

int dsda_HexenShowNextLocBehaviour(int* behaviour) {
  return false; // TODO
}

int dsda_HexenSkipDrawShowNextLoc(int* skip) {
  return false; // TODO
}

void dsda_HexenUpdateMapInfo(void) {
  // TODO
}

void dsda_HexenUpdateLastMapInfo(void) {
  // TODO
}

void dsda_HexenUpdateNextMapInfo(void) {
  // TODO
}

int dsda_HexenResolveCLEV(int* clev, int* episode, int* map) {
  char* next;

  if (!map_format.mapinfo)
    return false;

  // Catch invalid maps
  next = MAPNAME(*episode, P_TranslateMap(*map));
  if (W_CheckNumForName(next) == -1) {
    doom_printf("IDCLEV target not found: %s", next);
    *clev = false;
  }
  else
    *clev = true;

  return true;
}

dboolean partial_reset = false;

int dsda_HexenResolveINIT(int* init) {
  if (!map_format.mapinfo)
    return false;

  partial_reset = true;

  G_DeferedInitNew(gameskill, gameepisode,
                   MapInfo[QualifyMap(gamemap)].warpTrans);

  *init = true;

  return true;
}

int dsda_HexenMusicIndexToLumpNum(int* lump, int music_index) {
  const char* lump_name;

  if (!map_format.mapinfo)
    return false;

  if (music_index >= hexen_mus_hub)
    return false;

  if (!*MapInfo[QualifyMap(music_index)].songLump)
    lump_name = NULL;
  else
    lump_name = MapInfo[QualifyMap(music_index)].songLump;

  if (!lump_name)
    *lump = 0;
  else
    *lump = W_GetNumForName(lump_name);

  return true;
}

int dsda_HexenMapMusic(int* music_index, int* music_lump) {
  if (!map_format.mapinfo)
    return false;

  *music_lump = -1;
  *music_index = gamemap;

  return true;
}

int dsda_HexenInterMusic(int* music_index, int* music_lump) {
  return false; // TODO
}

int dsda_HexenStartFinale(void) {
  return false; // TODO
}

int dsda_HexenFTicker(void) {
  return false; // TODO
}

void dsda_HexenFDrawer(void) {
  return; // TODO
}

int dsda_HexenBossAction(mobj_t* mo) {
  return false; // TODO
}

int dsda_HexenHUTitle(const char** title) {
  if (!map_format.mapinfo)
    return false;

  *title = NULL;

  if (gamestate == GS_LEVEL && gamemap > 0 && gameepisode > 0)
    *title = MapInfo[QualifyMap(gamemap)].name;

  if (*title == NULL)
    *title = MAPNAME(gameepisode, gamemap);

  return true;
}

int dsda_HexenSkyTexture(int* sky) {
  return false; // TODO
}

int dsda_HexenPrepareInitNew(void) {
  extern int RebornPosition;

  if (!map_format.mapinfo)
    return false;

  SV_Init();

  if (partial_reset) {
    partial_reset = false;
    return true;
  }

  if (map_format.acs)
    P_ACSInitNewGame();

  // Default the player start spot group to 0
  RebornPosition = 0;

  return true;
}

int dsda_HexenPrepareIntermission(int* result) {
  return false; // TODO
}

int dsda_HexenPrepareFinale(int* result) {
  return false; // TODO
}

void dsda_HexenLoadMapInfo(void) {
  int map;
  int mapMax;
  int mcmdValue;
  mapInfo_t* info;
  char songMulch[10];
  const char* default_sky_name = DEFAULT_SKY_NAME;

  if (!map_format.mapinfo)
    return;

  mapMax = 1;

  if (gamemode == shareware)
    default_sky_name = "SKY2";

  // Put defaults into MapInfo[0]
  info = MapInfo;
  info->cluster = 0;
  info->warpTrans = 0;
  info->nextMap = 1; // Always go to map 1 if not specified
  info->sky1Texture = R_TextureNumForName(default_sky_name);
  info->sky2Texture = info->sky1Texture;
  info->sky1ScrollDelta = 0;
  info->sky2ScrollDelta = 0;
  info->doubleSky = false;
  info->lightning = false;
  info->fadetable = W_GetNumForName(DEFAULT_FADE_TABLE);
  M_StringCopy(info->name, UNKNOWN_MAP_NAME, sizeof(info->name));

  SC_OpenLump(MAPINFO_SCRIPT_NAME);
  while (SC_GetString()) {
    if (SC_Compare("MAP") == false)
      SC_ScriptError(NULL);

    SC_MustGetNumber();
    if (sc_Number < 1 || sc_Number > 99)
      SC_ScriptError(NULL);

    map = sc_Number;

    info = &MapInfo[map];

    // Save song lump name
    M_StringCopy(songMulch, info->songLump, sizeof(songMulch));

    // Copy defaults to current map definition
    memcpy(info, &MapInfo[0], sizeof(*info));

    // Restore song lump name
    M_StringCopy(info->songLump, songMulch, sizeof(info->songLump));

    // The warp translation defaults to the map number
    info->warpTrans = map;

    // Map name must follow the number
    SC_MustGetString();
    M_StringCopy(info->name, sc_String, sizeof(info->name));

    // Process optional tokens
    while (SC_GetString()) {
      if (SC_Compare("MAP")) { // Start next map definition
        SC_UnGet();
        break;
      }

      mcmdValue = MapCmdIDs[SC_MustMatchString(MapCmdNames)];
      switch (mcmdValue) {
        case MCMD_CLUSTER:
          SC_MustGetNumber();
          info->cluster = sc_Number;
          break;
        case MCMD_WARPTRANS:
          SC_MustGetNumber();
          info->warpTrans = sc_Number;
          break;
        case MCMD_NEXT:
          SC_MustGetNumber();
          info->nextMap = sc_Number;
          break;
        case MCMD_CDTRACK:
          SC_MustGetNumber();
          // not used
          break;
        case MCMD_SKY1:
          SC_MustGetString();
          info->sky1Texture = R_TextureNumForName(sc_String);
          SC_MustGetNumber();
          info->sky1ScrollDelta = sc_Number << 8;
          break;
        case MCMD_SKY2:
          SC_MustGetString();
          info->sky2Texture = R_TextureNumForName(sc_String);
          SC_MustGetNumber();
          info->sky2ScrollDelta = sc_Number << 8;
          break;
        case MCMD_DOUBLESKY:
          info->doubleSky = true;
          break;
        case MCMD_LIGHTNING:
          info->lightning = true;
          break;
        case MCMD_FADETABLE:
          SC_MustGetString();
          info->fadetable = W_GetNumForName(sc_String);
          break;
        case MCMD_CD_STARTTRACK:
        case MCMD_CD_END1TRACK:
        case MCMD_CD_END2TRACK:
        case MCMD_CD_END3TRACK:
        case MCMD_CD_INTERTRACK:
        case MCMD_CD_TITLETRACK:
          SC_MustGetNumber();
          // not used
          break;
      }
    }
    mapMax = map > mapMax ? map : mapMax;
  }

  SC_Close();

  MapCount = mapMax;
}

int dsda_HexenExitPic(const char** exit_pic) {
  return false; // TODO
}

int dsda_HexenEnterPic(const char** enter_pic) {
  return false; // TODO
}

int dsda_HexenPrepareEntering(void) {
  return false; // TODO
}

int dsda_HexenPrepareFinished(void) {
  return false; // TODO
}

int dsda_HexenMapLightning(int* lightning, int map) {
  if (!map_format.mapinfo)
    return false;

  *lightning = MapInfo[QualifyMap(map)].lightning;

  return true;
}

int dsda_HexenApplyFadeTable(void) {
  extern dboolean LevelUseFullBright;
  extern const lighttable_t** colormaps;

  int fade_lump;

  if (!map_format.mapinfo)
    return false;

  fade_lump = MapInfo[QualifyMap(gamemap)].fadetable;

  colormaps[0] = (const lighttable_t *) W_CacheLumpNum(fade_lump);

  if (fade_lump == W_GetNumForName("COLORMAP"))
    LevelUseFullBright = true;
  else
    LevelUseFullBright = false; // Probably fog ... don't use fullbright sprites

  return true;
}

int dsda_HexenMapCluster(int* cluster, int map) {
  if (!map_format.mapinfo)
    return false;

  *cluster = MapInfo[QualifyMap(map)].cluster;

  return true;
}
