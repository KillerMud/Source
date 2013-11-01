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
 * KILLER MUD is copyright 1999-2011 Killer MUD Staff (alphabetical)   *
 *                                                                     *
 * Andrzejczak Dominik   (kainti@go2.pl                 ) [Kainti    ] *
 * Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor     ] *
 * Koper Tadeusz         (jediloop@go2.pl               ) [Garloop   ] *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Skrzetnicki Krzysztof (tener@tenet.pl                ) [Tener     ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: tricks.c 155 2013-08-08 20:50:23Z drake $
 * $HeadURL: svn+ssh://svn@killer.mud.pl/killer/branches/current/src/tricks.c $
 *
 */
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "progs.h"
#include "lang.h"
#include "music.h"
#include "money.h"

int		calculate_spec_damage args ( ( CHAR_DATA *ch, CHAR_DATA *vch, OBJ_DATA *weapon, int spec_type ) );
void	raw_damage		args( (CHAR_DATA *ch,CHAR_DATA *victim,int dam) );
bool	check_increase_wounds	args( (CHAR_DATA *victim, int dam_type, int *dam) );
void	one_hit	 args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt, bool second ) );
void wear_obj_silent args( ( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace ) );
void wield_weapon_silent args( ( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace ) );

bool check_trick( CHAR_DATA *ch, CHAR_DATA *victim, int sn )
{
	if( !ch || !victim || sn <= 0 )
		return FALSE;

	if( IS_NPC(ch) )
		return FALSE;

	if( !ch->pcdata->tricks[sn] )
		return FALSE;

	if( number_range( 1, 1000 ) > trick_table[ sn ].chance )
		return FALSE;

	( *trick_table[ sn ].trick_fun ) ( sn, ch, victim );

	return TRUE;
}

void do_tricks( CHAR_DATA *ch, char *argument )
{
	int sn;
	DESCRIPTOR_DATA *d;
	CHAR_DATA *wch;


	if( IS_NPC(ch) )
		return;

	//pierw immo komendy
	if( IS_IMMORTAL( ch ) )
		send_to_char( "Masz dostepne komendy 'trick clear', 'trick set', 'trick lookup'.\n\r", ch );

	if ( IS_IMMORTAL( ch ) && !str_cmp( argument, "clear" ) )
	{
		for ( sn = 0; sn < MAX_TRICKS; sn++ )
			ch->pcdata->tricks[sn] = 0;
		send_to_char( "Nie masz trikow.\n\r", ch );
		return;
	}

	if ( IS_IMMORTAL( ch ) && !str_cmp( argument, "set" ) )
	{
		for ( sn = 0; sn < MAX_TRICKS; sn++ )
			ch->pcdata->tricks[sn] = 1;
		send_to_char( "Masz wszystkie triki.\n\r", ch );
		return;
	}

	if ( IS_IMMORTAL( ch ) && !str_cmp( argument, "lookup" ) )
	{
		send_to_char( "B�d�cy w grze gracze znaj� nast�puj�ce triki:\n\r", ch );
		for ( d = descriptor_list; d != NULL; d = d->next )
		{

        	if ( !d->character || d->connected != CON_PLAYING )
        		continue;

        	wch = ( d->original != NULL ) ? d->original : d->character;

        	if ( IS_IMMORTAL( wch ) )
        		continue;

			for ( sn = 0; sn < MAX_TRICKS; sn++ )
			{
				if ( trick_table[ sn ].name == NULL )
					break;

				if ( wch->pcdata->tricks[sn] )
				{
					if( trick_table[ sn ].which_skill2 == -1 && trick_table[ sn ].alternate_skill1 == -1 )
						print_char( ch, "%s, %s na %d lewie, zna %s, trik do umiej�tno�ci %s.\n\r", wch->name, get_class_name(wch), wch->level, trick_table[ sn ].name, skill_table[ trick_table[sn].which_skill ].name );
					else if( trick_table[ sn ].which_skill2 != -1 && trick_table[ sn ].alternate_skill1 == -1 )
						print_char( ch, "%s, %s na %d lewie, zna %s, trik do umiej�tno�ci %s i %s.\n\r", wch->name, get_class_name(wch), wch->level, trick_table[ sn ].name, skill_table[ trick_table[sn].which_skill ].name, skill_table[ trick_table[sn].which_skill2 ].name );
					else if( trick_table[ sn ].which_skill2 == -1 && trick_table[ sn ].alternate_skill1 != -1 )
						print_char( ch, "%s, %s na %d lewie, zna %s, trik do umiej�tno�ci %s lub %s.\n\r", wch->name, get_class_name(wch), wch->level, trick_table[ sn ].name, skill_table[ trick_table[sn].which_skill ].name, skill_table[ trick_table[sn].alternate_skill1 ].name );
					else
						print_char( ch, "%s, %s na %d lewie, zna %s, trik do umiej�tno�ci %s lub %s, oraz %s.\n\r", wch->name, get_class_name(wch), wch->level, trick_table[ sn ].name, skill_table[ trick_table[sn].which_skill ].name, skill_table[ trick_table[sn].alternate_skill1 ].name, skill_table[ trick_table[sn].which_skill2 ].name );
					continue;
				}
			}
        }
        send_to_char( "W celu sprawdzenia nauczycieli trick�w wpisz 'ltstat'.\n\r", ch );
        return;
	}


	if ( ch->position == POS_SLEEPING )
	{
		send_to_char( "�ni ci si�, �e pozna�<&e�/a�/o�> wszystkie istniej�ce tricki.\n\r", ch );
		return;
	}

	send_to_char("\n\r{b==<>==<>==<>==<>==<>==<>==<> {GZnane ci triki {b<>==<>==<>==<>==<>==<>==<>==<>==<>=={x\n\r\n\r",ch);

	for ( sn = 0; sn < MAX_TRICKS; sn++ )
	{
		if ( trick_table[ sn ].name == NULL )
			break;

		if ( ch->pcdata->tricks[sn] )
		{
			if( trick_table[ sn ].which_skill2 == -1 && trick_table[ sn ].alternate_skill1 == -1 )
				print_char( ch, "Znasz %s, trik do umiej�tno�ci %s.\n\r", trick_table[ sn ].name, skill_table[ trick_table[sn].which_skill ].name );
			else if( trick_table[ sn ].which_skill2 != -1 && trick_table[ sn ].alternate_skill1 == -1 )
				print_char( ch, "Znasz %s, trik do umiej�tno�ci %s i %s.\n\r", trick_table[ sn ].name, skill_table[ trick_table[sn].which_skill ].name, skill_table[ trick_table[sn].which_skill2 ].name );
			else if( trick_table[ sn ].which_skill2 == -1 && trick_table[ sn ].alternate_skill1 != -1 )
				print_char( ch, "Znasz %s, trik do umiej�tno�ci %s lub %s.\n\r", trick_table[ sn ].name, skill_table[ trick_table[sn].which_skill ].name, skill_table[ trick_table[sn].alternate_skill1 ].name );
			else
				print_char( ch, "Znasz %s, trik do umiej�tno�ci %s lub %s, oraz.\n\r", trick_table[ sn ].name, skill_table[ trick_table[sn].which_skill ].name, skill_table[ trick_table[sn].alternate_skill1 ].name, skill_table[ trick_table[sn].which_skill2 ].name );
			continue;
		}


/*		if ( get_skill( ch, trick_table[ sn ].which_skill ) < trick_table[ sn ].skill_percent )
		{
			print_char( ch, "Nie masz skilla wystarczajaco wytrenowanego by znac %s.\n\r", trick_table[ sn ].name );
			continue;
		}

		if ( ch->pcdata->tricks[sn] == 0 )
		{
			print_char( ch, "Nie znasz %s.\n\r", trick_table[ sn ].name );
			continue;
		}
		else
		{
			print_char( ch, "Znasz %s.\n\r", trick_table[ sn ].name );
			continue;
		}*/
	}

	send_to_char("\n\r{b==<>==<>==<>==<>==<>==<>==<>==<>==<>==<=>==<>==<>==<>==<>==<>==<>==<>==<>==<>=={x\n\r",ch);

}

void trick_null( int sn, CHAR_DATA *ch, CHAR_DATA *victim )
{
	send_to_char( "Niestety, to nie jest �aden trik!\n\r", ch );
	return ;
}

void trick_vertical_kick( int sn, CHAR_DATA *ch, CHAR_DATA *victim )
{
	int dam;
	OBJ_DATA *butki;

	if( 4*(ch->height)/3 < victim->height )
	{
		act("{vPodnosisz wysoko sw� praw� nog�, celuj�c w g�ow� $Z, jednak przeciwnik jest na to zbyt wysoki, tracisz r�wnowag� i przewracasz si�.{x", ch, NULL, victim, TO_CHAR );
		act("{v$n podnosi wysoko sw� praw� nog�, celuj�c najwyra�niej w tw� g�ow�, na szcz�cie ma na to za kr�tkie n�ki, traci r�wnowag� i przewraca si�.{x", ch, NULL, victim, TO_VICT );
		act("{v$n podnosi wysoko sw� praw� nog�, celuj�c najwyra�niej w g�ow� $Z, jednak potencjalna ofiara jest na to zbyt wysoka, $n traci r�wnowag� i przewraca si�.{x", ch, NULL, victim, TO_VICT );
		damage( ch, victim, 0, gsn_kick, DAM_BASH, FALSE );
		if ( ch->position > POS_SITTING )
			ch->position = POS_SITTING;
		WAIT_STATE( ch, 20 );
		return;
	}

	//pierwsze kopniecie
	dam = dice( 5, 8 );
	dam += GET_DAMROLL( ch, NULL );

	//bez butko tylko 75% dama
	if ( ( butki = get_eq_char( ch, WEAR_FEET ) ) == NULL )
	{
		dam = ( dam * 75 ) / 100;
	}
	else
	{
		if ( IS_SET( material_table[ butki->material ].flag, MAT_METAL ) )
			dam += number_range( 1, 6 );
	}

	if( victim->sex == SEX_FEMALE )
	{
		act("{vJednym szybkim ruchem wypychasz swoj� praw� nog� wysoko w g�r� trafiaj�c $C w podbr�dek i wyrzucaj�c j� wysoko w powietrze.{x", ch, NULL, victim, TO_CHAR );
		act("{v$n szybkim ruchem wypycha swoj� praw� nog� wysoko w g�r� trafiaj�c $C w podbr�dek i wyrzucaj�c j� wysoko w powietrze.{x", ch, NULL, victim, TO_NOTVICT );
	}
	else
	{
		act("{vJednym szybkim ruchem wypychasz swoj� praw� nog� wysoko w g�r� trafiaj�c $C w podbr�dek i wyrzucaj�c go wysoko w powietrze.{x", ch, NULL, victim, TO_CHAR );
		act("{v$n szybkim ruchem wypycha swoj� praw� nog� wysoko w g�r� trafiaj�c $C w podbr�dek i wyrzucaj�c go wysoko w powietrze.{x", ch, NULL, victim, TO_NOTVICT );
	}
	act("{v$n niespodziewanym ruchem wypycha swoj� praw� nog� wysoko w g�r� trafiaj�c ci� prosto w podbr�dek i wyrzuca w powietrze! Co� bardzo niebezpiecznie chrupn�o ci w szyi...{x", ch, NULL, victim, TO_VICT );

	damage( ch, victim, dam, gsn_kick, DAM_BASH, FALSE );

	//jak przezyl to dalej
	if( !victim || !victim->in_room )
	{
		WAIT_STATE( ch, 24);
		return;
	}

	dam = dice( 7, 7 );
	dam += GET_DAMROLL( ch, NULL );

	//bez butko tylko 75% dama
	if ( ( butki = get_eq_char( ch, WEAR_FEET ) ) == NULL )
	{
		dam = ( dam * 75 ) / 100;
	}
	else
	{
		if ( IS_SET( material_table[ butki->material ].flag, MAT_METAL ) )
			dam += number_range( 1, 6 );
	}


	if( victim->sex == SEX_FEMALE )
	{
		act("{vB�yskawicznie doskakujesz do opadaj�cej $Z, a twoja wracaj�ca z niezwyk�� szybko�ci� noga wbija j� w ziemi�. $N wydaje z siebie bolesny j�k, a z jej ust wyp�ywa czerwona piana.{x", ch, NULL, victim, TO_CHAR );
		act("{v$n b�yskawicznie doskakuje do opadaj�cej $Z, a $s wracaj�ca z niezwyk�� szybko�ci� noga wbija j� w ziemi�. $N wydaje z siebie bolesny j�k, a z jej ust wyp�ywa czerwona piana.{x", ch, NULL, victim, TO_NOTVICT );
	}
	else
	{
		act("{vB�yskawicznie doskakujesz do opadaj�cego $Z, a twoja wracaj�ca z niezwyk�� szybko�ci� noga wbija go w ziemi�. $N wydaje z siebie bolesny j�k, a z jego ust wyp�ywa czerwona piana.{x", ch, NULL, victim, TO_CHAR );
		act("{v$n b�yskawicznie doskakuje do opadaj�cego $Z, a $s wracaj�ca z niezwyk�� szybko�ci� noga wbija go w ziemi�. $N wydaje z siebie bolesny j�k, a z jego ust wyp�ywa czerwona piana.{x", ch, NULL, victim, TO_NOTVICT );
	}
	act("{vZanim jeszcze uderzy�<&e�/a�/o�> o grunt noga $z z niezwyk�� pr�dko�ci� spada na twoj� pier� i praktycznie wbija ci� w ziemi�! Wydajesz z siebie bole�ny j�k, a widoczno�� zaczyna ci zas�ania� wyp�ywaj�ca z twych ust czerwona piana.{x", ch, NULL, victim, TO_VICT );

	damage( ch, victim, dam, gsn_kick, DAM_BASH, FALSE );

	WAIT_STATE( ch, 24);

	if( !victim || !victim->in_room )
		return;

	if ( IS_AFFECTED( victim, AFF_STABILITY ) )
	{
		act("{vNagle jaka� niewidzialna si�a podnosi $c i stawia na nogi.{x", victim, NULL, NULL, TO_ROOM );
		act("{vNagle niewidzialna si�a podnosi ci� i stawia na nogi.{x", victim, NULL, NULL, TO_CHAR );
		WAIT_STATE( victim, 12 );
	}
	else
	{
		WAIT_STATE( victim, 36 );
		victim->position = POS_SITTING;
	}

	return ;
}

