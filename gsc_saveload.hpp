#ifndef _GSC_SAVELOAD_HPP_
#define _GSC_SAVELOAD_HPP_

/* gsc functions */
#include "../gsc.hpp"


void gsc_saveload_initclient(int id);
void gsc_saveload_save(int id);
void gsc_saveload_selectsave(int id);
void gsc_saveload_getorigin(int id);
void gsc_saveload_getangles(int id);
void gsc_saveload_getgroundentity(int id);
void gsc_saveload_getnadejumps(int id);

#endif