/*
 * $Id: sound2noise.c,v 1.1 2011/07/05 23:41:33 clivewebster Exp $
 * $Log: sound2noise.c,v $
 * Revision 1.1  2011/07/05 23:41:33  clivewebster
 * Moved to Audio folder
 *
 * Revision 1.3  2010/06/14 19:18:25  clivewebster
 * Add copyright license info
 *
 *
 * Copyright (C) 2010 Clive Webster (webbot@webbot.org.uk)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * sound2noise.c
 *
 *  Created on: 7 Jun 2010
 *      Author: Clive Webster
 */
#include "Text2Speech.h"

#if !defined(_WINDOWS_)
#include "../../timer.h"
#endif



static uint8_t default_pitch = 7;

// Random number seed
static uint8_t seed[3];

// The overall speed factor
static uint8_t timeFactor;

static const uint8_t SoundData[] PROGMEM= {
	// A - phoneme 'ae'
 0x74,0xFF,0xFF,0x04,0x30,0xFB,0xCF,0x88,0x88,0x78,0x45,0x95,0xCD,0x6A,0x44,0x85,
 0xAA,0x68,0x65,0x76,0x88,0x88,0x88,0x68,0x55,0x86,0xA9,0x8A,0x56,0x65,0x87,0xA8,
 0xCB,0x8A,0x56,0x85,0xBA,0x8A,0x78,0x87,0x88,0x88,0x88,0x66,0x55,0x76,0x88,0x56,
 0x54,0x65,0x66,0x56,0x55,0x44,0x54,0x66,0x56,0x55,0x65,0x56,0x54,0x86,0x46,0x44,

 // B - phoneme 'aa'
 0x96,0xEE,0xFF,0xED,0x44,0x10,0x52,0xD9,0xEF,0xBD,0x58,0x45,0x66,0x86,0x88,0x77,
 0xA8,0xBA,0x9B,0x48,0x44,0x53,0xA7,0xCB,0xAB,0x68,0x55,0x65,0x87,0x99,0x89,0x88,
 0x88,0xA9,0x99,0x88,0x78,0x98,0xA9,0x99,0x78,0x66,0x87,0x98,0x88,0x68,0x66,0x76,
 0x88,0x88,0x78,0x67,0x66,0x66,0x66,0x66,0x66,0x76,0x77,0x66,0x67,0x67,0x87,0x77,

 //C - phoneme 'ai'
 0x66,0x96,0xB8,0xFF,0xCB,0xFB,0x9A,0x85,0x73,0x78,0x58,0xBA,0x9D,0x99,0x89,0x88,
 0x44,0x64,0x58,0x86,0xA9,0xAA,0x88,0x86,0x58,0x54,0x76,0x87,0x88,0x99,0x9A,0x78,
 0x88,0x67,0x66,0x86,0x88,0x9A,0xCA,0xAA,0xA8,0x88,0x86,0x76,0x88,0x89,0x88,0x89,
 0x67,0x66,0x65,0x65,0x65,0x66,0x66,0x66,0x56,0x55,0x55,0x55,0x44,0x77,0x68,0x2B,

 //D - phoneme 'r'
 0x44,0x75,0xEA,0xFF,0xAC,0xDA,0xEE,0x5A,0x32,0x65,0x56,0x55,0x76,0xBA,0xCD,0x8B,
 0x87,0x99,0x48,0x22,0x53,0x77,0x66,0x97,0xAA,0xAB,0x8A,0x66,0x76,0x57,0x44,0x75,
 0x98,0x88,0x98,0xAA,0xBB,0xAB,0x89,0x87,0x88,0x56,0x75,0x88,0x88,0x88,0x88,0x68,
 0x56,0x55,0x55,0x55,0x55,0x55,0x66,0x56,0x66,0x56,0x55,0x55,0x44,0x65,0x66,0x55,

 //E - phoneme 'ux'
 0x44,0x54,0x55,0x55,0x76,0xB9,0xFD,0xFF,0xFF,0xEE,0xDE,0xCD,0xAB,0x68,0x45,0x23,
 0x32,0x44,0x44,0x55,0x76,0xA9,0xBA,0xBB,0xAB,0xAA,0x9A,0x89,0x68,0x55,0x44,0x44,
 0x54,0x65,0x66,0x87,0x98,0xA9,0xAA,0xAA,0x9A,0xA9,0xAA,0xAA,0x9A,0x89,0x88,0x88,
 0x88,0x88,0x66,0x55,0x55,0x55,0x55,0x45,0x44,0x54,0x55,0x55,0x45,0x44,0x44,0x54,

 //F - phoneme 'ao'
 0x55,0x65,0x66,0x67,0x87,0xB9,0xEC,0xFF,0xFF,0xCF,0x8A,0x24,0x01,0x31,0x75,0xDA,
 0xFE,0xFF,0xCE,0x8A,0x56,0x44,0x54,0x65,0x88,0x99,0xAA,0xAA,0xBB,0xBB,0xAA,0x89,
 0x57,0x45,0x54,0x75,0xA8,0xCB,0xDD,0xDD,0xBC,0x9A,0x68,0x56,0x55,0x65,0x87,0xA9,
 0xBB,0xBB,0x9A,0x88,0x66,0x55,0x55,0x66,0x87,0x98,0x99,0x99,0x88,0x67,0x56,0x55,

 //G - phoneme 'ee'
 0x35,0x43,0x07,0xE1,0xA4,0x79,0xBD,0xE8,0xFD,0xDA,0xCF,0xDB,0xFB,0x9A,0xAB,0x68,
 0x96,0x45,0x54,0x25,0x52,0x44,0x54,0x56,0x65,0x87,0x76,0x9A,0x98,0xAA,0x99,0xAA,
 0x89,0xA9,0x89,0x88,0x68,0x86,0x68,0x65,0x67,0x85,0x77,0x79,0xA9,0x87,0xAA,0xAA,
 0x9A,0xA9,0x98,0x88,0x88,0x67,0x66,0x55,0x55,0x55,0x54,0x55,0x44,0x45,0x54,0x34,

 //H - phoneme 'l'
 0x55,0x55,0x75,0xA8,0xDB,0xFE,0xEF,0xBE,0x8B,0x68,0x55,0x54,0x54,0x55,0x55,0x66,
 0x87,0x99,0xAA,0xAB,0xAA,0x89,0x66,0x55,0x55,0x55,0x65,0x76,0x87,0x88,0x99,0x99,
 0x99,0x9A,0x99,0x99,0x98,0x88,0x88,0x68,0x67,0x67,0x77,0x76,0x66,0x66,0x66,0x66,
 0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x55,

 //I - phoneme 'n'
 0x55,0x66,0x66,0x76,0x9A,0xA6,0xBB,0xCB,0xBB,0xDE,0xED,0xDD,0xDD,0xBC,0xBC,0xBA,
 0x9A,0x99,0x78,0x68,0x66,0x56,0x55,0x55,0x55,0x55,0x55,0x66,0x65,0x66,0x76,0x77,
 0x88,0x88,0x88,0x98,0x99,0xAA,0xAA,0xAA,0xBA,0xBA,0xAB,0xAA,0xAA,0x99,0x89,0x88,
 0x68,0x66,0x55,0x55,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x54,0x45,0x55,0x65,

 //J - phoneme 'm'
 0x32,0x43,0x34,0x43,0x44,0x55,0x55,0x65,0x76,0xB9,0xA8,0x9A,0xBB,0xCD,0xDD,0xDD,
 0xEE,0xED,0xDD,0xDC,0xBB,0xAB,0x99,0x89,0x88,0x66,0x56,0x55,0x45,0x44,0x54,0x55,
 0x55,0x55,0x65,0x66,0x87,0x88,0x88,0x99,0x99,0xAA,0xAA,0xBA,0xAB,0xBB,0xAB,0xBB,
 0xBB,0xAA,0x9A,0x89,0x88,0x78,0x66,0x56,0x55,0x44,0x44,0x34,0x33,0x33,0x34,0x43,

 //K - phoneme 'ah'
 0x55,0xD7,0xFF,0xBF,0x49,0x33,0x67,0x68,0x89,0xA9,0xDD,0x8B,0x24,0x22,0x95,0xAA,
 0x9A,0x89,0x99,0x58,0x34,0x54,0xA7,0xBB,0x8A,0x68,0x66,0x66,0x65,0x97,0xBA,0xAB,
 0x68,0x55,0x76,0x98,0x89,0x98,0x88,0x68,0x56,0x75,0x88,0x89,0x68,0x66,0x66,0x66,
 0x66,0x76,0x88,0x67,0x56,0x55,0x76,0x67,0x76,0x66,0x77,0x66,0x66,0x56,0x66,0x67,

 //L - phoneme 'v'
 0x99,0x99,0x99,0x78,0x77,0x77,0x67,0x66,0x77,0x77,0x77,0x98,0x99,0x99,0xA9,0xAA,
 0x99,0x99,0x99,0x78,0x77,0x77,0x67,0x66,0x77,0x77,0x77,0x98,0x99,0x99,0xA9,0xAA,
 0x89,0x99,0x99,0x99,0xAA,0x9A,0x99,0x99,0x89,0x77,0x77,0x77,0x66,0x76,0x77,0x77,
 0x87,0x99,0x99,0x99,0xAA,0x9A,0x99,0x99,0x89,0x77,0x77,0x77,0x66,0x76,0x77,0x77,

 //M - phoneme 'b'
 0x86,0x88,0x98,0x88,0x88,0x88,0x99,0x89,0x88,0x88,0x88,0x99,0x88,0x88,0x98,0x99,
 0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x78,0x88,0x87,0x77,0x77,
 0x88,0x88,0x77,0x87,0x88,0x88,0x77,0x66,0x66,0x66,0x56,0x55,0x55,0x55,0x66,0xB8,
 0xFE,0xFF,0xCE,0x9A,0x89,0x48,0x24,0x01,0x20,0x55,0x66,0x56,0x55,0x55,0x24,0x00,

 //N - phoneme '/h'
 0x88,0x88,0x68,0x86,0x78,0x97,0x89,0x88,0x76,0x76,0x88,0x89,0x99,0x88,0x68,0x56,
 0x87,0x99,0xA9,0x89,0x66,0x56,0x76,0x88,0x88,0x89,0x78,0x77,0x88,0x88,0x99,0x88,
 0x68,0x55,0x75,0xA9,0x9A,0x89,0x68,0x55,0x66,0x86,0xA9,0x9A,0x88,0x56,0x86,0x88,
 0x88,0x68,0x75,0x77,0x98,0xAA,0x98,0x89,0x58,0x55,0x66,0xB9,0xAB,0x8A,0x58,0x65,

 //O - phoneme 't'
 0x96,0x78,0x78,0xB9,0xB8,0x78,0x89,0x76,0x65,0x78,0x4B,0x87,0x75,0x8B,0xB5,0x76,
 0xA8,0xA9,0x44,0x5D,0x49,0x8B,0x83,0x95,0x78,0x1A,0x6C,0x7A,0xC2,0xB4,0x55,0x7B,
 0x85,0xA8,0x78,0x98,0x85,0x87,0xC4,0x85,0x85,0x3B,0xB8,0x47,0x78,0xA4,0x94,0x68,
 0x87,0x66,0x7A,0x7A,0x89,0x67,0xC8,0xA6,0x83,0x7B,0xC5,0xC1,0xB4,0x98,0x87,0xB3,

 //P - phoneme 'p'
 0x88,0x99,0x99,0xA9,0xDA,0xBC,0xED,0xCD,0xBB,0xCC,0xDC,0xFD,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x8C,0x04,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x32,0x45,0x13,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x11,0x10,0x11,0x11,0x32,0x54,0x55,0x76,0x76,

 //Q - phoneme 'k'
 0x99,0x58,0x65,0x88,0x48,0xA3,0xAB,0x76,0xC9,0x4A,0x21,0x84,0x69,0x44,0xC7,0x4A,
 0x52,0xA9,0x48,0x72,0xED,0x8B,0x78,0x55,0x24,0x32,0xF4,0xCF,0x10,0xFB,0x4F,0x54,
 0x98,0x68,0x74,0x88,0x56,0x85,0x89,0x56,0x87,0x78,0x76,0x98,0x99,0x99,0x89,0x68,
 0x76,0x67,0x65,0x98,0x78,0x44,0x54,0xB8,0xAB,0x69,0x55,0x76,0x88,0x56,0x65,0x98,

 //R - phoneme 'w'
 0x44,0x55,0x76,0xB9,0xDC,0xFE,0xFF,0xDE,0xCD,0xAB,0x9A,0x88,0x56,0x45,0x23,0x22,
 0x43,0x54,0x86,0x98,0xA9,0xAA,0x9A,0x99,0x99,0x89,0x88,0x56,0x45,0x44,0x54,0x55,
 0x66,0x76,0x87,0x88,0x98,0x99,0x99,0xA9,0x99,0xAA,0xAA,0xAA,0x9A,0x89,0x78,0x66,
 0x66,0x66,0x66,0x56,0x55,0x55,0x55,0x66,0x66,0x66,0x56,0x55,0x55,0x55,0x55,0x55,

 //S - phonene
 0x45,0xB6,0xFF,0xA5,0xFB,0x6A,0x84,0x95,0x56,0x73,0xD9,0x69,0xB8,0x8C,0x56,0x66,
 0x67,0x44,0xA6,0x8A,0x96,0xAA,0x68,0x65,0x76,0x46,0x75,0x99,0x88,0xA9,0x8A,0x67,
 0x76,0x67,0x65,0x97,0x88,0xC9,0x9A,0xA9,0xA9,0x59,0x65,0x89,0x55,0x98,0x88,0x67,
 0x66,0x46,0x54,0x66,0x55,0x86,0x56,0x76,0x56,0x55,0x56,0x65,0x45,0x87,0x58,0x75,

 //T - phoneme 'y'
 0x55,0x95,0x54,0x5C,0x87,0xA9,0xA8,0xAD,0xBB,0xBD,0x9D,0x9F,0xAB,0xAB,0x8A,0x8B,
 0x88,0x88,0x46,0x47,0x44,0x55,0x44,0x55,0x54,0x65,0x55,0x87,0x86,0x97,0x88,0x99,
 0x98,0xA9,0x99,0x9A,0x98,0x99,0x98,0x9A,0x99,0x99,0x98,0x8A,0x88,0x88,0x78,0x68,
 0x66,0x56,0x55,0x45,0x54,0x44,0x54,0x44,0x55,0x54,0x55,0x44,0x45,0x64,0x54,0x47,

 //U - phoneme 'ih'
 0xA4,0x58,0xD5,0x1F,0xD7,0x8F,0xD5,0x8F,0xA5,0x88,0x84,0x34,0x95,0x34,0xB6,0x5A,
 0xB7,0x6C,0x89,0x89,0x87,0x55,0x67,0x44,0x87,0x58,0x99,0x89,0x99,0x99,0x88,0x86,
 0x88,0x65,0x88,0x98,0x89,0xAB,0x9B,0xB9,0x89,0x86,0x58,0x65,0x55,0x54,0x55,0x67,
 0x66,0x88,0x66,0x87,0x55,0x56,0x64,0x46,0x75,0x46,0x84,0x59,0x75,0x89,0x74,0x67,

 //V - phoneme 'j'
 0x76,0x66,0x67,0x66,0x77,0x87,0x77,0x89,0x88,0x89,0x89,0x89,0xA8,0x99,0x9A,0xA9,
 0x9A,0xBA,0x9A,0x9A,0xA9,0x89,0x89,0x88,0x79,0x87,0x77,0x67,0x66,0x67,0x76,0x76,
 0x67,0x77,0x77,0x88,0x97,0x89,0x99,0x99,0xAA,0xA9,0x9A,0xAA,0xA9,0x9A,0x99,0x89,
 0x99,0x87,0x78,0x77,0x67,0x77,0x76,0x66,0x67,0x66,0x77,0x87,0x77,0x89,0x88,0x89,

 //W - phoneme 'd'
 0x3A,0xFB,0x98,0x6C,0xA9,0xA5,0x5A,0x89,0xA8,0x58,0x67,0x67,0x55,0x55,0x66,0x56,
 0x76,0x88,0x76,0x86,0x87,0x54,0x55,0x47,0x55,0x85,0x65,0x66,0x88,0x8A,0xA8,0xAA,
 0x89,0x98,0x89,0x67,0x88,0x89,0x98,0xBA,0xAB,0xBA,0xBC,0xAB,0xAA,0xAA,0x78,0x87,
 0x78,0x66,0x86,0x88,0x87,0x98,0x88,0x77,0x78,0x56,0x55,0x56,0x44,0x54,0x45,0x44,

 //X - phoneme 'nx'
 0x44,0xA6,0xAA,0xBB,0xCB,0xBD,0xBD,0xAB,0xAA,0xBA,0x9A,0x89,0x88,0x88,0x88,0x66,
 0x66,0x66,0x66,0x67,0x87,0x78,0x88,0x88,0x88,0x87,0x77,0x77,0x77,0x77,0x88,0x88,
 0x88,0x99,0xAA,0xAA,0xAA,0xAA,0x99,0x99,0x99,0x88,0x88,0x66,0x66,0x55,0x55,0x55,
 0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x65,0x56,0x55,0x55,0x55,0x55,0x45,

 //Y - phoneme 'oh'
 0x66,0x66,0xF7,0xFF,0x9E,0x5B,0x74,0x88,0x56,0x54,0x75,0xDB,0xBD,0x88,0x46,0x75,
 0x58,0x45,0x65,0xB7,0xCB,0x8A,0x68,0x56,0x76,0x56,0x54,0x87,0xBA,0xAB,0x78,0x77,
 0x66,0x66,0x56,0x96,0xDA,0xBD,0x8B,0x56,0x75,0x67,0x76,0x76,0x98,0x9A,0x88,0x66,
 0x65,0x77,0x66,0x56,0x66,0x87,0x66,0x55,0x65,0x66,0x56,0x65,0x66,0x77,0x88,0x67,

 //Z - phoneme 'o'
 0x95,0xDB,0xFF,0xFF,0x4A,0x02,0x00,0x73,0xFB,0xFF,0xAF,0x48,0x23,0x42,0x75,0x88,
 0x99,0x98,0xAA,0xAA,0x68,0x25,0x22,0x53,0xB8,0xED,0xBD,0x69,0x24,0x32,0x75,0xA9,
 0xAB,0x9A,0x88,0x88,0xA9,0x89,0x67,0x66,0x87,0x89,0x89,0x69,0x67,0x66,0x66,0x66,
 0x55,0x66,0x86,0x78,0x66,0x55,0x55,0x55,0x66,0x66,0x66,0x66,0x66,0x66,0x56,0x45,

 //[ - phoneme 'er'
 0x67,0xD7,0xFF,0xAB,0xCB,0xAC,0x89,0x26,0x52,0x87,0x56,0x86,0xBA,0xCB,0x8B,0x76,
 0x88,0x48,0x44,0x55,0x76,0x89,0x88,0xB9,0xAB,0x88,0x67,0x66,0x66,0x45,0x75,0x98,
 0x99,0x99,0x99,0xBA,0x8A,0x87,0x88,0x78,0x77,0x76,0x98,0x89,0x87,0x88,0x78,0x66,
 0x55,0x65,0x66,0x65,0x66,0x66,0x66,0x56,0x65,0x56,0x55,0x65,0x56,0x66,0x66,0x66,

 // phoneme - 'sh'
 0x8B,0x43,0x97,0x3B,0x93,0x4C,0x55,0x3F,0x74,0x59,0x58,0xC8,0x03,0xC7,0x26,0xC3,
 0x18,0x77,0x7A,0x34,0xC9,0x82,0xA1,0x2B,0x81,0x2D,0x76,0xA8,0x38,0x78,0x7A,0xE0,
 0x48,0xA6,0x83,0x47,0x3B,0xA5,0x3A,0x85,0x3B,0x98,0x64,0x89,0x56,0xB8,0x88,0x83,
 0x67,0x87,0x27,0x77,0x0D,0x73,0x5E,0x72,0xCC,0x70,0xD8,0x44,0x47,0x59,0x98,0x58,

 //] - phoneme 'g'
 0x76,0x87,0x88,0x98,0x99,0xAA,0xAA,0xAA,0xAA,0x99,0x89,0x88,0x88,0x77,0x66,0x66,
 0x55,0x45,0x41,0xB7,0xDC,0xBE,0x99,0xB9,0xBB,0x8A,0x45,0x43,0x54,0x55,0x24,0x42,
 0x65,0x76,0x56,0x54,0x65,0x77,0x66,0x86,0xCB,0xED,0xCE,0xBB,0xBA,0xAB,0x99,0x68,
 0x65,0x66,0x76,0x86,0x77,0x87,0x88,0x89,0x88,0x78,0x99,0x99,0x99,0x98,0x99,0x9A,

 // ^ - phoneme 's' (5d40)
 0xB6,0x75,0x5A,0x4B,0x99,0xB5,0x58,0x5B,0x89,0xA6,0x96,0x68,0x88,0xA6,0x78,0x4A,
 0x8B,0xA5,0xA5,0x4A,0x4B,0xB7,0xB4,0x7A,0x4C,0x87,0xC3,0x78,0x2C,0x4B,0xC5,0xA4,
 0x3A,0x5B,0x99,0x95,0x88,0x49,0xB7,0xB4,0x68,0x8A,0xA7,0xB5,0x78,0x4C,0x9A,0xA5,
 0x78,0x5A,0x87,0xB5,0x78,0x5C,0x6B,0xA6,0x78,0x5A,0xA7,0xB3,0x58,0x2D,0x89,0xD5,

 // _ - phomeme 'f'
 0x65,0x66,0x6A,0x66,0xA7,0x98,0x66,0x6A,0x66,0xA7,0xA6,0x67,0x66,0x68,0xA6,0x6A,
 0x67,0xA7,0x98,0x66,0x66,0xA7,0x9A,0xA5,0x86,0x66,0x77,0x68,0xA6,0x9A,0x59,0x66,
 0x89,0xA7,0x89,0x65,0x68,0x86,0xA6,0x76,0x68,0x86,0x66,0xAA,0x68,0x66,0x6A,0x76,
 0x86,0x76,0x7A,0x66,0xA6,0x6A,0x7A,0x66,0xA6,0x8A,0x76,0x7A,0x8A,0x95,0x6A,0xA6,

 // ` - phoneme 'z'
 0x89,0x89,0x89,0xA8,0x99,0x9A,0xA9,0x9A,0xBA,0x9A,0x9A,0xA9,0x89,0x89,0x88,0x79,
 0x87,0x77,0x67,0x66,0x67,0x76,0x76,0x67,0x77,0x77,0x88,0x97,0x89,0x99,0x99,0xAA,
 0xA9,0x9A,0xAA,0xA9,0x9A,0x99,0x89,0x99,0x87,0x78,0x77,0x67,0x77,0x76,0x66,0x67,
 0x66,0x77,0x87,0x77,0x89,0x88,0x89,0x89,0x89,0xA8,0x99,0x9A,0xA9,0x9A,0xBA,0x9A,

 //a - pause
 0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,
 0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,
 0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,
 0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,

 // b - phoneme 'th'
 0x88,0x88,0x87,0x88,0x79,0x86,0x88,0x76,0x86,0x86,0x66,0x66,0x76,0x78,0x98,0x88,
 0x8A,0x99,0x88,0x89,0x79,0x7A,0x8A,0x99,0x89,0xA8,0x98,0x98,0x68,0x88,0x89,0x79,
 0x78,0x78,0x88,0x86,0x88,0x67,0x68,0x87,0x87,0x85,0x67,0x7A,0x88,0x8A,0x8B,0x9A,
 0xA9,0x99,0xA9,0x98,0xA9,0xA9,0xA8,0x98,0xA8,0x96,0x86,0x76,0x76,0x78,0x76,0x66,

 // c - phoneme 'dh'
 0x67,0x98,0x99,0xA9,0xAA,0x9A,0xAA,0x89,0x88,0x78,0x67,0x76,0x67,0x76,0x87,0x88,
 0x99,0x9A,0xA9,0xAA,0x99,0x99,0x89,0x77,0x77,0x66,0x76,0x77,0x77,0x88,0x88,0xA9,
 0xAA,0xA9,0xAA,0x89,0x88,0x78,0x77,0x77,0x66,0x76,0x77,0x87,0x99,0x99,0xA9,0xAA,
 0x99,0x9A,0x89,0x88,0x77,0x66,0x87,0x99,0x99,0xAA,0xAA,0xA9,0x9A,0x88,0x88,0x77,

 // d - phoneme '/u`'
 0xD6,0xFC,0xCD,0x9C,0x88,0x88,0x56,0x44,0x76,0xBA,0xAB,0x99,0x89,0x78,0x56,0x44,
 0x65,0x98,0x99,0x99,0x99,0x89,0x56,0x45,0x64,0x87,0x88,0x98,0x99,0x99,0x68,0x55,
 0x65,0x86,0xA9,0xBB,0xAB,0x8A,0x68,0x56,0x66,0x87,0x88,0x88,0x78,0x56,0x55,0x55,
 0x55,0x65,0x66,0x66,0x56,0x66,0x55,0x55,0x56,0x45,0x65,0x87,0x88,0x66,0x55,0x55,

 // e - phoneme 'zh'
 0x98,0x89,0xAA,0x99,0xAA,0x89,0x89,0x67,0x77,0x66,0x77,0x76,0x88,0x97,0xA9,0xA9,
 0xAB,0x99,0x99,0x87,0x78,0x76,0x67,0x76,0x77,0x97,0x89,0xA9,0x9A,0xB9,0x99,0x99,
 0x78,0x88,0x66,0x76,0x66,0x87,0x88,0xA9,0x99,0xAA,0x99,0x9A,0x88,0x88,0x76,0x67,
 0x66,0x77,0x86,0x88,0x98,0x98,0x89,0xAA,0x99,0xAA,0x89,0x89,0x67,0x77,0x66,0x77,

 // f - phoneme 'eh'
 0x55,0x87,0xF4,0x8F,0x95,0x8F,0x23,0x67,0x86,0x75,0xFB,0x79,0xB8,0x29,0x53,0x76,
 0x66,0xB8,0x9C,0x78,0x88,0x45,0x65,0x87,0x97,0xAA,0x89,0x87,0x56,0x55,0x76,0x88,
 0x99,0x8A,0x68,0x66,0x55,0x76,0xAB,0x98,0xBA,0x58,0x65,0x56,0x76,0x99,0x89,0x88,
 0x68,0x55,0x66,0x66,0x98,0x88,0x77,0x66,0x55,0x56,0x66,0x66,0x86,0x57,0x87,0x56,

 // g - phoneme 'uh'
 0x21,0x32,0x44,0x44,0x64,0x86,0xAA,0x9A,0x77,0xB9,0xFD,0xFF,0xCD,0xBA,0xCB,0xCB,
 0x8A,0x55,0x65,0xA7,0xAA,0x89,0x98,0xB9,0xCC,0xAB,0x68,0x66,0x88,0x78,0x57,0x55,
 0x97,0xBA,0xAB,0x9A,0x99,0xAA,0x9A,0x68,0x66,0x86,0x98,0x99,0xDB,0xFD,0xFF,0xDF,
 0xAB,0x89,0x88,0x68,0x55,0x55,0x76,0x88,0x78,0x56,0x66,0x66,0x56,0x34,0x22,0x32

 //UNUSED
 //0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 //0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 //0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 //0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

typedef enum soundAction{
	ACTION_ADD_SH = 0,
    ACTION_ADD_TH,
	ACTION_ADD_S,
	ACTION_ADD_F,
	ACTION_NO_ACTION,
	ACTION_PAUSE,
	ACTION_FADE_IN
} SOUND_ACTION;

typedef struct strSoundIndex{
	uint8_t SoundNumber;	// 6 bits
	SOUND_ACTION action;	// 3 bits
	uint8_t byte2;			// 4 bits
} SOUND_INDEX;
static const SOUND_INDEX PROGMEM SoundIndex[] = {
									// Sound    Phoneme
	{ 0, ACTION_NO_ACTION,10},		// A
	{ 1, ACTION_NO_ACTION,10},		// B
	{ 2, ACTION_NO_ACTION,10},		// C
	{ 3, ACTION_NO_ACTION,10},		// D
	{ 4, ACTION_NO_ACTION, 8},		// E
	{ 5, ACTION_NO_ACTION,12},		// F
	{ 6, ACTION_NO_ACTION, 8},		// G
	{ 7, ACTION_NO_ACTION, 1},		// H
	{ 8, ACTION_NO_ACTION, 1},		// I
	{ 9, ACTION_NO_ACTION, 1},		// J
	{10, ACTION_NO_ACTION, 6},		// K
	{11, ACTION_ADD_F	 , 0},		// L			v (add 'f')
	{12, ACTION_PAUSE	 ,12},		// M
	{13, ACTION_FADE_IN	 , 0},		// N
	{14, ACTION_PAUSE	 ,12},		// O
	{15, ACTION_PAUSE	 ,12},		// P
	{16, ACTION_PAUSE	 ,12},		// Q
	{17, ACTION_NO_ACTION, 6},		// R
	{18, ACTION_NO_ACTION, 6},		// S
	{19, ACTION_NO_ACTION, 6},		// T
	{20, ACTION_NO_ACTION, 6},		// U
	{21, ACTION_ADD_SH,	   0},		// V			j (add 'sh')
	{22, ACTION_PAUSE	 ,12},		// W
	{23, ACTION_NO_ACTION, 4},		// X
	{24, ACTION_NO_ACTION, 8},		// Y
	{25, ACTION_NO_ACTION, 6},		// Z
	{26, ACTION_NO_ACTION,10},		// [
	{27, ACTION_FADE_IN	 , 0},		// '\\'
	{28, ACTION_PAUSE	 ,12},		// ]
	{29, ACTION_FADE_IN	 , 0},		// ^		     s
	{30, ACTION_FADE_IN	 , 0},		// _			 f
	{31, ACTION_ADD_S	 , 0},		// `			 z (add 's')
	{32, ACTION_NO_ACTION,10}, 		// a
	{33, ACTION_FADE_IN	 , 0},		// b			 th
	{34, ACTION_ADD_TH	 , 0},		// c			 dh (add 'th')
	{35, ACTION_NO_ACTION, 6},		// d
	{36, ACTION_ADD_SH	 , 0},		// e			zh (add 'sh')
	{37, ACTION_NO_ACTION, 8},		// f
	{38, ACTION_NO_ACTION, 8},		// g
	{32, ACTION_NO_ACTION,10},		// h
	{32, ACTION_NO_ACTION,10}		// i
};





#if !defined(_WINDOWS_)
	// PWM settings - set by 'speechInit'
	static uint16_t Volume[16];
	static const TimerCompare* channel;
	static const IOPin* pwmPin;
#else
	FILE * log_dest;
	void setLogFile(FILE * file){
		log_dest = file;
	}
#endif




#define roundup(x,y) (((x) + ((y)>>1))/(y))

#if !defined(_WINDOWS_)
void speechInit(const IOPin* pin){
	pwmPin = pin;
	timeFactor = roundup(cpu_speed,550000UL);

	// Use 20kHz PWM on the pin
	pwmInitHertz(pin,20000,50,null);

	// Find the compare values for volume levels 0-15
	channel = compareFromIOPin(pin);
	if(channel){
		const Timer* timer = compareGetTimer(channel);
		uint32_t top = timerGetTOP(timer)-1;

		for(int8_t v =0; v<16; v++){
			uint16_t delay = interpolateU(v, 0,15, 0,top);
			Volume[v] = delay;
		}
	}
}
#endif


static void sound(uint8_t b){
	b = (b & 15);

#ifdef _LOG_
	loggerSnd((char)(b+'0'));
#endif

#if !defined(_WINDOWS_)
	// Update PWM volume
	if(channel){
		compareSetThreshold(channel,Volume[b]);
	}
#endif

}

static void pause(uint8_t delay){
	#if !defined(_WINDOWS_)
	uint8_t r;
	for(r=timeFactor; r>0; r--){
		_delay_loop_1(delay);
	}
	#endif

}

static void delay(uint8_t d){
	#if defined(_LOG_) && defined(_WINDOWS_)
		loggerDelay(512UL * d);
	#endif

		while(d!=0){
		pause(0);	// 256
		pause(0);	// 256
		d--;
	}
}




/*
	Generate a random number
*/
static uint8_t random(void){
	uint8_t tmp = (seed[0] & 0x48) + 0x38;
	seed[0]<<=1;
	if(seed[1] & 0x80){
		seed[0]++;
	}
	seed[1]<<=1;
	if(seed[2] & 0x80){
		seed[1]++;
	}
	seed[2]<<=1;
	if(tmp & 0x40){
		seed[2]++;
	}
	return seed[0];
}




static uint8_t playTone(uint8_t soundNum,uint8_t soundPos,char pitch1, char pitch2, uint8_t count, uint8_t volume){
	const uint8_t* soundData = &SoundData[soundNum * 0x40];
	while(count-- > 0 ){
		uint8_t s;

		s = pgm_read_byte(&soundData[soundPos % 0x40]);

		sound((uint8_t)(s & volume));
//		sound(  (uint8_t)(  (s * (volume+1))  / 16)  );
		pause(pitch1);
		sound((uint8_t)((s>>4) & volume));
//		sound(  (uint8_t)(  ((s>>4) * (volume+1))  / 16)  );
		pause(pitch2);

		++soundPos;
	}
	return soundPos % 0x40;
}

static void play(uint8_t duration, uint8_t soundNumber){
	while(duration-- != 0){
		playTone(soundNumber,random(), 7,7, 10, 15);
	}
}


void setPitch(uint8_t pitch){
	default_pitch = pitch;
}

uint8_t getPitch(void){
	return default_pitch;
}





static void soundOff(void){
	#if !defined(_WINDOWS_)
//	sei();
	pwmSetDutyCycle(pwmPin,50);	// Set 50% duty cycle
	#endif
}

static void soundOn(void){
	#if !defined(_WINDOWS_)
	pwmSetDutyCycle(pwmPin,50);	// Set 50% duty cycle
//	cli();
	#endif

	// initialise random number seed
	seed[0]=0xecu;
	seed[1]=7;
	seed[2]=0xcfu;

}


void sayNoise(const char* sounds,const int8_t* modifier){

	// phonemes has list of sound bytes
	uint8_t
		phonemeIn,				// offset into text
		byte2,
		modifierIn;				// offset into stuff in modifier
	int8_t	punctuationPitchDelta;	// change in pitch due to fullstop or question mark
	SOUND_ACTION action;
	char phoneme;
	const SOUND_INDEX* soundIndex;
	uint8_t sound1Num;			// Sound data for the current phoneme
	uint8_t sound2Num;			// Sound data for the next phoneme
	uint8_t sound2Stop;			// Where the second sound should stop
	int8_t pitch1;			// pitch for the first sound
	int8_t pitch2;			// pitch for the second sound
	short i;
	uint8_t sound1Duration;		// the duration for sound 1. 0 to 255.

	#ifdef _LOG_
	{
		int p=0;
		loggerP(PSTR("Data:"));
		logger(sounds);
		loggerCRLF();
		loggerP(PSTR("Modifier:"));
		while(sounds[p]){
			uint8_t m = modifier[p];
			uint8_t h;
			h = m>>4;
			loggerc( (char)((h<=9) ? (h+'0') : (h-10+'A')) );	// hex char
			h=m & 15;
			loggerc( (char)((h<=9) ? (h+'0') : (h-10+'A')) );	// hex char
			loggerc(',');

			m = modifier[p+1];
			h = m>>4;
			loggerc( (char)((h<=9) ? (h+'0') : (h-10+'A')) );	// hex char
			h=m & 15;
			loggerc( (char)((h<=9) ? (h+'0') : (h-10+'A')) );	// hex char
			loggerc(',');
			p+=2;
		}
		loggerCRLF();
	}
#endif

	soundOn();

	// _630C
//		action=ACTION_NO_ACTION;
	punctuationPitchDelta=0;

	//Q19
	for(phonemeIn=0,modifierIn=0;sounds[phonemeIn]!=0; phonemeIn+=2, modifierIn+=2){
		uint8_t	duration;	// duration from text line
		uint8_t SoundPos;	// offset into sound data
		uint8_t fadeSpeed=0; // Fade speed between the two sounds. 0 to 255
		action=ACTION_NO_ACTION;

		phoneme=sounds[phonemeIn];
		if(phoneme=='z'){
			delay(15);
			goto nextPhoneme;
		}else if(phoneme=='#'){
			goto nextPhoneme;
		}else{

			// Collect info on sound 1
			soundIndex = &SoundIndex[phoneme - 'A'];
			sound1Num = pgm_read_byte(&soundIndex->SoundNumber);
			action = pgm_read_byte(&soundIndex->action);
			byte2 = pgm_read_byte(&soundIndex->byte2);

			duration = sounds[phonemeIn+1] - '0';	// Get duration from the input line
			if(duration!=1){
				duration<<=1;
			}
			duration += 6;							// scaled duration from the input line (at least 6)

			sound2Stop = 0x40 / 2;


			pitch1 = modifier[modifierIn];
			if(modifier[modifierIn + 1]==0 || pitch1==-1){
				pitch1 = 10;
				duration -= 6;
			}else if(modifier[modifierIn + 1]=='0' || duration==6){
				duration -= 6;
			}


			//q8
			pitch2 = modifier[modifierIn+2];
			if(modifier[modifierIn + 3]==0 || pitch2 == -1){
				pitch2 = 10;
			}


			if(action<ACTION_NO_ACTION){
				//sound1Num = 0;
				random();
				sound2Stop=(0x40 / 2)+2;
			}else if(action==ACTION_FADE_IN){
				uint8_t volume;					// volume mask
#ifdef _LOG_
//					loggerP(PSTR("\nA *******"));
//					logger_uint8(duration);
//					loggerCRLF();
#endif
				volume = (duration==6) ? 15 : 1;  /// volume mask
				for(duration <<= 2; duration>0; duration--){
					playTone(sound1Num,random(),8,12,17, volume);
					// Increase the volume
					if(++volume==16){
						volume = 15;	// full volume from now on
					}
				}
				goto nextPhoneme;

			}else if(action == ACTION_PAUSE){
				delay(25);
			}
		}


		// 6186
		pitch1 = pitch1 + default_pitch + punctuationPitchDelta;
		if(pitch1<1){
			pitch1=1;
		}

		pitch2 = pitch2 + default_pitch + punctuationPitchDelta;
		if(pitch2<1){
			pitch2=1;
		}

		// get next phoneme
		phoneme=sounds[phonemeIn + 2];

		if(phoneme==0 || phoneme=='z'){
			if(duration==1){
				delay(60);
			}
			phoneme='a';	// change to a pause
		}else{
			// s6
			if(byte2 != 1){
				// Get the average of byte2 for the two phonemes
				byte2 = (byte2 + pgm_read_byte(&SoundIndex[phoneme-'A'].byte2))>>1;
			}

			if(action < ACTION_NO_ACTION || pgm_read_byte(&SoundIndex[phoneme-'A'].action) != ACTION_NO_ACTION){
				phoneme ='a'; // change to a pause
			}
		}

		// S10 - 61e5
		sound2Num = pgm_read_byte(&SoundIndex[phoneme-'A'].SoundNumber);

		sound1Duration = 0x80;			// play half of sound 1
		if(sound2Num==sound1Num){
			byte2 = duration;
		}

		// S11
		if( (byte2>>1) == 0 ){
			sound1Duration = 0xff;				// play all of sound 1
		}else{
			// The fade speed between the two sounds
			fadeSpeed = (sound1Duration + (byte2>>1))/byte2;

			if(duration==1){
				sound2Stop = 0x40;				// dont play sound2
				sound1Duration = 0xff;			// play all of sound 1
				pitch1 = 12;
			}
		}

		SoundPos = 0;
		do{	// 6237
			uint8_t sound1Stop = (sound1Duration>>2) % 0x40;
			uint8_t sound1End = MIN(sound1Stop , sound2Stop);

			if( sound1Stop != 0 ){

#ifdef _LOG_
//					loggerP(PSTR("\nB Remaining="));
//					logger_uint8(duration);
//					loggerP(PSTR("Repeat min("));
//					logger_uint8(sound1Stop);
//					loggerP(PSTR(","));
//					logger_uint8(sound2Stop);
//					loggerP(PSTR(") times,Sound#="));
//					loggerc((char)(sound1Num+'A'));
//					loggerP(PSTR("["));
//					logger_uint8(SoundPos);
//					loggerP(PSTR("]\n"));
#endif
				SoundPos = playTone(sound1Num,SoundPos,pitch1,pitch1, sound1End, 15);
			}

			// s18
			if(sound2Stop != 0x40){
#ifdef _LOG_
//					loggerP(PSTR("\nC Remaining="));
//					logger_uint8(duration);
//					loggerP(PSTR("Repeat "));
//					logger_uint8((uint8_t)(sound2Stop-sound1End));
//					loggerP(PSTR(" times,Sound#="));
//					loggerc((char)(sound2Num+'A'));
//					loggerP(PSTR("["));
//					logger_uint8(SoundPos);
//					loggerP(PSTR("]\n"));
#endif

				SoundPos = playTone(sound2Num,SoundPos,pitch2,pitch2, (uint8_t)(sound2Stop - sound1End), 15);
			}

			//s23
			if(sound1Duration!=0xff && duration<byte2){
				// Fade sound1 out
				sound1Duration -= (sound1Duration>=fadeSpeed) ? fadeSpeed : sound1Duration;
			}


			// Call any additional sound
			if(action==ACTION_ADD_F){
				play(3,30);	// make an 'f' sound
			}else if(action==ACTION_ADD_S){
				play(3,29);	// make an 's' sound
			}else if(action==ACTION_ADD_TH){
				play(3,33);	// make a 'th' sound
			}else if(action==ACTION_ADD_SH){
				play(3,27);	// make a 'sh' sound
			}

		}while(--duration!=0);

nextPhoneme:

		// Scan ahead to find a '.' or a '?' as this will change the pitch
		punctuationPitchDelta=0;
		for(i=6; i>0; i--){
			char next = sounds[phonemeIn + (i * 2)];
			if(next=='i'){
				// found a full stop
				punctuationPitchDelta = 6 - i; // Lower the pitch. 0 .... 5
			}else if(next=='h'){
				// found a question mark
				punctuationPitchDelta = i - 6; // Raise the pitch 0 .... -5
			}
		}

		if(action == ACTION_PAUSE){
			delay(25);
		}


	} // next phoneme

	soundOff();

}