void trick_entwine( int sn, CHAR_DATA *ch, CHAR_DATA *victim )
{
	OBJ_DATA *whip;

	whip = get_eq_char( ch, WEAR_WIELD );

	if( IS_SET(ch->fight_data, FIGHT_TRICK_WIELDSECOND ) )
		whip = get_eq_char( ch, WEAR_SECOND );

	if( !whip )
		return;

	if( whip->item_type != ITEM_WEAPON || whip->value[0] != WEAPON_WHIP )
	{
		whip = get_eq_char( ch, WEAR_SECOND );
		if( whip->item_type != ITEM_WEAPON || whip->value[0] != WEAPON_WHIP )
		{
			bugf( "Tricks: %s odpalil si� trik entwine mimo ze nie ma whipow zadnych.", ch->name );
			return;
		}
	}

	if( victim->size - 1 > ch->size )
	{
		act("{vZr�cznie okr�casz $h wok� nogi $Z i ci�gniesz z ca�ej si�y, jednak przeciwnik jest zbyt wielki by uda�o ci si� go w ten spos�b przewr�ci�.{x", ch, whip, victim, TO_CHAR );
		act("{v$n zr�cznie okr�ca $h wok� twojej nogi i ci�gnie z ca�ej si�y, na szcz�cie nie jest ona wystarczaj�ca by wywr�ci� kogo� tak du�ego jak ty.{x", ch, whip, victim, TO_VICT );
		act("{v$n zr�cznie okr�ca $h wok� nogi $Z i ci�gnie z ca�ej si�y, jednak $s przeciwnik jest zbyt wielki by ta sztuczka si� uda�a.{x", ch, whip, victim, TO_NOTVICT );
		return;
	}

	act("{vZr�cznie okr�casz $h wok� nogi $Z i ci�gniesz z ca�ej si�y. $N z j�kni�ciem pada na plecy.{x", ch, whip, victim, TO_CHAR );
	act("{v$n zr�cznie okr�ca $h wok� twojej nogi i ci�gnie z ca�ej si�y. Z j�kni�ciem wywracasz si� na ziemi�.{x", ch, whip, victim, TO_VICT );
	act("{v$n zr�cznie okr�ca $h wok� nogi $Z i ci�gnie z ca�ej si�y, $N z j�kni�ciem pada na plecy.{x", ch, whip, victim, TO_NOTVICT );

	victim->position = POS_SITTING;
	WAIT_STATE( victim, 24 );

	return ;
}

void trick_riposte( int sn, CHAR_DATA *ch, CHAR_DATA *victim )
{
	act("{vWykorzystujesz chwilowe oszo�omienie przeciwnika i wyprowadzasz ripost�!{x", ch, NULL, victim, TO_CHAR );
	act("{v$n wykorzystuje twe chwilowe oszo�omienie i wyprowadza ripost�!{x", ch, NULL, victim, TO_VICT );
	act("{v$n wykorzystuje chwilowe oszo�omienie $Z i wyprowadza ripost�.{x", ch, NULL, victim, TO_NOTVICT );

	one_hit( ch, victim, TYPE_UNDEFINED, FALSE );
	return;
}


//wali w victima na poczatku serii i na koncu serii (dwa razy czyli)
void trick_cyclone( int sn, CHAR_DATA *ch, CHAR_DATA *victim )
{
	OBJ_DATA *wield, *second;
	CHAR_DATA *vch;

	wield = get_eq_char( ch, WEAR_WIELD );
	second = get_eq_char( ch, WEAR_SECOND );

	act("{vWyci�gasz swoje r�ce na boki, prostujesz w �okciach i wykonujesz gwa�towny obr�t, rani�c wszystkich wrog�w dooko�a!{x", ch, NULL, victim, TO_CHAR );
	act("{v$n wyci�ga swoje r�ce na boki, prostuje w �okciach i wykonuje gwa�towny obr�t, rani�c wszystkich dooko�a!{x", ch, NULL, victim, TO_ROOM );

	act("{v$p opada na $c!{x", victim, wield, NULL, TO_ROOM );
	act("{v$p opada na ciebie!{x", victim, wield, NULL, TO_CHAR );
	one_hit( ch, victim, TYPE_UNDEFINED, FALSE );
	act("{v$p opada na ciebie!{x", victim, second, NULL, TO_CHAR );
	act("{v$p opada na $c!{x", victim, second, NULL, TO_ROOM );
	one_hit( ch, victim, TYPE_UNDEFINED, TRUE );

	//jak takie same bronie i podobne wagi, to si� szybciej kreci
	if( (wield->weight+50 > second->weight) && ( second->weight > wield->weight-50 ) )
	{
		if( number_percent() < 50 )
		{
			act("{v$p opada na $c!{x", victim, second, NULL, TO_ROOM );
			act("{v$p opada na ciebie!{x", victim, second, NULL, TO_CHAR );
			one_hit( ch, victim, TYPE_UNDEFINED , TRUE );
		}
		else
		{
			act("{v$p opada na ciebie!{x", victim, wield, NULL, TO_CHAR );
			act("{v$p opada na $c!{x", victim, wield, NULL, TO_ROOM );
			one_hit( ch, victim, TYPE_UNDEFINED , FALSE );
		}
	}

	if ( is_affected( ch, gsn_wardance ) )
	{
		if( number_percent() < 50 )
		{
			act("{v$p opada na $c!{x", victim, second, NULL, TO_ROOM );
			act("{v$p opada na ciebie!{x", victim, second, NULL, TO_CHAR );
			one_hit( ch, victim, TYPE_UNDEFINED , TRUE );
		}
		else
		{
			act("{v$p opada na ciebie!{x", victim, wield, NULL, TO_CHAR );
			act("{v$p opada na $c!{x", victim, wield, NULL, TO_ROOM );
			one_hit( ch, victim, TYPE_UNDEFINED , FALSE );
		}
	}

	if ( is_affected( ch, gsn_berserk ) )//barb i tak nie moze dualwielda na mistrza, ale moze kiedys cos si� zmieni, to niech to se tu bedzie
	{
		if( number_percent() < 50 )
		{
			act("{v$p opada na $c!{x", victim, second, NULL, TO_ROOM );
			act("{v$p opada na ciebie!{x", victim, second, NULL, TO_CHAR );
			one_hit( ch, victim, TYPE_UNDEFINED , TRUE );
		}
		else
		{
			act("{v$p opada na ciebie!{x", victim, wield, NULL, TO_CHAR );
			act("{v$p opada na $c!{x", victim, wield, NULL, TO_ROOM );
			one_hit( ch, victim, TYPE_UNDEFINED , FALSE );
		}
	}

	if ( IS_AFFECTED( ch, AFF_HASTE ))
	{
		if( number_percent() < 50 )
		{
			act("{v$p opada na $c!{x", victim, second, NULL, TO_ROOM );
			act("{v$p opada na ciebie!{x", victim, second, NULL, TO_CHAR );
			one_hit( ch, victim, TYPE_UNDEFINED , TRUE );
		}
		else
		{
			act("{v$p opada na ciebie!{x", victim, wield, NULL, TO_CHAR );
			act("{v$p opada na $c!{x", victim, wield, NULL, TO_ROOM );
			one_hit( ch, victim, TYPE_UNDEFINED , FALSE );
		}
	}

	for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
	{
		if ( is_same_group( ch, vch ) )
			continue;

		if ( vch == victim )
			continue;

		if (!vch->fighting)
		{
			// po co ma niewalcz�cych atakowa�?
			continue;

		}

		act("{v$p opada na $c!{x", vch, wield, NULL, TO_ROOM );
		act("{v$p opada na ciebie!{x", vch, wield, NULL, TO_CHAR );
		one_hit( ch, vch, TYPE_UNDEFINED, FALSE );
		act("{v$p opada na ciebie!{x", vch, second, NULL, TO_CHAR );
		act("{v$p opada na $c!{x", vch, second, NULL, TO_ROOM );
		one_hit( ch, vch, TYPE_UNDEFINED, TRUE );

		//jak takie same bronie i podobne wagi, to si� szybciej kreci
		if( (wield->weight+50 > second->weight) && ( second->weight > wield->weight-50 ) )
		{
			if( number_percent() < 50 )
			{
				act("{v$p opada na ciebie!{x", vch, second, NULL, TO_CHAR );
				act("{v$p opada na $c!{x", vch, second, NULL, TO_ROOM );
				one_hit( ch, vch, TYPE_UNDEFINED , TRUE );
			}
			else
			{
				act("{v$p opada na $c!{x", vch, wield, NULL, TO_ROOM );
				act("{v$p opada na ciebie!{x", vch, wield, NULL, TO_CHAR );
				one_hit( ch, vch, TYPE_UNDEFINED , FALSE );
			}
		}

		if ( is_affected( ch, gsn_wardance ) )
		{
			if( number_percent() < 50 )
			{
				act("{v$p opada na ciebie!{x", vch, second, NULL, TO_CHAR );
				act("{v$p opada na $c!{x", vch, second, NULL, TO_ROOM );
				one_hit( ch, vch, TYPE_UNDEFINED , TRUE );
			}
			else
			{
				act("{v$p opada na $c!{x", vch, wield, NULL, TO_ROOM );
				act("{v$p opada na ciebie!{x", vch, wield, NULL, TO_CHAR );
				one_hit( ch, vch, TYPE_UNDEFINED , FALSE );
			}
		}

		if ( is_affected( ch, gsn_berserk ) )
		{
			if( number_percent() < 50 )
			{
				act("{v$p opada na ciebie!{x", vch, second, NULL, TO_CHAR );
				act("{v$p opada na $c!{x", vch, second, NULL, TO_ROOM );
				one_hit( ch, vch, TYPE_UNDEFINED , TRUE );
			}
			else
			{
				act("{v$p opada na $c!{x", vch, wield, NULL, TO_ROOM );
				act("{v$p opada na ciebie!{x", vch, wield, NULL, TO_CHAR );
				one_hit( ch, vch, TYPE_UNDEFINED , FALSE );
			}
		}

		if ( IS_AFFECTED( ch, AFF_HASTE ))
		{
			if( number_percent() < 50 )
			{
				act("{v$p opada na ciebie!{x", vch, second, NULL, TO_CHAR );
				act("{v$p opada na $c!{x", vch, second, NULL, TO_ROOM );
				one_hit( ch, vch, TYPE_UNDEFINED , TRUE );
			}
			else
			{
				act("{v$p opada na $c!{x", vch, wield, NULL, TO_ROOM );
				act("{v$p opada na ciebie!{x", vch, wield, NULL, TO_CHAR );
				one_hit( ch, vch, TYPE_UNDEFINED , FALSE );
			}
		}
	}

	act("{v$p opada na $c!{x", victim, wield, NULL, TO_ROOM );
	act("{v$p opada na ciebie!{x", victim, wield, NULL, TO_CHAR );
	one_hit( ch, victim, TYPE_UNDEFINED, FALSE );
	act("{v$p opada na ciebie!{x", victim, second, NULL, TO_CHAR );
	act("{v$p opada na $c!{x", victim, second, NULL, TO_ROOM );
	one_hit( ch, victim, TYPE_UNDEFINED, TRUE );

	//jak takie same bronie i podobne wagi, to si� szybciej kreci
	if( (wield->weight+50 > second->weight) && (second->weight > wield->weight-50) )
	{
		if( number_percent() < 50 )
		{
			act("{v$p opada na $c!{x", victim, second, NULL, TO_ROOM );
			act("{v$p opada na ciebie!{x", victim, second, NULL, TO_CHAR );
			one_hit( ch, victim, TYPE_UNDEFINED , TRUE );
		}
		else
		{
			act("{v$p opada na ciebie!{x", victim, wield, NULL, TO_CHAR );
			act("{v$p opada na $c!{x", victim, wield, NULL, TO_ROOM );
			one_hit( ch, victim, TYPE_UNDEFINED , FALSE );
		}
	}

	if ( is_affected( ch, gsn_wardance ) )
	{
		if( number_percent() < 50 )
		{
			act("{v$p opada na $c!{x", victim, second, NULL, TO_ROOM );
			act("{v$p opada na ciebie!{x", victim, second, NULL, TO_CHAR );
			one_hit( ch, victim, TYPE_UNDEFINED , TRUE );
		}
		else
		{
			act("{v$p opada na ciebie!{x", victim, wield, NULL, TO_CHAR );
			act("{v$p opada na $c!{x", victim, wield, NULL, TO_ROOM );
			one_hit( ch, victim, TYPE_UNDEFINED , FALSE );
		}
	}

	if ( is_affected( ch, gsn_berserk ) )//barb i tak nie moze dualwielda na mistrza, ale moze kiedys cos si� zmieni, to niech to se tu bedzie
	{
		if( number_percent() < 50 )
		{
			act("{v$p opada na $c!{x", victim, second, NULL, TO_ROOM );
			act("{v$p opada na ciebie!{x", victim, second, NULL, TO_CHAR );
			one_hit( ch, victim, TYPE_UNDEFINED , TRUE );
		}
		else
		{
			act("{v$p opada na ciebie!{x", victim, wield, NULL, TO_CHAR );
			act("{v$p opada na $c!{x", victim, wield, NULL, TO_ROOM );
			one_hit( ch, victim, TYPE_UNDEFINED , FALSE );
		}
	}

	if ( IS_AFFECTED( ch, AFF_HASTE ))
	{
		if( number_percent() < 50 )
		{
			act("{v$p opada na $c!{x", victim, second, NULL, TO_ROOM );
			act("{v$p opada na ciebie!{x", victim, second, NULL, TO_CHAR );
			one_hit( ch, victim, TYPE_UNDEFINED , TRUE );
		}
		else
		{
			act("{v$p opada na ciebie!{x", victim, wield, NULL, TO_CHAR );
			act("{v$p opada na $c!{x", victim, wield, NULL, TO_ROOM );
			one_hit( ch, victim, TYPE_UNDEFINED , FALSE );
		}
	}


	return;
}

void trick_staff_swirl( int sn, CHAR_DATA *ch, CHAR_DATA *victim )
{
	OBJ_DATA *kostur;
	kostur = get_eq_char( ch, WEAR_WIELD );

	act("{vPo zadanym ciosie chwytasz mocniej $h i obracaj�c swoj� bro� w d�oniach uderzasz $C drugim ko�cem.{x", ch, kostur, victim, TO_CHAR );
	act("{vPo zadaniu ciosu $n chwyta mocniej $h i obracaj�c swoj� bro� uderza ci� drugim ko�cem.{x", ch, kostur, victim, TO_VICT );
	act("{vPo zadaniu ciosu $n chwyta mocniej $h i obracaj�c swoj� bro� uderza $C drugim ko�cem.{x", ch, kostur, victim, TO_NOTVICT );

	one_hit( ch, victim, TYPE_UNDEFINED, FALSE );
	return;
}


