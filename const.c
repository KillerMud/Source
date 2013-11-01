/***********************************************************************
 *                                                                     *
 * Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,     *
 * Michael Seifert, Hans-Henrik St�feldt, Tom Madsen and Katja Nyboe   *
 *                                                                     *
 * Merc Diku Mud improvements copyright (C) 1992, 1993 by              *
 * Michael Chastain, Michael Quan, and Mitchell Tse                    *
 *                                                                     *
 *   ROM 2.4 is copyright 1993-1998 Russ Taylor                        *
 *        Russ Taylor (rtaylor@hypercube.org)                          *
 *        Gabrielle Taylor (gtaylor@hypercube.org)                     *
 *        Brian Moore (zump@rom.org)                                   *
 *   By using this code, you have agreed to follow the terms of the    *
 *   ROM license, in the file Rom24/doc/rom.license                    *
 *                                                                     *
 ***********************************************************************
 *                                                                     *
 * KILLER MUD is copyright 1999-2012 Killer MUD Staff (alphabetical)   *
 *                                                                     *
 * Andrzejczak Dominik   (kainti@go2.pl                 ) [Kainti    ] *
 * Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor     ] *
 * Koper Tadeusz         (garloop@killer-mud.net        ) [Garloop   ] *
 * Pietrzak Marcin       (marcin.pietrzak@mud.pl        ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                       (laszla.niet@gmail.com         ) [Laszlo    ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: const.c 141 2013-08-04 08:47:34Z drake $
 * $HeadURL: svn+ssh://svn@killer.mud.pl/killer/branches/current/src/const.c $
 *
 */
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "interp.h"
#include "lang.h"
#include "music.h"

// �ycie
//
const char * condition_info_all [] =
{
    "{G�adnych �lad�w{x",
    "{Gzadrapania{x",
    "{glekkie rany{x",
    "{f�rednie rany{x",
    "{fci�kie rany{x",
    "{rogromne rany{x",
    "{rledwo stoi{x",
    "{rumiera{x"
};
const char * condition_info_construct [] =
{
    "{G�adnych �lad�w{x",
    "{Gzadrapania{x",
    "{glekkie uszkodzenia{x",
    "{f�rednie uszkodzenia{x",
    "{fci�kie uszkodzenia{x",
    "{rogromne uszkodzenia{x",
    "{rledwo stoi{x",
    "{runieruchomiony{x"
};

//polskie literki i inne gowno
char const pol_iso	[] = "����󶼿��ʣ�Ӧ��";

char const pol_win 	[] =
{ 0xB9, 0xE6, 0xEA, 0xB3, 0xF1, 0xF3, 0x9C, 0x9F, 0xBF,
  0xA5, 0xC6, 0xCA, 0xA3, 0xD1, 0xD3, 0x8C, 0x8F, 0xAF, 0x00 };

char const pol_nopl	[] = "acelnoszzACELNOSZZ";

char const lower_pol  [] =
{
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, /* ........ */
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, /* ........ */
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, /* ........ */
  0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, /* ........ */
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, /*  !"#$%&' */
  0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, /* ()*+,-./ */
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, /* 01234567 */
  0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, /* 89:;<=>? */
  0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, /* @abcdefg */
  0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, /* hijklmno */
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, /* pqrstuvw */
  0x78, 0x79, 0x7a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, /* xyz[\]^_ */
  0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, /* `abcdefg */
  0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, /* hijklmno */
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, /* pqrstuvw */
  0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, /* xyz{|}~. */
  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, /* ........ */
  0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, /* ........ */
  0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, /* ........ */
  0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x93, 0x9f, /* ........ */
  0xa0, 0xb1, 0xa2, 0xb3, 0xa4, 0xa5, 0xb6, 0xa7, /* .�.�..�. */
  0xa8, 0xa9, 0xaa, 0xab, 0xbc, 0xad, 0xae, 0xbf, /* ....�..� */
  0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, /* .�.�..�. */
  0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, /* ....�..� */
  0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xe6, 0xc7, /* ......�. */
  0xc8, 0xc9, 0xea, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, /* ..�..... */
  0xd0, 0xf1, 0xd2, 0xf3, 0xd4, 0xd5, 0xd6, 0xd7, /* .�.�.... */
  0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, /* ........ */
  0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, /* ......�. */
  0x68, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, /* ..�..... */
  0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, /* .�.�.... */
  0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff  /* ........ */
};

char const upper_pol  [] =
{
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, /* ........ */
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, /* ........ */
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, /* ........ */
  0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, /* ........ */
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, /*  !"#$%&' */
  0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, /* ()*+,-./ */
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, /* 01234567 */
  0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, /* 89:;<=>? */
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, /* @ABCDEFG */
  0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, /* HIJKLMNO */
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, /* PQRSTUVW */
  0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, /* XYZ[\]^_ */
  0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, /* `ABCDEFG */
  0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, /* HIJKLMNO */
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, /* PQRSTUVW */
  0x58, 0x59, 0x5a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, /* XYZ{|}~. */
  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, /* ........ */
  0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, /* ........ */
  0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, /* ........ */
  0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x93, 0x9f, /* ........ */
  0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, /* .�.�..�. */
  0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, /* ....�..� */
  0xb0, 0xa1, 0xb2, 0xa3, 0xb4, 0xb5, 0xa6, 0xb7, /* .�.�..�. */
  0xb8, 0xb9, 0xba, 0xbb, 0xac, 0xbd, 0xbe, 0xaf, /* ....�..� */
  0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, /* ......�. */
  0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, /* ..�..... */
  0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, /* .�.�.... */
  0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, /* ........ */
  0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xc6, 0xe7, /* ......�. */
  0x68, 0xe9, 0xca, 0xeb, 0xec, 0xed, 0xee, 0xef, /* ..�..... */
  0xf0, 0xd1, 0xf2, 0xd3, 0xf4, 0xf5, 0xf6, 0xf7, /* .�.�.... */
  0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff  /* ........ */
};

char const upper_nopol  [] =
{
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, /* ........ */
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, /* ........ */
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, /* ........ */
  0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, /* ........ */
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, /*  !"#$%&' */
  0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, /* ()*+,-./ */
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, /* 01234567 */
  0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, /* 89:;<=>? */
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, /* @ABCDEFG */
  0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, /* HIJKLMNO */
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, /* PQRSTUVW */
  0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, /* XYZ[\]^_ */
  0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, /* `ABCDEFG */
  0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, /* HIJKLMNO */
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, /* PQRSTUVW */
  0x58, 0x59, 0x5a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, /* XYZ{|}~. */
  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, /* ........ */
  0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, /* ........ */
  0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, /* ........ */
  0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x93, 0x9f, /* ........ */
  0xa0, 0x41, 0xa2, 0x4c, 0xa4, 0xa5, 0x53, 0xa7, /* .A.L..S. */
  0xa8, 0xa9, 0xaa, 0xab, 0x5a, 0xad, 0xae, 0x5a, /* ....Z..Z */
  0xb0, 0x41, 0xb2, 0x4c, 0xb4, 0xb5, 0x53, 0xb7, /* .A.L..S. */
  0xb8, 0xb9, 0xba, 0xbb, 0x5a, 0xbd, 0xbe, 0x5a, /* ....Z..Z */
  0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0x43, 0xc7, /* ......C. */
  0xc8, 0xc9, 0x45, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, /* ..E..... */
  0xd0, 0x4e, 0xd2, 0x4f, 0xd4, 0xd5, 0xd6, 0xd7, /* .N.O.... */
  0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, /* ........ */
  0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0x43, 0xe7, /* ......C. */
  0x68, 0xe9, 0x45, 0xeb, 0xec, 0xed, 0xee, 0xef, /* ..E..... */
  0xf0, 0x4e, 0xf2, 0x4f, 0xf4, 0xf5, 0xf6, 0xf7, /* .N.O.... */
  0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff  /* ........ */
};

char const isoize_pol  [] =
{
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, /* ........ */
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, /* ........ */
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, /* ........ */
  0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, /* ........ */
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, /*  !"#$%&' */
  0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, /* ()*+,-./ */
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, /* 01234567 */
  0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, /* 89:;<=>? */
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, /* @ABCDEFG */
  0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, /* HIJKLMNO */
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, /* PQRSTUVW */
  0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, /* XYZ[\]^_ */
  0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, /* `abcdefg */
  0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, /* hijklmno */
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, /* pqrstuvw */
  0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, /* xyz{|}~. */
  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, /* ........ */
  0x88, 0x89, 0x8a, 0x8b, 0xa6, 0x8d, 0x8e, 0xac, /* ....�..� */
  0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, /* ........ */
  0x98, 0x99, 0x9a, 0x9b, 0xb6, 0x9d, 0x93, 0xbc, /* ....�..� */
  0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa1, 0xa6, 0xa7, /* .�.�.��. */
  0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, /* ....�..� */
  0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, /* .�.�..�. */
  0xb8, 0xb1, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, /* .�..�..� */
  0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, /* ......�. */
  0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, /* ..�..... */
  0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, /* .�.�.... */
  0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, /* ........ */
  0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, /* ......�. */
  0x68, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, /* ..�..... */
  0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, /* .�.�.... */
  0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff  /* ........ */
};

/*pogoda and stuff*/
WEATHER_DATA  weather_info[SECT_MAX];
/*****************/
const struct poison_type poison_table[MAX_POISON+1] =
{
	{
	"trucizna spowolniona",
	{ 0, 0, 0 },
	{ 0, 0, 1 },
	POISON_SLOWERED,
	{ 1, 4, 0 },
	},

	{
	"s�aba trucizna",
	{ 1,  5,  0 },
	{ 2,  5,  5 },
	POISON_WEAK,
	{ 2, 4, 1 },
	},

	{
	"�rednia trucizna",
	{ 2,  5,  10 },
	{ 4,  5,  30 },
	POISON_NORMAL,
	{ 0, 0, -1 },
	},

	{
	"silna trucizna",
	{ 4,  5,  20 },
	{ 5,  5,  60 },
	POISON_STRONG,
	{ 0, 0, -1 },
	},

	{
	"bardzo silna trucizna",
	{  4,  5, 45 },
	{  5,  5, 80 },
	POISON_STRONGEST,
	{ 0, 0, -1 },
	},

	{//5
	"trucizna usypiaj�ca",
	{  0,  0, 0 },
	{  0,  0, 0 },
	POISON_SLEEP,
	{  2, 4, 0  },
	},

	{
	"trucizna o�lepiaj�ca",
	{  1, 5, 0 },
	{  2, 5, 10 },
	POISON_BLIND,
	{ 0, 0, -1 },
	},

	{
	"trucizna parali�uj�ca",
	{  3, 5, 0 },
	{  3, 5, 20 },
	POISON_PARALIZING,
	{ 0, 0, -1 },
	},

	{//8
	"trucizna zabijaj�ca",
	{  4, 5, 30 },
	{  0, 0, 0 },
	POISON_DEATH,
	{ 0, 0, -1 },
	},

	{
	"trucizna os�abiaj�ca",
	{  1,  5, 0 },
	{  2,  5, 0 },
	POISON_WEAKEN,
	{ 0, 0, -1 },
	}
};



const struct school_type school_table[MAX_SCHOOL]=
{
	{/* 0 */
	"Odrzucanie",
	"Mistrz Odrzucania",
	"Mistrzyni Odrzucania",
	"Arcymistrz Odrzucania",
	"Arcymistrzyni Odrzucania",
	Odrzucanie,
	Przemiany|Iluzje,
	{1,0,0}
	},

	{/* 1 */
	"Przemiany",
	"Mistrz Przemian",
	"Mistrzyni Przemian",
	"Alchemik",
	"Alchemiczka",
	Przemiany,
	Odrzucanie|Nekromancja,
	{1,5,0},
	},

	{/* 2 */
	"Przywo�ania",
	"Mistrz Przywo�a�",
	"Mistrzyni Przywo�a�",
	"Wielki Przywo�ywacz",
	"Wielka Pani Przywo�a�",
	Przywolanie,
	Poznanie|Inwokacje,
	{1,5,0},
	},

	{/* 3 */
	"Poznanie",
	"Mistrz Poznania",
	"Mistrzyni Poznania",
	"M�drzec",
	"Wiedz�ca",
	Poznanie,
	Przywolanie,
	{1,5,2},
	},

	{/* 4 */
	"Zauroczenie",
	"Mistrz Zaurocze�",
	"Mistrzyni Zaurocze�",
	"Zaklinacz",
	"Pani Urok�w",
	Zauroczenie,
	Inwokacje|Nekromancja,
	{1,5,2},
	},

	{/* 5 */
	"Iluzje",
	"Mistrz Iluzji",
	"Mistrzyni Iluzji",
	"Iluzjonista",
	"Iluzjonistka",
	Iluzje,
	Nekromancja|Inwokacje|Odrzucanie,
	{1,4,0},
	},

	{/* 6 */
	"Inwokacje",
	"Mistrz Inwokacji",
	"Mistrzyni Inwokacji",
	"Mistyk",
	"Kszta�tuj�ca Energi�",
	Inwokacje,
	Zauroczenie|Przywolanie,
	{1,0,0},
	},

	{/* 7 */
	"Nekromancja",
	"Mistrz Nekromancji",
	"Mistrzyni Nekromancji",
	"Nekromanta",
	"W�adczyni Nieumar�ych",
	Nekromancja,
	Iluzje|Zauroczenie,
	{1,0,0},
	}
};

const struct armor_type armor_table[]=
{                                         //Mg,Kl,z�,wo,dr,pl,br,mni,brd,cz,sz
	{"pikowana",             {9, 9, 9, 10}, { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } },
	{"sk�rzana",             {9, 9, 8, 10}, { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } },
	{"�wiekowana sk�rzana",  {7, 8, 7, 10}, { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } },
	{"naszywanica",          {7, 7, 7, 10}, { 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1 } },

	{"�uskowa",              {6, 7, 6, 10}, { 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1 } },
	{"przeszywanica",        {6, 6, 6, 10}, { 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1 } },
	{"sk�ry zwierz�ce",      {6, 8, 6, 10}, { 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1 } },
	{"lekka kolczuga",       {6, 8, 5, 10}, { 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1 } },

	{"zwyk�a kolczuga",      {5, 7, 4, 10}, { 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1 } },
	{"paskowa",              {4, 5, 3, 10}, { 0, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0 } },
	{"pancerz z br�zu",      {4, 4, 4, 10}, { 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0 } },
	{"wzmacniana sk�rzana",  {4, 5, 4, 10}, { 0, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0 } },

	{"p�p�ytowa",           {3, 3, 3,  8}, { 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0 } },
	{"p�p�ytowa wzmaciana", {2, 3, 1,  8}, { 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0 } },
	{"p�ytowa",              {1, 2, 0,  8}, { 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0 } },
	{"p�ytowa wzmacniana",   {0, 0, 0,  7}, { 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0 } },

	{NULL, {0}, {0}}
};
const SECT_DATA	sector_table[]=
{
    /* UWAGA: Nie zmieniac kolejnosci!
       mem_druid_bonus - wp�ywa r�wnie� na "bonus" utraty move przez druida

       przy dodawniu nowego typu lokacji doda� w skillu do_hide (act_move.c) oraz mapuj (act_info.c) ew. do do_illuminate (act_obj.c)
       Name, mv_loss, heal_bonus, mem_bonus, mem_druid_bonus, min_dam, max_dam, wait, flag, herb_reset_time(WAZNE!!! musi byc mniejsze od 100), max_herb (max = 5)
     */
    {"wewn�trz",            1,   0,  0, -1,  0,   0, 1, SECT_NOWEATHER,                                                            60, 0},
    {"miasto",              1,   0,  0, -1,  0,   0, 1, SECT_LIGHT,                                                                60, 0},
    {"pole",                2,   0,  0,  1,  0,   0, 1, SECT_DRUID,                                                                20, 4},
    {"las",                 3,  10,  1,  2,  0,   0, 2, SECT_DRUID,                                                                10, 5},
    {"wzg�rza",             4,   0,  0,  1,  0,   0, 2, SECT_DRUID,                                                                20, 4},
    /*5*/
    {"g�ry",                6,   0,  0,  2,  0,   0, 3, SECT_DRUID,                                                                40, 2},
    {"woda p�yw",           4, -20,  0,  0,  0,   0, 2, SECT_NOMEM|SECT_NOREST|SECT_WATER|SECT_DRUID,                              50, 2},
    {"woda niep",           6, -30,  0,  0,  0,   0, 2, SECT_NOMEM|SECT_NOREST|SECT_WATERN|SECT_WATER|SECT_DRUID,                  60, 1},
    {"nieu�ywany",          1,   0,  0,  0,  0,   0, 0, 0,                                                                          0, 0},
    {"powietrze",           3, -30, -2,  0,  0,   0, 2, SECT_NOMEM|SECT_NOREST|SECT_AIR|SECT_DRUID,                                 0, 0},
    /*10*/
    {"pustynia",            7, -40, -3,  1,  0,   0, 4, SECT_THIRST|SECT_DRUID|SECT_HOT,                                           80, 1},
    {"puszcza",             6,  10,  1,  2,  0,   0, 3, SECT_DRUID,                                                                10, 5},
    {"bagno",               5, -20, -1,  0,  0,   0, 2, SECT_NOMEM|SECT_NOREST|SECT_DRUID,                                         20, 4},
    {"wys g�ry",            8, -10, -1,  2,  0,   0, 4, SECT_DRUID,                                                                55, 1},
    {"podziemia",           3,   0,  0,  1,  0,   0, 2, SECT_NOWEATHER,                                                            40, 2},
    /*15*/
    {"jaskinia",            3,  10,  0,  1,  0,   0, 1, SECT_NOWEATHER|SECT_DRUID,                                                 45, 2},
    {"tundra",              5, -20, -1,  2,  0,   0, 3, SECT_DRUID|SECT_COLD,                                                      40, 3},
    {"podziemia naturalne", 3,   0,  0,  1,  0,   0, 2, SECT_NOWEATHER|SECT_DRUID,                                                 55, 1},
    {"droga",               1,   0,  0,  0,  0,   0, 1, 0,                                                                         40, 2},
    {"rzeka",               3,   0,  0,  0,  0,   0, 2, SECT_NOMEM|SECT_NOREST|SECT_WATER|SECT_CANDRINK|SECT_DRUID,                35, 2},
    /*20*/
    {"jezioro",             5,   0,  0,  0,  0,   0, 2, SECT_NOMEM|SECT_NOREST|SECT_WATER|SECT_CANDRINK|SECT_DRUID,                40, 2},
    {"morze",               5,   0,  0,  0,  0,   0, 2, SECT_THIRST|SECT_NOMEM|SECT_NOREST|SECT_WATER|SECT_DRUID,                  50, 1},
    {"ocean",               5,   0,  0,  0,  0,   0, 2, SECT_THIRST|SECT_NOMEM|SECT_NOREST|SECT_WATER|SECT_DRUID,                  60, 1},
    {"lawa",                8, -40,  0,  0, 75, 200, 3, SECT_THIRST|SECT_NOMEM|SECT_NOREST|SECT_DRUID|SECT_HOT,                     0, 0},
    {"ruchome piaski",      9, -40,  0,  0,  0,   0, 5, SECT_THIRST|SECT_NOMEM|SECT_NOREST|SECT_DRUID,                              0, 0},
    /*25*/
    {"gor�ce �r�d�a",       3,  20,  1,  1,  0,   0, 3, SECT_CANDRINK|SECT_DRUID|SECT_HOT,                                          0, 0},
    {"lodowiec",            7, -20, -2,  2,  0,   0, 3, SECT_DRUID|SECT_COLD,                                                      80, 1},
    {"arktyczny l�d",       5, -10, -1,  1,  0,   0, 3, SECT_DRUID|SECT_COLD,                                                      90, 1},
    {"�cie�ka",             2,   0,  0,  0,  0,   0, 1, 0,                                                                         35, 2},
    {"pla�a",               3,  20,  1,  1,  0,   0, 2, SECT_DRUID,                                                                50, 1},
    /* 30 */
    {"eden",                1,  50,  3,  3,  3,   0, 2, SECT_LIGHT|SECT_DRUID|SECT_HOT,                                            10, 5},
    {"sawanna",             3,   0,  0,  0,  1,   0, 2, SECT_DRUID,                                                                25, 3},
    {"trawa",               3,   0,  0,  0,  1,   0, 1, SECT_DRUID,                                                                15, 5},
    {"step",                2,   0,  0,  0,  0,   0, 1, SECT_DRUID,                                                                30, 2},
    {"pod wod�",            6,   0,  0,  0,  0,   0, 4, SECT_NOMEM|SECT_NOREST|SECT_UNDERWATER|SECT_NOWEATHER|SECT_DRUID,          50, 2},
    /* 35 */
    {"g�rska �cie�ka",      4,   0,  0,  1,  0,   0, 3, SECT_DRUID,                                                                60, 1},
    {"plac",                2,   0,  0,  0,  0,   0, 1, 0,                                                                         80, 1},
    {"ciemna woda",         6,   0,  0,  0,  0,   0, 4, SECT_NOMEM|SECT_NOREST|SECT_UNDERWATER|SECT_NOWEATHER|SECT_DRUID,           0, 0},
    {"park",                2,  10,  1,  1,  0,   0, 1, SECT_DRUID,                                                                20, 4},
    {"podziemne jezioro",   5,   0,  0,  0,  0,   0, 2, SECT_NOMEM|SECT_NOREST|SECT_WATER|SECT_CANDRINK|SECT_NOWEATHER|SECT_DRUID, 60, 1},
    /* 40 */
    {"wydmy",               3,   0,  0,  1,  0,   0, 2, SECT_THIRST|SECT_DRUID|SECT_HOT,                                           55, 1},
    {"arena",               1,   0,  0,  0,  0,   0, 1, SECT_NOWEATHER,                                                             0, 0},
    {"��ka",                2,   0,  0,  1,  0,   0, 1, SECT_DRUID,                                                                 5, 5},
    {"stroma �cie�ka",      3,   0,  0,  1,  0,   0, 3, SECT_DRUID,                                                                60, 0},
    {"b�otna �cie�ka",      4,  -1,  0,  1,  0,   0, 3, SECT_DRUID,                                                                60, 0},
    /* 45 */
    {"podziemna droga",     1,   0,  0,  0,  0,   0, 1, SECT_NOWEATHER,                                                            40, 1},
    {"pustynna droga",      3, -30, -2,  1,  0,   0, 4, SECT_THIRST|SECT_DRUID|SECT_HOT,                                           80, 0},
    {"kopalnia",            4,   0,  0,  0,  0,   0, 2, SECT_NOWEATHER,                                                             0, 0},
    {"ruiny",               4,   0,  0,  0,  0,   0, 3, 0,                                                                          0, 0},
    {  NULL,0,0,0,0,0,0,0,0 }
};

const struct material_type material_table[] =
{
 /*
   NAME
   PRZYMIOTNIK
   HARDNESS czyli jak twardy jest material, istotne przy trafieniu czy bron/zbroja ulegla zniszczeniu
   FIRE
   ACID
   ELECTR
   COLD
   OTHER


 UWAGA: nie zmienia� kolejno�ci!

 Dodaj�c metalowy, rdzewiej�cy materia�, prosz� doda� go czaru spell_corrode
 w pliku spells_dru.c

 */

/*0*/
 { "unused",         "",               1,   1,   1,   1,   1,   1,   0 },
 { "drewno",         "drewniany",     10,   0,  20,  20,  45,  10,   0 },
 { "stal",           "stalowy",       60,  75,  35, 100, 100,  25,   MAT_METAL },
 { "elfia stal",     "stalowy",       75,  85,  55, 100, 100,  25,   MAT_METAL },
 { "doskona�a stal", "stalowy",       65,  80,  45, 100, 100,  25,   MAT_METAL },
/*5*/
 { "mied�",          "miedziany",     40,  65,   5, 100, 100,  25,   MAT_METAL },
 { "sk�ra",          "sk�rzany",      20,  35,   5,  75,  30,  10,   0 },
 { "adamantyt",      "adamantytowy",  92,  95,  70, 100, 100,  30,   MAT_METAL },
 { "mithril",        "mithrilowy",    90,  95,  85, 100, 100,  35,   MAT_METAL },
 { "z�oto",          "z�oty",         25,  45,  25, 100, 100,  25,   MAT_METAL },
/*10*/
 { "srebro",         "srebrny",       35,  55,  15, 100, 100,  25,   MAT_METAL },
 { "kryszta�",       "kryszta�owy",   70,  95, 100, 100, 100,  45,   MAT_EASYBREAK },
 { "nieznany",       "",               1,   0,   0,   0,   0,   0,   0 },
 { "�elazo",         "�elazny",       45,  75,  10, 100, 100,  20,   MAT_METAL },
 { "br�z",           "br�zowy",       45,  65,   5, 100, 100,  25,   MAT_METAL },
/*15*/
 { "szk�o",          "szklany",       55, 100, 100, 100, 100,  15,   MAT_EASYBREAK },
 { "kamie�",         "kamienny",      35,  95, 100, 100, 100,  50,   0 },
 { "jedzenie",       "",               0,   0,   0,   0,   0,   0,   0 },
 { "jedwab",         "jedwabny",       5,   0,   0,   0,  50,   0,   0 },
 { "glina",          "gliniany",       5,   0,   0,   0,  50,   0,   0 },
/*20*/
 { "papier",         "papierowy",      5,   0,   0,   0,  50,   0,   0 },
 { "ko��",           "ko�ciany",      35,  40,  10,  90,  60,  10,   0 },
 { "marmur",         "marmurowy",     20,  95, 100, 100, 100,  50,   0 },
 { "granit",         "granitowy",     30,  95, 100, 100, 100,  50,   0 },
 { "p��tno",         "p��cienny",      5,   0,   0,   0,  50,   0,   0 },
/*25*/
 { "len",            "lniany",         5,   0,   0,   0,  50,   0,   0 },
 { "we�na",          "we�niany",       5,   0,   0,   0,  50,   0,   0 },
 { "krzemie�",       "krzemienny",    35,  95, 100, 100, 100,  50,   0 },
 { "o��w",           "o�owiany",      20,  45,  15,  55, 100,  25,   MAT_METAL },
 { "filc",           "filcowy",       10,   0,   0,   0,  60,   0,   0 },
/*30*/
 { "kaszmir",          "kaszmirowy",     5,   0,   0,   0,  40,   0,   0 },
 { "porcelana",        "porcelanowy",   50, 100, 100, 100, 100,  15,   MAT_EASYBREAK },
 { "damasce�ska stal", "stalowy",       85,  90,  55, 100, 100,  35,   MAT_METAL },
 { "illit",            "illittowy",     91,  65,  10,  60, 100,  35,   MAT_METAL },
 { "platyna",          "platynowy",     45,  70,  10,  95,  95,  30,   MAT_METAL },
/*35*/
 { "at�as",            "at�asowy",       5,   0,   0,   5,  40,   0,   0 },
 { "diament",          "diamentowy",   100,  95, 100, 100, 100,  45,   0 },
 { "trzcina",          "trzcinowy",      5,   0,  15,  15,  40,   5,   0 },
 { "pergamin",         "pergaminowy",    5,   0,   0,   0,  50,   0,   0 },
 { "ro�lina",          "ro�linny",       5,   0,  15,  15,  40,   5,   0 },
/*40*/
 { "chityna",          "chitynowy",     30,  40,  25,  85,  40,  25,   0 },
 { "smocze �uski",     "smoczy",        65,  80,  40, 100, 100,  70,   0 },
 { "smoczy z�b",       "smoczy",        80, 100,  50, 100, 100,  80,   0 },
 { "mosi�dz",          "mosi�ny",      35,  60,   0, 100,  95,  25,   MAT_METAL },
 { "mocna sk�ra",      "sk�rzany",      25,  40,  20,  85,  40,  20,   0 },
/*45*/
 { "eteryczny",        "eteryczny",     30,  75,  75,  75,  75,  80,   0 },
 { "konopie",          "konopny",        5,   0,   5,   5,  60,   0,   0 },
 { "w�osie",           "",		         5,   0,   5,   5,  60,   0,   0 },
 { "drewno d�bowe",    "d�bowy",        40,   5,  25,  30,  60,  20,   0 },
 { "stalodrzew",       "stalodrzewny",  50,  30,  35,  45,  80,  40,   0 },
/*50*/
 { "bursztyn",         "bursztynowy",   15,  10,  75,  95,  40,  30,   0 },
 { "pierze",           "",               5,   0,   5,   5,   5,  50,   0 },
 { "luska",            "",              50,   0,  25,   5,   0,   0,   0 },
/**
 * jezeli dodajesz material, dodaj tez obsluge w spells_mag.c dla czaru
 * tworzacego golemy: spell_changestaff
 */
 { NULL,               NULL,             0,   0,   0,   0,   0,   0,   0 }
};


/* item type list */
const struct item_type		item_table	[]	=
{
	{	ITEM_LIGHT,			"light"		},
	{	ITEM_SCROLL,		"scroll"	},
	{	ITEM_WAND,			"wand"		},
	{  	ITEM_STAFF,			"staff"		},
	{ 	ITEM_WEAPON,		"weapon"	},
	{  	ITEM_TREASURE,		"treasure"	},
	{  	ITEM_ARMOR,			"armor"		},
	{	ITEM_POTION,		"potion"	},
	{	ITEM_CLOTHING,		"clothing"	},
	{   ITEM_FURNITURE,		"furniture"	},
	{	ITEM_TRASH,			"trash"		},
	{	ITEM_CONTAINER,		"container"	},
	{	ITEM_DRINK_CON,		"drink"		},
	{	ITEM_KEY,			"key"		},
	{	ITEM_FOOD,			"food"		},
	{	ITEM_MONEY,			"money"		},
	{	ITEM_BOAT,			"boat"		},
	{	ITEM_CORPSE_NPC,	"npc_corpse"},
	{	ITEM_CORPSE_PC,		"pc_corpse"	},
	{ 	ITEM_FOUNTAIN,		"fountain"	},
	{	ITEM_PILL,			"pill"		},
	{	ITEM_PROTECT,		"protect"	},
	{	ITEM_MAP,			"map"		},
	{	ITEM_PORTAL,		"portal"	},
	{	ITEM_ROOM_KEY,		"room_key"	},
	{	ITEM_GEM,			"gem"		},
	{	ITEM_JEWELRY,		"jewelry"	},
	{  	ITEM_JUKEBOX,		"jukebox"	},
	{  	ITEM_BOARD,			"board"		},
	{  	ITEM_PIECE,			"piece"		},
	{ 	ITEM_SPELLBOOK,		"spellbook"	},
	{  	ITEM_SPELLITEM,		"spellitem"	},
	{  	ITEM_ENVENOMER,		"envenomer"	},
	{ 	ITEM_BANDAGE,		"bandage"	},
	{ 	ITEM_TURN,			"turn_item"	},
	{  	ITEM_HERB,			"herb"		},
	{  	ITEM_SKIN,			"skin"		},
	{	ITEM_HORN,			"horn"		},
	{	ITEM_MUSICAL_INSTRUMENT, "musical_instrument"	},
	{	ITEM_SHIELD, 		"shield"	},
	{	ITEM_TROPHY, 		"trophy"	},
	{	ITEM_PIPE,			"pipe"		},
	{	ITEM_WEED,			"weed"		},
	{	ITEM_NOTEPAPER,			"notepaper"		},
	{ ITEM_TOOL, "tool" },
	{ ITEM_TOTEM, "totem" },
	{	0,		NULL		}
};


/* weapon selection table */
const	struct	weapon_type	weapon_table	[]	=
{
   { "sword",	WEAPON_SWORD,		&gsn_sword },
   { "mace",	WEAPON_MACE,		&gsn_mace },
   { "dagger",	WEAPON_DAGGER,		&gsn_dagger },
   { "axe",		WEAPON_AXE,			&gsn_axe },
   { "staff",	WEAPON_SPEAR,		&gsn_spear },
   { "flail",	WEAPON_FLAIL,		&gsn_flail },
   { "whip",	WEAPON_WHIP,		&gsn_whip },
   { "polearm",	WEAPON_POLEARM,		&gsn_polearm },
   { "staff",	WEAPON_STAFF,		&gsn_staff },
   { "short-sword",WEAPON_SHORTSWORD,	&gsn_shortsword },
   { "claws", WEAPON_CLAWS, &gsn_claws },
   { NULL,			0,	NULL	}
};

/* instrument selection table */
const	struct	instrument_type	instrument_table	[]	=
{
   { "ocarina",	INSTRUMENT_OCARINA,		&gsn_ocarina },
   { "flute",	INSTRUMENT_FLUTE,		&gsn_flute },
   { "lute",	INSTRUMENT_LUTE,		&gsn_lute },
   { "harp",	INSTRUMENT_HARP,		&gsn_harp },
   { "mandolin",INSTRUMENT_MANDOLIN,	&gsn_mandolin },
   { "piano",	INSTRUMENT_PIANO,		&gsn_piano },
   { NULL,			0,	NULL	}
};

/* wiznet table and prototype for future flag setting */
const   struct wiznet_type      wiznet_table    []              =
{
   {    "on",           WIZ_ON,         IM },
   {    "prefix",		WIZ_PREFIX,		IM },
   {    "ticks",        WIZ_TICKS,      IM },
   {    "logins",       WIZ_LOGINS,     IM },
   {    "sites",        WIZ_SITES,      L4 },
   {    "links",        WIZ_LINKS,      L7 },
   {	"newbies",		WIZ_NEWBIE,		IM },
   {	"spam",			WIZ_SPAM,		L5 },
   {    "deaths",       WIZ_DEATHS,     IM },
   {    "resets",       WIZ_RESETS,     L4 },
   {    "mobdeaths",    WIZ_MOBDEATHS,  L4 },
   {    "flags",		WIZ_FLAGS,		L5 },
   {	"penalties",	WIZ_PENALTIES,	L5 },
   {	"saccing",		WIZ_SACCING,	L5 },
   {	"levels",		WIZ_LEVELS,		IM },
   {	"load",			WIZ_LOAD,		L2 },
   {	"restore",		WIZ_RESTORE,	L2 },
   {	"snoops",		WIZ_SNOOPS,		L2 },
   {	"switches",		WIZ_SWITCHES,	L2 },
   {	"secure",		WIZ_SECURE,		L2 },
   {	"artefact",		WIZ_ARTEFACT,	L1 },
   {	"artefactload",	WIZ_ARTEFACTLOAD,L1},
   {	"mxp",			WIZ_MXP,		IM },
   {	"comm",			WIZ_COMM,		L3 },
   {	NULL,		0,		0  }
};

