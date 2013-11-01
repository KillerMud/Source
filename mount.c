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
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: mount.c 110 2013-07-18 08:34:31Z gurthg $
 * $HeadURL: svn+ssh://svn@killer.mud.pl/killer/branches/current/src/mount.c $
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
#include "act_obj.h"

void do_mount_on( CHAR_DATA *ch, CHAR_DATA *horse );
void one_hit  args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt, bool second ) );

// http://forum.mud.pl/viewtopic.php?t=6849

/**
 Wierzchowce i wszystkie moby z mountable.
 W dzisiejszych czasach u�ywa si� ich jako przeno�ne stoiska z floatem, dlatego
 chcia�bym poruszy� pare spraw, kt�re by� mo�e zaprocentuj� kiedy� zmianami
 w systemie wierzchowc�w. �eby nie przeci��a� systemu, zmiany mo�na by wprowadza� po kolei:

 I. Zm�czenie
 Konie powinny mie� swoj� pule mv, kt�re traci�y by przy przechodzeniu
 z lokacji do lokacji. Gracz niewprawiony w je�dzie konnej ( i nie tylko konnej )
 oczywi�cie te� by si� m�czy�, ale do�wiadczony je�dzie� traci�by mv bardzo wolno.
 Maj�c mistrza w Ridingu, stosunek by�by 10:1 - czyli 10 mv wierzchowca na 1 mv je�dzca.
 Ko� kt�ry straci�by ca�e mv, musia�by odpocz��, tak samo jak cz�owiek ( chocia� si�y regenerowa� by szybciej ).
 �eby nie bawi� si� w dodatkowe pola, mv moba by�y by naliczane na podstawie jego kondycji, rasy i size'u.

 Komendy: mount <nazwa> rest, mount <nazwa> stand
 Zmiany: Riding, Mount.

 II. Opieka nad Wierzchowcem
 Opr�cz oczywistych rzeczy, takich jak odpoczynek, ko� ( czy ichnia bestia )
 potrzebuje jedzenia i picia. Z piciem jest �atwo, starczy trafi� na jak��
 rzeczke przep�ywaj�c� przez trakt czy zaprowadzi� konia do koryta w stajni
 ( nie radze poi� z buk�ak�w ). Jednak z jedzeniem s� ju� pewne komplikacje.

 Wyr�niamy podzia� na pare typ�w:
 1. herbivorous - ro�lino�erne, pare sektor�w naturalnych lub pasza.
 2. carnivorous - mieso�erne, trup animala lub surowe mi�so.
 3. omnivorous - wszystko�erne, sektory naturalne, pasza, mi�so lub trup zwierzecia.
 4. unfeedable - Nie potrzebuje je�� i pi�, dotyczy istot pozasferowych, nieo�ywionych czy przywo�anych.

 Opr�cz tego w zale�no�ci od size moba, jedzenie czy picie b�dzie uzupe�nia�
 pewien procent paska. Np. ko� o size normal jedz�c pasze uzupe�ni 50% paska g�odu,
 inny ko� o size large uzupe�ni ju� tylko 40%. Gdyby by� jaki� ko� o size gigant,
 pasza uzupe�ni�a by mu tylko 10%. Dzia�a te� w drug� strone. Tak wi�c trzeba
 by by�o dba� o wieszchowca, by on by� zdatny do �ycia. Oczywi�cie nie musze dodawa�,
 �e g�odny i spragniony wierzchowiec wolniej regenerowa� by mv?

 Komendy: mount <nazwa> feed, mount <nazwa> feed <item>, mount <nazwa> water, mount <nazwa> water <item>
 Zmiany: Mount, Otype food ( doda� podzia� na rodzaj po�ywienia,
 np. mi�so, pasza, zielenina, racja podr�na ), naliczanie g�odu i pragnienia, size.
 */