void trick_flabbergast( int sn, CHAR_DATA *ch, CHAR_DATA *victim )
{
	int dam, dice_1, dice_2;
	OBJ_DATA *shield;
	AFFECT_DATA af;
	int level, dur;

	level = ch->level;

	dur = level / 7.5 - 1;

	shield = get_eq_char( ch, WEAR_SHIELD );

	switch( shield->value[0] )
	{
		case 0://puklerz
			dice_1 = 1;
			dice_2 = 4;
			break;
		case 1://lekka
			dice_1 = 2;
			dice_2 = 2;
			break;
		case 2://srednia
			dice_1 = 2;
			dice_2 = 3;
			break;
		case 3://ciezka
			dice_1 = 2;
			dice_2 = 4;
			break;
		default://pawez
			dice_1 = 2;
			dice_2 = 5;
			break;
	}

	act("{vBierzesz szeroki zamach i z ca�ej si�y walisz $C $j w ciemi�, a $E pada na ziemi� oszo�omion$R.{x", ch, shield, victim, TO_CHAR );
	act("{v$n bierze szeroki zamach i z ca�ej si�y wali ci� $j w ciemi�, padasz na ziemi� oszo�omion$R.{x", ch, shield, victim, TO_VICT );
	act("{v$n bierze szeroki zamach i z ca�ej si�y wali $C $j w ciemi�, a $E pada na ziemi� oszo�omion$R.{x", ch, shield, victim, TO_NOTVICT );

	dam = dice( 2*dice_1, 2*dice_2 ) + shield->value[ 5 ];
	dam += GET_DAMROLL( ch, shield );
	dam += GET_SKILL_DAMROLL( ch, shield );
	damage( ch, victim, dam, gsn_bash, DAM_BASH, FALSE );
	WAIT_STATE( victim, 48 );
	DAZE_STATE( victim, 108 );
if ( number_percent() < 33 )
	{
		af.where = TO_AFFECTS;
		af.type = sn;
		af.level = level;
		af.location = APPLY_NONE;
		af.duration = UMAX( 1, dur ); af.rt_duration = 0;
		af.modifier = 0;
		af.bitvector = &AFF_DAZE;
		affect_to_char( victim, &af, NULL, TRUE );
	}

	WAIT_STATE( ch, 24 );
	victim->position = POS_SITTING;
	return;
}

void trick_dragon_strike( int sn, CHAR_DATA *ch, CHAR_DATA *victim )
{
	CHAR_DATA *tch;
	OBJ_DATA *spear, *damned_weapon;
	AFFECT_DATA *damned;
	AFFECT_DATA bleeding_wound;
	int dam = 0, base_dam = 0, dam_bonuses = 0;
	bool wywracajacy = FALSE;

	spear = get_eq_char( ch, WEAR_WIELD );

	if( !spear )
		return;

	if( victim->size <= SIZE_LARGE )
	{
		if ( IS_SET( victim->form, FORM_WARM ) || IS_SET( victim->form, FORM_COLD_BLOOD ) )
		{
			if( victim->sex == SEX_FEMALE )
			{
				act("{vOdchodzisz kilka metr�w w ty�, po czym z rozbiegu skaczesz na $C wyci�gaj�c przed siebie $h. Spadasz na ni� niczym smok powalaj�c i praktycznie przybijaj�c do gruntu. Z dziury w piersi tryska strumie� krwi, a z ust wyp�ywa czerwona piana.{x", ch, spear, victim, TO_CHAR );
				act("{v$n odchodzi kilka metr�w w ty�, po czym z rozbiegu skacze na $C wyci�gaj�c przed siebie $h. Spada na ni� niczym smok powalaj�c i praktycznie przybijaj�c do gruntu. Z dziury w piersi tryska strumie� krwi, a z ust wyp�ywa czerwona piana.{x", ch, spear, victim, TO_NOTVICT );
			}
			else
			{
				act("{vOdchodzisz kilka metr�w w ty�, po czym z rozbiegu skaczesz na $C wyci�gaj�c przed siebie $h. Spadasz na niego niczym smok powalaj�c i praktycznie przybijaj�c do gruntu. Z dziury w piersi tryska strumie� krwi, a z ust wyp�ywa czerwona piana.{x", ch, spear, victim, TO_CHAR );
				act("{v$n odchodzi kilka metr�w w ty�, po czym z rozbiegu skacze na $C wyci�gaj�c przed siebie $h. Spada na niego niczym smok powalaj�c i praktycznie przybijaj�c do gruntu. Z dziury w piersi tryska strumie� krwi, a z ust wyp�ywa czerwona piana.{x", ch, spear, victim, TO_NOTVICT );
			}
			act("{v$n odchodzi kilka metr�w w ty�, po czym z rozbiegu skacze na ciebie wyci�gaj�c przed siebie $h. Spada na ciebie niczym smok powalaj�c i praktycznie przybijaj�c do gruntu. Z dziury w twej piersi tryska strumie� krwi, a z ust wyp�ywa czerwona piana.{x", ch, spear, victim, TO_VICT );
		}
		else
		{
			if( victim->sex == SEX_FEMALE )
			{
				act("{vOdchodzisz kilka metr�w w ty�, po czym z rozbiegu skaczesz na $C wyci�gaj�c przed siebie $h. Spadasz na ni� niczym smok powalaj�c i praktycznie przybijaj�c do gruntu.{x", ch, spear, victim, TO_CHAR );
				act("{v$n odchodzi kilka metr�w w ty�, po czym z rozbiegu skacze na $C wyci�gaj�c przed siebie $h. Spada na ni� niczym smok powalaj�c i praktycznie przybijaj�c do gruntu.{x", ch, spear, victim, TO_NOTVICT );
			}
			else
			{
				act("{vOdchodzisz kilka metr�w w ty�, po czym z rozbiegu skaczesz na $C wyci�gaj�c przed siebie $h. Spadasz na niego niczym smok powalaj�c i praktycznie przybijaj�c do gruntu.{x", ch, spear, victim, TO_CHAR );
				act("{v$n odchodzi kilka metr�w w ty�, po czym z rozbiegu skacze na $C wyci�gaj�c przed siebie $h. Spada na niego niczym smok powalaj�c i praktycznie przybijaj�c do gruntu.{x", ch, spear, victim, TO_NOTVICT );
			}
			act("{v$n odchodzi kilka metr�w w ty�, po czym z rozbiegu skacze na ciebie wyci�gaj�c przed siebie $h. Spada na ciebie niczym smok powalaj�c i praktycznie przybijaj�c do gruntu.{x", ch, spear, victim, TO_VICT );
		}
		wywracajacy = TRUE;
	}
	else
	{
		if (!str_cmp( race_table[ GET_RACE(victim) ].name, "drakolicz" )||
			!str_cmp( race_table[ GET_RACE(victim) ].name, "licz" )||
			!str_cmp( race_table[ GET_RACE(victim) ].name, "szkielet wojownik" )||
			!str_cmp( race_table[ GET_RACE(victim) ].name, "szkielet" ))
		{
			act("{vOdchodzisz kilka metr�w w ty�, po czym z rozbiegu skaczesz na $C wyci�gaj�c przed siebie $h. Spadasz na niego niczym smok wy�amuj�c ko�ci jego klatki piersiowej.{x", ch, spear, victim, TO_CHAR );
			act("{v$n odchodzi kilka metr�w w ty�, po czym z rozbiegu skacze na $C wyci�gaj�c przed siebie $h. Spada na niego niczym smok wy�amuj�c ko�ci jego klatki piersiowej.{x", ch, spear, victim, TO_NOTVICT );
		}
		else
		{
			if( victim->sex == SEX_FEMALE )
			{
				act("{vOdchodzisz kilka metr�w w ty�, po czym z rozbiegu skaczesz na $C wyci�gaj�c przed siebie $h. Spadasz na ni� niczym smok wbijaj�c sw� bro� bardzo g��boko w jej cia�o.{x", ch, spear, victim, TO_CHAR );
				act("{v$n odchodzi kilka metr�w w ty�, po czym z rozbiegu skacze na $C wyci�gaj�c przed siebie $h. Spada na ni� niczym smok wbijaj�c sw� bro� bardzo g��boko w jej cia�o.{x", ch, spear, victim, TO_NOTVICT );
			}
			else
			{
				act("{vOdchodzisz kilka metr�w w ty�, po czym z rozbiegu skaczesz na $C wyci�gaj�c przed siebie $h. Spadasz na niego niczym smok wbijaj�c sw� bro� bardzo g��boko w jego cia�o.{x", ch, spear, victim, TO_CHAR );
				act("{v$n odchodzi kilka metr�w w ty�, po czym z rozbiegu skacze na $C wyci�gaj�c przed siebie $h. Spada na niego niczym smok wbijaj�c sw� bro� bardzo g��boko w jego cia�o.{x", ch, spear, victim, TO_NOTVICT );
			}
		}
		act( "Dostales z dragon strika od $z, mobku.", ch, spear, victim, TO_VICT );
		wywracajacy = FALSE;
	}

	base_dam = dice( spear->value[ 1 ], spear->value[ 2 ] ) + spear->value[ 6 ] + calculate_spec_damage( ch, victim, spear, SD_TARGET_DAM );

	dam_bonuses = GET_SKILL_DAMROLL( ch, spear ) + GET_DAMROLL( ch, spear );

	if ( !IS_EVIL(victim) && ch->class == CLASS_BLACK_KNIGHT && ( damned_weapon = get_eq_char(ch,WEAR_WIELD)) != NULL )
	{
		for ( damned = damned_weapon->affected; damned != NULL; damned = damned->next )
		{
			if ( damned->type == gsn_damn_weapon )
			{
				if( IS_GOOD(victim) && !IS_AFFECTED( victim, AFF_PROTECT_EVIL ))
					dam_bonuses += URANGE( 1, damned->level/10 +  get_skill(victim,gsn_damn_weapon)/24, 6 );
				else
					dam_bonuses += URANGE( 1, damned->level/12 +  get_skill(victim,gsn_damn_weapon)/33, 4 );
				break;
			}
		}
	}

	//dobzi slabiej bija jak sa w grupce z death knightem z demoniczna aura
	for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
	{
		if( IS_GOOD(ch) && is_affected(tch,gsn_demon_aura) && is_same_group(ch,tch))
			dam_bonuses -= URANGE( 1, get_skill(tch,gsn_demon_aura)/24, 4);
	}

	if( IS_WEAPON_STAT( spear, WEAPON_VORPAL ) )
		dam = base_dam*6 + dam_bonuses*3;
	else if( IS_WEAPON_STAT( spear, WEAPON_SHARP ) )
		dam = base_dam*5.5 + dam_bonuses*2.8;
	else
		dam = base_dam*5 + dam_bonuses*2.5;

	if ( victim->position < POS_FIGHTING )
	{
		if ( !IS_AWAKE( victim ) )
			dam += dam / 2;
		else
			dam += dam / 4;
	}

	check_increase_wounds( victim, DAM_PIERCE, &dam );
	dam = check_resist( victim, DAM_PIERCE, dam );

	if (!str_cmp( race_table[ GET_RACE(victim) ].name, "drakolicz" )||
		!str_cmp( race_table[ GET_RACE(victim) ].name, "licz" )||
		!str_cmp( race_table[ GET_RACE(victim) ].name, "szkielet wojownik" )||
		!str_cmp( race_table[ GET_RACE(victim) ].name, "szkielet" ))
		dam /= 2;

	raw_damage( ch, victim, dam );
	damage( ch, victim, 0, TYPE_UNDEFINED, DAM_NONE, FALSE );//by rozpoczelo walke jak przy pierwszym ciosie wlazlo

	if( !victim || !victim->in_room )
		return;

	if ( ch->fighting == victim && !is_affected(victim,gsn_bleeding_wound ) && !is_undead(victim) &&
		( IS_SET( victim->form, FORM_WARM ) || IS_SET( victim->form, FORM_COLD_BLOOD ) ) )
	{
		bleeding_wound.where = TO_AFFECTS;
		bleeding_wound.type = gsn_bleeding_wound;
		bleeding_wound.level = 50;
		bleeding_wound.duration = number_range(10,20); bleeding_wound.rt_duration = 0;
		bleeding_wound.location = APPLY_NONE;
		bleeding_wound.modifier = 0;
		bleeding_wound.bitvector = &AFF_NONE;
		affect_to_char(	victim,	&bleeding_wound, NULL, TRUE );
	}

	dam = 0;

	if ( ch->fighting == victim && IS_WEAPON_STAT( spear, WEAPON_POISON ) )
	{
		int level;
		AFFECT_DATA *poison;

		if ( ( poison = affect_find( spear->affected, gsn_poison ) ) == NULL )
			level = 1;
		else
			level = URANGE( 0, poison->level, MAX_POISON );

		poison_to_char( victim, level );
		/* weaken the poison if it's temporary */
		if ( poison != NULL )
		{
			poison->duration = UMAX( 0, poison->duration - 1 ); poison->rt_duration = 0;

			if ( spear && poison->duration == 0 )
			{
				act( "Trucizna pokrywaj�ca ostrze $f wysycha.", ch, spear, NULL, TO_CHAR );
				affect_remove_obj( spear, poison );
			}
		}
	}

	/* wysysanie, nie zadaje obrazen bo juz zadalo wczesniej */
	if ( ch->fighting == victim && IS_WEAPON_STAT( spear, WEAPON_VAMPIRIC ) && !is_undead( victim ) && !IS_SET( victim->form, FORM_CONSTRUCT ) )
	{
		dam = number_range( base_dam / 4, base_dam / 2 );
		if (dam>0)
		{
			act( "$p wysysa �ycie z $z.", victim, spear, NULL, TO_ROOM );
			act( "Czujesz jak $p wysysa z ciebie energi�.", victim, spear, NULL, TO_CHAR );
			if (ch->hit + dam > 125 * ch->hit / 100)
			{
				act( "Energia wyssana z $Z ucieka bezpowrotnie.", ch, NULL, victim, TO_CHAR );
			}
			else
			{
				ch->hit = UMIN( ch->hit + dam, 125 * ch->hit / 100 );
			}
		}
	}

	if ( ch->fighting == victim && IS_WEAPON_STAT( spear, WEAPON_FLAMING ) || IS_WEAPON_STAT( spear, WEAPON_FLAMING_BURST ) )
	{
		dam = number_range( base_dam / 4, base_dam / 2 );
		dam = UMAX( dam, 1 );
		if (spear->liczba_mnoga)
		{
			act( "$p lekko przysma�aj� $C.", ch, spear, victim, TO_CHAR );
		}
		else
		{
			act( "$p lekko przysma�a $C.", ch, spear, victim, TO_CHAR );
		}
		act( "Parzysz si� przy zetkni�ciu z $j.", ch, spear, victim, TO_VICT );
		act( "$N parzy si� przy zetkni�ciu z $j.", ch, spear, victim, TO_NOTVICT );
		fire_effect( ( void * ) victim, 1, dam, TARGET_CHAR );
		damage( ch, victim, dam, 0, DAM_FIRE, FALSE );
	}

	if ( ch->fighting == victim && IS_WEAPON_STAT( spear, WEAPON_FROST ) || IS_WEAPON_STAT( spear, WEAPON_FROST_BURST ) )
	{
		dam = number_range( base_dam / 4, base_dam / 2 );
		dam = UMAX( dam, 1 );
		act( "$p zamra�a $c.", victim, spear, NULL, TO_ROOM );
		act( "Zimny dotyk $f zamienia ci� w bry�� lodu.", victim, spear, NULL, TO_CHAR );
		cold_effect( victim, 1, dam, TARGET_CHAR );
		damage( ch, victim, dam, 0, DAM_COLD, FALSE );
	}

	if ( ch->fighting == victim && IS_WEAPON_STAT( spear, WEAPON_SHOCKING ) || IS_WEAPON_STAT( spear, WEAPON_SHOCKING_BURST ) )
	{
		dam = number_range( base_dam / 4, base_dam / 2 );
		dam = UMAX( dam, 1 );
		act( "B�yskawica z $f trafia $c wywo�uj�c u niego niekontrolowane drgawki.", victim, spear, NULL, TO_ROOM );
		act( "$p aplikuje ci seri� elektrowstrzas�w.", victim, spear, NULL, TO_CHAR );
		shock_effect( victim, 1, dam, TARGET_CHAR );
		damage( ch, victim, dam, 0, DAM_LIGHTNING, FALSE );
	}

	if ( ch->fighting == victim && IS_WEAPON_STAT( spear, WEAPON_TOXIC ) || IS_WEAPON_STAT( spear, WEAPON_TOXIC_BURST ) )
	{
		dam = number_range( base_dam / 4, base_dam / 2 );
		dam = UMAX( dam, 1 );
		act( "Pokrywaj�cy $h kwas wy�era sk�r� $z.", victim, spear, NULL, TO_ROOM );
		act( "Pokrywaj�cy $h kwas wy�era tw� sk�r�!", victim, spear, NULL, TO_CHAR );
		acid_effect( victim, 1, dam, TARGET_CHAR );
		damage( ch, victim, dam, 0, DAM_ACID, FALSE );
	}

    if ( ch->fighting == victim && IS_WEAPON_STAT( spear, WEAPON_RESONANT ) || IS_WEAPON_STAT( spear, WEAPON_SONIC_BURST ) )
    {
        dam = number_range( base_dam / 4, base_dam / 2 );
        dam = UMAX( dam, 1 );
        if (IS_SET(victim->parts, PART_HEAD ) )
        {
            act( "Fala ultrad�wi�k�w z $f wdziera si� do g�owy $z.", victim, spear, NULL, TO_ROOM );
            act( "Fala ultrad�wi�k�w z $f wdziera si� do twojej g�owy wraz z pulsuj�cym b�lem.", victim, spear, NULL, TO_CHAR );
        }
        else
        {
            act( "Fala ultrad�wi�k�w z $f wdziera si� do cia�a $z.", victim, spear, NULL, TO_ROOM );
            act( "Fala ultrad�wi�k�w z $f wdziera si� do twojego cia�a wraz z pulsuj�cym b�lem.", victim, spear, NULL, TO_CHAR );
        }
		sound_effect( victim, 1, dam, TARGET_CHAR );
		damage( ch, victim, dam, 0, DAM_SOUND, FALSE );
	}

	//nie-undeadom zada polowe tego
	if ( ch->fighting == victim && IS_WEAPON_STAT( spear, WEAPON_SACRED ) || IS_WEAPON_STAT( spear, WEAPON_HOLY ) )
	{
		dam = number_range( base_dam / 4, base_dam / 2 );
		dam = UMAX( dam, 1 );
		act( "Niewielki strumie� �wi�tej mocy z $f trafia $c.", victim, spear, NULL, TO_ROOM );
		act( "Niewielki strumie� �wi�tej mocy z $f trafia ci� prosto w pier�.", victim, spear, NULL, TO_CHAR );
		damage( ch, victim, dam, 0, DAM_HOLY, FALSE );
		if( IS_NPC(victim) && victim->pIndexData->vnum == MOB_VNUM_AVATAR )
		{
			act( "{5W kr�tkim rozb�ysku �wiat�a $n zamienia si� w kupk� dymi�cego popio�u.{x", victim, NULL, NULL, TO_ROOM );
			raw_damage( ch, victim, 20 + get_max_hp(victim) );
		}
	}

	if( wywracajacy )
	{
		victim->position = POS_SITTING;
		WAIT_STATE( victim, 48 );
	}

	return;
}

