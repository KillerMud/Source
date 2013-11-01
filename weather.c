/*********************************************************************
 *                                                                   *
 * KILLER MUD is copyright 1999-2011 Killer MUD Staff (alphabetical) *
 *                                                                   *
 *   Dominik Andrzejczak (kainti@go2.pl                 ) [Kainti  ] *
 *   Jaron Krzysztof     (chris.jaron@gmail.com         ) [Razor   ] *
 *   Pietrzak Marcin     (marcin.pietrzak@mud.pl        ) [Gurthg  ] *
 *   Sawicki Tomasz      (furgas@killer-mud.net         ) [Furgas  ] *
 *   Trebicki Marek      (maro@killer.radom.net         ) [Maro    ] *
 *   Zdziech Tomasz      (t.zdziech@elka.pw.edu.pl      ) [Agron   ] *
 *                                                                   *
 *********************************************************************/
/* $Id: weather.c 110 2013-07-18 08:34:31Z gurthg $*/
#if defined(macintosh)

#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"

#define SKY_MAX 4


#define TOTAL_WEATHER_PROBABILITY 1000
#define CLEAR_SKY_RANGE  (TOTAL_WEATHER_PROBABILITY / SEASON_MAX)
#define CLOUDY_SKY_RANGE (TOTAL_WEATHER_PROBABILITY / SEASON_MAX)
#define RAINY_SKY_RANGE  (TOTAL_WEATHER_PROBABILITY / SEASON_MAX)
#define LIGHTNING_RANGE  (TOTAL_WEATHER_PROBABILITY / SEASON_MAX)


#define BASE_INFLUENCE ((TOTAL_WEATHER_PROBABILITY / SEASON_MAX) / 4)
#define PLUS4       (BASE_INFLUENCE * 4)
#define PLUS3       (BASE_INFLUENCE * 3)
#define PLUS2       (BASE_INFLUENCE * 2)
#define PLUS1       (BASE_INFLUENCE * 1)
#define NOINFLUENCE (BASE_INFLUENCE * 0)
#define MINUS1      (BASE_INFLUENCE * -1)
#define MINUS2      (BASE_INFLUENCE * -2)
#define MINUS3      (BASE_INFLUENCE * -3)
#define MINUS4      (BASE_INFLUENCE * -4)


typedef struct _weather_influence_data
{
        int clear_sky;
        int cloudy_sky;
        int rainy_sky;
        int lightning_sky;
} weather_influence_data;


static weather_influence_data base_weather =
{
    CLEAR_SKY_RANGE, CLOUDY_SKY_RANGE, RAINY_SKY_RANGE, LIGHTNING_RANGE
};


static weather_influence_data seasonal_weather_table[] =
{
   {NOINFLUENCE, NOINFLUENCE, NOINFLUENCE, MINUS2     },
   {MINUS3,      MINUS2,      NOINFLUENCE, NOINFLUENCE},
   {NOINFLUENCE, NOINFLUENCE, NOINFLUENCE, MINUS2     },
   {NOINFLUENCE, NOINFLUENCE, NOINFLUENCE, MINUS2     }
};