void do_mount( CHAR_DATA *ch, char *argument ){
    char horsename [ MAX_STRING_LENGTH ];
    char order [ MAX_STRING_LENGTH ];
    char item [ MAX_STRING_LENGTH ];
    CHAR_DATA *horse = NULL;

    argument = one_argument( argument, horsename );
    argument = one_argument( argument, order );
    argument = one_argument( argument, item );

    horse = get_char_room( ch, horsename );
    if(!horse){
    	print_char(ch, "Nie ma tu nikogo takiego!\n\r");
    	return;
    }

	if ( !IS_NPC(horse) || !EXT_IS_SET(horse->act, ACT_MOUNTABLE ) )
	{
		print_char(ch, "Czy %s wygl�da jak co�, na czym mo�na je�dzi�?!\n\r", horse->short_descr );
		return;
	}

    //jesli tylko  jeden argument, to normalny mount
    if(!strcmp(order, "")){
    	//V. Stajnie
//    	if(str_prefix(horsename, "list"))
//    		do_mount_list(ch);
//    	if(str_prefix(horsename, "offer"))
//    		do_mount_offer(ch);
//    	else
    		do_mount_on(ch, horse);
    }
    else {
    	//I. Zm�czenie
		if(!str_prefix(order, "stand")){
			if(horse->mounting){
				print_char(ch, "Przecie� %s ma je�d�ca!\n\r", horse->name2 );
				return;
			}
			do_stand(horse, "");
			horse->default_pos = POS_STANDING;
		}
		else if(!str_prefix(order, "rest")){
			if(ch->mount){
				print_char(ch, "Zsi�d� najpierw z %s!\n\r", horse->name2 );
				return;
			}
			if(horse->mounting){
				print_char(ch, "Przecie� %s ma je�d�ca!\n\r", horse->name2 );
				return;
			}
			do_rest(horse, "");
			horse->default_pos = POS_RESTING;
		}
		//II. Opieka nad Wierzchowcem
		else if(!str_prefix(order, "feed")){
			do_feed_body(ch, horse->name, item);
		}
		else if(!str_prefix(order, "water"))
			do_water_body(ch, horse->name, item);
		//III. Prowadzenie wierzchowca
//		else if(str_prefix(order, "follow"))
//			do_rest(horse, "");
//		else if(str_prefix(order, "nofollow"))
//			do_rest(horse, "");
//		else if(str_prefix(order, "stay"))
//			do_rest(horse, "");
		//IV. Przedmioty
//		else if(str_prefix(order, "check"))
//			do_rest(horse, "");
//		else if(str_prefix(order, "put"))
//			do_rest(horse, "");
//		else if(str_prefix(order, "get"))
//			do_rest(horse, "");
//		else if(str_prefix(order, "equip"))
//			do_rest(horse, "");
//		else if(str_prefix(order, "remove"))
//			do_rest(horse, "");
        //V. Stajnie
//		else if(str_prefix(order, "rent"))
//			do_rest(horse, "");
//		else if(str_prefix(order, "bring"))
//			do_rest(horse, "");
		else
			print_char(ch, "Co?!\n\r" );
    }

}


void do_mount_on( CHAR_DATA *ch,  CHAR_DATA *horse )
{
	bool show;
	int skill = get_skill( ch, gsn_riding );

	if ( ch->mount )
	{
		print_char(ch, "Przecie� dosiadasz ju� %s!\n\r", ch->mount->name4);
		return;
	}

	if ( horse == ch )
	{
		send_to_char( "Czego chcesz dosi���?\n\r", ch );
		return;
	}

	if ( !IS_NPC(horse) || !EXT_IS_SET(horse->act, ACT_MOUNTABLE ) )
	{
		print_char(ch, "Nie mo�esz dosi��� %s!\n\r", horse->name2 );
		return;
	}

	if ( horse->mounting )
	{
		print_char(ch, "%s ju� ma je�dzca.\n\r", horse->short_descr );
		return;
	}

	if ( horse->master && horse->master != ch )
	{
		act("$N nie pozwala ci na to.", ch, NULL, horse->master, TO_CHAR );
		return;
	}

	if ( horse->position < POS_STANDING )
	{
		send_to_char( "Wierzchowiec musi sta�, aby mo�na by�o na niego wsi���.\n\r", ch );
		return;
	}

	if ( horse->fighting )
	{
		print_char(ch, "%s teraz walczy, nie dasz rady wsi���.\n\r", horse->short_descr );
		return;
	}

    if ( is_fearing( horse, ch ) )
    {
        print_char(ch, "%s zbyt si� ciebie boi, nie dasz rady wsi���.\n\r", horse->short_descr );
        if ( number_percent() < 5 ) {
            one_hit( horse, ch, TYPE_UNDEFINED, FALSE );
        }
        return;
    }

	if ( ch->move <= 0 )
	{
		act( "Jeste� zbyt zm�czon<&y/a/e> by dosi��� wierzchowca.", ch, NULL, horse, TO_CHAR );
		return;
	}

	if (IS_AFFECTED(ch, AFF_SNEAK) || IS_AFFECTED(ch, AFF_HIDE)) {
		switch (horse->sex) {
			case SEX_NEUTRAL:
				act( "Pr�bujesz dosi��� $C, ale robisz to tak niespodziewanie, �e $N umyka przestraszone.", ch, NULL, horse, TO_CHAR );
				break;
			case SEX_MALE:
				act( "Pr�bujesz dosi��� $C, ale robisz to tak niespodziewanie, �e $N umyka przestraszony.", ch, NULL, horse, TO_CHAR );
				break;
			case SEX_FEMALE:
			default :
				act( "Pr�bujesz dosi��� $C, ale robisz to tak niespodziewanie, �e $N umyka przestraszona.", ch, NULL, horse, TO_CHAR );
				break;
		}
		ch->move -= number_range(1,4);
		if ( ch->move < 0 )
		{
			ch->move = 0;
		}
		return;
	}

	if(IS_AFFECTED(ch, AFF_FLYING))
    {
        affect_strip(ch, gsn_fly);
    }
	if(IS_AFFECTED(ch, AFF_FLOAT))
    {
        affect_strip(ch, gsn_float);
    }

	if ( skill == 0 || number_percent() > skill + 5 )
	{
		act( "Nieudolnie pr�bujesz dosi��� $C i po chwili l�dujesz na ziemi.", ch, NULL, horse, TO_CHAR );
		act( "$n nieudolnie pr�buje dosi��� $C i po chwili l�duje na ziemi.", ch, NULL, horse, TO_NOTVICT );
		ch->position = POS_SITTING;
		ch->move -= number_range(0,4);
		if ( ch->weight > 900 )
		{
			ch->move -= number_range(0,2);
		}
		ch->move = UMAX(0,ch->move);
		check_improve( ch, NULL, gsn_riding, TRUE, 70 );
		return;
	}

    ch->mount = horse;
    horse->mounting = ch;
	horse->default_pos = POS_STANDING;
	horse->position = POS_STANDING;
	show = TRUE;

	if ( IS_NPC( horse ) && HAS_TRIGGER( horse, TRIG_MOUNT ) )
    {
		show = !mp_percent_trigger( horse, ch, NULL, NULL, &TRIG_MOUNT );
    }
	if ( IS_NPC( horse ) && HAS_TRIGGER( horse, TRIG_DISMOUNT ) )
    {
		show = FALSE;
    }

	if ( show )
	{
		act( "Umiej�tnie dosiadasz $C.", ch, NULL, horse, TO_CHAR );
		act( "$n umiej�tnie dosiada $C.", ch, NULL, horse, TO_NOTVICT );
		check_improve( ch, NULL, gsn_riding, TRUE, 10 );
	}
	return;
}