void trick_decapitation( int sn, CHAR_DATA *ch, CHAR_DATA *victim )
{
	 if (!str_cmp( race_table[ GET_RACE(victim) ].name, "duch" )||
		!str_cmp( race_table[ GET_RACE(victim) ].name, "banshee" )||
		!str_cmp( race_table[ GET_RACE(victim) ].name, "cie�" )||
		!str_cmp( race_table[ GET_RACE(victim) ].name, "widmo" )||
		!str_cmp( race_table[ GET_RACE(victim) ].name, "zmora" )||
		!str_cmp( race_table[ GET_RACE(victim) ].name, "upi�r" ))
	{
		act("{vWyprowadzasz niezwykle celne, szybkie, p�askie ci�cie po szyi $Z z nadziej� odr�bania $S g�owy, jednak z przyczyn do�� oczywistych si� to nie zdarza. Mimo wszystko $C musia�o to zabole�...{x", ch, NULL, victim, TO_CHAR );
		act("{v$n wyprowadza niezwykle celne, szybkie, p�askie ci�cie po twojej szyi z nadziej� odr�bania ci g�owy, jednak z przyczyn do�� oczywistych si� to nie zdarza. Mimo wszystko, to bola�o...{x", ch, NULL, victim, TO_VICT );
		act("{v$n wyprowadza niezwykle celne, szybkie, p�askie ci�cie po szyi $Z z nadziej� odr�bania $S g�owy, jednak z przyczyn do�� oczywistych si� to nie zdarza. Mimo wszystko $C musia�o to zabole�...{x", ch, NULL, victim, TO_NOTVICT );
		one_hit( ch, victim, TYPE_UNDEFINED, FALSE );
		return;
	}

	if ( IS_SET( victim->form, FORM_WARM ) || IS_SET( victim->form, FORM_COLD_BLOOD ) )
	{
		act("{vWyprowadzasz niezwykle celne, szybkie, p�askie ci�cie i odcinasz $X g�ow�, kt�ra na tle fontanny krwii $S korpusu spada na ziemi�.{x", ch, NULL, victim, TO_CHAR );
		act("{v$n wyprowadza niezwykle celne, szybkie, p�askie ci�cie i odcina $X g�ow�, kt�ra na tle fontanny krwii $S korpusu spada na ziemi�.{x", ch, NULL, victim, TO_NOTVICT );
	}
	else
	{
		act("{vWyprowadzasz niezwykle celne, szybkie, p�askie ci�cie i odcinasz $X g�ow�.{x", ch, NULL, victim, TO_CHAR );
		act("{v$n wyprowadza niezwykle celne, szybkie, p�askie ci�cie i odcina $X g�ow�.{x", ch, NULL, victim, TO_NOTVICT );
	}
	act("{v$n wyprowadza niezwykle celne, szybkie, p�askie ci�cie w kierunku twojej szyi. Wi�cej nie uda�o ci si� juz zobaczy�.{x", ch, NULL, victim, TO_VICT );

	make_head( victim );
	raw_damage( ch, victim, 20 + get_max_hp( victim ) );
	REMOVE_BIT( ch->fight_data, FIGHT_CRITICAL_STRIKE );

	return;
}

void trick_thundering_whack( int sn, CHAR_DATA *ch, CHAR_DATA *victim )
{
	AFFECT_DATA af, *damned;
	OBJ_DATA *wield = get_eq_char( ch, WEAR_WIELD ), *damned_weapon;
	CHAR_DATA *tch;
	int dur, base_dam, dam_bonuses, dam;

	if ( !wield )
		return;

	if( victim->sex == SEX_FEMALE )
	{
		act("{vBierzesz pionowy zamach zza g�owy i z ca�ej si�y walisz $C $j prosto w ciemi�. $N momentalnie traci �wiadomo�� i czucie w ko�czynach, kolana si� pod ni� uginaj� i upada $E bezw�adnie na ziemi�.{x", ch, wield, victim, TO_CHAR );
		act("{v$n bierze pionowy zamach zza g�owy i z ca�ej si�y wali $C $j prosto w ciemi�. $N momentalnie traci �wiadomo�� i czucie w ko�czynach, kolana si� pod ni� uginaj� i upada $E bezw�adnie na ziemi�.{x", ch, wield, victim, TO_NOTVICT );
	}
	else
	{
		act("{vBierzesz pionowy zamach zza g�owy i z ca�ej si�y walisz $C $j prosto w ciemi�. $N momentalnie traci �wiadomo�� i czucie w ko�czynach, kolana si� pod nim uginaj� i upada $E bezw�adnie na ziemi�.{x", ch, wield, victim, TO_CHAR );
		act("{v$n bierze pionowy zamach zza g�owy i z ca�ej si�y wali $C $j prosto w ciemi�. $N momentalnie traci �wiadomo�� i czucie w ko�czynach, kolana si� pod nim uginaj� i upada $E bezw�adnie na ziemi�.{x", ch, wield, victim, TO_NOTVICT );
	}
	act("{v$n bierze pionowy zamach zza g�owy i z ca�ej si�y wali ci� $j prosto w ciemi�. Momentalnie tracisz �wiadomo�� i czucie w ko�czynach, kolana si� pod tob� uginaj� i upadasz bezw�adnie na ziemi�.{x", ch, wield, victim, TO_VICT );

	base_dam = dice( wield->value[ 1 ], wield->value[ 2 ] ) + wield->value[ 6 ] + calculate_spec_damage( ch, victim, wield, SD_TARGET_DAM );

	dam_bonuses = GET_SKILL_DAMROLL( ch, wield ) + GET_DAMROLL( ch, wield );

	if ( !IS_EVIL(victim) && ch->class == CLASS_BLACK_KNIGHT && ( damned_weapon = get_eq_char(ch,WEAR_WIELD)) != NULL )
	{
		for ( damned = damned_weapon->affected; damned != NULL; damned = damned->next )
		{
			if ( damned->type == gsn_damn_weapon )
			{
				if( IS_GOOD(victim) && !IS_AFFECTED( victim, AFF_PROTECT_EVIL ))
					dam_bonuses += URANGE( 1, damned->level/10 +  get_skill(victim,gsn_damn_weapon)/24, 6 );
				else
					dam_bonuses += URANGE( 1, damned->level/12 +  get_skill(victim,gsn_damn_weapon)/33, 4 );
				break;
			}
		}
	}

	//dobzi slabiej bija jak sa w grupce z death knightem z demoniczna aura
	for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
	{
		if( IS_GOOD(ch) && is_affected(tch,gsn_demon_aura) && is_same_group(ch,tch))
			dam_bonuses -= URANGE( 1, get_skill(tch,gsn_demon_aura)/24, 4);
	}

	dam = base_dam + dam_bonuses;

	if ( victim->position < POS_FIGHTING )
	{
		if ( !IS_AWAKE( victim ) )
			dam += dam / 2;
		else
			dam += dam / 4;
	}

	check_increase_wounds( victim, DAM_BASH, &dam );
	dam = check_resist( victim, DAM_BASH, dam );

	raw_damage( ch, victim, dam );
	damage( ch, victim, 0, TYPE_UNDEFINED, DAM_NONE, FALSE );//by rozpoczelo walke jak przy pierwszym ciosie wlazlo

	dam = 0;

	if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_POISON ) )
	{
		int level;
		AFFECT_DATA *poison;

		if ( ( poison = affect_find( wield->affected, gsn_poison ) ) == NULL )
			level = 1;
		else
			level = URANGE( 0, poison->level, MAX_POISON );

		poison_to_char( victim, level );
		/* weaken the poison if it's temporary */
		if ( poison != NULL )
		{
			poison->duration = UMAX( 0, poison->duration - 1 ); poison->rt_duration = 0;

			if ( wield && poison->duration == 0 )
			{
				act( "Trucizna pokrywaj�ca ostrze $f wysycha.", ch, wield, NULL, TO_CHAR );
				affect_remove_obj( wield, poison );
			}
		}
	}

	/* wysysanie, nie zadaje obrazen bo juz zadalo wczesniej */
	if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_VAMPIRIC ) && !is_undead( victim ) && !IS_SET( victim->form, FORM_CONSTRUCT ) )
	{
		dam = number_range( base_dam / 4, base_dam / 2 );
		if (dam>0)
		{
			act( "$p wysysa �ycie z $z.", victim, wield, NULL, TO_ROOM );
			act( "Czujesz jak $p wysysa z ciebie energi�.", victim, wield, NULL, TO_CHAR );
			if (ch->hit + dam > 125 * ch->hit / 100)
			{
				act( "Energia wyssana z $Z ucieka bezpowrotnie.", ch, NULL, victim, TO_CHAR );
			}
			else
			{
				ch->hit = UMIN( ch->hit + dam, 125 * ch->hit / 100 );
			}
		}
	}

	if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_FLAMING ) || IS_WEAPON_STAT( wield, WEAPON_FLAMING_BURST ) )
	{
		dam = number_range( base_dam / 4, base_dam / 2 );
		dam = UMAX( dam, 1 );
		if (wield->liczba_mnoga)
		{
			act( "$p lekko przysma�aj� $C.", ch, wield, victim, TO_CHAR );
		}
		else {
			act( "$p lekko przysma�a $C.", ch, wield, victim, TO_CHAR );
		}
		act( "Parzysz si� przy zetkni�ciu z $j.", ch, wield, victim, TO_VICT );
		act( "$N parzy si� przy zetkni�ciu z $j.", ch, wield, victim, TO_NOTVICT );
		fire_effect( ( void * ) victim, 1, dam, TARGET_CHAR );
		damage( ch, victim, dam, 0, DAM_FIRE, FALSE );
	}

	if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_FROST ) || IS_WEAPON_STAT( wield, WEAPON_FROST_BURST ) )
	{
		dam = number_range( base_dam / 4, base_dam / 2 );
		dam = UMAX( dam, 1 );
		act( "$p zamra�a $c.", victim, wield, NULL, TO_ROOM );
		act( "Zimny dotyk $f zamienia ci� w bry�� lodu.", victim, wield, NULL, TO_CHAR );
		cold_effect( victim, 1, dam, TARGET_CHAR );
		damage( ch, victim, dam, 0, DAM_COLD, FALSE );
	}

	if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_SHOCKING ) || IS_WEAPON_STAT( wield, WEAPON_SHOCKING_BURST ) )
	{
		dam = number_range( base_dam / 4, base_dam / 2 );
		dam = UMAX( dam, 1 );
		act( "B�yskawica z $f trafia $c wywo�uj�c u niego niekontrolowane drgawki.", victim, wield, NULL, TO_ROOM );
		act( "$p aplikuje ci seri� elektrowstrzas�w.", victim, wield, NULL, TO_CHAR );
		shock_effect( victim, 1, dam, TARGET_CHAR );
		damage( ch, victim, dam, 0, DAM_LIGHTNING, FALSE );
	}

	if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_TOXIC ) || IS_WEAPON_STAT( wield, WEAPON_TOXIC_BURST ) )
	{
		dam = number_range( base_dam / 4, base_dam / 2 );
		dam = UMAX( dam, 1 );
		act( "Pokrywaj�cy $h kwas wy�era sk�r� $z.", victim, wield, NULL, TO_ROOM );
		act( "Pokrywaj�cy $h kwas wy�era tw� sk�r�!", victim, wield, NULL, TO_CHAR );
		acid_effect( victim, 1, dam, TARGET_CHAR );
		damage( ch, victim, dam, 0, DAM_ACID, FALSE );
	}

    if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_RESONANT ) || IS_WEAPON_STAT( wield, WEAPON_SONIC_BURST ) )
    {
        dam = number_range( base_dam / 4, base_dam / 2 );
        dam = UMAX( dam, 1 );
        if (IS_SET(victim->parts, PART_HEAD ) )
        {
            act( "Fala ultrad�wi�k�w z $f wdziera si� do g�owy $z.", victim, wield, NULL, TO_ROOM );
            act( "Fala ultrad�wi�k�w z $f wdziera si� do twojej g�owy wraz z pulsuj�cym b�lem.", victim, wield, NULL, TO_CHAR );
        }
        else
        {
            act( "Fala ultrad�wi�k�w z $f wdziera si� do cia�a $z.", victim, wield, NULL, TO_ROOM );
            act( "Fala ultrad�wi�k�w z $f wdziera si� do twojego cia�a wraz z pulsuj�cym b�lem.", victim, wield, NULL, TO_CHAR );
        }
        sound_effect( victim, 1, dam, TARGET_CHAR );
        damage( ch, victim, dam, 0, DAM_SOUND, FALSE );
    }

	//nie-undeadom zada polowe tego
	if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_SACRED ) )
	{
		dam = number_range( base_dam / 4, base_dam / 2 );
		dam = UMAX( dam, 1 );
		act( "Niewielki strumie� �wi�tej mocy z $f trafia $c.", victim, wield, NULL, TO_ROOM );
		act( "Niewielki strumie� �wi�tej mocy z $f trafia ci� prosto w pier�.", victim, wield, NULL, TO_CHAR );
		damage( ch, victim, dam, 0, DAM_HOLY, FALSE );
		if( IS_NPC(victim) && victim->pIndexData->vnum == MOB_VNUM_AVATAR )
		{
			act( "{5W kr�tkim rozb�ysku �wiat�a $n zamienia si� w kupk� dymi�cego popio�u.{x", victim, NULL, NULL, TO_ROOM );
			raw_damage( ch, victim, 20 + get_max_hp(victim) );
		}
	}

	dur = dice( wield->value[ 1 ], wield->value[ 2 ] ) + wield->value[ 6 ];
	dur /= 3;

	af.where = TO_AFFECTS;
	af.type = 304;//daze
	af.level = ch->level;
	af.location = APPLY_NONE;
	af.duration = URANGE( 1, dur, 6 ); af.rt_duration = 0;
	af.modifier = 0;
	af.bitvector = &AFF_DAZE;
	affect_to_char( victim, &af, NULL, TRUE );

	DAZE_STATE( victim, 108 );
	WAIT_STATE( victim, 48 );
	WAIT_STATE( ch, 24 );

	victim->position = POS_SITTING;

	return;
}