static weather_influence_data sector_weather_table[SECT_MAX] =
{
   {NOINFLUENCE, NOINFLUENCE, NOINFLUENCE, NOINFLUENCE},
   {NOINFLUENCE, NOINFLUENCE, NOINFLUENCE, NOINFLUENCE},
   {NOINFLUENCE, NOINFLUENCE, PLUS1,       PLUS1      },
   {NOINFLUENCE, NOINFLUENCE, NOINFLUENCE, MINUS1     },
   {NOINFLUENCE, NOINFLUENCE, NOINFLUENCE, MINUS2     },

   {NOINFLUENCE, PLUS2,       PLUS2,       NOINFLUENCE},
   {NOINFLUENCE, NOINFLUENCE, NOINFLUENCE, NOINFLUENCE},
   {NOINFLUENCE, NOINFLUENCE, NOINFLUENCE, NOINFLUENCE},
   {NOINFLUENCE, NOINFLUENCE, NOINFLUENCE, NOINFLUENCE},
   {NOINFLUENCE, NOINFLUENCE, MINUS4,      NOINFLUENCE},

   {PLUS4,       MINUS4,      MINUS4,      MINUS4     },
   {NOINFLUENCE, NOINFLUENCE, NOINFLUENCE, MINUS1     },
   {MINUS3     , PLUS3      , PLUS3      , NOINFLUENCE},
   {NOINFLUENCE, PLUS3      , PLUS3      , NOINFLUENCE},
   {NOINFLUENCE, NOINFLUENCE, NOINFLUENCE, NOINFLUENCE},

   {NOINFLUENCE, NOINFLUENCE, NOINFLUENCE, NOINFLUENCE},
   {NOINFLUENCE, MINUS4     , MINUS4     , MINUS4     },
   {NOINFLUENCE, NOINFLUENCE, NOINFLUENCE, NOINFLUENCE},
   {NOINFLUENCE, NOINFLUENCE, NOINFLUENCE, NOINFLUENCE},
   {NOINFLUENCE, NOINFLUENCE, NOINFLUENCE, NOINFLUENCE},

   {NOINFLUENCE, NOINFLUENCE, NOINFLUENCE, NOINFLUENCE},
   {NOINFLUENCE, NOINFLUENCE, NOINFLUENCE, NOINFLUENCE},
   {NOINFLUENCE, NOINFLUENCE, MINUS4     , NOINFLUENCE},
   {PLUS4      , MINUS4     , MINUS4     , MINUS4     },
   {NOINFLUENCE, NOINFLUENCE, NOINFLUENCE, NOINFLUENCE},

   {NOINFLUENCE, NOINFLUENCE, NOINFLUENCE, NOINFLUENCE},
   {NOINFLUENCE, NOINFLUENCE, NOINFLUENCE, NOINFLUENCE},
   {NOINFLUENCE, NOINFLUENCE, NOINFLUENCE, NOINFLUENCE},
   {NOINFLUENCE, NOINFLUENCE, NOINFLUENCE, NOINFLUENCE},
   {PLUS4      , MINUS4     , MINUS4     , MINUS4     },
   {PLUS4,       MINUS4,      MINUS4,      MINUS4     },
   {NOINFLUENCE, NOINFLUENCE, NOINFLUENCE, NOINFLUENCE},
   {NOINFLUENCE, NOINFLUENCE, NOINFLUENCE, NOINFLUENCE}
};


static int weather_conversion_table[SECT_MAX][SKY_MAX] =
{
   {0, 1, 2, 3},
   {0, 1, 2, 3},
   {0, 1, 2, 3},
   {0, 1, 2, 3},
   {0, 1, 2, 3},

   {0, 1, 2, 3},
   {0, 1, 2, 3},
   {0, 1, 2, 3},
   {0, 1, 2, 3},
   {0, 1, 2, 3},

   {0, 0, 0, 3},
   {0, 1, 2, 3},
   {0, 1, 2, 3},
   {0, 1, 2, 3},
   {0, 1, 2, 3},

   {0, 1, 2, 3},
   {0, 1, 2, 3},
   {0, 1, 2, 3},
   {0, 1, 2, 3},
   {0, 1, 2, 3},

   {0, 1, 2, 3},
   {0, 1, 2, 3},
   {0, 1, 2, 3},
   {0, 1, 2, 3},
   {0, 1, 2, 3},

   {0, 1, 2, 3},
   {0, 1, 2, 3},
   {0, 1, 2, 3},
   {0, 1, 2, 3},
   {0, 1, 2, 3},

   {0, 1, 2, 3},
   {0, 0, 0, 3},
   {0, 1, 2, 3},
   {0, 1, 2, 3}
};


static int master_weather_table[SECT_MAX][SEASON_MAX][SKY_MAX];