void do_dismount_body( CHAR_DATA *ch )
{
    if ( ch->mount )
    {
        ch->mount->mounting = NULL;
        ch->mount = NULL;
    }
    else if ( ch->mounting )
    {
        ch->mounting->mount = NULL;
        ch->mounting = NULL;
    }
    else
    {
		bug( "Cos nie tak z do_dismount_body (%d).", 1 );
    }
}

void do_dismount( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA *victim;
	bool show;
	int skill = get_skill( ch, gsn_riding );

	if ( (victim = ch->mount) == NULL )
    {
        send_to_char( "Przecie� nie dosiadasz teraz �adnego wierzchowca.\n\r", ch );
        return;
	}


	if ( ch->position == POS_SLEEPING )
	  {
	    send_to_char( "�nisz o zsiadaniu z konia.\n\r", ch );
	    return;
	  }
	
	do_dismount_body(ch);

	ch->position = POS_STANDING;

	show = TRUE;
	if ( IS_NPC( victim ) && HAS_TRIGGER( victim, TRIG_DISMOUNT ) )
    {
		show = !mp_percent_trigger( victim, ch, NULL, NULL, &TRIG_DISMOUNT );
    }

	if ( IS_NPC( victim ) && HAS_TRIGGER( victim, TRIG_MOUNT ) )
    {
		show = FALSE;
    }

	if ( show )
	{
		if ( skill == 0 || number_percent() > skill + 5 )
        {
            act( "Nieudolnie pr�bujesz zsi��� z grzbietu $Z i po chwili l�dujesz na ziemi.", ch, NULL, victim, TO_CHAR );
            act( "$n nieudolnie pr�buje zsi��� z grzbietu $Z i po chwili l�duje na ziemi.", ch, NULL, victim, TO_NOTVICT );
            ch->position = POS_SITTING;
            ch->move -= number_range(0,4);
            if ( ch->weight > 900 )
            {
                ch->move -= number_range(0,2);
            }
            if ( ch->move < 0 )
            {
                ch->move = 0;
            }
            check_improve( ch, NULL, gsn_riding, TRUE, 80 );
        }
		else
		{
			act("Zr�cznie zeskakujesz z grzbietu $Z.", ch, NULL, victim, TO_CHAR );
			act("$n zr�cznie zeskakuje z grzbietu $Z.", ch, NULL, victim, TO_NOTVICT );
			check_improve( ch, NULL, gsn_riding, TRUE, 70 );
		}
	}
	return;
}

