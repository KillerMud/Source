
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
 * KILLER MUD is copyright 1999-2013 Killer MUD Staff (alphabetical)   *
 *                                                                     *
 * Andrzejczak Dominik   (kainti@go2.pl                 ) [Kainti    ] *
 * Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor     ] *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: act_move.c 155 2013-08-08 20:50:23Z drake $
 * $HeadURL: svn+ssh://svn@killer.mud.pl/killer/branches/current/src/act_move.c $
 *
 */
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
#include "interp.h"
#include "traps.h"
#include "money.h"
#include "track.h"
#include "mount.h"

char *	const	eng_dir_name	[]		=
{
	"north", "east", "south", "west", "up", "down"
};

char *	const	dir_name	[]		=
{
	"p�noc", "wsch�d", "po�udnie", "zach�d", "g�ra", "d�"
};

char *	const	dir_name2	[]		=
{
	"z p�nocy", "ze wschodu", "z po�udnia", "z zachodu", "z g�ry", "z do�u"
};

char *	const	dir_name_dir	[]		=
{
	"na p�noc", "na wsch�d", "na po�udnie", "na zach�d", "na g�r�", "w d�"
};

const	sh_int	rev_dir		[]		=
{
	2, 3, 0, 1, 5, 4
};

// w zaleznosci do czasu wyswietla sie odpowiedni napis
char * const move_name		[]		=
{
	"przybiega","przybiega truchtem","wkracza marszowym tempem","przychodzi","przychodzi spokojnym krokiem"
};


/*
 * Local functions.
 */
int	find_door	args( ( CHAR_DATA *ch, char *arg ) );
OBJ_DATA* has_key		args( ( CHAR_DATA *ch, int key ) );
void send_to_char_fail_open_lock(CHAR_DATA *ch);

int get_door( CHAR_DATA *ch, char* argument )
{
	if ( !str_cmp( argument, "n" )
			|| ( !EXT_IS_SET(ch->act, PLR_COMMANDSPL) && !str_prefix( argument, "north" ) )
			|| (  EXT_IS_SET(ch->act, PLR_COMMANDSPL) && !str_prefix( argument, "polnoc" ) ) ) return 0;

	if ( !str_cmp( argument, "e" )
			|| ( !EXT_IS_SET(ch->act, PLR_COMMANDSPL) && !str_prefix( argument, "east" ) )
			|| (  EXT_IS_SET(ch->act, PLR_COMMANDSPL) && !str_prefix( argument, "wschod" ) ) ) return 1;

	if ( !str_cmp( argument, "s" )
			|| ( !EXT_IS_SET(ch->act, PLR_COMMANDSPL) && !str_prefix( argument, "south" ) )
			|| (  EXT_IS_SET(ch->act, PLR_COMMANDSPL) && !str_prefix( argument, "poludnie" ) ) ) return 2;

	if ( !str_cmp( argument, "w" )
			|| ( !EXT_IS_SET(ch->act, PLR_COMMANDSPL) && !str_prefix( argument, "west" ) )
			|| (  EXT_IS_SET(ch->act, PLR_COMMANDSPL) && !str_prefix( argument, "zachod" ) ) ) return 3;

	if ( !str_cmp( argument, "u" )
			|| ( !EXT_IS_SET(ch->act, PLR_COMMANDSPL) && !str_prefix( argument, "up" ) )
			|| (  EXT_IS_SET(ch->act, PLR_COMMANDSPL) && !str_prefix( argument, "gora" ) ) ) return 4;

	if ( !str_cmp( argument, "d" )
			|| ( !EXT_IS_SET(ch->act, PLR_COMMANDSPL) && !str_prefix( argument, "down" ) )
			|| (  EXT_IS_SET(ch->act, PLR_COMMANDSPL) && !str_prefix( argument, "dol" ) ) ) return 5;

	return -1;
}
/*
void turn_into_dust_objects_sensitive_to_light( CHAR_DATA *ch, int dmg )
{
    OBJ_DATA * obj, *obj_next = NULL, *dust;
	AFFECT_DATA aff, *pAff;
    char buf[ MAX_STRING_LENGTH ];
    
     // w ciemny pokoju nie
    
    if (
            ch == NULL
            || ch->in_room == NULL
            || EXT_IS_SET( ch->in_room->room_flags, ROOM_DARK )
            || EXT_IS_SET( ch->in_room->room_flags, ROOM_MAGICDARK )
       )
    {
        return;
    }
    
     // tam gdzie nie ma pogody (wewnatrz) tez nie
   
    if ( IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_NOWEATHER )  )
    {
        return;
    }
    
     // no i w nocy tez nie
    
    if ( time_info.hour < HOUR_SUNRISE || time_info.hour > HOUR_SUNSET)
    {
        return;
    }
    
     // lec po wszystkim co nosi
     
    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
        OBJ_NEXT_CONTENT( obj, obj_next );
        
         // adamantytowe kasujemy
         
		if ( obj->material == 7 && !IS_OBJ_STAT( obj, ITEM_DARK )) // adamantyt, niechroniony czarem darkness
        {
            //raszer - co sie dzieje z przedmiotami adamantytowymi gdy dzialaja na nie promienie sloneczne
            //jesli juz nie sa rozgrzane, to rozgrzwaja sie
            if ( !( pAff = affect_find( obj->affected, gsn_heat_metal ) ) )
            {
             	if ( ( pAff = affect_find( obj->affected, gsn_chill_metal ) ) )
                {
                    act( "Szron pokrywajacy $p znika.", ch, obj, NULL, TO_CHAR );
                    affect_remove_obj( obj, pAff );
                    return;
                }

                aff.where	= TO_OBJECT;
                aff.type	= gsn_heat_metal;
                aff.level	= 30;
                aff.duration = 6; aff.rt_duration = 0;
                aff.location	= APPLY_NONE;
                aff.modifier	= 0;
                aff.bitvector	= &AFF_NONE;

                affect_to_obj( obj, &aff );
                if ( obj->liczba_mnoga )
                {
                    act( "$p rozgrzewaj� si� do czerwono�ci.", ch, obj, NULL, TO_ALL );
                }
                else
                {
                    act( "$p rozgrzewa si� do czerwono�ci.", ch, obj, NULL, TO_ALL );
                }

            }
            // przedmioty, ktore nie sa niezniszczalne - psuja sie
            if ( !IS_OBJ_STAT( obj, ITEM_UNDESTRUCTABLE ) ) {
                obj->condition -= dmg;
                //jesli sa w zlej kondycji, to slonce wygrywa
                if (obj->condition < 25) {
                    if ( obj->liczba_mnoga )
                    {
                        act( "$p rozb�yskuj� jasnym �wiat�em i zamienj� si� w kupk� popio�u.", ch, obj, NULL, TO_ALL );
                    }
                    else
                    {
                        act( "$p rozb�yskuje jasnym �wiat�em i zamienia si� w kupk� popio�u.", ch, obj, NULL, TO_ALL );
                    }
                    if ( is_artefact( obj ) )
                    {
                        extract_artefact( obj );
                    }
                    extract_obj( obj );
                    dust = create_object( get_obj_index( OBJ_VNUM_ASH ), FALSE );
                    obj_to_char( dust, ch );
                    sprintf(
                    buf,
                    "[%d][adamantyt] %s (%d) loss adamantium item: %s (%d).",
                    ch->in_room ? ch->in_room->vnum : 0,
                    ch->name,
                    ch->level,
                    strip_colour_codes( obj->short_descr ),
                    obj->pIndexData->vnum
                    );
                    log_string( buf );
                }
            }

        }
        
    }
	return;
}*/