static char* weather_strings[11][8] =
{
   {
   /* SECT_INSIDE */
      /* cloudless */
      "",
      "",
      /* cloudy */
      "",
      "",
      /* raining */
      "",
      "",
      /* lightning */
      "",
      ""
   },
   {
/* SECT_CITY */
      /* cloudless */
      "Nad miastem niebo staje si� ca�kiem bezchmurne.\n\r",
      "",
      /* cloudy */
      "Odg�osy deszczu b�bni�cego o dachy i rynny milkn� powoli.\n\r",
      "Ponad szczytami dach�w widzisz powoli sun�ce k��by chmur.\n\r",
      /* raining */
      "Piorun przetacza si� po niebie, a grzmot odbija si� g�o�nym echem od �cian s�siednich budynk�w.\n\r",
      "Deszcz wygrywa sw� monotonn� melodi� na szczytach dach�w.\n\r",
      /* lightning */
      "",
      "Niebo grzmi, a pioruny raz po raz rozcinaj� niebo siej�c z�owieszcze b�yski na dachach.\n\r"
   },
   {
/* SECT_FIELD */
      /* cloudless */
      "Ponad r�wnina otwiera si� czyste niebo.\n\r",
      "",
      /* cloudy */
      "Deszcz przestaje pada�.\n\r",
      "R�wnina ciemnieje, gdy na niebo wtaczaj� si� chmury, przes�aniaj�c je ca�kowicie.\n\r",
      /* raining */
      "Odg�osy piorun�w milkn� za horyzontem.\n\r",
      "Zaczyna pada�, powoli tworz� si� ka�u�e.\n\r",
      /* lightning */
      "",
      "Na otwartej przestrzeni pioruny tworz� si� na kszta�t gigantycznych drzew, czas si� schowa�.\n\r"
   },
   {
/* SECT_FOREST */
      /* cloudless */
      "Czyste niebo staje si� widoczne poprzez korony drzew, a puszcza znowu zaczyna t�tni� �yciem.\n\r",
      "",
      /* cloudy */
      "Wszechobecny odg�os kapi�cej z li�ci wody milknie powoli.\n\r",
      "Puszcza cichnie, gdy niebo przykrywaj� chmury.\n\r",
      /* raining */
      "Grzmoty wstrz�saj�ce puszcz� milkn� w oddali.\n\r",
      "Krople deszczu powoli przedzieraj� si� przez listowie.\n\r",
      /* lightning */
      "",
      "Niebo ponad koronami drzew przecinaj� potworne b�yski!\n\r"
   },
   {
/* SECT_HILLS */
      /* cloudless */
      "Szczyty pag�rk�w powoli wy�aniaj� si� zza chmur.\n\r",
      "",
      /* cloudy */
      "Przestaje pada�.\n\r",
      "Robi si� jakby ciszej, na horyzoncie wida� ci�kie, burzowe chmury.\n\r",
      /* raining */
      "Echa piorun�w z okolicznych pag�rk�w milkn�.\n\r",
      "Gleba chciwie spija krople rz�sistego deszczu.\n\r",
      /* lightning */
      "",
      "B�ysk roz�wietla niebo, grzmot przetacza si� pomi�dzy wzg�rzami.\n\r"
   },
   {
/* SECT_MOUNTAIN */
      /* cloudless */
      "Szczyty g�r majestatycznie wznosz� si� na tle jasnego nieba.\n\r",
      "",
      /* cloudy */
      "Chmury bielej� w s�o�cu i deszcz powoli milknie.\n\r",
      "Ci�kie, ciemne chmury otaczaj� szczyty.\n\r",
      /* raining */
      "Odg�os pioruna odbija si� echem w dolinie, wraca i milknie...\n\r",
      "Deszcz niemi�osiernie przenika twe ubranie i po chwili ca�e twoje ubranie sp�ywa wod�.\n\r",
      /* lightning */
      "",
      "Grzmot pioruna pot�nym echem obiega s�siednie wzg�rza.\n\r"
   },
   {
/* SECT_WATER_SWIM */
      /* cloudless */
      "Wygl�da na to, �e niebo si� przeja�nia.\n\r",
      "",
      /* cloudy */
      "Wszechobecny ha�as deszczu bij�cego o wod� cichnie z wolna.\n\r",
      "Woda woko�o ciemnieje i chmury zbieraj� si� ponad tob�.\n\r",
      /* raining */
      "Grzmoty piorun�w w oddali milkn�.\n\r",
      "Deszcz z w�ciek�o�ci� zaczyna m��ci� w�d� i co� na kszta�t piany unosi si� na powierzchni.\n\r",
      /* lightning */
      "",
      "Niebo ponad tob� grzmi, a ciemn� powierzchnie wody roz�wietlaj� b�yski.\n\r"
   },
   {
/* SECT_WATER_NOSWIM */
      /* cloudless */
      "Niebo ponad tob� rozja�nia si� z wolna.\n\r",
      "",
      /* cloudy */
      "Wszechobecny ha�as deszczu bij�cego o wod� cichnie z wolna.\n\r",
      "Woda woko�o ciemnieje i chmury zbieraj� si� nad tob�.\n\r",
      /* raining */
      "Grzmoty piorun�w w oddali milkn�.\n\r",
      "Deszcz z w�ciek�o�ci� zaczyna m��ci� w�d� i co� na kszta�t piany unosi si� na powierzchni.\n\r",
      /* lightning */
      "",
      "Niebo ponad tob� grzmi, a ciemn� powierzchnie wody roz�wietlaj� b�yski.\n\r"
   },
   {
/* SECT_TUNDRA */
      /* cloudless */
      "Chmury rzedn� i s�once o�lepiaj�co odbija si� od �niegu.\n\r",
      "",
      /* cloudy */
      "Cichy szmer opadaj�cych p�atk�w �niegu milknie... Cisza absolutna.\n\r",
      "Pot�na chmura przes�ania niebo, tak �e zdaje si� ono ni�sze i bardziej nieprzyjazne.\n\r",
      /* raining */
      "W ko�cu zawieja ustaje i widoczno�� poprawia si� nieco.\n\r",
      "Z pocz�tku drobne potem coraz wi�ksze, mi�kkie i puszyste p�atki �niegu padaj� na ziemie.\n\r",
      /* lightning */
      "",
      "Brutalny wiatr ze �wistem unosi zal�gaj�cy gdzieniegdzie �nieg. Robi si� coraz zimniej.\n\r"
   },
   {
/* SECT_AIR */
      /* cloudless */
      "Chmury wok� ciebie rozp�ywaj� si�.\n\r",
      "",
      /* cloudy */
      "Deszcz wok� ciebie powoli s�abnie, by ca�kiem znikn��...\n\r",
      "Chmury zaczynaj� z wolna wypi�trza� si� wok� ciebie.\n\r",
      /* raining */
      "B�yski piorun�w gasn�, huk cichnie... niebezpiecze�stwo oddala si�.\n\r",
      "Widzisz jak wok� ciebie w ch�odnym powietrzu krystalizuje woda i zaczyna opada� w d�.\n\r",
      /* lightning */
      "",
      "Przechodz� ci� dreszcze, gdy obok rysuje si� wst�ga pioruna, powietrze przechodzi zapachem ozonu!\n\r"
   },
   {
/* SECT_DESERT */
      /* cloudless */
      "Okrutne, rozpalone niebo rozpo�ciera si� nad tob�.\n\r",
      "",
      /* cloudy */
      "S�once zaczyna pali� niemi�osiernie.\n\r",
      "Niestety nie ma w pobli�u ani jednej chmurki.\n\r",
      /* raining */
      "",
      "",
      /* lightning */
      "",
      "Piorun przetacza si� po niebie z potwornym hukiem, wzniecaj�c piaskowa burz�.\n\r"
   }
};