void trick_strucking_wallop( int sn, CHAR_DATA *ch, CHAR_DATA *victim )
{
	OBJ_DATA *wield, *second;

	wield = get_eq_char( victim, WEAR_WIELD );
	second = get_eq_char( victim, WEAR_SECOND );

	if( is_affected(victim,gsn_glorious_impale) && !affect_find(victim->affected,gsn_glorious_impale)->modifier && get_eq_char(victim,WEAR_WIELD) && get_eq_char(victim,WEAR_WIELD)->value[0] == WEAPON_SPEAR )
		return;

	if( wield && !IS_OBJ_STAT( wield, ITEM_NOREMOVE ) )
	{
		act("{vCios by� tak silny, �e dostajesz skurczy na ca�ym ciele i $p wypada ci z r�k.{x", victim, wield, NULL, TO_CHAR );
		act("{vCios by� tak silny, �e $n dostaje skurczy na ca�ym ciele i $p wypada z $s r�k.{x", victim, wield, NULL, TO_ROOM );

		if ( is_artefact( wield ) && !IS_NPC( victim ) && !IS_IMMORTAL( victim ) )
			artefact_from_char( wield, victim );

		obj_from_char( wield );

		if ( IS_OBJ_STAT( wield, ITEM_NODROP ) || IS_OBJ_STAT( wield, ITEM_INVENTORY ) )
		{
			if ( is_artefact( wield ) && !IS_NPC( victim ) && !IS_IMMORTAL( victim ) )
				artefact_to_char( wield, victim );

			obj_to_char( wield, victim );
		}
		else
		{
			obj_to_room( wield, victim->in_room );
		}
	}

	if( second && !IS_OBJ_STAT( second, ITEM_NOREMOVE ) )
	{
		act("{vCios by� tak silny, �e dostajesz skurczy na ca�ym ciele i $p wypada ci z r�k.{x", victim, second, NULL, TO_CHAR );
		act("{vCios by� tak silny, �e $n dostaje skurczy na ca�ym ciele i $p wypada z $s r�k.{x", victim, second, NULL, TO_ROOM );

		if ( is_artefact( second ) && !IS_NPC( victim ) && !IS_IMMORTAL( victim ) )
			artefact_from_char( second, victim );

		obj_from_char( second );

		if ( IS_OBJ_STAT( second, ITEM_NODROP ) || IS_OBJ_STAT( second, ITEM_INVENTORY ) )
		{
			if ( is_artefact( second ) && !IS_NPC( victim ) && !IS_IMMORTAL( victim ) )
				artefact_to_char( second, victim );

			obj_to_char( second, victim );
		}
		else
		{
			obj_to_room( second, victim->in_room );
		}
	}

	DAZE_STATE( victim, 36 );


return;
}

void trick_shove( int sn, CHAR_DATA *ch, CHAR_DATA *victim )
{
	if( victim->sex == SEX_FEMALE )
	{
		act("{vWykorzystujesz brak r�wnowagi $Z i popychasz j� wywracaj�c na ziemi�.{x", ch, NULL, victim, TO_CHAR );
		act("{v$n wykorzystuje brak r�wnowagi $Z i popychasz j� wywracaj�c na ziemi�.{x", ch, NULL, victim, TO_NOTVICT );
	}
	else
	{
		act("{vWykorzystujesz brak r�wnowagi $Z i popychasz go wywracaj�c na ziemi�.{x", ch, NULL, victim, TO_CHAR );
		act("{v$n wykorzystuje brak r�wnowagi $Z i popychasz go wywracaj�c na ziemi�.{x", ch, NULL, victim, TO_NOTVICT );
	}
	act("{v$n wykorzystuje tw�j brak r�wnowagi i popycha ci� wywracaj�c na ziemi�.{x", ch, NULL, victim, TO_VICT );

	WAIT_STATE( victim, 24 );
	victim->position = POS_SITTING;

	return;
}