void move_char( CHAR_DATA *ch, int door, bool follow, OBJ_DATA *dragged )
{
	CHAR_DATA * fch, *sleeping_char, *fch_next;
	ROOM_INDEX_DATA *in_room;
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;
	AFFECT_DATA *af;
	int dir, counter = 0;
	int ex_dir[ MAX_DIR ];
	char buf[ MAX_STRING_LENGTH ];
	char direction [ MAX_STRING_LENGTH ];
	int wait_mod = 0;
	bool rand, check;
	int zwiadowca_skill, iWear, skill;
	OBJ_DATA *obj;

	// tutaj dajemy zaburzenie w orientacji wynik�e z czaru maze
	if ( IS_AFFECTED(ch, AFF_MAZE))
	{
		door = number_range( 0, 5 );
	}

	// sprawdzamy czy kierunek istnieje
	if ( door < 0 || door > 5 )
	{
		bug( "Do_move: bad door %d.", door );
		return;
	}

	//jesli gosc lata swoim umyslem
	if ( !IS_NPC( ch ) && ch->pcdata->mind_in )
	{
		in_room = ch->pcdata->mind_in;

		DEBUG_INFO( "move_mind(1):exit_check");
		if ( ( pexit = in_room->exit[ door ] ) == NULL
				|| ( to_room = pexit->u1.to_room ) == NULL )
		{
			send_to_char( "Nic nie ma w tamtym kierunku.\n\r", ch );
			return;
		}

		if ( IS_SET( pexit->exit_info, EX_NOPASS ) )
		{
			send_to_char( "Nie mo�esz si�gn�� tam swoim umys�em.\n\r", ch );
			return;
		}

		if ( mind_move_room( ch, to_room ) )
			do_function( ch, &do_look, "auto" );
		return;
	}

	// jak si� nie mo�e rusza� to nie ruszamy
	if ( !can_move( ch ) )
		return;

	if ( is_affected( ch, gsn_undead_resemblance ))
		affect_strip( ch, gsn_undead_resemblance );

	/* przeciazyl sie koles */
	if ( !ch->mount && get_carry_weight( ch ) > can_carry_w( ch ) )
	{
		if(IS_AFFECTED(ch, AFF_FLYING) || IS_AFFECTED(ch, AFF_FLOAT ))
		{
			send_to_char( "Pod wp�ywem obci��enia opadasz na ziemi�.\n\r", ch );
			affect_strip(ch, gsn_fly);
			affect_strip(ch, gsn_float);
		}
		send_to_char( "Nogi si� pod tob� uginaj�, nie dasz rady nigdzie i��.\n\r", ch );
		return;
	}

	if ( ch->mount )
	{
		if ( IS_AFFECTED( ch->mount, AFF_PARALYZE ) ) {
			send_to_char( "Chyba tw�j wierzchowiec nie mo�e si� poruszy�.\n\r", ch );
			return;
		}

        if ( ch->mount->position < POS_STANDING ) {
            send_to_char( "Chyba tw�j wierzchowiec nie mo�e si� poruszy�.\n\r", ch );
            return;
        }

		if ( dragged )
		{
			send_to_char( "Najpierw zsi�d� ze swojego wierzchowca.\n\r", ch );
			return;
		}
	}

	//jak konik sie z kims napierdala
	if ( ch->mount && ch->mount->fighting )
	{
		act( "$N teraz walczy, nie mo�e ci� tam zaprowadzi�.", ch, NULL, ch->mount, TO_CHAR );
		return;
	}

    if ( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_SNEAK ) && get_skill(ch,gsn_sneak) > 0 )
    {
        // 2008-05-13: Gurthg: dajmy wi�cej szans z�odziejowi
        // i 1% wszystkim znajduj�cym si� pod wp�ywem
        skill = UMAX( 1, ( get_skill( ch, gsn_hide ) + get_skill ( ch, gsn_sneak ) ) / 2 );
        if ( dragged || number_percent() > skill )
        {
            EXT_REMOVE_BIT( ch->affected_by, AFF_SNEAK );
        }
    }
	
    if ( !IS_NPC( ch ) && IS_AFFECTED( ch, AFF_SNEAK ) && !dragged )
    {
        if( get_skill(ch,gsn_sneak) > 0 )
        {
            skill = get_skill ( ch, gsn_sneak );
            if ( number_percent() < skill )
            {
                EXT_SET_BIT( ch->affected_by, AFF_SNEAK_INVIS );
            }
        }
        else
        {
            EXT_SET_BIT( ch->affected_by, AFF_SNEAK_INVIS );  //sneak z przedmiotow
        }
    }

	in_room = ch->in_room;

	DEBUG_INFO( "move_char(1):exit_check");
	/* moze tam isc?*/
	if ( ( pexit = in_room->exit[ door ] ) == NULL
			|| ( to_room = pexit->u1.to_room ) == NULL
			|| !can_see_room( ch, pexit->u1.to_room ) )
	{
		if ( ch->mount )
        {
			send_to_char( "Tw�j wierzchowiec nie mo�e i�� w tym kierunku.\n\r", ch );
        }
		else
        {
			send_to_char( "Nie mo�esz i�� w tym kierunku.\n\r", ch );
        }
		return;
	}

	if ( ch->mount && ( IS_SET( pexit->exit_info, EX_NO_MOUNT ) || EXT_IS_SET( to_room->room_flags, ROOM_INDOORS ) ) )
	{
		send_to_char( "Tw�j wierzchowiec nie mo�e i�� w tym kierunku.\n\r", ch );
		return;
	}

	//nie mozna na mouncie do wody
	if ( ch->mount && IS_SET( sector_table[ ch->in_room->exit[ door ] ->u1.to_room->sector_type ].flag, SECT_UNDERWATER ) )
	{
		send_to_char( "Tw�j wierzchowiec nie mo�e i�� w tym kierunku.\n\r", ch );
		return;
	}

	DEBUG_INFO( "move_char(2):entangle");
	if( IS_AFFECTED(ch, AFF_ENTANGLE ) )
	{
		send_to_char( "Ro�liny opl�tuj� twe nogi, nie mo�esz si� ruszy�!\n\r", ch );
		return;
	}

	DEBUG_INFO( "move_char(2):web");
	if( IS_AFFECTED(ch, AFF_WEB ) && get_curr_stat_deprecated(ch,STAT_DEX) < number_range(10,25) && !IS_AFFECTED( ch, AFF_STABILITY ) )
	{
		send_to_char( "Pr�bujesz si� ruszy�, jednak oblepiaj�ca ci� magiczna paj�czyna sklei�a twoje nogi. Wywracasz si� na ziemi�.\n\r", ch );
		act( "$n pr�buje si� ruszy�, jednak oblepiaj�ce $s nogi magiczna paj�czyna zapl�tuje si� mi�dzy nimi, i $n wyraca si� na ziemi�.", ch, NULL, NULL, TO_ROOM );
		ch->position = POS_SITTING;
		WAIT_STATE( ch, PULSE_VIOLENCE );
		return;
	}

	DEBUG_INFO( "move_char(2):slippery_floor");
	if( EXT_IS_SET( ch->in_room->room_flags, ROOM_SLIPPERY_FLOOR ) && !IS_AFFECTED(ch, AFF_FLYING ) && !IS_AFFECTED( ch, AFF_FLOAT ) && !IS_AFFECTED( ch, AFF_STABILITY ) && number_percent() > URANGE( 10, ch->level + ((get_curr_stat_deprecated(ch, STAT_DEX)-10)*5), 90 ) )
	{
		send_to_char( "Ruszasz przed siebie, jednak przez rozlan� na pod�o�u t�ust� i �lisk� substancj� twoja noga po�lizgn�a si� i padasz na ziemi�.\n\r", ch );
		act( "$n rusza przed siebie, jednak przez rozlan� na pod�o�u t�ust� i �lisk� substancj� $s noga po�lizgn�a si� i pada $e na ziemi�.", ch, NULL, NULL, TO_ROOM );
		ch->position = POS_SITTING;
		WAIT_STATE( ch, PULSE_VIOLENCE );
		return;
	}

	DEBUG_INFO( "move_char(2):po_ciemku");
	if( room_is_dark(ch,in_room) && room_is_dark(ch,to_room) && !IS_AFFECTED( ch, AFF_STABILITY ) && !ch->mount && !IS_NPC( ch ) && !IS_IMMORTAL( ch ) )
	{
		switch( ch->in_room->sector_type )
		{
			case 2://pola, laki
			case 31:
			case 32:
			case 33:
			case 42:
				if( !IS_AFFECTED(ch,AFF_FLYING) && !IS_AFFECTED(ch,AFF_FLOAT))
				{
					if( number_percent() + number_percent()/number_range(1,2) < 30 - get_curr_stat_deprecated(ch,STAT_DEX))
					{
						if( number_range(1,2) == 1 )
						{
							act( "W ciemno�ciach nie uda�o ci si� zauwa�y� wystaj�cej k�pki traw, potykasz si� o ni� i wywracasz si� na ziemi�.", ch, NULL, NULL, TO_CHAR );
							act( "$n potyka si� o jak�� niewidoczn� po ciemku kepk� traw i pada na ziemi�.", ch, NULL, NULL, TO_ROOM );
						}
						else
						{
							act( "W ciemno�ciach nie uda�o ci si� zauwa�y� wystaj�cego krzaczka, o kt�ry zahaczy�a twoja noga. Wywracasz si� na ziemi�.", ch, NULL, NULL, TO_CHAR );
							act( "$n zahacza nog� jaki� niewidoczny po ciemku wystaj�cy krzaczek i pada na ziemi�.", ch, NULL, NULL, TO_ROOM );
						}
						damage( ch, ch, number_range(1,3), 0,DAM_BASH,FALSE);
						ch->position = POS_SITTING;
						WAIT_STATE( ch, PULSE_VIOLENCE );
						return;
					}
				}
				break;
			case 3://lasy
			case 11:
			case 12:
			case 38:
				if( !IS_AFFECTED(ch,AFF_FLYING) && !IS_AFFECTED(ch,AFF_FLOAT))
				{
					if( number_percent() < 30 - get_curr_stat_deprecated(ch,STAT_DEX))
					{
						if( number_range(1,2) == 1 )
						{
							act( "W ciemno�ciach nie uda�o ci si� zauwa�yc wyrastaj�cego przed tob� drzewa. Wpadasz nosem w sam �rodek szerokiego pnia, odbijasz si� od niego i wywracasz si�.", ch, NULL, NULL, TO_CHAR );
							act( "$n wpada w jakie� rosn�ce nieoopodal drzewo, po ciemku niewidoczne. Wali $e nosem w pie� i wywraca si�.", ch, NULL, NULL, TO_ROOM );
						}
						else
						{
							act( "W ciemno�ciach nie uda�o ci si� zauwa�y� wystaj�cego korzenia, o kt�ry zahaczy�a twoja noga. Wywracasz si� na ziemi�.", ch, NULL, NULL, TO_CHAR );
							act( "$n zahacza nog� jaki� niewidoczny po ciemku wystaj�cy korze� i pada na ziemi�.", ch, NULL, NULL, TO_ROOM );
						}
						damage( ch, ch, number_range(1,3), 0,DAM_BASH,FALSE);
						ch->position = POS_SITTING;
						WAIT_STATE( ch, PULSE_VIOLENCE );
						return;
					}
				}
				else
				{
					if( number_percent()/2 + number_percent()/2 < 30 - get_curr_stat_deprecated(ch,STAT_DEX))
					{
						act( "W ciemno�ciach nie uda�o ci si� zauwa�yc wyrastaj�cego przed tob� drzewa. Rozp�dzon<&y/a/e> uderzasz w sam �rodek szerokiego pnia, odbijasz si� od niego i spadasz na ziemi� j�cz�c w bole�ciach.", ch, NULL, NULL, TO_CHAR );
						act( "$n rozp�dzon$r uderza w jakie� rosn�ce nieoopodal drzewo, po ciemku niewidoczne. Odbija si� od pnia i spada bezw�adnie na ziemi�.", ch, NULL, NULL, TO_ROOM );
						affect_strip( ch, gsn_fly );
						affect_strip( ch, gsn_float );
						damage( ch, ch, number_range(10,25), 0,DAM_BASH,FALSE);
						ch->position = POS_SITTING;
						WAIT_STATE( ch, 2*PULSE_VIOLENCE );
						return;
					}
				}
				break;
			case 4://gory
			case 5:
			case 13:
			case 35:
			case 43: // stroma �cie�ka
				if( !IS_AFFECTED(ch,AFF_FLYING) && !IS_AFFECTED(ch,AFF_FLOAT))
				{
					if( number_percent() + number_percent()/number_range(1,2) < 30 - get_curr_stat_deprecated(ch,STAT_DEX))
					{
						if( number_range(1,2) == 1 )
						{
							act( "W ciemno�ciach nie uda�o ci si� zauwa�y� jakiej� sterty kamieni, potykasz si� o ni� i wywracasz si� na ziemi�.", ch, NULL, NULL, TO_CHAR );
							act( "$n potyka si� o jakies kamyki i pada na ziemi�.", ch, NULL, NULL, TO_ROOM );
						}
						else
						{
							act( "W ciemno�ciach �le stawiasz stop�, kt�ra niespodziewanie przeje�d�a na kamykach prawie metr do przodu, a ty sa<&m/ma/mo> l�dujesz plecach.", ch, NULL, NULL, TO_CHAR );
							act( "$n nieuwa�nie stawia stop�, kt�ra nagle przeje�d�a na kamykach prawie metr do przodu, a $e l�duje na plecach.", ch, NULL, NULL, TO_ROOM );
						}
						damage( ch, ch, number_range(2,6), 0,DAM_BASH,FALSE);
						ch->position = POS_SITTING;
						WAIT_STATE( ch, PULSE_VIOLENCE );
						return;
					}
				}
				break;
			case 6://takie co sie nie da wywrocic
			case 7://glownie jakies rzeki czy jeziora
			case 9:
			case 19:
			case 20:
			case 21:
			case 22:
			case 23:
			case 25:
			case 34:
			case 37:
			case 39:
				break;
			case 10://piachy
			case 24:
			case 29:
			case 40:
				if( !IS_AFFECTED(ch,AFF_FLYING) && !IS_AFFECTED(ch,AFF_FLOAT))
				{
					if( number_percent() + number_percent()/number_range(1,2) < 30 - get_curr_stat_deprecated(ch,STAT_DEX))
					{
						act( "W ciemno�ciach jedn� stop� zary�<&e�/a�/o�> w jak�� wy�sz� g�rk� piasku, i to tak niefortunnie, �e wywracasz si�.", ch, NULL, NULL, TO_CHAR );
						act( "$n jedn� stop� zarywa w niewidoczn� po ciemku g�rk� piasku, i to tak niefortunnie, �e wywraca si�.", ch, NULL, NULL, TO_ROOM );
						damage( ch, ch, number_range(1,2), 0,DAM_BASH,FALSE);
						ch->position = POS_SITTING;
						WAIT_STATE( ch, PULSE_VIOLENCE );
						return;
					}
				}
				break;
			case 16: //sniegi
			case 26:
			case 27:
				if( !IS_AFFECTED(ch,AFF_FLYING) && !IS_AFFECTED(ch,AFF_FLOAT))
				{
					if( 2*number_percent()/3 < 30 - get_curr_stat_deprecated(ch,STAT_DEX))
					{
						act( "W ciemno�ciach stawiasz nog� na czym� bardzo �liskim, i po chwili ju� le�ysz przysypan$r �niegiem.", ch, NULL, NULL, TO_CHAR );
						act( "$n stawia nog� na czym� bardzo �liskim, i po chwili ju� le�y przysypan$r �niegiem.", ch, NULL, NULL, TO_ROOM );
						damage( ch, ch, number_range(1,4), 0,DAM_COLD,FALSE);
						damage( ch, ch, number_range(1,4), 0,DAM_BASH,FALSE);
						ch->position = POS_SITTING;
						WAIT_STATE( ch, PULSE_VIOLENCE );
						return;
					}
				}
				break;
			case 44: // b�otna �cie�ka
				if( !IS_AFFECTED(ch,AFF_FLYING) && !IS_AFFECTED(ch,AFF_FLOAT))
				{
					if( number_percent() + number_percent()/number_range(1,2) < 30 - get_curr_stat_deprecated(ch,STAT_DEX))
					{
						act( "W ciemno�ciach wpad<&��e/e�a/e�o>� w ka�u��, wykonuj�c efektowny �lizg na plecy.", ch, NULL, NULL, TO_CHAR );
						act( "$n wpada w ka�u�e wykonuj�c efektowny �lizg na plecy.", ch, NULL, NULL, TO_ROOM );
						damage( ch, ch, number_range(1,2), 0,DAM_BASH,FALSE);
						ch->position = POS_SITTING;
						WAIT_STATE( ch, PULSE_VIOLENCE );
						return;
					}
				}
				break;
			default://pozostale, jakies jaskinie czy miasta
				if( !IS_AFFECTED(ch,AFF_FLYING) && !IS_AFFECTED(ch,AFF_FLOAT))
				{
					if( number_percent() + number_percent()/number_range(1,2) < 30 - get_curr_stat_deprecated(ch,STAT_DEX))
					{
						act( "W ciemno�ciach potykasz si� o co� i wywracasz na ziemi�.", ch, NULL, NULL, TO_CHAR );
						act( "$n potyka si� o co�, niewidocznego zupe�nie w tych ciemno�ciach, i wywraca si� na ziemi�.", ch, NULL, NULL, TO_ROOM );
						damage( ch, ch, number_range(1,2), 0,DAM_BASH,FALSE);
						ch->position = POS_SITTING;
						WAIT_STATE( ch, PULSE_VIOLENCE );
						return;
					}
				}
				break;
		}
	}

	DEBUG_INFO( "move_char(2):senny_pijak");
	/* random exits, drunk stuff and sleepy */
	if ( !IS_IMMORTAL( ch )
			&& !follow
			&&
			( IS_SET( pexit->exit_info, EX_RANDOM ) ||
			  ( !IS_NPC( ch ) && number_percent() > 75 && ( ( ch->condition[ COND_DRUNK ] > DRUNK_FULL && ch->class != CLASS_SHAMAN && ch->class != CLASS_BARBARIAN && ch->class != CLASS_WARRIOR && ch->class != CLASS_BLACK_KNIGHT ) || ch->condition[ COND_SLEEPY ] == 0 ) ) ) )
	{
		rand = (IS_SET( pexit->exit_info, EX_RANDOM ) > 0);

		/*najpierw czyscimy tablice wyjsc*/
		for ( dir = 0;dir < MAX_DIR;dir++ )
			ex_dir[ dir ] = -1;

		/*teraz lecimy po exitach i wpisujemy do ex_dir*/
		for ( dir = 0;dir < MAX_DIR;dir++ )
			if ( in_room->exit[ dir ] != NULL
					&& ( ( ch->mount && !IS_SET( in_room->exit[ dir ]->exit_info, EX_NO_MOUNT ) ) || !ch->mount )
					&& ( in_room->exit[ dir ] ->u1.to_room != NULL )
					&& can_see_room( ch, in_room->exit[ dir ] ->u1.to_room ) )
				ex_dir[ counter++ ] = dir;
			else
				continue;

		/*  dobra, teraz w ex_dir powinny byc po kolei wszystkie
		 *  mozliwe exity w ktore moze isc gracz... oczywiscie
		 *  to jest kiedy !follow czyli tylko dla tego co wpisal
		 *  kierunek a nie tego co idzie za kims
		 *  to moze byc troche zabugowane...
		 *  zostalo teraz tylko wylosowac liczbe z zakresu
		 *  0 do counter i isc w tym kierunku
		 */
		dir = door;
		door = ex_dir[ number_range( 0, counter - 1 ) ];

		if ( door == -1 )
			return ; /*czyli bug*/

		pexit = in_room->exit[ door ];
		to_room = pexit->u1.to_room;

		/*hmm podwojne zabezpieczenie... niech se bedzie*/
		if ( !pexit || !to_room )
			return;

		if ( !IS_NPC( ch ) && dir != door && !rand )
		{
			if ( ch->condition[ COND_SLEEPY ] == 0 )
				act( "Jeste� tak �pi�c$r, �e idziesz nie w tym kierunku co trzeba.", ch, NULL, NULL, TO_CHAR );
			else
				act( "Nagle zaczyna kr�ci� ci si� w g�owie i idziesz nie w tym kierunku co trzeba.", ch, NULL, NULL, TO_CHAR );
		}
	}

	DEBUG_INFO( "move_char(2):aff_loop");
	if( !IS_IMMORTAL( ch ) &&
			IS_AFFECTED(ch, AFF_LOOP ) &&
			dice(1,8) != 1 &&
			!follow )
	{
		//pierw czy wogole gdzies pojdzie
		if( ch->position != POS_FIGHTING && dice(1,4) != 1 )
		{
			if ( number_range(0,1) )
				send_to_char( "Ruszasz przed siebie, jednak po chwili orientujesz si�, �e wr�ci�<&e�/a�/o�> do punktu wyj�cia.\n\r", ch );
			else
				send_to_char( "Idziesz przed siebie, nagle przecierasz oczy, ku twemu zaskoczeniu jeste� w tym samym miejscu.\n\r", ch );

			act( "$n chodzi w k�ko.", ch, NULL, NULL, TO_ROOM );
			return;
		}

		//a tu w losowa droge, przeklejone z pijanstwa/sennosci
		for ( dir = 0;dir < MAX_DIR;dir++ )
        {
			ex_dir[ dir ] = -1;
        }

		for ( dir = 0;dir < MAX_DIR;dir++ )
        {
			if ( in_room->exit[ dir ] != NULL
					&& ( ( ch->mount && !IS_SET( in_room->exit[ dir ]->exit_info, EX_NO_MOUNT ) ) || !ch->mount )
					&& ( in_room->exit[ dir ] ->u1.to_room != NULL )
					&& can_see_room( ch, in_room->exit[ dir ] ->u1.to_room ) )
            {
				ex_dir[ counter++ ] = dir;
            }
			else
            {
				continue;
            }
        }

		door = ex_dir[ number_range( 0, counter - 1 ) ];

		if ( door == -1 )
			return ;

		pexit = in_room->exit[ door ];
		to_room = pexit->u1.to_room;

		if ( !pexit || !to_room )
			return;

	}

	DEBUG_INFO( "move_char:exit_triggery");
	/*
	 * trigery ->jesli sa to odpalamy i koniec
	 */
	if ( !IS_NPC( ch ) && mp_exit_trigger( ch, door ) )
		return;
	if ( !IS_NPC( ch ) && rp_exit_trigger( ch, door ) )
		return;

	DEBUG_INFO( "move_char(3):zamki_na_piasku");
	/* zamki drzwi etc...*/
	if ( IS_SET( pexit->exit_info, EX_CLOSED )
			&& ( !IS_AFFECTED( ch, AFF_PASS_DOOR ) || IS_SET( pexit->exit_info, EX_NOPASS ) )
			&& !IS_TRUSTED( ch, ANGEL ) )
	{
		if( !pexit->liczba_mnoga )
			act( "$d jest zamkni�ta.", ch, NULL, pexit->keyword, TO_CHAR );
		else
			act( "$d s� zamkni�te.", ch, NULL, pexit->keyword, TO_CHAR );
		return;
	}

	DEBUG_INFO( "move_char(4):ukryty_smok");
	/* hidden secret stuff */
	if ( IS_SET( pexit->exit_info, EX_HIDDEN ) && !IS_SET( pexit->exit_info, EX_SECRET ) )
	{
		if ( !IS_AFFECTED( ch, AFF_DETECT_HIDDEN ) )
		{
			if ( ch->mount )
				send_to_char( "Tw�j wierzchowiec nie mo�e i�� w tym kierunku.\n\r", ch );
			else
				send_to_char( "Nie mo�esz i�� w tym kierunku.\n\r", ch );
			return;
		}
		else
		{
			send_to_char( "Droga nagle si� tu urywa.\n\r", ch );
			return;
		}

	}

	if ( IS_AFFECTED( ch, AFF_CHARM )
			&& ( !ch->fighting || !IS_NPC( ch ) )
			&& ch->master != NULL
			&& in_room == ch->master->in_room )
	{
		send_to_char( "Co? Chcia�<&/a/o>by� opu�ci� swojego ukochanego mistrza?\n\r", ch );
		return;
	}

	if ( !is_room_owner( ch, to_room ) && room_is_private( to_room ) )
	{
		send_to_char( "Hmmm, chyba jest tam ju� troch� za ciasno.\n\r", ch );
		return;
	}

	DEBUG_INFO( "move_char(5):czy_moze_move");
	if ( IS_NPC( ch ) )
	{
		if ( IS_SET( sector_table[ in_room->sector_type ].flag, SECT_AIR )
				|| IS_SET( sector_table[ to_room->sector_type ].flag, SECT_AIR ) )
		{
			if ( !IS_AFFECTED( ch, AFF_FLYING ) )
				return;
		}

		if ( IS_SET( sector_table[ in_room->sector_type ].flag, SECT_WATERN )
				|| IS_SET( sector_table[ to_room->sector_type ].flag, SECT_WATERN ) )
		{
			if ( !IS_AFFECTED( ch, AFF_FLYING ) )
				return;
		}
	}
	else
	{
		int move;

		if ( !IS_IMMORTAL( ch ) && ( IS_SET( sector_table[ in_room->sector_type ].flag, SECT_AIR ) || IS_SET( sector_table[ to_room->sector_type ].flag, SECT_AIR ) ) )
		{
			if ( ch->mount )
			{
				if ( !IS_AFFECTED( ch->mount, AFF_FLYING ) )
				{
					send_to_char( "Przecie� tw�j wierzchowiec nie umie lata�.\n\r", ch );
					return;
				}
			}
			else
			{
				if ( !IS_AFFECTED( ch, AFF_FLYING ) )
				{
					send_to_char( "Przecie� nie umiesz lata�.\n\r", ch );
					return;
				}
			}
		}

		if ( IS_SET( sector_table[ in_room->sector_type ].flag, SECT_WATER )
				|| IS_SET( sector_table[ to_room->sector_type ].flag, SECT_WATER ) )
		{
			if ( ch->mount )
			{
				if ( !IS_AFFECTED( ch->mount, AFF_FLYING ) ||
						!IS_AFFECTED( ch->mount, AFF_WATERWALK ) ||
						!IS_AFFECTED( ch->mount, AFF_SWIM ) )
				{
					send_to_char( "Chyba nie chcesz utopi� swojego wierzchowca.\n\r", ch );
					return;
				}
			}
			else
			{
				if ( !IS_AFFECTED( ch, AFF_FLYING ) && !IS_AFFECTED( ch, AFF_WATERWALK ) )
				{
					bool found = ( find_boat(ch) != NULL );

					if ( IS_IMMORTAL( ch ) )
						found = TRUE;

                    if ( !found )
					{
						send_to_char( "Potrzebujesz �odzi �eby si� tam dosta�.\n\r", ch );
						return;
					}
				}
			}
		}
        //Edited by Raszer, zmiany w sciaganiu mv z nieumarlych
		/* srednia mova */
        if( is_undead( ch ) )
        {
            move = 1;
        }
        else
        {
            move = ( sector_table[ in_room->sector_type ].mv_loss + sector_table[ to_room->sector_type ].mv_loss ) / 2;
        }
        //koniec edytowania bazowego sciagania mv
        if ( ch->mount )
        {
            skill = get_skill( ch, gsn_riding );
            // na latajacych konikach troche trudniej, ale jak wyjdzie to zawsze zuzywa 1 mv
            if (IS_AFFECTED( ch->mount, AFF_FLYING ) || IS_AFFECTED( ch->mount, AFF_FLOAT))
            {
                if ( number_percent() < URANGE( 10, ( 3 * skill / 4), 95 ) )
                {
                    move = 1;
                    if ( ch->move > move && ch->mount->move > move )
                    {
                        check_improve( ch, NULL, gsn_riding, TRUE, 80 );
                    }
                }
                else
                {
                    move = URANGE( 1, move / 2, 2 );
                    if(is_undead(ch))
                    {
                        move = 1;
                    }//undead tez chce sie uczyc, ale nie chce tracic wiecej mv
                    if ( ch->move > move && ch->mount->move > move )
                    {
                        check_improve( ch, NULL, gsn_riding, FALSE, 100 );
                    }
                }
            }
            else
            {
                if ( number_percent() < URANGE( 20, skill, 95 ) )
                {
                    move = URANGE( 1, move / 2, 2 );
                    if(is_undead(ch))
                    {
                        move = 1;
                    }
                    if ( ch->move > move && ch->mount->move > move )
                    {
                        check_improve( ch, NULL, gsn_riding, TRUE, 80 );
                    }
                }
                else
                {
                    move = URANGE( 1, move / 2, 4 );
                    if(is_undead(ch))
                    {
                        move = 1;
                    }
                    if ( ch->move > move && ch->mount->move > move )
                    {
                        check_improve( ch, NULL, gsn_riding, FALSE, 100 );
                    }
                }
            }
        }
		else
		{
			/* bez butkow trudniej sie idzie */
			if ( get_eq_char( ch, WEAR_FEET ) == NULL && GET_RACE(ch) != RACE_PC_HALFLING && !is_undead(ch) ) //dodano warunek dla nieumarlych
				move += number_range( 0, 2 );

			/* 75% zaladunku? no to sie ciezko idzie */
			if ( get_carry_weight( ch ) > ( 3 * can_carry_w( ch ) / 4 ) )
			{
                if(is_undead(ch))
				{
                  move += 1;
                }else
                {
                move += number_range( 1, 3 );
                }
            }
			/* jesli cos za soba ciagnie*/
			if ( dragged )
			{
				int dragged_weight = get_obj_weight( dragged ); //undeadzi mniej sie mecza, gdy ciagnal cos

				if ( dragged_weight <= can_carry_w( ch ) / 2 )
				{
                    if(is_undead(ch))
				   {
                    move += 1;
                    wait_mod = 1;
                   }else
                   {
					move += move / 2;
					wait_mod = 1;
                   }
				}
				else if ( dragged_weight <= can_carry_w( ch ) )
				{
                    if(is_undead(ch))
				    {
                     move += 1;
                     wait_mod = 2;
                    }else
                    {
					move += ( 3 * move / 4 );
					wait_mod = 2;
                    }
                }
				else if ( dragged_weight <= ( 3 * can_carry_w( ch ) / 2 ) )
				{
                    if(is_undead(ch))
				    {
                      move += 1;
                      wait_mod = 3;
                    }else
                    {
					move *= 2;
					wait_mod = 3;
                    }
				}
				else if ( dragged_weight <= 2 * can_carry_w( ch ) )
				{
                   if(is_undead(ch))
				   {
                    move += 1;
                    wait_mod = 5;
                   }else
                   {
					move *= 3;
					wait_mod = 5;
                   }
				}
				else if ( dragged_weight <= 3 * can_carry_w( ch ) )
				{
					if ( number_percent() < 40 )
					{
                         if(is_undead(ch))
				         {
                         move += 3;
                         wait_mod = 6;
                         }else
                         {
						move *= 3;
						wait_mod = 6;
                        }
					}
					else
					{
						act( "$p jest chyba dla ciebie za ci�kie, nie dasz rady.", ch, dragged, NULL, TO_CHAR );
						act( "$n pr�buje ruszy� $p, ale nie daje rady.", ch, dragged, NULL, TO_ROOM );
						ch->move = UMAX( ch->move - move, 0 );
						WAIT_STATE( ch, 6 );
						return;
					}
				}

			}

			/* dodatek dla druida */
			if ( ch->class == CLASS_DRUID )
			{
				if (IS_SET( sector_table[ in_room->sector_type ].flag, SECT_DRUID ))
				move = UMAX(1, move/2);
				else if ( ch->level > 8 && number_range( (ch->level*3)/2, ch->level*2 ) > number_percent() ) 
                move += 1;
			}

	/* Barbarzyncy nie powinni mie� wi�kszych problem�w z pokonywaniem g�r i podziemi
	   Szamani kt�rzy maj� na sobie czar 'spirit walk' tak�e lepiej si� poruszaj� po w/w lokacjach.*/
	 if (( IS_AFFECTED(ch, AFF_SPIRIT_WALK)) || ch->class == CLASS_BARBARIAN )
	 {
	  switch ( ch->in_room->sector_type )
	  {
		case  4: // wzg�rza
		case  5: // g�ry
		case 13: // wysokie g�ry
		case 14: // podziemia
		case 15: // jaskinia
		case 16: // tundra
		case 17: // podziemia naturalne
		case 26: // lodowiec
		case 27: // arktyczny l�d
		case 33: // step
		case 35: // g�rska �cie�ka
		case 43: // stroma �cie�ka
		case 47: // kopalnia
		move = UMAX(1, move/2);
		break;

		default:	break;
	  }
	 }

			/* latajacy 1/2 mova, min 1 */
			if ( IS_AFFECTED( ch, AFF_FLYING ) || IS_AFFECTED(ch, AFF_FLOAT ))
			{
				move = URANGE( 1, move / 2, 2 );
				if(is_undead(ch))
				{
                   move = 1;
                }
			}

			/* zwolnieni sie bardziej mecza */
			if ( IS_AFFECTED( ch, AFF_SLOW ) )
			{
                if(is_undead(ch))
                {
                    move += 1;
                    wait_mod += 3;
                }
                else
                {
                    move += 2;
                    wait_mod += 3;
                }
			}
			
			/* Przy�pieszeni sa troch� szybsi */
			if ( IS_AFFECTED( ch, AFF_SLOW ) )
			{
             wait_mod = UMIN( wait_mod, wait_mod - 1);
			}
        }
        /*
         * aura of vigor bonus
         */
        if ( is_affected( ch, skill_lookup( "aura of vigor" ) ) )
        {
            move = UMAX( 1, move - 1 );
            wait_mod /= 2;
#ifdef INFO
            print_char( ch, "aura of vigor: %d / %d.\n\r", move, wait_mod );
#endif
        }

        /*
         * koniec liczenia bonusa
         */
		if ( ch->move < move )
		{
			if ( ch->mount )
            {
				print_char( ch, "Nie masz ju� si�y, by si� dalej porusza�.\n\r" );
            }
			else
			{
				if ( dragged )
                {
					print_char( ch, "Nie dasz rady nic poci�gna�, <&sam/sama/samo> si� ledwo ruszasz.\n\r" );
                }
				else
                {
					print_char( ch, "Jeste� wyczerpan<&y/a/e>.\n\r" );
                }
			}
			return;
		}

        if ( !IS_IMMORTAL( ch ) )
        {
            //Rysand: podczas jazdy m�czy si� g��wnie ko�
            //dla skilla w okolicach 0 - move /(1, 2), 100 - move/ (6, 7)
            if(ch->mount)
			{
                if ( ch->mount->move < move )
                {
                    print_char( ch, "Tw�j wierzchowiec nie ma ju� si�y dalej jecha�.\n\r" );
                    return;
                }

                int riding = URANGE(1, get_skill(ch, gsn_riding)/17 + number_range(1, 2), 10);
                ch->mount->move -= UMIN( 1, move );
                move /= riding;
				wait_mod = UMIN( wait_mod, wait_mod - 2);
            }

            ch->move -= UMAX( 1, move );
            /**
             * increase wait if affected by sneak
             */
            if ( IS_AFFECTED( ch, AFF_SNEAK ) )
            {
                wait_mod *= 3;
                wait_mod /= 2;
            }
            WAIT_STATE( ch, ( sector_table[ in_room->sector_type ].wait + sector_table[ to_room->sector_type ].wait / 2 ) + wait_mod );
        }

        if ( !dragged && IS_AFFECTED( ch, AFF_SNEAK ) && get_skill(ch,gsn_sneak) > 0 )
        {
            check_improve( ch, NULL, gsn_sneak, TRUE, 300 );
        }
    }

    DEBUG_INFO( "move_char(6):ruch_z");
    /* ciagnacy nie moga sie skradac*/
    if ( ( !IS_AFFECTED( ch, AFF_SNEAK ) || dragged ) && ch->invis_level < LEVEL_HERO )
    {
        if ( ch->mount )
        {
            if ( IS_AFFECTED( ch->mount, AFF_FLYING ) || IS_AFFECTED(ch->mount, AFF_FLOAT) )
            {
                if ( pexit->vName && pexit->vName[ 0 ] != '\0' )
                {
                    if ( pexit->TextOut && pexit->TextOut[ 0 ] != '\0' )
                        sprintf( buf, "%s", pexit->TextOut );
                    else
                        sprintf( buf, "$N odlatuje nios�c na swoim grzbiecie $c (%s).", pexit->vName );
                }
                else
                {
                    if ( pexit->TextOut && pexit->TextOut[ 0 ] != '\0' )
                        sprintf( buf, "%s", pexit->TextOut );
                    else
                    {
                        if ( door == 4 )
                            sprintf( buf, "$N odlatuje do g�ry, nios�c na swoim grzbiecie $c." );
                        else
                            sprintf( buf, "$N odlatuje na %s, nios�c na swoim grzbiecie $c.", dir_name[ door ] );
                    }
                }
            }
            else
            {
                if ( pexit->vName && pexit->vName[ 0 ] != '\0' )
                {
                    if ( pexit->TextOut && pexit->TextOut[ 0 ] != '\0' )
                        sprintf( buf, "%s", pexit->TextOut );
                    else
                        sprintf( buf, "$N odchodzi nios�c na swoim grzbiecie $c (%s).", pexit->vName );
                }
                else
                {
                    if ( pexit->TextOut && pexit->TextOut[ 0 ] != '\0' )
                        sprintf( buf, "%s", pexit->TextOut );
                    else
                    {
                        if ( race_table[ GET_RACE(ch->mount) ].leave[ ch->mount->sex ] && race_table[ GET_RACE(ch->mount) ].leave[ ch->mount->sex ][ 0 ] != '\0' )
                            sprintf( buf, "$N %s %s %s, nios�c na swoim grzbiecie $c.",
                                    race_table[ GET_RACE(ch->mount) ].leave[ ch->mount->sex ] ,
                                    door == 4 ? "do" : "na",
                                    door == 4 ? "g�ry" : dir_name[ door ] );
                        else
                            sprintf( buf, "$N odchodzi %s %s, nios�c na swoim grzbiecie $c.",
                                    door == 4 ? "do" : "na",
                                    door == 4 ? "g�ry" : dir_name[ door ] );
                    }
                }
            }
            act( buf, ch, NULL, ch->mount, TO_ROOM );
        }
        else
        {
            if ( dragged )
            {
                if ( pexit->vName && pexit->vName[ 0 ] != '\0' )
                {
                    if ( pexit->TextOut && pexit->TextOut[ 0 ] != '\0' )
                        sprintf( buf, "%s", pexit->TextOut );
                    else
                        sprintf( buf, "$n odchodzi ci�gn�c za sob� %s (%s).", dragged->name4, pexit->vName );
                }
                else
                {
                    if ( pexit->TextOut && pexit->TextOut[ 0 ] != '\0' )
                        sprintf( buf, "%s", pexit->TextOut );
                    else
                    {
                        if ( race_table[ GET_RACE(ch) ].leave[ ch->sex ] && race_table[ GET_RACE(ch) ].leave[ ch->sex ][ 0 ] != '\0' )
                            sprintf( buf, "$n %s %s %s ci�gn�c za sob� %s.",
                                    race_table[ GET_RACE(ch) ].leave[ ch->sex ] ,
                                    door == 4 ? "do" : "na",
                                    door == 4 ? "g�ry" : dir_name[ door ],
                                    dragged->name4 );
                        else
                            sprintf( buf, "$n odchodzi %s %s ci�gn�c za sob� %s.",
                                    door == 4 ? "do" : "na",
                                    door == 4 ? "g�ry" : dir_name[ door ],
                                    dragged->name4 );
                    }
                }
            }
            else
                if ( IS_AFFECTED( ch, AFF_FLYING ) || IS_AFFECTED(ch, AFF_FLOAT) )
                {
                    if ( pexit->vName && pexit->vName[ 0 ] != '\0' )
                    {
                        if ( pexit->TextOut && pexit->TextOut[ 0 ] != '\0' )
                            sprintf( buf, "%s", pexit->TextOut );
                        else
                            sprintf( buf, "$n odlatuje (%s).", pexit->vName );
                    }
                    else
                    {
                        if ( pexit->TextOut && pexit->TextOut[ 0 ] != '\0' )
                            sprintf( buf, "%s", pexit->TextOut );
                        else
                        {
                            if ( door == 4 )
                                sprintf( buf, "$n odlatuje do g�ry." );
                            else
                                sprintf( buf, "$n odlatuje na %s.", dir_name[ door ] );
                        }
                    }
                }
                else if ( IS_SET( sector_table[ in_room->sector_type ].flag, SECT_WATER ) ||
                        IS_SET( sector_table[ in_room->sector_type ].flag, SECT_UNDERWATER ) )
                {
                    if ( pexit->vName && pexit->vName[ 0 ] != '\0' )
                    {
                        if ( pexit->TextOut && pexit->TextOut[ 0 ] != '\0' )
                            sprintf( buf, "%s", pexit->TextOut );
                        else
                            sprintf( buf, "$n odp�ywa (%s).", pexit->vName );
                    }
                    else
                    {
                        if ( pexit->TextOut && pexit->TextOut[ 0 ] != '\0' )
                            sprintf( buf, "%s", pexit->TextOut );
                        else
                        {
                            if ( door == 4 )
                                sprintf( buf, "$n odp�ywa do g�ry." );
                            else
                                sprintf( buf, "$n odp�ywa na %s.", dir_name[ door ] );
                        }
                    }
                }
                else
                {
                    if ( pexit->vName && pexit->vName[ 0 ] != '\0' )
                    {
                        if ( pexit->TextOut && pexit->TextOut[ 0 ] != '\0' )
                            sprintf( buf, "%s", pexit->TextOut );
                        else
                            sprintf( buf, "$n odchodzi (%s).", pexit->vName );
                    }
                    else
                    {
                        if ( pexit->TextOut && pexit->TextOut[ 0 ] != '\0' )
                            sprintf( buf, "%s", pexit->TextOut );
                        else
                        {
                            if ( race_table[ GET_RACE(ch) ].leave[ ch->sex ] && race_table[ GET_RACE(ch) ].leave[ ch->sex ][ 0 ] != '\0' )
                                sprintf( buf, "$n %s %s %s.",
                                        race_table[ GET_RACE(ch) ].leave[ ch->sex ] ,
                                        door == 4 ? "do" : "na",
                                        door == 4 ? "g�ry" : dir_name[ door ] );
                            else
                                sprintf( buf, "$n odchodzi %s %s.",
                                        door == 4 ? "do" : "na",
                                        door == 4 ? "g�ry" : dir_name[ door ] );
                        }
                    }
                }

            act( buf, ch, NULL, NULL, TO_ROOM );
        }
    }

    if( pexit->alarm != NULL )
    {
        if( pexit->alarm->level > number_range(18,24) )
        {
            print_char( pexit->alarm, "{RD�wi�k magicznego alarmu rozchodzi si� w twej g�owie. Wyczuwasz obecno�� {G%s{R.{x\n\r", ch->name2 );
        }
        else
        {
            send_to_char( "{RD�wi�k magicznego alarmu rozchodzi si� w twej g�owie.{x\n\r", pexit->alarm );
        }
        pexit->alarm = NULL;
        pexit_rev = pexit->u1.to_room->exit[ rev_dir[ door ] ];
        if( pexit_rev->alarm != NULL )
        {
            pexit_rev->alarm = NULL;
        }
    }