const 	struct attack_type	attack_table	[MAX_DAMAGE_MESSAGE]	=
{
/*  0 */
	{	"none",			"uderzenie",		-1		},
	{	"ciecie",		"ci�cie", 			DAM_SLASH	},
	{	"pchniecie",	"pchni�cie",		DAM_PIERCE	},
	{	"ciecie",		"ci�cie",			DAM_SLASH	},
	{	"whip",			"smagni�cie",		DAM_SLASH	},
/*  5 */
	{	"claw",			"szczypni�cie",		DAM_SLASH	},
	{	"podmuch",		"podmuch",			DAM_BASH	},
	{	"walniecie",	"walni�cie",		DAM_BASH	},
	{	"mwalniecie",	"mia�d��ce walni�cie",	DAM_BASH	},
	{	"grep",			"grep",				DAM_SLASH	},
/* 10 */
	{	"gryzienie",	"ugryzienie",		DAM_PIERCE	},
	{	"klucie",		"uk�ucie",			DAM_PIERCE	},
	{	"ssanie",		"ssanie",			DAM_BASH	},
	{	"uderzenie",	"uderzenie",		DAM_BASH	},
	{	"digestion",	"�r�ce uderzenie",	DAM_ACID	},
/* 15 */
	{	"szarza",		"szar�a",		DAM_BASH	},
	{	"klapniecie",		"klepni�cie",		DAM_BASH	},
	{	"uderzenie",		"uderzenie",		DAM_BASH	},
	{	"mindsuck",		"wyssanie umys�u",	DAM_MENTAL	},
	{	"magia",		"magia",		DAM_ENERGY	},
/* 20 */
	{	"bmoc",			"boska moc",		DAM_HOLY	},
	{	"ciecie",		"ci�cie",		DAM_SLASH	},
	{	"drapanie",		"drapni�cie",		DAM_SLASH	},
	{	"dziobanie",		"dziobni�cie",		DAM_PIERCE	},
	{	"dziobanieb",		"dziobni�cie",		DAM_BASH	},
/* 25 */
	{	"siekanie",		"siekni�cie",		DAM_SLASH	},
	{	"zadlenie",		"u��dlenie",		DAM_PIERCE	},
	{	"walenie",		"walni�cie",		DAM_BASH	},
	{	"sgryz",		"szokuj�ce ugryzienie",	DAM_LIGHTNING	},
	{	"ogryz",		"p�omienne ugryzienie",	DAM_FIRE	},
/* 30 */
	{	"mgryz",		"mro��ce ugryzienie",	DAM_COLD	},
	{	"kgryz",		"kwasowe ugryzienie",	DAM_ACID	},
	{	"chomp",		"chomp",		DAM_PIERCE	},
	{	"drain",		"wyssanie �ycia",	DAM_NEGATIVE	},
	{	"dzganie",		"d�gni�cie",		DAM_PIERCE	},
/* 35 */
	{	"slime",		"slime",		DAM_ACID	},
	{	"wstrzas",		"wstrz��ni�cie",	DAM_LIGHTNING	},
	{	"lupniecie",		"�upni�cie",		DAM_BASH	},
	{	"plomien",		"p�omie�",		DAM_FIRE	},
	{	"chlod",		"ch��d",		DAM_COLD	},
/* 40 */
	{	"k�sanie",		"uk�szenie",		DAM_PIERCE	},
	{	"pciecie",		"precyzyjne ci�cie",	DAM_SLASH	},
	{	"zciecie",		"zamaszyste ci�cie",	DAM_SLASH	},
	{	"udtarcz�",		"uderzenie tarcz�",		DAM_BASH	},
	{ "iluzcios",   "uderzenie",   DAM_MENTAL  },

	{	NULL,			NULL,			0	}
};

const	struct	pc_race_type	pc_race_table	[]	=
{
	{ "null race", "", 0, { 0, 0, 0, 0, 0 },
      { "" },  0,0,0, 0,  { 0, 0, 0, 0}, { 0, 0, 0, 0 }
	},

	{
	"cz�owiek",
	"Cz�owiek",
	1|2|4|8|16|32|64|128|256|512|1024,
	{ 0, 0, 0, 0, 0, 2, 1},
	{ "" },
	SIZE_MEDIUM ,
	LANG_COMMON,
	0,
	0,
	{160,215,140,190},
	{650, 1100, 500, 800},
	{16,20},
		{
		{1,1},
		{1,1},
		{1,1},
		{1,1},
		{1,1},
		{1,1},
		{1,1}
		}
	},
	{
	"elf",
	"Elf",
   	1|2|4|8|32|256,
	{ 0, 0,  0, 1, 0, 1, 0},
	{ "" },
	SIZE_SMALL,
	LANG_ELVEN,
	LANG_COMMON,
	LANG_DWARVEN|LANG_OGRE|LANG_ORCISH|LANG_TROLLISH,
	{170,210,165,190},
	{550, 800, 400, 650},
	{65,78},
		{{0,1},{1,2},{1,2},{1,2},{0,1},{0,1},{0,1}}
	},
	{
	"krasnolud",
	"Krasnolud",
	2|4|8|32|64|1024,
	{ 0, 0, 0, 0, 1, -1, 0},
	{ "" },
	SIZE_MEDIUM,
	LANG_DWARVEN,
	LANG_COMMON,
	LANG_ELVEN,
	{110, 150, 100, 145},
	{650,900, 550, 750},
	{37,43},
		{{1,2},{0,1},{1,1},{0,1},{2,3},{0,-1},{0,0}}
	},
	{
	"gnom",
	"Gnom",
	1|2|4|8|32,
	{ 0, 1, -1, 0, 0, 0, 0},
	{ "" },
	SIZE_SMALL,
	LANG_COMMON,
	LANG_DWARVEN|LANG_HALFLING,
	0,
	{110, 150, 100, 145},
	{450, 650, 350, 550},
	{27,33},
		{{0,1},{1,3},{0,1},{1,2},{0,1},{0,1},{0,1}}
	},
	{
	"p�elf",
	"P�elf",
	1|2|4|8|128|256|512,
	{ 0, 0, 0, 0, 0, 0, 0},
	{ "" },
	SIZE_MEDIUM,
	LANG_COMMON,
	LANG_ELVEN,
	0,
	{160,210,145,190},
	{650, 1000, 500, 800},
	{16,25},
		{{0,1},{0,1},{0,2},{0,2},{1,1},{0,2},{0,1}}
	},
	{
	"nizio�ek",
	"Nizio�ek",
	2|4|8,
	{ -1, 0, 0, 1, 0, 0, 2 },
	{ "sneak","hide" },
	SIZE_SMALL,
	LANG_COMMON|LANG_HALFLING,
	LANG_DWARVEN,
	0,
	{110, 150, 100, 145},
	{450, 650, 350, 550},
	{23,27},
		{{0,0},{0,1},{1,1},{1,3},{0,1},{0,0},{1,2}}
	},
	{
	"p�ork",
	"P�ork",
	8|64|1024,
	{ 1, -1, -1, 0, 1, -2, 0 },
	{ "" },
	SIZE_MEDIUM ,
	LANG_ORCISH,
	LANG_COMMON,
	LANG_ELVEN,
	{170,225,155,200},
	{850, 1350, 600, 1100},
	{14,18},
		{{2,2},{0,-1},{0,0},{1,1},{1,2},{0,0},{0,1}}
	}
};




/*
 * Class table.
 */
const	struct	class_type	class_table	[MAX_CLASS]	=
{
	{
	"Mag", //Nazwa klasy
	"Czarodziejka", //Nazwa klasy: kobita
	"Arcymag", //Nazwa dla 31 poziomu
	"Arcyczarodziejka",//Nazwa dla 31 poziomu: kobita
	"Mag", //Trzy litery odpowiadajace klasie
	STAT_INT, //Glowny atrybut
	20, //Thac0 (szansa trafienia) dla poziomu 0
	12, //Thac0 (szansa trafienia) dla poziomu 32 - im mniej tym lepsza.
	5, //Minimalna ilosc hp, zdobywana przy level upie
	8, //Maxymalna ilosc hp, zdobywana przy level upie
	0, //Numer castera (-1 powoduje ze postac nie czaruje)
		{{0,0},{2,3},{0,1},{0,2},{0,0},{1,2},{0,1}} //NOWE STATY
	},

	{
	"Kleryk",
	"Kap�anka",
	"Hierofant",
	"Arcykap�anka",
	"Cle",
	STAT_WIS,
	20,
	6,
	8,
	13,
	1,
		{{0,1},{0,1},{2,3},{0,0},{0,1},{0,2},{0,1}}
	},

	{
	"Z�odziej",
	"Z�odziejka",
	"Zab�jca",
	"Skrytob�jczyni",
	"Thi",
	STAT_DEX,
	20,
	7,
	9,
	14,
	-1,
		{{0,1},{1,2},{0,0},{2,3},{0,1},{0,0},{0,1}}

	},

	{
	"Wojownik",
	"Wojowniczka",
	"Szermierz Natchniony",
	"Fechmistrzyni",
	"War",
	STAT_STR,
	20,
	-6,
	15,
	20,
	-1,
		{{1,2},{0,1},{0,0},{1,2},{2,3},{0,0},{0,1}}
	},
	{
	"Paladyn",
	"Paladynka",
	"Krzy�owiec",
	"Inkwizytorka",
	"Pal",
	STAT_CHA,
	20,
	-2,
	15,
	20,
	 4,
		{{1,2},{0,0},{1,2},{1,2},{1,2},{1,2},{0,1}}
	},
	{
	"Druid",
	"Druidka",
	"Arcydruid",
	"W�adczyni Natury",
	"Dru",
	STAT_WIS,
	20,
	6,
	8,
	13,
	5,
		{{0,0},{0,0},{2,3},{0,0},{0,2},{0,1},{1,2}}
	},
	{
	"Barbarzy�ca",
	"Barbarzynka",
	"Berserker",
	"Dzika Kobieta",
	"Bar",
	STAT_STR,
	20,
	 2,
	18,
	25,
	-1,
		{{2,3},{0,0},{0,0},{1,2},{2,3},{0,0},{0,2}}
	},
	{
	"Mnich",
	"Mniszka",
	"Opat",
	"Prze�o�ona Zakonu",
	"Mni",
	STAT_WIS,
	20,
	-2,
	12,
	18,
	-1,
		{{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}} //UZUPELNIc!!!!
	},
	{
	"Bard",
	"Trubadurka",
	"Artysta",
	"Artystka",
	"Brd",
	STAT_CHA,
	20,
	7,
	9,
	14,
	8,
		{{0,0},{0,1},{0,0},{0,1},{0,0},{1,2},{0,1}}
	},
	{
	"Czarny Rycerz",
	"Czarna Stra�niczka",
	"Rycerz �mierci",
	"W�adczyni Mroku",
	"Blk",
	STAT_STR,
	20,
	-2,
	15,
	20,
	-1,
		{{2,2},{0,0},{0,1},{0,2},{1,2},{1,2},{0,1}}
	},
	{
	"Szaman",
	"Szamanka",
	"Krocz�cy W�r�d Duch�w",
	"Krocz�ca W�r�d Duch�w",
	"Sha",
	STAT_STR,
	20,
	 4,
	9,
	14,
	10,
		{{0,1},{0,0},{1,1},{0,1},{0,1},{0,0},{0,0}}
	}
};



/*
 * Attribute bonus tables.
 *
 * struct   str_app_type
 * {
 *     sh_int   tohit; - bonus do trafienia
 *     sh_int   todam; - bonus do obrazen
 *     sh_int   carry; - udzwig
 *     sh_int   wield; - bron na wield
 * };
 *
 */

const struct str_app_type str_app [256] =
{
{ -4, -3,   4,  0 },  /* 0  */
{ -4, -3,   6,  0 },
{ -4, -3,   8,  0 },
{ -4, -3,  10,  0 },
{ -4, -3,  12,  0 },
{ -4, -3,  14,  0 },
{ -4, -3,  16,  1 },  /* 6  */
{ -4, -3,  18,  1 },
{ -4, -3,  20,  1 },
{ -4, -3,  22,  1 },
{ -4, -3,  24,  1 },
{ -4, -3,  26,  1 },
{ -4, -3,  28,  2 },//12
{ -4, -3,  30,  2 },
{ -4, -3,  32,  2 },
{ -4, -3,  34,  2 },
{ -4, -3,  36,  2 },
{ -4, -3,  38,  2 },
{ -4, -3,  40,  3 },//18
{ -4, -3,  42,  3 },//19
{ -4, -3,  44,  3 },//20
{ -4, -3,  46,  3 },//21
{ -4, -3,  48,  3 },//22
{ -4, -3,  50,  3 },//23
{ -4, -3,  52,  4 },//24
{ -4, -3,  54,  4 },//25
{ -4, -3,  56,  4 },//26
{ -4, -3,  58,  4 },//27
{ -4, -3,  60,  4 },//28
{ -4, -3,  62,  4 },//29
{ -3, -2,  64,  5 },//30
{ -3, -2,  66,  5 },//31
{ -3, -2,  68,  5 },//32
{ -3, -2,  70,  5 },//33
{ -3, -2,  72,  5 },//34
{ -3, -2,  74,  5 },//35
{ -3, -2,  76,  6 },//36
{ -3, -2,  78,  6 },//37
{ -3, -2,  80,  6 },//38
{ -3, -2,  82,  6 },//39
{ -3, -2,  84,  6 },//40
{ -3, -2,  86,  6 },//41
{ -2, -1,  88,  7 },//42
{ -2, -1,  80,  7 },//43
{ -2, -1,  82,  7 },//44
{ -2, -1,  84,  7 },//45
{ -2, -1,  86,  7 },//46
{ -2, -1,  88,  7 },//47
{ -2, -1,  80,  8 },//48
{ -2, -1,  82,  8 },//49
{ -2, -1,  85,  8 },//50
{ -2, -1,  88,  8 },//51
{ -2, -1,  91,  8 },//52
{ -2, -1,  94,  8 },//53
{ -1, -1,  97,  9 },//54
{ -1, -1, 100,  9 },//55
{ -1, -1, 103,  9 },//56
{ -1, -1, 106,  9 },//57
{ -1, -1, 109,  9 },//58
{ -1, -1, 112,  9 },//59
{ -1,  0, 116, 10 },//60
{ -1,  0, 120, 10 },//61
{ -1,  0, 124, 10 },//62
{ -1,  0, 128, 10 },//63
{ -1,  0, 132, 10 },//64
{ -1,  0, 136, 10 },//65
{  0,  0, 140, 10 }, // 66
{  0,  0, 144, 11 }, // 67
{  0,  0, 148, 11 }, // 68
{  0,  0, 152, 11 }, // 69
{  0,  0, 157, 11 }, // 70
{  0,  0, 162, 11 }, // 71
{  0,  0, 167, 11 }, // 72
{  0,  0, 172, 12 }, // 73
{  0,  0, 177, 12 }, // 74
{  0,  0, 182, 12 }, // 75
{  0,  0, 187, 12 }, // 76
{  0,  0, 192, 12 }, // 77
{  0,  0, 197, 12 }, // 78
{  0,  0, 202, 13 }, // 79
{  0,  0, 208, 13 }, // 80
{  0,  0, 214, 13 }, // 81
{  0,  0, 220, 13 }, // 82
{  0,  0, 226, 13 }, // 83
{  0,  0, 232, 13 }, // 84
{  0,  0, 238, 14 }, // 85
{  0,  0, 244, 14 }, // 86
{  0,  0, 250, 14 }, // 87
{  0,  0, 256, 14 }, // 88
{  0,  0, 262, 14 }, // 89
{  0,  0, 269, 14 }, // 90
{  0,  0, 276, 15 }, // 91
{  0,  0, 283, 15 }, // 92
{  0,  0, 290, 15 }, // 93
{  0,  0, 297, 15 }, // 94
{  0,  0, 304, 15 }, // 95
{  0,  1, 311, 15 }, // 96
{  0,  1, 318, 16 }, // 97
{  0,  1, 325, 16 }, // 98
{  0,  1, 332, 17 }, // 99
{  0,  1, 340, 18 }, // 100
{  0,  1, 348, 19 }, // 101
{  1,  1, 356, 20 }, // 102
{  1,  1, 364, 21 }, // 103
{  1,  1, 372, 22 }, // 104
{  1,  1, 380, 22 }, // 105
{  1,  1, 388, 23 }, // 106
{  1,  1, 396, 24 }, // 107
{  1,  2, 402, 25 }, // 108
{  1,  2, 410, 25 }, // 109
{  1,  2, 419, 25 }, // 110
{  1,  2, 428, 25 }, // 111
{  1,  2, 437, 25 }, // 112
{  1,  2, 446, 25 }, // 113
{  1,  2, 445, 26 }, // 114
{  1,  2, 454, 27 }, // 115
{  1,  2, 463, 27 }, // 116
{  1,  2, 472, 28 }, // 117
{  1,  2, 481, 28 }, // 118
{  1,  2, 490, 29 }, // 119
{  1,  3, 500, 29 }, // 120
{  1,  3, 510, 30 }, // 121
{  1,  3, 520, 30 }, // 122
{  1,  3, 530, 30 }, // 123
{  1,  3, 540, 31 }, // 124
{  1,  3, 550, 31 }, // 125
{  2,  3, 560, 31 }, // 126
{  2,  3, 570, 32 }, // 127
{  2,  3, 580, 32 }, // 128
{  2,  3, 590, 32 }, // 129
{  2,  3, 601, 33 }, // 130
{  2,  3, 612, 33 }, // 131
{  2,  3, 623, 33 }, // 132
{  2,  3, 634, 34 }, // 133
{  2,  3, 645, 34 }, // 134
{  2,  3, 656, 34 }, // 135
{  2,  3, 667, 35 }, // 136
{  2,  3, 678, 35 }, // 137
{  3,  3, 689, 36 }, // 138
{  3,  3, 700, 36 }, // 139
{  3,  4, 712, 36 }, // 140
{  3,  4, 724, 36 }, // 141
{  3,  4, 736, 37 }, // 142
{  3,  4, 748, 37 }, // 143
{  3,  4, 760, 37 }, // 144
{  3,  4, 772, 38 }, // 145
{  3,  4, 784, 38 }, // 146
{  3,  4, 796, 38 }, // 147
{  3,  4, 808, 38 }, // 148
{  3,  4, 820, 39 }, // 149
{  4,  4, 833, 39 }, // 150
{  4,  4, 846, 40 }, // 151
{  4,  4, 859, 40 }, // 152
{  4,  4, 872, 40 }, // 153
{  4,  4, 885, 41 }, // 154
{  4,  4, 898, 42 }, // 155
{  4,  4, 911, 43 }, // 156
{  4,  4, 924, 44 }, // 157
{  4,  4, 937, 45 }, // 158
{  4,  4, 950, 46 }, // 159
{  4,  4, 964, 47 }, // 160
{  4,  4, 978, 48 }, // 161
{  4,  4, 992, 49 }, // 162
{  4,  4,1006, 50 }, // 163
{  4,  4,1020, 52 }, // 164
{  4,  4,1034, 54 }, // 165
{  4,  4,1048, 56 }, // 166
{  4,  4,1062, 58 }, // 167
{  4,  4,1076, 59 }, // 168
{  4,  4,1090, 60 }, // 169
{  4,  4,1205, 62 }, // 170
{  4,  4,1220, 64 }, // 171
{  4,  5,1235, 66 }, // 172
{  4,  5,1250, 68 }, // 173
{  4,  5,1265, 69 }, // 174
{  4,  5,1280, 70 }, // 175
{  4,  5,1295, 71 }, // 176
{  4,  5,1310, 72 }, // 177
{  4,  5,1325, 73 }, // 178
{  4,  5,1340, 74 }, // 179
{  5,  5,1356, 74 }, // 180
{  5,  5,1372, 75 }, // 181
{  5,  5,1388, 76 }, // 182
{  5,  5,1404, 77 }, // 183
{  5,  5,1420, 78 }, // 184
{  5,  5,1436, 79 }, // 185
{  5,  5,1452, 79 }, // 186
{  5,  5,1468, 80 }, // 187
{  5,  6,1484, 81 }, // 188
{  5,  6,1500, 82 }, // 189
{  5,  6,1517, 83 }, // 190
{  5,  6,1534, 84 }, // 191
{  5,  6,1551, 84 }, // 192
{  5,  6,1568, 85 }, // 193
{  5,  6,1585, 86 }, // 194
{  5,  6,1602, 87 }, // 195
{  5,  6,1619, 88 }, // 196
{  5,  6,1636, 89 }, // 197
{  5,  6,1653, 89 }, // 198
{  5,  6,1670, 90 }, // 199
{  5,  6,1688, 91 }, // 200
{  5,  6,1706, 92 }, // 201
{  5,  6,1724, 93 }, // 202
{  5,  7,1742, 94 }, // 203
{  5,  7,1760, 95 }, // 204
{  5,  7,1778, 96 }, // 205
{  5,  7,1796, 97 }, // 206
{  5,  7,1814, 98 }, // 207
{  5,  7,1832, 99 }, // 208
{  5,  7,1850, 100 }, // 209
{  5,  7,1869, 101 }, // 210
{  5,  8,1888, 102 }, // 211
{  5,  8,1917, 103 }, // 212
{  5,  8,1936, 104 }, // 213
{  5,  8,1955, 105 }, // 214
{  5,  8,1974, 106 }, // 215
{  5,  8,1993, 107 }, // 216
{  5,  8,2012, 108 }, // 217
{  5,  8,2031, 109 }, // 218
{  5,  9,2050, 110 }, // 219
{  5,  9,2070, 111 }, // 220
{  5,  9,2090, 112 }, // 221
{  5,  9,2110, 113 }, // 222
{  5,  9,2130, 114 }, // 223
{  5,  9,2150, 115 }, // 224
{  6,  9,2170, 116 }, // 225
{  6,  9,2190, 117 }, // 226
{  6,  9,2210, 118 }, // 227
{  6,  9,2230, 119 }, // 228
{  6,  9,2250, 200 }, // 229
{  6, 10,2270, 201 }, // 230
{  6, 10,2290, 202 }, // 231
{  6, 10,2310, 203 }, // 232
{  6, 10,2330, 204 }, // 233
{  6, 10,2350, 205 }, // 234
{  6, 11,2370, 206 }, // 235
{  6, 11,2390, 207 }, // 236
{  6, 11,2410, 208 }, // 237
{  6, 11,2430, 209 }, // 238
{  6, 11,2450, 210 }, // 239
{  6, 11,2470, 212 }, // 240
{  7, 11,2490, 214 }, // 241
{  7, 11,2510, 216 }, // 242
{  7, 12,2530, 218 }, // 243
{  7, 12,2550, 220 }, // 244
{  7, 12,2570, 223 }, // 245
{  7, 12,2600, 226 }, // 246
{  7, 12,2630, 229 }, // 247
{  7, 12,2660, 232 },  // 248
{  8, 12,2690, 235 },  // 249
{  8, 12,2720, 239 },  // 250
{  8, 13,2750, 243 },  // 251
{  8, 13,2780, 247 },  // 252
{  9, 13,2850, 252 },  // 253
{  9, 13,2900, 259 },  // 254
{ 10, 14,3000, 270 }  /* 255 */
};

const	struct	int_app_type	int_app		[34]		=
{//do studiowania ksiag i uczenia skilli, uczenie sie trikow, nie uzywane
	{  3,-99,  0},	/*  0 */
	{  5,-99,  0},	/*  1 */
	{  7,-99,  0},
	{  8,-50,  0},	/*  3 */
	{  9,-40,  0},
	{ 10,-30,  0},	/*  5 */
	{ 11,-20,  0},
	{ 12,-15,  0},
	{ 13,-10,  0},
	{ 15, -7,  6},
	{ 17, -4,  7},	/* 10 */
	{ 19,  0,  7},
	{ 22,  0,  7},
	{ 25,  0,  9},
	{ 28,  1,  9},
	{ 31,  2, 11},	/* 15 */
	{ 34,  3, 11},
	{ 37,  5, 14},
	{ 42,  8, 18},	/* 18 */
	{ 47, 10, -1},
	{ 55, 12, -1},	/* 20 */
	{ 60, 15, -1},
	{ 65, 20, -1},
	{ 72, 25, -1},
	{ 80, 30, -1},
	{ 85, 35, -1},	/* 25 */
	{ 90, 40, -1},
	{ 95, 50, -1},
	{ 96, 60, -1},	/* 28 */
	{ 97, 65, -1},
	{ 98, 69, -1},
	{ 99, 73, -1},
	{ 100, 77, -1},
	{ 100, 80, -1} /* 33*/
};

const	struct	wis_app_type	wis_app		[34]		=
{//dodatkowe czary kleryka/druida, szansa na zepsucie zaklecia, premia do klasy pancerza mnicha (wartosc * -10), dodatkowe czary dla szamana
	{ -3, 99, -6 },//0
	{ -3, 99, -6 },//1
	{ -3, 99, -6 },//2
	{ -3, 99, -5 },//3
	{ -3, 90, -5 },//4
	{ -2, 80, -4 },//5
	{ -2, 70, -4 },//6
	{ -2, 60, -3 },//7
	{ -1, 50, -3 },//8
	{ -1, 40, -2 },//9
	{ -1, 30, -1 },//10
	{ -1, 20, 0 },//11
	{ 0,  12, 0 },//12
	{ 0,  8, 0 },//13
	{ 0,  5, 0 },//14
	{ 0,  4, 0 },//15
	{ 1,  3, 1 },//16
	{ 1,  2, 1 },//17
	{ 2,  1, 2 },//18
	{ 2,  1, 2 },//19
	{ 2,  0, 3 },//20
	{ 3,  0, 3 },//21
	{ 3,  0, 4 },//22
	{ 4,  0, 4 },//23
	{ 4,  0, 5 },//24
	{ 4,  0, 5 },//25
	{ 5,  0, 6 },//26
	{ 5,  0, 6 },//27
	{ 6,  0, 7 }, //28
	{ 6,  0, 7 },
	{ 7,  0, 7 },
	{ 7,  0, 8 },
	{ 8,  0, 8 },
	{ 8,  0, 8 } /*33*/
};

const	struct	dex_app_type	dex_app		[34]		=
{//modyfikator klasy pancerza
	{   60 },//00
	{   55 },//01
	{   50 },//02
	{   45 },//03
	{   40 },//04
	{   35 },//05
	{   30 },//06
	{   25 },//07
	{   20 },//08
	{   15 },//09
	{   10 },//10
	{    5 },//11
	{    0 },//12
	{    0 },//13
	{    0 },//14
	{    0 },//15
	{    0 },//16
	{ -  5 },//17
	{ - 10 },//18
	{ - 15 },//19
	{ - 20 },//20
	{ - 25 },//21
	{ - 30 },//22
	{ - 35 },//23
	{ - 40 },//24
	{ - 45 },//25
	{ - 50 },//26
	{ - 55 },//27
	{ - 60 }, //28
	{ - 64 },
	{ - 67 },//30
	{ - 69 },
	{ - 71 },//32
	{ - 72 } //33
};

const	struct	con_app_type	con_app		[34]		=
{//mod do hp dla zbrojnych(woj,pal,barb,mnich), mod do hp dla reszty, odwrotnosc szansy % na daze_state przy 'to naprawde bolalo!'
	{ -3,-3, 20 },//0
	{ -3,-3, 25 },//1
	{ -3,-3, 30 },//2
	{ -3,-3, 35 },//3
	{ -3,-3, 40 },//4
	{ -2,-2, 45 },//5
	{ -2,-2, 50 },//6
	{ -2,-2, 55 },//7
	{ -1,-1, 60 },//8
	{ -1,-1, 65 },//9
	{  0,0, 70 },//10
	{  0,0, 75 },//11
	{  0,0, 80 },//12
	{  0,0, 85 },//13
	{  0,0, 88 },//14
	{  1,1, 90 },//15
	{  1,1, 95 },//16
	{  1,1, 97 },//17
	{  2,2, 99 },//18
	{  2,2, 99 },//19
	{  2,2, 99 },//20
	{  3,2, 99 },//21
	{  3,2, 99 },//22
	{  3,2, 99 },//23
	{  4,3, 99 },//24
	{  4,3, 99 },//25
	{  5,3, 99 },//26
	{  5,3, 99 },//27
	{  6,3, 100 }, //28
	{  7,4, 100 },
	{  8,4, 100 },
	{  9,5, 100 },
	{  10,5, 100 },
	{  11,5, 100 } //33


};

/*
 * Liquid properties.
 * Used in world.obj.
 */
const	struct	liq_type	liq_table	[]	=
{
/*
	liq_name,
	liq_name2,
	liq_name3,
	liq_name4,
	liq_name5,
	liq_name6,
	liq_color,
	(proof, full, thirst, food, size)
*/
// 0
	{"woda", "wody", "wodzie", "wod�", "wod�", "wodzie", "przezroczysty", {0, 0,  3, 0,  3}},
	{"piwo", "piwa", "piwu", "piwo", "piwem", "piwie", "piwny", {1, 0,  1, 1,  2}},
	{"czerwone wino", "czerwonego wina", "czerwonemu winu", "czerwone wino", "czerwonym winem", "czerwonym winie", "czerwony", {1, 0,  1, 0,  2}},
	{"jasne piwo", "jasnego piwa", "jasnemu piwu", "jasne piwo", "jasnym piwem", "jasnym piwie", "z�ocisty", {1, 0,  1, 0,  2}},
	{"ciemne piwo", "ciemnego piwa", "ciemnemu piwu", "ciemne piwo", "ciemnym piwem", "ciemnym piwie", "ciemnobr�zowy", {2, 0,  1, 1,  2}},
// 5
	{"whisky", "whisky", "whisky", "whisky", "whisky", "whisky", "br�zowo-z�oty", {3, 0,  1, 0,  2}},
	{"lemoniada", "lemoniady", "lemoniadzie", "lemoniad�", "lemoniad�", "lemoniadzie", "owocowy", {0, 0,  2, 0,  3}},
	{"gard�ogrzmot", "gard�ogrzmota", "gard�ogrzmotowi", "gard�ogrzmot", "gard�ogrzmotem", "gard�ogrzmocie", "bulgocz�cy", {4, 0,  1, 0,  2}},
	{"lokalny specja�", "lokalnego specja�u", "lokalnemu specja�owi", "lokalny specja�", "lokalnym specja�em", "lokalnym specjale", "przezroczysty", {4, 0,  1, 0,  2}},
	{"sok", "soku", "sokowi", "sok", "sokiem", "soku", "zielony", {0, 0,  1, 1,  3}},
// 10
	{"mleko", "mleka", "mleku", "mleko", "mlekiem", "mleku", "bia�y", {0, 0,  2, 1,  3}},
	{"herbata", "herbaty", "herbacie", "herbat�", "herbat�", "herbacie", "br�zowawy", {0, 0,  2, 0,  4}},
	{"kawa", "kawy", "kawie", "kaw�", "kaw�", "kawie", "czarny", {0, 0,  1, 0,  4}},
	{"krew", "krwi", "krwi", "krew", "krwi�", "krwi", "szkar�atny", {0, 0,  0, 1,  4}},
	{"s�ona woda", "s�onej wody", "s�onej wodzie", "s�on� wod�", "s�on� wod�", "s�onej wodzie", "przezroczysty", {0, 0, -1, 0,  1}},
// 15
	{"piwo imbirowe", "piwa imbirowego", "piwu imbirowemu", "piwo imbirowe", "piwem imbirowym", "piwie imbirowym", "br�zowy", {2, 0,  1, 1,  3}},
	{"piwo korzenne", "piwa korzennego", "piwu korzennemu", "piwo korzenne", "piwem korzennym", "piwie korzennym", "br�zowy", {2, 0,  1, 1,  3}},
	{"elfie wino", "elfiego wina", "elfiemu winu", "elfie wino", "elfim winem", "elfim winie", "zielony", {2, 0,  2, 0,  4}},
	{"bia�e wino", "bia�ego wina", "bia�emu winu", "bia�e wino", "bia�ym winem", "bia�ym winie", "z�oty", {2, 0,  2, 0,  4}},
	{"wino musuj�ce", "wina musuj�cego", "winu musuj�cemu", "wino musuj�ce", "winem musuj�cym", "winie musuj�cym", "z�otawy", {1, 0,  1, 0,  4}},
// 20
	{"mi�d pitny", "miodu pitnego", "miodowi pitnemu", "mi�d pitny", "miodem pitnym", "miodzie pitnym", "miodowy", {3, 0,  2, 1,  2}},
	{"wino r�ane", "wina r�anego", "winu r�anemu", "wino r�ane", "winem r�anym", "winie r�anym", "r�owe", {2, 0,  2, 0,  3}},
	{"wino silea�skie", "wina silea�skiego", "winu sila�skiemu", "wino silea�skie", "winem silea�skim", "winie silea�skim", "czerwony", {2, 0,  2, 0,  3}},
	{"w�dka", "w�dki", "w�dce", "w�dk�", "w�dk�", "w�dce", "przezroczysty", {4, 0,  1, 0,  2}},
	{"sok porzeczkowy", "soku porzeczkowego", "sokowi porzeczkowemu", "sok porzeczkowy", "sokiem porzeczkowym", "soku porzeczkowym", "czerwony", {0, 0,  3, 1,  2}},
// 25
	{"sok pomara�czowy", "soku pomara�czowego", "sokowi pomara�czowemu", "sok pomara�czowy", "sokiem pomara�czowym", "soku pomara�czowym", "pomara�czowy", {0, 0,  3, 1,  4}},
	{"spirytus", "spirytusu", "spirytusowi", "spirytus", "spirytusem", "spirytusie", "przezroczysty", {5, 0,  1, 0,  2}},
	{"brandy", "brandy", "brandy", "brandy", "brandy", "brandy", "z�oty", {3, 0,  1, 0,  2}},
	{"w�dka zio�owa", "w�dki zio�owej", "w�dce zio�owej", "w�dk� zio�ow�", "w�dk� zio�ow�", "w�dce zio�owej", "przezroczysty", {4, 0,  1, 0,  2}},
	{"woda ent�w", "wody ent�w", "wodzie ent�w", "wod� ent�w", "wod� ent�w", "wodzie ent�w", "przezroczysty", {0, 0,  4, 2,  2}},
// 30
	{"wino lodowe", "wina lodowego", "winu lodowemu", "wino lodowe", "winem lodowym", "winie lodowym", "purpurowe", {2, 0,  2, 0,  2}},
	{"�liwowica", "�liwowicy", "�liwowicy", "�liwowic�", "�liwowic�", "�liwowicy", "przezroczysty", {5, 0,  1, 0,  2}},
	{"wi�ni�wka", "wi�ni�wki", "wi�ni�wce", "wi�n�wk�", "wi�ni�wk�", "wi�ni�wce", "czerwony", {3, 0,  1, 0,  2}},
	{"wino jab�kowe", "wina jab�kowego", "winu jab�kowemu", "wino jab�kowe", "winem jab�kowym", "winie jab�kowym", "��tawy", {2, 0,  1, 0,  2}},
	{"rum", "rumu", "rumowi", "rum", "rumem", "rumie", "jasnobr�zowy", {4, 0,  1, 0,  2}},
// 35
	{"mocne wino owocowe", "mocnego wina owocowego", "mocnemu winu owocowemu", "mocne wino owocowe", "mocnym winem owocowym", "mocnym winie owocowym", "m�tny czerwonawy", {3, 0,  2, 1,  3}},
	{"samogon", "samogonu", "samogonowi", "samogon", "samogonem", "samogonie", "przezroczysty", {5, 0,  0, 0,  1}},
	{"absynt", "absyntu", "absyntowi", "absynt", "absyntem", "absyncie", "przezroczysty", {4, 0,  0, 0,  1}},
	{"b�oto", "b�ota", "b�otu", "b�oto", "b�otem", "b�ocie", "b�otnisty", {0, 0,  1, 2,  3}},
	{"sok brzozowy", "soku brzozowego", "sokowi brzozowemu", "sok brzozowy", "sokiem brzozowym", "soku brzozowym", "s�omkowy", {0, 0, 3, 0, 3}},
// 40
	{"kompot", "kompotu", "kompotowi", "kompot", "kompotem", "kompocie", "s�omkowy", {0, 0,  3, 1,  4}},
	{"dziegie�", "dziegciu", "dziegciowi", "dziegie�", "dziegciem", "dziegciu", "m�tnobr�zowawy", {0, 0,  3, 0,  3}},
	{"uzdrawiaj�ca woda", "uzdrawiaj�cej wody", "uzdrawiaj�cej wodzie", "uzdrawiaj�c� wod�", "uzdrawiaj�c� wod�", "uzdrawiaj�cej wodzie", "przezroczysty", {0, 0,  3, 0,  3}},
	{"kwas", "kwasu", "kwasowi", "kwas", "kwasem", "kwasie", "zielony", {0, 0,  0, 0, 1}},

// uwaga dodaj�c p�yn prosz� zmieni� maksymaln� warto�� losowan� w:
// spells_mag.c : spell_transmute_liquid
	{NULL, NULL, NULL, NULL, NULL, NULL, NULL, {   0, 0,  0, 0,  0 }}
};



