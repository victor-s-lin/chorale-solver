/*
 * File: chorale-constants.h
 * Name: Victor Lin
 * -------------------------
 * This file contains some constants that are used throughout the implementation.
 */

#ifndef CHORALECONSTANTS_H
#define CHORALECONSTANTS_H
#include "map.h"
#include <vector>

static const int BASS_MIN = 0;
static const int BASS_MAX = 24;
static const int TENOR_MIN = 12;
static const int TENOR_MAX = 31;
static const int ALTO_MIN = 19;
static const int ALTO_MAX = 36;
static const int SOPRANO_MIN = 24;
static const int SOPRANO_MAX = 43;
//static const std::map<std::string, int> lowestNote;
bool majorKey = true;

/*
 * This vector contains chord relationships. Each index of the vector corresponds to a chord (e.g. index 1 would be used for a I chord). Each index stores a vector of chords that are permitted to follow the chord at the index.
 * IMPORTANT: The major VII chord (for use in minor keys) is stored as index 8. 8 always refers to the major VII.
 */
static std::vector<std::vector<int>> chordRelations;

/*
 * This map associates numbers on the keyboard with the names of the keys.
 */
static Map<int, std::string> keyMap;


#endif // CHORALECONSTANTS_H