#ifdef ENABLE_NEW_TRACK
	save_track_data(ch, ch->in_room, to_room, 1);
#endif

	DEBUG_INFO( "move_char(6):char_from_to");
	char_from_room( ch );
	char_to_room( ch, to_room );

	DEBUG_INFO( "move_char(7):obj_from_to");
	if ( dragged )
	{
		if ( dragged->item_type == ITEM_LIGHT && dragged->value[4] != 0 )
		{
			--dragged->in_room->light;
			++ch->in_room->light;
		}
		obj_from_room ( dragged );
		obj_to_room ( dragged, ch->in_room );
	}

	/*
	 *  Moment przejscia z rooma do rooma
	 */

	DEBUG_INFO( "move_char:cautious_sleep");
	for( sleeping_char = ch->in_room->people; sleeping_char; sleeping_char = sleeping_char->next_in_room )
	{
		if( sleeping_char->position != POS_SLEEPING || !IS_AFFECTED( sleeping_char, AFF_CAUTIOUS_SLEEP ) )
			continue;

		if ( is_same_group( ch, sleeping_char ) )
			continue;

		if( IS_AFFECTED( sleeping_char, AFF_SLEEP ) )
		{
			affect_strip( sleeping_char, gsn_sleep );
			affect_strip( sleeping_char, gsn_healing_sleep );
			EXT_REMOVE_BIT( sleeping_char->affected_by, AFF_SLEEP );
		}

		send_to_char( "Nagle jaka� silna magia wyrywa ci� ze snu!\n\r", sleeping_char );
		do_stand( sleeping_char, "" );
	}

	DEBUG_INFO( "move_char(8):ruch_do");

	if ( ch->in_room->exit[ rev_dir[ door ] ] && !IS_NULLSTR( ch->in_room->exit[ rev_dir[ door ] ]->vName ) )
		sprintf( direction, "(%s)",  ch->in_room->exit[ rev_dir[ door ] ]->vName );
	else
		sprintf( direction, "%s", dir_name2[ rev_dir[ door ] ] );

    if ( ch->mount )
    {
        if ( IS_AFFECTED( ch->mount, AFF_FLYING ) || IS_AFFECTED( ch->mount, AFF_FLOAT ))
        {
            sprintf( buf, "$N w�a�nie przyleci%s, nios�c na swoim grzbiecie $c.", ch->mount->sex == 2 ? "a�a" : ch->mount->sex == 1 ? "a�" : "a�o" );
        }
        else
        {
            if ( race_table[ GET_RACE(ch->mount) ].arrive[ ch->mount->sex ] &&
                    race_table[ GET_RACE(ch->mount) ].arrive[ ch->mount->sex ][ 0 ] != '\0' )
                sprintf( buf, "$N w�a�nie %s %s, nios�c na swoim grzbiecie $c.",
                        race_table[ GET_RACE(ch->mount) ].arrive[ ch->mount->sex ], direction );
            else
                sprintf( buf, "$N w�a�nie przyby�%s %s, nios�c na swoim grzbiecie $c.", ch->mount->sex == 0 ? "o" : ch->mount->sex == 1 ? "" : "a", direction );
        }

        act( buf, ch, NULL, ch->mount, TO_ROOM );
    }
    else
    {
        if ( ( !IS_AFFECTED( ch, AFF_SNEAK ) || dragged ) && ch->invis_level < LEVEL_HERO )
        {
            if ( dragged )
            {
                if ( race_table[ GET_RACE(ch) ].arrive[ ch->sex ] && race_table[ GET_RACE(ch) ].arrive[ ch->sex ][ 0 ] != '\0' )
                    sprintf( buf, "$n w�a�nie %s %s ci�gn�c za sob� %s.",
                            race_table[ GET_RACE(ch) ].arrive[ ch->sex ],
                            direction,
                            dragged->name4 );
                else
                    sprintf( buf, "$n w�a�nie przyby�%s %s ci�gn�c za sob� %s.", ch->sex == 0 ? "o" : ch->sex == 1 ? "" : "a", dragged->name4, direction );
            }
            else if ( IS_AFFECTED( ch, AFF_FLYING ) || IS_AFFECTED(ch, AFF_FLOAT) )
            {
                sprintf( buf, "$n w�a�nie przylecia%s %s.", ch->sex == 2 ? "�a" : ch->sex == 1 ? "�" : "�o", direction );
            }
            else
            {
                if ( pexit->TextIn && pexit->TextIn[ 0 ] != '\0' )
                {
                    sprintf( buf, "%s", pexit->TextIn );
                }
                else
                {
                    if ( race_table[ GET_RACE(ch) ].arrive[ ch->sex ] &&
                            race_table[ GET_RACE(ch) ].arrive[ ch->sex ][ 0 ] != '\0' )
                        sprintf( buf, "$n w�a�nie %s %s.",
                                race_table[ GET_RACE(ch) ].arrive[ ch->sex ], direction );
                    else
                        sprintf( buf, "$n w�a�nie przyby�%s %s.", ch->sex == 0 ? "o" : ch->sex == 1 ? "" : "a", direction );
                }
            }

            act( buf, ch, NULL, NULL, TO_ROOM );
        }
    }

	if ( in_room == to_room )  /* no circular follows */
		return;

	DEBUG_INFO( "move_char(9):followersy");
	for ( fch = in_room->people; fch != NULL; fch = fch_next )
	{
		fch_next = fch->next_in_room;

		if ( IS_NPC( fch ) && EXT_IS_SET( to_room->room_flags, ROOM_NO_MOB ) &&
				!IS_AFFECTED( fch, AFF_CHARM ) )
			continue;

		if ( fch->master == ch &&
				fch->position == POS_STANDING &&
				can_see_room( fch, to_room ) &&
				can_move( fch ) &&
				!fch->fighting )
		{

			if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_LAW ) && EXT_IS_SET( fch->act, ACT_AGGRESSIVE ) )
			{
				act( "Nie mo�esz przyprowadza� $N do miasta.", ch, NULL, fch, TO_CHAR );
				act( "Nie mo�esz wej�� do miasta.", fch, NULL, NULL, TO_CHAR );
				continue;
			}

			if(IS_AFFECTED(fch, AFF_FLYING))
			{
				act( "Lecisz za $V.", fch, NULL, ch, TO_CHAR );
			}
			else if(IS_AFFECTED(fch, AFF_FLOAT))
			{
				act( "Lewitujesz za $V.", fch, NULL, ch, TO_CHAR );
			}
			else
			{
				act( "Idziesz za $V.", fch, NULL, ch, TO_CHAR );
			}
			move_char( fch, door, TRUE, NULL );
		}
	}

	sprintf( buf, "move_char(10): room [%5d]", ch->in_room->vnum );
	DEBUG_INFO( buf );
	if ( HAS_RTRIGGER( ch->in_room, TRIG_ENTRY ) )
		rp_enter_trigger( ch );

	if ( !ch->in_room )
		return;

	/*		if ( IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_UNDERWATER ))
			{
			send_to_char( "Przestajesz unosi� si� w powietrzu.\n\r", ch );
			affect_strip(ch, 52);
			}
	 */

	do_function(ch, &do_look, "auto");

	DEBUG_INFO( "move_char(11):trap_handler");
	room_trap_handler( ch );

	if ( !ch->in_room )
		return;

	DEBUG_INFO( "move_char(12):bosonodzy");
	/***************/
	/* wstawka dla bosonogich */
	if ( !IS_NPC( ch ) &&
			!IS_IMMORTAL( ch ) &&
			!ch->mount &&
			!IS_AFFECTED( ch, AFF_FLYING ) &&
			!IS_AFFECTED( ch, AFF_FLOAT) &&
			get_eq_char( ch, WEAR_FEET ) == NULL &&
			GET_RACE(ch) != RACE_PC_HALFLING )
	{
		int chance = 2, dam = 0;
		char *wound_msg = NULL;

		switch ( ch->in_room->sector_type )
		{
			case 0:
			case 14:
			case 15:
			case 17:
			case 18:
				chance += 2;
				wound_msg = "Ostry kamyczek wbi� ci si� w pi�t�!\n\r";
				dam = 2;
				break;
			case 1:
				switch ( number_range( 0, 1 ) )
				{
					case 0: wound_msg = ch->sex == 2
						? "W�a�nie wesz�a� na wielk�, zardzewia�� pinezk�!\n\rTwoja stopa eksploduje b�lem!\n\r"
							: "W�a�nie wszed�e� na wielk�, zardzewia�� pinezk�!\n\rTwoja stopa eksploduje b�lem!\n\r";
					case 1: wound_msg = "Czujesz, �e co� wbi�o ci si� w stop�!\n\rWielki, przezroczysty fragment butelki sterczy ci ze stopy!\n\r";
				}
				dam = 3;
				break;
			case 2:
				wound_msg = ch->sex == 2
					? "Wesz�a� na ostry kamyk!\n\r"
					: "Wszed�e� na ostry kamyk!\n\r";
				dam = 0;
				break;
			case 3:
			case 11:
				wound_msg = ch->sex == 2
					? "Wesz�a� w mrowisko! Au�, au�, au�!\n\r"
					: "Wszed�e� w mrowisko! Au�, au�, au�!\n\r";
				dam = 0;
				break;
			case 4:
				chance += 3;
				wound_msg = ch->sex == 2
					? "Przypadkowo kpn�a� spory g�az wystaj�cy z ziemi!\n\rBoli jak diabli...\n\r"
					: "Przypadkowo, kopn��e� spory g�az wystaj�cy z ziemi!\n\rBoli jak diabli...\n\r";
				dam = 2;
				break;
			case 5:
			case 13:
				chance += 5;
				wound_msg = "Twoje stopy nie�le krwawi�, przyda�yby si� jakie� buty!\n\r";
				dam = 2;
				break;
			case 23:
				chance += 60;
				wound_msg = "Twoje stopy zaczynaj� si� sma�y�!\n\r";
				dam = 5;
				break;

			default: chance = 0;break;
		}

		if ( number_percent() < chance )
		{
			if ( dam > 0 )
				ch->hit = UMAX( ch->hit - dam, 1 );

			send_to_char( wound_msg, ch );
			act( "$n podskakuje na jednej nodze, sycz�c z b�lu.", ch, NULL, NULL, TO_ROOM );
		}
	}

	/* jak jedzie na koniu to sprawdzanie czy czasami si� nie zrypie z konia */
	if ( !IS_NPC( ch ) && ch->mount )
	{
		int riding = get_skill( ch, gsn_riding );
		int range = 3;

		if ( number_range( 0, 1000 ) > 900 + riding && !stat_throw( ch, STAT_DEX ) )
		{
			if ( ch->in_room->sector_type == 3 || ch->in_room->sector_type == 11 || ch->in_room->sector_type == 16 )
				range = 4;
			switch ( number_range( 1, range ) )
			{
				case 1:
					act( "Czujesz jak zaczynasz si� zsuwa� z grzbietu $Z i po chwili l�dujesz jak $t na ziemi�.", ch, ch->sex == 2 ? "d�uga" : "d�ugi", ch->mount, TO_CHAR );
					act( "$n zaczyna si� zsuwa� z grzbietu $Z i po chwili l�duje jak $t na ziemi�.", ch, ch->sex == 2 ? "d�uga" : "d�ugi", ch->mount, TO_NOTVICT );
					break;
				case 2:
					act( "$N wykonuje nagle gwa�towny ruch. W ostatniej chwili pr�bujesz si� czego� przytrzyma�, ale ju� po chwili siedzisz obola�$r na ziemi.", ch, NULL, ch->mount, TO_CHAR );
					act( "$N wykonuje nagle gwa�towny ruch. $n w ostatniej chwili pr�buje si� czego� przytrzyma�, ale ju� po chwili siedzi obola�$r na ziemi.", ch, NULL, ch->mount, TO_NOTVICT );
					break;
				case 3:
					act( "$N bierze nagle ostry zakr�t i zanim si� spostrzegasz l�dujesz na plecach z g�o�nym chrupotem. $N zawraca i patrzy si� na ciebie beznami�tnym wzrokiem.", ch, NULL, ch->mount, TO_CHAR );
					act( "$N bierze nagle ostry zakr�t i obserwujesz jak $n l�duje na plecach z g�o�nym chrupotem. $N zawraca i patrzy si� na $c beznami�tnym wzrokiem.", ch, NULL, ch->mount, TO_NOTVICT );
					break;
				case 4:
					act( "W ostatniej chwili zauwa�asz nisko wisz�c� ga���. Po chwili le�ysz na ziemi kln�c jak szewc i wygra�aj�c w kierunku $Z.", ch, NULL, ch->mount, TO_CHAR );
					act( "$n nie zauwa�y$u w por� nisko wisz�cej ga��zi. Po chwili le�y na ziemi kln�c jak szewc i wygra�aj�c w kierunku $Z.", ch, NULL, ch->mount, TO_NOTVICT );
					break;
			}
			ch->mount->mounting = NULL;
			ch->mount = NULL;
			ch->position = POS_SITTING;
		}
	}

	//no to tu nature curse
	if( is_affected(ch, gsn_nature_curse))
	{
		if( ch->in_room->sector_type == 3 ||//las
				ch->in_room->sector_type == 11 ||//puszcza
				ch->in_room->sector_type == 12 ||//bagno
				ch->in_room->sector_type == 30 ||//eden
				ch->in_room->sector_type == 38 )//park
		{
			af = affect_find( ch->affected, gsn_nature_curse );
			WAIT_STATE( ch, af->level/4 );
			ch->move = UMAX( 0, ch->move - number_range(1,af->level/7));
			damage( ch, ch, dice(1,af->level/5)-1, TYPE_UNDEFINED, DAM_PIERCE, FALSE );
		}
	}
    /**
     * rozpadmini adamantytowe rzeczy
     */
    //turn_into_dust_objects_sensitive_to_light( ch, 1 );
    /**
     * utop ciagniete rzeczy
     */
	if ( dragged )
    {
		obj_fall_into_water( ch, dragged );
    }
	/*
	 * If someone is following the char, these triggers get activated
	 * for the followers before the char, but it's safer this way...
	 */
	DEBUG_INFO( "move_char(13):mob_entry_trig");
	if ( IS_NPC( ch ) && HAS_TRIGGER( ch, TRIG_ENTRY ) )
    {
		mp_percent_trigger( ch, NULL, NULL, NULL, &TRIG_ENTRY );
    }
	if ( !ch->in_room )
    {
		return;
    }
	DEBUG_INFO( "move_char(14):greet_trig");
	if ( !IS_NPC( ch ) )
    {
		mp_greet_trigger( ch );
    }
	return;
}