int calc_season( void )
{
	int season = 0;

	int day = time_info.month * DAYS_PER_MONTH + time_info.day;

	if ( day < ( DAYS_PER_YEAR / 8 ) )
	{
		season = SEASON_WINTER;
	}
	else if ( day < ( DAYS_PER_YEAR / 8 ) * 3 )
	{
		season = SEASON_SPRING;
	}
	else if ( day < ( DAYS_PER_YEAR / 8 ) * 5 )
	{
		season = SEASON_SUMMER;
	}
	else if ( day < ( DAYS_PER_YEAR / 8 ) * 7 )
	{
		season = SEASON_FALL;
	}
	else
	{
		season = SEASON_WINTER;
	}

	return season;
}

void update_daylight( char buf[ SECT_MAX ][ MAX_STRING_LENGTH ] )
{
	int sect;
	++time_info.hour;

	for ( sect = 0; sect < SECT_MAX; sect++ )
	{
		if ( IS_SET( sector_table[ sect ].flag, SECT_NOWEATHER ) )
			continue;

		switch ( time_info.hour )
		{
			case HOUR_DAY_BEGIN:
				weather_info[ sect ].sunlight = SUN_LIGHT;
				switch ( number_range( 1, 3 ) )
				{
					case ( 1 ) :
                        strcat( buf[ sect ], "Rozpocz�� si� dzie�, a� chce si� �y�!\n\r" );
						break;

					case ( 2 ) :
                        strcat( buf[ sect ], "Czas powita� nowy dzie�, kt�ry w�a�nie nadszed�!\n\r" );
						break;

					case ( 3 ) :
                        strcat( buf[ sect ], "Robi si� troszeczk� ja�niej i nadchodzi nowy dzie�!\n\r" );
						break;
				}
				break;
			case HOUR_SUNRISE:
				weather_info[ sect ].sunlight = SUN_RISE;
				switch ( number_range( 1, 3 ) )
                {
					case ( 1 ) :
                        strcat( buf[ sect ], "S�o�ce z wolna wy�ania si� zza widnokr�gu na wschodzie.\n\r" );
						break;

					case ( 2 ) :
                        strcat( buf[ sect ], "Pierwsze promienie s�o�ca omywaj� ci twarz.\n\r" );
						break;

					case ( 3 ) :
                        strcat( buf[ sect ], "S�o�ce wschodzi i rozja�nia nieco panuj�cy wcze�niej p�mrok.\n\r" );
						break;
				}
				break;
			case HOUR_SUNSET:
				weather_info[ sect ].sunlight = SUN_SET;
				switch ( number_range( 1, 4 ) )
                {
					case ( 1 ) :
                        strcat( buf[ sect ], "S�o�ce powoli zachodzi, roz�wietlaj�c wszystko z�otym blaskiem.\n\r" );
						break;
					case ( 2 ) :
                        strcat( buf[ sect ], "S�o�ce powoli znika za horyzontem ob�askawiaj�c ci� ostatnimi promieniami.\n\r" );
						break;
					case ( 3 ) :
                        strcat( buf[ sect ], "S�o�ce powoli zachodzi za horyzont �wiec�c coraz s�abiej.\n\r" );
						break;
					case ( 4 ) :
                        strcat( buf[ sect ], "S�o�ce zachodzi barwi�c ca�y �wiat na g��bok� czerwie�.\n\r" );
						break;
				}
				break;

			case HOUR_NIGHT_BEGIN:
				weather_info[ sect ].sunlight = SUN_DARK;
				switch ( number_range( 1, 4 ) )
                {
					case ( 1 ) :
                        strcat( buf[ sect ], "Zapad�a noc.\n\r" );
						break;
					case ( 2 ) :
                        strcat( buf[ sect ], "Zapada noc pogr��aj�c wszystko w mroku.\n\r" );
						break;
					case ( 3 ) :
                        strcat( buf[ sect ], "Nadchodzi noc.\n\r" );
						break;
					case ( 4 ) :
                        strcat( buf[ sect ], "W zapadaj�cej ciemno�ci s�ycha� typowo nocne odg�osy.\n\r" );
						break;
				}
				break;
			default:
				if ( time_info.hour > HOUR_DAY_BEGIN &&
				     time_info.hour < HOUR_SUNSET )
					weather_info[ sect ].sunlight = SUN_LIGHT;

				else if ( time_info.hour < HOUR_SUNRISE ||
                    time_info.hour > HOUR_NIGHT_BEGIN )
					weather_info[ sect ].sunlight = SUN_DARK;
		}
	}

	if ( time_info.hour == HOUR_MIDNIGHT )
    {
		time_info.hour = 0;
		time_info.day++;
	}

	if ( time_info.day >= DAYS_PER_MONTH )
	{
		time_info.day = 0;
		time_info.month++;
	}

	if ( time_info.month >= MONTHS_PER_YEAR )
	{
		time_info.month = 0;
		time_info.year++;
	}

    if ( time_info.moon_phase_passed >= MOON_PHASE_DURATION )
	{
		time_info.moon_phase_passed = 0;
        if ( ++time_info.moon_phase >= 8 )
            time_info.moon_phase = 0;
	}
}