void trick_ravaging_orb( int sn, CHAR_DATA *ch, CHAR_DATA *victim )
{
	CHAR_DATA *tch;
	OBJ_DATA *wield, *damned_weapon;
	AFFECT_DATA *damned, af;
	int dam = 0, base_dam = 0, dam_bonuses = 0, dur;
	bool dazujacy = FALSE;

	wield = get_eq_char( ch, WEAR_WIELD );

	if( !wield )
		return;

	if( victim->height < 7*(ch->height/5) )
	{
		if ( IS_SET( victim->form, FORM_WARM ) || IS_SET( victim->form, FORM_COLD_BLOOD ) )
		{
			if( victim->sex == SEX_FEMALE )
			{
				act("{vOdskakujesz szybko od $Z, po czym zaczynasz biec w jej stron� gro�nie wymachuj�c $j, wprawiaj�c przy tym kolczast� kul� w ruch. Wykonujesz pot�ny zamach i trafia ona $C prosto w skro�. Pada na ziemi� og�uszona, a z jej g�owy sp�ywa strumie� krwii.{x", ch, wield, victim, TO_CHAR );
				act("{v$n odskakuje szybko od $Z, po czym zaczyna biec w jej stron� gro�nie wymachuj�c $j, wprawiaj�c przy tym kolczast� kul� w ruch. $n wykonuje pot�ny zamach i trafia ona $C prosto w skro�. Pada na ziemi� og�uszona, a z jej g�owy sp�ywa strumie� krwii.{x", ch, wield, victim, TO_NOTVICT );
			}
			else
			{
				act("{vOdskakujesz szybko od $Z, po czym zaczynasz biec w jego stron� gro�nie wymachuj�c $j, wprawiaj�c przy tym kolczast� kul� w ruch. Wykonujesz pot�ny zamach i trafia ona $C prosto w skro�. Pada na ziemi� og�uszony, a z jego g�owy sp�ywa strumie� krwii.{x", ch, wield, victim, TO_CHAR );
				act("{v$n odskakuje szybko od $Z, po czym zaczyna biec w jego stron� gro�nie wymachuj�c $j, wprawiaj�c przy tym kolczast� kul� w ruch. $n wykonuje pot�ny zamach i trafia ona $C prosto w skro�. Pada na ziemi� og�uszony, a z jego g�owy sp�ywa strumie� krwii.{x", ch, wield, victim, TO_NOTVICT );
			}
			act("{v$n odskakuje szybko od ciebie, po czym zaczyna biec w tw� stron� gro�nie wymachuj�c $j, wprawiaj�c przy tym kolczast� kul� w ruch. $n wykonuje pot�ny zamach i trafia ona ci� prosto w skro�. Padasz na ziemi� og�uszony, a z twojej g�owy sp�ywa strumie� krwii.{x", ch, wield, victim, TO_VICT );
			dazujacy = TRUE;
		}
		else
		{
			if( victim->sex == SEX_FEMALE )
			{
				act("{vOdskakujesz szybko od $Z, po czym zaczynasz biec w jej stron� gro�nie wymachuj�c $j, wprawiaj�c przy tym kolczast� kul� w ruch. Wykonujesz pot�ny zamach i trafia ona $C prosto w skro�. Cios by� tak silny, �e $N wywraca si� na ziemi�.{x", ch, wield, victim, TO_CHAR );
				act("{v$n odskakuje szybko od $Z, po czym zaczyna biec w jej stron� gro�nie wymachuj�c $j, wprawiaj�c przy tym kolczast� kul� w ruch. $n wykonuje pot�ny zamach i trafia ona $C prosto w skro�. Cios by� tak silny, �e $N wywraca si� na ziemi�.{x", ch, wield, victim, TO_NOTVICT );
			}
			else
			{
				act("{vOdskakujesz szybko od $Z, po czym zaczynasz biec w jego stron� gro�nie wymachuj�c $j, wprawiaj�c przy tym kolczast� kul� w ruch. Wykonujesz pot�ny zamach i trafia ona $C prosto w skro�. Cios by� tak silny, �e $N wywraca si� na ziemi�.{x", ch, wield, victim, TO_CHAR );
				act("{v$n odskakuje szybko od $Z, po czym zaczyna biec w jego stron� gro�nie wymachuj�c $j, wprawiaj�c przy tym kolczast� kul� w ruch. $n wykonuje pot�ny zamach i trafia ona $C prosto w skro�. Cios by� tak silny, �e $N wywraca si� na ziemi�.{x", ch, wield, victim, TO_NOTVICT );
			}
			act("{v$n odskakuje szybko od ciebie, po czym zaczyna biec w tw� stron� gro�nie wymachuj�c $j, wprawiaj�c przy tym kolczast� kul� w ruch. $n wykonuje pot�ny zamach i trafia ona ci� prosto w skro�. Cios by� tak silny, �e wywracasz si� na ziemi�.{x", ch, wield, victim, TO_VICT );
			dazujacy = FALSE;
		}
	}
	else
	{
		if( victim->sex == SEX_FEMALE )
		{
			act("{vOdskakujesz szybko od $Z, po czym zaczynasz biec w jej stron� gro�nie wymachuj�c $j, wprawiaj�c przy tym kolczast� kul� w ruch. Wykonujesz pot�ny zamach celem trafienia przeciwniczki w skro�, jednak nie udaje ci si� si�gn�� do g�owy, wi�c wyprowadzasz tylko zwyk�y cios.{x", ch, wield, victim, TO_CHAR );
			act("{v$n odskakuje szybko od $Z, po czym zaczyna biec w jej stron� gro�nie wymachuj�c $j, wprawiaj�c przy tym kolczast� kul� w ruch. $n wykonuje pot�ny zamach celem trafienia przeciwniczki w skro�, jednak agresorowi nie udaje si� si�gn�� do g�owy, wi�c wyprowadza tylko zwyk�y cios.{x", ch, wield, victim, TO_NOTVICT );
		}
		else
		{
			act("{vOdskakujesz szybko od $Z, po czym zaczynasz biec w jego stron� gro�nie wymachuj�c $j, wprawiaj�c przy tym kolczast� kul� w ruch. Wykonujesz pot�ny zamach celem trafienia przeciwnika w skro�, jednak nie udaje ci si� si�gn�� do g�owy, wi�c wyprowadzasz tylko zwyk�y cios.{x", ch, wield, victim, TO_CHAR );
			act("{v$n odskakuje szybko od $Z, po czym zaczyna biec w jego stron� gro�nie wymachuj�c $j, wprawiaj�c przy tym kolczast� kul� w ruch. $n wykonuje pot�ny zamach celem trafienia przeciwnika w skro�, jednak agresorowi nie udaje si� si�gn�� do g�owy, wi�c wyprowadza tylko zwyk�y cios.{x", ch, wield, victim, TO_NOTVICT );
		}
		if( ch->sex == SEX_FEMALE )
			act("{v$n odskakuje szybko od ciebie, po czym zaczyna biec w tw� stron� gro�nie wymachuj�c $j, wprawiaj�c przy tym kolczast� kul� w ruch. $n wykonuje pot�ny zamach zamach celem trafienia ci� w skro�, jednak nie udaje jej si� si�gn�� do g�owy, wi�c wyprowadza tylko zwyk�y cios.{x", ch, wield, victim, TO_VICT );
		else
			act("{v$n odskakuje szybko od ciebie, po czym zaczyna biec w tw� stron� gro�nie wymachuj�c $j, wprawiaj�c przy tym kolczast� kul� w ruch. $n wykonuje pot�ny zamach zamach celem trafienia ci� w skro�, jednak nie udaje jej si� si�gn�� do g�owy, wi�c wyprowadza tylko zwyk�y cios.{x", ch, wield, victim, TO_VICT );

		one_hit( ch, victim, TYPE_UNDEFINED, FALSE );
		return;
	}

	base_dam = dice( wield->value[ 1 ], wield->value[ 2 ] ) + wield->value[ 6 ] + calculate_spec_damage( ch, victim, wield, SD_TARGET_DAM );

	dam_bonuses = GET_SKILL_DAMROLL( ch, wield ) + GET_DAMROLL( ch, wield );

	if ( !IS_EVIL(victim) && ch->class == CLASS_BLACK_KNIGHT && ( damned_weapon = get_eq_char(ch,WEAR_WIELD)) != NULL )
	{
		for ( damned = damned_weapon->affected; damned != NULL; damned = damned->next )
		{
			if ( damned->type == gsn_damn_weapon )
			{
				if( IS_GOOD(victim) && !IS_AFFECTED( victim, AFF_PROTECT_EVIL ))
					dam_bonuses += URANGE( 1, damned->level/10 +  get_skill(victim,gsn_damn_weapon)/24, 6 );
				else
					dam_bonuses += URANGE( 1, damned->level/12 +  get_skill(victim,gsn_damn_weapon)/33, 4 );
				break;
			}
		}
	}

	//dobzi slabiej bija jak sa w grupce z death knightem z demoniczna aura
	for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
	{
		if( IS_GOOD(ch) && is_affected(tch,gsn_demon_aura) && is_same_group(ch,tch))
			dam_bonuses -= URANGE( 1, get_skill(tch,gsn_demon_aura)/24, 4);
	}

	dam = base_dam*1.5 + dam_bonuses;

	if ( victim->position < POS_FIGHTING )
	{
		if ( !IS_AWAKE( victim ) )
			dam += dam / 2;
		else
			dam += dam / 4;
	}

	check_increase_wounds( victim, DAM_PIERCE, &dam );
	dam = check_resist( victim, DAM_PIERCE, dam );

	raw_damage( ch, victim, dam );
	damage( ch, victim, 0, TYPE_UNDEFINED, DAM_NONE, FALSE );//by rozpoczelo walke jak przy pierwszym ciosie wlazlo

	if( !victim || !victim->in_room )
		return;

	dam = 0;

	if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_POISON ) )
	{
		int level;
		AFFECT_DATA *poison;

		if ( ( poison = affect_find( wield->affected, gsn_poison ) ) == NULL )
			level = 1;
		else
			level = URANGE( 0, poison->level, MAX_POISON );

		poison_to_char( victim, level );
		/* weaken the poison if it's temporary */
		if ( poison != NULL )
		{
			poison->duration = UMAX( 0, poison->duration - 1 ); poison->rt_duration = 0;

			if ( wield && poison->duration == 0 )
			{
				act( "Trucizna pokrywaj�ca ostrze $f wysycha.", ch, wield, NULL, TO_CHAR );
				affect_remove_obj( wield, poison );
			}
		}
	}

	/* wysysanie, nie zadaje obrazen bo juz zadalo wczesniej */
	if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_VAMPIRIC ) && !is_undead( victim ) && !IS_SET( victim->form, FORM_CONSTRUCT ) )
	{
		dam = number_range( base_dam / 4, base_dam / 2 );
		if (dam>0)
		{
			act( "$p wysysa �ycie z $z.", victim, wield, NULL, TO_ROOM );
			act( "Czujesz jak $p wysysa z ciebie energi�.", victim, wield, NULL, TO_CHAR );
			if (ch->hit + dam > 125 * ch->hit / 100)
			{
				act( "Energia wyssana z $Z ucieka bezpowrotnie.", ch, NULL, victim, TO_CHAR );
			}
			else
			{
				ch->hit = UMIN( ch->hit + dam, 125 * ch->hit / 100 );
			}
		}
	}

	if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_FLAMING ) || IS_WEAPON_STAT( wield, WEAPON_FLAMING_BURST ) )
	{
		dam = number_range( base_dam / 4, base_dam / 2 );
		dam = UMAX( dam, 1 );
		if (wield->liczba_mnoga)
		{
			act( "$p lekko przysma�aj� $C.", ch, wield, victim, TO_CHAR );
		}
		else {
			act( "$p lekko przysma�a $C.", ch, wield, victim, TO_CHAR );
		}
		act( "Parzysz si� przy zetkni�ciu z $j.", ch, wield, victim, TO_VICT );
		act( "$N parzy si� przy zetkni�ciu z $j.", ch, wield, victim, TO_NOTVICT );
		fire_effect( ( void * ) victim, 1, dam, TARGET_CHAR );
		damage( ch, victim, dam, 0, DAM_FIRE, FALSE );
	}

	if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_FROST ) || IS_WEAPON_STAT( wield, WEAPON_FROST_BURST ) )
	{
		dam = number_range( base_dam / 4, base_dam / 2 );
		dam = UMAX( dam, 1 );
		act( "$p zamra�a $c.", victim, wield, NULL, TO_ROOM );
		act( "Zimny dotyk $f zamienia ci� w bry�� lodu.", victim, wield, NULL, TO_CHAR );
		cold_effect( victim, 1, dam, TARGET_CHAR );
		damage( ch, victim, dam, 0, DAM_COLD, FALSE );
	}

	if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_SHOCKING ) || IS_WEAPON_STAT( wield, WEAPON_SHOCKING_BURST ) )
	{
		dam = number_range( base_dam / 4, base_dam / 2 );
		dam = UMAX( dam, 1 );
		act( "B�yskawica z $f trafia $c wywo�uj�c u niego niekontrolowane drgawki.", victim, wield, NULL, TO_ROOM );
		act( "$p aplikuje ci seri� elektrowstrzas�w.", victim, wield, NULL, TO_CHAR );
		shock_effect( victim, 1, dam, TARGET_CHAR );
		damage( ch, victim, dam, 0, DAM_LIGHTNING, FALSE );
	}

	if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_TOXIC ) || IS_WEAPON_STAT( wield, WEAPON_TOXIC_BURST ) )
	{
		dam = number_range( base_dam / 4, base_dam / 2 );
		dam = UMAX( dam, 1 );
		act( "Pokrywaj�cy $h kwas wy�era sk�r� $z.", victim, wield, NULL, TO_ROOM );
		act( "Pokrywaj�cy $h kwas wy�era tw� sk�r�!", victim, wield, NULL, TO_CHAR );
		acid_effect( victim, 1, dam, TARGET_CHAR );
		damage( ch, victim, dam, 0, DAM_ACID, FALSE );
	}

	if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_RESONANT ) || IS_WEAPON_STAT( wield, WEAPON_SONIC_BURST ) )
	{
		dam = number_range( base_dam / 4, base_dam / 2 );
        dam = UMAX( dam, 1 );
        if (IS_SET(victim->parts, PART_HEAD ) )
        {
            act( "Fala ultrad�wi�k�w z $f wdziera si� do g�owy $z.", victim, wield, NULL, TO_ROOM );
            act( "Fala ultrad�wi�k�w z $f wdziera si� do twojej g�owy wraz z pulsuj�cym b�lem.", victim, wield, NULL, TO_CHAR );
        }
        else
        {
            act( "Fala ultrad�wi�k�w z $f wdziera si� do cia�a $z.", victim, wield, NULL, TO_ROOM );
            act( "Fala ultrad�wi�k�w z $f wdziera si� do twojego cia�a wraz z pulsuj�cym b�lem.", victim, wield, NULL, TO_CHAR );
        }
        sound_effect( victim, 1, dam, TARGET_CHAR );
        damage( ch, victim, dam, 0, DAM_SOUND, FALSE );
    }

	//nie-undeadom zada polowe tego
	if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_SACRED ) )
	{
		dam = number_range( base_dam / 4, base_dam / 2 );
		dam = UMAX( dam, 1 );
		act( "Niewielki strumie� �wi�tej mocy z $f trafia $c.", victim, wield, NULL, TO_ROOM );
		act( "Niewielki strumie� �wi�tej mocy z $f trafia ci� prosto w pier�.", victim, wield, NULL, TO_CHAR );
		damage( ch, victim, dam, 0, DAM_HOLY, FALSE );
		if( IS_NPC(victim) && victim->pIndexData->vnum == MOB_VNUM_AVATAR )
		{
			act( "{5W kr�tkim rozb�ysku �wiat�a $n zamienia si� w kupk� dymi�cego popio�u.{x", victim, NULL, NULL, TO_ROOM );
			raw_damage( ch, victim, 20 + get_max_hp(victim) );
		}
	}

	victim->position = POS_SITTING;
	WAIT_STATE( victim, 36 );

	if( dazujacy )
	{
		dur = dice( wield->value[ 1 ], wield->value[ 2 ] ) + wield->value[ 6 ];
		dur /= 3;

		af.where = TO_AFFECTS;
		af.type = 304;//daze
		af.level = ch->level;
		af.location = APPLY_NONE;
		af.duration = URANGE( 1, dur, 6 ); af.rt_duration = 0;
		af.modifier = 0;
		af.bitvector = &AFF_DAZE;
		affect_to_char( victim, &af, NULL, TRUE );

		DAZE_STATE( victim, dur * 18 );
	}

	return;
}

void trick_bleed( int sn, CHAR_DATA *ch, CHAR_DATA *victim )
{
	AFFECT_DATA bleed;
	int dur, mod;
	OBJ_DATA *wield = get_eq_char( ch, WEAR_WIELD );

	if( IS_SET(ch->fight_data, FIGHT_TRICK_WIELDSECOND ) )
		wield = get_eq_char( ch, WEAR_SECOND );

	if( !wield )
		return;
		
	if( get_curr_stat_deprecated(ch,STAT_DEX ) > 25 )
		dur = 6;
	else if ( get_curr_stat_deprecated(ch,STAT_DEX ) > 20 )
		dur = 5;
	else if ( get_curr_stat_deprecated(ch,STAT_DEX ) > 15 )
		dur = 4;
	else if ( get_curr_stat_deprecated(ch,STAT_DEX ) > 10 )
		dur = 3;
	else
		dur = 2;
	//Dodaje mno�nik na nowe staty, co to jest 3 stata max? :o
	if( get_curr_stat_deprecated(ch,STAT_STR ) > 25 )
		mod = 3*6;
	else if ( get_curr_stat_deprecated(ch,STAT_STR ) > 19 )
		mod = 2*6;
	else
		mod = 1*6;
		
	//Drake: dodanie sprawdzania nieumar�ych i konstrukt�w, oraz wy��czenie wampir�w i one_hit przy ataku na nieumar�ych/konstrukty.
	if (!str_cmp( race_table[ GET_RACE(victim) ].name, "wampir" ))
	dur /= 2;
	else if ( is_undead(victim) || IS_SET(victim->form, FORM_CONSTRUCT))
	{
	act("{vPodnosisz $h pr�buj�c szybkim ciosem zrani� $C, jednak nie bardzo si� da, wiec po prostu atakujesz normalnie.{x", ch, wield, victim, TO_CHAR );
    act("{v$n podnosi $h pr�buj�c zrani� $C szybkim ci�ciem, ale nie wie za bardzo jak to zrobi� wi�c po prostu atakuje normalnie.{x", ch, wield, victim, TO_NOTVICT );
    one_hit( ch, victim, TYPE_UNDEFINED, FALSE );
    return;
	}
	
	bleed.where = TO_AFFECTS;
	bleed.type = gsn_bleed;
	bleed.level = 50;
	bleed.duration = dur; bleed.rt_duration = 0;
	bleed.modifier = -mod;
	bleed.bitvector = &AFF_NONE;

	switch( number_range(1,3))
	{
		case 1:
			bleed.location = APPLY_STR;
			act("{vPodnosisz $h rani�c $C pod pach�. Widzisz jak krew tryska z rany a r�ka $Z wiotczeje.{x", ch, wield, victim, TO_CHAR );
			act("{v$n podnosi $h do przodu i do g�ry, d�gaj�c ci� bole�nie pod ramieniem. Chyba co� trafi� to czujesz jak z ka�d� chwil� ubywa si� w twojej r�ce.{x", ch, wield, victim, TO_VICT );
			act("{v$n podnosi $h rani�c $C pod pach�. Widzisz jak krew tryska z rany a r�ka $Z wiotczeje.{x", ch, wield, victim, TO_NOTVICT );
			break;
		case 2:
			bleed.location = APPLY_CON;
			act("{vPochylasz si� z wypadem do przodu i atakujesz ty� kolana $Z. Krew zaczyna si� s�czy� a sk�ra $Z zaczyna lekko bledn��.{x", ch, wield, victim, TO_CHAR );
			act("{v$n schyla si� szybko, robi b�yskawiczny wypad i trafia ci� od ty�u w okolice kolana. Up�yw krwi powoduje, �e zaczynasz s�abn��.{x", ch, wield, victim, TO_VICT );
			act("{v$n nurkuje w kierunku n�g $Z i wbija ostrze swej broni w �ydk� i u�miecha si� nieco na widok lej�cej si� krwi i bledn�cej sk�ry przeciwnika.{x", ch, wield, victim, TO_NOTVICT );
			break;
		case 3:
			bleed.location = APPLY_DEX;
			act("{vWbijasz $h w sk�r� po wewn�trznej stronie przedramienia $Z. Z zadowoleniem patrzysz jak $S cierpn�ce poma�u palce z coraz wi�kszym trudem utrzymuj� bro�.{x", ch, wield, victim, TO_CHAR );
			act("{v$n wbija $h tu� poni�ej twojego �okcia, poprzez okropny b�l zaczynasz traci� czucie w palcach.{x", ch, wield, victim, TO_VICT );
			act("{v$n wykonuje b�yskawiczny cios w przedrami� $Z niemal nie wypruwaj�c $S �y�, $N �ciska z�by z b�lu i stara si� mocniej schwyci� swoj� bro�.{x", ch, wield, victim, TO_NOTVICT );
			break;
	}

	affect_to_char( victim, &bleed, NULL, TRUE );

	return;
}