bool check_vname( CHAR_DATA *ch, int door, bool verbose )
{
	ROOM_INDEX_DATA * in_room;
	EXIT_DATA* pexit;

    // mob�w i immortali nie ma sensu stresowa� wirtualnymi nazwami
    // przeje��. No i nie bardzo jest taka potrzeba.
	if ( IS_NPC(ch) || IS_IMMORTAL(ch) ) return FALSE;

	if ( !IS_NPC( ch ) && ch->pcdata->mind_in )
    {
		in_room = ch->pcdata->mind_in;
    }
	else
    {
		in_room = ch->in_room;
    }

	if ( !in_room ) return FALSE;

	if ( (door < 0 || door >= MAX_DIR ) || ( pexit = in_room->exit[door] ) == NULL )
	{
		if ( verbose )
        {
			send_to_char( "Nie mo�esz i�� w tym kierunku.\n\r", ch );
        }
		return TRUE;
	}

	if ( pexit->vName && pexit->vName[0] != '\0' )
	{
		if ( verbose )
        {
			send_to_char( "Nie mo�esz i�� w tym kierunku.\n\r", ch );
        }
		return TRUE;
	}
	return FALSE;
}


void do_north( CHAR_DATA *ch, char *argument )
{
	if ( !check_vname( ch, DIR_NORTH, TRUE ) )
		move_char( ch, DIR_NORTH, FALSE, NULL );
	return;
}



void do_east( CHAR_DATA *ch, char *argument )
{
	if ( !check_vname( ch, DIR_EAST, TRUE ) )
		move_char( ch, DIR_EAST, FALSE, NULL );
	return;
}



void do_south( CHAR_DATA *ch, char *argument )
{
	if ( !check_vname( ch, DIR_SOUTH, TRUE ) )
		move_char( ch, DIR_SOUTH, FALSE, NULL );
	return;
}



void do_west( CHAR_DATA *ch, char *argument )
{
	if ( !check_vname( ch, DIR_WEST, TRUE ) )
		move_char( ch, DIR_WEST, FALSE, NULL );
	return;
}


void do_up( CHAR_DATA *ch, char *argument )
{
	if ( !check_vname( ch, DIR_UP, TRUE ) )
		move_char( ch, DIR_UP, FALSE, NULL );
	return;
}


void do_down( CHAR_DATA *ch, char *argument )
{
	if ( !check_vname( ch, DIR_DOWN, TRUE ) )
		move_char( ch, DIR_DOWN, FALSE, NULL );
	return;
}


int find_door( CHAR_DATA *ch, char *arg )
{
	ROOM_INDEX_DATA * in_room;
	EXIT_DATA *pexit;
	int door = -1;
	int iter;

	if ( !IS_NPC( ch ) && ch->pcdata->mind_in )
	{
		in_room = ch->pcdata->mind_in;
	}
	else
	{
		in_room = ch->in_room;
	}

	door = get_door( ch, arg );

	if ( !IS_NPC(ch) && door >= 0 && check_vname( ch, door, FALSE ) )
	{
		return -1;
	}

	if ( door < 0 )
	{
		door = -1;
		for ( iter = 0; iter <= 5; iter++ )
		{
			if ( ( pexit = in_room->exit[iter] ) != NULL
					&&   IS_SET(pexit->exit_info, EX_ISDOOR)
					&&   pexit->keyword != NULL
					&&   is_name( arg, pexit->keyword ) )
				return iter;
		}
	}

	if ( door < 0 )
	{
		door = -1;
		for ( iter = 0; iter <= 5; iter++ )
		{
			if ( ( pexit = in_room->exit[iter] ) != NULL
					&&   IS_SET(pexit->exit_info, EX_ISDOOR)
					&&   pexit->vName != NULL
					&&   !str_prefix( arg, pexit->vName ) )
				return iter;
		}
	}

	if ( door < 0 )
	{
		return -1;
	}

	if ( in_room->exit[door] == NULL )
	{
		return -1;
	}

	return door;
}

void do_open( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	OBJ_DATA *obj;
	int door;

	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Otworzy� co?\n\r", ch );
		return;
	}

	if ( ( door = find_door( ch, arg ) ) >= 0 )
	{
        /**
         * Dla le�ajuj�cych nie ma lito�ci.
         */
        if ( ch->position == POS_RESTING || ch->position == POS_SITTING ) //Brohacz: BUGFIX: dodalem sprawdzanie POS_SITTING
        {
            send_to_char( "Mo�e lepiej najpierw wsta�?\n\r", ch );
            return;
        }
		/* 'open door' */
		ROOM_INDEX_DATA * to_room;
		EXIT_DATA *pexit;
		EXIT_DATA *pexit_rev;

		pexit = ch->in_room->exit[ door ];

		if ( !IS_SET( pexit->exit_info, EX_ISDOOR ) )
		{
			send_to_char( "Otworzy� co?\n\r", ch );
			return;
		}

		if ( pexit->keyword != NULL
				&& pexit->keyword[ 0 ] != '\0'
				&& pexit->keyword[ 0 ] != ' ' )
		{
			if ( !IS_SET( pexit->exit_info, EX_CLOSED ) )
			{
				if( !pexit->liczba_mnoga )
					act( "$d jest ju� otwarta.", ch, NULL, pexit->keyword, TO_CHAR );
				else
					act( "$d s� ju� otwarte.", ch, NULL, pexit->keyword, TO_CHAR );
				return;
			}

			if ( IS_SET( pexit->exit_info, EX_LOCKED ) )
			{
				if( !pexit->liczba_mnoga )
					act( "$d jest zamkni�ta na klucz.", ch, NULL, pexit->keyword, TO_CHAR );
				else
					act( "$d s� zamkni�te na klucz.", ch, NULL, pexit->keyword, TO_CHAR );
				return;
			}
		}
		else
		{
			if ( !IS_SET( pexit->exit_info, EX_CLOSED ) )
			{
				send_to_char( "Drzwi s� ju� otwarte.\n\r", ch );
				return;
			}

			if ( IS_SET( pexit->exit_info, EX_LOCKED ) )
			{
				send_to_char( "Drzwi s� zamkni�te na klucz.\n\r", ch );
				return;
			}
		}

		REMOVE_BIT( pexit->exit_info, EX_CLOSED );

		room_exit_trap_handler( ch, pexit, TRAP_OPEN );

		if ( !ch->in_room )
			return;

		if ( !rp_locks_trigger( ch, door, &TRIG_OPEN ) )
		{
			if ( pexit->biernik != NULL
					&& pexit->biernik[ 0 ] != '\0'
					&& pexit->biernik[ 0 ] != ' ' )
			{
				act( "$n otwiera $d.", ch, NULL, pexit->biernik, TO_ROOM );
				act( "Otwierasz $d.", ch, NULL, pexit->biernik, TO_CHAR );
			}
			else
			{
				act( "$n otwiera $d.", ch, NULL, pexit->keyword, TO_ROOM );
				act( "Otwierasz $d.", ch, NULL, pexit->keyword, TO_CHAR );
			}
		}

		if ( !ch->in_room )
			return;

		/* open the other side */
		if ( ( to_room = pexit->u1.to_room ) != NULL
				&& ( pexit_rev = to_room->exit[ rev_dir[ door ] ] ) != NULL
				&& pexit_rev->u1.to_room == ch->in_room )
		{
			CHAR_DATA * rch;

			REMOVE_BIT( pexit_rev->exit_info, EX_CLOSED );
			for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
			{
				if( !pexit_rev->liczba_mnoga )
					act( "$d otwiera si�.", rch, NULL, pexit_rev->keyword, TO_CHAR );
				else
					act( "$d si� otwieraj�.", rch, NULL, pexit_rev->keyword, TO_CHAR );
			}
		}
		return;
	}


	if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
	{
		/* open portal */
		if ( obj->item_type == ITEM_PORTAL )
		{
			if ( !IS_SET( obj->value[ 1 ], EX_ISDOOR ) )
			{
				send_to_char( "Nie mo�esz tego zrobi�.\n\r", ch );
				return;
			}

			if ( !IS_SET( obj->value[ 1 ], EX_CLOSED ) )
			{
				send_to_char( "Portal jest ju� otwarty.\n\r", ch );
				return;
			}

			if ( IS_SET( obj->value[ 1 ], EX_LOCKED ) )
			{
				send_to_char( "Portal jest zamkni�ty na klucz.\n\r", ch );
				return;
			}

			REMOVE_BIT( obj->value[ 1 ], EX_CLOSED );

			obj_trap_handler( ch, obj, TRAP_OPEN );

			if ( !ch->in_room )
				return;

			if ( op_common_trigger( ch, obj, &TRIG_OBJOPEN ) )
				return;

			act( "Otwierasz $h.", ch, obj, NULL, TO_CHAR );
			act( "$n otwiera $h.", ch, obj, NULL, TO_ROOM );
			return;
		}

		/* 'open object' */
		if ( obj->item_type != ITEM_CONTAINER )
		{
			send_to_char( "Ten przedmiot nie jest pojemnikiem.\n\r", ch );
			return;
		}

		if ( !IS_SET( obj->value[ 1 ], CONT_CLOSED ) )
		{
			send_to_char( "Ju� jest otwarte.\n\r", ch );
			return;
		}

		if ( !IS_SET( obj->value[ 1 ], CONT_CLOSEABLE ) )
		{
			send_to_char( "Nie mo�esz tego zrobi�.\n\r", ch );
			return;
		}

		if ( IS_SET( obj->value[ 1 ], CONT_LOCKED ) )
		{
			send_to_char( "Ten obiekt jest zamkni�ty na klucz.\n\r", ch );
			return;
		}

		REMOVE_BIT( obj->value[ 1 ], CONT_CLOSED );

		obj_trap_handler( ch, obj, TRAP_OPEN );

		if ( !ch->in_room )
			return;

		if ( op_common_trigger( ch, obj, &TRIG_OBJOPEN ) )
			return;

		if ( !ch->in_room )
			return;

		act( "Otwierasz $h.", ch, obj, NULL, TO_CHAR );
		act( "$n otwiera $h.", ch, obj, NULL, TO_ROOM );
		return;
	}

	send_to_char( "Nie ma tu niczego takiego.\n\r", ch );
	return;
}


void do_close( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	OBJ_DATA *obj;
	int door;

	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Co zamkn��?\n\r", ch );
		return;
	}

	if ( ( door = find_door( ch, arg ) ) >= 0 )
	{
        /**
         * Dla le�ajuj�cych nie ma lito�ci.
         */
        if ( ch->position == POS_RESTING || ch->position == POS_SITTING ) //Brohacz: BUGFIX: dodalem sprawdzanie POS_SITTING
        {
            send_to_char( "Mo�e lepiej najpierw wsta�?\n\r", ch );
            return;
        }
		/* 'close door' */
		ROOM_INDEX_DATA * to_room;
		EXIT_DATA *pexit;
		EXIT_DATA *pexit_rev;

		pexit	= ch->in_room->exit[ door ];

		if ( !IS_SET( pexit->exit_info, EX_ISDOOR ) )
		{
			send_to_char( "Co zamkn��?\n\r", ch );
			return;
		}

		if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
		{
			if ( pexit->keyword != NULL
					&& pexit->keyword[ 0 ] != '\0'
					&& pexit->keyword[ 0 ] != ' ' )
			{
				if( !pexit->liczba_mnoga )
					act( "$d jest ju� zamkni�ta.", ch, NULL, pexit->keyword, TO_CHAR );
				else
					act( "$d s� ju� zamkni�te.", ch, NULL, pexit->keyword, TO_CHAR );
			}
			else
				send_to_char( "Te drzwi s� ju� zamkni�te.\n\r", ch );
			return;
		}

		SET_BIT( pexit->exit_info, EX_CLOSED );

		if ( !rp_locks_trigger( ch, door, &TRIG_CLOSE ) )
		{
			if ( pexit->biernik != NULL
					&& pexit->biernik[ 0 ] != '\0'
					&& pexit->biernik[ 0 ] != ' ' )
			{
				act( "$n zamyka $d.", ch, NULL, pexit->biernik, TO_ROOM );
				act( "Zamykasz $d.", ch, NULL, pexit->biernik, TO_CHAR );
			}
			else
			{
				act( "$n zamyka $d.", ch, NULL, pexit->keyword, TO_ROOM );
				act( "Zamykasz $d.", ch, NULL, pexit->keyword, TO_CHAR );
			}
		}

		/* close the other side */
		if ( ( to_room = pexit->u1.to_room ) != NULL
				&& ( pexit_rev = to_room->exit[ rev_dir[ door ] ] ) != 0
				&& pexit_rev->u1.to_room == ch->in_room )
		{
			CHAR_DATA * rch;

			SET_BIT( pexit_rev->exit_info, EX_CLOSED );
			for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
			{
				if( !pexit_rev->liczba_mnoga )
					act( "$d zamyka si�.", rch, NULL, pexit_rev->keyword, TO_CHAR );
				else
					act( "$d si� zamykaj�.", rch, NULL, pexit_rev->keyword, TO_CHAR );
			}
		}
		return;
	}


	if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
	{
		/* portal stuff */
		if ( obj->item_type == ITEM_PORTAL )
		{

			if ( !IS_SET( obj->value[ 1 ], EX_ISDOOR )
					|| IS_SET( obj->value[ 1 ], EX_NOCLOSE ) )
			{
				send_to_char( "Nie mo�esz tego zrobi�.\n\r", ch );
				return;
			}

			if ( IS_SET( obj->value[ 1 ], EX_CLOSED ) )
			{
				send_to_char( "Portal jest ju� zamkni�ty.\n\r", ch );
				return;
			}

			SET_BIT( obj->value[ 1 ], EX_CLOSED );

			if ( op_common_trigger( ch, obj, &TRIG_OBJCLOSE ) )
				return;

			act( "Zamykasz $h.", ch, obj, NULL, TO_CHAR );
			act( "$n zamyka $h.", ch, obj, NULL, TO_ROOM );
			return;
		}

		/* 'close object' */
		if ( obj->item_type != ITEM_CONTAINER )
		{
			send_to_char( "Ten przedmiot nie jest pojemnikiem.\n\r", ch );
			return;
		}

		if ( IS_SET( obj->value[ 1 ], CONT_CLOSED ) )
		{
			send_to_char( "Ten przedmiot jest ju� zamkni�ty.\n\r", ch );
			return;
		}

		if ( !IS_SET( obj->value[ 1 ], CONT_CLOSEABLE ) )
		{
			send_to_char( "Nie mo�esz tego zrobi�.\n\r", ch );
			return;
		}


		SET_BIT( obj->value[ 1 ], CONT_CLOSED );

		if ( op_common_trigger( ch, obj, &TRIG_OBJCLOSE ) )
			return;

		act( "Zamykasz $h.", ch, obj, NULL, TO_CHAR );
		act( "$n zamyka $h.", ch, obj, NULL, TO_ROOM );
		return;
	}

	send_to_char( "Nie ma tu niczego takiego.\n\r", ch );
	return;
}

OBJ_DATA* has_key( CHAR_DATA *ch, int key )
{
	OBJ_DATA * obj;

	for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
	{
		if ( obj->pIndexData->vnum == key )
			return obj;
	}

	return NULL;
}