void weather_update( bool update_time )
{
	char buf[ SECT_MAX ][ MAX_STRING_LENGTH ];
	DESCRIPTOR_DATA *d, *d_next;
	MURDER_LIST *tmp_death;
	int season;
	int diff;
	int sect;
	int sky;
	int i;
	int changed = 0;
	int dir = 0;
	int vnum = 0;

	for ( i = 0; i < SECT_MAX; i++ )
	{
		buf[ i ][ 0 ] = '\0';
	}

	/* Day/Night */
	if ( update_time )
		update_daylight( buf );

	season = calc_season();

	for ( sect = 0; sect < SECT_MAX; sect++ )
	{
		changed = 0;

		if ( IS_SET( sector_table[ sect ].flag, SECT_NOWEATHER ) )
			continue;

		sky = weather_info[ sect ].sky;

		weather_info[ sect ].mmhg += weather_info[ sect ].change;


#define CHANGE_FACTOR ((random() % 25) + 15)

		if ( weather_info[ sect ].sky == 0 )
		{
			if ( weather_info[ sect ].mmhg > master_weather_table[ sect ][ season ][ weather_info[ sect ].sky ] )
			{
				sky = weather_conversion_table[ sect ][ weather_info[ sect ].sky + 1 ];
				changed = 1;
				dir = 0;
			}
		}

		else if ( weather_info[ sect ].sky == SKY_MAX - 1 )
		{
			if ( weather_info[ sect ].mmhg < master_weather_table[ sect ][ season ][ weather_info[ sect ].sky - 1 ] )
			{
				sky = weather_conversion_table[ sect ][ weather_info[ sect ].sky - 1 ];
				changed = 1;
				dir = -1;
			}
		}

		else
		{
			if ( weather_info[ sect ].mmhg > master_weather_table[ sect ][ season ][ weather_info[ sect ].sky + 1 ] )
			{
				sky = weather_conversion_table[ sect ][ weather_info[ sect ].sky + 1 ];
				changed = 1;
			}
			else if ( weather_info[ sect ].mmhg < master_weather_table[ sect ][ season ][ weather_info[ sect ].sky - 1 ] )
			{
				sky = weather_conversion_table[ sect ][ weather_info[ sect ].sky - 1 ];
				changed = 1;
				dir = -1;
			}
		}

		/* Sky changed, reset counters */
		if ( changed )
		{
			/* Figure how much to change the weather */
			if ( sky == 0 )
			{
				/* Is always going to go up */
				weather_info[ sect ].change = CHANGE_FACTOR;

				/* reset mmhg in the middle of the current sky factor */
				/*weather_info[sect].mmhg = master_weather_table[sect][season][sky]
				/ 2;*/
				/* PaB: Cheat for a little more sunshine. ;-) */
				weather_info[ sect ].mmhg = 0;
			}
			else if ( sky == SKY_MAX - 1 )  /* Right now, SKY_LIGHTNING */
			{
				/* Is always going to come back down */
				weather_info[ sect ].change = -1 * CHANGE_FACTOR;

				/* reset mmhg in the middle of the current sky factor */
				weather_info[ sect ].mmhg = ( master_weather_table[ sect ][ season ][ sky ] -
				                              master_weather_table[ sect ][ season ][ sky - 1 ] ) / 2 +
				                            master_weather_table[ sect ][ season ][ sky ];
			}
			else
			{
				/* Might go up or down */
				diff = ( ( random() % 100 ) < 50 ) ? -1 : 1;
				weather_info[ sect ].change = diff * CHANGE_FACTOR;

				/* reset mmhg in the middle of the current sky factor */
				weather_info[ sect ].mmhg = ( master_weather_table[ sect ][ season ][ sky ] -
				                              master_weather_table[ sect ][ season ][ sky - 1 ] ) / 2 +
				                            master_weather_table[ sect ][ season ][ sky + dir ];
			}
		}

		/* Generate a proper change message */
		if ( changed )
		{
			if ( sky < weather_info[ sect ].sky )
			{
				if ( sect > 10 )
					strcat( buf[ sect ], "" );
				else
					strcat( buf[ sect ], weather_strings[ sect ][ sky * 2 ] );
			}
			else
			{
				if ( sect > 10 )
					strcat( buf[ sect ], "" );
				else
					strcat( buf[ sect ], weather_strings[ sect ][ sky * 2 + 1 ] );
			}
			weather_info[ sect ].sky = sky;
		}

	}

	for ( d = descriptor_list; d != NULL; d = d->next )
	{
		if ( !d->character || d->connected < 0 ) continue;
		if ( d->connected == CON_PLAYING &&
		     IS_OUTSIDE( d->character ) &&
		     IS_AWAKE( d->character ) &&
		     d->character &&
		     d->character->in_room &&
		     !EXT_IS_SET( d->character->in_room->room_flags, ROOM_NO_WEATHER ) &&
		     !IS_SET( sector_table[ d->character->in_room->sector_type ].flag, SECT_NOWEATHER ) &&
		     buf[ d->character->in_room->sector_type ][ 0 ] != '\0' &&
		     !EXT_IS_SET( d->character->in_room->room_flags, ROOM_MAGICDARK ) )
		{
			send_to_char( buf[ d->character->in_room->sector_type ], d->character );
		}
	}

	/* See if Divine Intervention is really going to piss on someone's
	 * parade.
	 */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
		d_next = d->next;
		if ( !d->character  || d->connected < 0 ) continue;

		if ( d->connected == CON_PLAYING &&
		     IS_OUTSIDE( d->character ) &&
		     d->character &&
		     d->character->in_room	&&
		     !IS_SET( sector_table[ d->character->in_room->sector_type ].flag, SECT_NOWEATHER ) )
		{
			if ( weather_info[ d->character->in_room->sector_type ].sky == SKY_LIGHTNING )
			{
				if ( number_percent() == 1 && number_percent() == 1 && number_percent() == 1 )
				{
					int sn = skill_lookup( "lightning" );
					vnum = d->character->in_room ? d->character->in_room->vnum : 0;
					send_to_char( "{YZZAAAAAAAPP! {WW�a�nie trafi� ci� piorun.{x\n\r", d->character );
					damage( d->character, d->character, number_range( 250, 400 ), sn, DAM_LIGHTNING, FALSE );
					shock_effect( d->character, 100, 50, TARGET_CHAR );

					if ( !d->character->in_room )
					{
						tmp_death = malloc( sizeof( *tmp_death ) );
						tmp_death->name = str_dup( "PIORUN" );
						tmp_death->char_level = d->character->level;
						tmp_death->victim_level = 0;
						tmp_death->room = vnum;
						tmp_death->time = current_time;
						tmp_death->next = d->character->pcdata->death_statistics.pkdeath_list;
						d->character->pcdata->death_statistics.pkdeath_list = tmp_death;
					}
				}
			}
		}
	}

	return ;
}