void trick_thigh_jab( int sn, CHAR_DATA *ch, CHAR_DATA *victim )
{
	AFFECT_DATA jab;
	int dur, mod;
	OBJ_DATA *wield = get_eq_char( ch, WEAR_WIELD );

	if( IS_SET(ch->fight_data, FIGHT_TRICK_WIELDSECOND ) )
		wield = get_eq_char( ch, WEAR_SECOND );

	if( !wield )
		return;

	if( ch->size == SIZE_SMALL )
	{
		if( victim->size == SIZE_SMALL )
		{
			act("{vSchylasz si� lekko i d�gasz $C $j w udo.{x",ch, wield, victim, TO_CHAR );
			act("{v$n schyla si� lekko i d�ga ciebie $j w udo.{x",ch, wield, victim, TO_VICT );
			act("{v$n schyla si� lekko i d�ga $C $j w udo.{x",ch, wield, victim, TO_NOTVICT );
		}
		else if( victim->size == SIZE_MEDIUM )
		{
			act("{vLekko si� odchylasz i d�gasz $C $j w udo.{x",ch, wield, victim, TO_CHAR );
			act("{v$n lekko si� odchyla i d�ga ciebie $j w udo.{x",ch, wield, victim, TO_VICT );
			act("{v$n lekko si� odchyla i d�ga $C $j w udo.{x",ch, wield, victim, TO_NOTVICT );
		}
		else
		{
			act("{vWyci�gasz r�k� do g�ry i d�gasz $C $j w udo.{x",ch, wield, victim, TO_CHAR );
			act("{v$n wyci�ga r�k� do g�ry i d�ga ciebie $j w udo.{x",ch, wield, victim, TO_VICT );
			act("{v$n wyci�ga r�k� do g�ry i d�ga $C $j w udo.{x",ch, wield, victim, TO_NOTVICT );
		}
	}
	else if ( ch->size == SIZE_MEDIUM )
	{
		if( victim->size == SIZE_SMALL )
		{
			act("{vMocno si� schylasz i d�gasz $C $j w udo.{x",ch, wield, victim, TO_CHAR );
			act("{v$n mocno si� odchyla i d�ga ciebie $j w udo.{x",ch, wield, victim, TO_VICT );
			act("{v$n mocno si� odchyla i d�ga $C $j w udo.{x",ch, wield, victim, TO_NOTVICT );
		}
		else if( victim->size == SIZE_MEDIUM )
		{
			act("{vLekko si� schylasz i d�gasz $C $j w udo.{x",ch, wield, victim, TO_CHAR );
			act("{v$n schyla si� lekko i d�ga ciebie $j w udo.{x",ch, wield, victim, TO_VICT );
			act("{v$n schyla si� lekko i d�ga $C $j w udo.{x",ch, wield, victim, TO_NOTVICT );
		}
		else
		{
			act("{vOdchylasz si� do ty�u i d�gasz $C $j w udo.{x",ch, wield, victim, TO_CHAR );
			act("{v$n odchyla si� do ty�u i d�ga ciebie $j w udo.{x",ch, wield, victim, TO_VICT );
			act("{v$n odchyla si� do ty�u i d�ga $C $j w udo.{x",ch, wield, victim, TO_NOTVICT );
		}
	}
	else
		return;

	if( get_curr_stat_deprecated(ch,STAT_DEX ) > 25 )
		dur = 5;
	else if ( get_curr_stat_deprecated(ch,STAT_DEX ) > 20 )
		dur = 4;
	else if ( get_curr_stat_deprecated(ch,STAT_DEX ) > 14 )
		dur = 3;
	else
		dur = 2;

	if( get_curr_stat_deprecated(victim,STAT_CON ) > 24 && dur > 2 )
		--dur;

	if( get_curr_stat_deprecated(ch,STAT_STR ) > 25 )
		mod = 3;
	else if ( get_curr_stat_deprecated(ch,STAT_STR ) > 19 )
		mod = 2;
	else
		mod = 1;

	if( get_curr_stat_deprecated(victim,STAT_CON ) > 24 && mod > 1 )
		--mod;

	jab.where = TO_AFFECTS;
	jab.type = gsn_thigh_jab;
	jab.level = 50;
	jab.duration = dur; jab.rt_duration = 0;
	jab.modifier = -mod;
	jab.location = APPLY_DEX;
	jab.bitvector = &AFF_SLOW;

	affect_to_char( victim, &jab, NULL, TRUE );

	return;
}

void trick_weapon_wrench( int sn, CHAR_DATA *ch, CHAR_DATA *victim )
{
	OBJ_DATA *whip, *wyrywana = NULL;
	int chance, dur;
	AFFECT_DATA abide;
    bool check;

	whip = get_eq_char( ch, WEAR_WIELD );

	if( IS_SET(ch->fight_data, FIGHT_TRICK_WIELDSECOND ) )
		whip = get_eq_char( ch, WEAR_SECOND );

	if( !whip )
		return;

	if( get_eq_char( victim, WEAR_WIELD ) && get_eq_char( victim, WEAR_SECOND ) )
		wyrywana = number_range(1,2) == 2 ? get_eq_char( victim, WEAR_WIELD ) : get_eq_char( victim, WEAR_SECOND );
	else if( get_eq_char( victim, WEAR_WIELD ) )
		wyrywana = get_eq_char( victim, WEAR_WIELD );
	else if( get_eq_char( victim, WEAR_SECOND ) )
		wyrywana = get_eq_char( victim, WEAR_SECOND );

	if( !wyrywana )
		return;

	chance = (3*get_skill(ch,gsn_disarm))/4;
	chance -= get_skill(victim,gsn_disarm)/4;

	if( IS_WEAPON_STAT(wyrywana, WEAPON_TWO_HANDS ) )
		chance -= chance/3;

    check = FALSE;
	if( whip == get_eq_char(ch,WEAR_SECOND ) && !check )
		chance += get_curr_stat_deprecated(ch,STAT_STR)*4;
	else
		chance += get_curr_stat_deprecated(ch,STAT_STR)*7;

	if( wyrywana == get_eq_char(victim,WEAR_SECOND ) && !check )
		chance -= get_curr_stat_deprecated(victim,STAT_STR)*4;
	else
		chance -= get_curr_stat_deprecated(victim,STAT_STR)*7;

	chance += UMAX(0,(get_curr_stat_deprecated(ch,STAT_DEX)-20)*4);

	if ( number_percent() < chance && !IS_OBJ_STAT( wyrywana, ITEM_NOREMOVE ) )
	{
		print_char( ch, "{vZwinnym ruchem owijasz %s wok� %s %s i wyrywasz przeciwnikowi bro� z r�k.{x\n\r{x", whip->name4, wyrywana->name2, victim->name2 );
		print_char( victim, "{v%s zwinnym ruchem owija %s wok� twej broni i wyrywa ci %s z r�k.{x\n\r{x", ch->name, whip->name4, wyrywana->name4 );
		act("{v$n zwinnym ruchem owija $h wok� broni $Z, po czym wyrywa mu j� z r�k.{x", ch, whip, victim, TO_NOTVICT );
		if ( is_artefact( wyrywana ) && !IS_NPC( victim ) && !IS_IMMORTAL( victim ) )
			artefact_from_char( wyrywana, victim );
		obj_from_char( wyrywana );
		if ( IS_OBJ_STAT( wyrywana, ITEM_NODROP ) || IS_OBJ_STAT( wyrywana, ITEM_INVENTORY ) )
		{
			if ( is_artefact( wyrywana ) && !IS_NPC( victim ) && !IS_IMMORTAL( victim ) )
				artefact_to_char( wyrywana, victim );
			obj_to_char( wyrywana, victim );
		}
		else
		{
			if( material_table[ wyrywana->material ].flag == MAT_METAL )
				act("$p uderza o ziemi� z g�o�nym brzd�kiem.", ch, wyrywana, NULL, TO_ALL );
			else
				act("$p uderza o ziemi� z g�o�nym stukotem.", ch, wyrywana, NULL, TO_ALL );
			obj_to_room( wyrywana, victim->in_room );
		}
	}
	else
	{
		dur = 1;

		if( victim->level < ch->level + 7 )
			++dur;

		if( get_curr_stat_deprecated(victim,STAT_STR) < get_curr_stat_deprecated(ch,STAT_STR) )
			++dur;

		if( get_skill(ch,gsn_disarm) > 90 && !get_skill(victim,gsn_disarm))
			++dur;

		print_char( ch, "{vZwinnym ruchem owijasz %s wok� %s %s i uniemo�liwiasz przeciwnikowi walk�.\n\r{x", whip->name4, wyrywana->name2, victim->name2 );
		print_char( victim, "{v%s zwinnym ruchem owija %s wok� twej broni i uniemo�liwia ci walk�.\n\r{x", ch->name, whip->name4 );
		act("{v$n zwinnym ruchem owija $h wok� broni $Z, uniemo�liwiaj�c mu tym sposobem walk�.{x", ch, whip, victim, TO_NOTVICT );

		if( wyrywana == get_eq_char(victim,WEAR_SECOND ) )
			abide.level = 51;
		else if( wyrywana == get_eq_char(victim,WEAR_WIELD ) && !IS_WEAPON_STAT(wyrywana, WEAPON_TWO_HANDS) )
			abide.level = 52;
		else
			abide.level = 50;

		abide.where = TO_AFFECTS;
		abide.type = gsn_weapon_wrench;
		abide.duration = dur; abide.rt_duration = 0;
		abide.location = APPLY_NONE;
		abide.modifier = 1;
		abide.bitvector = &AFF_NONE;
		affect_to_char( victim, &abide, NULL, FALSE );

		if( IS_SET(ch->fight_data, FIGHT_TRICK_WIELDSECOND ) )
			abide.modifier = -1;
		else
			abide.modifier = 0;

		affect_to_char( ch, &abide, NULL, FALSE );
	}

	return;
}

void trick_crushing_mace( int sn, CHAR_DATA *ch, CHAR_DATA *victim )
{
	OBJ_DATA *wield = get_eq_char( ch, WEAR_WIELD ), *body, *arms, *legs, *hands, *feet, *head, *hitted;
	int ok = 0, tried = 0;
	char buf[ MAX_STRING_LENGTH ];

	if( IS_SET(ch->fight_data, FIGHT_TRICK_WIELDSECOND ) )
		wield = get_eq_char( ch, WEAR_SECOND );

	if( !wield )
		return;

	body = get_eq_char( victim, WEAR_BODY );
	arms = get_eq_char( victim, WEAR_ARMS );
	legs = get_eq_char( victim, WEAR_LEGS );
	hands = get_eq_char( victim, WEAR_HANDS );
	feet = get_eq_char( victim, WEAR_FEET );
	head = get_eq_char( victim, WEAR_HEAD );

	do
	{
		++tried;
		switch (dice(1,10))
		{
			case 1:
			case 2:
			case 3:
				hitted = body;
				break;
			case 4:
				hitted = feet;
				break;
			case 5:
				hitted = head;
				break;
			case 6:
			case 7:
				hitted = arms;
				break;
			case 8:
			case 9:
				hitted = legs;
				break;
			case 10:
				hitted = hands;
				break;
		}

		if ( hitted == NULL )
			continue;

		ok = 1;
		if ( IS_OBJ_STAT(hitted, ITEM_UNDESTRUCTABLE) ||
			hitted->material == 6 ||
			hitted->material == 18 ||
			hitted->material == 24 ||
			hitted->material == 25 ||
			hitted->material == 26 ||
			hitted->material == 29 ||
			hitted->material == 30 ||
			hitted->material == 35 ||
			hitted->material == 37 ||
			hitted->material == 44 ||
			hitted->material == 45 ||
			hitted->material == 46 ||
			hitted->material == 47 )
		{
			print_char( ch, "{vWyprowadzasz %s silny i wywa�ony cios w %s %s, jednak efekt niczym si� nie r�ni od zwyk�ego ciosu.{x\n\r", wield->name5, hitted->name4, victim->name2 );
			print_char( victim, "{v%s wyprowadza silny i wywa�ony cios %s w %s %s, jednak efekt niczym si� nie r�ni od zwyk�ego ciosu.{x\n\r", IS_NPC( ch ) ? capitalize( ch->short_descr ) : ch->name, wield->name5, hitted->liczba_mnoga ? "twoje" : (hitted->gender == GENDER_ZENSKI ? "twoj�" : "tw�j"), hitted->name4 );
			sprintf( buf, "{v%s wyprowadza silny i wywa�ony cios %s w %s %s, jednak efekt niczym si� nie r�ni od zwyk�ego ciosu.{x\n\r", IS_NPC( ch ) ? capitalize( ch->short_descr ) : ch->name, wield->name5, hitted->name4, victim->name2 );
			act( buf, ch, NULL, victim, TO_NOTVICT );
			continue;
		}
		if ( hitted->material == 11 || hitted->material == 15 || hitted->material == 31 )
		{
			print_char( ch, "{vWyprowadzasz %s silny i wywa�ony cios, kt�ry przebija %s %s i wbija %s kawa�ki w %s cia�o, zwi�kszaj�c obra�enia.{x\n\r", wield->name5, hitted->name4, victim->name2, hitted->liczba_mnoga ? "ich" : (hitted->gender == GENDER_ZENSKI ? "jej" : "jego"), victim->sex == SEX_FEMALE ? "jej" : "jego" );
			print_char( victim, "{v%s wyprowadza silny i wywa�ony cios %s, kt�ry przebija %s %s i wbija %s kawa�ki w twoje cia�o, zwi�kszaj�c b�l.{x\n\r", IS_NPC( ch ) ? capitalize( ch->short_descr ) : ch->name, wield->name5, hitted->liczba_mnoga ? "twoje" : (hitted->gender == GENDER_ZENSKI ? "twoj�" : "tw�j"), hitted->name4, hitted->liczba_mnoga ? "ich" : (hitted->gender == GENDER_ZENSKI ? "jej" : "jego") );
			sprintf( buf, "{v%s wyprowadza silny i wywa�ony cios %s, kt�ry przebija %s %s i wbija %s kawa�ki w %s cia�o, zwi�kszaj�c obra�enia.{x\n\r", IS_NPC( ch ) ? capitalize( ch->short_descr ) : ch->name, wield->name5, hitted->name4, victim->name2, hitted->liczba_mnoga ? "ich" : (hitted->gender == GENDER_ZENSKI ? "jej" : "jego"), victim->sex == SEX_FEMALE ? "jej" : "jego" );
			act( buf, ch, NULL, victim, TO_NOTVICT );
			destroy_obj( hitted->carried_by, hitted );
			SET_BIT( ch->fight_data, FIGHT_CRUSHING_MACE_DAM );
			continue;
		}
		hitted->condition -= URANGE( 1, get_curr_stat_deprecated(ch,STAT_STR)-8, 20 );
		if (hitted->condition < 1 )
		{
			print_char( ch, "{vWyprowadzasz %s silny i wywa�ony cios, kt�ry przebija %s %s i wbija %s kawa�ki w %s cia�o, zwi�kszaj�c obra�enia.{x\n\r", wield->name5, hitted->name4, victim->name2, hitted->liczba_mnoga ? "ich" : (hitted->gender == GENDER_ZENSKI ? "jej" : "jego"), victim->sex == SEX_FEMALE ? "jej" : "jego" );
			print_char( victim, "{v%s wyprowadza silny i wywa�ony cios %s, kt�ry przebija %s %s i wbija %s kawa�ki w twoje cia�o, zwi�kszaj�c b�l.{x\n\r", IS_NPC( ch ) ? capitalize( ch->short_descr ) : ch->name, wield->name5, hitted->liczba_mnoga ? "twoje" : (hitted->gender == GENDER_ZENSKI ? "twoj�" : "tw�j"), hitted->name4, hitted->liczba_mnoga ? "ich" : (hitted->gender == GENDER_ZENSKI ? "jej" : "jego") );
			sprintf( buf, "{v%s wyprowadza silny i wywa�ony cios %s, kt�ry przebija %s %s i wbija %s kawa�ki w %s cia�o, zwi�kszaj�c obra�enia.{x\n\r", IS_NPC( ch ) ? capitalize( ch->short_descr ) : ch->name, wield->name5, hitted->name4, victim->name2, hitted->liczba_mnoga ? "ich" : (hitted->gender == GENDER_ZENSKI ? "jej" : "jego"), victim->sex == SEX_FEMALE ? "jej" : "jego" );
			act( buf, ch, NULL, victim, TO_NOTVICT );
			destroy_obj( hitted->carried_by, hitted );
			SET_BIT( ch->fight_data, FIGHT_CRUSHING_MACE_DAM );
		}
		else
		{
			print_char( ch, "{vWyprowadzasz %s silny i wywa�ony cios, kt�ry uszkadza %s %s.{x\n\r", wield->name5, hitted->name4, victim->name2 );
			print_char( victim, "{v%s wyprowadza silny i wywa�ony cios %s, kt�ry uszkadza %s %s.{x\n\r", IS_NPC( ch ) ? capitalize( ch->short_descr ) : ch->name, wield->name5, hitted->liczba_mnoga ? "twoje" : (hitted->gender == GENDER_ZENSKI ? "twoj�" : "tw�j"), hitted->name4 );
			sprintf( buf, "{v%s wyprowadza silny i wywa�ony cios %s, kt�ry uszkadza %s %s.{x\n\r", IS_NPC( ch ) ? capitalize( ch->short_descr ) : ch->name, wield->name5, hitted->name4, victim->name2 );
			act( buf, ch, NULL, victim, TO_NOTVICT );
		}
	} while( !ok && tried < 20 );

	return;
}