void do_lock( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	OBJ_DATA *obj;
	int door;

	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Co zamkn�� na klucz?\n\r", ch );
		return;
	}

	if ( ( door = find_door( ch, arg ) ) >= 0 )
	{
		/* 'lock door' */
		ROOM_INDEX_DATA * to_room;
		EXIT_DATA *pexit;
		EXIT_DATA *pexit_rev;

		pexit	= ch->in_room->exit[ door ];
		if ( !IS_SET( pexit->exit_info, EX_CLOSED ) )
		{
			if ( pexit->keyword != NULL
					&& pexit->keyword[ 0 ] != '\0'
					&& pexit->keyword[ 0 ] != ' ' )
			{
				if( !pexit->liczba_mnoga )
					act( "$d nie jest zamkni�ta.", ch, NULL, pexit->keyword, TO_CHAR );
				else
					act( "$d nie s� zamkni�te.", ch, NULL, pexit->keyword, TO_CHAR );
			}
			else
				send_to_char( "Te drzwi nie s� zamkni�te.\n\r", ch );
			return;
		}
		if ( pexit->key == 0 )
		{
			if ( pexit->keyword != NULL
					&& pexit->keyword[ 0 ] != '\0'
					&& pexit->keyword[ 0 ] != ' ' )
			{
				if( !pexit->liczba_mnoga )
					act( "$d nie mo�e by� zamkni�ta na klucz.", ch, NULL, pexit->keyword, TO_CHAR );
				else
					act( "$d nie mog� by� zamkni�te na klucz.", ch, NULL, pexit->keyword, TO_CHAR );
			}
			else
				send_to_char( "Drzwi nie mog� by� zamkni�te na klucz.\n\r", ch );
			return;

		}
		if ( IS_SET( pexit->exit_info, EX_BROKEN ) )
		{
			send_to_char( "Kto� majstrowa� przy zamku i go uszkodzi�.\n\r", ch ); return;
		}
		if ( !has_key( ch, pexit->key ) )
		{
			send_to_char( "Brak ci klucza.\n\r", ch ); return;
		}
		if ( IS_SET( pexit->exit_info, EX_LOCKED ) )
		{
			if ( pexit->keyword != NULL
					&& pexit->keyword[ 0 ] != '\0'
					&& pexit->keyword[ 0 ] != ' ' )
			{
				if( !pexit->liczba_mnoga )
					act( "$d jest ju� zamkni�ta na klucz.", ch, NULL, pexit->keyword, TO_CHAR );
				else
					act( "$d s� ju� zamkni�te na klucz.", ch, NULL, pexit->keyword, TO_CHAR );
			}
			else
				send_to_char( "Te drzwi s� ju� zamkni�te na klucz.\n\r", ch );
			return;
		}

		SET_BIT( pexit->exit_info, EX_LOCKED );

		if ( !rp_locks_trigger( ch, door, &TRIG_LOCK ) )
		{
			if ( pexit->biernik != NULL
					&& pexit->biernik[ 0 ] != '\0'
					&& pexit->biernik[ 0 ] != ' ' )
			{
				act( "$n zamyka $d na klucz.", ch, NULL, pexit->biernik, TO_ROOM );
				act( "Zamykasz $d na klucz.", ch, NULL, pexit->biernik, TO_CHAR );
			}
			else
			{
				act( "$n zamyka $d na klucz.", ch, NULL, pexit->keyword, TO_ROOM );
				act( "Zamykasz $d na klucz.", ch, NULL, pexit->keyword, TO_CHAR );
			}
		}
		/* lock the other side */
		if ( ( to_room = pexit->u1.to_room ) != NULL
				&& ( pexit_rev = to_room->exit[ rev_dir[ door ] ] ) != 0
				&& pexit_rev->u1.to_room == ch->in_room )
		{
			SET_BIT( pexit_rev->exit_info, EX_LOCKED );
		}
		return;
	}

	if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
	{
		/* portal stuff */
		if ( obj->item_type == ITEM_PORTAL )
		{
			if ( !IS_SET( obj->value[ 1 ], EX_ISDOOR )
					|| IS_SET( obj->value[ 1 ], EX_NOCLOSE ) )
			{
				send_to_char( "Nie mo�esz tego zrobi�.\n\r", ch );
				return;
			}
			if ( !IS_SET( obj->value[ 1 ], EX_CLOSED ) )
			{
				send_to_char( "Musisz najpierw to zamkn��.\n\r", ch );
				return;
			}

			if ( obj->value[ 4 ] < 0 || IS_SET( obj->value[ 1 ], EX_NOLOCK ) )
			{
				send_to_char( "Ten przedmiot nie mo�e by� zamkni�ty.\n\r", ch );
				return;
			}

			if ( IS_SET( obj->value[ 1 ], EX_BROKEN ) )
			{
				send_to_char( "Kto� majstrowa� przy zamku i go uszkodzi�.\n\r", ch );
				return;
			}

			if ( !has_key( ch, obj->value[ 4 ] ) )
			{
				send_to_char( "Brak ci klucza.\n\r", ch );
				return;
			}

			if ( IS_SET( obj->value[ 1 ], EX_LOCKED ) )
			{
				send_to_char( "To ju� jest zamkni�te na klucz.\n\r", ch );
				return;
			}

			SET_BIT( obj->value[ 1 ], EX_LOCKED );

			if ( op_common_trigger( ch, obj, &TRIG_OBJLOCK ) )
				return;

			act( "Zamykasz $h na klucz.", ch, obj, NULL, TO_CHAR );
			act( "$n zamyka $h na klucz.", ch, obj, NULL, TO_ROOM );
			return;
		}

		/* 'lock object' */
		if ( obj->item_type != ITEM_CONTAINER )
		{
			send_to_char( "Ten przedmiot nie jest pojemnikiem.\n\r", ch ); return;
		}
		if ( !IS_SET( obj->value[ 1 ], CONT_CLOSED ) )
		{
			send_to_char( "Ten przedmiot nie jest zamkni�ty.\n\r", ch ); return;
		}
		if ( obj->value[ 2 ] < 0 )
		{
			send_to_char( "Ten przedmiot nie mo�e by� zamkni�ty na klucz.\n\r", ch ); return;
		}
		if ( IS_SET( obj->value[ 1 ], CONT_BROKEN ) )
		{
			send_to_char( "Kto� majstrowa� przy zamku i go uszkodzi�.\n\r", ch ); return;
		}
		if ( !has_key( ch, obj->value[ 2 ] ) )
		{
			send_to_char( "Brak ci klucza.\n\r", ch ); return;
		}
		if ( IS_SET( obj->value[ 1 ], CONT_LOCKED ) )
		{
			send_to_char( "Ten przedmiot jest ju� zamkni�ty na klucz.\n\r", ch ); return;
		}

		SET_BIT( obj->value[ 1 ], CONT_LOCKED );

		if ( op_common_trigger( ch, obj, &TRIG_OBJLOCK ) )
			return;

		act( "Zamykasz $h na klucz.", ch, obj, NULL, TO_CHAR );
		act( "$n zamyka $h na klucz.", ch, obj, NULL, TO_ROOM );
		return;
	}

	send_to_char( "Nie ma tu niczego takiego.\n\r", ch );
	return;
}

void do_unlock( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	OBJ_DATA *obj;
	OBJ_DATA *key;
	int door;

	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Co odkluczy�?\n\r", ch );
		return;
	}

	if ( ( door = find_door( ch, arg ) ) >= 0 )
	{
		/* 'unlock door' */
		ROOM_INDEX_DATA * to_room;
		EXIT_DATA *pexit;
		EXIT_DATA *pexit_rev;

		pexit = ch->in_room->exit[ door ];
		if ( !IS_SET( pexit->exit_info, EX_CLOSED ) )
		{
			send_to_char( "To nie jest zamkni�te.\n\r", ch ); return;
		}
		if ( pexit->key == 0 )
		{
			send_to_char( "To nie mo�e by� odkluczone.\n\r", ch ); return;
		}
		if ( IS_SET( pexit->exit_info, EX_BROKEN ) )
		{
			send_to_char( "Kto� majstrowa� przy zamku i go uszkodzi�.\n\r", ch ); return;
		}
		if ( ( key = has_key( ch, pexit->key ) ) == NULL )
		{
			send_to_char( "Brakuje ci niestety klucza.\n\r", ch ); return;
		}
		if ( !IS_SET( pexit->exit_info, EX_LOCKED ) )
		{
			send_to_char( "To ju� jest odkluczone.\n\r", ch ); return;
		}

		REMOVE_BIT( pexit->exit_info, EX_LOCKED );

		room_exit_trap_handler( ch, pexit, TRAP_UNLOCK );

		if ( !ch->in_room )
			return;

		if ( !rp_locks_trigger( ch, door, &TRIG_UNLOCK ) )
		{
			if ( pexit->biernik != NULL
					&& pexit->biernik[ 0 ] != '\0'
					&& pexit->biernik[ 0 ] != ' ' )
			{
				act( "$n odklucza $d.", ch, NULL, pexit->biernik, TO_ROOM );
				act( "Odkluczasz $d.", ch, NULL, pexit->biernik, TO_CHAR );
			}
			else
			{
				act( "$n odklucza $d.", ch, NULL, pexit->keyword, TO_ROOM );
				act( "Odkluczasz $d.", ch, NULL, pexit->keyword, TO_CHAR );
			}
		}
		/* unlock the other side */
		if ( ( to_room = pexit->u1.to_room ) != NULL
				&& ( pexit_rev = to_room->exit[ rev_dir[ door ] ] ) != NULL
				&& pexit_rev->u1.to_room == ch->in_room )
		{
			REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
		}

		if ( IS_SET( pexit->exit_info, EX_EATKEY ) )
		{
			act( "$p rozp�ywa ci si� w d�oni i po chwili znika.", ch, key, NULL, TO_CHAR );
			obj_from_char( key );
			extract_obj( key );
		}
		return;
	}

	if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
	{
		/* portal stuff */
		if ( obj->item_type == ITEM_PORTAL )
		{
			if ( !IS_SET( obj->value[ 1 ], EX_ISDOOR ) )
			{
				send_to_char( "Nie mo�esz tego zrobi�.\n\r", ch );
				return;
			}

			if ( !IS_SET( obj->value[ 1 ], EX_CLOSED ) )
			{
				send_to_char( "Portal nie jest zamkni�ty.\n\r", ch );
				return;
			}

			if ( obj->value[ 4 ] < 0 )
			{
				send_to_char( "Portal nie mo�e by� odkluczony.\n\r", ch );
				return;
			}

			if ( IS_SET( obj->value[ 1 ], EX_BROKEN ) )
			{
				send_to_char( "Kto� majstrowa� przy zamku i go uszkodzi�.\n\r", ch );
				return;
			}

			if ( ( key = has_key( ch, obj->value[ 4 ] ) ) == NULL )
			{
				send_to_char( "Brak ci klucza.\n\r", ch );
				return;
			}

			if ( !IS_SET( obj->value[ 1 ], EX_LOCKED ) )
			{
				send_to_char( "Portal nie jest zamkni�ty na klucz.\n\r", ch );
				return;
			}

			REMOVE_BIT( obj->value[ 1 ], EX_LOCKED );

			obj_trap_handler( ch, obj, TRAP_UNLOCK );

			if ( !ch->in_room )
				return;

			if ( op_common_trigger( ch, obj, &TRIG_OBJUNLOCK ) )
				return;

			act( "Odkluczasz $h.", ch, obj, NULL, TO_CHAR );
			act( "$n otwiera zamek $f.", ch, obj, NULL, TO_ROOM );

			if ( IS_SET( obj->value[1], EX_EATKEY ) )
			{
				act( "$p rozp�ywa ci si� w d�oni i po chwili znika.", ch, key, NULL, TO_CHAR );
				obj_from_char( key );
				extract_obj( key );
			}
			return;
		}

		/* 'unlock object' */
		if ( obj->item_type != ITEM_CONTAINER )
		{
			send_to_char( "Ten przedmiot nie jest pojemnikiem.\n\r", ch ); return;
		}
		if ( !IS_SET( obj->value[ 1 ], CONT_CLOSED ) )
		{
			send_to_char( "Ten przedmiot nie jest zamkni�ty.\n\r", ch ); return;
		}
		if ( obj->value[ 2 ] < 0 )
		{
			send_to_char( "Ten przedmiot nie mo�e by� odkluczony.\n\r", ch ); return;
		}
		if ( IS_SET( obj->value[ 1 ], CONT_BROKEN ) )
		{
			send_to_char( "Kto� majstrowa� przy zamku i go uszkodzi�.\n\r", ch ); return;
		}
		if ( ( key = has_key( ch, obj->value[ 2 ] ) ) == NULL )
		{
			send_to_char( "Brakuje ci niestety klucza.\n\r", ch ); return;
		}
		if ( !IS_SET( obj->value[ 1 ], CONT_LOCKED ) )
		{
			send_to_char( "Ale� to ju� jest odkluczone.\n\r", ch ); return;
		}

		REMOVE_BIT( obj->value[ 1 ], CONT_LOCKED );

		obj_trap_handler( ch, obj, TRAP_UNLOCK );

		if ( !ch->in_room )
			return;

		if ( op_common_trigger( ch, obj, &TRIG_OBJUNLOCK ) )
			return;

		act( "Odkluczasz $h.", ch, obj, NULL, TO_CHAR );
		act( "$n otwiera zamek $f.", ch, obj, NULL, TO_ROOM );

		if ( IS_SET( obj->value[1], CONT_EATKEY ) )
		{
			act( "$p rozp�ywa ci si� w d�oni i po chwili znika.", ch, key, NULL, TO_CHAR );
			obj_from_char( key );
			extract_obj( key );
		}
		return;
	}

	send_to_char( "Nie ma tu niczego takiego.\n\r", ch );
	return;
}

void do_pick( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	OBJ_DATA *obj;
	int door;
	int chance;

    if( ch->fighting )
	{
		send_to_char("Lepiej skup si� na walce.\n\r", ch);
		return;
	}

	one_argument( argument, arg );

	if ( get_skill( ch, gsn_pick_lock ) <= 0 )
	{
	   send_to_char("Nie masz poj�cia jak to si� robi.\n\r", ch );
	   return;
	}

	if ( arg[ 0 ] == '\0' )
	{
		switch (ch->sex)
		{
			case SEX_NEUTRAL:
				send_to_char( "Gdzie chcia�oby� si� w�ama�?\n\r", ch );
				break;
			case SEX_MALE:
				send_to_char( "Gdzie chcia�by� si� w�ama�?\n\r", ch );
				break;
			case SEX_FEMALE:
				send_to_char( "Gdzie chcia�aby� si� w�ama�?\n\r", ch );
				break;
		}
		return;
	}

	WAIT_STATE( ch, skill_table[ gsn_pick_lock ].beats );

	/* look for guards */
	/*	for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
		{
		if ( IS_NPC(gch) && IS_AWAKE(gch) )
		{
		act( "$N stoi zbyt blisko.",ch, NULL, gch, TO_CHAR );
		return;
		}
		}*/

	/*	if ( !IS_NPC(ch) && number_percent( ) > get_skill(ch,gsn_pick_lock))
		{
		send_to_char( "Nie uda�o ci si�.\n\r", ch);
		check_improve(ch, NULL, gsn_pick_lock,FALSE,2);
		return;
		}*/

	if ( ( door = find_door( ch, arg ) ) >= 0 )
	{
		/* 'pick door' */
		ROOM_INDEX_DATA * to_room;
		EXIT_DATA *pexit;
		EXIT_DATA *pexit_rev;

		pexit = ch->in_room->exit[ door ];
		if ( !IS_SET( pexit->exit_info, EX_CLOSED ) && !IS_IMMORTAL( ch ) )
		{
			send_to_char( "To nie jest zamkni�te.\n\r", ch ); return;
		}
		if ( pexit->key == 0 && !IS_IMMORTAL( ch ) )
		{
			send_to_char( "Niestety, to nie mo�e by� odkluczone wytrychem.\n\r", ch ); return;
		}
		if ( !IS_SET( pexit->exit_info, EX_LOCKED ) )
		{
			send_to_char( "To jest ju� odkluczone.\n\r", ch ); return;
		}
		if ( IS_SET( pexit->exit_info, EX_PICKPROOF ) && !IS_IMMORTAL( ch ) )
		{
			send_to_char_fail_open_lock(ch);
			if ( pexit->biernik != NULL
					&& pexit->biernik[ 0 ] != '\0'
					&& pexit->biernik[ 0 ] != ' ' )
			{
				act( "$n pr�buje otworzy� $d wytrychem, na pr�no.", ch, NULL, pexit->biernik, TO_ROOM );
			}
			else
			{
				act( "$n pr�buje otworzy� $d wytrychem, na pr�no.", ch, NULL, pexit->keyword, TO_ROOM );
			}
			room_exit_trap_handler( ch, pexit, TRAP_PICKFAILED );
			return;
		}
		if ( IS_SET( pexit->exit_info, EX_BROKEN ) )
		{
			send_to_char( "Kto� majstrowa� przy zamku i go uszkodzi�.\n\r", ch ); return;
		}

		chance = get_skill( ch, gsn_pick_lock );
		if ( IS_SET( pexit->exit_info, EX_HARD ) ) chance -= 10;
		else
			if ( IS_SET( pexit->exit_info, EX_EASY ) ) chance += 10;
		chance = URANGE( 2, chance, 95 );

		if ( !IS_NPC( ch ) && number_percent( ) > chance )
		{
			room_exit_trap_handler( ch, pexit, TRAP_PICKFAILED );

			if ( !ch->in_room )
				return;
			if ( pexit->biernik != NULL
					&& pexit->biernik[ 0 ] != '\0'
					&& pexit->biernik[ 0 ] != ' ' )
			{
				act( "Pr�bujesz otworzy� $d wytrychem, na pr�no.", ch, NULL, pexit->biernik, TO_CHAR );
				act( "$n pr�buje otworzy� $d wytrychem, na pr�no.", ch, NULL, pexit->biernik, TO_ROOM );
			}
			else
			{
				act( "Pr�bujesz otworzy� $d wytrychem, na pr�no.", ch, NULL, pexit->keyword, TO_CHAR );
				act( "$n pr�buje otworzy� $d wytrychem, na pr�no.", ch, NULL, pexit->keyword, TO_ROOM );
			}
			check_improve( ch, NULL, gsn_pick_lock, FALSE, 55 );
			return;
		}

		REMOVE_BIT( pexit->exit_info, EX_LOCKED );

		room_exit_trap_handler( ch, pexit, TRAP_PICKOK );

		if ( !ch->in_room )
			return;

		if ( !rp_locks_trigger( ch, door, &TRIG_UNLOCK ) )
		{
			if ( pexit->biernik != NULL
					&& pexit->biernik[ 0 ] != '\0'
					&& pexit->biernik[ 0 ] != ' ' )
			{
				act( "$n otwiera $d wytrychem.", ch, NULL, pexit->biernik, TO_ROOM );
				act( "Otwierasz $d wytrychem.", ch, NULL, pexit->biernik, TO_CHAR );
			}
			else
			{
				act( "$n otwiera $d wytrychem.", ch, NULL, pexit->keyword, TO_ROOM );
				act( "Otwierasz $d wytrychem.", ch, NULL, pexit->keyword, TO_CHAR );
			}
		}

		/* szansa na spieprzenie zamka */
		if ( number_range( 1, 200 ) > 150 + get_curr_stat_deprecated( ch, STAT_DEX ) + get_curr_stat_deprecated( ch, STAT_LUC ) )
		{
			SET_BIT( pexit->exit_info, EX_BROKEN );
			send_to_char( "Niestety manipuluj�c zbyt nieostro�nie przy zamku uszkodzi�e� go.\n\r", ch );
		}

		check_improve( ch, NULL, gsn_pick_lock, TRUE, 45 );

		/* pick the other side */
		if ( ( to_room = pexit->u1.to_room ) != NULL
				&& ( pexit_rev = to_room->exit[ rev_dir[ door ] ] ) != NULL
				&& pexit_rev->u1.to_room == ch->in_room )
		{
			REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
		}
	}

	if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
	{
		/* portal stuff */
		if ( obj->item_type == ITEM_PORTAL )
		{
			if ( !IS_SET( obj->value[ 1 ], EX_ISDOOR ) )
			{
				send_to_char( "Nie mo�esz tego zrobi�.\n\r", ch );
				return;
			}

			if ( !IS_SET( obj->value[ 1 ], EX_CLOSED ) )
			{
				send_to_char( "To nie jest zamkni�te.\n\r", ch );
				return;
			}

			if ( obj->value[ 4 ] < 0 )
			{
				send_to_char( "To nie mo�e by� odkluczone.\n\r", ch );
				return;
			}

			if ( IS_SET( obj->value[ 1 ], EX_PICKPROOF ) )
			{
				send_to_char_fail_open_lock(ch);
				act( "$n pr�buje otworzy� wytrychem zamek $f, na pr�no.", ch, obj, NULL, TO_ROOM );
				obj_trap_handler( ch, obj, TRAP_PICKFAILED );
				return;
			}
			if ( IS_SET( obj->value[ 1 ], CONT_BROKEN ) )
			{
				send_to_char( "Kto� majstrowa� przy zamku i go uszkodzi�.\n\r", ch ); return;
			}

			chance = get_skill( ch, gsn_pick_lock );
			if ( IS_SET( obj->value[ 1 ], EX_HARD ) ) chance -= 10;
			else
				if ( IS_SET( obj->value[ 1 ], EX_EASY ) ) chance += 10;
			chance = URANGE( 2, chance, 95 );

			if ( !IS_NPC( ch ) && number_percent( ) > chance )
			{
				obj_trap_handler( ch, obj, TRAP_PICKFAILED );
				if ( !ch->in_room ) return;
				send_to_char_fail_open_lock(ch);
				act( "$n pr�buje otworzy� wytrychem zamek $f, na pr�no.", ch, obj, NULL, TO_ROOM );
				check_improve( ch, NULL, gsn_pick_lock, FALSE, 40 );
				return;
			}

			REMOVE_BIT( obj->value[ 1 ], EX_LOCKED );

			if ( !op_common_trigger( ch, obj, &TRIG_OBJUNLOCK ) )
			{
				obj_trap_handler( ch, obj, TRAP_PICKOK );

				if ( !ch->in_room )
					return;
				act( "Udaje ci si� otworzy�  wytrychem zamek $f.", ch, obj, NULL, TO_CHAR );
				act( "$n otwiera wytrychem zamek $f.", ch, obj, NULL, TO_ROOM );
			}

			/* szansa na spieprzenie zamka */
			if ( number_range( 1, 50 ) > get_curr_stat_deprecated( ch, STAT_DEX ) + get_curr_stat_deprecated( ch, STAT_LUC ) )
			{
				SET_BIT( obj->value[ 1 ], EX_BROKEN );
				send_to_char( "Niestety manipuluj�c zbyt nieostro�nie przy zamku uszkodzi�", ch );
				switch (ch->sex)
				{
					case SEX_NEUTRAL:
						send_to_char( "o", ch );
						break;
					case SEX_MALE:
						send_to_char( "e", ch );
						break;
					case SEX_FEMALE:
						send_to_char( "a", ch );
						break;
				}
				send_to_char( "� go.\n\r", ch );
			}
			check_improve( ch, NULL, gsn_pick_lock, TRUE, 60 );
			return;
		}
		/* 'pick object' */
		if ( obj->item_type != ITEM_CONTAINER )
		{
			send_to_char( "Dlaczego chcesz to otwiera�?\n\r", ch ); return;
		}
		if ( !IS_SET( obj->value[ 1 ], CONT_CLOSED ) )
		{
			send_to_char( "To nie jest zamkni�te.\n\r", ch ); return;
		}
		if ( obj->value[ 2 ] < 0 )
		{
			send_to_char( "To nie mo�e by� otwarte.\n\r", ch ); return;
		}
		if ( !IS_SET( obj->value[ 1 ], CONT_LOCKED ) )
		{
			send_to_char( "To jest ju� otwarte.\n\r", ch ); return;
		}
		if ( IS_SET( obj->value[ 1 ], CONT_PICKPROOF ) )
		{
			send_to_char_fail_open_lock(ch);
			obj_trap_handler( ch, obj, TRAP_PICKFAILED );
			act( "$n pr�buje otworzy� wytrychem zamek $f, na pr�no.", ch, obj, NULL, TO_ROOM );
			return;
		}
		if ( IS_SET( obj->value[ 1 ], CONT_BROKEN ) )
		{
			send_to_char( "Kto� majstrowa� przy zamku i go uszkodzi�.\n\r", ch ); return;
		}

		chance = get_skill( ch, gsn_pick_lock );
		if ( IS_SET( obj->value[ 1 ], CONT_HARD ) ) chance -= 10;
		else if ( IS_SET( obj->value[ 1 ], CONT_EASY ) ) chance += 10;
		chance = URANGE( 2, chance, 95 );

		if ( !IS_NPC( ch ) && number_percent( ) > chance )
		{
			obj_trap_handler( ch, obj, TRAP_PICKFAILED );

			if ( !ch->in_room )
				return;

			send_to_char_fail_open_lock(ch);

			act( "$n pr�buje otworzy� wytrychem zamek $f, na pr�no.", ch, obj, NULL, TO_ROOM );
			check_improve( ch, NULL, gsn_pick_lock, FALSE, 70 );
			return;
		}

		obj_trap_handler( ch, obj, TRAP_PICKOK );

		if ( !ch->in_room )
			return;

		REMOVE_BIT( obj->value[ 1 ], CONT_LOCKED );
		act( "Udaje ci si� otworzy� wytrychem zamek $f.", ch, obj, NULL, TO_CHAR );
		act( "$n otwiera wytrychem zamek $f.", ch, obj, NULL, TO_ROOM );

		/* szansa na spieprzenie zamka */
		if ( number_range( 1, 50 ) > get_curr_stat_deprecated( ch, STAT_DEX ) + get_curr_stat_deprecated( ch, STAT_LUC ) )
		{
			SET_BIT( obj->value[ 1 ], CONT_BROKEN );
			send_to_char( "Niestety manipuluj�c zbyt nieostro�nie przy zamku uszkodzi�", ch );
			switch (ch->sex)
			{
				case SEX_NEUTRAL:
					send_to_char( "o", ch );
					break;
				case SEX_MALE:
					send_to_char( "e", ch );
					break;
				case SEX_FEMALE:
					send_to_char( "a", ch );
					break;
			}
			send_to_char( "� go.\n\r", ch );
		}

		check_improve( ch, NULL, gsn_pick_lock, TRUE, 60 );
		return;
	}

	return;
}