void init_weather( void )
{
	int sect, seas;

	for ( sect = 0; sect < SECT_MAX; sect++ )
	{
		if ( IS_SET( sector_table[ sect ].flag, SECT_NOWEATHER ) )
			continue;

		weather_info[ sect ].change = CHANGE_FACTOR;
		weather_info[ sect ].mmhg = 200;
		weather_info[ sect ].sky = SKY_CLOUDLESS;

		for ( seas = 0; seas < SEASON_MAX; seas++ )
		{
			int cur = 0;

			if ( IS_SET( sector_table[ sect ].flag, SECT_NOWEATHER ) )
				continue;

			/* Each value is an accumulation of the preceeding */
			cur += UMAX( base_weather.clear_sky +
			             seasonal_weather_table[ seas ].clear_sky +
			             sector_weather_table[ sect ].clear_sky, 0 );
			master_weather_table[ sect ][ seas ][ SKY_CLOUDLESS ] = cur;

			cur += UMAX( base_weather.cloudy_sky +
			             seasonal_weather_table[ seas ].cloudy_sky +
			             sector_weather_table[ sect ].cloudy_sky, 0 );
			master_weather_table[ sect ][ seas ][ SKY_CLOUDY ] = cur;

			cur += UMAX( base_weather.rainy_sky +
			             seasonal_weather_table[ seas ].rainy_sky +
			             sector_weather_table[ sect ].rainy_sky, 0 );
			master_weather_table[ sect ][ seas ][ SKY_RAINING ] = cur;

			cur += UMAX( base_weather.lightning_sky +
			             seasonal_weather_table[ seas ].lightning_sky +
			             sector_weather_table[ sect ].lightning_sky, 0 );
			master_weather_table[ sect ][ seas ][ SKY_LIGHTNING ] = cur;
		}
	}
	weather_update( TRUE );
}