void trick_thousandslayer( int sn, CHAR_DATA *ch, CHAR_DATA *victim )
{
	//ten trik sie wykonuje w fight.c
	return;
}

void trick_glorious_impale( int sn, CHAR_DATA *ch, CHAR_DATA *victim )
{
	//opisy sa w fight.c
	OBJ_DATA *spear = get_eq_char( ch, WEAR_WIELD );
	AFFECT_DATA impale;
	int dur = URANGE(1,(get_curr_stat_deprecated(ch,STAT_STR) - 18)/2, 5 );

	if( IS_WEAPON_STAT( spear, WEAPON_SHARP ) )
		++dur;

	if( IS_WEAPON_STAT( spear, WEAPON_VORPAL ) )
		dur += 3;

	dur += (get_skill( ch, gsn_spear )+get_skill(ch, gsn_spear_mastery))/64;

	dur += (GET_AC( victim, AC_PIERCE )/10-5)/4;

	dur -= URANGE( 0, (get_curr_stat_deprecated(victim,STAT_CON)-18)/3, 4 );

	impale.where = TO_AFFECTS;
	impale.type = gsn_glorious_impale;
	impale.level = 50;
	impale.duration = URANGE( 1, dur, 4 ); impale.rt_duration = 0;
	impale.location = APPLY_NONE;
	impale.modifier = 0;
	impale.bitvector = &AFF_NONE;
	affect_to_char( ch, &impale, NULL, TRUE );

	impale.bitvector = &AFF_PARALYZE;
	impale.modifier = 1;
	affect_to_char( victim, &impale, NULL, TRUE );

	return;
}

void trick_divine_impact( int sn, CHAR_DATA *ch, CHAR_DATA *victim )
{
	CHAR_DATA *tch;
	int dam, skill = UMAX( get_skill(ch,gsn_smite),get_skill(ch,gsn_smite_good));

	if( IS_GOOD( ch ) )
	{
		act("{vWraz z ciosem $z z nieba sp�ywa z�oty strumie� boskiej energii.{x", ch, NULL, NULL, TO_ROOM );
		act("{vWraz z twym ciosem z nieba sp�ywa z�oty strumie� boskiej energii.{x", ch, NULL, NULL, TO_CHAR );
	}
	else
	{
		act("{vWraz z ciosem $z z nieba sp�ywa czerwony strumie� boskiej energii.{x", ch, NULL, NULL, TO_ROOM );
		act("{vWraz z twym ciosem z nieba sp�ywa czerwony strumie� boskiej energii.{x", ch, NULL, NULL, TO_CHAR );
	}

	for( tch = ch->in_room->people; tch; tch = tch->next_in_room )
	{
		if( ch == tch || is_same_group(ch,tch) )
			continue;

		if( IS_EVIL( ch ) && !IS_GOOD(tch ) )
			continue;

		if( IS_GOOD( ch ) && !IS_EVIL(tch ) )
			continue;

		dam = number_range( skill, skill*2 );
		dam = luck_dam_mod( ch, dam );
		if( stat_throw(ch,STAT_WIS) ) dam += dam/10;
		if( !stat_throw(ch,STAT_WIS) ) dam -= dam/10;
		if( stat_throw(ch,STAT_WIS) ) dam += dam/10;
		if( !stat_throw(ch,STAT_WIS) ) dam -= dam/10;
		if( stat_throw(ch,STAT_WIS) ) dam += dam/10;
		if( !stat_throw(ch,STAT_WIS) ) dam -= dam/10;
		if( stat_throw(ch,STAT_WIS) ) dam += dam/10;
		if( !stat_throw(ch,STAT_WIS) ) dam -= dam/10;

		act("{vStrumie� energii bole�nie pali tw� sk�r�!{x", tch, NULL, NULL, TO_CHAR );
		act("{v$n wyje z b�lu po zetkni�ciu ze strumieniem energii.{x", tch, NULL, NULL, TO_ROOM );

		if( is_affected( tch, gsn_mirror_image ) )
		{
			affect_strip( tch, gsn_mirror_image );
			send_to_char( "{vStrumie� energii niszczy wszystkie otaczaj�ce ci� lustrzane odbicia.\n\r{x", tch );
			act("{vStrumie� energii niszczy wszystkie lustrzane odbicia otaczaj�ce $c.{x", tch, NULL, NULL, TO_ROOM );
		}

		damage( ch, tch, dam, TYPE_UNDEFINED, DAM_HOLY, FALSE );
	}
}

void trick_dreadful_strike( int sn, CHAR_DATA *ch, CHAR_DATA *victim )
{
    AFFECT_DATA af;
    MOB_INDEX_DATA *mob;
    CHAR_DATA *zombie;
    char buf[ MAX_INPUT_LENGTH ];
    OBJ_DATA *corpse, *item, *item_next;
    int value, charisma_mod = get_curr_stat_deprecated( ch, STAT_CHA ) - 15, i, skill = get_skill( ch,gsn_torment);

    act("{vWykorzystujesz moment nieuwagi przeciwnika i z niezwyk�� brutalno�ci� zadajesz $X okropny cios.{x", ch, NULL, victim, TO_CHAR );
    act("{vZastygasz na u�amek sekundy w bezruchu. W oczach $n dostrzegasz dziwn� mg�� i w tym samym momencie $h opada na ciebie z niesamowit� si��.{x", ch, NULL, victim, TO_VICT );
    switch ( ch->sex )
    {
        case SEX_FEMALE:
            act("{vDostrzegasz jak $n zastyga na chwile w przera�eniu na widok $N, kt�re z niezwyk�� zawsi�to�ci� zadaje mu straszliwy cios.{x", ch, NULL, victim, TO_NOTVICT );
            break;
        case SEX_NEUTRAL:
            act("{vDostrzegasz jak $n zastyga na chwile w przera�eniu na widok $N, kt�ra z niezwyk�� zawsi�to�ci� zadaje mu straszliwy cios.{x", ch, NULL, victim, TO_NOTVICT );
            break;
        case SEX_MALE:
        default:
            act("{vDostrzegasz jak $n zastyga na chwile w przera�eniu na widok $N, kt�ry z niezwyk�� zawsi�to�ci� zadaje mu straszliwy cios.{x", ch, NULL, victim, TO_NOTVICT );
            break;
    }

    af.where = TO_AFFECTS;
    af.type = gsn_torment;
    af.level = 50;
    af.location = APPLY_NONE;
    af.duration = 20; af.rt_duration = 0;
    af.modifier = 0;
    af.bitvector = &AFF_NONE;
    af.visible = FALSE;
    affect_to_char( ch, &af, NULL, FALSE );

    value = 10 + ch->level * number_range( 7, 12 ) + 5*charisma_mod;
    value = ( 35 + (3 * skill) / 4 ) * value / 100;

    if( IS_EVIL(victim) )
    {
        value /= 4;
    }
    else if ( !IS_GOOD(victim ))
    {
        value /= 2;
    }

    check_improve( ch, victim, gsn_torment, TRUE, 5 );

    if ( victim->resists[RESIST_NEGATIVE] > 0 )
    {
        value = value*(100-victim->resists[RESIST_NEGATIVE])/100;
    }

    raw_damage( ch, victim, value );

    if( !victim || !victim->in_room || victim->hit < -10 )//no to robimy zombie
    {
        //szukamy pierwszego ciala na liscie
        for( corpse = ch->in_room->contents; corpse ; corpse = corpse->next )
        {
            if( corpse->item_type == ITEM_CORPSE_NPC )//znajdujemy
            {
                break;
            }
        }

        if( !corpse )
        {
            return;
        }

        if ( corpse->item_type == ITEM_CORPSE_NPC )
        {
            mob = get_mob_index( corpse->value[ 0 ] );
            if( !mob )
            {
                return;
            }

            if (
                    mob->size < SIZE_SMALL ||
                    mob->size > SIZE_LARGE ||
                    !IS_SET( race_table[ mob->race ].type, PERSON )
               )
            {
                return;
            }

            corpse->value[ 0 ] = 0;

            switch ( number_range( 1, 7 ) )
            {
                case 1:
                    if ( ( zombie = create_mobile( get_mob_index( MOB_VNUM_ZOMBIE ) ) ) == NULL )
                    {
                        return ;
                    }
                    /* making corpse */
                    /* name */
                    sprintf( buf, zombie->name, race_table[ mob->race ].name );
                    free_string( zombie->name );
                    zombie->name = str_dup( buf );

                    /* short */
                    sprintf( buf, zombie->short_descr, race_table[ mob->race ].name );
                    free_string( zombie->short_descr );
                    zombie->short_descr = str_dup( buf );

                    /* long */
                    sprintf( buf, zombie->long_descr, race_table[ mob->race ].name );
                    free_string( zombie->long_descr );
                    zombie->long_descr = str_dup( buf );
                    break;
                case 2:
                    if ( ( zombie = create_mobile( get_mob_index( MOB_VNUM_ZOMBIE1 ) ) ) == NULL )
                    {
                        return ;
                    }
                    break;
                case 3:
                    if ( ( zombie = create_mobile( get_mob_index( MOB_VNUM_ZOMBIE2 ) ) ) == NULL )
                    {
                        return ;
                    }
                    break;
                case 4:
                    if ( ( zombie = create_mobile( get_mob_index( MOB_VNUM_ZOMBIE3 ) ) ) == NULL )
                    {
                        return ;
                    }
                    break;
                case 5:
                    if ( ( zombie = create_mobile( get_mob_index( MOB_VNUM_ZOMBIE4 ) ) ) == NULL )
                    {
                        return ;
                    }
                    break;
                case 6:
                    if ( ( zombie = create_mobile( get_mob_index( MOB_VNUM_ZOMBIE5 ) ) ) == NULL )
                    {
                        return ;
                    }
                    break;
                case 7:
                    if ( ( zombie = create_mobile( get_mob_index( MOB_VNUM_ZOMBIE6 ) ) ) == NULL )
                    {
                        return ;
                    }
                    break;
            }

            zombie->spec_fun = NULL;
            money_reset_character_money( zombie );

            zombie->level = URANGE( 8, number_range( ch->level / 2, 2 * ch->level / 3 ), 16 );

            /* HP */
            for ( zombie->hit = 0, i = zombie->level; i >= 1; i-- )
            {
                zombie->hit += 16 + number_range( 1, 6 );
            }

            zombie->hit += ch->level;
            zombie->max_hit = zombie->hit;
            /* STATS */

            zombie->perm_stat[ STAT_STR ] = number_range(  50,  80 );
            zombie->perm_stat[ STAT_DEX ] = number_range(  60,  90 );
            zombie->perm_stat[ STAT_WIS ] = number_range(  60,  90 );
            zombie->perm_stat[ STAT_INT ] = number_range(  20,  40 );
            zombie->perm_stat[ STAT_CON ] = number_range( 100, 150 );
            zombie->perm_stat[ STAT_CHA ] = number_range(  20,  40 );
            zombie->perm_stat[ STAT_LUC ] = number_range(   1, 100 );

            zombie->size = mob->size;

            zombie->hit = get_max_hp(zombie);

            switch ( zombie->size )
            {
                case SIZE_SMALL:
                    zombie->weight = number_range( 250, 450 );
                    zombie->height = number_range( 40, 100 );
                    zombie->damage[ DICE_NUMBER ] = 2;
                    zombie->damage[ DICE_TYPE ] = 4;
                    zombie->perm_stat[ STAT_DEX ] += number_range( 0, 10 );
                    break;
                case SIZE_MEDIUM:
                    zombie->weight = number_range( 650, 1200 );
                    zombie->height = number_range( 140, 210 );
                    zombie->damage[ DICE_NUMBER ] = 2;
                    zombie->damage[ DICE_TYPE ] = 5;
                    zombie->perm_stat[ STAT_CON ] += number_range( 0, 10 );
                    break;
                case SIZE_LARGE:
                    zombie->weight = number_range( 850, 2200 );
                    zombie->height = number_range( 210, 260 );
                    zombie->damage[ DICE_NUMBER ] = 2;
                    zombie->damage[ DICE_TYPE ] = 6;
                    zombie->perm_stat[ STAT_STR ] += number_range( 0, 10 );
                    break;
                default: break;
            }

            zombie->damroll += URANGE( 0, ch->level / 7.5, 3 );

            for ( i = 0; i < 4; i++ )
            {
                zombie->armor[ i ] = 100 - 10 * ( zombie->level / 3 );
            }

            char_to_room( zombie, ch->in_room );
            EXT_SET_BIT( zombie->act, ACT_RAISED );
            EXT_SET_BIT( zombie->act, ACT_NO_EXP );

            act("Nagle $p powstaje.", zombie, corpse, NULL, TO_ROOM );

            for ( item = corpse->contains; item != NULL; item = item_next )
            {
                OBJ_NEXT_CONTENT( item, item_next );
                obj_from_obj( item );
                obj_to_char( item, zombie );
            }

            for ( item = zombie->carrying; item != NULL; item = item->next_content )
            {
                OBJ_NEXT( item, item_next );
                if ( item->item_type == ITEM_WEAPON )
                {
                    wield_weapon_silent( zombie, item, TRUE );
                }
                else
                {
                    wear_obj_silent( zombie, item, FALSE );
                }
            }

            extract_obj(corpse);

            if ( !add_charm( ch, zombie, FALSE ) )
            {
                EXT_SET_BIT( zombie->act, ACT_AGGRESSIVE );
                return ;
            }

            af.where = TO_AFFECTS;
            af.type = gsn_domination;
            af.level = ch->level;
            af.duration = -1; af.rt_duration = 0;
            af.location = 0;
            af.modifier = 0;
            af.bitvector = &AFF_CHARM;
            affect_to_char( zombie, &af, NULL, TRUE );

            add_follower( zombie, ch, TRUE );
        }
        else
        {
            return;
        }
    }
}