void do_secret( CHAR_DATA *ch, char *argument )
{
	int door;
	EXIT_DATA *pexit;

	if(argument[0]=='\0')
	{
		send_to_char("Gdzie to tajemne przejscie?\n\r",ch);
		return;
	}

	if( (door=find_door(ch,argument) ) >= 0)
	{
		pexit=ch->in_room->exit[door];

		if( IS_SET(pexit->exit_info, EX_SECRET ) &&
				!IS_SET(pexit->exit_info, EX_HIDDEN ) )
		{
			send_to_char("Odkrywasz sekretne przejscie.\n\r",ch);
			act("$n odkrywa przejscie za $d.",ch,NULL,pexit->keyword,TO_ROOM);
			SET_BIT(pexit->exit_info, EX_HIDDEN);
			return;
		}
		send_to_char("Przeciez to przejscie nie jest tak ukryte.\n\r",ch);
		return;
	}

	send_to_char("Gdzie to tajemne przejscie?\n\r",ch);
	return;
}


void do_stand( CHAR_DATA *ch, char *argument )
{
	PROG_LIST *prg;
	OBJ_DATA * obj = NULL;
	bool changed_pos = FALSE;
	bool show = TRUE;
	int prev_pos;

	if ( ch->mount )
	{
		print_char( ch, "Mo�e najpierw zsi�d� ze swojego wierzchowca.\n\r" );
		return;
	}

	if ( argument[ 0 ] != '\0' )
	{
		if ( ch->position == POS_FIGHTING )
		{
			print_char( ch, "Nie powin<&iene�/na�/no�> najpierw sko�czy� walki?\n\r" );
			return;
		}

		obj = get_obj_list( ch, argument, ch->in_room->contents );

		if ( obj == NULL )
		{
			send_to_char( "Nie widzisz tego tutaj.\n\r", ch );
			return;
		}

		if ( obj->item_type != ITEM_FURNITURE
				|| ( !IS_SET( obj->value[ 2 ], STAND_AT )
					&& !IS_SET( obj->value[ 2 ], STAND_ON )
					&& !IS_SET( obj->value[ 2 ], STAND_IN ) ) )
		{
			send_to_char( "Nie mozesz znalezc miejca na ktorym moznaby stanac.\n\r", ch );
			return;
		}
		if ( ch->on != obj && count_users( obj ) >= obj->value[ 0 ] )
		{
			if ( IS_SET( obj->value[ 2 ], STAND_AT ) )
				act_new( "Nie ma miejsca, aby stan�� przy $k.", ch, obj, NULL, TO_CHAR, POS_DEAD );
			else if ( IS_SET( obj->value[ 2 ], STAND_ON ) )
				act_new( "Nie ma miejsca, aby stan�� na $k.", ch, obj, NULL, TO_CHAR, POS_DEAD );
			else
				act_new( "Nie ma miejsca, aby stan�� w $k.", ch, obj, NULL, TO_CHAR, POS_DEAD );
			return;
		}

		if ( obj && ch->on != obj )
			changed_pos = TRUE;

		ch->on = obj;
	}

	if ( !obj )
		ch->on = NULL;

	// wstawka z rp_pos_trigger
	if ( !IS_NPC( ch ) && HAS_RTRIGGER( ch->in_room, TRIG_STAND ) )
	{
		for ( prg = ch->in_room->progs; prg != NULL; prg = prg->next )
		{
			if ( prg->trig_type != &TRIG_STAND )
				continue;
			if ( ( is_number( prg->trig_phrase ) && obj != NULL && ( obj->pIndexData->vnum == atoi( prg->trig_phrase ) ) )
					|| !str_cmp( prg->trig_phrase, "none" ) )
			{
				show = FALSE;
				break;
			}
		}
	}

	prev_pos = ch->position;

	switch ( ch->position )
	{
		case POS_SLEEPING:
			if ( IS_AFFECTED( ch, AFF_SLEEP ) )
			{
				send_to_char( "Nie mo�esz si� obudzi�!\n\r", ch );
				return;
			}

			if( EXT_IS_SET( ch->in_room->room_flags, ROOM_SLIPPERY_FLOOR ) && number_percent() > URANGE( 10, ch->level + ((get_curr_stat_deprecated(ch, STAT_DEX)-10)*5), 90 ) && !IS_AFFECTED( ch, AFF_STABILITY ) )
			{
				send_to_char( "Pr�bujesz wsta�, jednak przez rozlan� na pod�o�u t�ust� i �lisk� substancj� twoja noga po�lizgn�a si� i padasz na ziemi�.\n\r", ch );
				act( "$n pr�buje wsta�, jednak przez rozlan� na pod�o�u t�ust� i �lisk� substancj� $s noga po�lizgn�a si� i pada $e na ziemi�.", ch, NULL, NULL, TO_ROOM );
				ch->position = POS_SITTING;
				WAIT_STATE( ch, PULSE_VIOLENCE/2 );
				return;
			}

			if ( show )
			{
				if ( !GOT_PART( ch, PART_LEGS ) )
				{
					act( "Budzisz si�.", ch, obj, NULL, TO_CHAR );
					act( "$n budzi si�.", ch, obj, NULL, TO_ROOM );
				}
				else if ( obj == NULL )
				{
					send_to_char( "Budzisz si� i wstajesz.\n\r", ch );
					act( "$n budzi si� i wstaje.", ch, NULL, NULL, TO_ROOM );
				}
				else if ( IS_SET( obj->value[ 2 ], STAND_AT ) )
				{
					act_new( "Budzisz si� i stajesz przy $k.", ch, obj, NULL, TO_CHAR, POS_DEAD );
					act( "$n budzi si� i staje przy $k.", ch, obj, NULL, TO_ROOM );
				}
				else if ( IS_SET( obj->value[ 2 ], STAND_ON ) )
				{
					act_new( "Budzisz si� i stajesz na $k.", ch, obj, NULL, TO_CHAR, POS_DEAD );
					act( "$n budzi si� i staje na $k.", ch, obj, NULL, TO_ROOM );
				}
				else
				{
					act_new( "Budzisz si� i stajesz w $k.", ch, obj, NULL, TO_CHAR, POS_DEAD );
					act( "$n budzi si� i staje w $k.", ch, obj, NULL, TO_ROOM );
				}
			}
			rp_pos_trigger( ch, obj, &TRIG_STAND );

			if ( ch->position == prev_pos )
				ch->position = POS_STANDING;
			do_function( ch, &do_look, "auto" );
			break;

		case POS_RESTING:
			if ( ch->memming )
			{
				echo_mem( ch, MEM_STOP );
				ch->memming = NULL;
				echo_mem( ch, MEM_STOP_ALL );
			}

			if ( IS_AFFECTED( ch, AFF_RECUPERATE ) )
				affect_strip( ch, gsn_recuperate );

		case POS_SITTING:
			if( EXT_IS_SET( ch->in_room->room_flags, ROOM_SLIPPERY_FLOOR ) && number_percent() > URANGE( 10, ch->level + ((get_curr_stat_deprecated(ch, STAT_DEX)-10)*5), 90 ) && !IS_AFFECTED( ch, AFF_STABILITY ) )
			{
				send_to_char( "Pr�bujesz wsta�, jednak przez rozlan� na pod�o�u t�ust� i �lisk� substancj� twoja noga po�lizgn�a si� i padasz na ziemi�.\n\r", ch );
				act( "$n pr�buje wsta�, jednak przez rozlan� na pod�o�u t�ust� i �lisk� substancj� $s noga po�lizgn�a si� i pada $e na ziemi�.", ch, NULL, NULL, TO_ROOM );
				ch->position = POS_SITTING;
				WAIT_STATE( ch, PULSE_VIOLENCE/2 );
				return;
			}

			if ( show )
			{
				if ( (EXT_IS_SET(ch->act, ACT_MOUNTABLE)) && IS_SET( race_table[ GET_RACE(ch) ].type, ANIMAL ) && ch->fighting == NULL )
				{			
					act( "$n przestaje si� wypasa�.", ch, obj, NULL, TO_ROOM );
				}
				else if ( !GOT_PART( ch, PART_LEGS ) )
				{
					act( "Przestajesz odpoczywa�.", ch, obj, NULL, TO_CHAR );
					act( "$n przestaje odpoczywa�.", ch, obj, NULL, TO_ROOM );
				}
				else if ( obj == NULL )
				{
					send_to_char( "Wstajesz.\n\r", ch );
					act( "$n wstaje.", ch, NULL, NULL, TO_ROOM );
				}
				else if ( IS_SET( obj->value[ 2 ], STAND_AT ) )
				{
					act( "Przerywasz odpoczynek i stajesz przy $k.", ch, obj, NULL, TO_CHAR );
					act( "$n przerywa odpoczynek i staje przy $k.", ch, obj, NULL, TO_ROOM );
				}
				else if ( IS_SET( obj->value[ 2 ], STAND_ON ) )
				{
					act( "Przerywasz odpoczynek i stajesz na $k.", ch, obj, NULL, TO_CHAR );
					act( "$n przerywa odpoczynek i staje na $k.", ch, obj, NULL, TO_ROOM );
				}
				else
				{
					act( "Przerywasz odpoczynek i stajesz w $k.", ch, obj, NULL, TO_CHAR );
					act( "$n przerywa odpoczynek i staje w $k.", ch, obj, NULL, TO_ROOM );
				}
			}
			rp_pos_trigger( ch, obj, &TRIG_STAND );

			if ( ch->position == prev_pos )
				ch->position = POS_STANDING;
			break;

		case POS_STANDING:
			if ( changed_pos )
			{
				if ( show )
				{
					if ( IS_SET( obj->value[ 2 ], STAND_AT ) )
					{
						act( "Stajesz przy $k.", ch, obj, NULL, TO_CHAR );
						act( "$n staje przy $k.", ch, obj, NULL, TO_ROOM );
					}
					else if ( IS_SET( obj->value[ 2 ], STAND_ON ) )
					{
						act( "Stajesz na $k.", ch, obj, NULL, TO_CHAR );
						act( "$n staje na $k.", ch, obj, NULL, TO_ROOM );
					}
					else
					{
						act( "Stajesz w $k.", ch, obj, NULL, TO_CHAR );
						act( "$n staje w $k.", ch, obj, NULL, TO_ROOM );
					}
				}
				rp_pos_trigger( ch, obj, &TRIG_REST );
			}
			else
			{
				send_to_char( "Przecie� juz stoisz.\n\r", ch );
				return;
			}
			break;

		case POS_FIGHTING:
			send_to_char( "Ju� walczysz!\n\r", ch );
			return;
			break;
	}

	return;
}

bool can_rest(CHAR_DATA *ch, OBJ_DATA **boat)
{
	if ( ch->position == POS_FIGHTING || ch->fighting )
	{
		send_to_char( "Przecie� wci�� walczysz!\n\r", ch );
		return FALSE;
	}

	if ( IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_NOREST ) )
	{
		if ( IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_WATER)
		       || IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_WATERN) )
		{
			if ( ( *boat = find_boat(ch) ) == NULL ) {
				send_to_char( "Tutaj? Bez �odzi? Oszala�<&e/a/o>�?\n\r", ch );
				return FALSE;
			}
			else
				return TRUE;
		}

		send_to_char( "Tutaj? Oszala�<&e/a/o>�?\n\r", ch );
		return FALSE;
	}

	if ( ch->mount )
	{
		send_to_char( "Zsi�d� najpierw ze swojego wierzchowca!\n\r", ch );
		return FALSE;
	}
	return TRUE;
}

void do_rest( CHAR_DATA *ch, char *argument )
{
	PROG_LIST *prg;
	OBJ_DATA * obj = NULL;
	OBJ_DATA * boat = NULL;
	bool test = FALSE, show = TRUE, changed_pos = FALSE, check;
	int prev_pos;

    if (ch->position == POS_SLEEPING)
	{
		send_to_char("�nisz o wspania�ym wypoczynku.\n\r", ch );
		return;
	}

	if ( !can_rest(ch, &boat) )
		return;

	/* okay, now that we know we can rest, find an object to rest on */
	if ( argument[ 0 ] != '\0' )
	{
		obj = get_obj_list( ch, argument, ch->in_room->contents );
		if ( obj == NULL )
		{
			send_to_char( "Nie widzisz tego tutaj.\n\r", ch );
			return;
		}
	}
	else obj = ch->on;

	if ( obj != NULL )
	{
		if ( !can_see_obj(ch, obj) )
		{
			send_to_char("Nie widzisz tego tutaj.\n\r", ch);
			return;
		}

		if ( obj->item_type != ITEM_FURNITURE
				|| ( !IS_SET( obj->value[ 2 ], REST_ON )
					&& !IS_SET( obj->value[ 2 ], REST_IN )
					&& !IS_SET( obj->value[ 2 ], REST_AT ) ) )
		{
			send_to_char( "Nie mo�esz na tym odpoczywa�.\n\r", ch );
			return;
		}

		if ( obj != NULL && ch->on != obj && count_users( obj ) >= obj->value[ 0 ] )
		{
			if ( IS_SET( obj->value[ 2 ], REST_AT ) )
				act_new( "Nie ma wolnego miejsca przy $k.", ch, obj, NULL, TO_CHAR, POS_DEAD );
			else if ( IS_SET( obj->value[ 2 ], REST_ON ) )
				act_new( "Nie ma wolnego miejsca na $k.", ch, obj, NULL, TO_CHAR, POS_DEAD );
			else
				act_new( "Nie ma wolnego miejsca w $k.", ch, obj, NULL, TO_CHAR, POS_DEAD );
			return;
		}

		if ( obj && ch->on != obj )
			changed_pos = TRUE;

		ch->on = obj;
	}

	// wstawka z rp_pos_trigger
	if ( !IS_NPC( ch ) && HAS_RTRIGGER( ch->in_room, TRIG_REST ) )
	{
		for ( prg = ch->in_room->progs; prg != NULL; prg = prg->next )
		{
			if ( prg->trig_type != &TRIG_REST )
				continue;
			if ( ( is_number( prg->trig_phrase ) && obj != NULL && ( obj->pIndexData->vnum == atoi( prg->trig_phrase ) ) )
					|| !str_cmp( prg->trig_phrase, "none" ) )
			{
				show = FALSE;
				break;
			}
		}
	}

	prev_pos = ch->position;

	switch ( ch->position )
	{
		case POS_SLEEPING:
			if ( IS_AFFECTED( ch, AFF_SLEEP ) )
			{
				send_to_char( "Nie mo�esz sie obudzi�!\n\r", ch );
				return;
			}

			if ( show )
			{
				if ( boat ) {
					act( "Budzisz si� i zaczynasz odpoczywa� na $k.", ch, boat, NULL, TO_CHAR );
					act( "$n budzi si� i zaczyna odpoczywa� na $k.", ch, boat, NULL, TO_ROOM );
				}
				else if ( obj == NULL )
				{
					act( "Budzisz si� i zaczynasz odpoczywa�.", ch, NULL, NULL, TO_CHAR );
					act( "$n budzi si� i zaczyna odpoczywa�.", ch, NULL, NULL, TO_ROOM );
				}
				else if ( IS_SET( obj->value[ 2 ], REST_AT ) )
				{
					act( "Budzisz si� i zaczynasz odpoczywa� przy $k.", ch, obj, NULL, TO_CHAR );
					act( "$n budzi si� i zaczyna odpoczywa� przy $k.", ch, obj, NULL, TO_ROOM );
				}
				else if ( IS_SET( obj->value[ 2 ], REST_ON ) )
				{
					act( "Budzisz si� i zaczynasz odpoczywa� na $k.", ch, obj, NULL, TO_CHAR );
					act( "$n budzi si� i zaczyna odpoczywa� na $k.", ch, obj, NULL, TO_ROOM );
				}
				else
				{
					act( "Budzisz si� i zaczynasz odpoczywa� w $k.", ch, obj, NULL, TO_CHAR );
					act( "$n budzi si� i zaczyna odpoczywa� w $k.", ch, obj, NULL, TO_ROOM );
				}
			}
			rp_pos_trigger( ch, obj, &TRIG_REST );

			test = TRUE;
			if ( ch->position == prev_pos )
				ch->position = POS_RESTING;
			break;

		case POS_RESTING:
			if ( changed_pos )
			{
				if ( show )
				{
					if ( !GOT_PART( ch, PART_LEGS ) )
					{
						act( "Zaczynasz odpoczywa�.", ch, obj, NULL, TO_CHAR );
						act( "$n zaczyna odpoczywa�.", ch, obj, NULL, TO_ROOM );
					}
					else if ( boat ) {
						act( "Siadasz na $k i zaczynasz odpoczywa�.", ch, boat, NULL, TO_CHAR );
						act( "$n siada na $k i zaczyna odpoczywa�.", ch, boat, NULL, TO_ROOM );
					}
					else if ( IS_SET( obj->value[ 2 ], REST_AT ) )
					{
						act( "Siadasz przy $k i zaczynasz odpoczywa�.", ch, obj, NULL, TO_CHAR );
						act( "$n siada przy $k i zaczyna odpoczywa�.", ch, obj, NULL, TO_ROOM );
					}
					else if ( IS_SET( obj->value[ 2 ], REST_ON ) )
					{
						act( "Siadasz na $k i zaczynasz odpoczywa�.", ch, obj, NULL, TO_CHAR );
						act( "$n siada na $k i zaczyna odpoczywa�.", ch, obj, NULL, TO_ROOM );
					}
					else
					{
						act( "Siadasz w $k i zaczynasz odpoczywa�.", ch, obj, NULL, TO_CHAR );
						act( "$n siada w $k i zaczyna odpoczywa�.", ch, obj, NULL, TO_ROOM );
					}
				}
				rp_pos_trigger( ch, obj, &TRIG_REST );
			}
			else
			{
				send_to_char( "Przecie� ju� odpoczywasz.\n\r", ch );
				return;
			}
			return;

		case POS_STANDING:
			if ( is_affected( ch, gsn_fly ) )
			{
				if ( skill_table[ gsn_fly ].msg_off )
				{
					send_to_char( skill_table[ gsn_fly ].msg_off, ch );
					send_to_char( "\n\r", ch );
				}
				affect_strip( ch, gsn_fly );
			}

			if ( is_affected( ch, gsn_float ) )
			{
				if ( skill_table[ gsn_float ].msg_off )
				{
					send_to_char( skill_table[ gsn_float ].msg_off, ch );
					send_to_char( "\n\r", ch );
				}
				affect_strip( ch, gsn_float );
			}

            if ( show )
            {
                if ( !GOT_PART( ch, PART_LEGS ) )
                {
                    act( "Zaczynasz odpoczywa�.", ch, obj, NULL, TO_CHAR );
                    act( "$n zaczyna odpoczywa�.", ch, obj, NULL, TO_ROOM );
                }
				else if ( (EXT_IS_SET(ch->act, ACT_MOUNTABLE)) && IS_SET( race_table[ GET_RACE(ch) ].type, ANIMAL )  )
				{
                    act( "Odbiegasz kawa�ek i zaczynasz si� wypasa�.", ch, NULL, NULL, TO_CHAR );
                    act( "$n oddala si� kawa�ek i zaczyna si� wypasa�.", ch, NULL, NULL, TO_ROOM );				
				}
                else if ( boat )
				{
                    act( "Siadasz na $k i zaczynasz odpoczywa�.", ch, boat, NULL, TO_CHAR );
                    act( "$n siada na $k i zaczyna odpoczywa�.", ch, boat, NULL, TO_ROOM );
                }
                else if ( obj == NULL )
                {
                    act( "Siadasz i zaczynasz odpoczywa�.", ch, NULL, NULL, TO_CHAR );
                    act( "$n siada i zaczyna odpoczywa�.", ch, NULL, NULL, TO_ROOM );
                }
                else if ( IS_SET( obj->value[ 2 ], REST_AT ) )
                {
                    act( "Siadasz przy $k i zaczynasz odpoczywa�.", ch, obj, NULL, TO_CHAR );
                    act( "$n siada przy $k i zaczyna odpoczywa�.", ch, obj, NULL, TO_ROOM );
                }
                else if ( IS_SET( obj->value[ 2 ], REST_ON ) )
                {
                    act( "Siadasz na $k i zaczynasz odpoczywa�.", ch, obj, NULL, TO_CHAR );
                    act( "$n siada na $k i zaczyna odpoczywa�.", ch, obj, NULL, TO_ROOM );
                }
                else
                {
                    act( "Siadasz w $k i zaczynasz odpoczywa�.", ch, obj, NULL, TO_CHAR );
                    act( "$n siada w $k i zaczyna odpoczywa�.", ch, obj, NULL, TO_ROOM );
                }
            }
			rp_pos_trigger( ch, obj, &TRIG_REST );

			test = TRUE;
			if ( ch->position == prev_pos )
				ch->position = POS_RESTING;
			break;

		case POS_SITTING:
			if ( show )
			{
				if ( obj == NULL )
				{
					send_to_char( "Zaczynasz odpoczywa�.\n\r", ch );
					act( "$n zaczyna odpoczywa�.", ch, NULL, NULL, TO_ROOM );
				}
				else if ( boat ) {
					act( "Zaczynasz odpoczywa� na $k.", ch, boat, NULL, TO_CHAR );
					act( "$n zaczyna odpoczywa� na $k.", ch, boat, NULL, TO_ROOM );
				}
				else if ( IS_SET( obj->value[ 2 ], REST_AT ) )
				{
					act( "Zaczynasz odpoczywa� przy $k.", ch, obj, NULL, TO_CHAR );
					act( "$n zaczyna odpoczywa� przy $k.", ch, obj, NULL, TO_ROOM );
				}
				else if ( IS_SET( obj->value[ 2 ], REST_ON ) )
				{
					act( "Zaczynasz odpoczywa� na $k.", ch, obj, NULL, TO_CHAR );
					act( "$n zaczyna odpoczywa� na $k.", ch, obj, NULL, TO_ROOM );
				}
				else
				{
					act( "Zaczynasz odpoczywa� w $k.", ch, obj, NULL, TO_CHAR );
					act( "$n zaczyna odpoczywa� w $k.", ch, obj, NULL, TO_ROOM );
				}
			}
			rp_pos_trigger( ch, obj, &TRIG_REST );

			test = TRUE;
			if ( ch->position == prev_pos )
				ch->position = POS_RESTING;
			break;
	}

    /**
     * check has rider
     */
    if ( IS_NPC( ch ) && ch->mounting )
    {
        act( "$n zrzuca ci� z siebie.", ch, NULL, ch->mounting, TO_VICT );
        act( "$n zrzuca $Z z siebie.", ch, NULL, ch->mounting, TO_NOTVICT );
        ch->mounting->position = POS_RESTING;
        do_dismount_body( ch );
    }

	if ( test && ch->count_memspell > 0 )
	{
		echo_mem( ch, MEM_START );
		get_next_memming( ch );
	}
	return;
}