/*
 * The skill and spell table.
 * Slot numbers must never be changed as they appear in #OBJECTS sections.
 */
#define SLOT(n)	n


const struct  ShieldType ShieldList[] =
{
	{	0,		"puklerz",				2,			18,			0	},
	{	1,		"lekka tarcza",			3,			25,			5	},
	{	2,		"srednia tarcza",		4,			30,			8	},
	{	3,		"ciezka tarcza",		5,			35,			10	},
	{	4,		"pawez",				6,			45,			15	},
	{	0,		NULL,					0,			0,			0	}
};

/*
ziolka (herbs)
*/
const struct herb_type herb_table[]=
{
/* przykladowe
      {"laicka nazwa","laicki dopelniacz","laicki celownik","laicki biernik","laicki narzednik","laicki miejscownik",
      "nazwa","dopelniacz","celownik","biernik","narzednik","miejscownik",
      "opis wygladu",
      cena liczbowo, "jaka czesc rosliny PLANT_PART_", trudnosc liczbowo, trucizna (9 jesli nie),
      {0,0,0,0,0, //to prawdopodobienstwa w sectorach, ida po kolei jak w sector_table
       0,0,0,0,0,
       0,0,0,0,0,
       0,0,0,0,0,
       0,0,0,0,0,
       0,0,0,0,0,
       0,0,0,0,0,
       0,0,0,0,0,
       0,0,0,0,0},    //jest 45 czyli na zapas...
       efekt jedzenia liczbowo(czyli numer), "opis smaku",
       efekt wypalenia liczbowo(czyli numer), "opis wypalenia"
      }
*/
      {"unknown","unknown","unknown","unknown","unknown","unknown",
      "unknown","unknown","unknown","unknown","unknown","unknown",
      "unknown",
      0, 0, 0, 10,
      {0,0,0,0,0, //to prawdopodobienstwa w sectorach, ida po kolei jak w sector_table
       0,0,0,0,0,
       0,0,0,0,0,
       0,0,0,0,0,
       0,0,0,0,0,
       0,0,0,0,0,
       0,0,0,0,0,
       0,0,0,0,0,
       0,0,0,0,0,0,0,0,0,0},    //jest 45 czyli na zapas...
       0, "unknown",
       0, "unknown"
      },
      {"ogromna zielona bylina", "ogromnej zielonej byliny", "ogromnej zielonej bylinie", "ogromn� zielon� bylin�", "ogromn� zielon� bylin�", "ogromnej zielonej bylinie",
	     "arcydzi�giel litwor", "arcydzi�giel litwor", "arcydzi�giel litwor", "arcydzi�giel litwor", "arcydzi�giel litwor", "arcydzi�giel litwor",
	     "Ogromna zielona bylina s�u��ca do przygotowywania napar�w lecz�cych.",
	     200, PLANT_PART_PERENNIAL, 2, 9,
	     {0, 0, 5, 10, 10,	      15, 0, 0, 0, 0,	      0, 15, 35, 5, 0,	      0, 0, 0, 0, 0,	      0, 0, 0, 0, 0,	      0, 0, 0, 0, 0,	      100, 0, 10, 0, 0,	      5, 0, 0, 0, 0,	      0, 0, 20, 0, 0,0,0,0,0,0},          //jest 45 czyli na zapas...
	     0, "",
	     0, ""
	    },

	    {"du�y z�ty kwiatek", "du�ego ��tego kwiatka", "du�emu ��temu kwiatkowi", "du�y ��ty kwiatek", "du�ym ��tym kwiatkiem", "du�ym ��tym kwiatku",
	     "arnika g�rska", "arniki g�rskiej", "arnice g�rskiej", "arnik� g�rsk�", "arnik� g�rsk�", "arnice g�rskiej",
	     "Du�y ��ty kwiatek o w�a�ciwo�ciach leczniczych.",
	     1200, PLANT_PART_FLOWER, 21, 9,
	     {0, 0, 0, 0, 10,
	      25, 0, 0, 0, 0,
	      0, 0, 0, 35, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      100, 0, 0, 0, 0,
	      15, 0, 0, 0, 0,
	      0, 0, 5, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_POISON_LEVEL20, "opis smaku",
	     0, "opis wypalenia"
	    },

	    {"zielone pod�u�ne li�cie", "zielonych pod�u�nych li�ci", "zielonym pod�u�nym li�ciom", "zielone pod�u�ne li�cie", "zielonymi pod�u�nymi li��mi", "zielonych pod�u�nych li�ciach",
	     "babka lancetowata", "babki lancetowatej", "babce lancetowatej", "babk� lancetowat�", "babk� lancetowat�", "babce lancetowatej",
	     "Zielone pod�u�ne li�cie, od wiek�w wykorzystywane w medycynie.",
	     60, PLANT_PART_LEAF, 1, 9,
	     {0, 5, 50, 5, 5,	      0, 0, 0, 0, 0,	      0, 10, 0, 0, 0,	      0, 0, 0, 25, 0,	      0, 0, 0, 0, 0,	      0, 0, 0, 35, 0,	      100, 0, 60, 10, 0,	      0, 2, 0, 20, 0,	      0, 0, 40, 0, 0,0,0,0,0,0},
		PLANT_EFFECT_SMALL_HP, "opis smaku",
	     0, "opis wypalenia"
	    },
	    {"malutkie okr�g�e li�cie", "malutkich okr�g�ych li�ci", "malutkim okr�g�ym li�ciom", "malutkie okr�g�e li�cie", "malutkimi okr�g�ymi li��mi", "malutkich okr�g�ych li�ciach",
	     "bazylia wonna", "bazylii wonnej", "bazylii wonnej", "bazyli� wonn�", "bazyli� wonn�", "bazylii wonnej",
	     "Malutkie okr�g�e li�cie cenione jako przyprawa, do np. zup.",
	     85, PLANT_PART_LEAF, 31, 9,
	     {0, 0, 40, 10, 0,	      0, 0, 0, 0, 0,	      0, 20, 0, 0, 0,      0, 0, 0, 0, 0,	      0, 0, 0, 0, 0,	      0, 0, 0, 0, 0,	      100, 0, 10, 0, 0,	      0, 0, 0, 5, 0,0, 0, 10, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_SMALL_FOOD, "opis smaku",
	     0, "opis wypalenia"
	    },
	    {"malutkie pod�u�ne czerwone jagody", "malutkich pod�u�nych czerwonych jag�d", "malutkim pod�u�nym czerwonym jagodom", "malutkie pod�u�ne czerwone jagody", "malutkimi pod�u�nymi czerwonymi jagodami", "malutkich pod�u�nych czerwonych jagodach",
	     "berberys zwyczajny", "berberysu zwyczajnego", "berberysowi zwyczajnemu", "berberys zwyczajny", "berberysem zwyczajnym", "berberysie zwyczajnym",
	     "Malutkie pod�u�ne czerwone jagody, czasami jadalne i u�ywane jako przyprawa, jednak wie�� niesie, �e s� one magazynowane przez przer�nych mag�w.",
	     85, PLANT_PART_BERRYS, 15, 9,
	     {0, 0, 0, 30, 0,	      0, 0, 0, 0, 0,	      0, 20, 30, 0, 0,	      0, 0, 0, 0, 0,	      0, 0, 0, 0, 0,	      0, 0, 0, 0, 0,	      100, 0, 5, 0, 0,	      0, 0, 0, 10, 0,	      0, 0, 5, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_SMALL_FOOD, "opis smaku",
	     0, "opis wypalenia"
	    },
	    {"malutkie okr�g�e czarne owoce", "malutkich okr�g�ych czarnych owoc�w", "malutkim okr�g�ym czarnym owocom", "malutkie okr�g�e czarne owoce", "malutkimi okr�g�ymi czarnymi owocami", "malutkich okr�g�ych czarnych owocach",
	     "bez czarny", "bzu czarnego", "bzowi czarnemu", "bez czarny", "bezem czarnym", "bzie czarnym",
	     "Malutkie czarne owoce, delikatnie l�ni�ce i bardzo miekkie, wydzielaj�ce specyficzny zapach.",
	     60, PLANT_PART_FRUITS, 8, 9,
	     {0, 0, 20, 60, 10,
	      5, 0, 0, 0, 0,
	      0, 50, 0, 0, 0,
	      0, 0, 0, 10, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 10, 0,
	      100, 5, 5, 0, 0,
	      0, 0, 0, 40, 0,
	      0, 0, 20, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_SMALL_FOOD, "opis smaku",
	     0, "opis wypalenia"
	    },
	    {"roz�o�yste zielone k��cze", "roz�o�ystego zielonego k��cza", "roz�o�ystemu zielonemu k��czu", "roz�o�yste zielone k��cze", "roz�o�ystym zielonym k��czem", "roz�o�ystym zielonym k��czu",
	     "boberek tr�jlistkowy", "boberka tr�jlistkowego", "boberkowi tr�jlistkowemu", "boberek tr�jlistkowy", "boberkiem tr�jlistkowym", "boberku tr�jlistkowym",
	     "Malutkie, tr�jlistkowe ziele, zwane te� koniczyn�, czy kozio�kiem, s�ynne z przes�d�w o szcz�ciu, jakie przynosi znalezienie boberka... z czterema listkami.",
	     90, PLANT_PART_STALK, 5, 9,
	     {0, 0, 0, 5, 0,
	      0, 10, 10, 0, 0,
	      0, 10, 30, 0, 0,
	      0, 0, 0, 0, 5,
	      15, 0, 0, 0, 0,
	      0, 0, 0, 0, 15,
	      100, 0, 10, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 20, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_SMALL_FOOD, "opis smaku",
	     0, "opis wypalenia"
	    },
	    {"gar�� ma�ych czerwonych jag�d", "gar�ci ma�ych czerwonych jag�d", "gar�ci ma�ych czerwonych jag�d", "gar�� ma�ych czerwonych jag�d", "gar�ci� ma�ych czerwonych jag�d", "gar�ci ma�ych czerwonych jag�d",
	     "bor�wka czerwona", "bor�wki czerwonej", "bor�wce czerwonej", "bor�wk� czerwon�", "bor�wk� czerwon�", "bor�wce czerwonej",
	     "Owoce czerwone, kuliste jak jagody, cierpkie, gorzkawe i s�odkawo-kwa�ne. Nadaj� si� do spo�ycia. Sok z zielonych jag�d, zmieszany z wod�, jest doskona�ym napojem od�wie�aj�cym.",
	     65, PLANT_PART_BERRYS, 2, 9,
	     {0, 0, 5, 45, 20,
	      0, 0, 0, 0, 0,
	      0, 30, 20, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      100, 0, 5, 5, 0,
	      0, 0, 0, 10, 0,
	      0, 0, 10, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_SMALL_FOOD, "opis smaku",
	     0, "opis wypalenia"
	    },
	    {"gar�� ma�ych czarnych jag�d", "gar�ci ma�ych czarnych jag�d", "gar�ci ma�ych czarnych jag�d", "gar�� ma�ych czarnych jag�d", "gar�ci� ma�ych czarnych jag�d", "gar�ci ma�ych czarnych jag�d",
	     "bor�wka czarna", "bor�wki czarnej", "bor�wce czarnej", "bor�wk� czarn�", "bor�wk� czarn�", "bor�wce czarnej",
	     "Czarne owoce bor�wki s� kwaskowate, s�odkawe i troch� cierpkie. W medycynie ludowej jagody czarne s� znanym �rodkiem przeciwbiegunkowym.",
	     70, PLANT_PART_BERRYS, 6, 9,
	     {0, 0, 5, 55, 20,
	      0, 0, 0, 0, 0,
	      0, 40, 25, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      100, 0, 5, 5, 0,
	      0, 0, 0, 10, 0,
	      0, 0, 20, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_SMALL_FOOD, "opis smaku",
	     0, "opis wypalenia"
	    },
	    {"niewielki krzaczek z lancetowatymi li�ciami", "niewielkiego krzaczka z lancetowatymi li�ciami", "niewielkiemu krzeczkowi z lancetowatymi li�ciami", "niewielki krzeczek z lancetowatymi li�ciami", "niewielkim krzeczkiem z lancetowatymi li�ciami", "niewielkim krzeczku z lancetowatymi li�ciami",
	     "bylica estragon", "bylicy estragon", "bylicy estragon", "bylic� estragon", "bylic� estragon", "bylicy estragon",
	     "Krzaczek ten ma �odygi proste, na wp� zdrewnia�e, g�r� rozga��ziaj�ce si� i przechodz�ce w cienkie nibyk�osy i male�kie grona kwiat�w. Li�cie pod�u�ne, lancetowate, do�� liczne. Kwiaty male�kie, prawie kuliste, zielonkawo��te.",
	     180, PLANT_PART_BUSH, 35, 9,
	     {0, 0, 50, 10, 10,
	      0, 0, 0, 0, 0,
	      0, 5, 0, 0, 0,
	      0, 0, 0, 10, 5,
	      10, 0, 0, 0, 0,
	      0, 0, 0, 40, 0,
	      100, 0, 20, 0, 0,
	      0, 0, 0, 10, 0,
	      0, 0, 40, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_SMALL_FOOD, "opis smaku",
	     0, "opis wypalenia"
	    },
	    {"du�y szarozielony krzaczek", "du�ego szarozielonego krzaczka", "du�emu szarozielonemu krzaczkowi", "du�ego szarozielonego krzaczka", "du�ym szarozielonym krzaczkiem", "du�ym szarozielonym krzaczku",
	     "bylica pospolita", "bylicy pospolitej", "bylicy pospolitej", "bylic� pospolit�", "bylic� pospolit�", "bylicy pospolitej",
	     "Jest p�krzewem o g��wnym p�dzie p�zdrewnia�ym, rozga��ziaj�cym si�. Li�cie pierzastodzielne, coraz wy�ej jakby zanikaj�ce, z wierzchu ciemnozielone, spodem pokryte we�nistym bia�ym meszkiem skierowanym w po�udnie ku s�o�cu.",
	     45, PLANT_PART_BUSH, 20, 9,
	     {0, 0, 70, 25, 15,
	      10, 0, 0, 0, 0,
	      0, 20, 40, 5, 0,
	      0, 0, 0, 20, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 15, 0,
	      100, 10, 60, 40, 0,
	      10, 0, 0, 5, 0,
	      0, 0, 80, 0, 0,0,0,0,0,0},
	     0, "unknown",
	     0, "unknown"
	    },
	    {"kilka niewielkich czerwonych kwiatk�w", "kilku niewielkich czerwonych kwiatk�w", "kilku niewielkim czerwonym kwiatkom", "kilka niewielkich czerwonych kwiatk�w", "kilkoma niewielkimi czerwonymi kwiatkami", "kilku niewielkich czerwonych kwiatkach",
	     "centuria pospolita", "centurii pospolitej", "centurii pospolitej", "centuri� pospolit�", "centuri� pospolit�", "centurii pospolitej",
	     "�odygi w dolnej cz�ci tej ro�liny s� sze�ciok�tne, cz�ciowo puste, a w g�rnej cz�ci czterokrotne, w obr�bie kwiatostan�w rozga��zione. Li�cie, pod�u�ne, u�o�one naprzemian�egle, parami. Kwiaty r�owoczerwone.",
	     120, PLANT_PART_FLOWER, 45, 9,
	     {0, 0, 30, 20, 0,
	      0, 0, 0, 0, 0,
	      10, 10, 5, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 20,
	      100, 25, 15, 20, 0,
	      0, 0, 0, 10, 0,
	      25, 0, 30, 0, 0,0,0,0,0,0},
	     0, "unknown",
	     0, "unknown"
	    },
	    {"kilka niewielkich niebieskich kwiatk�w", "kilku niewielkich niebieskich kwiatk�w", "kilku niewielkim niebieskim kwiatkom", "kilka niewielkich niebieskich kwiatk�w", "kilkoma niewielkimi niebieskimi kwiatkami", "kilku niewielkich niebieskich kwiatkach",
	     "chaber b�awatek", "chabru b�awatka", "chabrowi b�awatkowi", "chaber b�awatek", "chabrem b�awatkiem", "chabrze b�awatku",
	     "Pospolity kwiat-chwast zbo�owy. Ro�nie tak�e na miedzach i nieu�ytkach, wyst�puje pojedynczo. Znany ze swoich w�a�ciwo�ci barwi�cych.",
	     45, PLANT_PART_FLOWER, 2, 9,
	     {0, 0, 90, 5, 25,
	      5, 0, 0, 0, 0,
	      0, 5, 0, 0, 0,
	      0, 0, 0, 5, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 20, 0,
	      100, 5, 60, 30, 0,
	      0, 0, 0, 15, 0,
	      0, 0, 80, 0, 0,0,0,0,0,0},
	     0, "unknown",
	     0, "unknown"
	    },
	    {"gruby bia�y korze�", "grubego bia�ego korzenia", "grubemu bia�emu korzeniowi", "gruby bia�y korze�", "grubym bia�ym korzeniem", "grubym bia�ym korzeniu",
	     "chrzan pospolity", "chrzanu pospolitego", "chrzanowi pospolitemu", "chrzan pospolity", "chrzanem pospolitym", "chrzanie pospolitym",
	     "Jest to poka�na ro�lina, o grubej �odydze, cz�sto zdrewnia�ej u spodu. Posiada ogromne li�cie. S�ynna z zastosowania jako przyprawa, po uprzednim odpowiednim przygotowaniu.",
	     90, PLANT_PART_ROOT, 35, 9,
	     {0, 0, 30, 5, 0,
	      0, 0, 0, 0, 0,
	      0, 5, 20, 0, 0,
	      0, 0, 0, 5, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 5, 0,
	      100, 0, 10, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 20, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_SM_SF, "unknown",
	     0, "unknown"
	    },
	    {"pomarszczony jasnobrunatny korze�", "pomarszczonego jasnobrunatnego korzenia", "pomarszczonemu jasnobrunatnemu korzeniowi", "pomarszczony jasnobrunatny korze�", "pomarszczonym jasnobrunatnym korzeniem", "pomarszczonym jasnobrunatnym korzeniu",
	     "cykoria podr�nik", "cykorii podr�nik", "cykorii podr�nik", "cykori� podr�nik", "cykori� podr�nik", "cykorii podr�nik",
	     "Dolne li�cie tej nietypowej ro�liny zebrane s� w swoiste rozety, pierzaste. G�rne natomiast - lancetowate. Kwiaty najcz�niej b��kitne. Niekt�rzy m�wi�, �e stanowi ona sk�adnik piwa... jako bawnik.",
	     25, PLANT_PART_ROOT, 45, 9,
	     {0, 0, 20, 10, 0,
	      0, 0, 0, 0, 0,
	      0, 5, 0, 0, 0,
	      0, 0, 0, 5, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 10, 0,
	      100, 0, 20, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 30, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_SMALL_FOOD, "unknown",
	     0, "unknown"
	    },
	    {"ma�y silnie rozga��ziony krzak", "ma�ego silnie rozga��zionego krzaka", "ma�emu silnie rozga��zionemu krzakowi", "ma�y silnie rozga��ziony krzak", "ma�ym silnie rozga��zionym krzakiem", "ma�ym silnie rozga��zionym krzaku",
	     "cz�ber ogrodowy", "cz�bru ogrodowego", "cz�browi ogrodowemu", "cz�ber ogrodowy", "cz�brem ogrodowym", "cz�brze ogrodowym",
	     "Silnie rozga��ziona, du�a, krzaczasta ro�lina. Li�cie ma w�skie, lancetowate. Kwiaty najcz�ciej pojedyncze, ma�e, bia�e, fio�kowe lub czerwonawe, wzrastaj�ce w k�tach li�ci.",
	     195, PLANT_PART_BUSH, 24, 9,
	     {0, 0, 30, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 5, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 10, 0,
	      100, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_CURE_POISON, "unknown",
	     0, "unknown"
	    },
	    {"ma�a bia�a cuchn�ca cebulka", "ma�ej bia�ej cuchn�cej cebulki", "ma�ej bia�ej cuchn�cej cebulce", "ma�� bia�� cuchn�c� cebulk�", "ma�� bia�� cuchn�c� cebulk�", "ma�ej bia�ej cuchn�cej cebulce",
	     "czosnek pospolity", "czosnku pospolitego", "czosnkowi pospolitemu", "czosnek pospolity", "czosnkiem pospolitym", "czosnku pospolitym",
	     "Niewielka cebulka wydzielaj�ca charakterystyczny zapach. Znana z pot�nych w�a�ciow�ci leczniczych. Niezast�piony �rodek przeciw wszelakim w�pierzom.",
	     40, PLANT_PART_BULB, 4, 9,
	     {0, 0, 50, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      100, 0, 10, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 20, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_SMALL_FOOD, "unknown",
	     0, "unknown"
	    },
	    {"kolba z�ocisto��tych kwiat�w", "kolby z�ocisto��tych kwiat�w", "kolbie z�ocisto��tych kwiat�w", "kolb� z�ocisto��tych kwiat�w", "kolb� z�ocisto��tych kwiat�w", "kolbie z�ocisto��tych kwiat�w",
	     "dziewanna wielokwiatowa", "dziewanny wielokwiatowej", "dziewannie wielokwiatowej", "dziewann� wielokwiatow�", "dziewann� wielokwiatow�", "dziewannie wielokwiatowej",
	     "Pi�kne, z�ocisto��te kwiatki o przyjemnym zapachu. Zwana tak�e ludowo gorzygrotem lub szabl�. Podobno mo�na z niej zrobi� smaczny napar.",
	     160, PLANT_PART_FLOWER, 55, 9,
	     {0, 0, 20, 5, 35,
	      5, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      100, 0, 5, 0, 0,
	      0, 0, 0, 5, 0,
	      0, 0, 40, 0, 0,0,0,0,0,0},
	     0, "unknown",
	     0, "unknown"
	    },
	    {"gar�� ��tych kwiatk�w", "gar�ci ��tych kwiatk�w", "gar�ci ��tych kwiatk�w", "gar�� ��tych kwiatk�w", "gar�ci� ��tych kwiatk�w", "gar�ci ��tych kwiatk�w",
	     "dziurawiec lekarski", "dziurawca lekarskiego", "dziurawcowi lekarskiemu", "dziurawiec lekarski", "dziurawcem lekarskim", "dziurawcu lekarskim",
	     "Niedu�y, charakterystyczy p�krzew. Kwiaty ��te, zebrane na szczytach p�d�w w niby-baldachy. Li�� dziurawca - ogl�dany pod �wiat�o - sprawia wra�enie podziurawionego szpilk�. W rzeczywisto�ci te niby male�kie otworki to zbiorniczki olejku - st�d nazwa tego ziela",
	     220, PLANT_PART_FLOWER, 10, 9,
	     {0, 0, 30, 40, 80,
	      60, 0, 0, 0, 0,
	      0, 15, 10, 30, 0,
	      0, 0, 0, 30, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 50, 0,
	      100, 0, 15, 10, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 70, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_SMALL_FOOD, "unknown",
	     0, "unknown"
	    },
	    {"malutki fioletowo��ty kwiatek", "malutkiego fioletowo��tego kwiatka", "malutkiemu fioletowo��temu kwiatkowi", "malutki fioletowo��ty kwiatek", "malutkim fioletowo��tym kwiatkiem", "malutkim fioletowo��tym kwiatku",
	     "fio�ek tr�jbarwny", "fio�ka tr�jbarwnego", "fio�kowi tr�jbarwnemu", "fio�ek tr�jbarwny", "fio�kiem tr�jbarwnym", "fio�ku tr�jbarwnym",
	     "Nie myli� tego fio�ka z fio�kiem polnym, kt�rego kwiaty s� mniejsze, a p�atki korony kwiatu nie s� wi�ksze od kielicha. Maj� te� inn� barw�. Kwiaty tr�jbarwne, g�rne p�atki fio�kowe, boczne ja�niejsze lub bia�e, a dolna warga ��ta.",
	     150, PLANT_PART_FLOWER, 10, 9,
	     {0, 0, 30, 30, 40,
	      20, 0, 0, 0, 0,
	      0, 10, 0, 5, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      100, 0, 20, 0, 0,
	      0, 0, 0, 60, 0,
	      0, 0, 70, 0, 0,0,0,0,0,0},
	     0, "unknown",
	     0, "unknown"
	    },
	    {"malutkie ��tawe kuleczki", "malutkich ��tawych kuleczek", "malutkim ��tawym kuleczkom", "malutkie ��tawe kuleczki", "malutkimi ��tawymi kuleczkami", "malutkich ��tawych kuleczkach",
	     "gorczyca bia�a", "gorczycy bia�ej", "gorczycy bia�ej", "gorczyc� bia��", "gorczyc� bia��", "gorczycy bia�ej",
	     "Ro�lina ta jest znana i stosowana od dawna. Ma jasno��te, lekko pikantne nasiona, kt�re s�u�� do aromatyzowania ryb, mi�sa, w�dlin, sos�w itp. �wie�e m�ode li�cie gorczycy s� spo�ywane niekiedy jako dodatek do sa�aty i kanapek.",
	     45, PLANT_PART_FRUITS, 20, 9,
	     {0, 0, 70, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 60, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      100, 0, 10, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 50, 0, 0,0,0,0,0,0},
	     0, "unknown",
	     0, "unknown"
	    },
	    {"gruby wrzecionowaty korze�", "grubego wrzecionowatego korzenia", "grubemu wrzecionowatemu korzeniowi", "gruby wrzecionowaty korze�", "grubym wrzecionowatym korzeniem", "grubym wrzecionowatym korzeniu",
	     "gorczyca z�ta", "gorczycy ��tej", "gorczycy ��tej", "gorczyc� ��t�", "gorczyc� ��t�", "gorczycy ��tej",
	     "Du�y, dziwny, wrzecionowaty korze� o bardzo intensywnym, ale przyjemnym zapachu. Wykorzystywany do preparowania ostrych przypraw przez ludno�� niekt�rych wsi.",
	     150, PLANT_PART_ROOT, 30, 9,
	     {0, 0, 0, 0, 30,
	      50, 0, 0, 0, 0,
	      0, 0, 70, 10, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      100, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 60, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_SMALL_FOOD, "unknown",
	     0, "unknown"
	    },
	    {"niewielka fioletowo ukwiecona bylina", "niewielkiej fioletowo ukwieconej bylinie", "niewielkiej fioletowo ukwieconej bylinie", "niewielk� fioletowo ukwiecon� bylin�", "niewielk� fioletowo ukwiecon� bylin�", "niewielkiej fioletowo ukwieconej bylinie",
	     "hyzop lekarski", "hyzopu lekarskiego", "hyzopowi lekarskiemu", "hyzop lekarski", "hyzopem lekarskim", "hyzopie lekarskim",
	     "Ta ro�lina jest p�krzewem ga��zistym o �odygach u do�u zdrewnia�ych i g�sto ulistnionych. Kwiaty posiada ciemnob��kitnofio�kowe, rzadziej bia�e lub r�owe. Jej wielkim walorem jest cudny, aromatyczny zapach.",
	     500, PLANT_PART_PERENNIAL, 70, 9,
	     {0, 0, 0, 0, 60,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      100, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 50, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_CURE_POISON, "unknown",
	     0, "unknown"
	    },
	    {"ma�a twarda ciemnogranatowa jagoda", "ma�ej twardej ciemnogranatowej jagody", "ma�ej twardej ciemnogranatowej jagodzie", "ma�� tward� ciemnogranatow� jagod�", "ma�� tward� ciemnogranatow� jagod�", "ma�ej twardej ciemnogranatowej jagodzie",
	     "ja�owiec pospolity", "ja�owca pospolitego", "ja�owcowi pospolitemu", "ja�owiec pospolity", "ja�owcem pospolitym", "ja�owcu pospolitym",
	     "Ogl�dasz wnikliwie ten wysmuk�y i wynios�y krzew iglasty. Pami�tasz, �e �ywotno�� jego przekracza �ycie cz�owieka, chocia� jego przyrosty s� bardzo ma�e. Ro�nie na ziemi ja�owej, piaszczystej, st�d jego nazwa, ale spotyka si� go te� cz�sto na torfowiskach i wrzosowiskach. Do ga��zki przytoczonych jest kilka du�ych, twardych, ciemnogranatowych owoc�w, kt�re wygl�daj� naprawd� �adnie.",
	     70, PLANT_PART_BERRYS, 1, 9,
	     {0, 0, 0, 60, 40,
	      0, 0, 0, 0, 0,
	      0, 70, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      100, 20, 40, 60, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 30, 0, 0,0,0,0,0,0},
	     0, "unknown",
	     0, "unknown"
	    },
	    {"ma�e bia�e kwiatki", "ma�ych bia�ych kwiatk�w", "ma�ym bia�ym kwiatkom", "ma�e bia�e kwiatki", "ma�ymi bia�ymi kwiatkami", "ma�ych bia�ych kwiatkach",
	     "jasnota bia�a", "jasnoty bia�ej", "jasnocie bia�ej", "jasnot� bia��", "jasnot� bia��", "jasnocie bia�ej",
	     "Ta ciekawa ro�lina jest niew�tpliwie jakim� chwastem. Przypomina troch� pokrzyw�, szczeg�lnie li�cie. Ma du�e, bia�e kwiaty, dziwnie �adne w por�wnaniu z reszt� ro�liny.",
	     10, PLANT_PART_FLOWER, 5, 9,
	     {0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 70, 0, 0,
	      0, 0, 0, 20, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 30, 0,
	      100, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 40, 0, 0,0,0,0,0,0},
	     0, "unknown",
	     0, "unknown"
	    },
	    {"niedu�e fioletowoczarne owoce", "niedu�ych fioletowoczarnych owoc�w", "niedu�ym fioletowoczarnym owocom", "niedu�e fioletowoczarne owoce", "niedu�ymi fioletowoczarnymi owocami", "niedu�ych fioletowoczarnych owocach",
	     "jezyna pospolita", "je�yny pospolitej", "je�ynie pospolitej", "je�yn� pospolit�", "je�yn� pospolit�", "je�ynie pospolitej",
	     "Te du�e, fioletowoczarne owoce wygl�daj� podobnie jak owoce maliny. S� jednak ciemne i o wiele twardsze, w smaku natomiast lekko kwa�ne, a nawet gorzkie. Jednak dojrza�e owoce bywaj� naprawd� mi�kkie i s�odkie. Owoc ten jest cz�sto przysmakiem wie�niak�w, s�u�y tak�e jako przyprawa.",
	     10, PLANT_PART_FRUITS, 2, 9,
	     {0, 0, 20, 50, 0,
	      0, 0, 0, 0, 0,
	      0, 10, 30, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      100, 0, 10, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 60, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_SMALL_FOOD, "unknown",
	     0, "unknown"
	    },
	    {"gar�� ma�ych suchych nasionek", "gar�ci ma�ych suchych nasionek", "gar�ci ma�ych suchych nasionek", "gar�� ma�ych suchych nasionek", "gar�ci� ma�ych suchych nasionek", "gar�ci ma�ych suchych nasionek",
	     "kolendra siewna", "kolendry siewnej", "kolendrze siewnej", "kolendr� siewn�", "kolendr� siewn�", "kolendrze siewnej",
	     "Te nasionka, niewielkie i suche, wygl�daj� naprawd� niepozornie. Jednak wystarczy je rozgnie��, czy rozetrze�, a zaczn� wydziela� delikatny, acz bardzo przyjemny zapach, podobny do mi�ty. Dzi�ki temu wykorzystywane s� jako sk�adniki aromatyczne i zapachowe.",
	     100, PLANT_PART_SEEDS, 25, 9,
	     {0, 0, 80, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      100, 50, 30, 40, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 60, 0, 0,0,0,0,0,0},
	     0, "unknown",
	     0, "unknown"
	    },
	    {"charakterystyczne d�oniaste li�cie", "charakterystycznych d�oniastych li�ci", "charakterystycznym d�oniastym li�ciom", "charakterystyczne d�oniaste li�cie", "charakterystycznymi d�oniastymi li�cmi", "charakterystycznych d�oniastych li�ciach",
	     "konopie", "konopi", "konopiom", "konopie", "konopiami", "konopiach",
	     "Te li�cie wygl�daj� naprawd� ciekawe. Maj� kszta�t d�oni. Poza tym nic tej ro�liny nie wyr�nia spo�r�d wielu innych.",
	     115, PLANT_PART_LEAF, 15, 9,
	     {0, 0, 60, 5, 0,
	      0, 0, 0, 0, 0,
	      0, 30, 10, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      100, 20, 50, 10, 0,
	      0, 0, 0, 10, 0,
	      0, 0, 80, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_HALLUCINATIONS, "unknown",
	     0, "unknown"
	    },
	    {"gar�� niewielkich sp�aszonych nasion", "gar�ci niewielkich sp�aszonych nasion", "gar�ci niewielkich sp�aszcznych nasion", "gar�� niewielkich sp�aszczonych nasion", "gar�ci� niewielkich sp�aszczonych nasion", "gar�ci niewielkich sp�aszczonych nasion",
	     "kozieradka pospolita", "kozieradki pospolitej", "kozieradce pospolitej", "kozieradk� pospolit�", "kozieradk� pospolit�", "kozieradce pospolitej",
	     "Niewielkie nasionka, przypominaj�ce kszta�tem fasolk�, czy groszek, musia�y znajdowa� si� kiedy� w jakim� str�ku. S� niewielkich rozmiar�w, owalne, br�zowe i dosy� twarde.",
	     75, PLANT_PART_SEEDS, 7, 9,
	     {0, 0, 50, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      100, 0, 40, 10, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 70, 0, 0,0,0,0,0,0},
	     0, "unknown",
	     0, "unknown"
	    },
	    {"liczne cienkie bia�e korzonki", "licznych cie�kich bia�ych korzonk�w", "licznym cienkim bia�ym korzonkom", "liczne cie�kie bia�e korzonki", "licznymi cienkimi bia�ymi korzonkami", "licznych cie�kich bia�ych korzonkach",
	     "koz�ek lekarski", "koz�ka lekarskiego", "koz�kowi lekarskiemu", "koz�ek lekarski", "koz�kiem lekarskim", "koz�ku lekarskim",
	     "Korzonki tej ro�liny wygl�daj� bardzo ciekawie. Jakby to by�y dziesi�tki grybych, bia�awych nici. S� dosy� mi�kkie, jakby znajdowa�a si� w nich jaka� ciecz. S�ynne z w�a�ciwo�ci medycznych, po uprzednim ususzeniu.",
	     160, PLANT_PART_ROOT, 33, 9,
	     {0, 0, 50, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 40, 0, 0,
	      0, 0, 0, 0, 20,
	      30, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      100, 0, 10, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 70, 0, 0,0,0,0,0,0},
	     0, "unknown",
	     0, "unknown"
	    },
	    {"bylina z pierzastosiecznymi li��mi", "byliny z pierzastosiecznymi li��mi", "bylinie z pierzastosiecznymi li��mi", "bylin� z pierzastosiecznymi li��mi", "bylin� z pierzastosiecznymi li��mi", "bylinie z pierzastosiecznymi li��mi",
	     "krwawnik pospolity", "krwawnika pospolitego", "krwawnikowi pospolitemu", "krwawnika pospolitego", "krwawnikiem pospolitym", "krwawniku pospolitym",
	     "Przygl�dasz si� tej ciekawej bylinie dok�adnie. Jest dosy� wysoka, jednak w�t�a. �ody�ki s� niezmiernie cienkie. A jednak jakim� sposobem utrzymuje du�e kwiatostany zebrane w nibybaldachimy.",
	     500, PLANT_PART_LEAF, 60, 9,
	     {0, 0, 10, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 25, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 40, 0,
	      100, 0, 30, 5, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 20, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_STOP_BLEEDING, "unknown",
	     0, "unknown"
	    },
	    {"ma�y krzaczek wraz z korzeniem", "ma�ego krzaczka wraz z korzeniem", "ma�emu krzaczkowi wraz z korzeniem", "ma�y krzaczek wraz z korzeniem", "ma�ym krzacziem wraz z korzeniem", "ma�ym krzaczki wraz z korzeniem",
	     "lubczyk ogrodowy", "lubczyku ogrodowego", "lubczykowi ogrodowemu", "lubczyk ogrodowy", "lubczykiem ogrodowym", "lubczyku ogrodowym",
	     "Zwyk�a polna ro�lina, o ciemno��tych kwiatach. Sta�a si� s�ynna dzi�ki swoim w�a�ciwo�ciom aromatycznym. Jest cz�stym sk�adnikiem zup i innych potraw. Szczeg�lnie popularna na wsi. Kr��y tak�e legenda o jej dzia�aniu, podobnym do afrodyzjaku.",
	     250, PLANT_PART_BUSH, 25, 9,
	     {0, 0, 40, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      100, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 30, 0, 0,0,0,0,0,0},
	     0, "unknown",
	     0, "unknown"
	    },
	    {"niewielkie pachn�ce ro�linki", "niewielkich pachn�cych ro�linek", "niewielkim pachn�cym ro�linkom", "niewielkie pachn�ce ro�linki", "niewielkimi pachn�cymi ro�linkami", "niewielkich pachn�cych ro�linkach",
	     "macierzanka piaskowa", "macierzanki piaskowej", "macierzance piaskowej", "macierzank� piaskow�", "macierzank� piaskow�", "macierzance piaskowej",
	     "Ro�lina ta zwana jest tak�e cz�berkiem. Jej wygl�d jest naprawd� osobliwy. S� to liczne ma�e ro�linki, zebrane w du�e k��cza. Jednak nie to jest najdziwniejsze. Ro�lina ta wyzdiela bardzo intensywny zapach, charakterystyczny aromat, dzi�ki czemu jest cz�sto wykorzystywana jako sk�adnik wielu olejk�w zapachowych i ma�ci rozgrzewaj�cych.",
	     120, PLANT_PART_BUSH, 11, 9,
     {0, 0, 0, 50, 0,
	     0, 0, 0, 0, 0,
	     0, 60, 0, 0, 0,
	     0, 0, 0, 0, 0,
	     0, 0, 0, 0, 0,
	     0, 0, 0, 0, 0,
	     100, 0, 0, 0, 0,
	     0, 0, 0, 0, 0,
	     0, 0, 20, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_SM_SF, "unknown",
	     0, "unknown"
     },
     {"ma�y pachn�cy krzaczek", "ma��go pachn�cego krzaczka", "ma�emu pachn�cemu krzaczkowi", "ma�y pachn�cy krzaczek", "ma�ym pachn�cym krzaczkiem", "ma�ym pachn�cym krzaczku",
	     "majeranek ogrodowy", "majeranku ogrodowego", "majerankowi ogrodowemu", "majeranek ogrodowy", "majerankiem ogrodowym", "majeranku ogrodowym",
	     "Niewielka ro�linka, rosn�ca w formie zwartego krzaczka. Jest to jedena z najpopularniejszych przypraw znanych na �wiecie. Mo�na j� doda� do ka�dego dania! Znane s� tak�e jej w�a�ciwo�ci lecznicze, szczeg�lnie pozytywnie wp�ywa na �o��dek.",
		     70, PLANT_PART_BUSH, 5, 9,
		     {0, 0, 50, 5, 0,
		     0, 0, 0, 0, 0,
		     0, 10, 0, 0, 0,
		     0, 0, 0, 0, 0,
		     0, 0, 0, 0, 0,
		     0, 0, 0, 0, 0,
		     100, 0, 20, 0, 0,
		     0, 0, 0, 0, 0,
		     0, 0, 60, 0, 0,0,0,0,0,0},
		     PLANT_EFFECT_SM_SF, "unknown",
		     0, "unknown"
	     },
		     {"du�y czerwony kwiat na �odydze", "du�ego czerwonego kwiatu na �odydze", "du�emu czerwonemu kwiatu na �odydze", "du�y czerwony kwiat na �odydze", "du�ym czerwonym kwiatem na �odydze", "du�ym czerwonym kwiecie na �odydze",
		     "mak pospolity", "maku pospolitego", "maku pospolitemu", "mak pospolity", "makiem pospolitym", "maku pospolitym",
		     "Ten charakterystyczny kwiat zna ka�de dziecko. Ogromne, czerwone p�atki s�u�� g��wnie do ozdoby w�os�w wiejskich dziewczyn. Mo�na go spotka� na ka�dej ��ce.",
			     15, PLANT_PART_FLOWER, 1, 9,
		     {0, 0, 90, 0, 40,
			     0, 0, 0, 0, 0,
			     0, 0, 0, 0, 0,
			     0, 0, 0, 20, 0,
			     0, 0, 0, 0, 0,
			     0, 0, 0, 40, 0,
			     100, 0, 50, 0, 0,
			     10, 0, 0, 0, 0,
			     0, 0, 100, 0, 0,0,0,0,0,0},
			     PLANT_EFFECT_SMALL_FOOD, "unknown",
			     0, "unknown"
		     },
		     {"du�e purpurowoczarne kwiaty", "du�ych purpurowoczarnych kwiat�w", "du�ym purpurowoczarnym kwiatom", "du�e purpurowoczarne kwiaty", "du�ymi purpurowoczarnymi kwiatami", "du�ych purpurowoczarnych kwiatach",
			     "malwa czarna", "malwy czarnej", "malwie czarnej", "malw� czarn�", "malw� czarn�", "malwie czarnej",
			     "Ten du�y, ciemnogranatowy kwiat wygl�da naprawd� majestatycznie. Ciemna barwa jego p�atk�w kontrastuje z jasnozielon� �odyg�. Wykorzystywany jest najcz�ciej jako barwnik, oczywi�cie ciemny. Ludowi znachowy wykorzystywali wywar z tych kwiat�w tak�e jako �rodek pomagaj�cy kobiecie w czasie jej jak�e kobiecych dolegliwo�ci.",
			     120, PLANT_PART_FLOWER, 5, 9,
		     {0, 0, 20, 0, 20,
			     0, 0, 0, 0, 0,
			     0, 0, 0, 0, 0,
			     0, 0, 0, 0, 0,
			     0, 0, 0, 0, 0,
			     0, 0, 0, 0, 0,
			     100, 0, 20, 0, 0,
			     0, 0, 0, 10, 0,
			     0, 0, 50, 0, 0,0,0,0,0,0},
			     0, "unknown",
			     0, "unknown"
		     },
		     {"cienkie pe�zaj�ce k��cze", "cie�skiego pe�zaj�cego k��cza", "cie�skiemu pe�zaj�cemu k��czu", "cie�kie pe�zaj�ce k��cze", "cie�kim pe�zaj�cym k��czem", "cie�kim pe�zaj�cym k��czu",
			     "marzanka wonna", "marzanki wonnej", "marzance wonnej", "marzank� wonn�", "marzank� wonn�", "marzance wonnej",
			     "Niewielka i niepozorna ro�lina, sk�adaj�ca si� g��wnie z d�ugich �odych i wielkich li�ci. Wykorzystywana od wiek�w w medycynie ludowej. Po ususzeniu zaczyna wydziela� przyjemny zapach, podobny do siana. St�d jej nazwa.",
				     150, PLANT_PART_STALK, 8, 9,
			     {0, 0, 0, 5, 0,
				     0, 0, 0, 0, 0,
				     0, 20, 60, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     100, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,0,0,0,0,0},
				     0, "unknown",
				     0, "unknown"
			     },
			     {"ma�e okr�g�e sk�rzaste li�cie", "ma�ych okr�g�ych sk�rzastych li�ci", "ma�ym okr�g�ym sk�rzastym li�ciom", "ma�e okr�g�e sk�rzaste li�cie", "ma�ymi okr�g�ymi sk�rzastymi li��mi", "ma�ych okr�g�ych sk�rzastych li�ciach",
				     "m�cznica lekarska", "m�cznicy lekarskiej", "m�cznicy lekarskiej", "m�cznic� lekarsk�", "m�cznic� lekarsk�", "m�cznicy lekarskiej",
				     "Najwa�niejszym elementem tej ro�linki s� jej ma�e, okr�g�e li�cie. W dotyku przypominaj� sk�r�. Jej jedynym zastosowaniem jest medycyna. Li�cie mo�na zagotowa�, stworzy� napar, kt�ry �wietnie dzia�a na drogi moczowe.",
				     185, PLANT_PART_LEAF, 26, 9,
			     {0, 0, 0, 50, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 40, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     100, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,0,0,0,0,0},
				     0, "unknown",
				     0, "unknown"
			     },
			     {"bylina z owalnosercowatymi li��mi", "byliny z owalnosercowatymi li��mi", "byliny z owalnosercowatymi li��mi", "bylin� z owalnosercowatymi li��mi", "bylin� z owalnosercowatymi li��mi", "bylinie z owalnosercowatymi li��mi",
				     "melisa lekarska", "melisy lekarskiej", "melisie lekarskiej", "melis� lekarsk�", "melis� lekarsk�", "melisie lekarskiej",
				     "Te niewielkie listki, troszk� przypominaj�ce kszta�tem serce, s� jedn� z najstarszych i najcz�ciej stosowanych ro�lin lekarskich. Uwarzony z nich napar dzia�a relaksuj�co, odstresowuj�co i oczyszczaj�co.",
				     220, PLANT_PART_LEAF, 22, 9,
			     {0, 0, 40, 25, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     100, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 20, 0, 0,0,0,0,0,0},
				     0, "unknown",
				     0, "unknown"
			     },

			     {"pachn�ce sercowate li�cie", "pachn�cych sercowatych li�ci", "pachn�cym sercowatym li�ciom", "pachn�ce sercowate li�cie", "pachn�cymi sercowatymi li��mi", "pachn�cych sercowatych li�ciach",
				     "mi�ta", "mi�ty", "mi�cie", "mi�t�", "mi�t�", "mi�cie",
				     "Przygl�daj�c si� tym sercowatym li��iom czujesz naprawd� przyjemny zapach. Gdy je lekko pocierasz i ugniatasz, to aromat si� nasila. Dlatego te� podr�nicy cz�sto stosuj� to ziele w czasie podr�y, bowiem dzia�a koj�co i uspakajaj�co. Jest te� znan� przypraw�.",
				     100, PLANT_PART_LEAF, 27, 9,
			     {0, 0, 10, 50, 0,
				     0, 0, 0, 0, 0,
				     0, 30, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     100, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 5, 0, 0,0,0,0,0,0},
				     PLANT_EFFECT_HIGH_MOVE, "unknown",
				     0, "unknown"

			     },
			     {"mlecz", "mlecza", "mleczowi", "mlecz", "mleczem", "mleczu",
				     "mniszek pospolity", "mniszka pospolitego", "mniszkowi pospolitemu", "mniszek pospolity", "mniszkiem pospolitym", "mniszku pospolitym",
				     "Jeden z najpopularniejszych chwast�w polnych, zwany potocznie mleczem - od swoistego 'mleczka' p�yn�cego w jego �odydze. Kt� z nas w dzieci�stwie nie bawi� si� nasionami mleczy - spadachronami - kt�re zdmuchiwa� prosto na twarz kole�anek i koleg�w ? Niewielu natomiast zdaje sobi� spraw� z w�a�ciwo�ci moczop�dnych tej ro�liny, je�eli zostanie odpowiednio przygotowana i podana.",
				     25, PLANT_PART_FLOWER, 1, 9,
			     {0, 0, 80, 20, 70,
				     40, 0, 0, 0, 0,
				     0, 15, 0, 0, 0,
				     0, 0, 0, 90, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 70, 0,
				     100, 0, 60, 50, 0,
				     0, 0, 0, 90, 0,
				     0, 0, 70, 0, 0,0,0,0,0,0},
				     0, "unknown",
				     0, "unknown"
			     },
			     {"walcowate rozga��zione zielone k��cze", "walcowatego rozga��zionego zielonego k��cza", "walcowatemu rozga��zionemu zielonemu k��czu", "walcowate rozga��zione zielone k��cze", "walcowatym rozga��zionym zielonym k��czem", "walcowatym rozga��zionym zielonym k��czu",
				     "mydlnica lekarska", "mydlnicy lekarskiej", "mydlnicy lekarskiej", "mydlnic� lekarsk�", "mydlnic� lekarsk�", "mydlnicy lekarskiej",
				     "Ta ciekawa ro�linka wygl�da jak zwyk�y polny kwiatek, z niewielkimi bia�ymi kwiatkami. Jednak jej sekret tkwi w korzeniu, kt�ry to ma w�a�ciow�ci myj�ce, niczym myd�o. St�d wzi�a si� te� nazwa tego ziela.",
				     250, PLANT_PART_STALK, 55, 9,
			     {0, 0, 50, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 50, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     100, 0, 20, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 60, 0, 0,0,0,0,0,0},
				     0, "unknown",
				     0, "unknown"
			     },
			     {"du�y pomara�czowy kwiat", "du�ego pomara�czowego kwiatu", "du�emu pomara�czowemu kwiatu", "du�y pomara�czowy kwiat", "du�ym pomara�czowym kwiatem", "du�ym pomara�czowym kwiecie",
				     "nagietek lekarski", "nagietka lekarskiego", "nagietkowi lekarskiemu", "nagietka lekarskiego", "nagietkiem lekarskim", "nagietku lekarskim",
				     "Dosy� pospolita ro�lina, �redniej wielko�ci. Jej du�e, ��te kwiaty mo�na cz�sto zobaczy� na pospolitej ��ce. Dlatego ludzie wykorzystuj� j� g��wnie w celach ozdobnych. Czasami wykorzystywana jako sk�adnik r�nych mikstur leczniczych.",
				     69, PLANT_PART_FLOWER, 5, 9,
			     {0, 0, 0, 0, 15,
				     5, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     100, 0, 15, 0, 0,
				     0, 0, 0, 10, 0,
				     0, 0, 25, 0, 0,0,0,0,0,0},
				     PLANT_EFFECT_AID, "unknown",
				     0, "unknown"
			     },
			     {"niedu�a bylina z ��tym kwiatostanem", "niedu�ej byliny z ��tym kwiatostanem", "niedu�ej bylinie z ��tym kwiatostanem", "niedu�� bylin� z ��tym kwiatostanem", "niedu�� bylin� z ��tym kwiatostanem", "niedu�ej bylinie z ��tym kwiatostanem",
				     "naw�o� pospolita", "naw�oci pospolitej", "naw�oci pospolitej", "naw�o� pospolit�", "naw�oci� pospolit�", "naw�oci pospolitej",
				     "Ogl�dasz t� pospolit� poln� bylin� i stwierdzasz, �e nie ma w niej nic ciekawego, poza charakterystyczn� �odyg�, kt�ra mo�e by� nawet fioletowa. Od niej odchodzi wiele mniejszych �ody�ek, na ko�cu kt�rych rosn� ma�e, zgrabne ��te kwiatuszki.",
				     33, PLANT_PART_PERENNIAL, 3, 9,
			     {0, 0, 30, 40, 0,
				     0, 0, 0, 0, 0,
				     0, 50, 70, 0, 0,
				     0, 0, 0, 20, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 30, 0,
				     100, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 40, 0, 0,0,0,0,0,0},
				     0, "unknown",
				     0, "unknown"
			     },
			     {"drobne zwisaj�ce ��te kwiaty", "drobnych zwisaj�cych ��tych kwiat�w", "drobnym zwisaj�cym ��tym kwiatom", "drobne zwisaj�ce ��te kwiaty", "drobnymi zwisaj�cymi ��tymi kwiatami", "drobnych zwisaj�cych ��tych kwiatach",
				     "nostrzyk lekarski", "nostrzyka lekarskiego", "nostrzykowi lekarskiemu", "nostrzyk lekarski", "nostrzykiem lekarskim", "nostrzyku lekarskim",
				     "Jest to ro�lina o �odydze wzniesionej, �redniej wysoko�ci, naga lub w g�rze ow�osiona. Li�cie o trzech listkach rombowojajowatych lub lancetowatych. Posiada ��dne, bia�e kwiaty motylkowe.",
				     135, PLANT_PART_FLOWER, 65, 9,
			     {0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 80, 0, 0,
				     0, 0, 0, 50, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 70, 0,
				     100, 0, 40, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 90, 0, 0,0,0,0,0,0},
				     0, "unknown",
				     0, "unknown"
			     },
			     {"niebieskofio�kowe drobne kwiatki", "niebieskofio�kowych drobnych kwiatk�w", "niebieskofio�kowym drobnym kwiatkom", "niebieskofio�kowe drobne kwiatki", "niebieskofio�kowymi drobnymi kwiatkami", "niebieskofio�kowych drobnych kwiatkach",
				     "og�recznik lekarski", "og�recznika lekarskiego", "og�recznikowi lekarskiemu", "og�recznika lekarskiego", "og�recznikiem lekarskim", "og�reczniku lekarskim",
				     "Ro�lina ta jest bardzo dziwna, ale intryguj�ca. Ca�a pokryta jest wyra�nie widocznymi w�oskami i po roztarciu wydziela intensywny zapach przypominaj�cy zapach �wie�ych og�rk�w. W okresie kwitnienia na ro�linie pojawiaj� si� stosunkowo du�e, niebieskie kwiaty zebrane w zwisaj�ce, baldachowate, kwiatostany. Owocem jest pod�u�na, ciemnobr�zowa nibyszyszka.",
				     675, PLANT_PART_FLOWER, 75, 9,
			     {0, 0, 40, 10, 0,
				     0, 0, 0, 0, 0,
				     0, 5, 20, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     100, 0, 50, 0, 0,
				     0, 0, 0, 60, 0,
				     0, 0, 60, 0, 0,0,0,0,0,0},
				     0, "unknown",
				     0, "unknown"
			     },
			     {"spory pomara�czowy kwiat", "sporego pomara�czowego kwiatu", "sporemu pomara�czowemu kwiatowi", "spory pomara�czowy kwiat", "sporym pomara�czowym kwiatem", "sporym pomara�czowym kwiecie",
				     "oman wielki", "omanu wielkiego", "omanowi wielkiemu", "oman wielki", "omanem wielkim", "omanie wielkim",
				     "Przygl�dasz si� dosy� d�ugo temu du�emu, pomara�czowemu kwiatu, bowiem jest on dosy� dziwny. Wygl�da tak, jakby dwa kolory walczy�y w nim o pierwsze�stwo - zielony z pomara�czowym. Jednak poza tym, jest to ca�kiem zwyczajna ro�lina, wydzielaj�ca lekki, przyjemny zapach.",
				     750, PLANT_PART_FLOWER, 95, 9,
			     {0, 0, 0, 30, 0,
				     0, 0, 0, 0, 0,
				     0, 5, 60, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     100, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 40, 0, 0,0,0,0,0,0},
				     0, "unknown",
				     0, "unknown"
			     },
			     {"d�ugie trawiaste g�adkie li�cie", "d�ugich trawiastych g�adkich li�ci", "d�ugim trawiastym g�adkim li�ciom", "d�ugie trawiaste g�adkie li�cie", "d�ugimi trawiastymi g�adkimi li��mi", "d�ugich trawiastych g�adkich li�ciach",
				     "perz w�a�ciwy", "perzu w�a�ciwego", "perzowi w�a�ciwemu", "perz w�a�ciwy", "perzem w�a�ciwym", "perzu w�a�ciwym",
				     "Ro�lina ta, podobna do trawy, potrafi rosn�� prawie wsz�dzie. Wciska si� w najmniejsze za�omy skalne i wykorzystuje ka�dy skrawek ziemi. S�ynne s� jej ogromne korzenie, kilkukrotnie wi�ksze i d�u�sze ni� sama ro�lina.",
					     1, PLANT_PART_LEAF, 1, 9,
				     {0, 0, 90, 0, 30,
					     0, 0, 0, 0, 0,
					     0, 0, 0, 0, 0,
					     0, 0, 0, 10, 0,
					     0, 0, 0, 0, 0,
					     0, 0, 0, 30, 0,
					     100, 0, 80, 0, 0,
					     0, 0, 0, 70, 0,
					     0, 0, 70, 0, 0,0,0,0,0,0},
					     0, "unknown",
					     0, "unknown"
				     },
				     {"du�e sercowate z�bkowane li�cie", "du�ych sercowatych z�bkowanych li�ci", "du�ym sercowatym z�bkowanym li�ciom", "du�e sercowate z�bkowane li�cie", "du�ymi sercowatymi z�bkowatymi li��mi", "du�ych sercowatych z�bkowanych li�ciach",
					     "podbia� pospolity", "podbia�u pospolitego", "podbia�owi pospolitemu", "podbia� pospolity", "podbia�em pospolitym", "podbiale pospolitym",
					     "Li�cie te nale�a�y kiedy� do dziwnej i charakterystycznej ro�liny, o wielkich ��tych kwiatach. S� one �wiatnym sk�adnikiem wielu mikstur i lek�w. Maj� wszechstronne w�a�ciwo�ci.",
						     75, PLANT_PART_LEAF, 19, 9,
					     {0, 0, 0, 0, 0,
						     0, 0, 0, 0, 0,
						     0, 50, 80, 0, 0,
						     0, 0, 0, 30, 0,
						     0, 0, 0, 0, 0,
						     0, 0, 0, 50, 0,
						     100, 0, 40, 0, 0,
						     0, 0, 0, 0, 0,
						     0, 0, 30, 0, 0,0,0,0,0,0},
						     0, "unknown",
						     0, "unknown"
					     },
					     {"zielone parz�ce li�cie", "zielonych parz�cych li�ci", "zielonym parz�cym li�ciom", "zielone parz�ce li�cie", "zielonymi parz�cymi li��mi", "zielonych parz�cych li�ciach",
						     "pokrzywa zwyczajna", "pokrzywy zwyczajnej", "pokrzywie zwyczajnej", "pokrzyw� zwyczajn�", "pokrzyw� zwyczajn�", "pokrzywie zwyczajnej",
						     "Charakterystyczna ro�lina polna, o wielu w�a�ciwo�ciach, tak�e medycznych. Jednak wykorzystywana jest g��wnie przez dzieci, kt�re chc� poparzy� ni� swoich r�wie�nik�w, bowiem jej li�cie zawieraj� specjalne substancje podra�niaj�ce na kr�tki czas sk�r�.",
						     8, PLANT_PART_LEAF, 1, 9,
					     {0, 0, 80, 50, 20,
						     0, 0, 0, 0, 0,
						     0, 80, 70, 0, 0,
						     0, 0, 0, 50, 0,
						     0, 0, 0, 0, 0,
						     0, 0, 0, 80, 0,
						     100, 0, 80, 0, 0,
						     0, 0, 0, 40, 0,
						     0, 0, 90, 0, 0   ,0,0,0,0,0},
						     PLANT_EFFECT_MINUS_MINOR_HP, "unknown",
						     0, "unknown"
					     },
					     {"drobniutkie zielone listki", "drobniutkich zielonych listk�w", "drobniutkim zielonym listkom", "drobniutkie zielone listki", "drobniutkimi zielonymi listkami", "drobniutkich zielonych listkach",
						     "po�onicznik nagi", "po�onicznika nagiego", "po�onicznikowi nagiemu", "po�onicznik nagi", "po�onicznikiem nagim", "po�oniczniku nagim",
						     "Niewielka ro�linka, intensywnie zielona, o r�wnie ma�ych listkach. Bardzo trudna do odnalezienia. Jednak posiada silne w�a�ciwo�ci moczop�dne.",
							     315, PLANT_PART_LEAF, 15, 9,
						     {0, 0, 0, 0, 0,
							     0, 0, 0, 0, 0,
							     0, 30, 0, 0, 0,
							     0, 0, 0, 0, 0,
							     0, 0, 0, 0, 0,
							     0, 0, 0, 0, 0,
							     100, 0, 60, 0, 0,
							     0, 0, 0, 0, 0,
							     0, 0, 30, 0, 0,0,0,0,0,0},
							     0, "unknown",
							     0, "unknown"
						     },
						     {"gar�� delikatnych czarnych owoc�w", "gar�ci delikatnych czarnych owoc�w", "gar�ci delikatnych czarnych owoc�w", "gar�� delikatnych czarnych owoc�w", "gar�ci� delikatnych czarnych owoc�w", "gar�ci delikatnych czarnych owoc�w",
							     "porzeczka czarna", "porzeczki czarnej", "porzeczce czarnej", "porzeczk� czarn�", "porzeczk� czarn�", "porzeczce czarnej",
							     "Te niewielkie, mi�ciutkie owoce pe�ne s� s�odkiego soku. Wykorzystywane s� najcz�ciej jako barwnik, lub po prostu jako pokarm, szczeg�lnie deserowy.",
							     55, PLANT_PART_FRUITS, 2, 9,
						     {0, 0, 0, 30, 0,
							     0, 0, 0, 0, 0,
							     0, 70, 0, 0, 0,
							     0, 0, 0, 40, 0,
							     0, 0, 0, 0, 0,
							     0, 0, 0, 0, 0,
							     100, 0, 0, 0, 0,
							     0, 0, 0, 40, 0,
							     0, 0, 0, 0, 0,0,0,0,0,0},
							     PLANT_EFFECT_SMALL_FOOD, "unknown",
							     0, "unknown"
						     },
						     {"gar�� ma�ych czarnych owoc�w", "gar�ci ma�ych czarnych owoc�w", "gar�ci ma�ych czarnych owoc�w", "gar�� ma�ych czarnych owoc�w", "gar�ci� ma�ych czarnych owoc�w", "gar�ci ma�ych czarnych owoc�w",
							     "poziomka pospolita", "poziomki pospolitej", "poziomce pospolitej", "poziomk� pospolit�", "poziomk� pospolit�", "poziomce pospolitej",
							     "Owoce te s� bardzo ma�e, przypominaj� malutkie truskawki. S� tak�e koloru czerwonego. W smaku bardzo s�odkie.",
							     55, PLANT_PART_FRUITS, 2, 9,
						     {0, 0, 0, 80, 0,
							     0, 0, 0, 0, 0,
							     0, 50, 0, 0, 0,
							     0, 0, 0, 40, 0,
							     0, 0, 0, 0, 0,
							     0, 0, 0, 0, 0,
							     100, 0, 0, 0, 0,
							     0, 0, 0, 40, 0,
							     0, 0, 0, 0, 0,0,0,0,0,0},
							     PLANT_EFFECT_SMALL_FOOD, "unknown",
							     0, "unknown"
						     },
						     {"niewielkie owalne li�cie", "niewielkich owalnych li�ci", "niewielkim owalnym li�ciom", "niewielkie owalne li�cie", "niewielkimi owalnymi li��mi", "niewielkich owalnych li�ciach",
							     "przetacznik lekarski", "przetacznika lekarskiego", "przetacznikowi lekarskiemu", "przetacznik lekarski", "przetacznikiem lekarskim", "przetaczniku lekarskim",
							     "Ogl�dasz te ciekawe kwiaty, u�o�one w w�owatych, d�ugich gronach i podziasz ich �agodn�, niebiesk� barw�. Na wsi nazywa si� j� 'lila', od koloru pewnego specjalnego gatunku lilji. Jednak okre�lenie 'ciemny b��kit nieba' pasuje tu r�wnie dobrze.",
							     172, PLANT_PART_LEAF, 25, 9,
						     {0, 0, 0, 60, 0,
							     0, 0, 0, 0, 0,
							     0, 20, 0, 0, 0,
							     0, 0, 0, 0, 0,
							     0, 0, 0, 0, 0,
							     0, 0, 0, 0, 0,
							     100, 0, 0, 0, 0,
							     0, 0, 0, 0, 0,
							     0, 0, 50, 0, 0,0,0,0,0,0},
							     0, "unknown",
							     0, "unknown"
						     },
	   {"w�skie sk�rzane listki", "w�skich sk�rzanych listk�w", "w�skich sk�rzanym listkom", "w�skie sk�rzane listki", "w��kimi sk�rzanymi listkami", "w�skich sk�rzanych listkach",
	   "rozmaryn lekarski", "rozmarynu lekarskiego", "rozmarynowi lekarskiemu", "rozmaryn lekarski", "rozmarynem lekarskim", "rozmarynie lekarskim",
							     "W�skie listki, kt�re ogl�dasz nale�� do wyj�tkowej ro�liny. W ludowej tradycji uchodzi�a ona za symbol mi�o�ci, ale tak�e i �mierci. Stosowano j� r�wnie� jako ozdob� i do sporz�dzania kadzide�. Obecnie ceniona jest jako ro�lina przyprawowa. ",
									     231, PLANT_PART_LEAF, 15, 9,
								     {0, 0, 0, 10, 0,
									     0, 0, 0, 0, 0,
									     0, 30, 0, 0, 0,
									     0, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,
									     100, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,0,0,0,0,0},
									     0, "unknown",
									     0, "unknown"
								     },
								     {"niewielkie niebieskozielone li�cie", "niewielkich niebieskozielonych li�ci", "niewielkim niebieskozielonym li�ciom", "niewielkie niebieskozielone li�cie", "niewielkimi niebieskozielonymi li��mi", "niewielkich niebieskozielonych li�ciach",
									     "ruta zwyczajna", "ruty zwyczajnej", "rucie zwyczajnej", "rut� zwyczajn�", "rut� zwyczajn�", "rucie zwyczajnej",
									     "Ro�lina ta jest uprawiana jako ro�lina ozdobna lub dla cel�w leczniczych. Ma silny aromatyczny zapach i kwitnie drobnymi ��tozielonymi kwiatkami. Kiedy� uchodzi�a za symbol niewinno�ci, dziewictwa.",
									     67, PLANT_PART_LEAF, 10, 9,
								     {0, 0, 45, 0, 40,
									     0, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,
									     100, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,0,0,0,0,0},
									     0, "unknown",
									     0, "unknown"
								     },
								     {"zielony jode�kowaty chwast", "zielonego jode�kowatego chwastu", "zielonemu jode�kowatemu chwastowi", "zielony jode�kowaty chwast", "zielonym jode�kowatym chwastem", "zielonym jode�kowatym chwa�cie",
									     "skrzyp polny", "skrzypu polnego", "skrzypowi polnemu", "skrzyp polny", "skrzypem polnym", "skrzypie polnym",
									     "Skrzyp �atwo pozna� - wyra�nie r�ni si� od powszechnie spotykanych ro�lin. Jego letnie p�dy przypominaj� miniaturowe choinki. Intensywnie zielone, szorstkie lodygi, przy pocieraniu jeden o drug� wydaj� charakterystyczny dzwi�k, skrzypienie.",
									     1, PLANT_PART_STALK, 1, 9,
								     {0, 0, 100, 0, 0,
									     0, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,
									     0, 0, 0, 30, 0,
									     0, 0, 0, 0, 0,
									     0, 0, 0, 40, 0,
									     100, 0, 30, 0, 0,
									     0, 0, 0, 20, 0,
									     0, 0, 50, 0, 0,0,0,0,0,0},
									     0, "unknown",
									     0, "unknown"
								     },
								     {"ciemnozielone tr�jk�tnoowalne li�cie", "ciemnozielonych tr�jk�tnoowalnych li�ci", "ciemnozielonym tr�jk�tnoowalnym li�ciom", "ciemnozielone tr�jk�tnoowalne li�cie", "ciemnozielonymi tr�jk�tnoowalnymi li��mi", "ciemnozielonych tr�jk�tnoowalnych li�ciach",
									     "�wietlik ��kowy", "�wietlika ��kowego", "�wietlikowi ��kowemu", "�wietlik ��kowy", "�wietlikiem ��kowym", "�wietliku ��kowym",
									     "Ciekawe listki, wygl�daj� tak, jakby kto� lekko �ci�� tr�jk�ty, po��czone ze sob� bokami. Sama ro�lina s�ynie z tego, �e �yje p�paso�ytniczo na korzeniach innych ro�lin ��kowych. Kwiaty na tle zieleni, przypominaj� �wieczki.",
									     165, PLANT_PART_LEAF, 9, 9,
								     {0, 0, 15, 35, 70,
									     0, 0, 0, 0, 0,
									     0, 5, 0, 0, 0,
									     0, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,
									     100, 0, 15, 0, 0,
									     0, 0, 0, 0, 0,
									     0, 0, 75, 0, 0,0,0,0,0,0},
									     0, "unknown",
									     0, "unknown"
								     },
								     {"rozeta z�bkowanych pierzastych li�ci", "rozety z�bkowanych pierzastych li�ci", "rozecie z�bkowanych pierzastych li�ci", "rozet� z�bkowanych pierzastych li�ci", "rozet� z�bkowanych pierzastych li�ci", "rozecie z�bkowanych pierzastych li�ci",
									     "tasznik pospolity", "tasznika pospolitego", "tasznikowi pospolitemu", "tasznik pospolity", "tasznikiem pospolitym", "taszniku pospolitym",
									     "Przygl�dasz si� tym ciekawym li�ciom zebranym w r�yczk�. Dooko�a nich widzisz bia�e, drobne kwiaty na szczycie ro�liny. Posiada tak�e owoce w postaci sercowatych �uszczynek, kt�re przypominaj� kszta�tem dawne torby pastusze, st�d nazwa ro�liny.",
									     65, PLANT_PART_LEAF, 7, 9,
								     {0, 0, 80, 0, 20,
									     5, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,
									     0, 0, 0, 45, 0,
									     0, 0, 0, 0, 0,
									     0, 0, 0, 85, 0,
									     100, 0, 50, 0, 0,
									     0, 0, 0, 70, 0,
									     0, 0, 70, 0, 0,0,0,0,0,0},
									     0, "unknown",
									     0, "unknown"
								     },
								     {"d�ugie mieczowate li�cie", "d�ugich mieczowatych li�ci", "d�ugim mieczowatym li�ciom", "d�ugie mieczowate li�cie", "d�ugimi mieczowatymi li��mi", "d�ugich mieczowatych li�ciach",
									     "tatarak zwyczajny", "tataraku zwyczajnego", "tatarakowi zwyczajnemu", "tatarak zwyczajny", "tatarakiem zwyczajnym", "tataraku zwyczajnym",
									     "Ro�lina ta wyst�puje pospolitcie w zbiorowiskacg b�otnych i wodnych. Posiada charakterystyczne, d�ugie, mieczowate li�cie. Jest silnie aromatyczna, wydziela specyficzny, mi�y zapach po roztarciu. Jest znana ze swoich w�a�ciwo�ci lekarskich.",
									     24, PLANT_PART_LEAF, 2, 9,
								     {0, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,
									     0, 0, 100, 0, 0,
									     0, 0, 0, 0, 50,
									     80, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,
									     100, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,0,0,0,0,0},
									     0, "unknown",
									     0, "unknown"
								     },
								     {"drobne ga��zki z r�owymi kwiatkami", "drobnych ga��zek z r�owymi kwiatkami", "drobnym ga��zkom z r�owymi kwiatkami", "drobne ga��zki z r�owymi kwiatkami", "drobnymi ga��zkami z r�owymi kwiatkami", "drobnych ga��zkach z r�owymi kwiatkami",
									     "tymianek pospolity", "tymianku pospolitego", "tymiankowi pospolitemu", "tymianek pospolity", "tymiankiem pospolitym", "tymianku pospolitym",
									     "Kiedy�, dawno dawno temu tymianek by� u�ywany do balsamowania zw�ok i dodawano go do kadzide�. Ponadto mia� dodawa� odwagi na polu walki. St�d jego nazwa, kt�ra oznacza odwag�, m�stwo. Do niedawna rycerze dostawali tymianek od swoich dam i przyk�adali go do ran. Poza tym kobiety wierz�, �e k�piel w naparze z ro�liny powi�ksza biust.",
												     89, PLANT_PART_FLOWER, 13, 9,
											     {0, 0, 50, 0, 20,
												     0, 0, 0, 0, 0,
												     0, 0, 0, 0, 0,
												     0, 0, 0, 0, 0,
												     0, 0, 0, 0, 0,
												     0, 0, 0, 0, 0,
												     100, 0, 0, 0, 0,
												     0, 0, 0, 0, 0,
												     0, 0, 40, 0, 0,0,0,0,0,0},
												     PLANT_EFFECT_SM_SF, "unknown",
												     0, "unknown"
											     },
											     {"poskr�cany bia�y korzonek", "poskr�canego bia�ego korzonka", "poskr�canemu bia�emu korzonkowi", "poskr�cany bia�y korzonek", "poskr�canym bia�ym korzonkiem", "poskr�canym bia�ym korzonku",
												     "wil�yna ciernista", "wil�yny ciernistej", "wil�ynie ciernistej", "wil�yn� ciernist�", "wil�yn� ciernist�", "wil�ycie ciernistej",
												     "Przygl�dasz si� tej niesamowitej ro�linie, a w�a�ciwie jej korzeniowi. Jest on d�ugi, z zewn�trz brunatny, na prze�omie ��tobia�y z wyra�nymi ciemniejszymi promieniami.",
												     93, PLANT_PART_ROOT, 4, 9,
											     {0, 0, 0, 0, 40,
												     0, 0, 0, 0, 0,
												     0, 0, 0, 0, 0,
												     0, 0, 0, 20, 0,
												     0, 0, 0, 0, 0,
												     0, 0, 0, 30, 0,
												     100, 0, 30, 0, 0,
												     0, 0, 0, 0, 0,
												     0, 0, 70, 0, 0,0,0,0,0,0},
												     0, "unknown",
												     0, "unknown"
											     },
											     {"ma�y krzaczek z drobnymi li��mi", "ma�ego krzaczka z drobnymi li��mi", "ma�emu krzaczkowi z drobnymi li��mi", "ma�y krzaczek z drobnymi li��mi", "ma�ym krzaczkiem z drobnymi li��mi", "ma�ym krzaczku z drobnymi li��mi",
												     "wrzos zwyczajny", "wrzosu zwyczajnego", "wrzosowi zwyczajnemu", "wrzos zwyczajny", "wrzosem zwyczajnym", "wrzosie zwyczajnym",
												     "Ten charakterystyczny, kolorowy krzaczek kwitnie na r�owo, a czasem na bia�o. Jego owocem jest charakterystyczna torebka. Wyst�puje w suchych, piaszczystych borach sosnowych i na terenach torfiastych. Ma w�a�ciwo�ci lecznicze mi�dzy innymi jest dobry na �o��dek.",
												     109, PLANT_PART_BUSH, 25, 9,
											     {0, 0, 10, 70, 50,
												     0, 0, 0, 0, 0,
												     0, 40, 0, 0, 0,
												     0, 0, 0, 0, 0,
												     0, 0, 0, 0, 0,
												     0, 0, 0, 0, 0,
												     100, 10, 90, 90, 0,
												     0, 0, 0, 0, 0,
												     0, 0, 80, 0, 0,0,0,0,0,0},
												     0, "unknown",
												     0, "unknown"
											     },
											     {"gruby br�zowy korze�", "grubego br�zowego korzenia", "grubemu br�zowemu korzeniowi", "gruby br�zowy korze�", "grubym br�zowym korzeniem", "grubym br�zowym korzeniu",
												     "�ywokost lekarski", "�ywokostu lekarskiego", "�ywokostowi lekarskiemu", "�ywokost lekarski", "�ywokostem lekarskim", "�ywoko�cie lekarskim",
												     "G��wnym i najwa�niejszym elementem tej ro�liny jest jej du�y, gruby, br�zowy korze�. Zas�yn�� on w zwiazku ze swoimi specyficznymi w�a�ciwo�ciami. Nawet nazwa ro�liny powsta�a st�d, i� dawniej u�ywano �ywokostu jako �rodka przy�pieszaj�cego zrastanie si� z�amanych ko�ci.",
												     335, PLANT_PART_ROOT, 22, 9,
											     {0, 0, 15, 0, 0,
												     0, 0, 0, 0, 0,
												     0, 0, 40, 0, 0,
												     0, 0, 0, 0, 10,
												     20, 0, 0, 0, 0,
												     0, 0, 0, 0, 10,
												     100, 0, 0, 0, 0,
												     0, 0, 0, 0, 0,
												     0, 0, 65, 0, 0,0,0,0,0,0},
												     PLANT_EFFECT_SF_SH, "unknown",
	0, "unknown"
	},
/*65*/                                 {"niedu�y okr�g�y kaktus","niedu�ego okr�g�ego kaktusa","niedu�emu okr�g�emu kaktusowi","niedu�ego okr�g�ego kaktusa","niedu�ym okr�g�ym kaktusem","niedu�ym okr�g�ym kaktusie",
                                          "mammilaria","mammilarii","mammilarii","mammilari�","mammilari�","mammilarii",
                                          "Jest to niedu�y okr�g�y kaktus pokryty wieloma drobniutkimi kolcami. Wydaj� si� one na do�� mi�kkie i niegro�ne, sprawiaj�c na pierwszy rzut oka wra�enie futerka. Ro�lina ta jest okr�g�a i niemal idealnie symetryczna.",
                                          670,PLANT_PART_CACTUS  ,90,9,
                                        {0,0,0,0,0,0,0,0,0,0,
                                        40,0,0,0,0,0,0,0,0,0,
                                         0,0,0,0,0,0,0,0,0,0,
                                         0,65,0,40,0,0,0,0,0,0,
                                         0,0,0,0,0,0,0,0,0},
                                         PLANT_EFFECT_SF_ST_MLH,"opis smaku",	0,"opis palenia"},

                                       {"kaktus z pi�knym ��tym kwiatem","kaktusa z pi�knym ��tym kwiatem","kaktusowi z pi�knym ��tym kwiatem","kaktusa z pi�knym ��tym kwiatem","kaktusem z pi�knym ��tym kwiatem","kaktusie z pi�knym ��tym kwiatem",
                                        "notokaktus","notokaktusa","notokaktusowi","notokaktusa","notokaktusem","notokaktusie",
                                         "W oczy rzuca si� pi�kny ��ty kwiat, kt�ry wyr�s� z tego niepozornego kaktusa.",
                                         	554,PLANT_PART_CACTUS  ,80,9,
                                         {0,0,0,0,0,0,0,0,0,0,
                                         35,0,0,0,0,0,0,0,0,0,
                                         	0,0,0,0,0,0,0,0,0,0,
                                         	0,55,0,50,0,0,0,0,0,0,
                                         	0,0,0,0,0,0,0,0,0},
                                         	PLANT_EFFECT_SF_ST_MLH,"opis smaku",	0,"opis palenia"},

                                        {"du�y w�ski kaktus","du�ego w�skiego kaktusa","du�emu w�skiemu kaktusowi","du�ego w�skiego kaktusa","du�ym w�skim kaktusem","du�ym w�skim kaktusie",
                                        	"aporokaktus","aporokaktusa","aporokaktusowi","aporokaktusa","aporokaktusem","aporokaktusie",
                                        	"Masz przed sob� niewielkiej wielko�ci ro�lin�, z kt�rej wyrasta niezliczona ilo�� kolc�w.",
                                        	380,PLANT_PART_CACTUS  ,80,9,
                                        	{0,0,0,0,0,0,0,0,0,0,
                                        	50,0,0,0,0,0,0,0,0,0,
                                        	0,0,0,0,0,0,0,0,0,0,
                                        	0,70,0,20,0,0,0,0,0,0,
                                        	0,0,0,0,0,0,0,0,0},
                                        	PLANT_EFFECT_SF_ST_MLH,"opis smaku",	0,"opis palenia"},

                                        {"du�y okr�g�y kaktus","du�ego okr�g�ego kaktusa","du�emu okr�g�emu kaktusowi","du�ego okr�g�ego kaktusa","du�ym okr�g�ym kaktusem","du�ym okr�g�ym kaktusie",
                                        	"echinokaktus","echinokaktusa","echinokaktusowi","echinokaktusa","echinokaktusem","echinokaktusie",
                                        	"Ro�lina ta jest kszta�tu kulistego i wsz�dzie porastaj� j� niewielkie kolce.",
                                        	472,PLANT_PART_CACTUS  ,90,9,
                                        	{0,0,0,0,0,0,0,0,0,0,
                                        	40,0,0,0,0,0,0,0,0,0,
                                        	0,0,0,0,0,0,0,0,0,0,
                                        	0,60,0,25,0,0,0,0,0,0,
                                        	0,0,0,0,0,0,0,0,0},
                                        	PLANT_EFFECT_MF_MT_MLH,"opis smaku",	0,"opis palenia"},

                                        {"niewielki p�aski kaktus","niewielkiego p�askiego kaktusa","niewielkiemu p�askiemu kaktusowi","niewielkiego p�askiego kaktusa","niewielkim p�askim kaktusem","niewielkim p�askim kaktusie",
                                        	"zygokaktus","zygokaktusa","zygokaktusowi","zygokaktusa","zygokaktusem","zygokaktusie",
                                        	"Jest to p�aska ro�lina, kt�rej kolce pn� si� idealnie w g�r�. Posiada owalny kszta�t i mi�y zapach.",
                                        	649,PLANT_PART_CACTUS  ,70,9,
                                        	{0,0,0,0,0,0,0,0,0,0,
                                        	25,0,0,0,0,0,0,0,0,0,
                                        	0,0,0,0,0,0,0,0,0,0,
                                        	0,50,0,55,0,0,0,0,0,0,
                                        	0,0,0,0,0,0,0,0,0},
                                        	PLANT_EFFECT_SF_ST_MLH,"opis smaku",	0,"opis palenia"},

                                         {"pi�kny du�y fioletowy kwiat","pi�knego du�ego fioletowego kwiata","pi�knemu du�emu fioletowemu kwiatowi","pi�kny du�y fioletowy kwiat","pi�knym du�ym fioletowym kwiatem","pi�knym du�ym fioletowym kwiecie",
                                         	"naparstnica purpurowa","naparstnicy purpurowej","naparstnicy purpurowej","naparstnic� purpurow�","naparstnic� purpurow�","naparstnicy purpurowej",
                                         	"Kwiat o olbrzymim, r�owym kielichu, kt�rego li�cie s� lekko postrz�pione.",
                                         	30,PLANT_PART_FLOWER  ,0,6,
                                         	{0,0,10,10,0,0,0,0,0,0,
                                         	0,15,0,0,0,0,0,0,0,0,
                                         	0,0,0,0,0,0,0,0,0,0,
                                         	0,0,0,0,0,0,0,0,40,0,
                                         	0,0,25,0,0,0,0,0,0},  PLANT_EFFECT_POISON_LEVEL10,"opis smaku",	0,"opis palenia"},

                                         {"dziwne czarne jagody","dziwnych czarnych jag�d","dziwnym czarnym jagodom","dziwne czarne jagody","dziwnymi czarnymi jagodami","dziwnych czarnych jagodach",
                                          "pokrzyk","pokrzyku","pokrzykowi","pokrzyk","pokrzykiem","pokrzyku",
                                          "Niewielki krzaczek jakiej� ro�linki, z kt�rego wyrastaj� czarne jagody. Te ma�e, czarne kuleczki wstydliwie kryj� si� pod g�adkimi li��mi.",
                                          128,PLANT_PART_BERRYS  ,10,0,
                                          {0,0,0,15,15,0,0,0,0,0,
                                           0,5,15,0,0,0,0,0,0,0,
                                           0,0,0,0,0,0,0,0,0,0,
                                           0,0,0,0,0,0,0,0,0,0,
                                           0,0,5,0,0,0,0,0,0},    PLANT_EFFECT_POISON_LEVEL20,"opis smaku",	0,"opis palenia"},

                                         {"du�e pierzaste li�cie","du�ych pierzastych li�ci","du�ym pierzastym li�ciom","du�e pierzaste li�cie","du�ymi pierzastymi li�ciami","du�ych pierzastych li�ciach",
                                         	"lulek czarny","lulka czarnego","lukowi czarnemu","lulka czarnego","lulkiem czarnym","lulku czarnym",
                                        	"Ro�lina o du�ych pierzastych li�ciach, kt�re wydzielaj� dra�ni�cy nos zapach. Ko�c�wki li�ci s� postrz�pione, zupe�nie jakby jakie� ma�e �yj�tka je ogryza�y. Ciekawe tylko, czy wysz�o im to na zdrowie.",
                                         	370,PLANT_PART_LEAF  ,50,1,
                                         	{0,0,0,0,0,0,0,0,0,0,
                                         	 0,0,0,0,0,0,10,0,0,0,
                                         	 0,0,0,0,0,0,0,0,0,0,
                                         	 0,10,0,0,0,0,0,0,0,0,
                                         	 10,0,0,0,0,0,0,0,0},	PLANT_EFFECT_POISON_LEVEL30,"opis smaku",	0,"opis palenia"},

                                          {"du�y bia�y p�aski kwiatek","du�ego bia�ego p�askiego kwiatka","du�emu bia�emu p�askiemu kwiatkowi","du�ego bia�ego p�askiego kwiatka","du�ym bia�ym p�askim kwiatkiem","du�ym bia�ym p�askim kwiatku",
                                           "ciemiernik bia�y","ciernika bia�ego","ciernikowi bia�emu","ciernika bia��go","ciernikiem bia�ym","cierniku bia�ym",
                                           "Ro�lina na kt�r� patrzysz posiada niewielkie listki, prostopadle wyrastaj�ce od �odygi. Na jej ko�cu znajduje si� olbrzymich rozmiar�w p�aski kwiat w bia�ym kolorze. Pi�knie pachnie.",
                                           189,PLANT_PART_FLOWER  ,20,4,
                                           {0,0,0,0,10,20,0,0,0,0,
                                            0,0,0,50,0,0,15,0,0,0,
                                            0,0,0,0,0,0,0,0,0,0,
                                            0,0,0,0,0,0,0,0,0,0,
                                            0,0,0,0,0,0,0,0,0},  PLANT_EFFECT_POISON_LEVEL30,"opis smaku",	0,"opis palenia"},

                                          {"licznie rozga��zione m�ode p�dy","licznie rozga��zionego m�odego p�du","licznie rozga��zionemu m�odemu p�dowi","licznie rozga��ziony m�odego p�d","licznie rozga��zionym m�odym p�dem","licznie rozga��zionym m�odym p�dzie",
                                           "glistnik jask�cze ziele","glistnika jask�czego ziela","glistnikowi jask�czego ziela","glistnika jask�czego ziela ","glistnik� jask�cze ziele","glistniku jask�cze ziele",
                                           "Ro�lina ta nie posiada nawet najmniejszego listka. Jedyne co widzisz to wij�ce si� wok� �odygi m�ode p�dy.",
                                           15,PLANT_PART_LEAF  ,0,6,
                                           {0,0,25,0,0,0,0,0,0,0,
                                            0,0,80,0,0,0,0,0,0,0,
                                            0,0,0,0,0,0,0,0,0,0,
                                            0,0,0,0,0,0,0,0,0,0,
                                            0,0,40,0,0,0,0,0,0},
                                            PLANT_EFFECT_POISON_LEVEL20,"opis smaku",	0,"opis palenia"},

                                          {"ma�y szarozielony omszony krzaczek","ma�ego szarozielonego omszonego krzaczka","ma�emu szarozielonemu omszonemu krzaczkowi","ma�ego szarozielonego omszonego krzaczek","ma�ym szarozielonym omszonym krzaczkiem","ma�ym szarozielonym omszonym krzaczku",
                                           "bylica pio�un","bylicy pio�una","bylicy pio�una","bylic� pio�una","bylic� pio�una","bylicy pio�una",
                                           "Ogl�dasz ten szarozielony krzaczek, kt�ry jest niewielkich rozmiar�w ? mie�ci si� na d�oni.",
                                           277,PLANT_PART_BUSH  ,50,7,
                                           {0,0,30,5,10,5,0,0,0,0,
                                            0,5,20,0,0,0,0,0,5,0,
                                            0,0,0,0,0,0,0,0,10,0,
                                            0,0,30,10,0,0,0,0,5,0,
                                            0,0,50,0,0,0,0,0,0},
                                            PLANT_EFFECT_POISON_LEVEL10,"opis smaku",	0,"opis palenia"},

                                          {"prze�liczny bia�y kwiat","prze�licznego bia�ego kwiata","prze�licznemu bia�emu kwiatowi","prze�licznego bia�ego kwiata","prze�licznym bia�ym kwiatem","prze�licznym bia�ym kwiecie",
                                           "narcyz bia�y","narcyza bia�ego","narcyzowi bia�emu","narcyza bia�ego","narcyzem bia�ym","narcyzie bia�ym",
                                           "Ro�lina o p�askich, l�ni�cych li�ciach w�r�d kt�rych wyrasta przepi�kny bia�y kwiat w formie kielicha.",
                                           10,PLANT_PART_FLOWER  ,0,8,
                                           {0,0,0,0,5,15,0,0,0,0,
                                            0,0,0,5,0,0,0,0,0,0,
                                            0,0,0,0,0,0,0,0,0,0,
                                            0,0,10,0,0,0,0,0,25,0,
                                            0,0,10,0,0,0,0,0,0},
                                            PLANT_EFFECT_POISON_LEVEL10,"opis smaku",	0,"opis palenia"},

                                          {"szary bluszcz z bia�ymi kwiatkami","szarego bluszczu z bia�ymi kwiatkami","szaremu bluszczowi z bia�ymi kwiatkami","szary bluszcz z bia�ymi kwiatkami","szarym bluszczem z bia�ymi kwiatkami","szarym bluszczu z bia�ymi kwiatkami",
                                           "przest�p dwupienny","przest�pu dwupiennego","przest�powi dwupiennemu","przest�pa dwupiennego","przyst�pem dwupiennym","przest�pie dwupiennym",
                                           "Ogl�dasz jak�� dziwn� ro�lin� o nieregularnych kszta�tach. Po�r�d m�odych p�d�w, wyrastaj� malutkie, bia�e kwiatuszki i kusz� swym wygl�dem w�drowc�w.",
                                           295,PLANT_PART_BUSH  ,40,5,
                                           {0,0,0,30,15,0,0,0,0,0,
                                            0,5,30,0,0,0,0,0,5,0,
                                            0,0,0,0,0,0,0,0,10,0,
                                            0,0,10,0,0,5,0,0,20,0,
                                            0,0,5,0,0,0,0,0,0},
                                            PLANT_EFFECT_POISON_LEVEL10,"opis smaku",	0,"opis palenia"},

                                              {"gar�� ma�ych bia�ych owoc�w","gar�ci ma�ych bia�ych owoc�w","gar�ci ma�ych bia�ych owoc�w","gar�� ma�ych bia�ych owoc�w","gar�ci� ma�ych bia�ych owoc�w","gar�ci ma�ych bia�ych owoc�w",
                                               "�nieguliczka bia�a","�nieguliczki bia�ej","�nieguliczce bia�ej","�nieguliczk� bia��","�nieguliczk� bia��","�nieguliczce bia�ej",
                                               "Ogl�dasz ma�e, bia�e owoce o owalnym kszta�cie. Nie wydzielaj� �adnego zapachu, jednak mimo to wygl�daj� na ca�kiem smaczne.",
                                               5,PLANT_PART_FRUITS  ,0,8,
                                               {0,0,0,10,40,0,0,0,0,0,
                                                0,5,60,0,0,0,0,0,0,0,
                                                0,0,0,0,0,0,0,0,0,0,
                                                0,0,0,0,0,0,0,0,80,0,
                                                0,0,0,0,0,0,0,0,0},
                                                PLANT_EFFECT_POISON_LEVEL1,"opis smaku",	0,"opis palenia"},

                                              {"dzwonkowate fioletowe kwiaty","dzwonkowatych fioletowych kwiat�w","dzwonkowatym fioletowym kwiatom","dzwonkowate fioletowe kwiat�w","dzwonkowatymi fioletowymi kwiatami","dzwonkowatych fioletowych kwiatach",
                                               "tojad mocny","tojada mocnego","tojadowi mocnemu","tojada mocnego","tojadem mocnym","tojadzie mocnym",
                                               "Przepi�kne kwiaty w fioletowym kolorze. Maj� one kszta�t dzwonu i zach�caj� swym s�odkim zapachem.",
                                               	12,PLANT_PART_FLOWER  ,0,2,
                                               	{0,0,0,0,40,50,0,0,0,0,
                                               	 0,0,0,25,0,0,0,0,0,0,
                                               	 0,0,0,0,0,0,0,0,0,0,
                                               	 0,0,5,0,0,10,0,0,10,0,
                                               	 0,0,5,0,0,0,0,0,0},
                                               	 PLANT_EFFECT_POISON_PARALIZING,"opis smaku",	0,"opis palenia"},

                                               {"intensywnie czerwone jagody","intensywnie czerwonych jag�d","intensywnie czerwonym jagodom","intensywnie czerwone jagody","intensywnie czerwonymi jagodami","intensywnie czerwonych jagodach",
                                               	"wawrzynek wilcze�yko","wawrzynka wilcze�yka","wawrzykowi wilcze�yko","wawrzynka wilcze�ykiem","wawrzykiem wilcze�ykiem","wawrzyku wilcze�yku",
                                               	"Niewielkie, krwistoczerwone jagody. S� okr�g�e niczym ma�e kuleczki i pachn� niezwykle smakowicie.",
                                               	57,PLANT_PART_BERRYS  ,10,0,
                                               	{0,0,0,20,30,15,0,0,0,0,
                                               	 0,10,0,0,0,0,0,0,0,0,
                                               	 0,0,0,0,0,0,0,0,0,0,
                                               	 0,0,0,0,0,0,0,0,0,0,
                                               	 0,0,0,0,0,0,0,0,0},	PLANT_EFFECT_POISON_BLIND,"opis smaku",	0,"opis palenia"},

                                                {"niedu�e br�zowe grzyby","niedu�ych br�zowych grzyb�w","niedu�ym br�zowym grzybom","niedu�e br�zowe grzyby","niedu�ymi br�zowymi grzybami","niedu�ych br�zowych grzybach",
                                                 "prawdziwki","prawdziwk�w","prawdziwkom","prawdziwki","prawdziwkami","prawdziwkach",
                                                 "Ogl�dasz niedu�e, br�zowe grzyby. Wydzielaj� lekko gorzki zapach a ca�y br�zowy kapelusz opiera si� na solidnej, bia�ej n�ce.",
                                                 86,PLANT_PART_FUNGUS  ,20,9,
                                                 {0,0,0,30,0,0,0,0,0,0,
                                                  0,40,0,0,0,0,0,0,0,0,
                                                  0,0,0,0,0,0,0,0,0,0,
                                                  0,0,0,0,0,0,0,0,0,0,
                                                  0,0,0,0,0,0,0,0,0},
                                                  PLANT_EFFECT_MEDIUM_FOOD,"opis smaku",	0,"opis palenia"},

                                                {"ma�e ��te grzybki","ma�ych ��tych grzybk�w","ma�ym ��tym grzybkom","ma�e ��te grzybki","ma�ymi ��tymi grzybkami","ma�ych ��tych grzybkach",
                                                 "kurki","kurek","kurkom","kurki","kurkami","kurkach",
                                                 "Malutkie grzybki, kt�re maj� �adny ��ty kolor. W ca�o�ci s� postrz�pione i powykrzywiane, jednak mimo to nadaj� si� chyba do jedzenia.",
                                                 126,PLANT_PART_FUNGUS  ,30,9,
                                                 {0,0,0,50,0,0,0,0,0,0,
                                                  0,65,0,0,0,0,0,0,0,0,
                                                  0,0,0,0,0,0,0,0,0,0,
                                                  0,0,0,0,0,0,0,0,0,0,
                                                  0,0,0,0,0,0,0,0,0},
                                                  PLANT_EFFECT_MEDIUM_FOOD,"opis smaku",	0,"opis palenia"},

                                                {"du�y grzyb z br�zowymi kropkami","du�ego grzyba z br�zowymi kropkami","du�emu grzybowi z br�zowymi kropkami","du�ego grzyba z br�zowymi kropkami","du�ym grzybem z br�zowymi kropkami","du�ym grzybie z br�zowymi kropkami",
                                                 "kania","kani","kaniom","kanie","kaniami","kaniach",
                                                 "Grzyb wysoki na kilkana�cie centymetr�w z kapeluszem pokrytym jakimi� br�zowymi kropkami. Opiera si� na cieniutkiej n�ce, kt�rej bia�y nalot zostawia �lady na twoich r�kach.",
                                                 15,PLANT_PART_FUNGUS  ,0,9,
                                                 {0,0,0,40,0,0,0,0,0,0,
                                                  0,30,0,0,0,0,0,0,0,0,
                                                  0,0,0,0,0,0,0,0,0,0,
                                                  0,0,0,0,0,0,0,0,0,0,
                                                  0,0,0,0,0,0,0,0,0},
                                                  PLANT_EFFECT_MEDIUM_FOOD,"opis smaku",	0,"opis palenia"},

                                                 {"�rednie rudoczerwone grzyby","�rednich rudoczerwonych grzyb�w","�rednim rudoczerwonym grzybom","�rednie rudoczerwone grzybki","�rednimi rudoczerwonymi grzybami","�rednich rudoczerwonych grzybach",
                                                  "rydze","rydzy","rydzom","rydze","rydzami","rydzach",
                                                  "Niewielkie grzyby o rudym, postrz�pionym kapeluszu w kszta�cie kielicha. Grzyby te brudz� twoje r�ce jak�� wydzielin� w kolorze czerwonym.",
                                                  38,PLANT_PART_FUNGUS  ,10,9,
                                                  {0,0,0,10,0,0,0,0,0,0,
                                                   0,40,0,0,0,0,0,0,0,0,
                                                   0,0,0,0,0,0,0,0,0,0,
                                                   0,0,0,0,0,0,0,0,0,0,
                                                   0,0,0,0,0,0,0,0,0},
                                                   PLANT_EFFECT_MEDIUM_FOOD,"opis smaku",	0,"opis palenia"},

                                                 {"du�y �adny lekko zielonkawy grzyb","du�ego �adnego lekko zielonkawego grzyba","du�emu �adnemu lekko zielonkawemu grzybowi","du�ego �adnego lekko zielonkawego grzyba","du�ym �adnym lekko zielonkawym grzybem","du�ym �adnym lekko zielonkawym grzybie",
                                                  "zielonka","zielonki","zielonkom","zielonke","zielonk�","zielonce",	"Du�ych rozmiar�w grzyb o bardzo �adnym, zielonkawym kolorze. Wygl�da na ca�kiem smaczny.",
                                                  18,PLANT_PART_FUNGUS  ,0,9,
                                                  {0,0,0,20,0,0,0,0,0,0,
                                                   0,70,0,0,0,0,0,0,0,0,
                                                   0,0,0,0,0,0,0,0,0,0,
                                                   0,0,0,0,0,0,0,0,0,0,
                                                   0,0,0,0,0,0,0,0,0},
                                                   PLANT_EFFECT_MEDIUM_FOOD,"opis smaku",	0,"opis palenia"},

                                                 {"ma�e brunatne grzybki","ma�ych brunatnych grzybk�w","ma�ym brunatnym grzybkom","ma�e brunatne grzybki","ma�ymi brunatnymi grzybkami","ma�ych brunatnych grzybkach",
                                                  "podgrzybki","podgrzybk�w","podgrzybkom","podgrzybki","podgrzybkami","podgrzybkach",
                                                  "Malutkie grzyby o brunatnym kolorze. Ich owalne kapelusze dumnie opieraj� si� na solidnej, grubej n�ce.",
                                                  77,PLANT_PART_FUNGUS  ,30,9,
                                                  {0,0,0,70,0,0,0,0,0,0,
                                                  	0,90,0,0,0,0,0,0,0,0,
                                                  	0,0,0,0,0,0,0,0,0,0,
                                                  	0,0,0,0,0,0,0,0,0,0,
                                                  	0,0,0,0,0,0,0,0,0},
                                                  	PLANT_EFFECT_MEDIUM_FOOD,"opis smaku",	0,"opis palenia"},

                                                 {"�rednie pomara�czowe grzyby","�rednich pomara�czowych grzyb�w","�rednim pomara�czowym grzybom","�rednie pomara�czowe grzyby","�rednimi pomara�czowymi grzybami","�rednich pomara�czowych grzybach",
                                                  "ko�larze","ko�larzy","ko�larzom","ko�larze","ko�larzami","ko�larzach",
                                                  "Zwyczajne, �rednich rozmiar�w grzyby w pomara�czowym kolorze.",
                                                  28,PLANT_PART_FUNGUS  ,10,9,
                                                  {0,0,0,60,0,0,0,0,0,0,
                                                  	0,40,0,0,0,0,0,0,0,0,
                                                  	0,0,0,0,0,0,0,0,0,0,
                                                  	0,0,0,0,0,0,0,0,0,0,
                                                  	0,0,0,0,0,0,0,0,0},
                                                  	PLANT_EFFECT_MEDIUM_FOOD,"opis smaku",	0,"opis palenia"},

                                                 {"spory grzyb o lepkim kapeluszu","sporego grzyba o lepkim kapeluszu","sporemu grzybowi o lepkim kapeluszu","sporego grzyba o lepkim kapeluszu","sporym grzybem o lepkim kapeluszu","sporym grzybie o lepkim kapeluszu",
                                                  "ma�lak","ma�laka","ma�lakowi","ma�laka","ma�lakiem","ma�laku",
                                                  "Ogl�dasz spory grzyb, kt�rego br�zowy kapelusz pokryty jest jak�� dziwn�, lepk� mazi�.",
                                                  8,PLANT_PART_FUNGUS  ,0,9,
                                                  {0,0,0,50,0,0,0,0,0,0,
                                                  	0,20,0,0,0,0,0,0,0,0,
                                                  	0,0,0,0,0,0,0,0,0,0,
                                                  	0,0,0,0,0,0,0,0,0,0,
                                                  	0,0,0,0,0,0,0,0,0},
                                                  	PLANT_EFFECT_MEDIUM_FOOD,"opis smaku",	0,"opis palenia"},

                                                 {"mn�stwo malutkich br�zowych grzybk�w","mn�stwa malutkich br�zowych grzybk�w","mn�stwu malutkich br�zowych grzybk�w","mn�stwo malutkich br�zowych grzybk�w","mn�stwem malutkich br�zowych grzybk�w","mn�stwie malutkich br�zowych grzybk�w",
                                                  "opie�ki","opie�ka","opie�kowi","opie�ka","opie�kiem","opie�ku",
                                                  "Mn�stwo malutkich grzybk�w w br�zowym kolorze, kt�rych kapelusze opieraj� si� na cieniute�kich n�zkach.",
                                                  133,PLANT_PART_FUNGUS  ,30,9,
                                                  {0,0,0,70,0,0,0,0,0,0,
                                                  	0,40,0,0,0,0,0,0,0,0,
                                                  	0,0,0,0,0,0,0,0,0,0,
                                                  	0,0,0,0,0,0,0,0,0,0,
                                                  	0,0,0,0,0,0,0,0,0},
                                                  	PLANT_EFFECT_MEDIUM_FOOD,"opis smaku",	0,"opis palenia"},

                                                  {"niedu�e bia�e okr�g�e grzyby","niedu�ych bia�ych okr�g�ych grzyb�w","niedu�ym bia�ym okr�g�ym grzybom","niedu�e bia�e okr�g�e grzyby","niedu�ymi bia�ymi okr�g�ymi grzybami","niedu�ych bia�ych okr�g�ych grzybach",
                                                  	"pieczarki jadalne","pieczarek jadalnych","pieczarkom jadalnym","pieczarki jadalne","pieczarkami jadalnymi","pieczarkach jadalnych",
                                                  	"S� to niedu�e, bia�e grzyby o okr�g�ych kszta�tach.",
                                                  	80,PLANT_PART_FUNGUS  ,20,9,
                                                  	{0,0,50,30,0,0,0,0,0,0,
                                                  	 0,20,0,0,0,0,0,0,0,0,
                                                  	 0,0,0,0,0,0,0,0,0,0,
                                                  	 0,0,0,0,0,0,0,0,0,0,
                                                  	 0,0,40,0,0,0,0,0,0},
                                                  	 PLANT_EFFECT_MEDIUM_FOOD,"opis smaku",	0,"opis palenia"},

                                                   {"du�y br�zowy grzyb z d�ugim s�upkiem","du�ego br�zowego grzyba z d�ugim s�upkiem","du�emu br�zowemu grzybowi z d�ugim s�upkiem","du�ego br�zowego grzyba z d�ugim s�upkiem","du�ym br�zowym grzybem z d�ugim s�upkiem","du�ym br�zowym grzybie z d�ugim s�upkiem",
                                                    "turek","turka","turkowi","turka","turkiem","turku",
                                                    "Sporych rozmiar�w grzyb o niewielkim, br�zowym kapeluszu i niespotykanie d�ugim s�upku.",
                                                    15,PLANT_PART_FUNGUS  ,0,9,
                                                    {0,0,0,50,0,0,0,0,0,0,
                                                     0,60,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0},
                                                     PLANT_EFFECT_MEDIUM_FOOD,"opis smaku",	0,"opis palenia"},

                                                   {"ma�y okr�g�y grzybek","ma�ego okr�g�ego grzybka","ma�emu okr�g�emu grzybkowi","ma�ego okr�g�ego grzybka","ma�ym okr�g�ym grzybkiem","ma�ym okr�g�ym grzybku",
                                                    "purchawka","purchawki","purchawce","purchawk�","purchawk�","purchawce",
                                                    "Malutki, okr�g�y grzybek w szarym kolorze. Jest on niezwykle nap�cznia�y, zupe�nie jakby w swoim wn�trzu skrywa� jak�� tajemnice.",
                                                    92,PLANT_PART_FUNGUS  ,20,9,
                                                    {0,0,0,80,0,0,0,0,0,0,
                                                     0,70,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0},
                                                     PLANT_EFFECT_MEDIUM_FOOD,"opis smaku",	0,"opis palenia"},

                                                   {"mn�stwo ma�ych blador�owych grzybk�w","mn�stwa ma�ych blador�owych grzybk�w","mn�stwu ma�ych blador�owych grzybk�w","mn�stwo ma�ych blador�owych grzybk�w","mn�stwem ma�ych blador�owych grzybk�w","mn�stwie ma�ych blador�owych grzybk�w",
                                                    "twardzioszki przydro�ne","twardzioszk�w przydro�nych","twardzoszkom przydro�nym","twardzoszki przydro�ne","twardzioszkami przydro�nymi","twardzioszkach przydro�nych",
                                                    "Ogromna ilo�� ma�ych, blador�owych grzybk�w, kt�re wydzielaj� s�odki zapach.",
                                                    136,PLANT_PART_FUNGUS  ,30,9,
                                                    {0,0,45,40,0,0,0,0,0,0,
                                                     0,30,0,0,0,0,0,0,60,0,
                                                     0,0,0,0,0,0,0,0,50,0,
                                                     0,0,0,0,0,15,0,0,0,0,
                                                     0,0,55,0,0,0,0,0,0},
                                                     PLANT_EFFECT_MEDIUM_FOOD,"opis smaku",	0,"opis palenia"},

                                                   {"spory charakterystyczny bia�y grzyb","sporego charakterystycznego bia�ego grzyba","sporemu charakterystycznemu bia�emu grzybowi","sporego charakterystycznego bia�ego grzyba","sporym charakterystycznym bia�ym grzybem","sporym charakterystycznym bia�ym grzybie",
                                                    "czerniak ko�pakowaty","czerniaka ko�pakowatego","czerniakowi ko�pakowatemu","czerniaka ko�pakowatego","czerniakiem ko�pakowatym","czerniaku ko�pakowatym",
                                                    "Jest to sporych rozmiar�w grzyb. Posiada on charakterystyczne ciemne naloty, kt�re idealnie kontrastuj� z jego bia�� barw�.",
                                                    10,PLANT_PART_FUNGUS  ,0,9,
                                                    {0,0,50,20,0,0,0,0,0,0,
                                                     0,30,70,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,60,0,0,0,0,0,0},
                                                     PLANT_EFFECT_MEDIUM_FOOD,"opis smaku",	0,"opis palenia"},

                                                   {"du�y br�zowy grzyb wkl�s�y w �rodku","du�ego br�zowego grzyba wkl�s�ego w �rodku","du�emu br�zowemu grzybowi wkl�s�emu w �rodku","du�ego br�zowego grzyba wkl�s�ego w �rodku","du�ym br�zowym grzybem wkl�s�ym w �rodku","du�ym br�zowym grzybie wkl�s�ym w �rodku",
                                                    "dwupier�cieniak cesarski","dwupier�cieniaka cesarskiego","dwupier�cieniakowi cesarskiemu","dwupier�cieniaka cesarskiego","dwupier�cieniakiem cesarskiem","dwupier�cieniaku cesarskim",
                                                    "Du�y grzyb, kt�ry wydziela lekko kwa�ny zapach. Jego kapelusz posiada charakterystyczny kszta�t kielicha, sp�aszczonego ku brzegom.",
                                                    32,PLANT_PART_FUNGUS  ,0,9,
                                                    {0,0,0,35,0,0,0,0,0,0,
                                                     0,35,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0},
                                                     PLANT_EFFECT_MEDIUM_FOOD,"opis smaku",	0,"opis palenia"},

                                                   {"niedu�y grzyb z czerwonymi blaszkami","niedu�ego grzyba z czerwonymi blaszkami","niedu�emu grzybowi z czerwonymi blaszkami","niedu�ego grzyba z czerwonymi blaszkami","niedu�ym grzybem z czerwonymi blaszkami","niedu�ym grzybie z czerwonymi blaszkami",
                                                    "borowik szata�ski","borowika szata�skiego","borowikowi szata�skiemu","borowika szata�skiego","borowikiem szata�skim","borowiku szata�skim",
                                                    "Niedu�y grzyb o kapeluszu w pi�knym, jednolicie br�zowym kolorze. Stoi dumnie na solidnej, grubej nodze i kusi swymi czerwonymi blaszkami.",
                                                    116,PLANT_PART_FUNGUS  ,30,3,
                                                    {0,0,0,10,0,0,0,0,0,0,
                                                     0,20,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0},
                                                     PLANT_EFFECT_POISON_DEATH,"opis smaku",	0,"opis palenia"},

                                                   {"du�y czerwony grzyb w bia�e kropki","du�ego czerwonego grzyba w bia�e kropki","du�emu czerwonemu grzybowi w bia�e kropki","du�ego czerwonego grzyba w bia�e kropki","du�ym czerwonym grzybem w bia�e kropki","du�ym czerwonym grzybie w bia�e kropki",
                                                    "muchomor czerwony","muchomora czerwonego","muchomorowi czerwonego","muchomora czerwonego","muchomorem czerwonym","muchomorze czerwonym",
                                                    "Masz przed sob� du�ego grzyba. Jego kapelusz jest w jednolitym, czerwonym kolorze w ca�o�ci pokryty �adnymi, bia�ymi plamkami. Dzielnie stoi na swej jasnej, pod�u�nej n�ce i opiera si� przeciwno�ci� lasu.",
                                                    145,PLANT_PART_FUNGUS  ,30,2,
                                                    {0,0,0,25,0,0,0,0,0,0,
                                                     0,55,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0},
                                                     PLANT_EFFECT_POISON_WEAKEN,"opis smaku",	0,"opis palenia"},

                                                    {"du�y br�zowy bezkszta�tny grzyb","du�ego br�zowego bezkszta�tnego grzyba","du�emu br�zowemu bezkszta�tnemu grzybowi","du�ego br�zowego bezkszta�tnego grzyba","du�ym br�zowym bezkszta�tnym grzybem","du�ym br�zowym bezkszta�tnym grzybie",
                                                     "krowiak podwini�ty","krowiaka podwini�tego","krowiakowi podwini�temu","krowiaka podwini�tego","krowiakiem podwini�tym","krowiaku podwini�tym",
                                                     "Sporych rozmiar�w br�zowy grzyb o bli�ej nieokre�lonych kszta�tach.",
                                                     36,PLANT_PART_FUNGUS  ,10,3,
                                                     {0,0,0,30,0,0,0,0,0,0,
                                                      0,60,0,0,0,0,0,0,0,0,
                                                      0,0,0,0,0,0,0,0,0,0,
                                                      0,0,0,0,0,0,0,0,0,0,
                                                      0,0,0,0,0,0,0,0,0},
                                                      PLANT_EFFECT_POISON_WEAKEN,"opis smaku",	0,"opis palenia"},

                                                     {"ma�y bia�y stercz�cy grzyb","ma�ego bia�ego stercz�cego grzyba","ma�emu bia�emu stercz�cemu grzybowi","ma�ego bia�ego stercz�cego grzyba","ma�ym bia�ym stercz�cym grzybem","ma�ym bia�ym stercz�cym grzybie",
                                                     	"muchomor jadowity","muchomora jadowitego","muchomorowi jadowitemu","muchomora jadowitego","muchomorem jadowitym","muchomorze jadowitym",
                                                     	"Malutki bia�y grzybek, wygl�daj�cy ca�kiem smakowicie.",
                                                     	29,PLANT_PART_FUNGUS  ,10,2,
                                                     	{0,0,0,40,0,0,0,0,0,0,
                                                     	 0,45,0,0,0,0,0,0,0,0,
                                                     	 0,0,0,0,0,0,0,0,0,0,
                                                     	 0,0,0,0,0,0,0,0,0,0,
                                                     	 0,0,0,0,0,0,0,0,0},
                                                     	 PLANT_EFFECT_POISON_SLEEP,"opis smaku",	0,"opis palenia"},

                                                      {"spory br�zowy grzyb","sporego br�zowego grzyba","sporemu br�zowemu grzybowi","sporego br�zowego grzyba","sporym br�zowym grzybem","sporym br�zowym grzybie",
                                                       "muchomor sromotnikowy","muchomora sromotnikowego","muchomorowi sromotnitkowemu","muchomora sromotnikowego","muchomorem sromotkinowym","muchomorze sromotnikowym",
                                                       "�adnych kszta�t�w grzybek o szarym kapeluszu. Jego trzon jest niespotykanie gruby.",
                                                       36,PLANT_PART_FUNGUS  ,10,1,
                                                       {0,0,0,55,0,0,0,0,0,0,
                                                        0,45,0,0,0,0,0,0,0,0,
                                                        0,0,0,0,0,0,0,0,0,0,
                                                        0,0,0,0,0,0,0,0,0,0,
                                                        0,0,0,0,0,0,0,0,0},
                                                        PLANT_EFFECT_POISON_DEATH,"opis smaku",	0,"opis palenia"},

                                                      {"malutkie zielone p�atki","malutkich zielonych p�atk�w","malutkim zielonym p�atkom","malutkie zielone p�atki","malutkimi zielonymi p�atkami","malutkich zielonych p�atkach",
                                                       "zygotnik le�ny","zygotnika le�nego","zygotnikowi le�nemu","zygotnika le�nego","zygotnikiem le�nym","zygotniku le�nym",
                                                       "Malutkie p�atki jakiej� ro�linki w bardzo �adnym, jasnozielonym kolorze. S� niezwykle delikatne i pokryte delikatnym meszkiem.",
                                                       288,PLANT_PART_LEAF  ,50,9,
                                                       {0,0,0,30,0,0,0,0,0,0,
                                                        0,60,0,0,0,0,0,0,0,0,
                                                        0,0,0,0,0,0,0,0,0,0,
                                                        0,0,0,0,0,0,0,0,0,0,
                                                        0,0,0,0,0,0,0,0,0},
                                                        PLANT_EFFECT_NONE,"opis smaku",	0,"opis palenia"},

                                                      {"male�kie zielone p�atki","male�kich zielonych p�atk�w","male�kim zielonym p�atkom","male�kie zielone p�atki","male�kimi zielonymi p�atkami","male�kich zielonych p�atkach",
                                                       "zygotnik g�rski","zygotnika g�rskiego","zygotnikowi g�rskiemu","zygotnkika g�rskiego","zygotnika g�rskim","zygotniku g�rskim",
                                                       "Drobniute�skie p�atki jakiej� ro�linki w ca�o�ci pokryte ciemnozielonym meszkiem.",
                                                       602,PLANT_PART_LEAF  ,70,9,
                                                       {0,0,0,0,40,60,0,0,0,0,
                                                        0,0,0,50,0,0,0,0,0,0,
                                                        0,0,0,0,0,0,0,0,0,0,
                                                        0,0,0,0,0,30,0,0,0,0,
                                                        0,0,0,0,0,0,0,0,0},
                                                        PLANT_EFFECT_NONE,"opis smaku",	0,"opis palenia"},

                                                      {"du�y zgni�ozielony li��","du�ego zgni�ozielonego li�cia","du�emu zgni�ozielonemu li�ciowi","du�ego zgni�ozielonego li�cia","du�ym zgni�ozielonym li�ciem","du�ym zgni�ozielonym li�ci",
                                                       "bagienne ziele","bagiennego ziela","bagiennemu zielowi","bagienne ziele","bagiennym zielem","bagiennym zielu",
                                                       "Du�y li�� w kolorze zgni�ozielonym. Wydziela niespotykanie kwa�ny zapach.",
                                                       89,PLANT_PART_LEAF  ,0,9,
                                                       {0,0,0,0,0,0,0,0,0,0,
                                                        0,0,80,0,0,0,0,0,0,0,
                                                        0,0,0,0,0,0,0,0,0,0,
                                                        0,0,0,0,0,0,0,0,0,0,
                                                        0,0,0,0,0,0,0,0,0},
                                                        PLANT_EFFECT_NONE,"opis smaku",	0,"opis palenia"},

                                                       {"strza�kowate szare li�cie","strza�kowatych szarych li�ci","strza�kowatym szarym li�ciom","strza�kowate szare li�cie","strza�kowatymi szarymi li�ciami","strza�kowatych szarych li�ciach",
                                                        "szary poraz","szarego poraza","szaremu porazowi","szarego poraza","szarym porazem","szarym porazie",
                                                        "Niewielkie, strza�kowate li�cie. Maj� szary kolor i s� niespotykanie g�adkie.",
                                                        66,PLANT_PART_LEAF  ,20,9,
                                                        {0,0,0,0,0,0,0,0,0,0,
                                                        	0,0,20,0,0,40,0,0,0,0,
                                                        	0,0,0,0,0,0,0,0,0,0,
                                                        	0,0,0,0,0,0,0,0,0,0,
                                                        	0,0,30,0,0,0,0,0,0},
                                                        	PLANT_EFFECT_PP_BLIND,"opis smaku",	0,"opis palenia"},

                                                        {"czarno-��te cuchn�ce li�cie","czarno-��tych cuchn�cych li�ci","czarno-��tym cuchn�cym li�ciom","czarno-��te cuchn�ce li�cie","czarno-��tymi cuchn�cymi li�ciami","czarno-��tych cuchn�cych li�ciach",
                                                        	"czarny �niwiarz","czarnego �niwiarza","czarnemu �niwiarzowi","czarnego �niwiarza","czarnym �niwiarzem","czarnym �niwiarzu",
                                                        	"Dziwnego kszta�tu li�cie o czarno-z�tym zabarwieniu. Wydzielaj� charakterystyczny, cuchn�cy od�r i ca�e klej� si� od jakiej� lepkiej cieczy.",
                                                        	488,PLANT_PART_LEAF  ,50,0,
                                                        	{0,0,0,0,0,0,0,0,0,0,
                                                        	 0,10,0,0,0,0,0,0,0,0,
                                                        	 0,0,0,0,0,0,0,0,0,0,
                                                        	 0,0,0,0,0,0,0,0,0,0,
                                                        	 0,0,0,0,0,0,0,0,0},
                                                        	 PLANT_EFFECT_MINUS_BIG_HP,"opis smaku",	0,"opis palenia"},

                                                        {"czerwono-ziemisty mech","czerwono-ziemistego mchu","czerwono-ziemistemu mchowi","czerwono-ziemisty mech","czerwono-ziemistym mchem","czerwono-ziemistym mchu",
                                                        	"krwawy mech","krwawego mchu","krwawemu mchowi","krwawy mech","krwawym mchem","krwawym mchu",
                                                        	"Malutka kupka mchu w czerwono-ziemistym kolorze. Pachnie zupe�nie jak mu�!",
                                                        	86,PLANT_PART_BUSH  ,20,9,
                                                        	{0,0,0,20,0,0,0,0,0,0,
                                                        	 0,5,45,0,0,0,0,0,0,0,
                                                        	 0,0,0,0,0,0,0,0,0,0,
                                                        	 0,0,0,0,0,0,0,0,0,0,
                                                        	 0,0,25,0,0,0,0,0,0},
                                                        	 PLANT_EFFECT_NONE,"opis smaku",	0,"opis palenia"},

                                                         {"l�ni�cy z�otem kwiat","l�ni�cego z�otem kwiata","l�ni�cemu z�otem kwiatowi","l�ni�cego z�otem kwiat","l�ni�cym z�otem kwiatem","l�ni�cym z�otem kwiecie",
                                                          "szczyglik z�ocisty","szczyglika z�ocistego","szczyglikowi z�ocistemu","szczyglika z�oscistego","szczyglkiem z�ocistym","szczygliku z�ocistym",
                                                          "Przepi�kny dzwoniasty kwiat, kt�ry w ca�o�ci mieni si� kolorem l�ni�cego z�ota.",
                                                          31,PLANT_PART_FLOWER  ,10,9,
                                                          {0,0,0,0,0,0,0,0,0,0,
                                                           0,0,0,0,0,0,0,0,0,0,
                                                           0,0,0,0,0,0,0,0,0,0,
                                                           0,0,25,0,0,0,0,0,0,0,
                                                           0,0,35,0,0,0,0,0,0},
                                                           PLANT_EFFECT_ENDURE_POISON,"opis smaku",	0,"opis palenia"},

                                                         {"kolczasty zgnilozielony wodorost","kolczastego zgni�ozielonego wodorostu","kolczastemu zgni�ozielonemu wodorostowi","kolczastego zgni�ozielonego wodorosta","kolczastym zgni�ozielonym wodorostem","kolczastym zgni�ozielonym wodoro�cie",
                                                          "morski kie�","morskiego k�a","morskiemu k�owi","morskiego k�a","morskim k�em","morskim kle",
                                                          "Bezkszta�tna ro�linka z kt�rej wyrastaj� ma�e kolce o zgni�ozielonym kolorze.",
                                                          500,PLANT_PART_LEAF  ,60,9,
                                                          {0,0,0,0,0,0,0,0,0,0,
                                                           0,0,0,0,0,0,0,0,0,0,
                                                           0,20,20,0,0,0,0,0,0,0,
                                                           0,0,0,0,0,0,0,0,0,0,
                                                           10,0,0,0,0,0,0,0,0},
                                                           PLANT_EFFECT_POISON_LEVEL20,"opis smaku",	0,"opis palenia"},

                                                         {"ma�y k��bek bia�ych korzonk�w","ma�ego k��bka bia�ych korzonk�w","ma�emu k��bkowi bia�ych korzonk�w","ma�y k��bek bia�ych korzonk�w","ma�ym k��bkiem bia�ych korzonk�","ma�ym k��bku bia�ych korzonk�w",
                                                          "wiatrorost","wiatrorosta","wiatrorostowi","wiatrorosta","wiatrorostem","wiatroro�cie",
                                                          "Niewielka ilo�� poskr�canych i pozwijanych bia�ych korzonk�w.",
                                                          39,PLANT_PART_ROOT  ,0,9,
                                                          {0,0,0,0,0,0,0,0,0,50,
                                                          15,0,0,0,0,0,0,0,0,0,
                                                           0,0,0,0,0,0,10,15,0,5,
                                                           0,0,0,0,0,0,0,0,0,0,
                                                           0,10,0,0,0,0,0,0,0},
                                                           PLANT_EFFECT_NONE,"opis smaku",	0,"opis palenia"},

                                                          {"ma�a twarda ciernista ga��zka","ma�ej twardej ciernistej ga��zki","ma�ej twardej ciernistej ga��zce","ma�� tward� ciernist� ga��zk�","ma�� tward� ciernist� ga��zk�","ma�ej twardej ciernistej ga��zce",
                                                           "wilczy kie�","wilczego k�a","wilczemu k�owi","wilczego k�a","wilczym k�em","wilczym kle",
                                                           "Obro�ni�ta malutkimi cierniami, niezwykle twarda ga��zka.",
                                                           595,PLANT_PART_NONE ,70,9,
                                                           {0,0,0,0,0,0,0,0,0,0,
                                                            0,0,70,0,0,0,0,0,0,0,
                                                            0,0,0,0,0,0,0,0,0,0,
                                                            0,0,0,0,0,0,0,0,0,0,
                                                            0,0,0,0,0,0,0,0,0},
                                                            PLANT_EFFECT_MINUS_LITTLE_HP,"opis smaku",	0,"opis palenia"},

                                                         {"kolbowaty czarny grzyb","kolbowatego czarnego grzyba","kolbowatemu czarnemu grzybowi","kolbowatego czarnego grzyba","kolbowatym czarnym grzybem","kolbowatym czarnym grzybie",
                                                          "tymora czarna","tymory czarnej","tymorze czarnej","tymor� czarn�","tymor� czarn�","tymorze czarnej",
                                                          "W ca�o�ci pokryty czarnym kolorem grzyb o kolbowatym kapeluszu i grubej n�ce.",
                                                          101,PLANT_PART_FUNGUS  ,20,7,
                                                          {0,0,0,0,0,0,0,0,0,0,
                                                           0,0,0,0,0,0,0,0,0,0,
                                                           0,0,0,0,0,0,0,0,0,0,
                                                           0,0,45,20,0,0,0,0,0,0,
                                                           0,0,0,0,0,0,0,0,0},
                                                           PLANT_EFFECT_NONE,"opis smaku",	0,"opis palenia"},

                                                          {"ma�e ��te parz�ce listki","ma�ych ��tych parz�cych listk�w","ma�ym ��tym parz�cym listkom","ma�e ��te parz�ce listki","ma�ymi parz�cymi listkami","ma�ych ��tych parz�cych listkach",
                                                           "�a�obka popielna","�a�obki popielatej","�a�obce popielatej","�a�obk� popielat�","�a�obk� popielat�","�a�obce popielatej",
                                                           "Ma�e, postrz�pione przy ko�cach listki w ��tym kolorze.",
                                                           373,PLANT_PART_LEAF  ,60,6,
                                                           {0,0,0,0,0,0,0,0,0,0,
                                                           45,0,0,0,0,0,0,0,0,0,
                                                            0,0,0,0,0,0,0,0,0,20,
                                                            0,0,0,0,0,0,0,0,0,0,
                                                           25,0,0,0,0,0,0,0,0},
                                                           PLANT_EFFECT_NONE,"opis smaku",	0,"opis palenia"},

                                                          {"powykr�cane czarne ga��zki","powykr�canych czarnych ga��zek","powykr�canym czarnym ga��zkom","powykr�cane czarne ga��zki","powykr�canymi czarnymi ga��zkami","powykr�canych czarnych ga��zkach",
                                                           "czarnokrzew","czarnokrzewu","czarnekrzewowi","czarnokrzew","czarnokrzewem","czarnokrzewie",
                                                           "Pozwijane i poskr�cane w r�nych kierunkach czarne ga��zki.",
                                                           92,PLANT_PART_NONE ,20,9,
                                                           {0,0,0,0,0,0,0,0,0,0,
                                                            0,0,0,0,0,0,65,0,0,0,
                                                            0,0,0,0,0,0,25,25,0,0,
                                                            0,0,0,0,0,0,0,0,0,0,
                                                            0,0,0,0,0,0,0,0,0},
                                                            PLANT_EFFECT_SHOCKING_GRASP,"opis smaku",	0,"opis palenia"},

                                                          {"pi�kny wysoki z�oto-srebrny kwiat","pi�knego wysokiego z�oto-srebrnego kwiata","pi�knemu wysokiemu z�oto-srebrnemu kwiatowi","pi�knego wysokiego z�oto-srebrnego kwiata","pi�knym wysokim z�oto-srebrnym kwiatem","pi�knym wysokim z�oto-srebrnym kwiacie",
                                                           "boski duszczyk","boskiego duszczyka","boskiemu duszczykowi","boskiego duszczyka","boskim duszczykiem","boskim duszczyku",
                                                           "Niezwykle du�ych rozmiar�w kwiat, kt�ry wydziela przyjemny, s�odki zapach. ��te p�atki idealnie kontrastuj� ze srebrnym wn�trzem kwiatu.",
                                                           86,PLANT_PART_FLOWER  ,20,9,
                                                           {0,0,0,10,0,0,0,0,0,0,
                                                            0,0,0,0,0,0,0,0,0,0,
                                                            0,0,0,0,0,0,0,0,0,0,
                                                            0,0,0,0,0,0,0,0,0,0,
                                                            0,0,50,0,0,0,0,0,0},
                                                            PLANT_EFFECT_SLOW_POISON,"opis smaku",	0,"opis palenia"},

                                                           {"ciemnozielone pod��ne zio�o","ciemnozielonego pod��nego zio�a","ciemnozielonemu pod��nemu zio�u","ciemnozielone pod��ne zio�o","ciemnozielonym pod��nym zio�em ","ciemnozielone pod��ne ziele",
                                                           	"eathe arthis","eathe artis","eathe artis","eathe artis","eathe artis","eathe artis",
                                                           	"Zwyczajna prosta ro�linka w zielonym kolorze. Nie wydziela prawie �adnego zapachu i zapewne smakuje r�wnie nijako.",
                                                           	750,PLANT_PART_PERENNIAL  ,40,9,
                                                           	{0,0,0,20,0,0,0,0,0,0,
                                                           	 0,25,0,0,0,0,0,0,0,0,
                                                           	 0,0,0,0,0,0,0,0,0,0,
                                                           	 0,0,0,0,0,0,0,0,0,0,
                                                           	 0,0,10,0,0,0,0,0,0},
                                                           	 PLANT_EFFECT_HIGH_HP,"opis smaku",	0,"opis palenia"},

                                                            {"ma�y z�oty grzybek","ma�ego z�otego grzybka","ma�emu z�otemu grzybkowi","ma�ego z�otego grzybka","ma�ym z�otym grzybkiem","ma�ym z�otym grzybku",
                                                             "korronnik szlachetny","korronnika szlachetnego","korronnikowi szlachetnemu","korronnika szlachetnego","korronnikiem szlachetnym","korronniku szlachetnym",
                                                             "Malutki grzybek, kt�rego kapelusz w z�otym kolorze prawie w ca�o�ci przys�ania cieniutk� n�k�.",
                                                             160,PLANT_PART_FUNGUS  ,30,9,
                                                             {0,0,0,0,0,0,0,0,0,0,
                                                              0,10,0,0,0,0,0,0,0,0,
                                                              0,0,0,0,0,0,0,0,0,0,
                                                              0,0,0,0,0,0,0,0,0,0,
                                                              0,0,0,0,0,0,0,0,0},
                                                              PLANT_EFFECT_SMALL_FOOD,"opis smaku",	0,"opis palenia"},

                                                             {"du�y bia�y grzyb","du�ego bia�ego grzyba","du�emu bia�emu grzybowi","du�ego bia�ego grzyba","du�ym bia�ym grzybem","du�ym bia�ym grzybie",
                                                              "gromnik","gromnika","gromnikowi","gromnika","gromnikiem","gromniku",
                                                              "Sporych rozmiar�w grzyb w bia�ym kolorze, gdzieniegdzie pokryty szarymi plamkami.",
                                                              27,PLANT_PART_FUNGUS  ,10,5,
                                                              {0,0,0,0,0,0,0,0,0,0,
                                                               0,0,0,0,20,5,0,40,0,0,
                                                               0,0,0,0,0,0,0,0,0,0,
                                                               0,0,0,0,0,0,0,0,0,0,
                                                               0,0,0,0,0,0,0,0,0},
                                                               PLANT_EFFECT_SMALL_FOOD,"opis smaku",	0,"opis palenia"},

                                                            {"zielony fosforyzuj�cy grzyb","zielonego fosforyzuj�cego grzyba","zielonemu fosforyzuj�cemu grzbowi","zielonego fosforyzuj�cego grzyba","zielonym fosforyzuj�cym grzybem","zielonym fosforyzuj�cym grzybie",
                                                             "szalarek zielony","szalarka zielonego","szalarkowi zielonemu","szalarka zielonego","szalarkiem zielonym","szalarku zielonym",
                                                             "Sporych rozmiar�w zielony grzyb, w ca�o�ci pokryty jak�� dziwn�, jasnozielon� wydzielin� o dziwnym zapachu i konstystencji.",
                                                             74,PLANT_PART_FUNGUS  ,20,9,
                                                             {0,0,0,0,0,0,0,0,0,0,
                                                              0,0,0,0,30,10,0,70,0,0,
                                                              0,0,0,0,0,0,0,0,0,0,
                                                              0,0,0,0,0,0,0,0,0,0,
                                                              0,0,0,0,0,0,0,0,0},
                                                              PLANT_EFFECT_SMALL_FOOD,"opis smaku",	0,"opis palenia"},

                                                         {"fioletowy fosforyzuj�cy grzyb","fioletowego fosforyzuj�cego grzyba","fioletowemu fosforyzuj�cemu grzbowi","fioletowego fosforyzuj�cego grzyba","fioletowym fosforyzuj�cym grzybem","fioletowym fosforyzuj�cym grzybie",
                                                          "szalarek fioletowy","szalarka fioletowego","szakarkowi fioletowemu","szalarka fioletowego","szalarkiem fioletowym","szalarku fioletowym",
                                                          "Ma�y grzybek, kt�ry w ca�o�ci pokryty jest jak�� dziwn�, lepk� fioletow� ciecz�.",
                                                          125,PLANT_PART_FUNGUS  ,20,9,
                                                          {0,0,0,0,0,0,0,0,0,0,
                                                           0,0,0,0,5,0,0,20,0,0,
                                                           0,0,0,0,0,0,0,0,0,0,
                                                           0,0,0,0,0,0,0,0,0,0,
                                                           0,0,0,0,0,0,0,0,0},
                                                           PLANT_EFFECT_SMALL_FOOD,"opis smaku",	0,"opis palenia"},

                                                         {"lekko fosforyzuj�ca ��ta ple��","lekko fosforyzuj��ej ��tej ple�ni","lekko fosforyzuj�cej ��tej ple�ni","lekko fosforyzuj�c� ��t� ple��","lekko fosforyzuj�c� ��t� ple�ni�","lekko fosforyzuj�cej ��tej ple�ni",
                                                         "z�otawka","z�otawki","z�otawce","z�otawk�","z�otawk�","z�otawce",
                                                         "Ple�� w bardzo �adnym, ��tym kolorze.",
                                                         323,PLANT_PART_NONE ,60,9,
                                                         {0,0,0,0,0,0,0,0,0,0,
                                                          0,5,0,0,0,0,0,25,0,0,
                                                          0,0,0,0,0,0,0,0,0,0,
                                                          0,0,0,0,0,0,0,0,0,0,
                                                          0,0,0,0,0,0,0,0,0},
                                                          PLANT_EFFECT_SMALL_FOOD,"opis smaku",	0,"opis palenia"},

{"zwyk�a szara ple��","zwyk�ej szarej ple�ni","zwyk�ej szarej ple�ni","zwykl� szar� ple��","zwyk�� szar� ple�ni�","zwyk�ej szarej ple�ni",	"ple��","ple�ni","ple�ni","ple��","ple�ni�","ple�ni",	"Ple�� w brzydkim, szarym kolorze. Potwornie �mierdzi i jest dziwnie klej�ca.",	172,PLANT_PART_NONE ,50,8,	{30,30,0,20,0,0,10,10,0,0,	0,40,100,0,20,80,0,100,0,50,	70,20,10,0,0,0,0,0,0,0,	0,0,0,0,50,0,10,100,0,100,	0,10,0,0,0,0,0,0,0},	PLANT_EFFECT_MINUS_SMALL_THIRST,"opis smaku",	0,"opis palenia"},
{"ma�y skamienia�y grzybek","ma�ego skamienia�ego grzybka","ma�emu skamienia�emu grzybkowi","ma�ego smienia�ego grzybka","ma�ym skamienia�ym grzybkiem","ma�ym skamienia�ym grzybku",	"skamienia�y gambis","skamienia�ego gambisa","skamienia�emu gambisowi","skamienia�ego gambisa","skamienia�ym gambisem","skamienia�ym gambisie",	"Ma�y szary grzybek wygl�da jakby by� z kamienia.",	614,PLANT_PART_FUNGUS  ,70,9,	{0,0,0,0,0,0,0,0,0,0,	0,0,0,30,10,40,0,20,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0},	PLANT_EFFECT_MINUS_DEX_P10,"opis smaku",	0,"opis palenia"},
{"du�y ognistoczerwony grzyb","du�ego ognistoczerwonego grzyba","du�emu ognistoczerwonemu grzybowi","du�ego ognistoczerwonego grzyba","du�ym ognistoczerwonym grzybem","du�ym ognistoczerwonym grzybie",	"ognisty grzyb","ognistego grzyba","ognistemu grzybowi","ognistego grzyba","ognistym grzybem","ognistym grzybie",	"Du�ych rozmiar�w grzyb w ognistoczerwonym kolorze. S�upek, na kt�rym opiera si� kapelusz jest niespotykanie gruby.",	62,PLANT_PART_FUNGUS  ,30,9,	{0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,80,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0},	PLANT_EFFECT_SMALL_FOOD,"opis smaku",	0,"opis palenia"},
{"intensywnie b��kitny porost","intenstywnie b��kitnego porostu","intenstywnie b��kitnemu porostowi","intensywnie b��kitny porost","intensywnie b��kitnym porostem","intensywnie b��kitnym poro�cie",	"porost lodowcowy","porosta lodowcowego","porostowi lodowcowemu","porost lodowcowy","porostem lodowcowym","poro�cie lodowcowym",	"Dziwnych kszta�t�w b��kitny porost.",	332,PLANT_PART_BUSH  ,40,9,	{0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,20,0,0,0,	0,0,0,0,0,0,75,45,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0},	PLANT_EFFECT_SMALL_FOOD,"opis smaku",	0,"opis palenia"},
{"ma�y kolczasty grzybek","ma�ego kolczastego grzybka","ma�emu kolczastemu grzybkowi","ma�ego kolczastego grzybka","ma�ym kolczastym grzybkiem","ma�ym kolczastym grzybku",	"�dzierkoszcz�yk","�dzierkoszcz�yka","�dzierkoszcz�ykowi","�dzierkoszcz�yka","�dzierkoszcz�ykiem","�dzierkoszcz�yku",	"Malutki grzybek w pomara�czowym kolorze. Z jego kapelusza wyrastaj� niewielkie igie�ki gotowe wbi� si� we wszystko, co tylko si� do nich zbli�y.",	481,PLANT_PART_FUNGUS  ,60,9,	{0,0,0,0,0,45,0,0,0,0,	0,0,0,40,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,	0,45,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0},	PLANT_EFFECT_HALLUCINATIONS,"opis smaku",	0,"opis palenia"},
{"du�y czarny grzyb z brodawkami","du�ego czarnego grzyba z brodawkami","du�emu czarnemu grzybowi z brodawkami","du�ego czarnego grzyba z brodawkami","du�ym czarnym grzybem z brodawkami","du�ym czarnym grzybie z brodawkami",	"karlin zab�jczy","karlina zab�jczego","karlinowi zab�jczemu","karlina zab�jczego","karlinem zab�jczym","karlinie zab�jczym",	"Du�y grzyb w czarnym kolorze, w ca�o�ci pokryty jest dziwnymi brodawkami.",	116,PLANT_PART_FUNGUS  ,30,2,	{0,0,0,0,0,0,0,0,0,0,	0,0,25,0,15,0,0,5,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0},	PLANT_EFFECT_POISON_PARALIZING,"opis smaku",	0,"opis palenia"},
{"niewielki bezkszta�ty grzyb","niewielkiego bezkszta�tnego grzyba","niewielkiemu bezkszta�tnemu grzybowi","niewielkiego bezkszta�tnego grzyba","niewielkiem bezkszta�tnym grzybem","niewielkim bezkszta�tnym grzybie",	"boliar b�otny","boliara b�otnego","boliarowi b�otnemu","boliara b�otnego","boliarem b�otnym","boliarze b�otnym",	"Niewielki grzyb, kt�rego kapelusz posiada bli�ej nieokre�lone kszta�ty.",	84,PLANT_PART_FUNGUS  ,10,9,	{0,0,0,0,0,0,0,0,0,0,	0,0,75,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0},	PLANT_EFFECT_SMALL_FOOD,"opis smaku",	0,"opis palenia"},
{"male�kie br�zowe grzybki","male�kich br�zowych grzybk�w","male�kim br�zowym grzybkom","male�kie br�zowe grzybki","male�kimi br�zowymi grzybkami","male�kich br�zowych grzybkach",	"pod�ciernik","pod�ciernika","pod�ciernikowi","pod�ciernika","pod�ciernikiem","pod�cierniku",	"Malutkie grzybki w br�zowym kolorze o niezbyt przyjemnym zapachu.",	168,PLANT_PART_FUNGUS  ,30,9,	{0,0,10,25,40,20,0,0,0,0,	0,55,5,10,0,0,0,0,20,0,	0,0,0,0,0,0,0,0,20,0,	0,5,35,10,0,30,0,0,10,0,	0,0,5,0,0,0,0,0,0},	PLANT_EFFECT_SMALL_FOOD,"opis smaku",	0,"opis palenia"},
{"szara krzaczasta ple��","szarej krzaczastej ple�ni","szarej krzaczastej ple�ni","szar� krzaczast� ple��","szar� krzaczast� ple�ni�","szarej krzaczastej ple�ni",	"eppar nahor","eppar nahor","eppar nahor","eppar nahor","eppar nahor","eppar nahor",	"Szara, krzaczasta ple�� o bardzo dziwnej konsystencji i jeszcze dziwniejszym zapachu.",	227,PLANT_PART_NONE ,50,9,	{0,0,0,0,0,0,0,0,0,0,	0,0,0,0,20,55,0,40,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0},	PLANT_EFFECT_NONE,"opis smaku",	0,"opis palenia"},
{"malutki czarnoszary grzybek","malutkiego czarnoszarego grzybka","malutkiemu czarnoszaremu grzybkowi","malutki czarnoszary grzybek","malutkim czarnoszarym grzybkiem","malutkim czarnoszarym grzybku",	"eaghar ort'gha","eaghar ort'gha","eaghar ort'gha","eaghar ort'gha","eaghar ort'gha","eaghar ort'gha",	"Niespotykanie malutki grzybek w czarnoszarym kolorze.",	84,PLANT_PART_FUNGUS  ,20,9,	{0,0,0,0,0,0,0,0,0,0,	0,0,0,0,45,20,0,35,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0},	PLANT_EFFECT_NONE,"opis smaku",	0,"opis palenia"},

	{"unknown", "unknown", "unknown", "unknown", "unknown", "unknown",
		    "unknown", "unknown", "unknown", "unknown", "unknown", "unknown",
		    "unknown",
		    0, 0, 0, 10,
		    {0, 0, 0, 0, 0,       //to prawdopodobienstwa w sectorach, ida po kolei jak w sector_table
		     0, 0, 0, 0, 0,
		     0, 0, 0, 0, 0,
		     0, 0, 0, 0, 0,
		     0, 0, 0, 0, 0,
		     0, 0, 0, 0, 0,
		     0, 0, 0, 0, 0,
		     0, 0, 0, 0, 0,
		     0, 0, 0, 0, 0},          //jest 45 czyli na zapas...
		    0, "unknown",
		    0, "unknown"
	    },



};

//rellik: carve, czesci ciala do wyciecia przez carve
//uwaga: jesli dodajesz cos z vnum i taka sama rasa bez vnum to z vnum musi byc przed rasa inaczej wezmie zawsze wpis z rasa. Tak jak niedzwiedz.
const 	struct monster_body_parts	body_parts_table	[]		=
{
    /* PROSZE NIE ZMIENIAC KOLEJNOSCI */
    /*
       {
       keyword, dope�niacz, celownik, biernik, narz�dnik, miejscownik,
       mianownik, long_desc, item_desc,
       race, vnum, identyfikator czesci dla danej rasy, difficult, chance, cost, gender, czy_niszczy_sk�r�, czy_ma_funkcje, kolor, czy pokazywa� �e do wyci�cia
       }, komentarz: je�li vnum=0 szuka po race jesli !=0 to musi sie vnum zgadzac
     */
    { // 0
        "�uska", "{G�uski smoka{x", "{G�usce smoka{x", "{G�uske smoka{x", "{G�uska smoka{x", "{G�usce smoka{x",
        "{G�uska smoka{x", "Ogromna {G�uska{x smoka", "Ogromna {G�uska{x smoka",
        "smok", 0, A, 60, 30, 12000, SEX_FEMALE, TRUE, FALSE, "{G", TRUE
    },
    { // 1
        "kie� kielsmoka", "{Wk�a smoka{x", "{Wk�owi smoka{x", "{Wkie� smoka{x", "{Wk�em smoka{x", "{Wkle smoka{x",
        "{Wkie� smoka{x", "{WKie�{x smoka", "Ogromny polyskuj�cy jadem {Wkie�{x smoka",
        "smok", 0, B, 80, 70, 15000, SEX_MALE, FALSE, TRUE, "{W", FALSE
    },
    { // 2
        "pazur", "{rpazura smoka{x", "{rpazurowi smoka{x", "{rpazur smoka{x", "{rpazurem smoka{x", "{rpazurze smoka{x",
        "{rpazur smoka{x", "{rPazur{x smoka",	"Ogromny ostry {rpazur{x smoka",
        "smok", 0, C, 50, 50, 10000, SEX_MALE, FALSE, TRUE, "{r", TRUE
    },
    { // 3
        "flaki", "{yflak�w{x", "{yflakom{x", "{yflaki{x", "{yflakami{x", "{yflakach{x",
        "{yflaki{x", "Rozwalone {yflaki{x, niesmaczny widok",	"Cuchn�ce, paruj�ce {yflaki{x, od biedy pewnie mo�na to zje��.",
        "pies", 0, A, 10, 100, 1, SEX_NEUTRAL, TRUE, TRUE, "{y", TRUE
    },
    { // 4
        "genitalia", "{wgenitali�w{x", "{wgenitaliom{x", "{wgenitalia{x", "{wgenitaliami{x", "{wgenitaliach{x",
        "{wgenitalia{x", "Jaki� pechowiec straci� swoje {wgenitalia{x",	"Chyba mo�esz u�y� tego jako bardzo sugestywnego naszyjnika",
        "czlowiek", 6030, A, 20, 60, 10, SEX_NEUTRAL, FALSE, TRUE, "{w", FALSE
    },
    { // 5 Ogromny niedzwiedz
        "ucho", "{wucha nied�wiedzia{x", "{wuchu nied�wiedzia{x", "{wucho nied�wiedzia{x", "{wuchem nied�wiedzia{x", "{wuchu nied�wiedzia{x",
        "{wucho nied�wiedzia{x", "Nied�wied�, kt�ry by� w�a�cicielem tego {wucha{x, musia� by� ogromny", "Tak, to by� wyj�tkowy {wnied�wied�{x, teraz zosta�o tylko {wucho{x.",
        "nied�wied�", 7805, A, 40, 80, 60, SEX_NEUTRAL, FALSE, TRUE, "{w", FALSE
    },
    { // 6 reszta niedzwiedzi
        "ucho", "{yucha nied�wiedzia{x", "{yuchu nied�wiedzia{x", "{yucho nied�wiedzia{x", "{yuchem nied�wiedzia{x", "{yuchu nied�wiedzia{x",
        "{yucho nied�wiedzia{x", "Nied�wiedzie {yucho{x",	"{yUcho nied�wiedzia{x, na pami�tk�.",
        "nied�wied�", 0, A, 40, 80, 60, SEX_NEUTRAL, FALSE, TRUE, "{y", TRUE
    },
    { // 7
        "ogon kita", "{Rogona lisa{x", "{Rogonowi lisa{x", "{Rogon lisa{x", "{Rogonem lisa{x", "{Rogonie lisa{x",
        "{Rogon lisa{x", "Lisia kita poniewiera si� tutaj na ziemi.", "Pi�kna lisia {Rkita{x.",
        "lis", 0, A, 20, 70, 3000, SEX_MALE, FALSE, TRUE, "{R", TRUE
    },
    { // 8
        "�apka zaj�cza zajeczalapka", "{W�apki zaj�czej{x", "{W�apce zaj�czej{x", "{W�apka zaj�cza{x", "{W�apk� zaj�cz�{x", "{W�apce zaj�czej{x",
        "{W�apka zaj�cza{x", "Na ziemi le�y sobie zaj�cza �apka", "{WZaj�cza �apka{x, pewnie przynosi szcz�cie.",
        "zaj�c", 0, A, 30, 70, 60, SEX_FEMALE, TRUE, TRUE, "{W", TRUE
    },
    { // 9
        "serce kolosa", "{sserca kolosa{x", "{ssercu kolosa{x", "{sserce kolosa{x", "{ssercem kolosa{x", "{ssercu kolosa{x",
        "{sserce kolosa{x", NULL, NULL,
        "umberkolos", 0, A, 25, 65, 10, SEX_NEUTRAL, TRUE, TRUE, "{s", TRUE
    },
    { // 10
        "piszczel", "{Wpiszczeli ogra{x", "{Wpiszczeli ogra{x", "{Wpiszczel ogra{x", "{Wpiszczel� ogra{x", "{Wpiszczeli ogra{x",
        "{Wpiszczel ogra{x", "Ogromna {Wpiszczel{x ogra", "Ogromna {Wpiszczel{x wyrwana z jakiego� ogra, wygl�da jak ca�kiem s�uszna maczuga.",
        "ogr", 0, A, 45, 40, 100, SEX_FEMALE, TRUE, TRUE, "{W", TRUE
    },
    { // 11
        "rogi demona", "{Rrog�w{x {rdemona{x.", "{Rrogom{x {rdemona{x", "{Rrogi{x {rdemona{x.", "{Rrogami{x {rdemona{x.", "{Rrogach{x {rdemona{x.",
        "{Rrogi{x {rdemona{x", "Zakrwawione {Rrogi{x {rdemona{x.", "Te rogi musia�y nale�e� kiedy� do bardzo pot�nego i specyficznego stworzenia. Wydaje ci si�, �e nadal s� nasi�kni�te jak�� moc�. Wa�� ponad 5 kilogram�w. By�yby wspania�ym trofeum dla ka�dego bohatera. Chyba, �e znajdziesz dla nich jakie� inne zastosowanie...",
        "demon", 2891, A, 50, 50, 22000, SEX_NEUTRAL, FALSE, TRUE, "{R", FALSE
    },
    { // 12
        "szpony gryfa", "{Wszponow {Dgryfa{x", "{Wszponom {Dgryfa{x", "{Wszpony {Dgryfa{x", "{Wszponami{Dgryfa{x", "{Wszponach {Dgryfa{x",
        "{Wszpony {Dgryfa{x", "{Wszpony {Dgryfa{x ze zwisaj�cymi strz�pkami sk�ry", "Szpony na tyle ostre, �e ci�ko je podnie�� bez kaleczenia si�. Ich rozmiar i kszta�t dobitnie �wiatczy o pot�dze stworzenia, z cia�a kt�rego zosta�y wyci�te. Sporo wa�� i wydaj� si� by� twardsze ni� stal. Gdy ich dotykasz czujesz lekkie mrowienie d�oni. Wspania�e trofeum dla prawdziwych �owc�w.",
        "gryf", 42820, A, 80, 70, 12000, SEX_NEUTRAL, FALSE, TRUE, "{W", TRUE
    },
    { //13
        "skrzydlo nietoperza skrzydlo-nietoperza", "{sskrzyd�a{x nietoperza", "{sskrzyd�u{x nietoperza", "{sskrzyd�o{x nietoperza", "{sskrzyd�em{x nietoperza", "{sskrzyd�u{x nietoperza",
        "{sskrzyd�o{x nietoperza", "Obci�te {sskrzyd�o{x nietoperza", "{sSkrzyd�o{x obci�te jakiemu� nietoperzowi. Ciekawe po co to komu.",
        "nietoperz", 0, A, 1, 95, 10, SEX_NEUTRAL, TRUE, TRUE, "{s", TRUE
    },
    { //14
        "ogon szczura ogon-szczura", "szczurzego {wogona{x", "szczurzemu {wogonowi{x", "szczurzy {wogon{x", "szczurzym {wogonem{x", "szczurzym {wogonie{x",
        "szczurzy {wogon{x", "Obci�ty szczurzy {wogon{x", "{sOgon{x, wygl�da na sczurzy.",
        "szczur", 0, A, 1, 95, 10, SEX_NEUTRAL, FALSE, TRUE, "{w", TRUE
    },
    { //15
        "rogjednorozca r�g jednoro�ca", "{Wroga jednoro�ca{x", "{Wrogu jednoro�ca{x", "{Wr�g jednoro�ca{x", "{Wrogiem jednoro�ca{x", "{Wrogu jednoro�ca{x",
        "{Wr�g jednoro�ca{x", "Obci�ty jednoro�cowi {Wr�g{x", "D�ugi {Wr�g jednoro�ca{x �wiec�cy bladym blaskiem.",
        "jednoro�ec", 0, A, 70, 50, 22000, SEX_NEUTRAL, FALSE, TRUE, "{W", TRUE
    },
    { //16
        "k�y wilka", "{Wk��w wilka{x", "{Wk�om wilka{x", "{Wk�y wilka{x", "{Wk�ami wilka{x", "{Wk�ach wilka{x",
        "{Wk�y wilka{x", "Pi�kne, ostre wilcze {Wk�y{x", "Ostre jak brzytwa {Wk�y{x wilka.",
        "wilk", 0, A, 40, 70, 4500, SEX_NEUTRAL, FALSE, TRUE, "{W", TRUE
    },
    { //17
        "ogon wywerny", "{gogona wywerny{x", "{gogonowi wywerny{x", "{gogon wywerny{x", "{gogonem wywerny{x", "{gogonowi wywerny{x",
        "{gogon wywerny{x", "D�ugi {gogon{x zako�czony jadowitym grotem.", "D�ugi {gogon{x zako�czony jadowitym grotem.",
        "wywerna", 0, A, 50, 70, 9000, SEX_MALE, FALSE, TRUE, "{g", TRUE
    },
    { //18
        "pazury niedzwiedzia", "{wpazur�w nied�wiedzia{x", "{wpazurom nied�wiedzia{x", "{wpazury nied�wiedzia{x", "{wpazurami nied�wiedzia{x", "{wpazurach nied�wiedzia{x",
        "{wpazury nied�wiedzia{x", "Pazury, s�dz�c po wielko�ci i kszta�cie - nied�wiedzie.", "Pazury, s�dz�c po wielko�ci i kszta�cie - nied�wiedzie.",
        "nied�wied�", 0, B, 30, 70, 4500, SEX_NEUTRAL, FALSE, TRUE, "{w", TRUE
    },
    { //19
        "k�y tyranozaura", "{Wk��w tyranozaura{x", "{Wk�om tyranozaura{x", "{Wk�y tyranozaura{x", "{Wk�ami tyranozaura{x", "{Wk�ach tyranozaura{x",
        "{Wk�y tyranozaura{x", "Olbrzymie k�y tyranozaura.", "Olbrzymie k�y, wyci�te z tyranozaura.",
        "gad", 3049, A, 30, 90, 360000, SEX_NEUTRAL, FALSE, TRUE, "{W", TRUE
    },
    { //20
        "k�y likantropa", "{Wk��w likantropa{x", "{Wk�om likantropa{x", "{Wk�y likantropa{x", "{Wk�ami likantropa{x", "{Wk�ach likantropa{x",
        "{Wk�y likantropa{x", "Ostre jak brzytwy k�y likantropa.", "Niesamowicie ostre k�y, wyci�te z likantropa.",
        "likantrop", 0, A, 30, 75, 15000, SEX_NEUTRAL, FALSE, TRUE, "{W", TRUE
    },
    { //21
        "serce behemota sercebehemota", "{Rserca{x {rbehemota{x", "{Rsercu{x {rbehemota{x", "{Rserce{x {rbehemota{x", "{Rsercem{x {rbehemota{x", "{Rsercu{x {rbehemota{x",
        "{rserce{x {Rbehemota{x", "Stale ociekaj�ce {rkrwi�{x serce pot�nego behemota.", "Serce wielko�ci g�owy, najprawdopodobniej wyci�te z jakiego� mitycznego potwora",
        "behemot", 17929, A, 60, 55, 36000, SEX_NEUTRAL, TRUE, TRUE, "{R", TRUE
    },
    { //22
        "serce zielonego smoka sercesmoka", "{rserca{x {Gzielonego smoka{x", "{rsercu{x {Gzielonego smoka{x", "{Rserce{x {Gzielonego smoka{x", "{Rsercem{x {Gzielonego smoka{x", "{Rsercu{x {Gzielonego smoka{x",
        "{rserce{X {Gzielonego smoka{x", "Nadal pulsuj�ce niesamowit� moc� serce smoka.", "Pulsuj�ce magi� serce wyci�te ze smoka.",
        "smok", 9320, A, 70, 45, 86000, SEX_NEUTRAL, TRUE, TRUE, "{G", FALSE
    },
    { //23
        "poro�e", "poro�a", "poro�u", "poro�e", "poro�em", "poro�u",
        "poro�e", "Na ziemi le�y poro�e jelenia", "Obracasz w d�oni wyci�te poro�e jelenia, trofeum w sam raz nadaj�ce si� by powiesi� je na �cianie.",
        "jele�", 0, A, 30, 75, 3000, SEX_NEUTRAL, FALSE, TRUE, "{W", TRUE
    },
    { // 24
        "kr�licza �apka", "{Wkr�liczej �apki{x", "{Wkr�liczej �apce{x", "{Wkr�licza �apka{x", "{Wkr�licz� �apk�{x", "{Wkr�liczej �apce{x",
        "{Wkr�licza �apka{x", "Na ziemi le�y w kurzu kr�licza �apka." ,"Kr�licza �apka wygl�da normalnie.",
        "kr�lik", 0, A, 30, 70, 60, SEX_FEMALE, FALSE, TRUE, "{W", TRUE
    },
    { // 25
        "trupi palec trupi-palec", "{Wtrupiego palca{x", "{Wtrupiemu palcu{x", "{Wtrupi palec{x", "{Wtrupim palcem{x", "{Wtrupim palcu{x",
        "{Wtrupi palec{x", "Le�y tutaj palec jakiego� trupa." ,"Widzisz tutaj palec, oderwany z jakiego� trupa.",
        "zombi", 0, A, 10, 80, 1, SEX_MALE, FALSE, TRUE, "{W", TRUE
    },

    { // 26
        "szpony ghula szpony-ghula", "{yszpon�w {cghula{x", "{yszponom {cghula{x", "{yszpony {cghula{x", "{yszponami {cghula{x", "{yszponach {cghula{x",
        "{yszpony {cghula{x", "Ociekajace smierdzaca mazia szpony leza tutaj." ,"Niewielkie szpony, ca�e pokryt� mazia, kt�ra powoduje, �e chce ci si� rzygac przy ka�dym spojrzeniu na nie.",
        "ghul", 0, A, 15, 80, 1, SEX_FEMALE, FALSE, TRUE, "{y", TRUE
    },

    { // 27
        "szpony ghasta szpony-ghasta", "{yszpon�w {cghasta{x", "{yszponom {cghasta{x", "{yszpony {cghasta{x", "{yszponami {cghasta{x", "{yszponach {cghasta{x",
        "{yszpony {cghasta{x", "Ociekajace smierdzaca mazia szpony leza tutaj." ,"Niewielkie szpony, ca�e pokryt� mazia, kt�ra powoduje, �e chce ci si� rzygac przy ka�dym spojrzeniu na nie.",
        "ghast", 0, A, 20, 80, 1, SEX_FEMALE, FALSE, TRUE, "{y", TRUE
    },

    { //28
        "czaszka licza czaszka-licza", "{Wczaszki licza{x", "{Wczaszce licza{x", "{Wczaszka licza{x", "{Wczaszk� licza{x", "{Wczaszce licza{x",
        "{Wczaszka licza{x", "Widzisz tutaj wiekow�, niemal czarn� czaszk�.", "Czaszka, ze staro�ci i od walania si� w brudzie znacznie poczernia�a. Potyliczna cze�� jest nieco peknieta, dok�adnie tam gdzie brakuj� kawa�ka kosci.",
        "licz", 0, A, 30, 75, 0, SEX_FEMALE, FALSE, TRUE, "{W", TRUE
    },

    { //29
        "k�y wampira k�y-wampira", "{Dwampirzych k��w{x", "{Dwampirzym k�om{x", "{Dwampirze k�y{x", "{Dwampirzymi k�ami{x", "{Dwampirzych k�ach{x",
        "{Dwampirze k�y{x", "Ostre jak brzytwy k�y le�� tutaj.", "Niesamowicie ostre k�y, wyci�te jakiemu� wampirowi.",
        "wampir", 0, A, 35, 75, 9500, SEX_NEUTRAL, FALSE, TRUE, "{W", TRUE
    },


    {
        NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL,
        NULL, 0, 0, 0, 0, 0, 0, FALSE, FALSE, NULL
    },
};



const	struct	trick_type	trick_table	[MAX_TRICKS]	=
{

/*struct   trick_type
{
    char*	name; nazwa trika
    int		which_skill; ktorego skilla jest to trick ( w sn )
    int		which_skill2; jak -1 to nic nie robi, w przypadku gry trik ma wymagac dwoch skilli, np dragon strike 'spear' i 'twohanded weapon'
    int		alternate_skill1;//jak -1 to nic, skill, ktory jest alternatywa dla which_skill
    int		skill_percent; na ile musisz skilla wytrenowac by sie mozna bylo tricka nauczyc
    TRICK_FUN* trick_fun; sama funkcja
    int		frequency; na ile tickow gwiazdkuje <- to jeszcze nie dziala
    int		chance;  jaka szansa ze zaskoczy przy uzyciu skilla (w dziesiatych procenta!)
    int		learn_freq; co ile tickow mozna probowac introduce <- to jeszcze nie dziala


};*/
//NIE ZMIENIAC KOLEJNOSCI!
	{//0
	"reserved",
	0,
	-1,
	-1,
	0,
	trick_null,
	0,
	0,
	0,
	},

	{//1
	"vertical kick",
	119,//sn kicka
	-1,//nie wymaga innych skilli
	-1,//i nie ma alternatywnego
	85,//trza miec na 85%
	trick_vertical_kick,//wywolywana funkcyjka
	7,//jak juz zaskoczy to przez nastepne 7 tickow nie zaskoczy)
	45,//4,5% ze zaskoczy przy uzyciu skilla kick
	8,//co 8 tickow mozna probowac introduce
	},

	{//2
	"entwine",
	112,//whip
	-1,
	-1,
	80,
	trick_entwine,
	1,
	45,//4,5% na kazde trafienie whipem
	6,
	},

	{//3
	"riposte",
	120, //parry
	-1,
	-1,
	85,
	trick_riposte,
	1,
	53,//5,3% na kazde udane parry
	8,
	},

	{//4
	"cyclone",
	211, //dualwield
	-1,
	-1,
	85,
	trick_cyclone,
	10,
	15,//1,5% na kazda runde walki walczac dualwieldem
	10,
	},

	{//5
	"flabbergast",
	114, //bash
	-1,
	-1,
	75,
	trick_flabbergast,
	3,
	95,//9,5% na kazdego udanego basha
	6,
	},

	{//6
	"staff swirl",
	108, //staff
	187, //twohanded weapon
	-1,
	75,
	trick_staff_swirl,
	3,
	60,//6% na kazda runde walki uzywajac 2h staffa
	8,
	},

	{//7
	"dragon strike",
	110, //spear
	187, //twohanded weapon
	-1,
	91,
	trick_dragon_strike,
	5,
	20,//2% na kazda runde walki uzywajac 2h wlocznie (zamiast rundy walki robi cos milego)
	10,
	},

	{//8
	"decapitation",
	103, //axe
	238, //critical strike
	-1,
	91,
	trick_decapitation,
	8,
	10,//1% przy udanym trafionym critical striku jakimkolwiek toporem
	10,
	},

	{//9
	"thundering whack",
	209, //stun
	-1,
	-1,
	85,
	trick_thundering_whack,
	3,
	40,//4% przy jakimkolwiek stunie
	6,
	},

	{//10
	"strucking wallop",
	173, //charge
	283, //mighty blow
	-1,
	85,
	trick_strucking_wallop,
	3,
	75,//7,5% (przy szarzy wywracajacej only)
	8,
	},

	{//11
	"shove",
	117, //dodge
	122, //trip
	-1,
	75,
	trick_shove,
	2,
	70,//7% przy udanym dodge
	6,
	},

	{//12
	"ravaging orb",
	105, //flail
	187, //twohanded weapon
	-1,
	80,
	trick_ravaging_orb,
	3,
	25,//2,5% na kazda runde walki 2h flailem (zamiast rundy walki robi cos milego)
	8,
	},

	{//13
	"bleed",
	111, //sword
	-1,
	188, //short-sword
	80,
	trick_bleed,
	2,
	18,//1,8% przy kazdym trafieniu swordem badz short swordem
	8,
	},

	{//14
	"thigh jab",
	104, //dagger
	-1,
	-1,
	80,
	trick_thigh_jab,
	3,
	18,//1,8% przy kazdym trafieniu sztyletem
	8,
	},

	{//15
	"weapon wrench",
	385, //whip mastery
	116, //disarm
	-1,
	75,
	trick_weapon_wrench,
	3,
	25,//2,5% na wejscie przy zwyklym ciosie, 25% na wejscie jak ktos proboje parowac kogos z tym trikiem
	8,
	},

	{//16
	"crushing mace",
	106, //mace
	274, //mace mastery
	-1,
	80,
	trick_crushing_mace,
	3,
	20,//2% na wejscie przy zwyklym ciosie macka
	8,
	},

	{//17
	"thousandslayer",
	425, //cleave
	424, //overwhelming
	-1,
	75,
	trick_thousandslayer,
	3,
	150,//15% na wejscie przy cleavie
	8,
	},

	{//18
	"glorious impale",
	110, //spear mastery
	187, //twohanded weapon
	-1,
	82,
	trick_glorious_impale,
	5,
	16,//1,6% na kazdy trafiony cios 2h wlocznia
	10,
	},

	{//19
	"divine impact",
	225, //smite evil
	-1,
	430,//smite good
	75,
	trick_divine_impact,
	5,
	48,//4,8% przy smicie
	10,
	},

	{//20
	"dreadful strike",
	424, //overwh
	423, //torment
	-1,
	75,
	trick_dreadful_strike,
	5,
	40,//4% przy ciosie overwhelmingiem kiedy ma sie naladowanego tormenta
	10,
	},
};

//rellik: komponenty, struktura wi���ca czar z komponentem.
const struct spell_items_type spell_items_table[] =
{
    //kolejno�� hmm ... nieistotna; dobrze pisa� w grupach sn
    //ilo�� wszystkich komponent�w ograniczona rozmiarem bigflag (w tej chwili jest 10 int�w wi�c daje 320 warto�ci, w razie potrzeby zwi�kszy�
    //maksymalnie 99 komponent�w per sn, max 30 grup per sn+nazwa_czaru, max 30 element�w w grupie
    /*
       numer komponentowo-spellowy - MUSI BY� UNIKATOWY I NIE MO�E BY� 0!!! (u�ywany do kojarzenia komponentu z wiedz� o nim postaci oraz do okre�lania funkcji specjalnych po wyczerpaniu)
       					poniewa� kolejno�� jest dowolna to jest to najpewniejszy identyfikator tandemu komponent-spell
       numer spella,
       wewn. nazwa spella,
       nazwa komponentu do u�ytkownika,
       nazwa komponentu (istniej�ca w name przedmiotu),
       opis podczas wykorzystywania
       czy wybucha po roz�adowaniu (specjalne rzeczy w magic.c=>spell_item_destroy)
       czy wybucha po sko�czeniu timera (specjalne rzeczy w magic.c=>spell_item_destroy)
       czy posiada specjaln� funkcj� sprawdzaj�c�
       numer grupy ( od 1 do 30 ) - je�li 0 to nie jest elementem grupy, max 30 grup per sn i 30 komponentow w grupie
     */
    {  1, 94, "summon PC", "kielsmoka", "Kie� smoka", "Czubek k�a l�ni b��kitnym �wiat�em gdy kumuluje si� na nim energia, po czym jej w�ska wi�zka wystrzeliwuje w g�r�.", TRUE, FALSE, FALSE, 0 },
    {  2, 63, "ident jak poznaniak", "lisc-zwyczajny", "{gli��{x", "{gLi��{x zwija si� i kurczy.", TRUE, FALSE, FALSE, 1 },
    {  3, 63, "ident jak poznaniak", "skrzydlo-nietoperza", "{sskrzyd�o{x nietoperza", "{sskrzyd�o{x nietoperza trzepocze po�miertnym skurczem mi�ni.", TRUE, FALSE, FALSE, 1 },
    {  4, 63, "ident jak poznaniak", "ogon-szczura", "szczurzy {wogon{x", "szczurzy {wogon{x wypr�a si�.", TRUE, FALSE, FALSE, 1 },
    {  5, 63, "ident jak poznaniak", "nefryt", "{Gnefryt{x", "Przez chwil� ziele� {gnefrytu{x staje si� g��bsza.", FALSE, FALSE, FALSE, 1 },
    /* * free 6-18 */
    { 20, 278, "raise ghoul", "szpony-ghula", "{bszpony{x {sghula{x", "{bSzpony{x {sghula{x wbijaj� si� g��boko w cia�o.", TRUE, TRUE, FALSE, 0 },
    { 21, 279, "raise ghast", "szpony-ghasta", "{bszpony{x {sghasta{x", "{bSzpony{x {sghasta{x wbijaj� si� g��boko w cia�o.", TRUE, TRUE, FALSE, 0 },
    { 22, 92, "mirror", "lustro-mirror", "{Clustro{x", "Czar odbija si� w {Clustrze{x powoduj�c powstanie dodatkowych odbi�.", TRUE, FALSE, FALSE, 0 },
    { 23, 258, "raise zombie", "trupi-palec", "{strupi palec{x", "{sTrupi palec{x przy�o�ony do cia�a wrasta i zdaje si� je o�ywia�.", TRUE, TRUE, FALSE, 0 },
    { 24, 281, "orb of entropy", "kamienny-pierscien", "{wkamienny pier�cie�{x", "{wkamienny pier�cie�{x dr�y przepe�niony moc�.{x", TRUE, FALSE, TRUE, 0 },
    { 25, 135, "liveoak slaby", "drobne-kawalki-kory", "{ydrobne kawa�ki kory{x", "Z {ydrobnych kawa�k�w kory{x tworzy si� poka�ne drzewo.", TRUE, FALSE, FALSE, 0 },
    { 26, 135, "liveoak mocny", "kawalki-kory", "{ykawa�ki kory{x", "Z {ykawa�k�w kory{x tworzy si� ogromne drzewo.", TRUE, FALSE, FALSE, 0 },

    /**
     * druid
     */
    /* druid: Sn: 368 Spell: ring of vanion */
    { 19, 368, "ring of vanion", "liscwielkiegodebu", "{Gwielki zielony lisc{x", "{GWielki zielony li��{x traci sw�j kolor, robi si� coraz cie�szy, po czym rozpada si� bez �ladu.", TRUE, TRUE, FALSE, 0 },
    { 29, 368, "ring of vanion", "zoladzwielkiegodebu", "{Wsrebrzysto po�yskuj�cy, ma�y �o��d�{x", "{SSrebrzysto po�yskuj�cy �o��d�{x kurczy si�, po czym znika z cichym trzaskiem.", TRUE, FALSE, FALSE, 0 },
    /* druid: Sn: 369 Spell: circle of vanion */
    { 27, 369, "circle of vanion", "liscwielkiegodebu", "{Gwielki zielony li��{x", "{GWielki zielony li��{x traci sw�j kolor, robi si� coraz cie�szy, po czym rozpada si� bez �ladu.", TRUE, TRUE, FALSE, 1 },
    { 28, 369, "circle of vanion", "zoladzwielkiegodebu", "{Wsrebrzysto po�yskuj�cy, ma�y �o��d�{x", "{SSrebrzysto po�yskuj�cy �o��d�{x kurczy si�, po czym znika z cichym trzaskiem.", TRUE, FALSE, FALSE, 1 },
    /* druid: Sn: 241 Spell: alicorn lance */
    { 30, 241, "alicorn lance", "rogjednorozca", "{Wr�g jednorozca{x", "Zawarta w rogu magiczna moc jednoro�ca uwalnia si�.", TRUE, FALSE, FALSE, 0 },

    /**
     * acid spells
     */
    /* Sn:   1 Spell: acid blast                       Szkola: Inwokacje     Profesje: Mag*/
    {  31,   1, "acid based spell", "buteleczkazkwasem", "{gbuteleczka z kwasem{x", "{gKropla kwasu znika w powietrzu{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 175 Spell: resist acid                      Szkola: Odrzucanie    Profesje: Mag Kleryk Druid*/
    {  33, 175, "acid based spell", "buteleczkazkwasem", "{gbuteleczka z kwasem{x", "{gKropla kwasu znika w powietrzu{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 193 Spell: acid hands                       Szkola: Przemiany     Profesje: Mag*/
    {  34, 193, "acid based spell", "buteleczkazkwasem", "{gbuteleczka z kwasem{x", "{gKropla kwasu znika w powietrzu{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 268 Spell: endure acid                      Szkola: Odrzucanie    Profesje: Mag Kleryk Druid*/
    {  35, 268, "acid based spell", "buteleczkazkwasem", "{gbuteleczka z kwasem{x", "{gKropla kwasu znika w powietrzu{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 405 Spell: acid arrow                       Szkola:               Profesje: Mag*/
    {  36, 405, "acid based spell", "buteleczkazkwasem", "{gbuteleczka z kwasem{x", "{gKropla kwasu znika w powietrzu{x.", TRUE, FALSE, FALSE, 0 },

    /**
     * lighting spells
     */
    /* Sn: 269 Spell: endure lightning                 Szkola: Odrzucanie    Profesje: Mag Kleryk Druid */
    {  37, 269, "lighting based spell", "bursztyn", "{ykawa�ek bursztynu{x", "{yDrobne {Yiskry{y skacz� po bursztynie{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 146 Spell: resist lightning                 Szkola: Odrzucanie    Profesje: Mag Kleryk Druid */
    {  32, 146, "lighting based spell", "bursztyn", "{ykawa�ek bursztynu{x", "{yDrobne {Yiskry{y skacz� po bursztynie{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn:  69 Spell: lightning bolt */
    {  53, 69, "lighting based spell", "bursztyn", "{ykawa�ek bursztynu{x", "{yDrobne {Yiskry{y skacz� po bursztynie{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn:  88 Spell: shocking grasp    */
    {  54, 88, "lighting based spell", "bursztyn", "{ykawa�ek bursztynu{x", "{yDrobne {Yiskry{y skacz� po bursztynie{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn:  13 Spell: chain lightning */
    {  55, 13, "lighting based spell", "bursztyn", "{ykawa�ek bursztynu{x", "{yDrobne {Yiskry{y skacz� po bursztynie{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 296 Spell: thunder bolt*/
    {  56, 296, "lighting based spell", "bursztyn", "{ykawa�ek bursztynu{x", "{yDrobne {Yiskry{y skacz� po bursztynie{x.", TRUE, FALSE, FALSE, 0 },

    /**
     * fire based spells
     */
    /* Sn: 266 Spell: endure fire                      Szkola: Odrzucanie    Profesje: Mag Kleryk Druid */
    {  38, 266, "fire based spell", "agatognisty", "{ragat {Rognisty{x", "{rAgat {Rognisty{r b�yska ciep�ym �wiat�em{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 143 Spell: resist fire*/
    {  57, 143, "fire based spell", "agatognisty", "{ragat {Rognisty{x", "{rAgat {Rognisty{r b�yska ciep�ym �wiat�em{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn:  48 Spell: fireball*/
    {  58, 48, "fire based spell", "agatognisty", "{ragat {Rognisty{x", "{rAgat {Rognisty{r b�yska ciep�ym �wiat�em{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 477 Spell: flame lace */
    {  59, 477, "fire based spell", "agatognisty", "{ragat {Rognisty{x", "{rAgat {Rognisty{r b�yska ciep�ym �wiat�em{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 264 Spell: burst of flame */
    {  60, 264, "fire based spell", "agatognisty", "{ragat {Rognisty{x", "{rAgat {Rognisty{r b�yska ciep�ym �wiat�em{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 265 Spell: burst of fire */
    {  61, 265, "fire based spell", "agatognisty", "{ragat {Rognisty{x", "{rAgat {Rognisty{r b�yska ciep�ym �wiat�em{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 240 Spell: sunscorch  */
    {  62, 240, "fire based spell", "agatognisty", "{ragat {Rognisty{x", "{rAgat {Rognisty{r b�yska ciep�ym �wiat�em{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn:  50 Spell: flamestrike   */
    {  63, 50, "fire based spell", "agatognisty", "{ragat {Rognisty{x", "{rAgat {Rognisty{r b�yska ciep�ym �wiat�em{x.", TRUE, FALSE, FALSE, 0 },

    /**
     * cold based spells
     */
    /* Sn: 267 Spell: endure cold                      Szkola: Odrzucanie    Profesje: Mag Kleryk Druid */
    {  39, 267, "cold based spell", "srebrnyproszek", "{Dsrebrny proszek{x", "Odrobina {Dsrebrnego proszku{x rozmywa si� w powietrzu.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 145 Spell: resist fire*/
    {  64, 145, "cold based spell", "srebrnyproszek", "{Dsrebrny proszek{x", "Odrobina {Dsrebrnego proszku{x rozmywa si� w powietrzu.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 196 Spell: cold snap  */
    {  65, 196, "cold based spell", "srebrnyproszek", "{Dsrebrny proszek{x", "Odrobina {Dsrebrnego proszku{x rozmywa si� w powietrzu.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 195 Spell: frost hands */
    {  66, 195, "cold based spell", "srebrnyproszek", "{Dsrebrny proszek{x", "Odrobina {Dsrebrnego proszku{x rozmywa si� w powietrzu.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 185 Spell: cone of cold */
    {  67, 185, "cold based spell", "srebrnyproszek", "{Dsrebrny proszek{x", "Odrobina {Dsrebrnego proszku{x rozmywa si� w powietrzu.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 284 Spell: frost rift */
    {  68, 284, "cold based spell", "srebrnyproszek", "{Dsrebrny proszek{x", "Odrobina {Dsrebrnego proszku{x rozmywa si� w powietrzu.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 299 Spell: ice bolt */
    {  69, 299, "cold based spell", "srebrnyproszek", "{Dsrebrny proszek{x", "Odrobina {Dsrebrnego proszku{x rozmywa si� w powietrzu.", TRUE, FALSE, FALSE, 0 },
    /**
     * enchant weapon
     * */
    {  40,  43, "enchant sredni", "sercebehemota", "{Rserce{x {rbehemota{x", "{Rserce{x {rbehemota{x zaczyna pulsowa� nagle czerwonym �wiat�em!", TRUE, FALSE, FALSE, 0 },
    {  41,  43, "enchant mocny", "sercesmoka", "{Rserce{x {Gzielonego smoka{x", "{Rserce{x {Gzielonego smoka{x rozb�yska nagle niezwyk�ym �wiat�em!", TRUE, FALSE, FALSE, 0 },
    /**
     * animate dead
     * */
    {  42, 232, "Animate z licza", "czaszka-licza", "{Wczaszka licza{x", "{WCzaszka licza{x otacza si� czerwon� aur�!", TRUE, FALSE, FALSE, 0},
    {  43, 232, "Animate z wampira", "k�y-wampira", "{Dwampirze k�y{x", "{Dwampirze k�y{x otaczaj� si� czerwon� aur� i rozpadaj� w py�!", TRUE, FALSE, FALSE, 0},
    {  44, 232, "Animate z mumii", "banda�e-mumii", "kupka bandazy", "Banda�e mumii otaczaj� si� czerwon� aur� i rozpadaj� w py�!", TRUE, FALSE, FALSE, 0},
    {  45, 232, "Animate z upiora", "upiorny-py�", "kupka popiolu", "Upiorny py� po�yskuje czerwon� aur� i rozwiewa si�!", TRUE, FALSE, FALSE, 0},
       /**
     * Heal & Harm
     * */
    {  46,  61, "komponent do heal", "�za-isoryka", "fiolka z �z� Isoryka", "Fiolka z �z� Isoryka otacza sie bia�ym �wiat�em!", TRUE, FALSE, FALSE, 0},
    {  47,  59, "komponent do harm", "czaszka-licza", "{Wczaszka licza{x", "{WCzaszka licza{x otacza si� czerwon� aur�!", TRUE, FALSE, FALSE, 0},

    /**
     * spell_mending
     */
    {  48, 170, "komponent do mend", "ksiezycowyproszek", "{Wksi�ycowy proszek{x", "Ksi�ycowy proszek znika w dalikatnym b�ysku �wiat�a.", FALSE, FALSE, FALSE, 0 },
    /**
     * spell_elemental_devastation
     */
    {  49,  99, "acid based spell", "buteleczkazkwasem", "{gbuteleczka z kwasem{x", "{gKropla kwasu znika w powietrzu{x.", TRUE, FALSE, FALSE, 0 },
    {  50,  99, "lighting based spell", "bursztyn", "{ykawa�ek bursztynu{x", "{yDrobne {Yiskry{y skacz� po bursztynie{x.", TRUE, FALSE, FALSE, 0 },
    {  51,  99, "fire based spell", "agatognisty", "{ragat {Rognisty{x", "{rAgat {Rognisty{r b�yska ciep�ym �wiat�em{x.", TRUE, FALSE, FALSE, 0 },
    {  52,  99, "cold based spell", "srebrnyproszek", "{Dsrebrny proszek{x", "Odrobina {Dsrebrnego proszku{x rozmywa si� w powietrzu.", TRUE, FALSE, FALSE, 0 },
    /**
    *blink
    **/
    {  70,  415 , "longer blink", "iluzorycznypyl", "{DIluzoryczny {Wpy�{x", "{DIluzoryczny {Wpy� {Ymigocze przez chwil�{x.", TRUE, TRUE, FALSE, 0 },
    /**
    *create greater illusion
    **/
     {  71,  315 , "cgilvl2", "magiczny-pryzmat", "{Bmagiczny {Ypryzmat{x", "{BMagiczny {Ypryzmat {Rrozb�yskuj� szkar�atnym �wiat�em{x.", TRUE, TRUE, FALSE, 0 },
     {  72,  315 , "cgilvl1", "oko-smoka", "oko smoka", "�renica oka smoka powi�ksza si� na chwil�.", TRUE, TRUE, FALSE, 0 },

     {  73, 402, "wac1", "oko-obserwatora", "oko obserwatora", "�renica oka obserwatora powi�ksza si� na chwil�.", TRUE, TRUE, FALSE,0 },
    /**
     * full haste decrising caster wait time after casting
     * spell: haste
     * sn:    60
     */

    /*
     * UWAGA!
     *
     * Dodajac nowy komponent sprawdz, zeby pierwsza liczba byla NOWA
     * ABSOLUTNIE nie wolno zmieniac tych liczb, bo sa one wiaza w pliku
     * gracza informacje o znanych komponentach.
     *
     * dodatkowo w pliku merc.h ZMIEN wartosc stalej MAX_COMPONENTS
     *
     * Nowy komponent dodaj POWYZEJ tego kometarza.
     */
    /* null - zakonczenie tablicy */
    { 0, 9999, NULL, NULL, NULL, NULL, FALSE, 0 }
};

int spell_items_table_count()
{
	return ( ( sizeof( spell_items_table ) / sizeof( spell_items_table[0] ) ) - 2 );
}

/**
 * Stat info in do_score
 *
 * created by Odkurzacz
 */
 
const char * stat_info_list [] =
{
	"beznadziejna.",
	"godna po�a�owania.",
	"bardzo niska.", 
	"poni�ej przeci�tnej.",
	"�rednia.",
	"nieprzeci�tna.",
	"niez�a.",
	"wysoka.",
	"niezmiernie wysoka.",
	"niespotykana.",
	"legendarna.",
	"p�boska."
};

/**
 * level info common ch->level < 11
 *
 * created by Laszlo (2008-09-04)
 */
const char * level_info_common [] =
{
    "Zjadacz czerstwych bu�ek.",
    "Przeci�tny zjadacz chleba.",
    "Zwyk�y mieszkaniec mie�ciny.",
    "Nieugi�ty morderca s�owik�w.",
    "Pogromca szczur�w.",
    "Podr�nik gubi�cy si� w gwarze miasta.",
    "Osoba, kt�ra nie boi si� wyj�� za pr�g domu.",
    "Osoba, kt�ra dobrze zna miasto.",
    "Podr�nik stawiajacy pierwsze kroki za miastem.",
    "W�drowiec rozpoczynaj�cy sw� w�dr�wk� po �wiecie."
};
/**
 * level info common ch->level > 10 && CLASS_WARRIOR
 *
 * created by Laszlo (2008-09-04)
 */
const char * level_info_warrior [] =
{
    "Pocz�tkuj�cy adept sztuki wojennej.",
    "Osoba pragn�ca pozna� nowe techniki walki.",
    "Pocz�tkujacy w�drowiec szukaj�cy przyg�d w dalekim �wiecie.",
    "Osoba, kt�ra pozna�a ju� pewne techniki walki.",
    "Wojownik nie boj�cy si� walki.",
    "Bohater, splamiony troch� krwi� wrog�w.",
    "Wojownik staraj�cy si� doskonali� technik� walki.",
    "Kto�, kto posiad� ju� pewn� wiedz� na temat walki.",
    "Osoba splamiona krwi� wrog�w.",
    "Podr�nik przemierzaj�cy trakty i bezdro�a �wiata.",
    "Podr�nik, kt�ry potrafi przys�u�y� si� sprawie.",
    "Osoba, kt�ra nie boi si� przyg�d.",
    "Kto�, kto zas�yn�� ju� troch� ze swych czyn�w.",
    "Wojownik, kt�ry doskona�e opanowa� sztuk� wojenna.",
    "Bohater, kt�ry zas�yn�� ze swoich czyn�w w ca�ym �wiecie.",
    "Odwa�ny podr�nik, kt�ry nie l�ka si� trudno�ci.",
    "Nieugi�ty pogromca najprzer�niejszych stwor�w.",
    "Wojownik, kt�ry w ca�o�ci jest pokryty zakrzep�� krwi� wrog�w.",
    "Nieustraszony wojownik.",
    "Bohater, kt�rego zna prawie ca�y �wiat.",
    "Legendarny bohater"
};
/**
 * level info common ch->level > 10 && CLASS_PALADIN
 *
 * created by Laszlo (2008-09-04)
 */
const char * level_info_paladin [] =
{
	"Adept nieznaj�cy jeszcze pe�nej r�nicy miedzy dobrem a z�em.",
	"Osoba zaczynaj�ca kroczy� �cie�k� cn�t.",
	"Pocz�tkuj�cy pogromca z�a",
	"Pogromca ghuli.",
	"Nieustraszony pogromca ghast�w.",
	"Bohater nieznaj�cy jeszcze swych mo�liwo�ci.",
	"Paladyn, maj�cy ju� pewne do�wiadczenie w niszczeniu z�a.",
	"Osoba znaj�ca ju� granice dobra i z�a.",
	"Adept sztuki palady�skiej.",
	"�mia�ek krocz�cy �cie�k� dobra.",
	"Odwa�ny �mia�ek walcz�cy ze z�em.",
	"Przyk�adny paladyn.",
	"Osoba niszcz�ca z�o tego �wiata.",
	"Paladyn nios�cy �ar prawdziwej wiary.",
	"Obro�ca wd�w i niewiast.",
	"Paladyn, kt�rego imi� budzi groz� w z�ych istotach.",
	"Bohater idealnie krocz�cy �cie�k� prawa i sprawiedliwo�ci",
	"Ulubieniec Portena.",
	"Osoba, kt�rego l�ka si� z�o Tamery.",
	"Niedo�cigniony w wyplenianiu z�a.",
	"Uciele�nienie wszystkich cn�t tego �wiata."
};
/**
 * level info common ch->level > 10 && CLASS_CLERIC
 *
 * created by Tanon (2008-09-04)
 */
const char * level_info_cleric [] =
{
	"Nowicjusz �wi�tyni.",
	"Poczatkujacy adept �wi�tyni.",
	"Adept �wi�tyni.",
	"Szukaj�cy �cie�ek wiary.",
	"Pielgrzym.",
	"Niestrudzony pielgrzym.",
	"Krocz�cy �cie�kami wiary.",
	"Osoba, kt�ra odnalaz�a wiar�.",
	"Nios�cy wiar�.",
	"S�uga bog�w.",
	"Niestrudzony s�uga bog�w.",
	"Osoba gotowa, by szerzy� wiar�.",
	"Diakon �wi�tyni.",
	"Kap�an �wi�tyni.",
	"Czcigodny kap�an.",
	"Arcykap�an.",
	"Natchniony wol� bog�w.",
	"Uzdrowiciel.",
	"Nios�cy nadziej�.",
	"Obro�ca wiary.",
	"Pos�aniec bog�w."
};
/**
 * level info common ch->level > 10 && CLASS_BARBARIAN
 *
 * created by Laszlo (2008-09-04)
 */
const char * level_info_barbarian [] =
{
	"Pij�cy chrzczone piwo.",
	"Upijaj�cy si� paroma �ykami jalowc�wki.",
	"Pocz�tkuj�cy smakosz piwa.",
	"Racz�cy si� najpodlejszymi alkoholami �wiata.",
	"Ja�owe podniebienie.",
	"Osoba, kt�ra posmakowa�a sporo trunk�w.",
	"Niestroni�cy od okazji do flaszeczki.",
	"Kochaj�cy sobie goln��.",
	"Ulubieniec karczmarzy.",
	"Postrach gorzelnik�w.",
	"Wytrawny smakosz piwska.",
	"Pogromca bary�ek",
	"Gard�ogrzmot.",
	"Nieznaj�cy umiaru.",
	"Stalowa w�troba.",
	"Pij�cy w zam�cie bitwy.",
	"Pijana furia.",
	"Pijany berserker.",
	"Bestia nieznaj�ca umiaru.",
	"Bohater, kt�ry pi� piwa we wszystkich karczmach Tamery.",
	"Mithrilowa w�troba."
};
/**
 * level info common ch->level > 10 && CLASS_BLACK_KNIGHT
 *
 * created by Tanon(2008-09-04)
 */
const char * level_info_black_knight [] =
{
	"Podr�nik na rozstajach dr�g.",
	"Zagubiony w chaszczach moralno�ci.",
	"Odkrywaj�cy �cie�k� z�a.",
	"Krocz�cy �cie�k� z�a.",
	"Ciemi�ca uci�nionych.",
	"Morderca kobiet i dzieci.",
	"Splamiony krwi� niewinnych.",
	"Pomiot chaosu.",
	"S�uga chaosu.",
	"Rycerz chaosu.",
	"Czciciel mroku.",
	"S�uga mroku.",
	"Blu�nierca.",
	"Pot�piony przez bog�w.",
	"Rycerz mroku.",
	"Nios�cy rozpacz.",
	"W�adca cierpienia.",
	"Wkraczaj�cy w sfer� ciemno�ci.",
	"Rycerz ciemno�ci.",
	"Pan ciemno�ci.",
	"Ksi��e ciemno�ci."
};
/**
 * level info common ch->level > 10 && CLASS_THIEF
 *
 * created by Tanon (2008-09-04)
 */
const char * level_info_thief [] =
{
	"Przedstawiciel marginesu spo�ecznego.",
	"Kradn�cy dzieciom cukierki.",
	"Miejska menda.",
	"Zwyczajna miejska szuja.",
	"Plugawy doliniarz.",
	"Lepkie r�czki.",
	"Cholerny z�odziejaszek.",
	"Paskudny opryszek.",
	"Parszywy szubrawiec.",
	"Oprych natchniony.",
	"Rabu� karawan.",
	"Niestrudzony przepatrywacz sakiewek.",
	"Znawca budowy zamk�w i k��dek.",
	"Bandyta brutalny lecz subtelny.",
	"Z�odziej nieprzeci�tny.",
	"Znawca anatomii plec�w.",
	"Znany zabijaka.",
	"S�awny w�amywacz.",
	"Zab�jca zab�jc�w.",
	"Sztyletu cie�.",
	"Ksi��e z�odziei."
};
/**
 * level info common ch->level > 10 && CLASS_MAG
 *
 * created by Laszlo (2008-09-04)
 */
const char * level_info_mag [] =
{
	"Nowicjusz magiczny.",
	"Pocz�tkuj�cy adept magii.",
	"Adept znaj�cy ju� pewne podstawy.",
	"Adept doskonal�cy podstawy magii.",
	"Kto�, kto posiad� ju� podstawow� wiedz� magiczn�.",
	"Adept magicznych s��w.",
	"Adept magicznych gest�w.",
	"Osoba znaj�ca ju� magiczne s�owa i gesty.",
	"Osoba mog�ca uznawa� siebie za maga.",
	"Mag rozpoczynaj�cy swoje pierwsze badania.",
	"Czarodziej poznaj�cy coraz to nowe arkana.",
	"Nieustraszony poszukiwacz wiedzy.",
	"Czarodziej o kt�rym zaczynaj� kr��y� wie�ci po �wiecie.",
	"Mag doskonal�cy sw�j kunszt.",
	"Mag, kt�rego odkrycia zapewni�y mu spore uznanie.",
	"Nieustraszony badacz zjawisk magicznych.",
	"Uczony, kt�rego wiedza przewy�sza zwyk�ych �miertelnik�w.",
	"Czarownik, kt�rego erudycja przy�miewa innych.",
	"M�drzec uznany na ca�ym �wiecie.",
	"Mistrz magii, znaj�cy wszystkie jej arkana.",
	"Prawdziwy arcymistrz, znawca najtajniejszych formu� i gest�w magicznych."
};
/**
 * level info common ch->level > 10 && CLASS_DRUID
 *
 * created by Tanon (2008-09-04)
 */
const char * level_info_druid [] =
{
	"Lubi�cy siedzie� na trawie.",
	"Mi�o�nik kwiat�w.",
	"Amator grzybk�w.",
	"Pachn�cy �ci�k�.",
	"Mi�o�nik zieleni.",
	"Kochaj�cy zwierz�ta.",
	"Ulubieniec gryzoni.",
	"Cz�owiek lasu.",
	"Pan saren i jeleni.",
	"Znawca lasu.",
	"Spogl�daj�cy prosto w s�o�ce.",
	"Zaklinacz wiatru.",
	"Szukaj�cy r�wnowagi.",
	"Stra�nik natury.",
	"Obro�ca natury.",
	"Nieugi�ty obro�ca natury.",
	"W�adaj�cy mocami natury.",
	"Pan ��k i las�w.",
	"W�adca zwierz�t.",
	"W�adca bestii.",
	"W�adca natury."
};
/**
 * level info common ch->level > 10 && CLASS_BARD
 */
const char * level_info_bard [] =
{
};
/**
 * level info common ch->level > 10 && CLASS_MONK
 */
const char * level_info_monk [] =
{
};

/**
 * level info common ch->level > 10 && CLASS_SHAMAN
 */
const char * level_info_shaman [] =
{
	"Ten, kt�ry szuka pomocy duch�w.",
	"Ten, kt�ry podr�uje drog� przodk�w.",
	"Ten, kt�ry poznaje podstawy rytua��w.",
	"Ten, kt�ry poszukuje drogi w�r�d g�r.",
	"Ten, kt�ry szuka m�dro�ci przodk�w.",
	"Ten, kt�ry rozmawia z duchami.",
	"Ten, kt�ry w�druje w�r�d g�r.",
	"Ten, kt�ry si� nie zatrzymuje.",
	"Ten, kt�ry poszukuje wizji.",
	"Ten, kt�ry rozkazuje duchom.",
	"Ten, kt�ry szepta s�owa m�dro�ci.",
	"Ten, kt�ry patrzy z g�rskiego szczytu.",
	"Ten, kt�ry widzi to co niewidoczne.",
	"Ten, kt�ry odprawia rytua�y.",
	"Ten, kt�ry m�wi g�osem g�r.",
	"Ten, kt�ry znalaz� cel.",
	"Ten, kt�ry m�wi g�osem duch�w.",
	"Ten, kt�ry dost�pi� wizji.",
	"Ten, kt�ry odnalaz� m�dro��.",
	"Ten, kt�ry jest W�adc� G�r.",
	"Ten, kt�ry jest W�adc� G�r."
};

struct repair_item_type repair_item_table[] =
{
    {	"bronie",		REP_ITEM_WEAPON,	"wszystkie rodzaje broni"	},
    {	"dlugie miecze",	REP_ITEM_SWORDS,	"konkretny typ broni"		},
    {	"topory",		REP_ITEM_AXE,		"konkretny typ broni"		},
    {	"mace",			REP_ITEM_MACE,		"konkretny typ broni"		},
    {	"sztylety",		REP_ITEM_DAGGER,	"konkretny typ broni"		},
    {	"wlocznie",		REP_ITEM_SPEAR,		"konkretny typ broni"		},
    {	"korbacze",		REP_ITEM_FLAIL,		"konkretny typ broni"		},
    {	"bicze",		REP_ITEM_WHIP,		"konkretny typ broni"		},
    {	"drzewcowe",		REP_ITEM_POLEARM,	"konkretny typ broni"		},
    {	"laski",		REP_ITEM_STAFF,		"konkretny typ broni"		},
    {	"krotkie miecze",	REP_ITEM_SHSWORD,	"konkretny typ broni"		},
    {	"miecze",		REP_ITEM_SWORDS,	"ogolnie miecze, dlugie, krotkie, 2-reczne"	},
    {	"obuchowe",		REP_ITEM_BLUDGEONS,	"wszyskie bronie obuchowe: mace, flail, staff, polearm" },
    {	"zbroje",		REP_ITEM_ARMOR,		"wszystkie zbroje"			},
    {	"zbroje lekkie",	REP_ITEM_ARMOR_LIGHT,	"zbroje lekkie 0-3 w tablicy pancerzy"	},
    {	"zbroje srednie",	REP_ITEM_ARMOR_MEDIUM,	"zbroje 4-7 w tablicy pancerzy"		},
    {	"zbroje ciezkie",	REP_ITEM_ARMOR_HEAVY,	"zbroje 8-11 w tablicy pancerzy"	},
    {	"zbroje b.ciezkie",	REP_ITEM_ARMOR_VHEAVY,	"zbroje 12-15 w tablicy pancerzy"	},
    {	"odziez",		REP_ITEM_CLOTHING,	"obiekty typu clothing"	},
    {	"bizuteria",		REP_ITEM_JEWELRY,	"obiekty typu jewelry"	},
    {	"kamienie",		REP_ITEM_GEM,		"obiekty typu gem"	},
    {	"instrumenty muzyczne",		REP_ITEM_MUSICAL_INSTRUMENT,		"bardowe instrumenty muzyczne"	},
    {	"tarcze",		REP_ITEM_SHIELD,	"wszystkie tarcze"	},
    { "narz�dzia", REP_ITEM_TOOL, "wszystkie narz�dzia" },
    { "szpony",  REP_ITEM_CLAWS, "konkretny typ broni" },
    {	NULL,			0,			NULL			}
};