void do_sit ( CHAR_DATA *ch, char *argument )
{
	PROG_LIST *prg;
	OBJ_DATA * obj = NULL;
	OBJ_DATA * boat = NULL;
	bool changed_pos = FALSE;
	bool show = TRUE;
	int prev_pos;

	if ( !can_rest(ch, &boat) )
		return;

	if ( !GOT_PART( ch, PART_LEGS ) )
	{
		send_to_char( "Troch� trudno b�dzie ci usi���, nie masz n�g!\n\r", ch );
		return;
	}

	/* okay, now that we know we can sit, find an object to sit on */
	if ( argument[ 0 ] != '\0' )
	{
		obj = get_obj_list( ch, argument, ch->in_room->contents );
		if ( obj == NULL )
		{
			send_to_char( "Nie widzisz tego tutaj.\n\r", ch );
			return;
		}
	}
	else obj = ch->on;

	if ( obj != NULL )
	{

		if ( !can_see_obj(ch, obj) )
		{
			send_to_char("Nie widzisz tego tutaj.\n\r", ch);
			return;
		}

		if ( obj->item_type != ITEM_FURNITURE && obj->item_type != ITEM_MUSICAL_INSTRUMENT)
		{
			send_to_char( "Nie mo�esz na tym usi���.\n\r", ch );
			return;
		}

		if ( obj->item_type == ITEM_MUSICAL_INSTRUMENT && IS_SET( obj->wear_flags, ITEM_TAKE ))
		{
			send_to_char( "Nie mo�esz na tym usi���.\n\r", ch );
			return;
		}

		if ( obj->item_type == ITEM_FURNITURE && (!IS_SET( obj->value[ 2 ], SIT_ON ) && !IS_SET( obj->value[ 2 ], SIT_IN ) && !IS_SET( obj->value[ 2 ], SIT_AT ) ))
		{
			send_to_char( "Nie mo�esz na tym usi���.\n\r", ch );
			return;
		}

		if ( (obj->item_type == ITEM_FURNITURE) && ( obj != NULL && ch->on != obj && count_users( obj ) >= obj->value[ 0 ] ))
		{
			act_new( "Nie ma miejsca na $k.", ch, obj, NULL, TO_CHAR, POS_DEAD );
			return;
		}

		if ( (obj->item_type == ITEM_MUSICAL_INSTRUMENT) && ( obj != NULL && ch->on != obj && count_users( obj ) >= 1 ))
		{
			act_new( "Nie ma miejsca przy $k.", ch, obj, NULL, TO_CHAR, POS_DEAD );
			return;
		}

		if ( obj && ch->on != obj )
			changed_pos = TRUE;

		ch->on = obj;
	}

	// wstawka z rp_pos_trigger
	if ( !IS_NPC( ch ) && HAS_RTRIGGER( ch->in_room, TRIG_SIT ) )
	{
		for ( prg = ch->in_room->progs; prg != NULL; prg = prg->next )
		{
			if ( prg->trig_type != &TRIG_SIT )
				continue;
			if ( ( is_number( prg->trig_phrase ) && obj != NULL && ( obj->pIndexData->vnum == atoi( prg->trig_phrase ) ) )
					|| !str_cmp( prg->trig_phrase, "none" ) )
			{
				show = FALSE;
				break;
			}
		}
	}

	prev_pos = ch->position;

	switch ( ch->position )
	{
		case POS_SLEEPING:
			if ( IS_AFFECTED( ch, AFF_SLEEP ) )
			{
				send_to_char( "Nie mo�esz si� obudzi�!\n\r", ch );
				return;
			}

			if ( show )
			{
				if ( boat ) {
					act( "Budzisz si�, siadasz wygodnie na $k i pr�bujesz rozejrze� dooko�a.", ch, boat, NULL, TO_CHAR );
					act( "$n budzi si� i siada na $k.", ch, boat, NULL, TO_ROOM );
				}
				else if ( obj == NULL )
				{
					act( "Budzisz si�, siadasz wygodnie i pr�bujesz rozejrze� dooko�a.", ch, NULL, NULL, TO_CHAR );
					act( "$n budzi si� i siada na ziemi.", ch, NULL, NULL, TO_ROOM );
				}
				else if ( obj->item_type == ITEM_FURNITURE && IS_SET( obj->value[ 2 ], SIT_AT ) )
				{
					act( "Budzisz si�, siadasz przy $k i pr�bujesz troch� rozejrze�.", ch, obj, NULL, TO_CHAR );
					act( "$n budzi si� i siada przy $k.", ch, obj, NULL, TO_ROOM );
				}
				else if ( obj->item_type == ITEM_FURNITURE && IS_SET( obj->value[ 2 ], SIT_ON ) )
				{
					act( "Budzisz si�, siadasz na $k i pr�bujesz troch� rozejrze�.", ch, obj, NULL, TO_CHAR );
					act( "$n budzi si� i siada przy $k.", ch, obj, NULL, TO_ROOM );
				}
				else if ( obj->item_type == ITEM_MUSICAL_INSTRUMENT )
				{
					act( "Budzisz si�, siadasz przy $k i pr�bujesz troch� rozejrze�.", ch, obj, NULL, TO_CHAR );
					act( "$n budzi si� i siada przy $k.", ch, obj, NULL, TO_ROOM );
				}
				else
				{
					act( "Budzisz si�, siadasz w $k i pr�bujesz troch� rozejrze�.", ch, obj, NULL, TO_CHAR );
					act( "$n budzi si� i siada w $k.", ch, obj, NULL, TO_ROOM );
				}
			}

			rp_pos_trigger( ch, obj, &TRIG_SIT );

			if ( ch->position == prev_pos )
				ch->position = POS_SITTING;
			do_function(ch, &do_look, "auto" );
			break;

		case POS_RESTING:
			if ( ch->memming )
			{
				echo_mem( ch, MEM_STOP );
				ch->memming = NULL;
				echo_mem( ch, MEM_STOP_ALL );
			}

			if ( IS_AFFECTED( ch, AFF_RECUPERATE ) )
				affect_strip( ch, gsn_recuperate );

			if ( show )
			{
				if ( obj == NULL )
					send_to_char( "Przestajesz odpoczywa�.\n\r", ch );
				else if ( obj->item_type == ITEM_FURNITURE && IS_SET( obj->value[ 2 ], SIT_AT ) )
				{
					act( "Siadasz przy $k.", ch, obj, NULL, TO_CHAR );
					act( "$n siada przy $k.", ch, obj, NULL, TO_ROOM );
				}
				else if ( obj->item_type == ITEM_FURNITURE && IS_SET( obj->value[ 2 ], SIT_ON ) )
				{
					act( "Siadasz na $k.", ch, obj, NULL, TO_CHAR );
					act( "$n siada na $k.", ch, obj, NULL, TO_ROOM );
				}
				else if ( obj->item_type == ITEM_MUSICAL_INSTRUMENT )
				{
					act( "Siadasz przy $k.", ch, obj, NULL, TO_CHAR );
					act( "$n siada przy $k.", ch, obj, NULL, TO_ROOM );
				}
				else
				{
					act( "Siadasz w $k.", ch, obj, NULL, TO_CHAR );
					act( "$n siada w $k.", ch, obj, NULL, TO_ROOM );
				}
			}

			rp_pos_trigger( ch, obj, &TRIG_SIT );

			if ( ch->position == prev_pos )
				ch->position = POS_SITTING;
			break;

		case POS_SITTING:
			if ( changed_pos )
			{
				if ( show )
				{
					if ( boat ) {
						act( "Siadasz na $k.", ch, boat, NULL, TO_CHAR );
						act( "$n siada na $k.", ch, boat, NULL, TO_ROOM );
					}
					else if ( obj->item_type == ITEM_FURNITURE && IS_SET( obj->value[ 2 ], SIT_AT ) )
					{
						act( "Siadasz przy $k.", ch, obj, NULL, TO_CHAR );
						act( "$n siada przy $k.", ch, obj, NULL, TO_ROOM );
					}
					else if ( obj->item_type == ITEM_FURNITURE && IS_SET( obj->value[ 2 ], SIT_ON ) )
					{
						act( "Siadasz na $k.", ch, obj, NULL, TO_CHAR );
						act( "$n siada na $k.", ch, obj, NULL, TO_ROOM );
					}
					else if ( obj->item_type == ITEM_MUSICAL_INSTRUMENT )
					{
						act( "Siadasz przy $k.", ch, obj, NULL, TO_CHAR );
						act( "$n siada przy $k.", ch, obj, NULL, TO_ROOM );
					}
					else
					{
						act( "Siadasz w $k.", ch, obj, NULL, TO_CHAR );
						act( "$n siada w $k.", ch, obj, NULL, TO_ROOM );
					}
				}

				rp_pos_trigger( ch, obj, &TRIG_SIT );
			}
			else
				send_to_char( "Przecie� ju� siedzisz.\n\r", ch );
			return;

		case POS_STANDING:
			if ( is_affected( ch, gsn_fly ) )
			{
				if ( skill_table[ gsn_fly ].msg_off )
				{
					send_to_char( skill_table[ gsn_fly ].msg_off, ch );
					send_to_char( "\n\r", ch );
				}
				affect_strip( ch, gsn_fly );
			}

			if ( is_affected( ch, gsn_float ) )
			{
				if ( skill_table[ gsn_float ].msg_off )
				{
					send_to_char( skill_table[ gsn_float ].msg_off, ch );
					send_to_char( "\n\r", ch );
				}
				affect_strip( ch, gsn_float );
			}

			if ( show )
			{
				if ( boat ) {
					act( "Siadasz wygodnie na $k.", ch, boat, NULL, TO_CHAR );
					act( "$n siada na $k.", ch, boat, NULL, TO_ROOM );
				}
				else if ( obj == NULL )
				{
					act( "Siadasz wygodnie.", ch, NULL, NULL, TO_CHAR );
					act( "$n siada na ziemi.", ch, NULL, NULL, TO_ROOM );
				}
				else if ( obj->item_type == ITEM_FURNITURE && IS_SET( obj->value[ 2 ], SIT_AT ) )
				{
					act( "Siadasz przy $k.", ch, obj, NULL, TO_CHAR );
					act( "$n siada przy $k.", ch, obj, NULL, TO_ROOM );
				}
				else if ( obj->item_type == ITEM_FURNITURE && IS_SET( obj->value[ 2 ], SIT_ON ) )
				{
					act( "Siadasz na $k.", ch, obj, NULL, TO_CHAR );
					act( "$n siada na $k.", ch, obj, NULL, TO_ROOM );
				}
				else if ( obj->item_type == ITEM_MUSICAL_INSTRUMENT )
				{
					act( "Siadasz przy $k.", ch, obj, NULL, TO_CHAR );
					act( "$n siada przy $k.", ch, obj, NULL, TO_ROOM );
				}
				else
				{
					act( "Siadasz w $k.", ch, obj, NULL, TO_CHAR );
					act( "$n siada w $k.", ch, obj, NULL, TO_ROOM );
				}
			}

			rp_pos_trigger( ch, obj, &TRIG_SIT );

			if ( ch->position == prev_pos )
				ch->position = POS_SITTING;
			break;
	}

	return;
}

void do_sleep( CHAR_DATA *ch, char *argument )
{
	PROG_LIST *prg;
	OBJ_DATA * obj = NULL;
	OBJ_DATA * boat = NULL;
	bool show = TRUE;
	int prev_pos;

	if ( !can_rest(ch, &boat) )
		return;

	if ( argument[ 0 ] != '\0' )
	{
		obj = get_obj_list( ch, argument, ch->in_room->contents );
		if ( obj == NULL )
		{
			send_to_char( "Nie widzisz tego tutaj.\n\r", ch );
			return;
		}
	}
	else obj = ch->on;

	if ( obj != NULL )
	{
		if ( !can_see_obj(ch, obj) )
		{
			send_to_char("Nie widzisz tego tutaj.\n\r", ch);
			return;
		}

		if ( obj->item_type != ITEM_FURNITURE
				|| ( !IS_SET( obj->value[ 2 ], SLEEP_ON )
					&& !IS_SET( obj->value[ 2 ], SLEEP_IN )
					&& !IS_SET( obj->value[ 2 ], SLEEP_AT ) ) )
		{
			send_to_char( "Nie mo�esz u�y� tego do spania!\n\r", ch );
			return;
		}

		if ( obj != NULL && ch->on != obj && count_users( obj ) >= obj->value[ 0 ] )
		{
			if ( IS_SET( obj->value[ 2 ], SLEEP_AT ) )
				act_new( "Nie ma wolnego miejsca przy $k.", ch, obj, NULL, TO_CHAR, POS_DEAD );
			else if ( IS_SET( obj->value[ 2 ], SLEEP_ON ) )
				act_new( "Nie ma wolnego miejsca na $k.", ch, obj, NULL, TO_CHAR, POS_DEAD );
			else
				act_new( "Nie ma wolnego miejsca w $k.", ch, obj, NULL, TO_CHAR, POS_DEAD );
			return;
		}

		ch->on = obj;
	}

	// wstawka z rp_pos_trigger
	if ( !IS_NPC( ch ) && HAS_RTRIGGER( ch->in_room, TRIG_SLEEP ) )
	{
		for ( prg = ch->in_room->progs; prg != NULL; prg = prg->next )
		{
			if ( prg->trig_type != &TRIG_SLEEP )
				continue;
			if ( ( is_number( prg->trig_phrase ) && obj != NULL && ( obj->pIndexData->vnum == atoi( prg->trig_phrase ) ) )
					|| !str_cmp( prg->trig_phrase, "none" ) )
			{
				show = FALSE;
				break;
			}
		}
	}

	prev_pos = ch->position;

	switch ( ch->position )
	{
		case POS_SLEEPING:
			send_to_char( "�ni ci si�, �e k�adziesz si� spa�.\n\r", ch );
			return;

		case POS_RESTING:
			if ( IS_AFFECTED( ch, AFF_RECUPERATE ) )
				affect_strip( ch, gsn_recuperate );
			if ( ch->memming )
			{
				echo_mem( ch, MEM_STOP );
				ch->memming = NULL;
				echo_mem( ch, MEM_STOP_ALL );
			}

		case POS_SITTING:
		case POS_STANDING:
			if ( is_affected( ch, gsn_fly ) )
			{
				if ( skill_table[ gsn_fly ].msg_off )
				{
					send_to_char( skill_table[ gsn_fly ].msg_off, ch );
					send_to_char( "\n\r", ch );
				}
				affect_strip( ch, gsn_fly );
			}

			if ( is_affected( ch, gsn_float ) )
			{
				if ( skill_table[ gsn_float ].msg_off )
				{
					send_to_char( skill_table[ gsn_float ].msg_off, ch );
					send_to_char( "\n\r", ch );
				}
				affect_strip( ch, gsn_float );
			}

			if ( show )
			{
				if ( obj == NULL )
				{
					send_to_char( "K�adziesz si� spa�.\n\r", ch );
					act( "$n k�adzie si� spa�.", ch, NULL, NULL, TO_ROOM );
				}
				else if ( boat ) {
					act( "K�adziesz si� spa� na $k.", ch, boat, NULL, TO_CHAR );
					act( "$n k�adzie si� spa� na $k.", ch, boat, NULL, TO_ROOM );
				}
				else if ( IS_SET( obj->value[ 2 ], SLEEP_AT ) )
				{
					act( "K�adziesz si� spa� przy $k.", ch, obj, NULL, TO_CHAR );
					act( "$n k�adzie si� spa� przy $k.", ch, obj, NULL, TO_ROOM );
				}
				else if ( IS_SET( obj->value[ 2 ], SLEEP_ON ) )
				{
					act( "K�adziesz si� spa� na $k.", ch, obj, NULL, TO_CHAR );
					act( "$n k�adzie si� spa� na $k.", ch, obj, NULL, TO_ROOM );
				}
				else
				{
					act( "K�adziesz si� spa� w $k.", ch, obj, NULL, TO_CHAR );
					act( "$n k�adzie si� spa� w $k.", ch, obj, NULL, TO_ROOM );
				}
			}
			rp_pos_trigger( ch, obj, &TRIG_SLEEP );

			if ( ch->position == prev_pos )
				ch->position = POS_SLEEPING;
			break;

		case POS_FIGHTING:
			send_to_char( "Walcz, p�niej sobie po�pisz!\n\r", ch );
			return;
	}

	return;
}

void do_wake( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	CHAR_DATA *victim;

	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		if ( IS_AWAKE( ch ) )
			send_to_char( "Przecie� nie �pisz.\n\r", ch );
		else
			do_function( ch, &do_sit, "" );
		return;
	}

	if ( !IS_AWAKE( ch ) )
	{
		send_to_char( "�ni ci si�, �e kogo� budzisz!\n\r", ch );
		return;
	}

	if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{
		send_to_char( "Nie widzisz tu nikogo takiego.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( victim, AFF_PARALYZE ) )
	{
		act( "$N nie mo�e si� poruszy�.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( IS_AWAKE( victim ) )
	{
		act( "$N nie �pi.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( IS_AFFECTED( victim, AFF_SLEEP ) || ( !IS_NPC( ch ) && ch->pcdata->mind_in ) )
	{
		act( "Nie uda�o ci si� obudzic $Z!", ch, NULL, victim, TO_CHAR );
		return;
	}

	if( victim->sex == SEX_FEMALE )
	{
		act( "Szturchasz $C pr�buj�c j� obudzi�.", ch, NULL, victim, TO_CHAR );
		act( "$n szturcha $C pr�buj�c j� obudzi�.", ch, NULL, victim, TO_NOTVICT );
	}
	else
	{
		act( "Szturchasz $C pr�buj�c go obudzi�.", ch, NULL, victim, TO_CHAR );
		act( "$n szturcha $C pr�buj�c go obudzi�.", ch, NULL, victim, TO_NOTVICT );
	}

	act_new( "$n budzi ci�.", ch, NULL, victim, TO_VICT, POS_SLEEPING );

	if ( GOT_PART( ch, PART_LEGS ) )
		do_function( victim, &do_sit, "" );
	else
		do_function( victim, &do_stand, "" );
	return;
}

void do_unsneak( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
    {
        return;
    }

    if( !IS_AFFECTED( ch, AFF_SNEAK ) )
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

    if( ch->fighting )
	{
		send_to_char("Lepiej skup si� na walce.\n\r", ch);
		return;
	}

    if ( IS_AFFECTED( ch, AFF_SNEAK ) )
    {
        EXT_REMOVE_BIT( ch->affected_by, AFF_SNEAK );
		affect_strip( ch, gsn_sneak );
        send_to_char( "Przestajesz si� skrada�.\n\r", ch );
    }
}

void do_sneak( CHAR_DATA *ch, char *argument )
{
	AFFECT_DATA af;
	OBJ_DATA *obj;
	int skill, iWear;

	if( ( skill = get_skill(ch,gsn_sneak) ) <= 0 )
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if( ch->fighting )
	{
		send_to_char("Lepiej skup si� na walce.\n\r", ch);
		return;
	}

	if ( ch->mount )
	{
		send_to_char( "Trudno b�dzie ci si� bezszelestnie porusza� siedz�c na wierzchowcu.\n\r", ch );
		return;
	}

    if ( IS_AFFECTED( ch, AFF_BLIND ) && !IS_AFFECTED( ch, AFF_PERFECT_SENSES ) )
   {
        send_to_char( "Nic nie widzisz, nie dasz rady si� skrada�.\n\r", ch );
        return;
   }

	if ( IS_AFFECTED( ch, AFF_SNEAK ) )
	{
		switch ( number_range( 0, 5 ) )
		{
			case 0:
				send_to_char( "Skradasz si� jak tylko potrafisz.\n\r", ch );
				break;
			case 1:
				send_to_char( "Stawiasz kroki lekkie jak wiatr, nikt nie powinien ci� us�ysze�.\n\r", ch );
				break;
			case 2:
				send_to_char( "No skradasz, si� skradasz... ca�y czas.\n\r", ch );
				break;
			case 3:
				send_to_char( "Bardziej si� skrada� to si� ju� nie da.\n\r", ch );
				break;
			case 4:
				send_to_char( "Przecie� poruszasz si� bezszelestnie.\n\r", ch );
				break;
			default:
				send_to_char( "Ca�y czas si� skradasz.\n\r", ch );
				break;
		}
		return;
	}

	if ( ch->class == CLASS_BARD )
	{
		for ( iWear = 0; iWear < MAX_WEAR - 1; iWear++ )
		{
			if ( ( obj = get_eq_char( ch, iWear ) ) == NULL )
				continue;

			if ( obj->item_type != ITEM_ARMOR )
				continue;

			if ( iWear != WEAR_HEAD && iWear != WEAR_BODY &&
					iWear != WEAR_LEGS && iWear != WEAR_FEET &&
					iWear != WEAR_HANDS && iWear != WEAR_ARMS )
				continue;

			if ( obj->value[4] != 0 && obj->value[4] != 1 && obj->value[4] != 2 && obj->value[4] != 3 && obj->value[4] != 6 )
			{
				send_to_char( "Twoja zbroja zbyt ci� uwiera.\n\r", ch );
				return;
			}
		}
	}

    /**
     * ze wzgl�du na to �e senak szkoli si� podczas u�ywania, bazowa
     * szansa nie mo�e by� na poziomie 1% - bo to frustruj�ce i bezsensowne
     */
    skill += URANGE( 0, ch->level, 20 );
    skill = URANGE( 20, skill, 95 );
    /**
     * tniemy barda o 25%
     * r�wnie� szans� pocz�tkow�
     */
    if (ch->class == CLASS_BARD )
    {
        skill -= skill/4;
    }
    /**
     * sprawdzamy czy si� uda�o
     */
    if ( number_percent() < skill )
    {
        /**
         * komunikujemy graczowi fakt wykonania
         */
        send_to_char( "Pr�bujesz porusza� si� bezszelestnie.\n\r", ch );
        /**
         * komunikujemy otoczeniu
         * TODO: przelecie� si� po PC w lokacji i doda� indywidualne
         *       szansy na zorientowanie si� o braku dzwi�ku
         */
        if ( number_percent() < URANGE( 5, 40 - skill / 3, 30 ) )
        {
            act( "Nagle przestajesz s�ysze� ruchy $z.", ch, NULL, NULL, TO_ROOM );
        }
        af.where       = TO_AFFECTS;
        af.type        = gsn_sneak;
        af.level       = ch->level;
        af.duration    = 5 + 2 * ch->level;
        af.rt_duration = 0;
        af.location    = APPLY_NONE;
        af.modifier    = 0;
        af.bitvector   = &AFF_SNEAK;
        affect_to_char( ch, &af, NULL, TRUE );
	}
    else
    {
        send_to_char( "Pr�bujesz porusza� si� bezszelestnie, ale chyba nic ci nie wychodzi.\n\r", ch );
    }
    WAIT_STATE( ch, 11 - skill / 10 );
    return;
}

void do_hide( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *obj;
	int skill, iWear;

	if( ( skill = get_skill(ch,gsn_hide) ) < 1 )
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	if( ch->fighting )
	{
		send_to_char("Lepiej skup si� na walce.\n\r", ch);
		return;
	}

	if ( IS_AFFECTED( ch, AFF_BLIND ) && !IS_AFFECTED( ch, AFF_PERFECT_SENSES ) )
	{
	      send_to_char( "Nic nie widzisz, nie dasz rady si� schowa�.\n\r", ch );
	      return;
	}

	if ( IS_AFFECTED(ch, AFF_HIDE) )
	{
		switch (number_range( 0, 3))
		{
			case 0:
				send_to_char( "Teraz jeste� do�� dobrze ukryt<&y/a/e>.\n\r", ch );
				break;
			case 1:
				send_to_char( "Lepiej si� tutaj ju� nie schowasz.\n\r", ch );
				break;
			case 2:
				send_to_char( "Wydaje ci si� �e jeste� ju� dobrze ukryt<&y/a/e>.\n\r", ch );
				break;
			default:
				send_to_char( "Chyba znalaz�<&e/a/o>� sobie odpowiedni� kryj�wk�.\n\r", ch );
				break;
		}
		return;
	}

	if ( ch->mount )
	{
		send_to_char( "Trudno b�dzie ci si� ukry� siedz�c na wierzchowcu.\n\r", ch );
		return;
	}

	if ( ch->move < 1 )
	{
		send_to_char( "Nie za bardzo masz si�� �eby si� ukry�.\n\r", ch );
		return;
	}

	if (ch->class == CLASS_BARD )
	{
		for ( iWear = 0; iWear < MAX_WEAR - 1; iWear++ )
		{
			if ( ( obj = get_eq_char( ch, iWear ) ) == NULL )
				continue;

			if ( obj->item_type != ITEM_ARMOR )
				continue;

			if ( iWear != WEAR_HEAD && iWear != WEAR_BODY &&
					iWear != WEAR_LEGS && iWear != WEAR_FEET &&
					iWear != WEAR_HANDS && iWear != WEAR_ARMS )
				continue;

			if ( obj->value[4] != 0 && obj->value[4] != 1 && obj->value[4] != 2 && obj->value[4] != 3 && obj->value[4] != 6 )
			{
				send_to_char( "Twoja zbroja zbyt ci� uwiera.\n\r", ch );
				return;
			}
		}
		skill -= skill/4;
	}

	ch->move -= number_range(1,4);
	if (ch->move < 0)
	{
		ch->move = 0;
	}

	if( EXT_IS_SET( ch->in_room->room_flags, ROOM_NOHIDE ) )
	{
		print_char(ch,"Nie bardzo masz jak si� ukry�.\n\r");
		return;
	}

	switch (ch->in_room->sector_type)
	{
		case  0: // wewn�trz
		case 14: // podziemia
		case 15: // jaskinia
		case 17: // podziemia naturalne
			send_to_char( "Bez k�opotu znajdujesz zacienione miejsce, w kt�rym pr�bujesz si� ukry�.\n\r", ch );
			skill += 15;
			break;

		case  1: // miasto
		case 36: // plac
		case 38: // park
			send_to_char( "W zgie�ku miasta by� mo�e uda ci si� ukry� bez �adnych k�opot�w.\n\r", ch );
			skill += 10;
			break;

		case  3: // las
		case 11: // puszcza
		case 16: // tundra
			send_to_char( "W�r�d drzew i krzew�w szukasz miejsca, zeby si� ukry�.\n\r", ch );
			break;

		case 12: // bagno
			send_to_char( "Mi�kke pod�o�e znacz�co utrudnia ci pr�be ukrycia.\n\r", ch );
			skill -= 10;
			break;

		case  4: // wzg�rza
		case  5: // g�ry
		case 13: // wysokie g�ry
		case 35: // g�rska �cie�ka
		case 43: // stroma �cie�ka
			send_to_char( "Twarde pod�o�e i wsz�dobylskie skalne od�amki utrudniani� ukrywanie si�.\n\r", ch );
			skill -= 5;
			break;

		case 18: // droga
		case 28: // �cie�ka
			send_to_char( "Bardzo trudno b�dzie ci si� ukry� w tym miejscu.\n\r", ch );
			skill /= 2;
			break;

		case 23: // lawa
			send_to_char( "Przypala ci� lawa i chyba nie dasz rady si� ukry�.\n\r", ch );
			skill /= 2;
			break;

		case 24: // ruchome piaski
		case 30: // eden
			send_to_char( "Pr�bujesz si� ukry�, ale tutaj jest to bardzo trudne.\n\r", ch );
			skill -= 10;
			break;

		case 26: // lodowiec
		case 27: // arktyczny l�d
			send_to_char( "Pr�bujesz si� ukry�, ale jest tu tak bia�o, �e b�dzie to bardzo trudne.\n\r", ch );
			skill -= 10;
			break;


		case 10: // pustynia
		case 29: // pla�a
		case 40: // wydmy
			send_to_char( "Pr�bujesz ukry� si� w�r�d wydm.\n\r", ch );
			break;

		case  2: // pole
		case 31: // sawanna
		case 32: // trawa
		case 33: // step
		case 42: // ��ka
			send_to_char( "Pr�bujesz ukry� si� w�r�d r�znych ro�lin.\n\r", ch );
			break;

		case 44: // b�otna �cie�ka
			send_to_char( "Pr�bujesz si� ukry�, ale �lizgasz si� po pod�o�u i chyba ci si� nie uda.\n\r", ch );
			skill -= 10;
			break;

		case  6: // woda p�ywalna
		case  7: // woda niep�ywalna
		case  8: // nieu�ywany
		case  9: // powietrze
		case 19: // rzeka
		case 20: // jezioro
		case 21: // morze
		case 22: // ocean
		case 25: // gor�ce �r�d�a
		case 34: // podwod�
		case 37: // ciemna woda
		case 39: // podziemne jezioro
		case 41: // arena
			send_to_char( "Pr�bujesz si� ukry�, ale nie bardzo masz jak.\n\r", ch );
			return;

		default:
			send_to_char( "Pr�bujesz si� ukry�.\n\r", ch );
			break;
	}

	if ( number_percent( ) < skill)
	{
		EXT_SET_BIT( ch->affected_by, AFF_HIDE);
		WAIT_STATE(ch, 12);
	}
    else
    {
		WAIT_STATE(ch, 12 - skill / 10 );
    }

	return;
}

/*
 * Contributed by Alander.
 */
void do_visible( CHAR_DATA *ch, char *argument )
{
	if (ch->position == POS_SLEEPING)
	{
		send_to_char("�nisz o rzeczach widzialnych i niewidzialnych.\n\r", ch );
		return;
	}

	if (!((IS_AFFECTED(ch, AFF_INVISIBLE) || IS_AFFECTED(ch, AFF_UNDEAD_INVIS) || IS_AFFECTED(ch, AFF_ANIMAL_INVIS))))
	{
		send_to_char("Przecie� nie jeste� niewidzialn<&y/a/e>.\n\r", ch );
		return;
	}

	if (IS_AFFECTED(ch, AFF_INVISIBLE))
	{
		send_to_char( "Przestajesz by� niewidzialn<&y/a/e>.\n\r", ch );
	}

	if (IS_AFFECTED(ch, AFF_ANIMAL_INVIS))
	{
		send_to_char( "Przestajesz by� niewidzialn<&y/a/e> dla zwierz�t.\n\r", ch );
	}

	if (IS_AFFECTED(ch, AFF_UNDEAD_INVIS))
	{
		send_to_char( "Przestajesz by� niewidzialn<&y/a/e> dla nieumar�ych.\n\r", ch );
	}
	strip_invis( ch, FALSE, TRUE );
	act( "$n materializuje si� powoli.", ch, NULL, NULL, TO_ROOM );
	return;
}

int go_door( CHAR_DATA *ch, char *argument, bool verbose )
{
	EXIT_DATA *pexit;
	int door = -1;
	int iter;

	door = get_door( ch, argument );

	if ( door >= 0 && check_vname( ch, door, verbose ) )
		return -1;

	if ( door >= 0 )
		return door;
	else
	{
		door = -1;
		for ( iter = 0; iter <= 5; iter++ )
		{
			if ( ( pexit = ch->in_room->exit[iter] ) != NULL
					&&  pexit->vName != NULL
					&&  !str_prefix( argument, pexit->vName ) )
			{
				door = iter;
				break;
			}
		}
	}

	if ( door < 0 || ch->in_room->exit[door] == NULL )
	{
		if ( verbose )
        {
			send_to_char( "Nie mo�esz i�� w tym kierunku.\n\r", ch );
        }
		return -1;
	}

	return door;
}

void do_go( CHAR_DATA *ch, char *argument )
{
	int gdoor;
	gdoor = go_door( ch, argument, TRUE );

	if ( gdoor >= 0 )
		move_char( ch, gdoor, FALSE, NULL );
}

void send_to_char_fail_open_lock(CHAR_DATA *ch)
{
	switch ( number_range( 1, 12 ) )
	{
		case 1:
			send_to_char( "Majstrujesz przy zamku, jednak twoje wysi�ki spe�zaj� na niczym.\n\r", ch );
			break;
		case 2:
			send_to_char( "Si�gasz wytrychem w g��b mechanizmu, jednak nie udaje ci si� go otworzy�.\n\r", ch );
			break;
		case 3:
			send_to_char( "Grzebiesz, grzebiesz, ale niczego nie uda�o ci si� osi�gn��.\n\r", ch );
			break;
		case 4:
			send_to_char( "Wyginasz wytrych w lewo, potem w prawo, ale nic si� nie dzieje.\n\r", ch );
			break;
		case 5:
			send_to_char( "Ogl�dasz dok�adnie zamek, kt�rego nie uda�o ci si� otworzy�.\n\r", ch );
			break;
		case 6:
			switch (ch->sex)
			{
				case SEX_NEUTRAL:
					send_to_char( "Niczego nie osi�gn�o� i rozczarowane wyci�gasz wytrych z zamka.\n\r", ch );
					break;
				case SEX_MALE:
					//http://www.sjp.pl/co/osi%B1gn%B1%E6
					send_to_char( "Niczego nie osi�gn��e� i rozczarowany wyci�gasz wytrych z zamka.\n\r", ch );
					break;
				case SEX_FEMALE:
					send_to_char( "Niczego nie osi�gn�a� i rozczarowana wyci�gasz wytrych z zamka.\n\r", ch );
					break;
			}
			break;
		case 7:
			send_to_char( "Ten zamek jest chyba zbyt skomplikowany �eby uda�o ci si� go otworzy�.\n\r", ch );
			break;
		case 8:
			switch (ch->sex)
			{
				case SEX_NEUTRAL:
					send_to_char( "Zamek kt�ry pr�bowa�o� otworzy� pozostaje niepokonany.\n\r", ch );
					break;
				case SEX_MALE:
					send_to_char( "Zamek kt�ry pr�bowa�e� otworzy� pozostaje niepokonany.\n\r", ch );
					break;
				case SEX_FEMALE:
					send_to_char( "Zamek kt�ry pr�bowa�a� otworzy� pozostaje niepokonany.\n\r", ch );
					break;
			}
			break;
		case 9:
			send_to_char( "Kombinujesz z tym zamkiem, ale niczego nie udaje ci si� osi�gn��.\n\r", ch );
			break;
		case 10:
			send_to_char( "Co� stukocze i puka, ale opr�cz tego nic si� nie dzieje.\n\r", ch );
			break;
		case 11:
			send_to_char( "Wciskasz wytrych, ale ten zgina si� na nieoczekiwanej przez ciebie przeszkodzie.\n\r", ch );
			break;
		default:
			send_to_char( "Nie uda�o ci si�.\n\r", ch );
			break;
	}
}

bool mind_move_room( CHAR_DATA * ch, ROOM_INDEX_DATA * pRoomIndex )
{
	AFFECT_DATA * aff;
	if ( IS_NPC( ch ) || !IS_AFFECTED( ch, AFF_ASTRAL_JOURNEY) )
		return FALSE;

	if ( ch->pcdata->mind_in )
	{
		if ( ch == ch->pcdata->mind_in->minds )
		{
			ch->pcdata->mind_in->minds = ch->next_mind_in_room;
		}
		else
		{
			CHAR_DATA *prev;

			for ( prev = ch->pcdata->mind_in->minds; prev; prev = prev->next_mind_in_room )
			{
				if ( prev->next_mind_in_room == ch )
				{
					prev->next_mind_in_room = ch->next_mind_in_room;
					break;
				}
			}
		}
	}

	aff = affect_find( ch->affected, gsn_astral_journey );
	if ( aff )
		aff->level--;

	//je�li podano Room i ma affecta i si� nie wyczerpal to wrzucamy go do tego rooma
	if ( pRoomIndex && aff && aff->level > 0 )
	{
		ch->pcdata->mind_in		= pRoomIndex;
		ch->next_mind_in_room	= pRoomIndex->minds;
		pRoomIndex->minds		= ch;
		ch->position			= POS_SLEEPING;
		return TRUE;
	}
	else
	{
		ch->pcdata->mind_in = NULL;
		ch->next_mind_in_room = NULL;
		send_to_char( "Tw�j umys� powraca z powrotem do cia�a.\n\r", ch );
		if ( aff )
			affect_remove( ch, aff );
		return FALSE;
	}
}
