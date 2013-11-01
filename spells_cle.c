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
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (tzdziech@gmail.com            ) [Agron     ] *
 * Mikolajski Krystian   (krs256@gmail.com              ) [Raszer    ]                                                                    *
 ***********************************************************************
 *
 * $Id: spells_cle.c 155 2013-08-08 20:50:23Z drake $
 * $HeadURL: svn+ssh://svn@killer.mud.pl/killer/branches/current/src/spells_cle.c $
 *
 */
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
/*
 *
 * CZARY KAP�A�SKIE
 *
 */

extern char *	target_name;

bool check_dispel          args( ( int dis_level, CHAR_DATA *victim, int sn ) );
bool check_improve_strenth args( ( CHAR_DATA *ch, CHAR_DATA *victim, bool verbose ) );
bool remove_obj            args( ( CHAR_DATA *ch, int iWear, bool fReplace ) );
bool saves_dispel          args( ( int dis_level, int spell_level, int duration ) );
void raw_kill              args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void set_fighting          args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );

void heal_char( CHAR_DATA *ch, CHAR_DATA *victim, sh_int value, bool all )
{
	if ( ! ch || ! victim )
    {
		return;
    }
	sh_int heal_value;
	int _heal, luck = get_curr_stat( ch, STAT_LUC ), tmp, i;
	AFFECT_DATA *aff_bandage;

	int exp_gain = 0;

	if ( !all && ( is_undead( victim ) || IS_SET( victim->form, FORM_CONSTRUCT ) ) )
		return;

	if ( is_affected( ch, gsn_life_transfer ) )
	{
		send_to_char( "Na razie nie masz do�� si� witalnych.\n\r", ch );
		return;
	}

	_heal = victim->hit;

	// modyfikator zale�ny od szcz�cia dodatni
	if ( number_range( 0, luck ) > 90 ) _heal = ( _heal * 11 ) / 10;
	// modyfikator zale�ny od szcz�cia ujemny
	if ( number_range( 0, luck/6 + LUCK_BASE_MOD ) < 3 ) _heal = ( _heal * 9 ) / 10;

	if ( IS_NPC( ch ) )
    {
		value *= 4;
        value /= 3;
    }

	if ( victim->position != POS_FIGHTING && victim->fighting == NULL )
	{
		if ( is_affected ( victim, gsn_bandage ) )
		{
			aff_bandage = affect_find ( victim->affected, gsn_bandage );
			if ( aff_bandage->modifier > 0 )
			{
				value += ( value * aff_bandage->modifier ) / 100;
				send_to_char( "Czujesz lekkie szczypanie pod opatrunkiem, ktory najwyra�niej zwi�kszy� lecznicza moc czaru.\n\r", victim );
				send_to_char( "Czujesz, �e pob�ogos�awione banda�e zwi�kszaj� lecznicza moc twojego czaru.\n\r", ch );
			}
		}
	}

	heal_value = UMIN( get_max_hp( victim ) + 11 - victim->hit, value );
	heal_value = 100 * heal_value / get_max_hp( victim );

	/* exp dla castera za leczenie. najpierw procentowo, potem dzielimy przez 100*/

	exp_gain = number_range( 90, 110 ) * IS_NPC( victim ) ? 50 : 130;
	exp_gain = UMIN( 300, (exp_gain * UMIN( value, (get_max_hp(victim) - victim->hit) ) ) / 100 );
	if ( ch != victim )
	   gain_exp( ch, exp_gain, TRUE );

	victim->hit = UMIN( victim->hit + value, get_max_hp( victim ) );
	tmp = value;

	for ( i = 0; i <= 2; i++ )
	{
		if ( tmp <= victim->counter[ i ] )
		{
			victim->counter[ i ] -= tmp;
			tmp = 0;
		}
		else
		{
			tmp -= victim->counter[ i ];
			victim->counter[ i ] = 0;
		}
	}

	_heal = victim->hit - _heal;

	if ( _heal < 1 )
		return;

	if ( heal_value > 75 )
	{
		send_to_char( "Twoje cia�o wype�nia lecznicze ciep�o, prawie wszystkie twoje rany goj� si�.\n\r", victim );
		act( "Prawie wszystkie rany $z goj� si�.", victim, NULL, NULL, TO_ROOM );
	}
	else if ( heal_value > 50 )
	{
		send_to_char( "Twoje cia�o wype�nia lecznicze ciep�o, wiekszo�� twoich ran goi si�.\n\r", victim );
		act( "Wiekszo�� ran $z goi si�.", victim, NULL, NULL, TO_ROOM );
	}
	else if ( heal_value > 25 )
	{
		send_to_char( "Twoje cia�o wype�nia lecznicze ciep�o, kilka twoich g�ebokich ran goi si�.\n\r", victim );
		act( "Kilka g��bokich ran $z goi si�.", victim, NULL, NULL, TO_ROOM );
	}
	else if ( heal_value > 10 )
	{
		send_to_char( "Twoje cia�o wype�nia lecznicze ciep�o, kilka twoich ran goi si�.\n\r", victim );
		act( "Kilka ran $z goi si�.", victim, NULL, NULL, TO_ROOM );
	}
	else if ( heal_value > 5 )
	{
		send_to_char( "Czujesz jak przez twoje cia�o przep�ywa fala ciep�a, kilka zranie� znika.\n\r", victim );
		act( "Kilka zranie� $z znika.", victim, NULL, NULL, TO_ROOM );
	}
	else if ( heal_value > 0 )
	{
		send_to_char( "Twoje cia�o wype�nia lecznicze ciep�o, kilka siniak�w znika.\n\r", victim );
		act( "Niekt�re z siniak�w $z znikaj�.", victim, NULL, NULL, TO_ROOM );
	}
	update_pos( victim );
	return;
}

void heal_undead( CHAR_DATA *ch, CHAR_DATA *victim, sh_int value )
{
	sh_int heal_value;
	int _heal, luck = get_curr_stat( ch, STAT_LUC );

	if ( !is_undead( victim ) )
    {
		return;
    }

    if ( ch == victim )
    {
		send_to_char( "W�a�ciwie to nic si� nie dzieje.\n\r", ch );
        return;
    }

	_heal = victim->hit;

	// modyfikator zale�ny od szcz�cia dodatni
	if ( number_range( 0, luck ) > 90 ) _heal = ( _heal * 105 ) / 100;
	// modyfikator zale�ny od szcz�cia ujemny
	if ( number_range( 0, luck/6 + LUCK_BASE_MOD ) < 3 ) _heal = ( _heal * 9 ) / 10;

	heal_value = UMIN( get_max_hp( victim ) + 11 - victim->hit, value );
	heal_value = 100 * heal_value / get_max_hp( victim );
	victim->hit = UMIN( victim->hit + value, get_max_hp( victim ) );
	_heal = victim->hit - _heal;

	if ( _heal < 1 )
    {
		return;
    }

    if ( heal_value > 75 )
    {
        act( "Negatywna energia wype�nia ca�� pow�ok� $z wszczepiaj�c w ni� nowe �ycie.", victim, NULL, NULL, TO_ROOM );
        if ( ! IS_NPC( victim ) )
        {
            send_to_char( "Negatywna energia wype�nia ca�<&ego/�/e> ciebie i wszczepia nowe �ycie.\n\r", victim );
        }
    }
    else if ( heal_value > 50 )
    {
        act( "Wi�kszo�� ubytk�w $z regeneruje si�.", victim, NULL, NULL, TO_ROOM );
        if ( ! IS_NPC( victim ) )
        {
            send_to_char( "Wi�kszo�� twoich ubytk�w regeneruje si�.\n\r", victim );
        }
    }
    else if ( heal_value > 25 )
    {
        act( "Kilka powa�niejszych ubytk�w $z regeneruje si�.", victim, NULL, NULL, TO_ROOM );
        if ( ! IS_NPC( victim ) )
        {
            send_to_char( "Kilka twoich powa�niejszych ubytk�w regeneruje si�.\n\r", victim );
        }
    }
    else if ( heal_value > 10 )
    {
        act( "Kilka ubytk�w $z regeneruje si�.", victim, NULL, NULL, TO_ROOM );
        if ( ! IS_NPC( victim ) )
        {
            send_to_char( "Kilka twoich ubytk�w regeneruje si�.\n\r", victim );
        }
    }
    else if ( heal_value > 5 )
    {
        act( "Kilka mniejszych ubytk�w $z regeneruje si�.", victim, NULL, NULL, TO_ROOM );
        if ( ! IS_NPC( victim ) )
        {
            send_to_char( "Kilka twoich mniejszych ubytk�w regeneruje si�.\n\r", victim );
        }
    }
    else if ( heal_value > 0 )
    {
        act( "Jeden ma�y ubytek $z regeneruje si�.", victim, NULL, NULL, TO_ROOM );
        if ( ! IS_NPC( victim ) )
        {
            send_to_char( "Tw�j jeden ma�y ubytek regeneruje si�.\n\r", victim );
        }
    }
    update_pos( victim );
    return;
}

/* LECZENIA (I RANIENIA) W KOLEJNOSCI SILY */

/* leczenie lekkich ran (skladnia: cast 'cure light' [kto])*/
void spell_cure_light( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
    bool check = FALSE;
    AFFECT_DATA *paf;
    int holy_heal_mod = 100, luck = get_curr_stat( ch, STAT_LUC );;
    bool pool_mod = TRUE;
	
	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 2 )
	{
		send_to_char( "Nie uda�o ci si�.\n\r", ch );
		return;
	}	

    if ( ( paf = affect_find( ch->affected, gsn_prayer_last )) != NULL )
    {
         if (paf->modifier > 24 && paf->modifier < 36)
         {
            holy_heal_mod = 50;
            pool_mod = FALSE;
            send_to_char( "Czujesz jak brak modlitwy os�abia twoj� moc.\n\r", ch );
         }

         if (paf->modifier >= 36)
         {
            send_to_char( "Inkantujesz zakl�cie lecz�ce, jednak nie mo�esz znale�� nawet iskry mocy w wype�niaj�cej cie pustce.\n\r", ch );
            return;
         }

    }
    if ( ( paf = affect_find( ch->affected, gsn_holy_pool )) != NULL  && pool_mod == TRUE)
       {
           if(paf->modifier > 20 && number_percent() > URANGE(21,paf->modifier,40))
           {
             paf->modifier -= 1;
             holy_heal_mod += 30;

           }

           if(paf->modifier > 40 && number_percent() > URANGE(21,paf->modifier,40))
           {
             paf->modifier -= 1;
             holy_heal_mod += 30;

           }

           if(paf->modifier > 60 && number_percent() > URANGE(21,paf->modifier,40))
           {
             paf->modifier -= 1;
             holy_heal_mod += 30;

           }
           //minusy
           if (paf->modifier < -80)
           {
              if(number_percent() + ( paf->modifier / 2 ) > 0 )
              {
              send_to_char( "Pr�bujesz zebra� w sobie si�� lecz�c�, jednak czujesz, �e boska moc zosta�a ci zabrana.\n\r", ch );
              return;
              }else
              {
               send_to_char( "Pr�bujesz zebra� w sobie si�� lecz�c�, po chwili twoje serce wype�nia przera�enie, z twoim zakl�ciem sta�o si� co� okropnego.\n\r", ch );
               if ( spell_damage( ch, ( CHAR_DATA * ) vo, 6 + dice( 1, 5 ) + URANGE(1, level/2, 15) , sn, DAM_NEGATIVE, TRUE ) == TRUE )
                 {
		         send_to_char( "Czujesz si� gorzej.\n\r", victim );
                 if ( ch != victim )
			     act( "$n czuje si� gorzej.", victim, NULL, NULL, TO_ROOM );
			     return;
                 }

              }
          }
           if (paf->modifier < -60 )
           {
              if(number_percent() - paf->modifier/2 > 0)
              {
              holy_heal_mod -= 30;
              }else
              {
              send_to_char( "Pr�bujesz zebra� w sobie si�� lecz�c�, jednak czujesz, �e boska moc zosta�a ci zabrana.\n\r", ch );
              return;
              }
           }
           if (paf->modifier < -40) holy_heal_mod -= 30;
           if (paf->modifier < -20) holy_heal_mod -= 30;
       }
     if(holy_heal_mod > 100) send_to_char( " Czujesz jak si�a twojego zakl�cia ro�nie. \n\r", ch);
     if(holy_heal_mod < 100) send_to_char( " Czujesz jak si�a twojego zakl�cia s�abnie. \n\r", ch);



	if ( IS_NPC( ch ) || ch->class == CLASS_CLERIC || check )
    {
		heal_char( ch, victim, 6 + dice( 1, 5 ) + URANGE(1, level/2, 15), FALSE );
    }
	else
    {
		heal_char( ch, victim, ((6 + dice( 1, 5 ) + URANGE(1, level/2, 15))*holy_heal_mod)/100, FALSE );
    }
}

int cause_damage_helper( int kind, int level )
{
    return ( 10 * kind + dice( 2 * kind, 4) + URANGE( 0, (level * kind)/2, kind * 15 ) );
}
			
/* zadawanie lekkich ran (skladnia: cast 'cause light' [kto]) */
void spell_cause_light( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA * victim = ( CHAR_DATA * ) vo;
    int dam, luck = get_curr_stat( ch, STAT_LUC );
	
	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 2 )
	{
		send_to_char( "Nie uda�o ci si�.\n\r", ch );
		return;
	}	
	
    if ( is_undead( victim ) || IS_SET( victim->form, FORM_CONSTRUCT ) )
    {
        return;
    }
    dam = luck_dam_mod( ch, cause_damage_helper( 1, level ) );
    if ( spell_damage( ch, ( CHAR_DATA * ) vo, dam , sn, DAM_NEGATIVE, TRUE ) == TRUE )
    {
        send_to_char( "Czujesz si� gorzej.\n\r", victim );
        if ( ch != victim )
        {
            act( "$n czuje si� gorzej.", victim, NULL, NULL, TO_ROOM );
        }
    }
    return;
}

/* leczenie �rednich ran (skladnia: cast 'cure moderate wounds' [kto]) */
void spell_cure_moderate( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA *paf;
    int holy_heal_mod = 100, luck = get_curr_stat( ch, STAT_LUC );;
    bool pool_mod = TRUE;
	
	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 3 )
	{
		send_to_char( "Nie uda�o ci si�.\n\r", ch );
		return;
	}		
	
if ( ( paf = affect_find( ch->affected, gsn_prayer_last )) != NULL )
    {
         if (paf->modifier > 24 && paf->modifier < 36)
         {
            holy_heal_mod = 50;
            pool_mod = FALSE;
            send_to_char( "Czujesz jak brak modlitwy os�abia tw� moc.\n\r", ch );
         }

         if (paf->modifier > 36)
         {
            send_to_char( "Inkantujesz zakl�cie lecz�ce, jednak nie mo�esz znale�� nawet iskry mocy w wype�niaj�cej cie pustce.\n\r", ch );
            return;
         }

    }
    if ( ( paf = affect_find( ch->affected, gsn_holy_pool )) != NULL  && pool_mod == TRUE)
       {
           if(paf->modifier > 20 && number_percent() > URANGE(10,paf->modifier/2,40))
           {
             paf->modifier -= 1;
             holy_heal_mod += 30;

           }

           if(paf->modifier > 40 && number_percent() > URANGE(10,paf->modifier/3,40))
           {
             paf->modifier -= 1;
             holy_heal_mod += 30;

           }

           if(paf->modifier > 60 && number_percent() > URANGE(10,paf->modifier/3,40))
           {
             paf->modifier -= 1;
             holy_heal_mod += 30;

           }
           //minusy
           if (paf->modifier < -80)
           {
              if(number_percent() + (paf->modifier/2) > 0)
              {
              send_to_char( "Pr�bujesz zebra� w sobie si�� lecz�c�, jednak czujesz, �e boska moc zosta�a ci zabrana.\n\r", ch );
              return;
              }else
              {
                send_to_char( "Pr�bujesz zebra� w sobie si�� lecz�c�, po chwili twoje serce wype�nia przera�enie, z twoim zakl�ciem sta�o si� co� okropnego.\n\r", ch );

              if ( spell_damage( ch, ( CHAR_DATA * ) vo, 6 + dice( 1, 5 ) + URANGE(1, level/2, 15) , sn, DAM_NEGATIVE, TRUE ) == TRUE )
              {
		         send_to_char( "Czujesz si� gorzej.\n\r", victim );
                 if ( ch != victim )
			     act( "$n czuje si� gorzej.", victim, NULL, NULL, TO_ROOM );
			     return;
              }

              }
           }
           if (paf->modifier < -60 )
           {
              if(number_percent() - paf->modifier/2 > 0)
              {
              holy_heal_mod -= 30;
              }else
              {
              send_to_char( "Pr�bujesz zebra� w sobie si�� lecz�c�, jednak czujesz, �e boska moc zosta�a ci zabrana.\n\r", ch );
              return;
              }
           }
           if (paf->modifier < -40) holy_heal_mod -= 30;
           if (paf->modifier < -20) holy_heal_mod -= 30;
       }
     if(holy_heal_mod > 100) send_to_char( " Czujesz jak si�a twojego zakl�cia ro�nie. \n\r", ch);
     if(holy_heal_mod < 100) send_to_char( " Czujesz jak si�a twojego zakl�cia s�abnie. \n\r", ch);

	if ( IS_NPC( ch ) || ch->class == CLASS_CLERIC || ch->class == CLASS_DRUID )
    {
		heal_char( ch, victim, 15 + dice( 3, 6 ) + URANGE(5, level, 30), FALSE );
    }
	else
    {
		heal_char( ch, victim, ((15 + dice( 3, 6 ) + URANGE(5, level, 30))*holy_heal_mod)/100, FALSE );
    }
}

void spell_cause_moderate( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA * victim = ( CHAR_DATA * ) vo;
    int dam, luck = get_curr_stat( ch, STAT_LUC );
	
	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 3 )
	{
		send_to_char( "Nie uda�o ci si�.\n\r", ch );
		return;
	}	
	
    if ( is_undead( victim ) || IS_SET( victim->form, FORM_CONSTRUCT ) )
    {
        return;
    }
    dam = luck_dam_mod( ch, cause_damage_helper( 2, level ) );
    if ( spell_damage( ch, ( CHAR_DATA * ) vo, dam , sn, DAM_NEGATIVE, TRUE ) == TRUE )
    {
        send_to_char( "Czujesz si� �le.\n\r", victim );
        if ( ch != victim )
        {
            act( "$n czuje si� �le.", victim, NULL, NULL, TO_ROOM );
        }
    }
    return;
}

/* leczenie powaznych ran (skladnia: cast 'cure serious' [kto]) */
void spell_cure_serious( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA * victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA *paf;
    int holy_heal_mod = 100, luck = get_curr_stat( ch, STAT_LUC );
    bool pool_mod = TRUE;
	
	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 4 )
	{
		send_to_char( "Nie uda�o ci si�.\n\r", ch );
		return;
	}	
	
    if ( ( paf = affect_find( ch->affected, gsn_prayer_last )) != NULL )
    {
        if (paf->modifier > 24 && paf->modifier < 36)
        {
            holy_heal_mod = 50;
            pool_mod = FALSE;
            send_to_char( "Czujesz jak brak modlitwy os�abia tw� moc.\n\r", ch );
        }

        if (paf->modifier > 36)
        {
            send_to_char( "Inkantujesz zakl�cie lecz�ce, jednak nie mo�esz znale�� nawet iskry mocy w wype�niaj�cej cie pustce.\n\r", ch );
            return;
        }

    }
    if ( ( paf = affect_find( ch->affected, gsn_holy_pool )) != NULL  && pool_mod == TRUE)
    {
        if(paf->modifier > 20 && number_percent() > URANGE(10,paf->modifier/2,40))
        {
            paf->modifier -= 2;
            holy_heal_mod += 30;

        }

        if(paf->modifier > 40 && number_percent() > URANGE(10,paf->modifier/3,40))
        {
            paf->modifier -= 1;
            holy_heal_mod += 30;

        }

        if(paf->modifier > 60 && number_percent() > URANGE(10,paf->modifier/3,40))
        {
            paf->modifier -= 1;
            holy_heal_mod += 30;

        }
        //minusy
        if (paf->modifier < -80)
        {
            if(number_percent() + (paf->modifier/2) > 0)
            {
                send_to_char( "Pr�bujesz zebra� w sobie si�� lecz�c�, jednak czujesz, �e boska moc zosta�a ci zabrana.\n\r", ch );
                return;
            }else
            {
                send_to_char( "Pr�bujesz zebra� w sobie si�� lecz�c�, po chwili twoje serce wype�nia przera�enie, z twoim zakl�ciem sta�o si� co� okropnego.\n\r", ch );

                if ( spell_damage( ch, ( CHAR_DATA * ) vo, 6 + dice( 1, 5 ) + URANGE(1, level/2, 15) , sn, DAM_NEGATIVE, TRUE ) == TRUE )
                {
                    send_to_char( "Czujesz si� gorzej.\n\r", victim );
                    if ( ch != victim )
                        act( "$n czuje si� gorzej.", victim, NULL, NULL, TO_ROOM );
                    return;
                }

            }
        }
        if (paf->modifier < -60 )
        {
            if(number_percent() - paf->modifier/2 > 0)
            {
                holy_heal_mod -= 30;
            }else
            {
                send_to_char( "Pr�bujesz zebra� w sobie si�� lecz�c�, jednak czujesz, �e boska moc zosta�a ci zabrana.\n\r", ch );
                return;
            }
        }
        if (paf->modifier < -40) holy_heal_mod -= 30;
        if (paf->modifier < -20) holy_heal_mod -= 30;
    }
    if(holy_heal_mod > 100) send_to_char( " Czujesz jak si�a twojego zakl�cia ro�nie. \n\r", ch);
    if(holy_heal_mod < 100) send_to_char( " Czujesz jak si�a twojego zakl�cia s�abnie. \n\r", ch);

    if ( IS_NPC( ch ) || ch->class == CLASS_CLERIC || ch->class == CLASS_DRUID )
    {
        heal_char( ch, victim, 20 + dice( 4, 6 ) + URANGE(10, (level*2)/3, 50), FALSE );
    }
    else
    {
        if(ch->class == CLASS_PALADIN)
        {
            heal_char( ch, victim,(( 20 + dice( 4, 6 ) + URANGE(10, (level*2)/3, 50))*holy_heal_mod)/100, FALSE );
            //Raszer zmiany w cs dla paladyna. Pal ma najwyzej 19 level magiczny.
        }
    }
}

/* zadawanie powaznych ran (skladnia: cast 'cause seriuos' <kto>) */
void spell_cause_serious( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA * victim = ( CHAR_DATA * ) vo;
    int dam, luck = get_curr_stat( ch, STAT_LUC );
	
	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 4 )
	{
		send_to_char( "Nie uda�o ci si�.\n\r", ch );
		return;
	}
	
    if ( is_undead( victim ) || IS_SET( victim->form, FORM_CONSTRUCT ) )
    {
        return;
    }
    dam = luck_dam_mod( ch, cause_damage_helper( 3, level ) );
    if ( spell_damage( ch, ( CHAR_DATA * ) vo, dam , sn, DAM_NEGATIVE, TRUE ) == TRUE )
    {
        send_to_char( "Czujesz si� du�o gorzej.\n\r", victim );
        if ( ch != victim )
        {
            act( "$n czuje si� du�o gorzej.", victim, NULL, NULL, TO_ROOM );
        }
    }
    return;
}

/* leczenie krytycznych ran (skladnia: cast 'cure critical' [kto]) */
void spell_cure_critical( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int luck = get_curr_stat( ch, STAT_LUC );
	
	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 5 )
	{
		send_to_char( "Nie uda�o ci si�.\n\r", ch );
		return;
	}

	if ( IS_NPC( ch ) || ch->class == CLASS_CLERIC )
		heal_char( ch, victim, 50 + dice( 6, 6 ) + URANGE(10, level*2, 55), FALSE ); // 66-141 
		else
		heal_char( ch, victim, 50 + dice( 6, 6 ) + URANGE(0, level*2, 50), FALSE );

	return;
}

/* zadawanie krytycznych ran (skladnia: cast 'cause critical' <kto>) */
void spell_cause_critical( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA * victim = ( CHAR_DATA * ) vo;
    int dam, luck = get_curr_stat( ch, STAT_LUC );
	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 5 )
	{
		send_to_char( "Nie uda�o ci si�.\n\r", ch );
		return;
	}
    if ( is_undead( victim ) || IS_SET( victim->form, FORM_CONSTRUCT ) )
    {
        return;
    }
    dam = luck_dam_mod( ch, cause_damage_helper( 4, level ) );
    if ( spell_damage( ch, ( CHAR_DATA * ) vo, dam , sn, DAM_NEGATIVE, TRUE ) == TRUE )
    {
        send_to_char( "Czujesz si� naprawd� �le.\n\r", victim );
        if ( ch != victim )
        {
            act( "$n czuje si� naprawd� �le.", victim, NULL, NULL, TO_ROOM );
        }
    }
    return;
}

/* Uzdrowienie (skladnia: cast heal [kto]) */
void spell_heal( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int luck = get_curr_stat( ch, STAT_LUC );

	
	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 6 )
	{
		send_to_char( "Nie uda�o ci si�.\n\r", ch );
		return;
	}	
	
	if ( !IS_NPC( ch ) )
	{
	heal_char( ch, victim, 55 + dice( 10, 5 ) + URANGE(10, level*2, 80), FALSE ); // 75-175 
	}
	else
	{
	heal_char( ch, victim, 150 + dice( 10, 20 ) , FALSE );
	}
	
	return;
}


void spell_harm( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int dam, luck = get_curr_stat( ch, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 10 )
	{
		send_to_char( "Nie uda�o ci si�.\n\r", ch );
		return;
	}

	if ( is_undead( victim ) || IS_SET( victim->form, FORM_CONSTRUCT ) )
		return;

		
	if ( IS_NPC( ch ) )
	{
	dam = 150 + dice( 10, 20 );
	dam = luck_dam_mod( ch, dam );
	}
   	else if( !spell_item_check( ch, sn , "komponent do harm" ) )
    {
		dam = luck_dam_mod( ch, cause_damage_helper( 5, level ) );
    }
    else
	{
	dam = luck_dam_mod( ch, cause_damage_helper( 6, level ) );
	}

	spell_damage( ch, victim, dam, sn, DAM_NEGATIVE , TRUE );

	return;
}

/* KONIEC LECZEN (RANIEN) */

/* blogoslawienstwo (skladnia: cast bless [kto])
 *
 * ogolnie ma byc:
 * na grupke/goscia:  na  bron  : HR +(1-2)
 * HR +(2-3)              zbroje: AC -(2-3)
 * VS -(1-2)              inne  : VS -(1-2)
 */
void spell_bless( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim;
	OBJ_DATA *obj;
	AFFECT_DATA af;
	int s_level, s_duration;
	int luck = get_curr_stat( ch, STAT_LUC );

	if ( target == TARGET_OBJ )
	{
		obj = ( OBJ_DATA * ) vo;

		if ( IS_OBJ_STAT( obj, ITEM_BLESS ) )
		{
            switch(obj->gender)
            {
                case GENDER_MESKI:
    			    act( "$p jest ju� pob�ogos�awiony.", ch, obj, NULL, TO_CHAR );
                    break;
                case GENDER_ZENSKI:
    			    act( "$p jest ju� pob�ogos�awiona.", ch, obj, NULL, TO_CHAR );
                    break;
                case GENDER_NIJAKI:
    			    act( "$p jest ju� pob�ogos�awione.", ch, obj, NULL, TO_CHAR );
                    break;
                case GENDER_MESKOOSOBOWY:
    			    act( "$p s� ju� pob�ogos�awione.", ch, obj, NULL, TO_CHAR );
                    break;
                case GENDER_NONE:
                default:
        			send_to_char( "Ten przedmiot jest ju� pob�ogos�awiony.\n\r", ch );
                    break;
            }
			return;
		}

		if ( IS_OBJ_STAT( obj, ITEM_EVIL ) )
		{
			AFFECT_DATA * paf;

			paf = affect_find( obj->affected, gsn_curse );
			if ( !saves_dispel( level, paf != NULL ? paf->level : 1, 0 ) )
			{
				if ( paf != NULL ) affect_remove_obj( obj, paf );
				act( "Zimna aura z�a wok� $f rozp�ywa si�.", ch, obj, NULL, TO_ALL );
				EXT_REMOVE_BIT( obj->extra_flags, ITEM_EVIL );
				return;
			}
			else
			{
				act( "Nic nie poradzisz wobec mrocznej magii $f.", ch, obj, NULL, TO_CHAR );
				return;
			}
		}

		if ( number_range( 0, luck/6 + LUCK_BASE_MOD ) == 0 )
		{
			send_to_char( "Nie uda�o ci si� wyb�aga� b�ogos�awie�stwa.\n\r", ch );
			return;
		}

		s_level = level - 10;
		if ( number_range( 0, luck ) > 60 ) s_level += 5;
		if ( number_range( 0, luck ) > 90 ) s_level += 5;
		if ( number_range( 0, luck ) < 30 ) s_level -= 5;
		if ( number_range( 0, luck ) < 18 ) s_level -= 5;

		s_duration = 4 + level / 4;
		if ( number_range( 0, luck ) < 30 ) s_duration -= 1;
		if ( number_range( 0, luck ) < 18 ) s_duration -= 1;

		if ( obj->item_type == ITEM_WEAPON )
		{
			af.where = TO_OBJECT;
			af.type = sn;
			af.level = UMAX( 1, s_level );
			af.duration = UMAX( 1, s_duration ); af.rt_duration = 0;
			af.bitvector = &ITEM_BLESS;
			af.location = APPLY_HITROLL;
			af.modifier = 1 + ( level > 15 ? 1 : 0 );
			affect_to_obj( obj, &af );
		}
		else
		{
			act( "$p nie da si� pob�ogos�awi�.", ch, obj, NULL, TO_CHAR );
			return;
		}

		act( "$p p�onie b�ogos�awion� aur�.", ch, obj, NULL, TO_ALL );
		return;
	}

	victim = ( CHAR_DATA * ) vo;

	if ( is_affected( victim, sn ) )
	{
		if ( victim == ch )
		{
			send_to_char( "Ju� jeste� b�ogos�awion<&y/a/e>.\n\r", victim );
		}
		else
		{
			switch ( victim->sex )
			{
				case 0:
					act( "$N ju� jest b�ogos�awione.", ch, NULL, victim, TO_CHAR );
					break;
				case 2:
					act( "$N ju� jest b�ogos�awiona.", ch, NULL, victim, TO_CHAR );
					break;
				default:
					act( "$N ju� jest b�ogos�awiony.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}
		return;
	}

	if ( victim->class == CLASS_BLACK_KNIGHT && IS_GOOD(ch) || is_undead(victim) )
	{
		send_to_char( "Zakl�cie nie zadzia�a�o.\n\r", ch );
		return;
	}

	if ( number_range( 0, luck/6 + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda�o ci si� wyb�aga� b�ogos�awie�stwa.\n\r", ch );
		return;
	}

	s_level = level - 10;
	if ( number_range( 0, luck ) > 90 ) s_level += 5;
	if ( number_range( 0, luck ) > 60 ) s_level += 5;
	if ( number_range( 0, luck ) < 30 ) s_level -= 5;
	if ( number_range( 0, luck ) < 18 ) s_level -= 5;

	s_duration = 4 + level / 7;
	if ( number_range( 0, luck ) > 90 ) s_duration += 1 + level / 6;
	if ( number_range( 0, luck ) > 60 ) s_duration += level / 6;
	if ( number_range( 0, luck ) < 30 ) s_duration -= 1;
	if ( number_range( 0, luck ) < 18 ) s_duration -= 1;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = UMAX( 1, s_level );
	af.duration = UMAX( 1, s_duration ); af.rt_duration = 0;
	af.location = APPLY_HITROLL;
	af.modifier = 1 + ( level > 15 ? 1 : 0 );
	af.bitvector = &AFF_NONE;

	affect_to_char( victim, &af, NULL, TRUE );

	af.location = APPLY_SAVING_SPELL;
	af.modifier = -1 - ( level > 15 ? 1 : 0 );
	affect_to_char( victim, &af, NULL, TRUE );

	act( "Na $c sp�ywa boskie b�ogos�awie�stwo.", victim, NULL, NULL, TO_ROOM );
	if ( victim == ch ) send_to_char( "Sam<&/a/o> siebie b�ogos�awisz.\n\r", victim );
	else act( "Za po�rednictwem $z sp�ywa na ciebie boskie b�ogos�awie�stwo.", ch, NULL, victim, TO_VICT );

	return;
}

/* Oczyszczenie pokarmu (skladnia: cast 'purify food' <co>)
 * odswieza plyny, jedzenie
 */
void spell_purify_food( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj = ( OBJ_DATA * ) vo;
	int luck = get_curr_stat( ch, STAT_LUC );
	if ( obj->item_type != ITEM_DRINK_CON && obj->item_type != ITEM_FOOD )
	{
		send_to_char( "Tylko jedzenie i picie mo�esz w ten spos�b oczy�ci�.\n\r", ch );
		return;
	}
	if ( !obj->value[ 3 ] )
	{
		act( "$p nie jest zatrute.", ch, obj, NULL, TO_CHAR );
		return;
	}
	if ( number_range( 0, luck ) > 5 )
	{
		obj->value[ 3 ] = 0;
		act( "Usuwasz trucizn� z $f.", ch, obj, NULL, TO_CHAR );
		act( "$n usuwa trucizn� z $f.", ch, obj, NULL, TO_ROOM );
	}
	else
    {
		act( "Nie uda�o ci si� usun�� trucizny z $f.", ch, obj, NULL, TO_CHAR );
    }
	return;
}

/*
 * S�up ognia (skladnia: cast flamestrike <kto>)
 */

void spell_flamestrike( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
//    OBJ_DATA *obj;
	int dam, luck = get_curr_stat( ch, STAT_LUC ), type = 0;

	if ( number_range( 0, luck/6 + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda�o ci si� przywo�a� kolumny p�omieni.\n\r", ch );
		return;
	}

	if ( !IS_OUTSIDE( ch ) || IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_NOWEATHER ) )
	{
		type = 0;
		dam = number_range( 15, 25 ) + dice( level, 4 );
	}
	else if( ch->level > 25 && number_percent() < 5*(get_curr_stat( ch, STAT_WIS)/6-20) )
	{
		type = 2;
		if( ch->class == CLASS_DRUID )
			dam = number_range( 15, 35 ) + dice( level, 5 ) + number_range( 10, (level+get_curr_stat( ch, STAT_WIS)/6+get_curr_stat(ch,STAT_LUC)/6-10)/2 );
		else
			dam = number_range( 15, 35 ) + dice( level, 5 ) + number_range( 4, (level+get_curr_stat( ch, STAT_WIS)/6+get_curr_stat(ch,STAT_LUC)/6-10)/2 );
	}
	else
	{
		type = 1;
		if( ch->class == CLASS_DRUID )
			dam = number_range( 15, 35 ) + dice( level, 4 ) + number_range( 5, (level+get_curr_stat( ch, STAT_WIS)/6)/3 );
		else
			dam = number_range( 15, 35 ) + dice( level, 4 ) + number_range( 0, (level+get_curr_stat( ch, STAT_WIS)/6)/3 );
	}

	dam = luck_dam_mod( ch, dam );

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		dam -= dam / 4;
		if ( type != 2 && saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		{
			dam -= dam / 4;
			if( type != 1 )
			{
            	if ( ch == victim )
	                act( "Otaczasz si� kolumn� p�omieni.", ch, NULL, victim, TO_CHAR );
            	else
	                act( "Kolumna p�omieni obejmuje $C.", ch, NULL, victim, TO_CHAR );
				act( "Kolumna rycz�cych p�omieni obejmuje ci�.", ch, NULL, victim, TO_VICT );
				act( "Kolumna rycz�cych p�omieni obejmuje $C.", ch, NULL, victim, TO_NOTVICT );
			}
			else
			{
                act( "S�up ognia sp�ywa z nieba dos�ownie stop� od ciebie przypalaj�c bole�nie tw� sk�r�.", ch, NULL, victim, TO_VICT );
				act( "S�up ognia sp�ywa z nieba dos�ownie stop� od $z przypalaj�c bole�nie $s sk�r�.", victim, NULL, NULL, TO_ROOM );
			}
		}
		else
        {
        	if( type == 2 )
			{
                act( "S�up ognia sp�ywa z nieba prosto na ciebie, pokrywaj�c ci� <&ca�ego/ca��/ca�e> p�omieniami.", ch, NULL, victim, TO_VICT );
				act( "S�up ognia sp�ywa z nieba prosto na $c, pokrywaj�c ca�e $s cia�o p�omieniami.", victim, NULL, NULL, TO_ROOM );
			}
        	else if( type == 1 )
			{
                act( "S�up ognia sp�ywa z nieba prosto na ciebie, pokrywaj�c ci� p�omieniami prawie w ca�o�ci.", ch, NULL, victim, TO_VICT );
                if( victim->sex == SEX_FEMALE )
					act( "S�up ognia sp�ywa z nieba prosto na $c, pokrywaj�c j� prawie w ca�o�ci p�omieniami.", victim, NULL, NULL, TO_ROOM );
				else
					act( "S�up ognia sp�ywa z nieba prosto na $c, pokrywaj�c go prawie w ca�o�ci p�omieniami.", victim, NULL, NULL, TO_ROOM );
			}
        	else
        	{
            	if ( ch == victim )
	                act( "Otaczasz si� kolumn� pot�nych p�omieni.", ch, NULL, victim, TO_CHAR );
            	else
	               	act( "Kolumna pot�nych p�omieni otacza $C.", ch, NULL, victim, TO_CHAR );
				act( "Kolumna rycz�cych p�omieni otacza ci�.", ch, NULL, victim, TO_VICT );
				act( "Kolumna rycz�cych p�omieni otacza $C.", ch, NULL, victim, TO_NOTVICT );
			}
        }
	}
	else
    {
    	if( type == 2 )
		{
			act( "Olbrzymi s�up ognia sp�ywa z nieba prosto na ciebie, prawie toniesz w p�omieniach!", ch, NULL, victim, TO_VICT );
			act( "Olbrzymi s�up ognia sp�ywa z nieba prosto na $c, $s cia�o prawie tonie w p�omienich!", victim, NULL, NULL, TO_ROOM );
			DAZE_STATE( victim, URANGE( 0, 25 - get_curr_stat( victim, STAT_CON )/6, 12 ));
		}
		else if( type == 1 )
		{
			act( "S�up ognia sp�ywa z nieba prosto na ciebie, pokrywaj�c ci� <&ca�ego/ca��/ca�e> p�omieniami.", ch, NULL, victim, TO_VICT );
			act( "S�up ognia sp�ywa z nieba prosto na $c, pokrywaj�c ca�e $s cia�o p�omieniami.", victim, NULL, NULL, TO_ROOM );
		}
		else
		{
        	if ( ch == victim )
	            act( "Otaczasz si� ca�kowicie kolumn� rycz�cych p�omieni!", ch, NULL, victim, TO_CHAR );
        	else
	       		act( "Kolumna rycz�cych p�omieni ca�kowicie otacza $C!", ch, NULL, victim, TO_CHAR );
			act( "Kolumna rycz�cych p�omieni spada na ciebie!", ch, NULL, victim, TO_VICT );
			act( "Kolumna rycz�cych p�omieni opada na $C.", ch, NULL, victim, TO_NOTVICT );
		}
    }
    if( spell_item_check( ch, sn , "fire based spell" ) )
    {
      dam += dam/5;
    }

	spell_damage( ch, victim, dam, sn, DAM_FIRE, FALSE );
	fire_effect( victim, level, dam, TARGET_CHAR );
	return;
}

/* Swiatlo (skladnia: cast 'light')
 *
 * no stworzy kule swiatla (nie mam teraz pomyslu na nic innego)
 * ew moze zrobic aureolke bedzie po klerycku *giggle*
 */

void spell_light( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj;
	int luck = get_curr_stat( ch, STAT_LUC );
	int timer;
	AFFECT_DATA *paf;


	if ( !IS_NPC( ch ) && number_range( 0, luck/6 + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "W ostatniej chwili rozpraszasz swoj� uwag�.\n\r", ch );
		return;
	}

	timer = 10 + level / 10;
	if ( number_range( 0, luck ) > 90 ) timer += 2;
	if ( number_range( 0, luck ) < 18 ) timer -= 3;


	if ( ( paf = affect_find( ch->affected, gsn_prayer_last )) != NULL )
    {
        if ( paf->modifier < 30 )
        {
             if ( ( paf = affect_find( ch->affected, gsn_holy_pool )) != NULL )
                  {

                    if ( number_percent() - paf->modifier * 2 < 0 && number_percent() < 30 )
                    {
                       send_to_char( "Czujesz jak zakl�cie rezonuje z zebran� w tobie moc�.\n\r", ch );
	                   timer += 10;
	                   obj = create_object( get_obj_index( OBJ_VNUM_GOOD_BALL ), FALSE );
                       paf->modifier -= 10;
	                   if (paf->modifier < 1) paf->modifier = 1;
	                   obj_to_char( obj, ch );
                       obj->timer = UMAX( 1, timer );
                       act( "Na r�ku $z pojawia si� mlecznobia�a kula �wiat�a.", ch, obj, NULL, TO_ROOM );
		               act( "Na twoim r�ku pojawia si� mlecznobia�a kula �wiat�a.", ch, obj, NULL, TO_CHAR );
                       return;
                    }

                    if (number_percent() < (-paf->modifier))
                    {
                       send_to_char( "Czujesz jak wype�niaj�cy ci� mrok uniemo�liwia ci zebranie �wiat�a.\n\r", ch );
	                   return;

                    }


                  }


        }else
        {
             send_to_char( "Brak modlitwy powoduje i� �wiat�o gromadz�ce si� na twojej d�oni wygasa.\n\r", ch );
	         return;

        }

    }
    obj = create_object( get_obj_index( OBJ_VNUM_LIGHT_BALL ), FALSE );
	obj_to_char( obj, ch );
	obj->timer = UMAX( 1, timer );

	act( "Na r�ku $z pojawia si� kula �wiat�a.", ch, obj, NULL, TO_ROOM );
	act( "Na twoim r�ku pojawia si� kula �wiat�a.", ch, obj, NULL, TO_CHAR );
	return;
}

void spell_spirit_light( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    OBJ_DATA * obj;
    int luck = get_curr_stat( ch, STAT_LUC );
    int timer;

    if ( !IS_NPC( ch ) && number_range( 0, luck/6 + LUCK_BASE_MOD ) == 0 )
    {
        send_to_char( "W ostatniej chwili rozpraszasz swoj� uwag�.\n\r", ch );
        return;
    }

    timer = 10 + level / 10;
    if ( number_range( 0, luck ) > 90 )
    {
        timer += 2;
    }
    else if ( number_range( 0, luck ) < 18 )
    {
        timer -= 2;
    }

    if ( IS_EVIL( ch ) )
    {
        obj = create_object( get_obj_index( OBJ_VNUM_EVIL_BALL ), FALSE );
        act( "Na r�ku $z pojawia si� ciemnoczerwona, pulsuj�ca kula �wiat�a.", ch, obj, NULL, TO_ROOM );
        act( "Na twoim r�ku pojawia si� ciemnoczerwona, pulsuj�ca kula �wiat�a.", ch, obj, NULL, TO_CHAR );
    }
    else if ( IS_GOOD( ch ) )
    {
        obj = create_object( get_obj_index( OBJ_VNUM_GOOD_BALL ), FALSE );
        act( "Na r�ku $z pojawia si� mlecznobia�a kula �wiat�a.", ch, obj, NULL, TO_ROOM );
        act( "Na twoim r�ku pojawia si� mlecznobia�a kula �wiat�a.", ch, obj, NULL, TO_CHAR );
    }
    else if ( IS_NEUTRAL( ch ) )
    {
        obj = create_object( get_obj_index( OBJ_VNUM_NEUTRAL_BALL ), FALSE );
        act( "Na r�ku $z pojawia si� b��kitna kula boskiego �wiat�a.", ch, obj, NULL, TO_ROOM );
        act( "Na twoim r�ku pojawia si� b��kitna kula boskiego �wiat�a.", ch, obj, NULL, TO_CHAR );
    }
    else

    {
        send_to_char( "W�a�ciwie to nic si� nie dzieje.\n\r", ch );
        return;
    }
    obj_to_char( obj, ch );
    obj->timer = UMAX( 1, timer );
}

void spell_spirit_hammer( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   OBJ_DATA * obj = NULL;
   int value, skill = get_skill ( ch, gsn_mace );
   int luck = get_curr_stat( ch, STAT_LUC );

   if ( number_range( 0, LUCK_BASE_MOD + luck/6 ) < 1 )
      {
	 send_to_char( "Nie uda�o ci si� przywo�a� widmowego m�ota.\n\r", ch );
	 return;
      }

   if ( get_hand_slots( ch, WEAR_WIELD ) >= 2 )
      {
	 send_to_char( "Masz ju� dwie zaj�te r�ce. Zdejmij co�.\n\r", ch );
	 return;
      }

   if ( get_eq_char( ch, WEAR_WIELD ) )
      {
	 send_to_char( "Nosisz ju� jak�� bro�. Zdejmij j�.\n\r", ch );
	 return;
      }

   obj = create_object( get_obj_index( OBJ_VNUM_SPIRITUAL_HAMMER ), FALSE );

   if ( !obj )
      {
	 send_to_char( "Puff ... i nic.\n\r", ch );
	 bug( "Brak obiektu w dla czaru spell_spiritual_weapon.", 0 );
	 return;
      }

   EXT_SET_BIT( obj->extra_flags, ITEM_GLOW );
   EXT_SET_BIT( obj->extra_flags, ITEM_MAGIC );

   value = 2 * level;
   /*
    * moby dluzej
    * PC - zalezy od szczescia
    */
   if ( IS_NPC ( ch ) )
      {
	 value *= 4;
      }
   else
      {
	 if ( number_range( 0, luck ) > 90 ) value *= 2;
	 if ( number_range( 0, luck ) < 30 ) value /= 2;
      }
   obj->timer = value;

   value = 2 * level;
   if ( number_range( 0, luck ) > 90 ) value = ( value * 9 ) / 10;
   if ( number_range( 0, luck ) < 30 ) value = ( value * 115 ) / 100;
   obj->rent_cost = value;

   value = level / 3;
   if ( number_range( 0, luck ) > 60 ) ++value;
   if ( number_range( 0, luck ) < 18 ) --value;

   /*
    * dodaje bonus za posiadane umiejetnosci w danej broni
    */
   if ( skill > 50 ) { value++; }
   if ( skill > 30 ) { value++; }
   if ( skill > 10 ) { value++; }

   value = UMAX( 0, value );

   /*
    * mobom limitujemy mozliwe do wyloswania bronie
    */
   if ( IS_NPC ( ch ) )
      {
	 value = UMIN ( 8, value );
      }

   switch ( value )
      {
      case 0:
	 obj->value[ 1 ] = 1;
	 obj->value[ 2 ] = 4;
	 obj->value[ 5 ] = 1;
	 obj->value[ 6 ] = 1;
	 break;
      case 1:
	 obj->value[ 1 ] = 1;
	 obj->value[ 2 ] = 5;
	 obj->value[ 5 ] = 1;
	 obj->value[ 6 ] = 1;
	 break;
      case 2:
	 obj->value[ 1 ] = 1;
	 obj->value[ 2 ] = 6;
	 obj->value[ 5 ] = 1;
	 obj->value[ 6 ] = 1;
	 break;
      case 3:
	 obj->value[ 1 ] = 1;
	 obj->value[ 2 ] = 5;
	 obj->value[ 5 ] = 2;
	 obj->value[ 6 ] = 1;
	 break;
      case 4:
	 obj->value[ 1 ] = 1;
	 obj->value[ 2 ] = 5;
	 obj->value[ 5 ] = 3;
	 obj->value[ 6 ] = 1;
	 break;
      case 5:
	 obj->value[ 1 ] = 1;
	 obj->value[ 2 ] = 5;
	 obj->value[ 5 ] = 4;
	 obj->value[ 6 ] = 1;
	 break;
      case 6:
	 obj->value[ 1 ] = 2;
	 obj->value[ 2 ] = 4;
	 obj->value[ 5 ] = 1;
	 obj->value[ 6 ] = 1;
	 break;
      case 7:
	 obj->value[ 1 ] = 2;
	 obj->value[ 2 ] = 4;
	 obj->value[ 5 ] = 2;
	 obj->value[ 6 ] = 1;
	 break;
      case 8:
	 obj->value[ 1 ] = 2;
	 obj->value[ 2 ] = 4;
	 obj->value[ 5 ] = 3;
	 obj->value[ 6 ] = 1;
	 break;
      case 9:
	 obj->value[ 1 ] = 2;
	 obj->value[ 2 ] = 4;
	 obj->value[ 5 ] = 2;
	 obj->value[ 6 ] = 2;
	 break;
      case 10:
	 obj->value[ 1 ] = 2;
	 obj->value[ 2 ] = 5;
	 obj->value[ 5 ] = 2;
	 obj->value[ 6 ] = 2;
	 break;
      case 11:
	 obj->value[ 1 ] = 2;
	 obj->value[ 2 ] = 4;
	 obj->value[ 5 ] = 3;
	 obj->value[ 6 ] = 3;
	 break;
      default:
	 obj->value[ 1 ] = 2;
	 obj->value[ 2 ] = 4;
	 obj->value[ 5 ] = 4;
	 obj->value[ 6 ] = 4;
	 break;
      }

   /**
    * dodatkowy bonus broni
    */

   if ( !IS_NPC ( ch ) )
      {

	 if ( value > 6 && number_percent() < 30 )
	    {
	       SPEC_DAMAGE *specdam = new_spec_damage();

	       specdam->type   = SD_TYPE_ALIGN;
	       // FIXME: doda� zale�no�ci od alignu klera
	       specdam->param1 = 0; // align ofiary
	       specdam->param2 = 1; // align atakujacego

	       specdam->target_type = SD_TARGET_DAM;
	       specdam->bonus  = URANGE ( 1, level/5, 7 );
	       specdam->chance = URANGE ( 10, 2*level, 70 );

	       specdam->next  = obj->spec_dam;
	       obj->spec_dam = specdam;
	    }

	 if ( value > 7 && number_percent() < 20 )
	    {
	       SPEC_DAMAGE *specdam = new_spec_damage();

	       specdam->type   = SD_TYPE_MAGIC;
	       specdam->param1 = DAM_HOLY;
	       specdam->param2 = 0;
	       specdam->bonus  = URANGE ( 50, 10*level, 200 );
	       specdam->chance = URANGE ( 1, level/10, 4 );
	       specdam->next  = obj->spec_dam;
	       obj->spec_dam = specdam;
	    }

	 if ( value > 9 && number_percent() < 10 )
	    {
	       SPEC_DAMAGE *specdam = new_spec_damage();

	       specdam->type   = SD_TYPE_ALIGN;
	       // FIXME: doda� zale�no�ci od alignu klera
	       specdam->param1 = 0; // align ofiary
	       specdam->param2 = 1; // align atakujacego

	       specdam->target_type = SD_TARGET_MAG;
	       specdam->bonus  = URANGE ( 1, level/10, 3 );
	       specdam->chance = URANGE ( 10, 10+2*level, 70 );

	       if ( IS_IMMORTAL( ch ) || ( value > 6 && number_percent() < 30 ) )
		  {
		     switch ( number_range( 1, 3 * LEVEL_HERO - level ) )
			{
			case 1:
			   SET_BIT( obj->value[ 4 ], WEAPON_FLAMING );
			   break;
			case 2:
			   SET_BIT( obj->value[ 4 ], WEAPON_FROST );
			   break;
			case 3:
			   SET_BIT( obj->value[ 4 ], WEAPON_SHOCKING );
			   break;
			case 4:
			   SET_BIT( obj->value[ 4 ], WEAPON_POISON );
			   break;
			case 5:
			   SET_BIT( obj->value[ 4 ], WEAPON_TOXIC );
			   break;
			case 6:
			   SET_BIT( obj->value[ 4 ], WEAPON_SACRED );
			   break;
			case 7:
			   SET_BIT( obj->value[ 4 ], WEAPON_RESONANT );
			   break;
			case 8:
			   add_new_affect( obj, 0, APPLY_SKILL, gsn_mace, number_range( 1, level/3 ), &AFF_NONE, 0, TRUE );
			   break;
			default:
			   break;
			}
		  }
	    }
      }

   /*
    * logowanie jaka bron zostala stworzona, ale tylko gracze
    */
   if ( !IS_NPC ( ch ) )
      {
	 sprintf
            (
             log_buf,
             "[%d] spell_spirit_hammer: %s (%d), skill: %d, chance: %d",
             ch->in_room ? ch->in_room->vnum : 0,
             IS_NPC( ch )? ch->short_descr : ch->name,
             ch->level,
             skill,
             value
	     );
	 log_string( log_buf );
      }

   obj_to_char( obj, ch );
   equip_char( ch, obj, WEAR_WIELD, TRUE );

   act( "W d�oni $z pojawia si� widmowy m�ot.", ch, NULL, NULL, TO_ROOM );
   send_to_char( "W twojej d�oni pojawia si� widmowy m�ot.\n\r", ch );

   // - Tener -
   // UWAGA: Nie nale�y zwalnia� zaalokowanego specdama, chyba �e si� go nie wykorzysta�o!
   //        Zakomentowuj� zamiast usun��, by ostrze�enie zosta�o dla potomno�ci
   //
   // free_spec_damage( specdam );

   return;
}

OBJ_DATA * spiritual_armor_obj_helper( int vnum, int timer, int level, int pierce, int bash, int slash, int exotic )
{
    OBJ_DATA * obj = create_object( get_obj_index( vnum ), FALSE );
    obj->timer      = timer;
    obj->cost       = 0;
    obj->rent_cost  = 2 * level;
    obj->value[ 0 ] = pierce;
    obj->value[ 1 ] = bash;
    obj->value[ 2 ] = slash;
    obj->value[ 3 ] = exotic;
    obj->value[ 4 ] = 0;
    obj->value[ 5 ] = 0;
    obj->value[ 6 ] = 0;
    return obj;
}

void spell_spiritual_armor( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    OBJ_DATA * obj = NULL;
    int timer, pierce, bash, slash, exotic;
    extern int wear_to_itemwear[ MAX_WEAR ];
    int luck = get_curr_stat( ch, STAT_LUC );

    timer = 5 + level / 2;
    if ( number_range( 0, luck ) > 90 )
    {
        timer *= 2;
    }
    if ( number_range( 0, luck ) < 30 )
    {
        timer /= 2;
    }

    pierce = 4 - ( level / 7.5 );
    if ( number_range( 0, luck ) > 90 ) pierce -= 1;
    if ( number_range( 0, luck ) < 30 ) pierce += 2;

    slash = 4 - ( level / 7.5 );
    if ( number_range( 0, luck ) > 90 ) slash -= 1;
    if ( number_range( 0, luck ) < 30 ) slash += 2;

    bash = 4 - ( level / 7.5 );
    if ( number_range( 0, luck ) > 90 ) bash -= 1;
    if ( number_range( 0, luck ) < 30 ) bash += 2;

    exotic = 5 - ( level / 10 );
    if ( number_range( 0, luck ) > 90 ) exotic -= 1;
    if ( number_range( 0, luck ) < 30 ) exotic += 2;

    pierce = URANGE( -15, pierce, 10 );
    slash  = URANGE( -15, slash,  10 );
    bash   = URANGE( -15, bash,   10 );
    exotic = URANGE( -15, exotic, 10 );

    if ( !get_eq_char( ch, WEAR_HEAD ) && IS_SET( race_table[ GET_RACE( ch ) ].wear_flag, wear_to_itemwear[ WEAR_HEAD ] ) )
    {
        obj = spiritual_armor_obj_helper( OBJ_VNUM_SPIRITUAL_ARMOR_HEAD, timer, level, pierce, bash, slash, exotic );
        obj_to_char( obj, ch );
        equip_char( ch, obj, WEAR_HEAD, TRUE );
        send_to_char( "Na twojej g�owie pojawia si� widmowy he�m.\n\r", ch );
        act( "Na g�owie $z pojawia si� widmowy he�m.", ch, NULL, NULL, TO_ROOM );
    }

    if ( !get_eq_char( ch, WEAR_BODY ) && IS_SET( race_table[ GET_RACE( ch ) ].wear_flag, wear_to_itemwear[ WEAR_BODY ] ) )
    {
        obj = spiritual_armor_obj_helper( OBJ_VNUM_SPIRITUAL_ARMOR_BODY, timer, level, pierce, bash, slash, exotic );
        obj_to_char( obj, ch );
        equip_char( ch, obj, WEAR_BODY, TRUE );
        send_to_char( "Twoje cia�o okrywa widmowa zbroja.\n\r", ch );
        act( "Cia�o $z okrywa widmowa zbroja.", ch, NULL, NULL, TO_ROOM );
    }

    if ( !get_eq_char( ch, WEAR_LEGS ) && IS_SET( race_table[ GET_RACE( ch ) ].wear_flag, wear_to_itemwear[ WEAR_LEGS ] ) )
    {
        obj = spiritual_armor_obj_helper( OBJ_VNUM_SPIRITUAL_ARMOR_LEGS, timer, level, pierce, bash, slash, exotic );
        obj_to_char( obj, ch );
        equip_char( ch, obj, WEAR_LEGS, TRUE );
        send_to_char( "Twoje nogi chroni� teraz widmowe spodnie.\n\r", ch );
        act( "Na nogach $z tworz� si� widmowe spodnie.", ch, NULL, NULL, TO_ROOM );
    }

    if ( !get_eq_char( ch, WEAR_ARMS ) && IS_SET( race_table[ GET_RACE( ch ) ].wear_flag, wear_to_itemwear[ WEAR_ARMS ] ) )
    {
        obj = spiritual_armor_obj_helper( OBJ_VNUM_SPIRITUAL_ARMOR_ARMS, timer, level, pierce, bash, slash, exotic );
        obj_to_char( obj, ch );
        equip_char( ch, obj, WEAR_ARMS, TRUE );
        send_to_char( "Widmowe naramienniki chroni� twoje ramiona.\n\r", ch );
        act( "Na ramionach $z pojawiaja si� widmowe naramienniki.", ch, NULL, NULL, TO_ROOM );
    }

    if ( !get_eq_char( ch, WEAR_HANDS ) && IS_SET( race_table[ GET_RACE( ch ) ].wear_flag, wear_to_itemwear[ WEAR_HANDS ] ) )
    {
        obj = spiritual_armor_obj_helper( OBJ_VNUM_SPIRITUAL_ARMOR_HANDS, timer, level, pierce, bash, slash, exotic );
        obj_to_char( obj, ch );
        equip_char( ch, obj, WEAR_HANDS, TRUE );
        send_to_char( "Na twoich d�oniach pojawiaj� si� widmowe r�kawice.\n\r", ch );
        act( "Na d�oniach $z pojawiaj� si� widmowe r�kawice.", ch, NULL, NULL, TO_ROOM );
    }

    if ( !get_eq_char( ch, WEAR_FEET ) && IS_SET( race_table[ GET_RACE( ch ) ].wear_flag, wear_to_itemwear[ WEAR_FEET ] ) )
    {
        obj = spiritual_armor_obj_helper( OBJ_VNUM_SPIRITUAL_ARMOR_FEET, timer, level, pierce, bash, slash, exotic );
        obj_to_char( obj, ch );
        equip_char( ch, obj, WEAR_FEET, TRUE );
        send_to_char( "Czujesz, �e stopy chronia ci widmowe buty.\n\r", ch );
        act( "Widzisz jak widmowe buty pojawiaj� si� na nogach $z.", ch, NULL, NULL, TO_ROOM );
    }

    if ( !obj )
    {
        send_to_char( "W�a�ciwie to nic si� nie dzieje.\n\r", ch );
    }
    return;
}

/* Leczenie slepoty (skladnia: cast 'cure blindness' <kto>)
 *
 * no to jest standartowy cure blindness
 *
 */
void spell_cure_blindness( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;

	if ( !IS_AFFECTED( victim, AFF_BLIND ) )
	{
		if ( victim == ch )
			send_to_char( "Nie jeste� o�lepion<&y/a/e>.\n\r", ch );
		else
		{
			switch ( victim->sex )
			{
				case 0:
					act( "$N nie wygl�da na �lepe.", ch, NULL, victim, TO_CHAR );
					break;
				case 2:
					act( "$N nie wygl�da na �lep�.", ch, NULL, victim, TO_CHAR );
					break;
				default:
					act( "$N nie wygl�da na �lepego.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}
		return;
	}

	affect_strip( victim, gsn_blindness );
	affect_strip( victim, gsn_wind_charger );
	affect_strip( victim, 215 ); //pwb
	affect_strip( victim, 51 ); //pyrotechnics
	affect_strip( victim, 240 ); //sunscorch
	EXT_REMOVE_BIT( victim->affected_by, AFF_BLIND );

	act( "$n odzyskuje wzrok.", victim, NULL, NULL, TO_ROOM );
	send_to_char( "Odzyskujesz wzrok.\n\r", victim );

	return;
}

/* TU IDA WSZYSTKIE HOLDY
 * rozroznianie czy animal/monster/... po wpisie ->type w race_type
 */

/* Unieruchomienie osoby (skladnia: cast 'hold person' <kto>)
 */
void spell_hold_person( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int duration;
	int luck = get_curr_stat( ch, STAT_LUC );
	int vluck = get_curr_stat( victim, STAT_LUC );

	/*sprawdzanie bez powiadomienia bledu*/
	if ( !IS_SET( race_table[ GET_RACE( victim ) ].type , PERSON )
	     || IS_AFFECTED( victim, AFF_FREE_ACTION ) || is_undead( victim ) )
	{
		send_to_char( "Nic si� nie dzieje.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( victim, AFF_PARALYZE ) ||
	     saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		send_to_char( "Nic si� nie dzieje.\n\r", ch );
		return;
	}

	if ( number_range( 0, luck/6 + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda�o ci si�.\n\r", ch );
		return;
	}

	if ( number_range( 0, vluck - luck ) > 30 )
	{
		send_to_char( "Twoja ofiara mia�a naprawd� du�o szcz�cia i nie uda�o ci si�.\n\r", ch );
		return;
	}

    // stripujemy invisa je�eli target jest graczem
    if (!IS_NPC(victim))
    {
        strip_invis(ch, TRUE, TRUE);
    }

	duration = 1 + ( level / 10 );

	if ( number_range( 0, luck ) > 90 ) ++duration;
	else if ( number_range( 0, vluck ) < 18 ) ++duration;

	if ( number_range( 0, luck ) < 30 ) --duration;
	if ( number_range( 0, vluck ) > 90 ) --duration;

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			if ( level > 24 )
				++duration;
			if ( number_range( 1, 2 ) == 1 )
				++duration;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.location = APPLY_NONE;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.modifier	= 0;
	af.bitvector = &AFF_PARALYZE;
	affect_to_char( victim, &af, NULL, TRUE );

	//kasowanie prewait�w
	if ( victim->wait_char )
	{
		free_pwait_char( victim->wait_char );
		victim->wait_char = NULL;
		victim->wait = 2;
	}

	switch ( victim->sex )
	{
		case 0:
			send_to_char( "Zosta�o� unieruchomione!\n\r", victim );
			break;
		case 2:
			send_to_char( "Zosta�a� unieruchomiona!\n\r", victim );
			break;
		default:
			send_to_char( "Zosta�e� unieruchomiony!\n\r", victim );
			break;
	}

	act( "$n zatrzymuje si� w miejscu.", victim, NULL, NULL, TO_ROOM );
	return;
}
/* Unieruchomienie z�ego (skladnia: cast 'hold evil' <kto>)
 */
void spell_hold_evil( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af, *paf;
	int duration;
	int luck = get_curr_stat( ch, STAT_LUC );
	int vluck = get_curr_stat( victim, STAT_LUC );
	int holy_dur_mod = 0;
	bool hold_self = FALSE, bonus_conditions = TRUE;

	/*sprawdzanie z powiadomieniem o b��dzie*/
	if ( !IS_EVIL( victim ) )
	{
		send_to_char( "Ten czar jest skuteczny tylko przeciw Z�u.\n\r", ch );
		return;
	}
	
	if ( !IS_NPC(victim) && victim->level > 27 )
	{
		send_to_char( "Tak pot�znego z�a nie dasz rady unieruchomi�!\n\r", ch);
		return;
	}


	/*sprawdzanie bez powiadomienia bledu*/
	if ( IS_AFFECTED( victim, AFF_FREE_ACTION ) )
	{
		send_to_char( "Nic si� nie dzieje.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( victim, AFF_PARALYZE ) ||
	     saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		send_to_char( "Nic si� nie dzieje.\n\r", ch );
		return;
	}

	if ( number_range( 0, luck/6 + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda�o ci si�.\n\r", ch );
		return;
	}

	if ( number_range( 0, vluck - luck ) > 30 )
	{
		send_to_char( "Twoja ofiara mia�a naprawd� du�o szcz�cia i nie uda�o ci si�.\n\r", ch );
		return;
	}

    // stripujemy invisa je�eli target jest graczem
    if (!IS_NPC(victim))
    {
        strip_invis(ch, TRUE, TRUE);
    }

    if ( ( paf = affect_find( ch->affected, gsn_prayer_last )) != NULL )
    {
        if ( paf->modifier > 36 )
        {
            //niemodlacy sie nie maja pozytywnych bonusow
            bonus_conditions = FALSE;
        }

    }
    //efekty pozytywne przy holy prayerze
    if ( ( paf = affect_find( ch->affected, gsn_holy_pool )) != NULL)
    {
      if( bonus_conditions == TRUE)
      {
         if(paf->modifier > 20 && number_percent() < 30)
         {
            holy_dur_mod += 2;
            paf->modifier -= 3;
         }

         if(paf->modifier > 50 && number_percent() < 30)
         {
            holy_dur_mod += 2;
            paf->modifier -= 4;
         }

         if(paf->modifier > 70 && number_percent() < 30)
         {
            holy_dur_mod += 2;
            paf->modifier -= 5;
         }
      }
        if(paf->modifier < -70)
         {
            hold_self = TRUE;
         }
        if(paf->modifier < -50 && paf->modifier > -70)
         {
            holy_dur_mod -=1;
         }
        if(paf->modifier < -30 && paf->modifier > -70)
         {
            holy_dur_mod -=1;
         }

    }


	duration = 1 + ( level / 10 );
    duration += holy_dur_mod;
	if ( number_range( 0, luck ) > 90 ) ++duration;
	else if ( number_range( 0, vluck ) < 18 ) ++duration;

	if ( number_range( 0, luck ) < 30 ) --duration;
	if ( number_range( 0, vluck ) > 90 ) --duration;

    duration = URANGE(1, duration, 10);

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.location = APPLY_NONE;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.modifier	= 0;
	af.bitvector = &AFF_PARALYZE;
	if(hold_self == TRUE)
	{
       affect_to_char( ch, &af, NULL, TRUE );

       	//kasowanie prewait�w
        	if ( ch->wait_char )
         	{
          		free_pwait_char( ch->wait_char );
            	ch->wait_char = NULL;
            	ch->wait = 2;
          	}
            send_to_char( "Wymawiaj�c s�owa zakl�cia czujesz jak mrok rozlewa si� po twoim ciele, a czar obraca sw� moc przeciw tobie!\n\r", victim );
            act( "Widzisz jak $C otacza przez chwile mroczna aura.", ch, NULL, NULL, TO_ROOM );
           	switch ( ch->sex )
           	{
           		case 0:
               	send_to_char( "Zosta�o� unieruchomione!\n\r", ch );
		        break;
		        case 2:
			    send_to_char( "Zosta�a� unieruchomiona!\n\r", ch );
			    break;
		        default:
                send_to_char( "Zosta�e� unieruchomiony!\n\r", ch );
			    break;
            }
            act( "$n zatrzymuje si� w miejscu.", ch, NULL, NULL, TO_ROOM );
            return;


    }
	affect_to_char( victim, &af, NULL, TRUE );

	//kasowanie prewait�w
	if ( victim->wait_char )
	{
		free_pwait_char( victim->wait_char );
		victim->wait_char = NULL;
		victim->wait = 2;
	}

	switch ( victim->sex )
	{
		case 0:
			send_to_char( "Zosta�o� unieruchomione!\n\r", victim );
			break;
		case 2:
			send_to_char( "Zosta�a� unieruchomiona!\n\r", victim );
			break;
		default:
			send_to_char( "Zosta�e� unieruchomiony!\n\r", victim );
			break;
	}
    if(holy_dur_mod > 0)
    {
      send_to_char( "Czujesz jak moc twojego zakl�cia ro�nie!\n\r", ch );
      act( "Widzisz jak $c otacza przez chwil� jasna aura.", ch, NULL, NULL, TO_ROOM );
    }
    if(holy_dur_mod < 0)
    {
      send_to_char( "Czujesz jak moc twojego zakl�cia s�abnie!\n\r", ch );
      act( "Widzisz jak $c otacza przez chwil� ciemna aura.", ch, NULL, NULL, TO_ROOM );
    }
	act( "$n zatrzymuje si� w miejscu.", victim, NULL, NULL, TO_ROOM );
	return;
}

void spell_hold_monster( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA * victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;
    int duration;
    int luck  = 0;
    int vluck = 0;

    /*sprawdzanie bez powiadomienia bledu*/
    if ( !IS_SET( race_table[ GET_RACE( victim ) ].type , MONSTER ) || IS_AFFECTED( victim, AFF_FREE_ACTION ) )
    {
        send_to_char( "Nic si� nie dzieje.\n\r", ch );
        return;
    }

    if ( IS_AFFECTED( victim, AFF_PARALYZE ) || saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
    {
        send_to_char( "Nic si� nie dzieje.\n\r", ch );
        return;
    }

    luck  = get_curr_stat( ch, STAT_LUC );
    if ( number_range( 0, luck + LUCK_BASE_MOD ) < 5 )
    {
        send_to_char( "Nie uda�o ci si�.\n\r", ch );
        return;
    }

    vluck = get_curr_stat( victim, STAT_LUC );
    if ( vluck - 30 > luck && number_range( 0, vluck - luck ) > 30 )
    {
        send_to_char( "Twoja ofiara mia�a naprawd� du�o szcz�cia i nie uda�o ci si�.\n\r", ch );
        return;
    }
    // stripujemy invisa je�eli target jest graczem
    if (!IS_NPC(victim))
    {
        strip_invis(ch, TRUE, TRUE);
    }
    duration = 1 + ( level / 10 );
    if ( number_range( 0, luck ) > 90 )
    {
        ++duration;
    }
    else if ( number_range( 0, vluck ) < 18 )
    {
        ++duration;
    }
    if ( number_range( 0, luck ) < 30 )
    {
        --duration;
    }
    if ( number_range( 0, vluck ) > 90 )
    {
        --duration;
    }
    /* Bonus dla specjalisty */
    if ( !IS_NPC( ch ) )
    {
        if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
        {
            if ( level > 24 )
            {
                ++duration;
            }
            if ( number_range( 1, 2 ) == 1 )
            {
                ++duration;
            }
        }
    }

    af.bitvector   = &AFF_PARALYZE;
    af.duration    = UMAX( 1, duration );
    af.level       = level;
    af.location    = APPLY_NONE;
    af.modifier    = 0;
    af.rt_duration = 0;
    af.type        = sn;
    af.where       = TO_AFFECTS;

    affect_to_char( victim, &af, NULL, TRUE );

    //kasowanie prewait�w
    if ( victim->wait_char )
    {
        free_pwait_char( victim->wait_char );
        victim->wait_char = NULL;
        victim->wait = 2;
    }

    switch ( victim->sex )
    {
        case SEX_NEUTRAL:
            send_to_char( "Zosta�o� unieruchomione!\n\r", victim );
            break;
        case SEX_FEMALE:
            send_to_char( "Zosta�a� unieruchomiona!\n\r", victim );
            break;
        case SEX_MALE:
        default:
            send_to_char( "Zosta�e� unieruchomiony!\n\r", victim );
            break;
    }

    act( "$n zatrzymuje si� w miejscu.", victim, NULL, NULL, TO_ROOM );
    return;
}

void spell_hold_undead( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int duration;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	/*sprawdzanie bez powiadomienia bledu*/
	if ( IS_AFFECTED( victim, AFF_FREE_ACTION ) || !is_undead( victim ) )
	{
		send_to_char( "Nic si� nie dzieje.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( victim, AFF_PARALYZE ) || saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		send_to_char( "Nic si� nie dzieje.\n\r", ch );
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda�o ci si�.\n\r", ch );
		return;
	}

	if ( number_range( 0, vluck - luck ) > 5 )
	{
		send_to_char( "Twoja ofiara mia�a naprawd� du�o szcz�cia i nie uda�o ci si�.\n\r", ch );
		return;
	}

    // stripujemy invisa je�eli target jest graczem
    if (!IS_NPC(victim))
    {
        strip_invis(ch, TRUE, TRUE);
    }

	duration = 1 + ( level / 10 );

	if ( number_range( 0, luck ) > 15 ) ++duration;
	else if ( number_range( 0, vluck ) < 3 ) ++duration;

	if ( number_range( 0, luck ) < 5 ) --duration;
	if ( number_range( 0, vluck ) > 15 ) --duration;

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			if ( level > 24 )
				++duration;
			if ( number_range( 1, 2 ) == 1 )
				++duration;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.location = APPLY_NONE;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.modifier	= 0;
	af.bitvector = &AFF_PARALYZE;
	affect_to_char( victim, &af, NULL, TRUE );

	//kasowanie prewait�w
	if ( victim->wait_char )
	{
		free_pwait_char( victim->wait_char );
		victim->wait_char = NULL;
		victim->wait = 2;
	}

	switch ( victim->sex )
	{
		case 0:
			send_to_char( "Zosta�o� unieruchomione!\n\r", victim );
			break;
		case 2:
			send_to_char( "Zosta�a� unieruchomiona!\n\r", victim );
			break;
		default:
			send_to_char( "Zosta�e� unieruchomiony!\n\r", victim );
			break;
	}
	act( "$n zatrzymuje si� w miejscu.", victim, NULL, NULL, TO_ROOM );
	return;
}

void spell_hold_animal( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int duration;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	/*sprawdzanie bez powiadomienia bledu*/
	if ( !IS_SET( race_table[ GET_RACE( victim ) ].type , ANIMAL )
	     || IS_AFFECTED( victim, AFF_FREE_ACTION ) )
	{
		send_to_char( "Nic si� nie dzieje.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( victim, AFF_PARALYZE ) ||
	     saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		send_to_char( "Nic si� nie dzieje.\n\r", ch );
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda�o ci si�.\n\r", ch );
		return;
	}

	if ( number_range( 0, vluck - luck ) > 5 )
	{
		send_to_char( "Twoja ofiara mia�a naprawd� du�o szcz�cia i nie uda�o ci si�.\n\r", ch );
		return;
	}

    // stripujemy invisa je�eli target jest graczem
    if (!IS_NPC(victim))
    {
        strip_invis(ch, TRUE, TRUE);
    }

	duration = 1 + ( level / 10 );

	if ( number_range( 0, luck ) > 15 ) ++duration;
	else if ( number_range( 0, vluck ) < 3 ) ++duration;

	if ( number_range( 0, luck ) < 5 ) --duration;
	if ( number_range( 0, vluck ) > 15 ) --duration;

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			if ( level > 24 )
				++duration;
			if ( number_range( 1, 2 ) == 1 )
				++duration;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.location = APPLY_NONE;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.modifier	= 0;
	af.bitvector = &AFF_PARALYZE;
	affect_to_char( victim, &af, NULL, TRUE );

	//kasowanie prewait�w
	if ( victim->wait_char )
	{
		free_pwait_char( victim->wait_char );
		victim->wait_char = NULL;
		victim->wait = 2;
	}

	switch ( victim->sex )
	{
		case 0:
			send_to_char( "Zosta�o� unieruchomione!\n\r", victim );
			break;
		case 2:
			send_to_char( "Zosta�a� unieruchomiona!\n\r", victim );
			break;
		default:
			send_to_char( "Zosta�e� unieruchomiony!\n\r", victim );
			break;
	}

	act( "$n zatrzymuje si� w miejscu.", victim, NULL, NULL, TO_ROOM );
	return;
}

void spell_hold_plant( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int duration;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	/*sprawdzanie bez powiadomienia bledu*/
	if ( !IS_SET( race_table[ GET_RACE( victim ) ].type , PLANT )
	     || IS_AFFECTED( victim, AFF_FREE_ACTION ) )
	{
		send_to_char( "Nic si� nie dzieje.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( victim, AFF_PARALYZE ) ||
	     saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		send_to_char( "Nic si� nie dzieje.\n\r", ch );
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda�o ci si�.\n\r", ch );
		return;
	}

    // stripujemy invisa je�eli target jest graczem
    if (!IS_NPC(victim))
    {
        strip_invis(ch, TRUE, TRUE);
    }

	duration = 1 + ( level / 10 );

	if ( number_range( 0, luck ) > 15 ) ++duration;
	else if ( number_range( 0, vluck ) < 3 ) ++duration;

	if ( number_range( 0, luck ) < 5 ) --duration;
	if ( number_range( 0, vluck ) > 15 ) --duration;

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			if ( level > 24 )
				++duration;
			if ( number_range( 1, 2 ) == 1 )
				++duration;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.location = APPLY_NONE;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.modifier	= 0;
	af.bitvector = &AFF_PARALYZE;
	affect_to_char( victim, &af, NULL, TRUE );

	//kasowanie prewait�w
	if ( victim->wait_char )
	{
		free_pwait_char( victim->wait_char );
		victim->wait_char = NULL;
		victim->wait = 2;
	}

	switch ( victim->sex )
	{
		case 0:
			send_to_char( "Zosta�o� unieruchomione!\n\r", victim );
			break;
		case 2:
			send_to_char( "Zosta�a� unieruchomiona!\n\r", victim );
			break;
		default:
			send_to_char( "Zosta�e� unieruchomiony!\n\r", victim );
			break;
	}

	act( "$n zatrzymuje si� w miejscu.", victim, NULL, NULL, TO_ROOM );
	return;
}

/* metamorfoza laski w bron (skladnia: cast 'transmute staff')

 *
 * czarek daje lasce affecta do HR i DR reszte trzeba dac w fight.c
 * sprawdza czy ma bron, albo laske, ktora ma affecta, jesli tak, to wali jak
 * z broni z obrazeniami standartowego miecza (ale magicznymi!)
 * jak ju� robisz walke, to zostawiam to Tobie *grin*
 */
void spell_transmute_staff( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj;
	AFFECT_DATA af;
	int duration, mod;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int wis_mod = URANGE( -2, ( get_curr_stat_deprecated( ch, STAT_WIS ) / 2 ) - 9, 3 );

	if ( target != TARGET_OBJ )
	{
		send_to_char( "Umiesz przemienia� tylko laski w bro�.\n\r", ch );
		return;
	}

	obj = ( OBJ_DATA * ) vo;

	if ( obj->item_type != ITEM_WEAPON || obj->value[ 0 ] != WEAPON_STAFF )
	{
		send_to_char( "Umiesz przemienia� tylko laski w bro�.\n\r", ch );
		return;
	}

	if ( IS_OBJ_STAT( obj, ITEM_TRANSMUTE ) )
	{
		act( "$p jest ju� przemieniona.", ch, obj, NULL, TO_CHAR );
		return;
	}

	if ( IS_OBJ_STAT( obj, ITEM_MAGIC ) )
	{
		act( "Ta bro� jest magiczna.", ch, obj, NULL, TO_CHAR );
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda�o ci si�.\n\r", ch );
		return;
	}

	duration = ( ( level / 2 ) * ( 100 + luck ) ) / 100;
	duration += wis_mod;
	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 ) duration /= 2;

	mod = URANGE( 1, level / 10, 3 );
	if ( number_range( 0, luck ) > 10 ) ++mod;
	if ( number_range( -2, wis_mod ) > 0 ) ++mod;
	if ( dice( 2, luck ) < 9 ) --mod;
	if ( dice( UMAX( 1, wis_mod ), 10 ) < 5 ) --mod;

	af.where = TO_OBJECT;
	af.type = sn;
	af.level = level;
	af.duration = duration; af.rt_duration = 0;
	af.bitvector = &ITEM_TRANSMUTE;
	af.location = APPLY_HITROLL;
	af.modifier = mod;
	affect_to_obj( obj, &af );
	af.location = APPLY_DAMROLL;
	af.modifier = mod;
	if ( level > 14 )
		af.bitvector = &ITEM_MAGIC;
	affect_to_obj( obj, &af );

	act( "$p p�onie jasnym �wiat�em.", ch, obj, NULL, TO_ALL );
	return;
}

/* Rozkaz (skladnia: cast command <kto> <co_ma_zrobic>)
 *
 * no wiec ja to bym widzial dokladnie tak, jak mob force <co>, ale dziala na
 * flee, rest, sleep, stand... bo mialo byc na 1 wyraz. tylko trza sprzwdzac
 * bo beda przekrety np. c command <kto> quit i bierzemy eq
 * jest zrobione tak, ze bardziej inteligentnych do niczego nie zmusimy
 */

void spell_command( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * vch;
	char vict_name[ MAX_INPUT_LENGTH ];
	char command[ MAX_INPUT_LENGTH ];
	char *tar = ( char * ) vo;
	bool ok = FALSE;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int vluck = 0;
	int cmd;

	if ( !tar || tar[ 0 ] == '\0' )
	{
		send_to_char( "Komu chcesz co� rozkaza�?\n\r", ch );
		return;
	}

	tar = one_argument( tar, vict_name );

	vch = get_char_room( ch, vict_name );

	if ( !vch )
	{
		send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
		return;
	}

	if ( !tar || !tar[ 0 ] )
	{
		send_to_char( "Co chcesz rozkaza�?\n\r", ch );
		return;
	}

	if ( ch == vch )
	{
		send_to_char( "Nie uwa�asz, �e prze�amywanie w�asnej woli to lekki bezsens?\n\r", ch );
		return;
	}

	if ( ch->mount && vch->mounting == ch )
	{
		send_to_char( "Najpierw zsi�d� z wierzchowca.\n\r", ch );
		return;
	}

	/* no to trzeba jakis wzorek wymyslec, coby si� nie zawsze wyszlo
	 * trza by to jakos od inta uzaleznic i troche losowosci dodac
	 */
	if ( is_safe( ch, vch ) )
	{
		send_to_char( "Nic si� nie sta�o.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( vch, AFF_MINOR_GLOBE ) || IS_AFFECTED( vch, AFF_GLOBE ) || IS_AFFECTED( vch, AFF_MAJOR_GLOBE ) || IS_AFFECTED( vch, AFF_ABSOLUTE_MAGIC_PROTECTION ) )
	{
		act( "Twoje zakl�cie znika przy zetkni�ciu ze sfer� otaczaj�c� $C.", ch, NULL, vch, TO_CHAR );
		act( "Zakl�cie $z znika przy zetkni�ciu z otaczaj�c� ci� sfer�.\n\r", ch, NULL, vch, TO_VICT );
		act( "Zakl�cie $z znika przy zetkni�ciu z otaczaj�c� $C sfer�.", ch, NULL, vch, TO_NOTVICT );
		return;
	}

	if ( IS_AFFECTED( vch, AFF_REFLECT_SPELL ) )
	{
		print_char( ch, "Twoje zakl�cie odbija si� od ochronnej tarczy %s.\n\r", vch->name2 );
		print_char( vch, "Zakl�cie %s odbija si� od ochronnej tarczy.\n\r", ch->name2 );
		act( "Zakl�cie $z odbija si� od ochronnej tarczy $Z.", ch, NULL, vch, TO_NOTVICT );
		return;
	}

	if ( IS_AFFECTED( vch, AFF_PARALYZE ) )
	{
		act( "$N nie jest w stanie wykona� twojego polecenia.", ch, NULL, vch, TO_CHAR );
		return;
	}

	if ( saves_spell_new( vch, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		if ( IS_NPC( vch ) )
		{
			act( "$N nie zwraca na uwagi na to co robisz.", ch, NULL, vch, TO_CHAR );
		}
		else
		{
			act( "$N �mieje si� z ciebie.", ch, NULL, vch, TO_CHAR );
			act( "$n pr�bowa� wyda� ci rozkaz, jednak ty si� z tego �miejesz.", ch, NULL, vch, TO_VICT );
		}
		if ( vch->fighting == NULL && can_see( vch, ch ) && can_move( vch ) )
		{
			if ( vch->position == POS_SITTING || vch->position == POS_RESTING )
			{
				vch->position = POS_STANDING;
				act( "Wstajesz.", vch, NULL, NULL, TO_CHAR );
				act( "$n wstaje.", vch, NULL, NULL, TO_ROOM );
			}

			if ( IS_NPC( vch ) && HAS_TRIGGER( vch, TRIG_KILL ) )
				mp_percent_trigger( vch, ch, NULL, NULL, &TRIG_KILL );

			multi_hit( vch, ch, TYPE_UNDEFINED );
		}
		return;
	}

	if ( vch->resists[ RESIST_MENTAL ] > 0 && number_percent() < vch->resists[ RESIST_MENTAL ] )
		return;

	vluck = get_curr_stat_deprecated( vch, STAT_LUC );

	if ( number_range( 0, luck ) < 1 )
	{
		send_to_char( "Nie uda�o ci si�.\n\r", ch );
		return;
	}
	if ( number_range( 0, vluck - luck ) > 5 )
	{
		send_to_char( "Twoja ofiara mia�a naprawd� du�o szcz�cia i nie uda�o ci si�.\n\r", ch );
		return;
	}

	/* zawsze mozna cos dopisac */
	if ( EXT_IS_SET( ch->act, PLR_COMMANDSPL ) )
	{
		if ( !str_prefix( tar, "uciekaj" ) ||
		     !str_prefix( tar, "spij" ) ||
		     !str_prefix( tar, "usiadz" ) ||
		     !str_prefix( tar, "wstan" ) ||
		     !str_prefix( tar, "odpocznij" ) ||
		     !str_prefix( tar, "gora" ) ||
		     !str_prefix( tar, "dol" ) ||
		     !str_prefix( tar, "polnoc" ) ||
		     !str_prefix( tar, "wschod" ) ||
		     !str_prefix( tar, "poludnie" ) ||
		     !str_prefix( tar, "zachod" ) ||
		     !str_prefix( tar, "kopnij" ) ||
		     !str_prefix( tar, "powal" ) )
			ok = TRUE;
	}
	else
	{
		if ( !str_prefix( tar, "flee" ) ||
		     !str_prefix( tar, "sleep" ) ||
		     !str_prefix( tar, "sit" ) ||
		     !str_prefix( tar, "stand" ) ||
		     !str_prefix( tar, "rest" ) ||
		     !str_prefix( tar, "up" ) ||
		     !str_prefix( tar, "down" ) ||
		     !str_prefix( tar, "north" ) ||
		     !str_prefix( tar, "east" ) ||
		     !str_prefix( tar, "south" ) ||
		     !str_prefix( tar, "west" ) ||
		     !str_prefix( tar, "kick" ) ||
		     !str_prefix( tar, "bash" ) )
			ok = TRUE;
	}

    if ( ok )
    {
        switch ( vch->sex )
        {
            case 0 :
                act( "Uleg�o� silnej woli $z", ch, NULL, vch, TO_VICT );
                break;
            case 2 :
                act( "Uleg�a� silnej woli $z", ch, NULL, vch, TO_VICT );
                break;
            default :
                act( "Uleg�e� silnej woli $z", ch, NULL, vch, TO_VICT );
                break;
        }
        switch ( ch->sex )
        {
            case 0 :
                act( "Prze�ama�e� woln� wol� $Z.", ch, NULL, vch, TO_CHAR );
                break;
            case 2 :
                act( "Prze�ama�a� woln� wol� $Z.", ch, NULL, vch, TO_CHAR );
                break;
            default :
                act( "Prze�ama�e� woln� wol� $Z.", ch, NULL, vch, TO_CHAR );
                break;
        }

        if ( EXT_IS_SET( ch->act, PLR_COMMANDSPL ) )
        {
            for ( cmd = 0; cmd_table[ cmd ].name[ 0 ] != '\0'; cmd++ )
            {
                if ( NOPOL( tar[ 0 ] ) == NOPOL( cmd_table[ cmd ].name_pl[ 0 ] )
                        && !str_prefix( tar, cmd_table[ cmd ].name_pl ) )
                {
                    sprintf( command, "%s", cmd_table[ cmd ].name );
                    interpret( vch, command );
                    break;
                }
            }
        }
        else
        {
            interpret( vch, tar );
        }
    }
    else
    {
        send_to_char( "To jest zbyt skomplikowane.\n\r", ch );
        return;
    }

    return;
}

/* Regeneracja konczyc (skladnia: cast regenerate <kto> <co>)
 *
 * w sumie ja to widze tak, ze mozna regenerowac w zaleznosci od parametru
 * nie wiem, co ma odpadac, a co nie, wiec zostawiam puste
 *
 */
void spell_regenerate( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ), dur, lev;

	if ( IS_AFFECTED( victim, AFF_REGENERATION ) )
	{
		if ( ch == victim )
			send_to_char( "Przecie� ju� jeste� pod dzia�eniem tego czaru.\n\r", victim );
		else
			act( "$N jest ju� pod dzia�aniem tego czaru.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( number_range( 0, luck ) < 1 )
	{
		send_to_char( "Nie uda�o ci si�.\n\r", ch );
		return;
	}

	dur = 2;
	if ( number_range( 0, get_curr_stat_deprecated( ch, STAT_INT ) ) > 15 )
		++dur;
	if ( number_range( 0, get_curr_stat_deprecated( ch, STAT_WIS ) ) > 15 )
		++dur;
	if ( number_range( 0, level ) > 20 )
		++dur;

	lev = 2 + level / 7.5;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = lev;
	af.duration = dur; af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_REGENERATION;

	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Odczuwasz przyjemne mrowienie na ca�ym ciele.\n\r", victim );

	if ( ch == victim )
		return;

	act( "Widzisz jak $N dr�y na ca�ym ciele.", ch, NULL, victim, TO_CHAR );
	return;
}

/* Niewidzialny dla martiwakow (skladnia: cast 'undead invis' [co])
 */
void spell_undead_invis( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim;
	OBJ_DATA *obj;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int duration = 2 + level / 4;
	if ( number_range( 0, luck ) > 10 ) duration += 2;
	if ( number_range( 0, luck ) < 3 ) duration -= 2;

	/* object invi sibility - niech bedzie i dla rzeczy */
	if ( target == TARGET_OBJ )
	{
		obj = ( OBJ_DATA * ) vo;

		if ( IS_OBJ_STAT( obj, ITEM_UNDEAD_INVIS ) )
		{
			act( "Nieumarli nie widz� ju� $f.", ch, obj, NULL, TO_CHAR );
			return;
		}

		if ( number_range( 0, luck ) < 1 )
		{
			act( "Nie uda�o ci si� ukry� $f przed nieumar�ymi.", ch, obj, NULL, TO_CHAR );
			return;
		}

		af.where = TO_OBJECT;
		af.type = sn;
		af.level = level;
		af.duration = duration; af.rt_duration = 0;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = &ITEM_UNDEAD_INVIS;
		affect_to_obj( obj, &af );

		act( "$p znika przed wzrokiem nieumar�ych.", ch, obj, NULL, TO_ALL );
		return;
	}

	/* character invisibility */
	victim = ( CHAR_DATA * ) vo;

	if ( IS_AFFECTED( victim, AFF_UNDEAD_INVIS ) )
	{
		if ( ch == victim )
			send_to_char( "Nieumarli ju� ciebie nie widz�.\n\r", ch );
		else
			act( "Nieumarli nie widz� ju� $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( number_range( 0, luck ) < 1 )
	{
		if ( ch == victim )
			send_to_char( "Nie uda�o ci si� ukry� przed nieumar�ymi.\n\r", ch );
		else
			act( "Nie uda�o ci si� ukry� $Z przed nieumar�ymi.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( ch != victim )
	{
		luck = get_curr_stat_deprecated( victim, STAT_LUC );
		if ( number_range( 0, luck ) > 10 ) duration += 2;
		if ( number_range( 0, luck ) < 3 ) duration -= 2;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_UNDEAD_INVIS;
	affect_to_char( victim, &af, NULL, TRUE );

	switch ( victim->sex )
	{
		case 0:
			act( "$n staje si� niewidzialne dla nieumar�ych.", victim, NULL, NULL, TO_ROOM );
			send_to_char( "Stajesz si� niewidzialne dla martwiak�w.\n\r", victim );
			break;
		case 2:
			act( "$n staje si� niewidzialna dla nieumar�ych.", victim, NULL, NULL, TO_ROOM );
			send_to_char( "Stajesz si� niewidzialna dla martwiak�w.\n\r", victim );
			break;
		default:
			act( "$n staje si� niewidzialny dla nieumar�ych.", victim, NULL, NULL, TO_ROOM );
			send_to_char( "Stajesz si� niewidzialny dla martwiak�w.\n\r", victim );
			break;
	}

	return;
}

/* Niewidzialny dla zwierzat (skladnia: cast 'animal invis' [co])
 */

void spell_animal_invis( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim;
	OBJ_DATA *obj;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int duration = 2 + level / 4;
	if ( number_range( 0, luck ) > 10 ) duration += 2;
	if ( number_range( 0, luck ) < 5 ) duration -= 2;

	/* object invi sibility - niech bedzie i dla rzeczy */
	if ( target == TARGET_OBJ )
	{
		obj = ( OBJ_DATA * ) vo;

		if ( IS_OBJ_STAT( obj, ITEM_ANIMAL_INVIS ) )
		{
			act( "Zwierz�ta nie widz� ju� $f.", ch, obj, NULL, TO_CHAR );
			return;
		}

		if ( number_range( 0, luck ) < 1 )
		{
			act( "Nie uda�o ci si� ukry� $f przed zwierz�tami.", ch, obj, NULL, TO_CHAR );
			return;
		}

		af.where	= TO_OBJECT;
		af.type	= sn;
		af.level	= level;
		af.duration = duration; af.rt_duration = 0;
		af.location	= APPLY_NONE;
		af.modifier	= 0;
		af.bitvector	= &ITEM_ANIMAL_INVIS;
		affect_to_obj( obj, &af );

		act( "$p znika przed wzrokiem zwierz�t.", ch, obj, NULL, TO_ALL );
		return;
	}

	/* character invisibility */
	victim = ( CHAR_DATA * ) vo;

	if ( IS_AFFECTED( victim, AFF_ANIMAL_INVIS ) )
	{
		act( "Zwierz�ta nie widz� ju� $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( number_range( 0, luck ) < 1 )
	{
		if ( ch != victim )
			act( "Nie uda�o ci si� ukry� $Z przed wzrokiem zwierz�t.", ch, NULL, victim, TO_CHAR );
		else
			act( "Nie uda�o ci si� ukry� przed wzrokiem zwierz�t.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( ch != victim )
	{
		luck = get_curr_stat_deprecated( victim, STAT_LUC );
		if ( number_range( 0, luck ) > 10 ) duration += 2;
		if ( number_range( 0, luck ) < 5 ) duration -= 2;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_ANIMAL_INVIS;
	affect_to_char( victim, &af, NULL, TRUE );

	switch ( victim->sex )
	{
		case 0:
			act( "$n staje si� niewidzialne dla zwierz�t.", victim, NULL, NULL, TO_ROOM );
			send_to_char( "Stajesz si� niewidzialne dla zwierz�t.\n\r", victim );
			break;
		case 1:
			act( "$n staje si� niewidzialny dla zwierz�t.", victim, NULL, NULL, TO_ROOM );
			send_to_char( "Stajesz si� niewidzialny dla zwierz�t.\n\r", victim );
			break;
		default:
			act( "$n staje si� niewidzialna dla zwierz�t.", victim, NULL, NULL, TO_ROOM );
			send_to_char( "Stajesz si� niewidzialna dla zwierz�t.\n\r", victim );
			break;
	}

	return;
}

/* Ochrona przed zlem (skladnia: cast 'protection evil' [kto])
 *
 * tutaj nie bedzie nic dawac, bo ma dzialac tylko w walce ze zlymi
 * wiec nie widze sensu dawac teraz do affectow hr+2,ac-2 lepiej to
 * zrobic w czasie walki, sprawdzic alignment atakujacego i wtedy ew
 * doliczac ac/hr. troche bedzie tych zmian w fight.c
 */
void spell_protection_evil( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( IS_AFFECTED( victim, AFF_PROTECT_EVIL ) )
	{
		if ( victim == ch )
			send_to_char( "Jeste� ju� chronion<&y/a/e> przed z�em.\n\r", ch );
		else
		{
			switch ( victim->sex )
			{
				case 0:
					act( "$N jest ju� chronione przed z�em.", ch, NULL, victim, TO_CHAR );
					break;
				case 2:
					act( "$N jest ju� chroniona przed z�em.", ch, NULL, victim, TO_CHAR );
					break;
				default :
					act( "$N jest ju� chroniony przed z�em.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}
		return;
	}

	if ( IS_AFFECTED( victim, AFF_PROTECT_GOOD ) )
	{
		if ( victim == ch )
			send_to_char( "Jeste� ju� chronion<&y/a/e> przed dobrem.\n\r", ch );
		else
		{
			switch ( victim->sex )
			{
				case 0:
					act( "$N jest ju� chronione przed dobrem.", ch, NULL, victim, TO_CHAR );
					break;
				case 2:
					act( "$N jest ju� chroniona przed dobrem.", ch, NULL, victim, TO_CHAR );
					break;
				default :
					act( "$N jest ju� chroniony przed dobrem.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}

		return;
	}

	if ( IS_EVIL( victim ) )
	{
		if ( victim == ch )
			send_to_char( "Nie mo�esz powiaza� si� z dobrem!\n\r", ch );
		else
			act( "$N nie mo�e wi�za� si� z dobrem!", ch, NULL, victim, TO_CHAR );

		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( ch != victim )
			act( "Nie uda�o ci si� ochroni� $Z przed z�em.", ch, NULL, victim, TO_CHAR );
		else
			send_to_char( "Nie uda�o ci si� ochroni� siebie przed z�em.\n\r", ch );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 20 + level / 3; af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_PROTECT_EVIL;
	affect_to_char( victim, &af, NULL, TRUE );

	switch ( victim->sex )
	{
		case 0:
			send_to_char( "Czujesz si� powi�zane z dobrem.\n\r", victim );
			break;
		case 2:
			send_to_char( "Czujesz si� powi�zana z dobrem.\n\r", victim );
			break;
		default :
			send_to_char( "Czujesz si� powi�zany z dobrem.\n\r", victim );
			break;
	}
	if ( ch != victim )
		switch ( victim->sex )
		{
			case 0:
				act( "$N jest chronione przed z�em.", ch, NULL, victim, TO_CHAR );
				break;
			case 2:
				act( "$N jest chroniona przed z�em.", ch, NULL, victim, TO_CHAR );
				break;
			default :
				act( "$N jest chroniony przed z�em.", ch, NULL, victim, TO_CHAR );
				break;
		}
	return;
}

/* Ochrona przed dobrem (skladnia: cast 'protection good' [kto])
 */
void spell_protection_good( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( IS_AFFECTED( victim, AFF_PROTECT_EVIL ) )
	{
		if ( victim == ch )
			send_to_char( "Jeste� ju� chronion<&y/a/e> przed z�em.\n\r", ch );
		else
		{
			switch ( victim->sex )
			{
				case 0:
					act( "$N jest ju� chronione przed z�em.", ch, NULL, victim, TO_CHAR );
					break;
				case 2:
					act( "$N jest ju� chroniona przed z�em.", ch, NULL, victim, TO_CHAR );
					break;
				default :
					act( "$N jest ju� chroniony przed z�em.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}
		return;
	}

	if ( IS_AFFECTED( victim, AFF_PROTECT_GOOD ) )
	{
		if ( victim == ch )
			send_to_char( "Jeste� ju� chronion<&y/a/e> przed dobrem.\n\r", ch );
		else
		{
			switch ( victim->sex )
			{
				case 0:
					act( "$N jest ju� chronione przed dobrem.", ch, NULL, victim, TO_CHAR );
					break;
				case 2:
					act( "$N jest ju� chroniona przed dobrem.", ch, NULL, victim, TO_CHAR );
					break;
				default :
					act( "$N jest ju� chroniony przed dobrem.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}
		return;
	}

	if ( IS_GOOD( victim ) )
	{
		if ( victim == ch )
			send_to_char( "Nie mo�esz powi�za� si� ze zlem!\n\r", ch );
		else
			act( "$N nie mo�e wi�za� si� ze z�em!", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( ch != victim )
			act( "Nie uda�o ci si� ochroni� $Z przed dobrem.", ch, NULL, victim, TO_CHAR );
		else
			send_to_char( "Nie uda�o ci si� ochroni� siebie przed dobrem.\n\r", ch );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 20 + level / 3; af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_PROTECT_GOOD;
	affect_to_char( victim, &af, NULL, TRUE );

	switch ( victim->sex )
	{
		case 0:
			send_to_char( "Czujesz si� powi�zane ze z�em.\n\r", victim );
			break;
		case 2:
			send_to_char( "Czujesz si� powi�zana ze z�em.\n\r", victim );
			break;
		default :
			send_to_char( "Czujesz si� powi�zany ze z�em.\n\r", victim );
			break;
	}
	if ( ch != victim )
		switch ( victim->sex )
		{
			case 0:
				act( "$N jest teraz chronione przed dobrem.", ch, NULL, victim, TO_CHAR );
				break;
			case 2:
				act( "$N jest teraz chroniona przed dobrem.", ch, NULL, victim, TO_CHAR );
				break;
			default :
				act( "$N jest teraz chroniony przed dobrem.", ch, NULL, victim, TO_CHAR );
				break;
		}
	return;
}

void spell_resist_fire( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ), mod, dur;

	if ( IS_AFFECTED( victim, AFF_RESIST_FIRE ) )
	{
		if ( victim == ch )
			send_to_char( "Nie mo�esz by� bardziej chronion<&y/a/e>.\n\r", ch );
		else
		{
			switch ( ch->sex )
			{
				case 0:
					act( "$N nie mo�e by� bardziej chronione.", ch, NULL, victim, TO_CHAR );
					break;
				case 2:
					act( "$N nie mo�e by� bardziej chroniona.", ch, NULL, victim, TO_CHAR );
					break;
				default :
					act( "$N nie mo�e by� bardziej chroniony.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( ch == victim )
        {
            send_to_char( "Nie uda�o ci si� zwi�kszy� swojej odporno�ci na ogie�.\n\r", ch );
        }
        else
        {
            act( "Nie uda�o ci si� zwi�kszy� odporno�ci $Z na ogie�.", ch, NULL, victim, TO_CHAR );
        }
        return;
	}



	mod = 30+level/2;
	dur = 4 + level / 6;

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur += 1 + level / 10;
			mod += level/2;
		}
	}

	    if( !spell_item_check( ch, sn , "fire based spell" ) )
    {
        send_to_char( "Brakuje ci agatu ognistego by zakl�cie osi�gne�o pe�nie mocy.\n\r", ch );
        mod = mod - 10;
    }

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = mod;
	af.duration = dur; af.rt_duration = 0;
	af.location = APPLY_RESIST;
	af.modifier = RESIST_FIRE;
	af.bitvector = &AFF_RESIST_FIRE;
	affect_to_char( victim, &af, NULL, TRUE );

	switch ( victim->sex )
	{
		case 0:
			send_to_char( "Czujesz, �e teraz jeste� bardziej odporne na ogie�.\n\r", victim );
			act( "$n jest teraz bardziej odporne na ogie�.", victim, NULL, NULL, TO_ROOM );
			break;
		case 2:
			send_to_char( "Czujesz, �e teraz jeste� bardziej odporna na ogie�.\n\r", victim );
			act( "$n jest teraz bardziej odporna na ogie�.", victim, NULL, NULL, TO_ROOM );
			break;
		default :
			send_to_char( "Czujesz, �e teraz jeste� bardziej odporny na ogie�.\n\r", victim );
			act( "$n jest teraz bardziej odporny na ogie�.", victim, NULL, NULL, TO_ROOM );
			break;
	}

	return;
}

/* Odpornosc na zimno (skladnia: cast 'resist cold' [kto])
 */

void spell_resist_cold( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ), mod, dur;

	if ( IS_AFFECTED( victim, AFF_RESIST_COLD ) )
	{

		if ( victim == ch )
			send_to_char( "Nie mo�esz by� bardziej chronion<&y/a/e>.\n\r", ch );
		else
		{
			switch ( ch->sex )
			{
				case 0:
					act( "$N nie mo�e by� bardziej chronione.", ch, NULL, victim, TO_CHAR );
					break;
				case 2:
					act( "$N nie mo�e by� bardziej chroniona.", ch, NULL, victim, TO_CHAR );
					break;
				default :
					act( "$N nie mo�e by� bardziej chroniony.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( ch == victim )
        {
			send_to_char( "Nie uda�o ci si� zwi�kszy� swojej odporno�ci na zimno.\n\r", ch );
        }
		else
        {
			act( "Nie uda�o ci si� zwi�kszy� odporno�ci $Z na zimno.", ch, NULL, victim, TO_CHAR );
        }
		return;
	}

	mod = 30+level/2;
	dur = 4 + level / 6;

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur += 1 + level / 10;
			mod += level/2;
		}
	}

	    if( !spell_item_check( ch, sn , "cold based spell" ) )
    {
        send_to_char( "Brakuje ci srebrnego proszku by zakl�cie osi�gne�o pe�nie mocy.\n\r", ch );
        mod = mod - 10;
    }

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = mod;
	af.duration = dur; af.rt_duration = 0;
	af.location = APPLY_RESIST;
	af.modifier = RESIST_COLD;
	af.bitvector = &AFF_RESIST_COLD;
	affect_to_char( victim, &af, NULL, TRUE );

	switch ( victim->sex )
	{
		case 0:
			send_to_char( "Czujesz, �e teraz jeste� bardziej odporne na zimno.\n\r", victim );
			act( "$n jest teraz bardziej odporne na zimno.", victim, NULL, NULL, TO_ROOM );
			break;
		case 2:
			send_to_char( "Czujesz, �e teraz jeste� bardziej odporna na zimno.\n\r", victim );
			act( "$n jest teraz bardziej odporna na zimno.", victim, NULL, NULL, TO_ROOM );
			break;
		default :
			send_to_char( "Czujesz, �e teraz jeste� bardziej odporny na zimno.\n\r", victim );
			act( "$n jest teraz bardziej odporny na zimno.", victim, NULL, NULL, TO_ROOM );
			break;
	}

	return;
}

void spell_resist_lightning( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ), mod, dur;

	if ( IS_AFFECTED( victim, AFF_RESIST_LIGHTNING ) )
	{
		if ( victim == ch )
			send_to_char( "Nie mo�esz by� bardziej chronion<&y/a/e>.\n\r", ch );
		else
		{
			switch ( ch->sex )
			{
				case 0:
					act( "$N nie mo�e by� bardziej chronione.", ch, NULL, victim, TO_CHAR );
					break;
				case 2:
					act( "$N nie mo�e by� bardziej chroniona.", ch, NULL, victim, TO_CHAR );
					break;
				default :
					act( "$N nie mo�e by� bardziej chroniony.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( ch == victim )
		{
			send_to_char( "Nie uda�o ci si� zwi�kszy� swojej odporno�ci na elektryczno��.\n\r", ch );
		}
		else
		{
			act( "Nie uda�o ci si� zwi�kszy� odporno�ci $Z na elektryczno��.", ch, NULL, victim, TO_CHAR );
		}
		return;
	}

	mod = 30+level/2;
	dur = 4 + level / 6;

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur += 1 + level / 10;
			mod += level/2;
		}
	}

	    if( !spell_item_check( ch, sn , "lighting based spell" ) )
    {
		send_to_char( "Brakuje ci kawaleczka bursztynu by zakl�cie osi�gne�o pe�nie mocy.\n\r", ch );
		mod = mod - 10;
    }

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = mod;
	af.duration = dur; af.rt_duration = 0;
	af.location = APPLY_RESIST;
	af.modifier = RESIST_ELECTRICITY;
	af.bitvector = &AFF_RESIST_LIGHTNING;
	affect_to_char( victim, &af, NULL, TRUE );

	switch ( victim->sex )
	{
		case 0:
			send_to_char( "Czujesz, �e teraz jeste� bardziej odporne na elektryczno��.\n\r", victim );
			act( "$n jest teraz bardziej odporne na elektryczno��.", victim, NULL, NULL, TO_ROOM );
			break;
		case 2:
			send_to_char( "Czujesz, �e teraz jeste� bardziej odporna na elektryczno��.\n\r", victim );
			act( "$n jest teraz bardziej odporna na elektryczno��.", victim, NULL, NULL, TO_ROOM );
			break;
		default :
			send_to_char( "Czujesz, �e teraz jeste� bardziej odporny na elektryczno��.\n\r", victim );
			act( "$n jest teraz bardziej odporny na elektryczno��.", victim, NULL, NULL, TO_ROOM );
			break;
	}

	return;
}

void spell_resist_acid( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ), mod, dur;

	if ( IS_AFFECTED( victim, AFF_RESIST_ACID ) )
	{
		if ( victim == ch )
			send_to_char( "Nie mo�esz by� bardziej chronion<&y/a/e>.\n\r", ch );
		else
		{
			switch ( ch->sex )
			{
				case 0:
					act( "$N nie mo�e by� bardziej chronione.", ch, NULL, victim, TO_CHAR );
					break;
				case 2:
					act( "$N nie mo�e by� bardziej chroniona.", ch, NULL, victim, TO_CHAR );
					break;
				default :
					act( "$N nie mo�e by� bardziej chroniony.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( ch == victim )
        {
			send_to_char( "Nie uda�o ci si� zwi�kszy� swojej odporno�ci na kwas.\n\r", ch );
        }
		else
        {
			act( "Nie uda�o ci si� zwi�kszy� odporno�ci $Z na kwas.", ch, NULL, victim, TO_CHAR );
        }
		return;
	}

	mod = 30+level/2;
	dur = 4 + level / 6;

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur += 1 + level / 10;
			mod += level/2;
		}
	}

	    if( !spell_item_check( ch, sn , "acid based spell" ) )
    {
		send_to_char( "Brakuje ci kwasu by zakl�cie osi�gne�o pe�nie mocy.\n\r", ch );
		mod = mod - 10;
    }

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = mod;
	af.duration = dur; af.rt_duration = 0;
	af.location = APPLY_RESIST;
	af.modifier = RESIST_ACID;
	af.bitvector = &AFF_RESIST_ACID;
	affect_to_char( victim, &af, NULL, TRUE );

	switch ( victim->sex )
	{
		case 0:
			send_to_char( "Czujesz, �e teraz jeste� bardziej odporne na kwas.\n\r", victim );
			act( "$n jest teraz bardziej odporne na kwas.", victim, NULL, NULL, TO_ROOM );
			break;
		case 2:
			send_to_char( "Czujesz, �e teraz jeste� bardziej odporna na kwas.\n\r", victim );
			act( "$n jest teraz bardziej odporna na kwas.", victim, NULL, NULL, TO_ROOM );
			break;
		default :
			send_to_char( "Czujesz, �e teraz jeste� bardziej odporny na kwas.\n\r", victim );
			act( "$n jest teraz bardziej odporny na kwas.", victim, NULL, NULL, TO_ROOM );
			break;
	}

	return;
}

void spell_resist_magic( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( IS_AFFECTED( victim, AFF_RESIST_FIRE ) ||
	     IS_AFFECTED( victim, AFF_RESIST_COLD ) ||
	     IS_AFFECTED( victim, AFF_RESIST_LIGHTNING ) ||
	     IS_AFFECTED( victim, AFF_RESIST_MAGIC ) )
	{
		if ( victim == ch )
			send_to_char( "Nie mo�esz by� bardziej chronion<&y/a/e>.\n\r", ch );
		else
		{
			switch ( victim->sex )
			{
				case SEX_NEUTRAL:
					act( "$N nie mo�e by� bardziej chronione.", ch, NULL, victim, TO_CHAR );
					break;
				case SEX_FEMALE:
					act( "$N nie mo�e by� bardziej chroniona.", ch, NULL, victim, TO_CHAR );
					break;
				case SEX_MALE:
				default :
					act( "$N nie mo�e by� bardziej chroniony.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( ch == victim )
        {
			send_to_char( "Nie uda�o ci si� zwi�kszy� swojej odporno�ci na magi�.\n\r", ch );
        }
		else
        {
			act( "Nie uda�o ci si� zwi�kszy� odporno�ci $Z na magi�.", ch, NULL, victim, TO_CHAR );
        }
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 3; af.rt_duration = 0;
	af.location = APPLY_SAVING_SPELL;
	af.modifier = -5;
	af.bitvector = &AFF_RESIST_MAGIC;
	affect_to_char( victim, &af, NULL, TRUE );

	switch ( victim->sex )
	{
		case 0:
			act( "$n jest teraz lepiej chronione przed magi�.", victim, NULL, NULL, TO_ROOM );
			send_to_char( "Czujesz, �e teraz jeste� lepiej chronione przed magi�.\n\r", victim );
			break;
		case 2:
			act( "$n jest teraz lepiej chroniona przed magi�.", victim, NULL, NULL, TO_ROOM );
			send_to_char( "Czujesz, �e teraz jeste� lepiej chroniona przed magi�.\n\r", victim );
			break;
		default :
			act( "$n jest teraz lepiej chroniony przed magi�.", victim, NULL, NULL, TO_ROOM );
			send_to_char( "Czujesz, �e teraz jeste� lepiej chroniony przed magi�.\n\r", victim );
			break;
	}

	return;
}

void spell_brave_cloak( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	CHAR_DATA *vch;
	AFFECT_DATA af;
	int duration;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( is_affected( victim, gsn_fear ) )
	{
		if ( check_dispel( level, victim, gsn_fear ) && number_range( 0, luck ) > 0 )
		{
			send_to_char( "Tw�j strach mija.\n\r", victim );
			act( "$n ju� si� nikogo nie boi.", victim, NULL, NULL, TO_ROOM );
		}
		else
		{
			act( "Nie uda�o ci si� uwolni� $Z od strachu.", ch, NULL, victim, TO_CHAR );
			return;
		}
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if( ch == victim )
			act( "Nie uda�o ci si� otoczy� siebie p�aszczem odwagi.", ch, NULL, victim, TO_CHAR );
		else
			act( "Nie uda�o ci si� otoczy� $Z p�aszczem odwagi.", ch, NULL, victim, TO_CHAR );
		return;
	}

	duration = 4 + level / ( ( number_range( 0, luck ) > 10 ) ? 6 : 7.5 );
	if ( number_range( 0, luck ) > 15 ) duration += 1;
	if ( number_range( 0, luck ) < 5 ) duration -= 2;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = URANGE( 20, level * 3, 90 );
	af.location = APPLY_RESIST;
	af.modifier = RESIST_FEAR;
	af.bitvector = &AFF_BRAVE_CLOAK;

	if ( victim != ch )
	{
		luck = get_curr_stat_deprecated( victim, STAT_LUC );
		if ( number_range( 0, luck ) > 15 ) duration += 1;
		if ( number_range( 0, luck ) < 5 ) duration -= 2;
		af.duration = UMAX( 1, duration ); af.rt_duration = 0;

		affect_to_char( victim, &af, NULL, TRUE );

		act( "Otaczasz si� p�aszczem odwagi.", ch, NULL, victim, TO_VICT );
		act( "$n otacza si� p�aszczem odwagi.", victim, NULL, victim, TO_ROOM );
	}
	else
	{
		for ( vch = ch->in_room->people;vch;vch = vch->next_in_room )
			if ( !is_affected( vch, sn ) && is_same_group( vch, ch ) )
			{
				luck = get_curr_stat_deprecated( vch, STAT_LUC );
				if ( number_range( 0, luck ) > 15 ) duration += 1;
				if ( number_range( 0, luck ) < 5 ) duration -= 2;
				af.duration = UMAX( 1, duration ); af.rt_duration = 0;

				affect_to_char( vch, &af, NULL, TRUE );
				act( "Otaczasz si� p�aszczem odwagi.", vch, NULL, NULL, TO_CHAR );
				act( "$n otacza si� p�aszczem odwagi.", vch, NULL, vch, TO_ROOM );
			}
	}
	return;
}

void spell_remove_fear( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( !IS_AFFECTED( victim, AFF_FEAR ) )
	{
		if ( victim == ch ) send_to_char( "Przecie� niczego si� nie boisz.\n\r", ch );
		else
			if ( victim->sex == 0 )
			{
				act( "$N nie jest przera�one.", ch, NULL, victim, TO_CHAR );
			}
			else if ( victim->sex == 1 )
			{
				act( "$N nie jest przera�ony.", ch, NULL, victim, TO_CHAR );
			}
			else
			{
				act( "$N nie jest przera�ona.", ch, NULL, victim, TO_CHAR );
			}
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( victim == ch ) send_to_char( "Nie uda�o ci si� przgna� strachu.\n\r", ch );
		else
		{
			switch ( victim->sex )
			{
				case 0:
					act( "$n nadal jest przera�one.", victim, NULL, NULL, TO_ROOM );
					break;
				case 1:
					act( "$n nadal jest przera�ony.", victim, NULL, NULL, TO_ROOM );
					break;
				default:
					act( "$n nadal jest przera�ona.", victim, NULL, NULL, TO_ROOM );
					break;
			}
		}
		return;
	}

	affect_strip( victim, gsn_fear );
	EXT_REMOVE_BIT( victim->affected_by, AFF_FEAR );

	send_to_char( "Tw�j strach mija.\n\r", victim );
	if ( victim->sex == 0 )
	{
		act( "$n ju� nie jest przera�one.", victim, NULL, NULL, TO_ROOM );
	}
	else if ( victim->sex == 1 )
	{
		act( "$n ju� nie jest przera�ony.", victim, NULL, NULL, TO_ROOM );
	}
	else
	{
		act( "$n ju� nie jest przera�ona.", victim, NULL, NULL, TO_ROOM );
	}

	return;
}

/* Sanktuarium (skladnia: cast sanctuary [kto])
 * a moze by sancta zrobic jak w baldurze? nikt cie nie moze zaatakowac,
 * az sam nie podejmiesz jakiejs ofensywnej akcji, albo minie czas czaru?
 * na razie zostawiam samego affecta.
 */
void spell_sanctuary( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat( ch, STAT_LUC );
	int duration;

	if ( IS_AFFECTED( victim, AFF_SANCTUARY ) )
	{
		if ( victim == ch )
		{
			send_to_char( "Jeste� ju� otoczon<&y/a/e> bia�� aur�.\n\r", victim );
		}
		else
		{
			if ( victim->sex == 0 )
			{
				act( "$N jest ju� otoczony bia�� aur�.", ch, NULL, victim, TO_CHAR );
			}
			else if ( victim->sex == 1 )
			{
				act( "$N jest ju� otoczony bia�� aur�.", ch, NULL, victim, TO_CHAR );
			}
			else
			{
				act( "$N jest ju� otoczony bia�� aur�.", ch, NULL, victim, TO_CHAR );
			}
		}
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( victim == ch ) send_to_char( "Nie uda�o ci si� otoczy� bia�� aur�.\n\r", ch );
		else act( "Nie uda�o ci si� otoczy� $Z bia�� aur�.", ch, NULL, victim, TO_CHAR );
		return;
	}

	duration = 3 + level/10 + UMAX( 0, level - LEVEL_HERO );
	if ( number_range( 0, OLD_28_VALUE ) > luck ) duration -= 1;
	if ( number_range( 0, luck ) < 30 ) duration -= 1;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 2, duration );
    af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_SANCTUARY;
	affect_to_char( victim, &af, NULL, TRUE );

	act( "$n otacza si� bia�� aur�.", victim, NULL, NULL, TO_ROOM );
	if ( victim == ch ) send_to_char( "Otacza ci� bia�a aura.\n\r", victim );
	else act( "$n otacza ci� bia�� aur�.", ch, NULL, victim, TO_VICT );

	return;
}

void spell_silence( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int vluck = get_curr_stat_deprecated( victim, STAT_LUC );
	int duration;

	if ( IS_AFFECTED( victim, AFF_SILENCE ) || saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) || IS_AFFECTED( victim, AFF_PERFECT_SENSES ) )
		return;

	if ( number_range( 0, vluck - luck ) > 5 )
	{
		send_to_char( "Twoja ofiara mia�a naprawd� du�o szcz�cia i nie uda�o ci si�.\n\r", ch );
		return;
	}

	duration = 4 + level / 7.5;
	if ( number_range( 0, luck ) > 15 ) duration += 1;
	if ( number_range( 0, luck ) < 5 ) duration -= 1;

	if ( number_range( 0, vluck ) > 15 ) duration -= 1;
	if ( number_range( 0, vluck ) < 5 ) duration += 1;

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			if ( level > 24 )
				++duration;
			++duration;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = &AFF_SILENCE;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Zosta�<&e�/a�/e�> uciszon<&y/a/e>!\n\r", victim );

	switch ( victim->sex )
	{
		case 0:
			act( "$n staje si� nagle bardzo ciche.", victim, NULL, NULL, TO_ROOM );
			break;
		case 1:
			act( "$n staje si� nagle bardzo cichy.", victim, NULL, NULL, TO_ROOM );
			break;
		default:
			act( "$n staje si� nagle bardzo cicha.", victim, NULL, NULL, TO_ROOM );
			break;
	}
	return;
}

/*  Drake:
 *  W do_damage sprawdza, czy wr�g ma mniej ni� 50% hp i aida na sobie, i je�li ma, to go leczy za modyfikator z czaru.
 *  Isnero: chwilowo przywracam stary aid
 */

void spell_aid( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int duration;

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( victim == ch ) send_to_char( "Nie uda�o ci si� sprowadzi� ochronnej boskiej mocy.\n\r", ch );
		else act( "Nie uda�o ci si� sprowadzi� ochronnej boskiej mocy na $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}
	
	if ( is_affected( victim, gsn_aid))
	{
	if ( victim == ch )
	act( "Jeste� ju� otoczony ochronn�, bosk� moc�.", ch, NULL, NULL, TO_CHAR);
	else
	act( "Wybrany cel jest ju� otoczony ochronn� bosk� moc�.", ch, NULL, NULL, TO_CHAR );
	return;
	}

	duration = 8 + level / 3;
	if ( number_range( 0, luck ) > 15 ) duration += 2;
	if ( number_range( 0, luck ) < 5 ) duration -= 2;

	af.where = TO_AFFECTS;
	af.type = gsn_aid;
	af.level = level;
	af.duration = UMAX( 8, duration ); af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 10 + ( ch->level * 2 ) ;
	af.bitvector = &AFF_AID;
	affect_to_char( victim, &af, NULL, TRUE );

	act( "Na $c sp�ywa boska moc.", victim, NULL, NULL, TO_ROOM );
	if ( victim == ch ) send_to_char( "Sam<&/a/o> powodujesz, �e sp�ywa na ciebie boska moc.\n\r", victim );
	else act( "Za po�rednictwem $z sp�ywa na ciebie boska moc.", ch, NULL, victim, TO_VICT );

	return;
}

void spell_chant( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim;
	CHAR_DATA *vch;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int duration, mod;

	if ( !ch->fighting || ch->fighting->in_room != ch->in_room )
	{
		send_to_char( "Ju� mia�<&e/a/o>� zacz�� �piewa� psalm, kiedy zda�<&e/a/o>� spraw�, �e nikt tu nie walczy.\n\r", ch );
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Co� zak��ci�o twoje skupienie i nie doko�czy�<&e/a/o>� �piewu.\n\r", ch );
		return;
	}

	duration = 2 + level / ( 10 + ( ( number_range( 0, luck ) > 10 ) ? 0 : 3 ) );
	if ( number_range( 0, luck ) > 15 ) duration += 1;
    if ( number_range( 0, luck ) < 5 ) duration -= 1;

	if ( ch->class == CLASS_CLERIC )
    {
		mod = URANGE( 2, 2 * level / 15 + ( number_range( 0, luck ) > 12 ? 1 : 0 ), 5 );
    }
    else
    {
        mod = URANGE( 1, ch->level / 10 + ( number_range( 0, luck ) > 12 ? 1 : 0 ), 3 );
    }
	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.location = APPLY_DAMROLL;
	af.modifier = mod;
	af.bitvector = &AFF_CHANT;
	/* dla grupki ch */

	for ( vch = ch->in_room->people;vch;vch = vch->next_in_room )
	{
		if ( !is_same_group( vch, ch ) || IS_AFFECTED( vch, AFF_CHANT )
		     || IS_AFFECTED( vch, AFF_PRAYER ) || !vch->fighting )
			continue;

		affect_to_char( vch, &af, "+Zyska�<&e�/a�/o�> przychylno�� bog�w", TRUE );
	}

	af.modifier = -mod;

	/* dla grupki victim */
	victim = ch->fighting;

	for ( vch = ch->in_room->people;vch;vch = vch->next_in_room )
	{
		if ( !is_same_group( vch, victim ) || IS_AFFECTED( vch, AFF_CHANT )
		     || IS_AFFECTED( vch, AFF_PRAYER ) || !vch->fighting )
			continue;

		affect_to_char( vch, &af, "-Bogowie nie sprzyjaj� tobie", TRUE );
	}

	act( "$n zaczyna nuci� psalm.", ch, NULL, NULL, TO_ROOM );
	act( "Zaczynasz nuci� psalm.", ch, NULL, NULL, TO_CHAR );
	return;
}

/* Stworzenie wody (skladnia: cast 'create water')
 * napelnia jeden obiekt aktualnie */

void spell_create_water( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj = ( OBJ_DATA * ) vo;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
    AFFECT_DATA *paf;

	if ( obj->item_type != ITEM_DRINK_CON )
	{
		print_char( ch, "%s nie mo�na nape�ni� wod�.\n\r", capitalize( obj->name2 ) );
		return;
	}

	if ( obj->value[ 0 ] == obj->value[ 1 ] )
	{
		print_char( ch, "W %s nie zmie�ci si� ju� nic wi�cej.\n\r", obj->name6 );
		return;
	}

	if ( obj->value[ 2 ] != LIQ_WATER && obj->value[ 1 ] > 0 )
	{
		print_char( ch, "W %s jest jakas inna ciecz.\n\r", obj->name6 );
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		print_char( ch, "Nie uda�o ci si� wype�ni� %s wod�.\n\r", obj->name2 );
		return;
	}
	if ( ( paf = affect_find( ch->affected, gsn_prayer_last )) != NULL )
    {
        if ( paf->modifier < 30 )
        {
             if ( ( paf = affect_find( ch->affected, gsn_holy_pool )) != NULL )
                  {

                    if (number_percent()  < paf->modifier)
                    {
                       obj->value[ 2 ] = LIQ_HEALING_WATER;
	                   obj->value[ 1 ] = obj->value[ 0 ];
	                   send_to_char( "Czujesz jak zebran� wod� zaczyna przenika� wype�niaj�ca ci� �wi�ta moc.\n\r", ch );
	                   act( "$n nape�nia $h uzdrawiaj�c� wod�.", ch, obj, NULL, TO_ROOM );
	                   print_char( ch, "%s nape�nia si� uzdrawiaj�c� wod�.\n\r", capitalize( obj->short_descr ) );
	                   paf->modifier -= 1;
	                   if (paf->modifier < 1) paf->modifier = 1;
                       return;
                    }

                    if (number_percent() < (-paf->modifier))
                    {
                       obj->value[ 2 ] = LIQ_WATER;
	                   obj->value[ 1 ] = obj->value[ 0 ];
	                   obj->value[ 3 ] = 1;
	                   send_to_char( "Czujesz jak z zakl�ciem dzieje si� co� z�ego.\n\r", ch );
	                   act( "$n nape�nia $h  wod�.", ch, obj, NULL, TO_ROOM );
	                   print_char( ch, "%s nape�nia si� wod�.\n\r", capitalize( obj->short_descr ) );
	                   return;

                    }


                  }


        }

    }

	act( "$n nape�nia $h wod�.", ch, obj, NULL, TO_ROOM );
	print_char( ch, "%s nape�nia si� wod�.\n\r", capitalize( obj->short_descr ) );

	obj->value[ 2 ] = LIQ_WATER;
	obj->value[ 1 ] = obj->value[ 0 ];
	return;
}

/*
 * Tener:
 * create healing water : dok�adna kopia create water, za wyj�tkiem innych komunikat�w oraz rodzaju p�ynu.
 * Nie mam si�y abstrahowa� tego czarku
 */

void spell_create_healing_water( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj = ( OBJ_DATA * ) vo;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( obj->item_type != ITEM_DRINK_CON )
	{
		print_char( ch, "%s nie mo�na nape�ni� uzdrawiaj�c� wod�.\n\r", capitalize( obj->name2 ) );
		return;
	}

	if ( obj->value[ 0 ] == obj->value[ 1 ] )
	{
		print_char( ch, "W %s nie zmie�ci si� ju� nic wi�cej.\n\r", obj->name6 );
		return;
	}

	if ( obj->value[ 2 ] != LIQ_HEALING_WATER && obj->value[ 1 ] > 0 )
	{
		print_char( ch, "W %s jest jakas inna ciecz.\n\r", obj->name6 );
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		print_char( ch, "Nie uda�o ci si� wype�ni� %s uzdrawiaj�c� wod�.\n\r", obj->name2 );
		return;
	}

	act( "$n nape�nia $h uzdrawiaj�c� wod�.", ch, obj, NULL, TO_ROOM );
	print_char( ch, "%s nape�nia si� uzdrawiaj�c� wod�.\n\r", capitalize( obj->short_descr ) );

	obj->value[ 2 ] = LIQ_HEALING_WATER;
	obj->value[ 1 ] = obj->value[ 0 ];
	return;
}

/* Dobre jagody (skladnia: cast goodbarry)
 *
 * robi iles tam jagod, ktore z proga daja hp
 */

void spell_goodbarry( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj;
	int i, ile;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_NOREST ) || ch->in_room->sector_type == 25 )
	{
		send_to_char( "Nie uda�o ci si� sprowadzi� jag�d.\n\r", ch );
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda�o ci si� sprowadzi� jag�d.\n\r", ch );
		return;
	}

	ile = number_range( level / 4, level / 2 );

	if ( !stat_throw( ch, STAT_WIS ) && !stat_throw( ch, STAT_LUC ) ) ile /= 2;
	if ( stat_throw( ch, STAT_WIS ) && stat_throw( ch, STAT_LUC ) ) ++ile;

	ile = URANGE( 1, ile, 9 );

	for ( i = 0; i < ile; i++ )
	{
		obj = create_object( get_obj_index( OBJ_VNUM_GOODBARRY ), FALSE );
		obj->value[ 0 ] = 6;
		obj->value[ 1 ] = 0;
		obj_to_room( obj, ch->in_room );
	}

	if ( ile == 1 ) act( "Na ziemi pojawia si� jagoda.", ch, NULL, NULL, TO_ALL );
	else act( "Na ziemi pojawia si� kilka jag�d.", ch, NULL, NULL, TO_ALL );

	return;
}

/* Chodzenie po wodzie (skladnia: cast waterwalk [kto])
 */
void spell_waterwalk( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int duration;

	if ( IS_AFFECTED( victim, AFF_WATERWALK ) )
	{
		if ( victim == ch )
			send_to_char( "Ju� potrafisz chodzi� po wodzie.\n\r", ch );
		else
		{
			switch ( victim->sex )
			{
				case 0:
					send_to_char( "Ono ju� to potrafi.\n\r", ch );
					break;
				case 1:
					send_to_char( "On ju� to potrafi.\n\r", ch );
					break;
				default :
					send_to_char( "Ona ju� to potrafi.\n\r", ch );
					break;
			}
		}
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda�o ci si�.\n\r", ch );
		return;
	}

	duration = 3 + ch->level / ( 10 + ( ( number_range( 0, luck ) > 10 ) ? 0 : 5 ) );

	if ( number_range( 0, luck ) > 15 ) duration += 1;
	if ( number_range( 0, luck ) < 5 ) duration -= 2;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.location = APPLY_NONE;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.modifier = 0;

	af.bitvector = &AFF_WATERWALK;
	affect_to_char( victim, &af, NULL, TRUE );

	act( "Rzucony przez $z czar powoduje, �e od teraz mo�esz chodzi� po wodzie.", ch, NULL, victim, TO_VICT );
	if ( victim == ch )
		send_to_char( "Teraz potrafisz chodzic po wodzie!\n\r", victim );
	else
		act( "Uda�o ci si� rzuci� zakl�cie na $Z.", ch, NULL, victim, TO_CHAR );

	return;
}

/**
 * Poznanie charakteru (skladnia: cast 'know alignment' <kto>)
 */
void spell_know_alignment( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	char buf[ MAX_STRING_LENGTH ];
	char *color;

	if ( victim == ch )
	{
		send_to_char( "Nie dowiesz si� niczego nowego o sobie.\n\r", victim );
		return;
	}

	act( "$n patrzy si� na ciebie przenikliwie.", ch, NULL, victim, TO_VICT );

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		act( "Nie uda�o ci si� pozna� charakteru $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( victim->alignment > 900 ) color = "{Gjasnozielon�";
	else if ( victim->alignment > 700 ) color = "{gzielon�";
	else if ( victim->alignment > 500 ) color = "{Gciemnozielon�";
	else if ( victim->alignment > 300 ) color = "{bciemnoniebiesk�";
	else if ( victim->alignment > -100 ) color = "{Cjasnoniebiesk�";
	else if ( victim->alignment > -300 ) color = "{bciemnoniebiesk�";
	else if ( victim->alignment > -500 ) color = "{rciemnoczerwon�";
	else if ( victim->alignment > -700 ) color = "{Rczerwon�";
	else if ( victim->alignment > -900 ) color = "{Rjasnoczerwon�";
	else color = "{R^fmigoczac� czerwon�";

	switch ( victim->sex )
	{
		case 0:
			sprintf( buf, "$N otoczone jest %s{x aur�.", color );
			break;
		case 1:
			sprintf( buf, "$N otoczony jest %s aur�.", color );
			break;
		default:
			sprintf( buf, "$N otoczona jest %s aur�.", color );
			break;
	}

	act( buf, ch, NULL, victim, TO_CHAR );

	return;
}

/* Oddychanie woda (skladnia: cast 'water breathing' [kto])
 */

void spell_water_breathing( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int duration;

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( victim != ch )
			act( "Nie uda�o ci si� rzuci� zakl�cia oddychania pod wod� na $Z.", ch, NULL, victim, TO_CHAR );
		else
			send_to_char( "Nie uda�o ci si� rzuci� zakl�cia oddychania pod wod�.\n\r", ch );
		return;
	}

	duration = 2 + level / ( 9 + ( ( number_range( 0, luck ) > 10 ) ? 0 : 3 ) );
	if ( number_range( 0, luck ) > 15 ) duration += 1;
	if ( number_range( 0, luck ) < 5 ) duration -= 2;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_WATERBREATH;
	affect_to_char( victim, &af, NULL, TRUE );

	if ( victim != ch )
		act( "Pod wp�ywem twojego zakl�cia $Z mo�e oddycha� pod wod�.", ch, NULL, victim, TO_CHAR );

	if ( !IS_NPC( victim ) && victim->pcdata->oxygen > 0 &&
	     IS_SET( sector_table[ victim->in_room->sector_type ].flag, SECT_UNDERWATER ) )
	{
		victim->pcdata->oxygen = 0;
		send_to_char( "Ze zdziwieniem stwierdzasz �e mo�esz oddycha� pod wod�.\n\r", ch );
		act( "$n z niedowierzaniem pr�buje oddycha� wod�.", victim, NULL, NULL, TO_ROOM );
	}
	else
		send_to_char( "Mo�esz teraz oddycha� pod wod�.\n\r", victim );

	return;
}

/* Przepedzenie dobra (skladnia: cast 'dispel good' <kto>)
 *
 * hmm no z opisu by wynikalo, ze ma albo odsylac w kosmos(slay)
 * albo mocno walic i zmuszac do ucieczki
 * nie wiem za bardzo jak to mam zrobic. Na razie bedzie walic w zaleznosci
 * od alignmentu ofiary i rzucajacego. pozniej ew si� cosik wymysli
 */
 /*Drake: Wywalanie slayowania z dispela, dodanie - przy duzych roznicach poziom�w
 kler wali poprostu wiecej (+1d10 za kazdy 1 poziom ponad 8 roznicy) obrazen.*/
void spell_dispel_good( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int dam, vluck, modyfikator;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( victim == ch )
	{
		send_to_char( "Sam<&/a/o> siebie chcesz odp�dzi�?\n\r", ch );
		return;
	}

	if ( IS_GOOD( ch ) )
	{
		send_to_char( "Tylko kto� powi�zany ze Z�em mo�e przep�dza� Dobro!\n\r", ch );
		return;
	}

	if ( !IS_GOOD( victim ) )
	{
		send_to_char( "Ten czar jest skuteczny tylko przeciw Dobru.\n\r", ch );
		return;
	}

	vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( number_range( 0, luck + vluck ) == 0 )
	{
		act( "Nie uda�o ci si� przep�dzi� Dobra w $B.", ch, NULL, victim, TO_CHAR );
		return;
	}

	//Dopalka przy duzej roznicy poziomow do obrazen.
	modyfikator = level - (victim->level) - 8;
	if (modyfikator < 0) modyfikator = 0;

	/*damage zalezny od alignmentu
	 * 1000-(-1000) /16 = 125
	 * 350-(-350)   /16 = 44
	 * chyba wystarczy nie?
	 * dodatkowo modyfikatory od szcz�cia
	 */

	dam = ( victim->alignment - ch->alignment ) / 16 + dice( modyfikator, 10) + dice (2, 12);

	if ( number_range( 0, luck ) > 15 ) dam += 10;
	if ( number_range( 0, luck ) < 5 ) dam -= 10;
	if ( number_range( 0, vluck ) > 15 ) dam -= 10;
	if ( number_range( 0, vluck ) < 5 ) dam += 10;

	dam = luck_dam_mod( ch, dam );

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		dam -= dam/4;
	 	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
			dam -= dam/4;
	}

	if ( IS_NEUTRAL( ch ) )
	   dam = (dam*3)/4;
	   
	if ( IS_AFFECTED( victim, AFF_ENERGY_SHIELD ))
	{
	send_to_char( "Otaczaj�ca ci� tarcza energi os�abi�a moc czaru.\n\r", victim );
	dam -= dam/5;
	}
	
	spell_damage( ch, vo, dam, sn, DAM_NONE, TRUE );
	return;
}

/* Przepedzenie zla (skladnia: cast 'dispel evil' <kto>)
 *
 * to samo co dispel_good
 */
void spell_dispel_evil( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA *paf;
	int dam, vluck, modyfikator, holy_dam_mod = 100;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	bool holy_bonus = TRUE;

	if ( victim == ch )
	{
		send_to_char( "Sam<&/a/o> siebie chcesz odp�dzi�?\n\r", ch );
		return;
	}

	if ( IS_EVIL( ch ) )
	{
		send_to_char( "Tylko kto� si�� Dobra mo�e przepedza� Z�o!\n\r", ch );
		return;
	}
	if ( !IS_EVIL( victim ) )
	{
		send_to_char( "Ten czar jest skuteczny tylko przeciw Z�u.\n\r", ch );
		return;
	}

   //blokada braku modlitwy
    if ( ( paf = affect_find( ch->affected, gsn_prayer_last )) != NULL )
    {
        if ( paf->modifier > 48 )
        {
            send_to_char( "Starasz si� zebra� w sobie moce dobra, jednak tw�j B�g ci nie sprzyja. \n\r", ch );
            act( "$n stara si� skoncentrowa�, jednak co� mu najwyra�niej przeszkodzi�o, poniewa� otwiera oczy z zdziwieniem.", ch, NULL, NULL, TO_ROOM );
            holy_bonus = FALSE;
            return;
        }
        else if ( paf->modifier > 36 )
        {
            send_to_char( "Zbieraj�c w sobie moce dobra wyczuwasz, ze co� jest nie tak. \n\r", ch );
            holy_dam_mod = 50;
            holy_bonus = FALSE;
        }
    }
    //bonusy od bycia dobrym, zlym paladynem
    if ( ( paf = affect_find( ch->affected, gsn_holy_pool )) != NULL && holy_bonus == TRUE)
    {
         if (paf->modifier > 30 && number_percent() < paf->modifier/2)
         {
             holy_dam_mod += 20;
             paf->modifier -= 3;
         }

          if (paf->modifier > 60 && number_percent() < paf->modifier/3)
         {
             holy_dam_mod += 20;
             paf->modifier -= 4;
         }

          if (paf->modifier < - 40 )
         {
             holy_dam_mod -= 20;

         }

          if (paf->modifier < - 60 )
         {
             holy_dam_mod -= 30;

         }

          if (paf->modifier < - 80 && paf->modifier > -100 )
         {
          send_to_char( "Rzucaj�c zakl�cie czujesz nag�y ucisk w gardle, w oczach ci ciemnieje, a ca�a zgromadzona moc rozchodzi si� w nico��. \n\r", ch );
          act( "$n stara si� skoncentrowa�, nagle zaczyna sapa�, jakby kto� go dusi�. Po chwili odzyskuje trze�wo�� umys�u.", ch, NULL, NULL, TO_ROOM );
          return;
         }

           if (paf->modifier <= - 100 )
         {
          send_to_char( "Inkantuj�c zakl�cie zaczynasz czu� b�l w sercu, a tw�j umys� wype�nia strach. \n\r", ch );
          act( "$n wypowiada zakl�cie, a w jego oczach pojawia si� przera�enie.", ch, NULL, NULL, TO_ROOM );
          dam = (-paf->modifier/2);
          spell_damage( ch, ch, dam, sn, DAM_NONE, TRUE );
          return;
         }

    }

	vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( number_range( 0, luck + vluck ) == 0 )
	{
		act( "Nie uda�o ci si� przep�dzi� Z�a w $B.", ch, NULL, victim, TO_CHAR );
		return;
	}

		//Dopalka przy duzej roznicy poziomow do obrazen.
	modyfikator = level - victim->level - 8;
	if (modyfikator < 0) modyfikator = 0;

	/*
	 *
	 * Wyliczanie damage zale�nego od alignmentu
	 * 1000-(-1000) /16 = 125
	 * 350-(-350)   /16 = 44
	 * chyba wystarczy nie?
	 *
	 */
  dam = ( ch->alignment - victim->alignment ) / 16 + dice( modyfikator, 10) + dice (2, 12);//Raszer, poprawa dmg, ch->align - victim->align zamiast odwrotnie
	/*
	 * losowanie szaans na zmniejszenie lub zwi�kszenie dam
	 */
	if ( number_range( 0, luck ) > 15 ) dam += 10;
	if ( number_range( 0, luck ) < 5 ) dam -= 10;
	if ( number_range( 0, vluck ) > 15 ) dam -= 10;
	if ( number_range( 0, vluck ) < 5 ) dam += 10;

	dam = luck_dam_mod( ch, dam );
	/*
	 * Damage po modyfikacjach szans:
	 *
	 * maksymalny - 156 + 1-10 za kazdy poziom roznicy powyzej 9.
	 * minimalny  - 27
	 *
	 */

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		dam -= dam / 4;
		if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
			dam -= dam/4;
	}

	 if ( ch->class == CLASS_PALADIN && !IS_NPC(ch) )
	 dam -= dam / number_range( 4, 5 );

	if ( IS_NEUTRAL( ch ) )
	   dam = (dam*3)/4;
	   
	if ( IS_AFFECTED( victim, AFF_ENERGY_SHIELD ))
	{
	send_to_char( "Otaczaj�ca ci� tarcza energi os�abi�a moc czaru.\n\r", victim );
	dam -= dam/5;
	}	   
	   
    if (holy_dam_mod > 100) send_to_char( "Czujesz jak zakl�cie zaczyna rezonowa� z zgromadzon� w tobie �wi�t� moc�. \n\r", ch );
    if (holy_dam_mod < 100) send_to_char( "Czujesz jak zebrana w tobie ciemno�� os�abia moc zakl�cia. \n\r", ch );
    dam *= holy_dam_mod/100;
	spell_damage( ch, vo, dam, sn, DAM_NONE, TRUE );
	return;
}

/* Wykrycie dobra (skladnia: cast 'detect good' [kto])
 *
 * tutaj bedzie tylko affect, reszta w patrzeniu i ruszaniu sie
 */

void spell_detect_good( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int duration, luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( IS_AFFECTED( victim, AFF_DETECT_GOOD ) )
	{
		if ( victim == ch ) send_to_char( "Przecie� ju� potrafisz wykry� dobro.\n\r", ch );
		else act( "$N ju� potrafi wykry� dobro.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( victim != ch )
		{
			switch ( victim->sex )
			{
				case 0:
					act( "Nie uda�o ci si� sprawi�, by $N potrafi�o wykrywa� dobro.", ch, NULL, victim, TO_CHAR );
					break;
				case 1:
					act( "Nie uda�o ci si� sprawi�, by $N potrafi� wykrywa� dobro.", ch, NULL, victim, TO_CHAR );
					break;
				default:
					act( "Nie uda�o ci si� sprawi�, by $N potrafi�a wykrywa� dobro.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}
		else
			send_to_char( "Nie uda�o ci si� sprawi�, �eby� wykrywa�<&/a/o> dobro.\n\r", ch );
		return;
	}

	duration = level;
	if ( number_range( 0, luck ) > 15 ) duration += number_range( 2, level / 5 );
	if ( number_range( 0, luck ) < 5 ) duration /= 2;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = &AFF_DETECT_GOOD;
	affect_to_char( victim, &af, NULL, TRUE );
	send_to_char( "Wyra�niej dostrzegasz bia�� magi�.\n\r", victim );
	if ( ch != victim )
		act( "Pod wp�ywem twojego zakl�cia $Z dotrzega przejawy bia�ej magii.", ch, NULL, victim, TO_CHAR );
	return;
}

/* Wykrycie zla (skladnia: cast 'detect evil' [kto])
 *
 * tutaj bedzie tylko affect, reszta w patrzeniu i ruszaniu sie
 */

void spell_detect_evil( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int duration, luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( IS_AFFECTED( victim, AFF_DETECT_EVIL ) )
	{
		if ( victim == ch ) send_to_char( "Przecie� ju� potrafisz wykryc zlo.\n\r", ch );
		else act( "$N ju� potrafi wykryc zlo.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( victim != ch )
		{
			switch ( victim->sex )
			{
				case 0:
					act( "Nie uda�o ci si� sprawi�, by $N potrafi�o wykrywa� z�o.", ch, NULL, victim, TO_CHAR );
					break;
				case 1:
					act( "Nie uda�o ci si� sprawi�, by $N potrafi� wykrywa� z�o.", ch, NULL, victim, TO_CHAR );
					break;
				default:
					act( "Nie uda�o ci si� sprawi�, by $N potrafi�a wykrywa� z�o.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}
		else
			send_to_char( "Nie uda�o ci si� sprawi�, �eby� wykrywa�<&/a/o> z�o.\n\r", ch );
		return;
	}

	duration = level;
	if ( number_range( 0, luck ) > 15 ) duration += number_range( 2, level / 5 );
	if ( number_range( 0, luck ) < 5 ) duration /= 2;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = &AFF_DETECT_EVIL;
	affect_to_char( victim, &af, NULL, TRUE );
	send_to_char( "Wyrazniej dostrzegasz czarna magie.\n\r", victim );
	if ( ch != victim )
		act( "Pod wp�ywem twojego zakl�cia $Z dotrzega przejawy czarnej magii.", ch, NULL, victim, TO_CHAR );
	return;
}

/* Plomienne ostrze (skladnia: cast 'flame blade')
 *
 * Wlasciwie to bedzie to samo, co spirit_hammer, tez bron
 * tylko obrazenia beda od ognia
 */

void spell_flame_blade( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    OBJ_DATA * obj = NULL;
    int duration, luck = get_curr_stat_deprecated( ch, STAT_LUC );

    if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
    {
        send_to_char( "Nie uda�o ci si� przywo�a� p�omiennego ostrza.\n\r", ch );
        return;
    }

    if ( get_hand_slots( ch, WEAR_WIELD ) >= 2 )
    {
        send_to_char( "Masz ju� dwie zaj�te r�ce. Zdejmij co�.\n\r", ch );
        return;
    }

    if ( !get_eq_char( ch, WEAR_WIELD ) )
    {
        obj = create_object( get_obj_index( OBJ_VNUM_FLAMEBLADE ), FALSE );

        if ( !obj )
        {
            bug( "Brak obiektu: OBJ_VNUM_FLAMEBLADE.", 0 );
            return;
        }

        duration = level;
        if ( number_range( 0, luck ) > 10 )
        {
            duration *= 2;
        }
        obj->timer = duration;
        obj->rent_cost = 2 * level;

        obj->value[ 1 ] = 2 + ( ( number_range( 0, luck ) > 10 ) ? 1 : 0 ) + ( ( number_range( 0, level ) > 17 ) ? 1 : 0 );
        obj->value[ 2 ] = 3 + ( ( number_range( 0, luck ) > 10 ) ? 1 : 0 ) + ( ( number_range( 0, level ) > 17 ) ? 1 : 0 );
        //Brohacz: przeglad broni z czarow
        //		obj->value[ 5 ] = URANGE( 1, level / 6, 5 );
        //		obj->value[ 6 ] = URANGE( 1, level / 6, 5 );
        obj->value[ 5 ] = 0;
        obj->value[ 6 ] = 0;

        if ( number_range( 1, 2*level ) > 10 )
            obj->value[ 5 ] += 1;

        if ( number_range( 1, 2*level ) > 15 )
            obj->value[ 5 ] += 1;

        if ( number_range( 1, 2*level ) > 10 )
            obj->value[ 6 ] += 1;

        if ( number_range( 1, 2*level ) > 15 )
            obj->value[ 6 ] += 1;

        obj_to_char( obj, ch );
        equip_char( ch, obj, WEAR_WIELD, TRUE );
        act( "Z r�ki $z wyrasta p�omienne ostrze.", ch, NULL, NULL, TO_ROOM );
        send_to_char( "Z twej r�ki wyrasta p�omienne ostrze.\n\r", ch );

        if ( number_range( 0, 620 ) < level )  // koles ma farta i przywolal lepsze ostrze
        {
            obj->value[ 1 ] += 1;
            obj->value[ 2 ] += 1;
            obj->value[ 5 ] += 1;
            obj->value[ 6 ] += 1;

            send_to_char( "P�omienne ostrze rozb�yska przez chwil� bardzo jasnym p�omieniem, kt�ry po chwili przygasa.\n\r", ch );
        }

        return;
    }
    send_to_char( "Przecie� trzymasz ju� co� w r�ce.\n\r", ch );
    return;
}

/* Ochrona przed energia (skladnia: cast 'energy shield' [kto])
 *
 * resist na negatywna i pozytywna energie
 *
 */
void spell_energy_shield( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ), dur, mod;

	if ( IS_AFFECTED( victim, AFF_ENERGY_SHIELD ) || IS_AFFECTED( victim, AFF_RESIST_NEGATIVE ))
	{
		if ( ch == victim )
			send_to_char( "Nie mo�esz by� ju� bardziej chronion<&y/a/e>.\n\r", ch );
		else
			switch ( victim->sex )
			{
				case 0:
					act( "$N nie mo�e by� ju� bardziej chronione.", ch, NULL, victim, TO_CHAR );
					break;
				case 1:
					act( "$N nie mo�e by� ju� bardziej chroniony.", ch, NULL, victim, TO_CHAR );
					break;
				default:
					act( "$N nie mo�e by� ju� bardziej chroniona.", ch, NULL, victim, TO_CHAR );
					break;
			}
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( victim != ch )
			act( "Nie uda�o ci si� otoczy� $Z polem energii.", ch, NULL, victim, TO_CHAR );
		else
			send_to_char( "Nie uda�o ci si� otoczy� polem energii.\n\r", ch );
		return;
	}

	mod = 30 + level/2 + ( ( luck > 15 ) ? 5 : 0 );
	dur = 3 + level / 6 + ( ( luck > 15 ) ? 1 : 0 );
	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			mod += level / 2;
			dur += level / 6;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = mod;
	af.duration = dur; af.rt_duration = 0;
	af.location = APPLY_RESIST;
	af.modifier = RESIST_NEGATIVE;
	af.bitvector = &AFF_ENERGY_SHIELD;
	affect_to_char( victim, &af, NULL, TRUE );

	af.modifier = RESIST_HOLY;
	affect_to_char( victim, &af, NULL, TRUE );

	if ( ch == victim )
	{
		act( "$n otacza si� polem energii.", ch, NULL, victim, TO_ROOM );
		send_to_char( "Czujesz, �e chroni ci� pole energii.\n\r", victim );
	}
	else
	{
		act( "$n otacza $C polem energii.", ch, NULL, victim, TO_NOTVICT );
		act( "$n otacza ci� polem energii.", ch, NULL, victim, TO_VICT );
		act( "Otaczasz $C polem energii.", ch, NULL, victim, TO_CHAR );
	}
	return;
}

void spell_prayer( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim;
	CHAR_DATA *vch;
	AFFECT_DATA af;
	int duration, luck = get_curr_stat_deprecated( ch, STAT_LUC ), mod;

	if ( !ch->fighting || ch->fighting->in_room != ch->in_room )
	{
		send_to_char( "Nie bierzesz udzia�u w walce.\n\r", ch );
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "W ostatniej chwili rozpraszasz swoj� uwag�.\n\r", ch );
		return;
	}

	duration = 2 + level / 10;

	if ( ch->class == CLASS_CLERIC )
    {
		mod = URANGE( 2, 2 * level / 15 + ( number_range( 0, luck ) > 12 ? 1 : 0 ), 5 );
    }
	else
    {
		mod = URANGE( 1, ch->level / 10 + ( number_range( 0, luck ) > 12 ? 1 : 0 ), 3 );
    }

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = duration; af.rt_duration = 0;
	af.location = APPLY_HITROLL;
	af.bitvector = &AFF_PRAYER;

	/* dla grupki ch (+1) */
	for ( vch = ch->in_room->people;vch;vch = vch->next_in_room )
	{
		if ( !is_same_group( vch, ch ) || IS_AFFECTED( vch, AFF_PRAYER ) || IS_AFFECTED( vch, AFF_CHANT ) || !vch->fighting )
			continue;

		luck = get_curr_stat_deprecated( vch, STAT_LUC );

		af.modifier = mod;

		affect_to_char( vch, &af, "+Jeste� pod wp�ywem b�ogos�awie�stwa celno�ci", TRUE );
	}

	/* dla grupki victim (-1) */
	victim = ch->fighting;

	for ( vch = ch->in_room->people;vch;vch = vch->next_in_room )
	{
		if ( !is_same_group( vch, victim ) || IS_AFFECTED( vch, AFF_PRAYER ) || IS_AFFECTED( vch, AFF_CHANT ) || !vch->fighting )
			continue;

		luck = get_curr_stat_deprecated( vch, STAT_LUC );

		af.modifier = - mod;

		affect_to_char( vch, &af, "-Jeste� pod wp�ywem przekle�stwa celno�ci", TRUE );
	}

	act( "$n zaczyna nuci� modlitw�.", ch, NULL, NULL, TO_ROOM );
	act( "Zaczynasz nuci� modlitw�.", ch, NULL, NULL, TO_CHAR );
	return;
}

/* Fajerwerki (skladnia: cast pyrotechnics)
 *
 * wlasciwie, to daje blinda na 3 ticki wszystkim poza grupa kaplana
 * i innymi kaplanami
 */

void spell_pyrotechnics( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * vch;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda�o ci si� wywo�a� wybuchu.\n\r", ch );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dice( 1, 3 ) + ch->level / 10 + ( number_range( 0, luck ) > 10 ) ? 1 : 0 ; af.rt_duration = 0;
	af.location = APPLY_HITROLL;
	af.bitvector = &AFF_BLIND;

	act( "Przera�liwie jasna kula unosi si� w powietrze i eksploduje {Wbia�ym{x �wiat�em!", ch, NULL, NULL, TO_ALL );

	for ( vch = ch->in_room->people;vch;vch = vch->next_in_room )
	{
		if ( IS_AFFECTED( vch, AFF_BLIND ) || is_safe( ch, vch ) || is_same_group( vch, ch ) || IS_AFFECTED( vch, AFF_PERFECT_SENSES ) || is_undead(vch) )
			continue;

		if ( dice( 2, level ) < vch->level || saves_spell_new( vch, skill_table[sn].save_type, skill_table[sn].save_mod, ch, sn ) )
		{
			if ( vch->fighting == NULL && can_see( vch, ch ) && can_move( vch ) )
			{
				if ( IS_NPC( vch ) && HAS_TRIGGER( vch, TRIG_KILL ) )
					mp_percent_trigger( vch, ch, NULL, NULL, &TRIG_KILL );

				multi_hit( vch, ch, TYPE_UNDEFINED );
			}
			continue;
		}

		af.modifier = -4 + ( number_range( 0, get_curr_stat_deprecated( vch, STAT_LUC ) ) > 15 ) ? 1 : 0;
		affect_to_char( vch, &af, NULL, TRUE );

		act( "O�lepiasz $C!", ch, NULL, vch, TO_CHAR );
		send_to_char( "Zosta�<&e/a/o>� o�lepion<&y/a/e>!\n\r", vch );
		act( "Jaskrawe �wiat�o o�lepia $C!", ch, NULL, vch, TO_NOTVICT );
	}

	return;
}

/* Zdjecie paralizu (skladnia: cast 'remove paralysis' <kto>)
 *
 * zdejmuje holdy, hmm z truciznami nie wiem jak bo ich jeszcze nie ma
 *
 */
void spell_remove_paralysis( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA *af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( !IS_AFFECTED( victim, AFF_PARALYZE ) )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "$N nie jest sparali�owane.", ch, NULL, victim, TO_CHAR );
				break;
			case 1:
				act( "$N nie jest sparali�owany.", ch, NULL, victim, TO_CHAR );
				break;
			default:
				act( "$N nie jest sparali�owana.", ch, NULL, victim, TO_CHAR );
				break;
		}
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		act( "Nie uda�o ci si� usun�� parali�u $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	for ( af = victim->affected; af != NULL; af = af->next )
	{
		if ( af->bitvector == &AFF_PARALYZE )
			affect_strip( victim, af->type );
	}

    act( "$n odzyskuje kontrol� nad swoim cia�em.", victim, NULL, NULL, TO_ROOM );
    send_to_char( "Odzyskujesz kontrol� nad swoim cia�em.\n\r", victim );

	return;
}

void spell_blindness( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int duration;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( IS_AFFECTED( victim, AFF_BLIND ) || IS_AFFECTED( victim, AFF_PERFECT_SENSES ) || saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		return;

	if ( number_range( 0, luck + vluck ) == 0 )
	{
		act( "Nie uda�o ci si� o�lepi� $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	duration = 4;
	if ( number_range( 0, luck ) > 15 ) ++duration;
	if ( number_range( 0, luck ) < 5 ) --duration;

	if ( number_range( 0, vluck ) > 15 ) --duration;
	if ( number_range( 0, vluck ) < 5 ) ++duration;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.bitvector = &AFF_BLIND;
	affect_to_char( victim, &af, NULL, TRUE );

	switch ( victim->sex )
	{
		case 0:
			send_to_char( "Zosta�o� o�lepione.\n\r", victim );
			act( "$n wygl�da na o�lepione.", victim, NULL, NULL, TO_ROOM );
			break;
		case 1:
			send_to_char( "Zosta�e� o�lepiony.\n\r", victim );
			act( "$n wygl�da na o�lepionego.", victim, NULL, NULL, TO_ROOM );
			break;
		default :
			send_to_char( "Zosta�a� o�lepiona.\n\r", victim );
			act( "$n wygl�da na o�lepion�.", victim, NULL, NULL, TO_ROOM );
			break;
	}
	return;
}

void spell_slow_poison( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA *af, *af_next;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( !is_affected( victim, gsn_poison ) )
	{
		if ( victim == ch )
			send_to_char( "Nie jeste� zatrut<&y/a/e>.\n\r", ch );
		else
		{
			switch ( victim->sex )
			{
				case 0:
					act( "$N nie jest chyba zatrute.", ch, NULL, victim, TO_CHAR );
					break;
				case 1:
					act( "$N nie jest chyba zatruty.", ch, NULL, victim, TO_CHAR );
					break;
				default:
					act( "$N nie jest chyba zatruta.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}
		return;
	}

	for ( af = victim->affected; af; af = af_next )
	{
		af_next = af->next;

		if ( af->type != gsn_poison )
			continue;

		if ( af->bitvector == &AFF_POISON )
		{
			if ( af->level == 0 )
			{
				if ( victim == ch )
					send_to_char( "Kr���ca w twojej krwi trucizna zosta�a ju� wcze�niej spowolniona.\n\r", ch );
				else
					act( "Kr���ca w krwi $Z trucizna zosta�a ju� spowolniona.", ch, NULL, victim, TO_CHAR );
				return;
			}
		}
	}

	if ( ch == victim )
	{
		if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
		{
			send_to_char( "Nie uda�o ci si� zwolni� dzia�ania trucizny kr���cej w twojej krwi.\n\r", ch );
			return;
		}
		else
		{
			if ( number_range( 0, luck + vluck + LUCK_BASE_MOD / 2 ) == 0 )
			{
				act( "Nie uda�o ci si� zwolni� dzia�ania trucizny kr���cej w krwi $Z.", ch, NULL, victim, TO_CHAR );
				return;
			}
		}

		for ( af = victim->affected; af; af = af_next )
		{
			af_next = af->next;

			if ( af->type != gsn_poison )
				continue;

			if ( af->bitvector == &AFF_POISON )
			{
				send_to_char( "Trucizna kr���ca w twojej krwi zosta�a spowolniona.\n\r", victim );
				if ( ch != victim )
					act( "Trucizna kr���cej w krwi $Z zosta�a spowolniona.", ch, NULL, victim, TO_CHAR );
				af->level = 0;
			}
			else
				affect_remove( victim, af );
		}
	}
	return;
}

void spell_lesser_restoration( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA *paf_next, *paf;
	sh_int gsn_to_strip = 0, roll = 0;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda�o ci si�.\n\r", ch );
		return;
	}
	
		/*if ( is_affected( victim, gsn_maze) && number_percent() < ( ch->level * 3 ))
		affect_strip (victim, gsn_maze); 

		if( is_affected( victim, gsn_confusion ) && number_percent() < ( ch->level * 3 ))
		affect_strip (victim, gsn_confusion);*/

	roll = number_range( 1, 4 );

	for ( paf = victim->affected; paf != NULL; paf = paf_next )
	{
		paf_next = paf->next;

		if ( paf->type <= 0 || paf->type >= MAX_SKILL )
			continue;

		if ( skill_table[ paf->type ].spell_fun == spell_null )
			continue;

		gsn_to_strip = 0;

		switch ( paf->location )
		{
			case APPLY_STR:
			case APPLY_DEX:
			case APPLY_INT:
			case APPLY_WIS:
			case APPLY_CON:
			case APPLY_CHA:
			case APPLY_LUC:
			case APPLY_SEX:
			case APPLY_CLASS:
			case APPLY_HEIGHT:
			case APPLY_WEIGHT:
			case APPLY_HIT:
				if ( paf->modifier >= 0 )
					continue;

				gsn_to_strip = paf->type;
				break;
			default: continue;
		}

		/* tylko spelle */
		if ( gsn_to_strip == 0 )
			continue;

		roll--;
		affect_strip( victim, gsn_to_strip );

		if ( skill_table[ gsn_to_strip ].msg_off )
		{
			send_to_char( skill_table[ gsn_to_strip ].msg_off, victim );
			send_to_char( "\n\r", victim );
		}

		if ( roll == 0 )
			break;
	}

	act( "Na $c sp�ywa oczyszczaj�ca si�a.", victim, NULL, NULL, TO_ROOM );
	if ( victim == ch ) send_to_char( "Czujesz, jak ogarnia ci� oczyszczaj�ca si�a.\n\r", victim );
	else
	{
		act( "Dzi�ki tobie na $C sp�ywa oczyszczaj�ca si�a.", ch, NULL, victim, TO_CHAR );
		act( "Za po�rednictwem $z sp�ywa na ciebie oczyszczaj�ca si�a.", ch, NULL, victim, TO_VICT );
	}

	return;
}

void spell_restoration( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA *paf_next, *paf;
	sh_int gsn_to_strip = 0;
	bool exit = FALSE;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( victim != ch )
			act( "Nie uda�o ci si� przywo�a� oczyszczaj�ca si�y na $Z.", ch, NULL, victim, TO_CHAR );
		else
			send_to_char( "Nie uda�o ci si� przywo�a� pot�nej, oczyszczaj�cej si�y.\n\r", ch );
		return;
	}
	
	/*	if ( is_affected( victim, gsn_hallucinations ) )
		affect_strip( victim, gsn_to_strip );
		
		if ( is_affected( victim, gsn_maze) )
		affect_strip (victim, gsn_maze); 
		
		if( is_affected( victim, gsn_confusion ) )
		affect_strip (victim, gsn_confusion);

		if( IS_AFFECTED( victim, AFF_LOOP ) )
		affect_strip (victim, AFF_LOOP);*/
	
		
	while ( TRUE )
	{
		for ( exit = TRUE, paf = victim->affected; paf != NULL; paf = paf_next )
		{
			paf_next = paf->next;

			if ( paf->type <= 0 || paf->type >= MAX_SKILL )
				continue;

			if ( skill_table[ paf->type ].spell_fun == spell_null )
				continue;

			gsn_to_strip = 0;

			switch ( paf->location )
			{
				case APPLY_STR:
				case APPLY_DEX:
				case APPLY_INT:
				case APPLY_WIS:
				case APPLY_CON:
				case APPLY_CHA:
				case APPLY_LUC:
				case APPLY_SEX:
				case APPLY_CLASS:
				case APPLY_HEIGHT:
				case APPLY_WEIGHT:
				case APPLY_HIT:
				case APPLY_EXP:
					if ( paf->modifier >= 0 )
						continue;
					gsn_to_strip = paf->type;
					break;
				default:
					continue;
			}

			/* tylko spelle */
			if ( gsn_to_strip == 0 )
				continue;

			if ( skill_table[ gsn_to_strip ].msg_off &&
			     skill_table[ gsn_to_strip ].msg_off != '\0' )
			{
				send_to_char( skill_table[ gsn_to_strip ].msg_off, victim );
				send_to_char( "\n\r", victim );
			}

			affect_strip( victim, gsn_to_strip );
			exit = FALSE;
			break;
		}

		if ( exit )
			break;
	}

	act( "Na $c sp�ywa pot�na, oczyszczaj�ca si�a.", victim, NULL, NULL, TO_ROOM );
	if ( victim == ch ) send_to_char( "Czujesz, jak ogarnia ci� pot�na, oczyszczaj�ca si�a.\n\r", victim );
	else
	{
		act( "Dzi�ki tobie na $C sp�ywa pot�na, oczyszczaj�ca si�a.", ch, NULL, victim, TO_CHAR );
		act( "Za po�rednictwem $z sp�ywa na ciebie pot�na, oczyszczaj�ca si�a.", ch, NULL, victim, TO_VICT );
	}

	if ( !IS_NPC( ch ) && ch->condition[ COND_SLEEPY ] > 10 )
		ch->condition[ COND_SLEEPY ] = 10;

	return;
}

void spell_divine_favor( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af, *paf,*paff;
	int duration, mod = level / 3, holy_dur_mod = 0, holy_dam_mod = 0;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( is_affected( victim, sn ) )
	{
		send_to_char( "Przywo�ane wcze�niej b�stwa s� ci nadal przychylne.\n\r", ch );
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda�o ci si� zdoby� przychylno�ci b�stwa.\n\r", ch );
		return;
	}
    if ( ( paff = affect_find( ch->affected, gsn_prayer_last )) != NULL)
    {
         if( paff->modifier < 24)
         {
             //bonusy tylko gdy modlimy sie regularnie

             if( (paf = affect_find( ch->affected, gsn_holy_pool)) != NULL)
             {
                 if (paf->modifier > 25 && number_percent() < URANGE(15, paf->modifier/2, 40))
                 {
                     holy_dur_mod += 2;
                     paf->modifier -= 1;
                 }

                 if (paf->modifier > 50 && number_percent() < 30)
                 {
                     holy_dam_mod += 2;
                     paf->modifier -= 1;
                 }

                 if (paf->modifier > 70 && number_percent() < 30)
                 {
                     holy_dam_mod += 2;
                     paf->modifier -= 2;
                 }
             }
         }
          if( (paf = affect_find( ch->affected, gsn_holy_pool)) != NULL)
          {
              if (-(paf->modifier) > 20 ) holy_dam_mod -= 2;
              if (-(paf->modifier) > 45 ) holy_dam_mod -= 2;
              if (-(paf->modifier) > 65 && -(paf->modifier) <= 85 )
              {
                send_to_char( "Pr�bujesz zdoby� przychylno�� swego boga, jednak odwraca si� on od twojej pro�by.\n\r", ch );
		        return;
              }
              if (-(paf->modifier) > 85 ) mod = -3;
          }


    }

	duration = 3 + level / 5 + holy_dur_mod;
    mod += holy_dam_mod;
	if ( number_range( 0, luck ) > 15 ) ++duration;
	if ( number_range( 0, luck ) < 3 ) --duration;
	if ( number_range( 0, get_curr_stat_deprecated( ch, STAT_CHA ) ) > 15 ) ++duration;
	if ( number_range( 0, get_curr_stat_deprecated( ch, STAT_CHA ) ) < 3 ) --duration;
	if ( number_range( 0, get_curr_stat_deprecated( ch, STAT_WIS ) ) > 15 ) ++duration;
	if ( number_range( 0, get_curr_stat_deprecated( ch, STAT_WIS ) ) < 3 ) --duration;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.location = APPLY_HITROLL;
	af.modifier = mod;
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, NULL, TRUE );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.location = APPLY_DAMROLL;
	af.modifier = mod;
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, NULL, TRUE );

	if(mod < 0)
	{
           send_to_char( "Co� dzieje si� nie tak, z otaczaj�c� ci� aur�.\n\r", victim );
           act( "$n otacza si� iskrz�c� po�wiat�, kt�ra emanuje dziwnym ch�odem.", victim, NULL, NULL, TO_ROOM );
           return;
    }else
    {
         if(holy_dam_mod > 0 || holy_dur_mod > 0) send_to_char( "Czujesz jak moc zakl�cia ro�nie.\n\r", victim );
         if(holy_dam_mod < 0 ) send_to_char( "Czujesz jak zgromadzona w tobie ciemno�� os�abia dzia�anie zakl�cia.\n\r", victim );

    }

	send_to_char( "Nagle otacza ci� iskrz�ca si�, bia�a po�wiata.\n\r", victim );
	act( "$n otacza si� iskrz�c�, bia�� po�wiat�.", victim, NULL, NULL, TO_ROOM );
	return;
}

void spell_divine_power( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int mod_str, mod_hp, duration;
	int luck = get_curr_stat( ch, STAT_LUC );
	
	if ( is_affected( victim, sn ) )
	{
		send_to_char( "Ju� jeste� obdarzon<&y/a/e> bosk� moc�.\n\r", ch );
		return;
	}
	
	if ( number_range( 1, luck + LUCK_BASE_MOD ) < 6 )
	{
		send_to_char( "Nie uda�o ci si� zdoby� boskiej mocy.\n\r", ch );
		return;
	}		

	mod_str = UMAX( 0, 24 - get_curr_stat_deprecated( victim, STAT_STR ) );
	
	if ( EXT_IS_SET( ch->act, ACT_BOSS  ) && ch->level >= 39 )
	mod_hp = number_range( 30, 45 ) * get_max_hp( victim ) / 100;
	else if ( EXT_IS_SET( ch->act, ACT_BOSS  ) && ch->level <= 38 )
	mod_hp = UMIN( number_range( 30, 45 ) * get_max_hp( victim ) / 100,  750);
	else
	mod_hp = UMIN( number_range( 30, 45 ) * get_max_hp( victim ) / 100,  500);

		duration = 5 + level / 3; 
		if ( number_range( 1, luck ) > 60 ) ++duration;
		if ( number_range( 1, luck ) < 15 ) --duration;
		
		if ( mod_str > 0 )
		{
		af.where = TO_AFFECTS;
		af.type = sn;
		af.level = level;
		af.duration = UMAX( 1, duration ); af.rt_duration = 0;
		af.location = APPLY_STR;
		af.modifier = mod_str;
		af.bitvector = &AFF_NONE;
		affect_to_char( victim, &af, NULL, TRUE );
		}
		
		af.where = TO_AFFECTS;
		af.type = sn;
		af.level = level;
		af.duration = UMAX( 1, duration ); af.rt_duration = 0;
		af.location = APPLY_HIT;
		af.modifier = mod_hp;
		af.bitvector = &AFF_NONE;
		affect_to_char( victim, &af, NULL, TRUE );

		af.where = TO_AFFECTS;
		af.type = sn;
		af.level = level;
		af.duration = UMAX( 1, duration ); af.rt_duration = 0;
		af.location = APPLY_HITROLL;
		af.modifier = UMIN(level/4, 6);
		af.bitvector = &AFF_NONE;
		affect_to_char( victim, &af, NULL, TRUE );

		victim->hit += mod_hp;		
	
	if ( is_affected( victim, sn ) )
	{
		send_to_char( "Czujesz jak zaczyna rozpiera� ci� pot�na energia, przez chwile wydaje ci si�, �e twoje cia�o eksploduje.\n\r", victim );
		act( "$n zaczyna promieniowa� bia�ym �wiat�em.", victim, NULL, NULL, TO_ROOM );
	}
	else
		send_to_char( "Nie uda�o ci zdoby� boskiej mocy.\n\r", ch );

	return;
}

void spell_bane( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int duration;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( is_affected( victim, sn ) )
	{
		if ( ch == victim )
			send_to_char( "Ju� jeste� przekl�t<&y/a/e>.\n\r", ch );
		else
			switch ( victim->sex )
			{
				case 0:
					act( "$N jest ju� przekl�te.", ch, NULL, victim, TO_CHAR );
					break;
				case 1:
					act( "$N jest ju� przekl�ty.", ch, NULL, victim, TO_CHAR );
					break;
				default:
					act( "$N jest ju� przekl�ta.", ch, NULL, victim, TO_CHAR );
					break;
			}
		return;
	}

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) && number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		act( "Nie uda�o ci si� przekln�� $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	duration = 2 + level / 6;
	if ( number_range( 0, luck ) > 15 ) duration += 1;
	if ( number_range( 0, luck ) < 5 ) duration /= 2;

	if ( ch != victim )
		luck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( number_range( 0, luck ) > 15 ) duration--;
	if ( number_range( 0, luck ) < 5 ) duration++;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.location = APPLY_HITROLL;
	af.modifier = -1;
	af.bitvector = &AFF_NONE;

	affect_to_char( victim, &af, NULL, TRUE );

	af.location  = APPLY_SAVING_SPELL;
	af.modifier  = 1;
	affect_to_char( victim, &af, NULL, TRUE  );

	if ( ch != victim )
	{
		act( "$n okrywa $C aur� przekle�stwa.", ch, NULL, victim, TO_NOTVICT );
		act( "Okrywasz si� aur� przekle�stwa.", ch, NULL, victim, TO_VICT );
		act( "Okrywasz $C aur� przekle�stwa.", ch, NULL, victim, TO_CHAR );
	}
	else
	{
		act( "$n okrywa si� aur� przekle�stwa.", ch, NULL, NULL, TO_NOTVICT );
		act( "Okrywasz si� aur� przekle�stwa.", ch, NULL, NULL, TO_CHAR );
	}

	return;
}

//metalowe graty nagrzewaja si� i zadaja dam
//npc same zdejmuja graty, gracze recznie musza
//dam zalezny od czesci pancerza, to dalej, co runde walki spradzane
//nagrzanie moze zniszczyc metale slabo odporne na cieplo
//tutaj tylko nakladamy affect na obiekt[y]
void spell_heat_metal( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim;
	OBJ_DATA *obj, *objects[ 10 ];
	int items_heated = 0, items, i;
	AFFECT_DATA aff, *pAff, *af;

	if ( target == TARGET_OBJ )
	{
		obj = ( OBJ_DATA * ) vo;

		if ( !IS_SET( material_table[ obj->material ].flag, MAT_METAL ) ||
		     IS_OBJ_STAT( obj, ITEM_NOMAGIC ) ||
		     ( IS_OBJ_STAT( obj, ITEM_BLESS ) && number_percent() < 75 ) ||
		     ( IS_OBJ_STAT( obj, ITEM_MAGIC ) && number_percent() < 25 ) ||
		     ( obj->item_type != ITEM_ARMOR && obj->item_type != ITEM_WEAPON ) )
			return;

		if ( ( pAff = affect_find( obj->affected, gsn_heat_metal ) ) )
		{
			act( "Ten przedmiot jest ju� wystarczajaco rozgrzany.", ch, obj, NULL, TO_CHAR );
			return;
		}

		if ( ( pAff = affect_find( obj->affected, gsn_chill_metal ) ) )
		{
			act( "Szron pokrywajacy $p znika.", ch, obj, NULL, TO_CHAR );
			affect_remove_obj( obj, pAff );
			return;
		}

		aff.where	= TO_OBJECT;
		aff.type	= sn;
		aff.level	= level;
		aff.duration = 2 + level / 7.5; aff.rt_duration = 0;
		aff.location	= APPLY_NONE;
		aff.modifier	= 0;
		aff.bitvector	= &AFF_NONE;

		affect_to_obj( obj, &aff );

		act( "$p rozgrzewaj� si� do czerwono�ci.", ch, obj, NULL, TO_ALL );
		return;
	}

	victim = ( CHAR_DATA * ) vo;

	//rzut obronny - ale ofiara tylko
	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		return;

	for ( i = 0; i < 10; i++ )
		objects[ i ] = NULL;

	items_heated = number_range( 1, 1 + level / 10 );

	for ( items = 0, obj = victim->carrying; obj != NULL; obj = obj->next_content )
	{
		if ( obj->wear_loc == WEAR_NONE )
			continue;

		switch ( obj->wear_loc )
		{
			case WEAR_FEET:
			case WEAR_LEGS:
			case WEAR_BODY:
			case WEAR_HEAD:
			case WEAR_SHIELD:
			case WEAR_ARMS:
			case WEAR_HANDS:
			case WEAR_WIELD:
			case WEAR_SECOND: break;
			default: continue;
		}

		if ( !IS_SET( material_table[ obj->material ].flag, MAT_METAL ) )
			continue;

		if ( IS_OBJ_STAT( obj, ITEM_NOMAGIC ) )
			continue;

		if ( IS_OBJ_STAT( obj, ITEM_BLESS ) && number_percent() < 75 )
			continue;

		if ( IS_OBJ_STAT( obj, ITEM_MAGIC ) && number_percent() < 25 )
			continue;

		if ( obj->item_type != ITEM_ARMOR && obj->item_type != ITEM_WEAPON )
			continue;

		objects[ items++ ] = obj;

		if ( items >= 10 )
			break;
	}

	if ( items == 0 )
		return;

	//affect nakladany
	aff.where	= TO_OBJECT;
	aff.type	= sn;
	aff.level	= level;
	aff.duration = 2 + level / 7.5; aff.rt_duration = 0;
	aff.location	= APPLY_NONE;
	aff.modifier	= 0;
	aff.bitvector	= &AFF_NONE;

	for ( i = 0; i < items_heated; i++ )
	{
		obj = objects[ number_range( 0, items - 1 ) ];

		if ( affect_find( obj->affected, sn ) )
			continue;

		affect_to_obj( obj, &aff );

		switch ( obj->wear_loc )
		{
			case WEAR_FEET:
			case WEAR_LEGS:
			case WEAR_HANDS:
			case WEAR_ARMS:
				act( "$p rozgrzewaj� si� do czerwono�ci.", ch, obj, NULL, TO_ALL );
				break;
			default:
				act( "$p rozgrzewa si� do czerwono�ci.", ch, obj, NULL, TO_ALL );
				break;
		}

		if ( IS_NPC( victim ) )
		{
			if ( !IS_AFFECTED( victim, AFF_PARALYZE ) && victim->position > POS_SLEEPING )
			{
				if ( !IS_OBJ_STAT( obj, ITEM_NOREMOVE ) )
				{
					if ( obj->item_type == ITEM_ARMOR )
						act( "$n j�czy i w wielkim po�piechu zdejmuje $h!", victim, obj, NULL, TO_ROOM );
					else if ( obj->item_type == ITEM_WEAPON )
						act( "$p wypada $x z d�oni parz�c j�.", victim, obj, NULL, TO_ROOM );
					unequip_char( victim, obj );
				}
			}
			else if ( number_percent() > 85 )
			{
				for ( af = victim->affected; af != NULL; af = af->next )
				{
					if ( af->bitvector == &AFF_PARALYZE )
					{
						affect_strip( victim, af->type );
						if ( !IS_NPC( victim ) )
						{
							send_to_char( skill_table[ sn ].msg_off, victim );
							send_to_char( "\n\r", victim );
						}
					}
				}
				act( "Z powodu b�lu, $n odzyskuje kontrol� nad swoim cia�em.", victim, NULL, NULL, TO_ROOM );
			}
		}
		else if ( victim->position > POS_SLEEPING )
		{
			act( "$p pali twoj� sk�r�!", victim, obj, NULL, TO_CHAR );
			if ( number_percent() > 0 && IS_AFFECTED( victim, AFF_PARALYZE ) )
			{
				act( "Z powodu b�lu, $n odzyskuje kontrol� nad swoim cia�em.", victim, NULL, NULL, TO_ROOM );
				send_to_char( "Z powodu b�lu, odzyskujesz kontrol� nad swoim cia�em.\n\r", ch );
				EXT_REMOVE_BIT( victim->affected_by, AFF_PARALYZE );
			}
		}

		if ( items == 1 )
			break;
	}
}

void spell_chill_metal( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim;
	OBJ_DATA *obj, *objects[ 10 ];
	int items_heated = 0, items, i;
	AFFECT_DATA aff, *pAff;

	if ( target == TARGET_OBJ )
	{
		obj = ( OBJ_DATA * ) vo;

		if ( !IS_SET( material_table[ obj->material ].flag, MAT_METAL ) ||
		     IS_OBJ_STAT( obj, ITEM_NOMAGIC ) ||
		     ( IS_OBJ_STAT( obj, ITEM_BLESS ) && number_percent() < 75 ) ||
		     ( IS_OBJ_STAT( obj, ITEM_MAGIC ) && number_percent() < 25 ) ||
		     ( obj->item_type != ITEM_ARMOR && obj->item_type != ITEM_WEAPON ) )
			return;

		if ( ( pAff = affect_find( obj->affected, gsn_chill_metal ) ) )
		{
			act( "Ten przedmiot jest ju� lodowato zimny.", ch, obj, NULL, TO_CHAR );
			return;
		}

		if ( ( pAff = affect_find( obj->affected, gsn_heat_metal ) ) )
		{
			act( "$p przestaje swiecic gleboka czerwienia i wraca do normalnej temperatury.", ch, obj, NULL, TO_CHAR );
			affect_remove_obj( obj, pAff );
			return;
		}

		aff.where	= TO_OBJECT;
		aff.type	= sn;
		aff.level	= level;
		aff.duration = 2 + level / 7.5; aff.rt_duration = 0;
		aff.location	= APPLY_NONE;
		aff.modifier	= 0;
		aff.bitvector	= &AFF_NONE;

		affect_to_obj( obj, &aff );

		act( "$p pokrywa si� gruba warstwa szronu.", ch, obj, NULL, TO_ALL );
		return;
	}

	victim = ( CHAR_DATA * ) vo;

	//rzut obronny - ale ofiara tylko
	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		return;

	for ( i = 0; i < 10; i++ )
		objects[ i ] = NULL;

	items_heated = number_range( 1, 1 + level / 10 );

	for ( items = 0, obj = victim->carrying; obj != NULL; obj = obj->next_content )
	{
		if ( obj->wear_loc == WEAR_NONE )
			continue;

		switch ( obj->wear_loc )
		{
			case WEAR_FEET:
			case WEAR_LEGS:
			case WEAR_BODY:
			case WEAR_HEAD:
			case WEAR_SHIELD:
			case WEAR_ARMS:
			case WEAR_HANDS:
			case WEAR_WIELD:
			case WEAR_SECOND: break;
			default: continue;
		}

		if ( !IS_SET( material_table[ obj->material ].flag, MAT_METAL ) )
			continue;

		if ( IS_OBJ_STAT( obj, ITEM_NOMAGIC ) )
			continue;

		if ( IS_OBJ_STAT( obj, ITEM_MAGIC ) && number_percent() < 25 )
			continue;

		if ( obj->item_type != ITEM_ARMOR && obj->item_type != ITEM_WEAPON )
			continue;

		objects[ items++ ] = obj;

		if ( items >= 10 )
			break;
	}

	if ( items == 0 )
		return;

	//affect nakladany
	aff.where	= TO_OBJECT;
	aff.type	= sn;
	aff.level	= level;
	aff.duration = 2 + level / 7.5; aff.rt_duration = 0;
	aff.location	= APPLY_NONE;
	aff.modifier	= 0;
	aff.bitvector	= &AFF_NONE;

	for ( i = 0; i < items_heated; i++ )
	{
		obj = objects[ number_range( 0, items - 1 ) ];

		if ( affect_find( obj->affected, sn ) )
			continue;

		affect_to_obj( obj, &aff );

		switch ( obj->wear_loc )
		{
			case WEAR_FEET:
			case WEAR_LEGS:
			case WEAR_HANDS:
			case WEAR_ARMS:
				act( "$p pokrywaja si� gruba warstwa szrony.", ch, obj, NULL, TO_ALL );
				break;
			default:
				act( "$p pokrywa si� gruba warstwa szronu.", ch, obj, NULL, TO_ALL );
				break;
		}

		if ( IS_NPC( victim ) )
		{
			if ( obj->item_type == ITEM_ARMOR )
				act( "$n jeczy i w wielkim pospiechu zdejmuje $h!", victim, obj, NULL, TO_ROOM );
			else if ( obj->item_type == ITEM_WEAPON )
				act( "$p wypada $x z dloni.", victim, obj, NULL, TO_ROOM );

			if ( !IS_OBJ_STAT( obj, ITEM_NOREMOVE ) )
				unequip_char( victim, obj );
		}
		else
			act( "$p zamraza twoja skore!", victim, obj, NULL, TO_CHAR );

		if ( items == 1 )
			break;
	}
}

void spell_mass_healing( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * gch;
	int heal_value, members = 0, refresh_value;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda�o ci si� uzdrowi� swych przyjaci�.\n\r", ch );
		return;
	}

	heal_value = number_range( 70, 90 ) + level;
	if ( number_range( 0, luck ) < 3 ) heal_value /= 2;
	if ( number_range( 0, luck ) > 15 ) heal_value *= ( number_range( 100, 110 ) ) / 100;

	refresh_value = number_range( 25, 40 ) + level / 2;
	if ( number_range( 0, luck ) < 3 ) refresh_value /= 2;

	for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
	{
		if ( is_same_group( ch, gch ) )
		{
			members++;
			if ( ( ch->class == CLASS_CLERIC ) && ( gch->hit == get_max_hp( gch ) ) )
				members--;
		}
	}

	if ( members > 8 )
	{
		heal_value /= 3;
		refresh_value /= 3;
	}
	else if ( members > 6 )
	{
		heal_value /= 2;
		refresh_value /= 2;
	}
	else if ( members > 4 )
	{
		heal_value -= heal_value / 4;
		refresh_value -= refresh_value / 4;
	}

	for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
		if ( is_same_group( ch, gch ) )
		{
			heal_char( ch, gch, heal_value, FALSE );
			send_to_char( "Czujesz pote�n� energie wype�niaj�c� twoje cia�o.\n\r", gch );

			if ( gch->move != gch->max_move )
			{
				gch->move = UMIN( gch->move + refresh_value, gch->max_move );

				if ( gch->max_move == gch->move )
					send_to_char( "Zm�czenie ca�kowicie ust�puje.\n\r", gch );
				else
					send_to_char( "Czujesz jak zm�czenie powoli ust�puje.\n\r", gch );
			}
		}
}

void spell_luck( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int vnum_unluck = 292;

	if ( is_affected( victim, gsn_luck ) )
	{
		if ( victim == ch )
			send_to_char( "Nie mo�esz mie� wi�cej szcz�cia!\n\r", ch );
		else
			act( "$N nie mo�e mie� wi�cej szcz�cia.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( is_affected( victim, vnum_unluck ) )
	{
		act( "Otaczaj�ca $x {yzgni�o{x-{Gzielona{x aura nieszcz�cia zanika.", victim, NULL, NULL, TO_ROOM );
		send_to_char( "Otaczaj�ca ci� {yzgni�o{x-{Gzielona{x aura nieszcz�cia zanika.\n\r", victim );
		affect_strip( victim, vnum_unluck );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = gsn_luck;
	af.level = level;
	if ( ch->class == CLASS_PALADIN )
	{
		af.duration = level / 6; af.rt_duration = 0;
		af.modifier = URANGE( 1, level / 10, 3 );
	}
	else
	{
		af.duration = 1 + level / 4; af.rt_duration = 0;
		af.modifier = UMAX( 1, level / 10 );
	}
	af.location = APPLY_LUC;
	af.bitvector = &AFF_NONE;

	affect_to_char( victim, &af, NULL, TRUE );
	send_to_char( "Czujesz, �e szcz�cie zaczyna ci sprzyja�!\n\r", victim );
	act( "Dooko�a $z rozb�yska {Gzielono{x-{Ys�oneczna{x otoczka szcz�cia!", victim, NULL, NULL, TO_ROOM );
	return;
}

void spell_create_symbol( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj = NULL;
	int value;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	AFFECT_DATA af;

	if ( is_affected( ch, gsn_create_symbol ) )
	{
		send_to_char( "Dzisiaj bogowie nie wys�uchaj� twoich modlitw.\n\r", ch );
		return;
	}

	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nie uda�o ci si� przywo�a� magicznego symbolu.\n\r", ch );
		return;
	}

	if ( ch->alignment > 250 )
	{
		obj = create_object( get_obj_index( OBJ_VNUM_CREATE_SYMBOL_GOOD ), FALSE );
	}
	else if ( ch->alignment < 250 && ch->alignment > -250 )
	{
		obj = create_object( get_obj_index( OBJ_VNUM_CREATE_SYMBOL_NEUTRAL ), FALSE );
	}
	else if ( ch->alignment < -250 )
	{
		obj = create_object( get_obj_index( OBJ_VNUM_CREATE_SYMBOL_EVIL ), FALSE );
	}

	if ( !obj )
	{
		send_to_char( "W�a�ciwie to nic si� nie dzieje.\n\r", ch );
		bug( "Brak obiektu: OBJ_VNUM_CREATE_SYMBOL_*.", 0 );
		return;
	}

	value = dice( 2, 4 ) + level / 7.5;
	if ( number_range( 0, luck ) > 15 ) value += 2;
	if ( number_range( 0, luck ) < 5 ) value -= 3;
	obj->timer = URANGE( 3, value, 6 );

	value = 2 * level;
	if ( number_range( 0, luck ) > 15 ) value = ( value * 9 ) / 10;
	if ( number_range( 0, luck ) < 5 ) value = ( value * 115 ) / 100;
	obj->rent_cost = value;

	value = level / 7.5;
	if ( number_range( 0, luck ) > 15 ) ++value;
	if ( number_range( 0, luck ) < 5 ) --value;

	EXT_SET_BIT( obj->extra_flags, ITEM_GLOW );
	EXT_SET_BIT( obj->extra_flags, ITEM_MAGIC );

	switch ( UMAX( 0, value ) )
	{
		case 0:
			obj->value[ 1 ] = 2;
			obj->value[ 2 ] = 4;
			break;
		case 1:
			obj->value[ 1 ] = 3;
			obj->value[ 2 ] = 5;
			break;
		case 2:
			obj->value[ 1 ] = 5;
			obj->value[ 2 ] = 7;
			break;
		case 3:
			obj->value[ 1 ] = 7;
			obj->value[ 2 ] = 10;
			break;
		default:
			obj->value[ 1 ] = 10;
			obj->value[ 2 ] = 15;
			break;
	}

	obj_to_char( obj, ch );

	af.where = TO_AFFECTS;
	af.type = gsn_create_symbol;
	af.level = 0;
	af.duration = 48; af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_NONE;
	affect_to_char( ch, &af, NULL, FALSE );

	send_to_char( "W twoim inwentarzu pojawia si� symbol wiary.\n\r", ch );
	return;
}

/*
 * Zwi�ksza timer, zanim co� si� stanie trucizn�
 */
void spell_slow_rot( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * vobj;
	OBJ_DATA * obj = ( OBJ_DATA * ) vo;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int count = 0;
	int food = 0;

	if ( level < 15 && ( obj->item_type != ITEM_FOOD || obj->carried_by != ch ) )
	{
		send_to_char( "Czar mo�esz rzuci� na pojedyncz� sztuk� jedzenia znajduj�c� si� w twoim inwentarzu.\n\r", ch );
		return;
	}

	if ( level >= 15 && ( ( obj->item_type != ITEM_FOOD && obj->item_type != ITEM_CONTAINER ) || obj->carried_by != ch ) )
	{
		send_to_char( "Czar mo�esz rzuci� na pojedyncz� sztuk� jedzenia lub pojemnik zawieraj�cy jedzenie.\n\r", ch );
		return;
	}

	//pojedynczy food
	if ( obj->item_type == ITEM_FOOD )
	{
		if ( obj->value[ 3 ] )
		{
			act( "$p �mierdzi zgnilizn�, niczego nie da si� zrobi�.", ch, obj, NULL, TO_CHAR );
			return;
		}

		if ( obj->value[ 1 ] == 0 )
		{
			act( "Przecie� $p nie mo�e zgni�.", ch, obj, NULL, TO_CHAR );
			return;
		}

		if ( obj->value[ 1 ] > 7 * level )
		{
			act( "Nie dasz rady bardziej spowolni� gnicia $f.", ch, obj, NULL, TO_CHAR );
			return;
		}

		if ( number_range( 0, luck ) > 1 )
		{
			obj->value[ 1 ] = 7 + 7 * level;
			act( "Spowalniasz procesy gnilne zachodz�ce w $k.", ch, obj, NULL, TO_CHAR );
			act( "$n spowalnia procesy gnilne zachodz�ce w $k.", ch, obj, NULL, TO_ROOM );
		}
		else
			act( "Nie uda�o ci si� spowolni� proces�w gnilnych zachodz�cych w $k.", ch, obj, NULL, TO_CHAR );

		if ( number_range( 0, luck ) < 2 )
			obj->value[ 1 ] = UMAX( 1, obj->value[ 1 ] - 10 );

		return;
	}

	for ( vobj = obj->contains; vobj; vobj = vobj->next_content )
	{
		if ( vobj->item_type != ITEM_FOOD )
			continue;

		food++;

		if ( vobj->value[ 3 ] || vobj->value[ 1 ] == 0 ||
		     vobj->value[ 1 ] > 7 * level || number_range( 0, luck ) <= 3 )
			continue;

		count++;

		vobj->value[ 1 ] = 7 + 7 * level;

		if ( number_range( 0, luck ) < 4 )
			vobj->value[ 1 ] = UMAX( 1, vobj->value[ 1 ] - 10 );
	}

	if ( food == 0 )
		act( "W $k nie ma �adnego jedzenia.", ch, obj, NULL, TO_CHAR );
	else
	{
		if ( count == 0 )
			act( "Nie uda�o ci si� spowolni� proces�w gnilnych zachodz�cych w jedzeniu znajduj�cym si� w $k.", ch, obj, NULL, TO_CHAR );
		else
		{
			act( "Spowalniasz procesy gnilne zachodz�ce w jedzeniu znajduj�cym si� w $k.", ch, obj, NULL, TO_CHAR );
			act( "$n spowalnia procesy gnilne zachodz�ce w jedzeniu znajduj�cym si� w $k.", ch, obj, NULL, TO_ROOM );
		}
	}
	return;
}

/*
 * zmiania si�� jednej wybranej osoby z grupy, kosztem pozosta�ych
 */
void spell_champions_strength( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	CHAR_DATA *vch;
	int count = 0, count2 = 0, count3 = 0;

	if ( !is_same_group( ch, victim ) )
	{
		send_to_char( "To si� nie uda, musisz wybra� kogo� ze swojej grupy.\n\r", ch );
		return;
	}

	if ( is_affected( ch, sn ) )
	{
		send_to_char( "Przecie� dzielisz ju� z kim� swoj� si��.\n\r", ch );
		return;
	}

	if ( is_affected( victim, sn ) )
	{
		act( "Z $V ju� si� kto� podzieli� swoj� si��.", ch, NULL, victim, TO_CHAR );
		return;
	}

    for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
    {
        if (
                IS_NPC( vch ) ||
                !is_same_group( vch, ch ) ||
                is_affected( vch, sn ) ||
                check_improve_strenth ( ch, vch, FALSE )
           )
        {
            continue;
        }

        if ( vch != victim && !IS_NPC( vch ) )
        {
            count++;
        }
    }

	count = UMIN( count, level / 6 );

	count2 = count3 = 1;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level	= level;
	af.duration = 10 + level / 6; af.rt_duration = 0;
	af.location = APPLY_STR;
	af.modifier = -1;
	af.bitvector = &AFF_NONE;
	affect_to_char( ch, &af, NULL, TRUE );

	switch ( ch -> sex )
	{
		case 0:
			send_to_char( "Czujesz si� nieco s�absze.\n\r", ch );
			act( "$n wygl�da na nieco s�absze.", ch, NULL, NULL, TO_ROOM );
			break;
		case 1:
			send_to_char( "Czujesz si� nieco s�abszy.\n\r", ch );
			act( "$n wygl�da na nieco s�abszego.", ch, NULL, NULL, TO_ROOM );
			break;
		default:
			send_to_char( "Czujesz si� nieco s�absza.\n\r", ch );
			act( "$n wygl�da na nieco s�absz�.", ch, NULL, NULL, TO_ROOM );
			break;
	}

	for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
	{
		if ( IS_NPC( vch ) )
			continue;

		if ( !is_same_group( vch, ch ) )
			continue;

		if ( is_affected( vch, sn ) )
			continue;

		if ( vch != victim && count2 < count )
		{
			af.where = TO_AFFECTS;
			af.type = sn;
			af.level	= level;
			af.duration = 10 + level / 6; af.rt_duration = 0;
			af.location = APPLY_STR;
			af.modifier = -1;
			af.bitvector = &AFF_NONE;
			affect_to_char( vch, &af, NULL, TRUE );

			switch ( vch -> sex )
			{
				case 0:
					send_to_char( "Czujesz si� nieco s�absze.\n\r", vch );
					act( "$n wygl�da na nieco s�absze.", vch, NULL, NULL, TO_ROOM );
					break;
				case 1:
					send_to_char( "Czujesz si� nieco s�abszy.\n\r", vch );
					act( "$n wygl�da na nieco s�abszego.", vch, NULL, NULL, TO_ROOM );
					break;
				default:
					send_to_char( "Czujesz si� nieco s�absza.\n\r", vch );
					act( "$n wygl�da na nieco s�absz�.", vch, NULL, NULL, TO_ROOM );
					break;
			}
			count3++;
		}
		count2++;
	}

	if ( !( count3 > 0 ) )
		return;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level	= level;
	af.duration = 10 + level / 6; af.rt_duration = 0;
	af.location = APPLY_STR;
	af.modifier = ( count3 * 4 ) / 3;
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Czujesz zbieraj�c� si� w tobie si�� ca�ej grupy.\n\r", victim );

	switch ( count3 )
	{
		case 1:
			act( "$n puszy nieco swoje mi�nie.", victim, NULL, NULL, TO_ROOM );
			break;
		case 2:
			act( "$n napina swoje mi�nie.", victim, NULL, NULL, TO_ROOM );
			break;
		case 3:
			act( "$n mocno napina swoje mi�nie.", victim, NULL, NULL, TO_ROOM );
			break;
		case 4:
			act( "$n pot�nie napina swoje mi�nie.", victim, NULL, NULL, TO_ROOM );
			break;
		case 5:
			act( "$n niewyobra�alnie napina swoje mi�nie.", victim, NULL, NULL, TO_ROOM );
			break;
		default:
			act( "$n pokazuje swoje mi�nie.", victim, NULL, NULL, TO_ROOM );
			break;
	}

	return;
}

/* RT calm spell stops all fighting in the room */

void spell_calm( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * vch;
	int mlevel = 0;
	int count = 0;
	int high_level = 0;
	int chance;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( ch->position != POS_FIGHTING && !ch->fighting )
	{
		send_to_char( "Przecie� z nikim nie walczysz.\n\r", ch );
		return;
	}

	/* get sum of all mobile levels in the room */
	for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
	{
		if ( vch != ch && vch->position == POS_FIGHTING )
		{
			count++;
			if ( IS_NPC( vch ) )
				mlevel += vch->level;
			else
				mlevel += vch->level / 2;
			high_level = UMAX( high_level, vch->level );
		}
	}

	/* compute chance of stopping combat */
	chance = UMAX( 1, 4 * level - high_level - 2 * count );

	if ( number_range( 0, luck ) > 15 ) chance = ( 11 * chance ) / 10;
	if ( number_range( 0, luck ) < 5 ) chance = chance / 2;

	chance = UMAX( chance, mlevel + 1 );

	if ( IS_IMMORTAL( ch ) )     /* always works */
		mlevel = 0;

	if ( number_range( 0, chance ) >= mlevel )      /* hard to stop large fights */
	{
		for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
		{
			if ( !vch->fighting && vch->position != POS_FIGHTING )
				continue;

			if ( is_same_group( vch, ch ) )
			{
				if ( vch->fighting || vch->position == POS_FIGHTING )
					stop_fighting( vch, FALSE );
				continue;
			}

			if ( is_undead( vch ) )
			{
				act( "$N nic sobie nie robi z twoich wysi�k�w.", ch, NULL, vch, TO_CHAR );
				continue;
			}

			if ( IS_AFFECTED( vch, AFF_CALM ) )
			{
				send_to_char( "Tym razem to si� chyba nie uda.\n\r", ch );
				continue;
			}

			if ( IS_AFFECTED( vch, AFF_BERSERK ) || is_affected( vch, skill_lookup( "frenzy" ) ) )
			{
				print_char( ch, "Nie dasz rady uspokoi� %s.\n\r", vch->name2 );
				continue;
			}

			if ( vch->fighting || vch->position == POS_FIGHTING )
			{
				if ( !IS_NPC( vch ) )
					send_to_char( "Fala spokoju przep�ywa przez twoje cia�o.\n\r", vch );

				act( "Przez $c przep�ywa fala spokoju.", vch, NULL, NULL, TO_ROOM );

				af.where = TO_AFFECTS;

				af.type = sn;
				af.level = level;
				af.duration = level / 4; af.rt_duration = 0;
				af.bitvector = &AFF_CALM;
				af.modifier = -2;

				if ( !IS_NPC( vch ) )
					af.modifier = -5;

				af.location = APPLY_HITROLL;
				affect_to_char( vch, &af, NULL, TRUE );

				af.location = APPLY_DAMROLL;
				affect_to_char( vch, &af, NULL, TRUE );

				stop_hating( vch, vch->fighting, FALSE );
				stop_hunting( vch );

				vch->fighting = NULL;
				vch->position = POS_STANDING;

			}
		}
		stop_fighting( ch, FALSE );
	}
	else
		send_to_char( "Nie uda�o ci si� nikogo uspokoi�.\n\r", ch );
}

void spell_curse( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim;
	OBJ_DATA *obj;
	AFFECT_DATA af;

	/* deal with the object case first */
	if ( target == TARGET_OBJ )
	{
		obj = ( OBJ_DATA * ) vo;
		if ( IS_OBJ_STAT( obj, ITEM_EVIL ) )
		{
			act( "$p jest ju� wype�niony z�em.", ch, obj, NULL, TO_CHAR );
			return;
		}

		if ( IS_OBJ_STAT( obj, ITEM_BLESS ) )
		{
			AFFECT_DATA * paf;

			paf = affect_find( obj->affected, skill_lookup( "bless" ) );
			if ( !saves_dispel( level, paf != NULL ? paf->level :   /* obj->level*/1, 0 ) )
			{
				if ( paf != NULL )
					affect_remove_obj( obj, paf );
				act( "$p �wieci czerwon� aur�.", ch, obj, NULL, TO_ALL );
				EXT_REMOVE_BIT( obj->extra_flags, ITEM_BLESS );
				return;
			}
			else
			{
				act( "�wieta aura $f jest zbyt silna �eby j� pokona�.", ch, obj, NULL, TO_CHAR );
				return;
			}
		}

		af.where = TO_OBJECT;
		af.type = sn;
		af.level = level;
		af.duration = -1; af.rt_duration = 0; /* a co :)*/
		/*2 * level;*/
		af.location = APPLY_SAVING_SPELL;
		af.modifier = + 1;
		af.bitvector = &ITEM_EVIL;
		affect_to_obj( obj, &af );

		act( "$p �wieci przekl�t� aur�.", ch, obj, NULL, TO_ALL );
		return;
	}

	/* character curses */
	victim = ( CHAR_DATA * ) vo;

	if ( IS_AFFECTED( victim, AFF_CURSE ) || saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		return;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = -1; af.rt_duration = 0;
	/*    2*level;*/
	af.location = APPLY_HITROLL;
	af.modifier = -1 * ( level / 7.5 );
	af.bitvector = &AFF_CURSE;
	affect_to_char( victim, &af, NULL, TRUE );

	af.location = APPLY_SAVING_SPELL;
	af.modifier = level / 7.5;
	affect_to_char( victim, &af, NULL, TRUE );

	switch ( victim->sex )
	{
		case 0:
			send_to_char( "Czujesz si� nieczyste.\n\r", victim );
			break;
		case 1:
			send_to_char( "Czujesz si� nieczysty.\n\r", victim );
			break;
		default:
			send_to_char( "Czujesz si� nieczysta.\n\r", victim );
			break;
	}
	if ( ch != victim )
		act( "$N wygl�da do�� nieprzyjemnie.", ch, NULL, victim, TO_CHAR );
	return;
}

void spell_detect_poison( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj = ( OBJ_DATA * ) vo;
	if ( obj->item_type == ITEM_DRINK_CON || obj->item_type == ITEM_FOOD )
	{
		if ( obj->value[ 3 ] != 0 )
			send_to_char( "Czujesz tu zapach trucizny.\n\r", ch );
		else
			send_to_char( "To wygl�da normalnie.\n\r", ch );
	}
	else if ( obj->item_type == ITEM_WEAPON )
	{
		if ( IS_SET( obj->value[ 4 ], WEAPON_POISON ) )
			print_char( ch, "Na %s znajdujesz ciemne plamy i wiesz, �e ta bro� jest zatruta.\n\r", obj->name5 );
		else
			send_to_char( "To nie wygl�da na zatrute.\n\r", ch );
	}
	else
		send_to_char( "To nie wygl�da na zatrute.\n\r", ch );
	return;
}

/* RT recall spell is back */
void spell_word_of_recall( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	ROOM_INDEX_DATA *location;
	unsigned short int last_rent;

	if ( IS_NPC( victim ) )
		return;

	if ( ( last_rent = victim->pcdata->last_rent ) <= 0 )
		return;

	if ( ( location = get_room_index( last_rent ) ) == NULL )
		return;

	if ( EXT_IS_SET( victim->in_room->room_flags, ROOM_NO_RECALL ) || EXT_IS_SET( victim->in_room->room_flags, ROOM_NOMAGIC ) )
		return;

	if ( victim->fighting != NULL )
		stop_fighting( victim, TRUE );

	act( "$n znika.", victim, NULL, NULL, TO_ROOM );
	char_from_room( victim );
	char_to_room( victim, location );
	act( "$n pojawia si� tu.", victim, NULL, NULL, TO_ROOM );
	do_function( victim, &do_look, "auto" );

	return;
}

/* Uzdrowienie (skladnia: cast heal [kto])
 */

void spell_life_transfer( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck, dam, heal;

	if ( victim == ch )
	{
		send_to_char( "Nie dasz rady rzuci� tego zakl�cia na siebie.\n\r", ch );
		return;
	}

	if ( is_undead( victim ) || IS_SET( victim->form, FORM_CONSTRUCT ) )
	{
		act( "$n nie jest dobrym celem do przeniesienia swojego �ycia.", ch, victim, NULL, TO_CHAR );
		return;
	}

	if ( victim->hit == get_max_hp( victim ) )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "$N wygl�da na ca�kiem zdrowe.", ch, NULL, victim, TO_CHAR );
				break;
			case 1:
				act( "$N wygl�da na ca�kiem zdrowego.", ch, NULL, victim, TO_CHAR );
				break;
			default:
				act( "$N wygl�da na ca�kiem zdrow�.", ch, NULL, victim, TO_CHAR );
				break;
		}
		return;
	}

	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda�o ci si�.\n\r", ch );
		return;
	}

	if ( is_affected( ch, gsn_life_transfer ) )
	{
		send_to_char( "Na razie nie masz do�� si� witalnych.\n\r", ch );
		return;
	}

	// taki wz�r daje
	// 40% ran casterowi na 20 levelu
	// 26% ran casterowi na 32 levelu
	// 20% ran casterowi na 40 levelu
	heal = get_max_hp( victim ) - victim->hit;
	dam = 8 * heal / level;

	if ( dam > ch->hit )
	{
		heal = ch->hit * level / 8;
		heal_char( ch, victim, heal, FALSE );
	}
	else
	{
		victim->hit = get_max_hp( victim );
		send_to_char( "Twoje cia�o wype�nia lecznicze ciep�o, wszystkie twoje rany goj� si�.\n\r", victim );
		act( "Wszystkie rany $z goj� si�.", victim, NULL, NULL, TO_ROOM );
	}

	if ( is_affected( victim, gsn_feeblemind ) )
		affect_strip( victim, gsn_feeblemind );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level	= 50;
	af.duration = 6; af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_NONE;
	affect_to_char( ch, &af, NULL, TRUE );

	act( "Czujesz jak twoje �ycie przep�ywa do $Z.", ch, NULL, victim, TO_CHAR );
	spell_damage( ch, ch, dam, sn, DAM_HARM, FALSE );

	return;
}

void spell_mass_bless( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	CHAR_DATA *gch;
	int duration = 20, luck = get_curr_stat_deprecated( ch, STAT_LUC ), count = 0;
/*
	if ( ch->position == POS_FIGHTING || ch->fighting )
	{
		send_to_char( "Nie mo�esz si� skoncentrowa�.\n\r", ch );
		return;
	}
*/
	/* zale�no�� od szcz�cia */
	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda�o ci si� rzuci� zakl�cia.\n\r", ch );
		return;
	}

	/* bonus od szcz�cia */
	if ( number_range( 0, luck ) > 15 ) duration++;
	if ( number_range( 0, luck ) < 5 ) duration--;

	for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
	{
		if ( !is_same_group( gch, ch ) || is_affected( gch, 3 ) )
			continue;

		if ( gch->class == CLASS_BLACK_KNIGHT && IS_GOOD(ch) )
			continue;

		af.where = TO_AFFECTS;
		af.type = 3;
		af.level = level;
		af.duration = 2 + level / 7.5; af.rt_duration = 0;
		af.location = APPLY_HITROLL;
		af.modifier = 1 + ( level > 15 ? 1 : 0 );
		af.bitvector = &AFF_NONE;

		affect_to_char( gch, &af, NULL, TRUE );

		af.location = APPLY_SAVING_SPELL;
		af.modifier = - 1 - ( level > 15 ? 1 : 0 );
		affect_to_char( gch, &af, NULL, TRUE );

		act( "Na $c sp�ywa boskie b�ogos�awie�stwo.", gch, NULL, NULL, TO_ROOM );
		if ( gch == ch ) send_to_char( "Sam<&/a/o> siebie b�ogos�awisz.\n\r", ch );
		else act( "Za po�rednictwem $z sp�ywa na ciebie boskie b�ogos�awie�stwo.", ch, NULL, gch, TO_VICT );

		count++;
	}

	if ( count > 0 )
		send_to_char( "Uda�o ci si� sprowadzi� b�ogos�awie�stwo.\n\r", ch );
	else
		send_to_char( "Uda�o ci si� sprowadzi� b�ogos�awie�stwo, ale nikt go nie potrzebowa�.\n\r", ch );

	return;
}

void spell_mass_luck( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	CHAR_DATA *gch;
	int count = 0;
	int vnum_unluck = 292;
/*
	if ( ch->position == POS_FIGHTING || ch->fighting )
	{
		send_to_char( "Nie mo�esz si� skoncentrowa�.\n\r", ch );
		return;
	}
*/
	af.where = TO_AFFECTS;
	af.type = gsn_luck;
	af.level = level;
	af.duration = 1 + level / 5; af.rt_duration = 0;
	af.modifier = level / 10;
	af.location = APPLY_LUC;
	af.bitvector = &AFF_NONE;

	for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
	{
		if ( !is_same_group( gch, ch ) || is_affected( gch, gsn_luck ) )
			continue;

		count++;

		if ( is_affected( gch, vnum_unluck ) )
		{
			act( "Otaczaj�ca $x {yzgni�o{x-{Gzielona{x aura nieszcz�cia zanika.", gch, NULL, NULL, TO_ROOM );
			send_to_char( "Otaczaj�ca ci� {yzgni�o{x-{Gzielona{x aura nieszcz�cia zanika.\n\r", gch );
			affect_strip( gch, vnum_unluck );
			continue;
		}

		affect_to_char( gch, &af, NULL, TRUE );
		send_to_char( "Czujesz, �e szcz�cie zaczyna ci sprzyja�!\n\r", gch );
		act( "Dooko�a $z rozb�yska {Gzielono-{Ys�oneczna{x otoczka szcz�cia!", gch, NULL, NULL, TO_ROOM );
	}

    if ( count == 0 )
    {
        send_to_char( "Nie uda�o ci si� zwi�kszy� niczyjego szcz�cia.\n\r", ch );
    }
	return;
}

void spell_consecrate( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	if ( IS_EVIL( ch ) )
	{
		send_to_char( "Nie jeste� w stanie po�wi�ci� tego miejsca.\n\r", ch );
		return;
	}

	if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_CONSECRATE ) )
	{
		send_to_char( "To miejsce jest ju� po�wi�cone.\n\r", ch );
		return;
	}

	if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_DESECRATE ) )
	{
		remove_event( EVENT_DESECRATE_END, ch->in_room, NULL, 0 );
		EXT_REMOVE_BIT( ch->in_room->room_flags, ROOM_DESECRATE );
		send_to_char( "Wyp�dzasz si�y z�a z tego miejsca.\n\r", ch );
		return;
	}

	if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_MAGICDARK ) )
	{
		EXT_REMOVE_BIT( ch->in_room->room_flags, ROOM_MAGICDARK );
		if ( ch->in_room->people )
			act( "Mrok opuszcza to miejsce.", ch->in_room->people, NULL, NULL, TO_ALL );
		return;
	}

	EXT_SET_BIT( ch->in_room->room_flags, ROOM_CONSECRATE );
	act( "Czujesz, �e temu miejscu zaczynaj� sprzyja� si�y dobra.", ch, NULL, NULL, TO_ALL );
	create_event( EVENT_CONSECRATE_END, PULSE_TICK * ( 2 + level / 6 ), ch->in_room, NULL, 0 );
	return;
}

void spell_desecrate( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	if ( IS_GOOD( ch ) )
	{
		send_to_char( "Nie jeste� w stanie zbeszcze�ci� tego miejsca.\n\r", ch );
		return;
	}

	if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_DESECRATE ) )
	{
		send_to_char( "To miejsce jest ju� zbeszczeszone.\n\r", ch );
		return;
	}

	if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_CONSECRATE ) )
	{
		remove_event( EVENT_CONSECRATE_END, ch->in_room, NULL, 0 );
		EXT_REMOVE_BIT( ch->in_room->room_flags, ROOM_CONSECRATE );
		send_to_char( "Wyp�dzasz si�y dobra z tego miejsca.\n\r", ch );
		return;
	}

	EXT_SET_BIT( ch->in_room->room_flags, ROOM_DESECRATE );
	act( "Czujesz, �e temu miejscu zaczynaj� sprzyja� si�y z�a.", ch, NULL, NULL, TO_ALL );
	create_event( EVENT_DESECRATE_END, PULSE_TICK * ( 2 + level / 6 ), ch->in_room, NULL, 0 );
	return;
}

void spell_poison( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim;
	OBJ_DATA *obj;
	AFFECT_DATA af;
	int poison_level;

	if ( target == TARGET_OBJ )
	{
		obj = ( OBJ_DATA * ) vo;
		if ( obj->item_type == ITEM_FOOD || obj->item_type == ITEM_DRINK_CON )
		{
			if ( IS_OBJ_STAT( obj, ITEM_BLESS ) )
			{
				act( "Twoje zakl�cie nie zdo�a�o zatru� $f.", ch, obj, NULL, TO_CHAR );
				return;
			}
			obj->value[ 3 ] = 1;
			act( "$p pokrywa si� truj�cym nalotem.", ch, obj, NULL, TO_ALL );
			return;
		}

		if ( obj->item_type == ITEM_WEAPON )
		{
			if ( IS_WEAPON_STAT( obj, WEAPON_FLAMING )
			     || IS_WEAPON_STAT( obj, WEAPON_FROST )
			     || IS_WEAPON_STAT( obj, WEAPON_SHOCKING )
			     || IS_WEAPON_STAT( obj, WEAPON_TOXIC )
			     || IS_WEAPON_STAT( obj, WEAPON_RESONANT )
			     || IS_WEAPON_STAT( obj, WEAPON_SACRED )
			     || IS_WEAPON_STAT( obj, WEAPON_HOLY )
			     || IS_WEAPON_STAT( obj, WEAPON_FLAMING_BURST )
			     || IS_WEAPON_STAT( obj, WEAPON_FROST_BURST )
			     || IS_WEAPON_STAT( obj, WEAPON_TOXIC_BURST )
			     || IS_WEAPON_STAT( obj, WEAPON_SONIC_BURST )
			     || IS_WEAPON_STAT( obj, WEAPON_SHOCKING_BURST )
			     || IS_OBJ_STAT( obj, ITEM_BLESS )
				)
			{
				act( "Nie masz co pr�bowa� zatru� $f.", ch, obj, NULL, TO_CHAR );
				return;
			}
				
			if ( obj->value[ 0 ] == WEAPON_MACE ||
				 obj->value[ 0 ] == WEAPON_FLAIL ||
				 obj->value[ 0 ] == WEAPON_STAFF )
			{
			send_to_char( "Zatru� mo�esz tylko bro� sieczn� lub k�uj�c�.\n\r", ch );
			return;
			}

			if ( IS_WEAPON_STAT( obj, WEAPON_POISON ) )
			{
				act( "To ju� jest zatrute.", ch, obj, NULL, TO_CHAR );
				return;
			}
			//Raszer sciecie poisona wrzucanego na bron, bylo af.level = 1 + level/10
			poison_level = level / 10;
			if((poison_level > 2) && (number_range(0, 100) <50)) { poison_level = 2;}
			
			af.where	= TO_WEAPON;
			af.type	= sn;
			af.level	= poison_level;
			af.duration = level / 7.5; af.rt_duration = 0;
			af.location	= WEAPON_POISON;
			af.modifier	= 0;
			af.bitvector = &AFF_NONE;
			affect_to_obj( obj, &af );

			act( "$p pokrywa si� �mierciono�nym nalotem.", ch, obj, NULL, TO_ALL );
			return;
		}

		act( "Nie mo�esz zatru� $f.", ch, obj, NULL, TO_CHAR );
		return;
	}

	victim = ( CHAR_DATA * ) vo;

	if ( is_undead( victim ) || IS_SET( victim->form, FORM_CONSTRUCT ) )
	{
		act( "Nie dasz rady zatru� $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	strip_invis( ch, TRUE, TRUE );

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		act( "$n chwilowo lekko zielenieje.", victim, NULL, NULL, TO_ROOM );
		send_to_char( "Czujesz si� bardzo �le, na szcz�cie to szybko mija.\n\r", victim );
		return;
	}

	poison_to_char( victim, 1 + level / 10 );
	spell_damage( ch, victim, 0, gsn_poison, DAM_NONE , FALSE );
	send_to_char( "Czujesz si� bardzo �le.\n\r", victim );
	act( "$n wygl�da bardzo �le.", victim, NULL, NULL, TO_ROOM );
	return;
}

void spell_change_liquid( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj;
	char buf [ MAX_STRING_LENGTH ];

	if ( target == TARGET_OBJ )
	{
		obj = ( OBJ_DATA * ) vo;
		if ( obj->item_type == ITEM_DRINK_CON )
		{
			// PUSTEGO nie b�dziemy transmutowa�
			if ( obj->value[ 1 ] == 0 )
			{
				act( "Ten pojemnik jest pusty.", ch, NULL, NULL, TO_CHAR );
				return;
			}

			// ZATRUTEGO nie b�dziemy transmutowa�
			if ( obj->value[ 3 ] )
			{
				sprintf( buf, "Nie dasz rady przemieni� %s w %s.\n\r", liq_table[ obj->value[ 2 ] ].liq_name2, obj->name6 );
				send_to_char( buf, ch );
				return;
			}
			// zamieniamy wod� w wino
			if ( obj->value[ 2 ] == 0 )
			{
				act( "Zmieniasz wod� w $k w wino.", ch, obj, NULL, TO_CHAR );
				act( "$n zmienia wod� w $k w wino.", ch, obj, NULL, TO_ROOM );
				obj->value[ 2 ] = 2;
				return;
			}
			// zamieniamy wino w wod�
			if ( obj->value[ 2 ] == 2 || obj->value[ 2 ] == 30 || obj->value[ 2 ] == 33 )
			{
				act( "Zmieniasz wino w $k w wod�.", ch, obj, NULL, TO_CHAR );
				act( "$n zmienia wino w $k w wod�.", ch, obj, NULL, TO_ROOM );
				obj->value[ 2 ] = 0;
				return;
			}

			sprintf( buf, "Nie wiesz w co zmieni� %s.\n\r", liq_table[ obj->value[ 2 ] ].liq_name4 );
			send_to_char( buf, ch );
			return;
		}
		else
		{
			act( "$p nie jest pojemnikiem na ciecz.", ch, obj, NULL, TO_CHAR );
			return;
		}
	}
	else
	{
		act( "W czym chcesz zmieni� p�yn?", ch, NULL, NULL, TO_CHAR );
	}
}

void spell_ray_of_light( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	if ( IS_EVIL( ch ) )
	{
		send_to_char( "Nie dasz rady przywo�a� �wiat�o�ci.\n\r", ch );
		return;
	}
	if ( !EXT_IS_SET( ch->in_room->room_flags, ROOM_MAGICDARK ) )
	{
		send_to_char( "Nie wyczywasz tutaj magicznej ciemno�ci.\n\r", ch );
		return;
	}
	EXT_REMOVE_BIT( ch->in_room->room_flags, ROOM_MAGICDARK );
	if ( ch->in_room->people )
		act( "Mrok opuszcza to miejsce.", ch->in_room->people, NULL, NULL, TO_ALL );

	return;
}

void spell_holy_weapons( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj = NULL;
	OBJ_DATA *obj2 = NULL;
	AFFECT_DATA af, *paf;
	int value;
	int weap_type, weap_type2;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int cha = get_curr_stat_deprecated( ch, STAT_CHA );
	int cursed_weapons = 20387;

	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nie uda�o ci si� przywo�a� �wi�tych broni.\n\r", ch );
		return;
	}

	if ( is_affected ( ch, gsn_holy_weapons ) )
	{
		send_to_char( "Niedawno prosi�<&e�/a�/o�> ju� o pomoc swego boga, lepiej nie nadu�ywa� jego mi�osierdzia.\n\r", ch );
		return;
	}

	if ( get_eq_char( ch, WEAR_WIELD ) || get_eq_char( ch, WEAR_SECOND ) )
	{
		send_to_char( "Nosisz ju� jak�� bro�. Zdejmij j�.\n\r", ch );
		return;
	}

	if ( get_eq_char( ch, WEAR_LIGHT ) || get_eq_char( ch, WEAR_HOLD ) || get_eq_char( ch, WEAR_SHIELD ) || get_eq_char( ch, WEAR_INSTRUMENT ) )
	{
		send_to_char( "Twoje r�ce s� zaj�te, od�� wszystkie zb�dne przedmioty.\n\r", ch );
		return;
	}

	//bardzo zly paladyn
	if ( ( paf = affect_find( ch->affected, gsn_holy_pool )) != NULL )
	{
         if( paf->modifier <= -50 && paf->modifier > -100)
         {
                af.where = TO_AFFECTS;
             	af.type = sn;
              	af.level = level;
               	af.duration = UMAX( 1, 30 - ( ( ch->level - 24 ) * 3 ) ); af.rt_duration = 0;
               	af.location = APPLY_NONE;
               	af.modifier = 0;
              	af.bitvector = &AFF_NONE;
               	af.visible = FALSE;
             	affect_to_char( ch, &af, NULL, FALSE );
                send_to_char( "Twoje nieczyste sumienie sprawia, i� Porten odwraca si� od twojej pro�by.\n\r", ch );
		        return;

         }
         if( paf->modifier <= -100)
         {
             weap_type = cursed_weapons;
             obj = create_object( get_obj_index( weap_type ), FALSE );
	         obj2 = create_object( get_obj_index( weap_type ), FALSE );
	         if ( !obj )
		     return;
	         if ( !obj2 )
		     return;
		     obj->timer = 10;
	         obj2->timer = 10;
             obj_to_char( obj, ch );
	         equip_char( ch, obj, WEAR_WIELD, TRUE );
          	 obj_to_char( obj2, ch );
	         equip_char( ch, obj2, WEAR_SECOND, TRUE );
	         af.where = TO_AFFECTS;
             af.type = sn;
             af.level = level;
	         af.duration = UMAX( 1, 30 - ( ( ch->level - 24 ) * 3 ) ); af.rt_duration = 0;
	         af.location = APPLY_NONE;
	         af.modifier = 0;
	         af.bitvector = &AFF_NONE;
	         af.visible = FALSE;
	         affect_to_char( ch, &af, NULL, FALSE );

	         act( "Gdy $n ko�czy modlitw� powietrze przeszywa niepokoj�ca aura, a w jego d�oniach pojawiaj� si� dwie przekl�te bronie.", ch, NULL, NULL, TO_ROOM );
	         send_to_char( "Gdy ko�czysz modlitw� przestrze� dooko�a ciebie przeszywa niepokoj�ca aura, a twoich d�oniach pojawiaj� si� otoczone krwist� aur� baty.\n\r", ch );
	         return;



         }

    }

  if ( get_skill(ch,gsn_mace_mastery) > 0 )
		weap_type = OBJ_VNUM_HOLY_HAMMER;
  else if ( get_skill(ch,gsn_sword_mastery) > 0 )
		weap_type = OBJ_VNUM_HOLY_SWORD;
	else if ( get_skill(ch,gsn_flail_mastery) > 0 )
		weap_type = OBJ_VNUM_HOLY_FLAIL;
	else
	{
		if ( get_skill( ch, gsn_mace ) >= get_skill( ch, gsn_flail )
			&& get_skill( ch, gsn_mace ) >= get_skill( ch, gsn_sword ) )
				weap_type = OBJ_VNUM_HOLY_HAMMER;
		else if ( get_skill( ch, gsn_sword ) >= get_skill( ch, gsn_flail ) )
				weap_type = OBJ_VNUM_HOLY_SWORD;
		else
				weap_type = OBJ_VNUM_HOLY_FLAIL;
	}

  weap_type2 = weap_type;

  value = number_range( 0, luck + 3*cha );

  switch ( value )
  {
  	case 0:
  	case 1:
  	case 2:
  	case 3:
  	case 4:
  	case 5:
		weap_type = ( number_range(0,2) != 0 ? ( number_range(0,1) == 0 ? OBJ_VNUM_HOLY_HAMMER : OBJ_VNUM_HOLY_FLAIL ) : OBJ_VNUM_HOLY_SWORD );
		weap_type2 = ( number_range(0,2) != 0 ? ( number_range(0,1) == 0 ? OBJ_VNUM_HOLY_HAMMER : OBJ_VNUM_HOLY_FLAIL ) : OBJ_VNUM_HOLY_SWORD );
		break;
  	default:
  		break;
  }

	obj = create_object( get_obj_index( weap_type ), FALSE );
	obj2 = create_object( get_obj_index( weap_type2 ), FALSE );

	if ( !obj )
		return;
	if ( !obj2 )
		return;

	value = dice( 2, 4 ) + level / 7.5;
	if ( number_range( 0, luck ) > 15 ) value += 1;
	if ( number_range( 0, luck ) < 5 ) value -= 1;
	obj->timer = UMAX( 3, value );
	obj2->timer = UMAX( 3, value );

	value = 2 * level;
	if ( number_range( 0, luck ) > 15 ) value = ( value * 9 ) / 10;
	if ( number_range( 0, luck ) < 5 ) value = ( value * 115 ) / 100;
	obj->rent_cost = value;
	obj2->rent_cost = value;

	value = level / 7.5;
	 //bonus dla dobrego paladyna
  if (paf != NULL)
  {
          if( paf->modifier > 50 && number_percent() > paf->modifier/2)
          {
              value++;
              paf->modifier -= 15;
              send_to_char( "Czujesz jak zgromadzona w tobie �wi�ta moc zaczyna pulsowa�.\n\r", ch );

          }

          if( paf->modifier > 70 && number_percent() > paf->modifier/4)
          {
              value++;
              paf->modifier -= 15;
              send_to_char( "Czujesz jak otacza ci� �aska twego boga.\n\r", ch );

          }

  }
	if ( number_range( 0, luck ) > 15 ) ++value;
	if ( number_range( 0, luck ) < 5 ) --value;

	EXT_SET_BIT( obj->extra_flags, ITEM_GLOW );
	EXT_SET_BIT( obj->extra_flags, ITEM_MAGIC );
	EXT_SET_BIT( obj2->extra_flags, ITEM_GLOW );
	EXT_SET_BIT( obj2->extra_flags, ITEM_MAGIC );

	switch ( UMAX( 0, value ) )
	{
		case 0:
			obj->value[ 1 ] = 1;
			obj->value[ 2 ] = 5;
			obj->value[ 5 ] = 1;
			obj->value[ 6 ] = 1;
			obj2->value[ 1 ] = 1;
			obj2->value[ 2 ] = 5;
			obj2->value[ 5 ] = 1;
			obj2->value[ 6 ] = 1;
			break;
		case 1:
			obj->value[ 1 ] = 1;
			obj->value[ 2 ] = 6;
			obj->value[ 5 ] = 2;
			obj->value[ 6 ] = 2;
			obj2->value[ 1 ] = 1;
			obj2->value[ 2 ] = 6;
			obj2->value[ 5 ] = 2;
			obj2->value[ 6 ] = 2;
			break;
		case 2:
			obj->value[ 1 ] = 2;
			obj->value[ 2 ] = 3;
			obj->value[ 5 ] = 3;
			obj->value[ 6 ] = 3;
			obj2->value[ 1 ] = 2;
			obj2->value[ 2 ] = 3;
			obj2->value[ 5 ] = 3;
			obj2->value[ 6 ] = 3;
			break;
		case 3:
			obj->value[1] = 2;
			obj->value[2] = 4;
			obj->value[5] = 4;
			obj->value[6] = 4;
			obj2->value[1] = 2;
			obj2->value[2] = 4;
			obj2->value[5] = 4;
			obj2->value[6] = 4;
			break;
		default:
			obj->value[1] = 2;
			obj->value[2] = 5;
			obj->value[5] = 4;
			obj->value[6] = 4;
			obj2->value[1] = 2;
			obj2->value[2] = 5;
			obj2->value[5] = 4;
			obj2->value[6] = 4;
			break;
	}

	obj_to_char( obj, ch );
	equip_char( ch, obj, WEAR_WIELD, TRUE );
	obj_to_char( obj2, ch );
	equip_char( ch, obj2, WEAR_SECOND, TRUE );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 1, 30 - ( ( ch->level - 24 ) * 3 ) ); af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_NONE;
	af.visible = FALSE;
	affect_to_char( ch, &af, NULL, FALSE );

	act( "Gdy $n ko�czy modlitw� wraz z szaro-niebieskim �wiat�em w $s d�oniach pojawiaj� si� dwie �wi�te bronie.", ch, NULL, NULL, TO_ROOM );
	send_to_char( "Gdy ko�czysz modlitw� z delikatnym rozb�yskiem �wiat�a w twoich d�oniach pojawiaj� si� dwie �wi�te bronie.\n\r", ch );
	return;
}

/* strach. cast fear <kto>
   jak nie ma brave_cloak'a i nie wyjdzie rzut obronny, to gosc
   dostaje affecta. nie moze atakowac(bo si� boi :) i ucieka z walki
*/
void spell_fear( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA * victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;
    int luck, vluck;

    if ( victim == ch )
    {
        send_to_char( "Ojej, jak tu strasznie!\n\r", ch );
        return;
    }

    /**
     * w przypadku bycia pod wplywem brave cloak, dajemy szanse na jego zdjecie
     */
    if ( is_affected(victim, gsn_brave_cloak ) )
    {
        if ( number_percent() > 50 && check_dispel( level, victim, gsn_brave_cloak ) )
        {
            send_to_char( "Chroni�cy ci� p�aszcz odwagi znika.\n\r", ch );
            act( "Chroni�cy $z p�aszcz odwagi znika.", victim, NULL, NULL, TO_ROOM );
            affect_strip( victim, gsn_brave_cloak );
            if ( IS_AFFECTED( victim, AFF_BRAVE_CLOAK ) )
            {
                EXT_REMOVE_BIT( victim->affected_by, AFF_BRAVE_CLOAK );
            }
        }
    }

    if ( is_affected(victim, gsn_brave_cloak ) || IS_AFFECTED( victim, AFF_BRAVE_CLOAK ) || is_undead( victim ) || IS_SET( victim->form, FORM_CONSTRUCT ))
    {
        switch ( victim->sex )
        {
            case SEX_MALE:
                act( "$N jest odporny na strach.\n\r", ch, NULL, victim, TO_CHAR );
                break;
            case SEX_FEMALE:
                act( "$N jest odporna na strach.\n\r", ch, NULL, victim, TO_CHAR );
                break;
            case SEX_NEUTRAL:
            default:
                act( "$N jest odporne na strach.\n\r", ch, NULL, victim, TO_CHAR );
                break;
        }
        return;
    }

    if ( IS_AFFECTED( victim, AFF_FEAR ) )
    {
        switch ( victim->sex )
        {
            case SEX_MALE:
                act( "$N jest ju� wystarcz�jaco przera�ony.\n\r", ch, NULL, victim, TO_CHAR );
                break;
            case SEX_FEMALE:
                act( "$N jest ju� wystarcz�jaco przera�ona.\n\r", ch, NULL, victim, TO_CHAR );
                break;
            case SEX_NEUTRAL:
            default:
                act( "$N jest ju� wystarcz�jaco przera�one.\n\r", ch, NULL, victim, TO_CHAR );
                break;
        }
        return;
    }

    /* luck */

    luck = get_curr_stat_deprecated( ch, STAT_LUC );
    vluck = get_curr_stat_deprecated( victim, STAT_LUC );

    if ( number_range( 0, luck + vluck ) == 0 )
    {
        act( "Nie uda�o ci si� przestraszy� $Z.", ch, NULL, victim, TO_CHAR );
        return;
    }

    /* odpornosc? */
    if ( victim->resists[ RESIST_FEAR ] > 0 && number_percent() < victim->resists[ RESIST_FEAR ] )
    {
        return;
    }

    /* nie ma straszenia starszych :)*/
    if ( victim->level > level + UMAX( 1, level / 10 ) )
    {
        return;
    }

    if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
    {
        return;
    }

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = 2; af.rt_duration = 0;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = &AFF_FEAR;
    affect_to_char( victim, &af, NULL, TRUE );

    send_to_char( "Zaczynasz odczuwa� paniczny strach.\n\r", victim );
    act( "$n panikuje ze strachu.", victim, NULL, NULL, TO_ROOM );

    if ( victim->fighting )
    {
        interpret( victim, "flee" );
        interpret( victim, "flee" );
        interpret( victim, "flee" );
    }

    if ( IS_NPC( victim ) )
    {
        start_fearing( victim, ch );
    }

    return;
}

void spell_healing_ring( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * gch;
	int heal_value, members = 0, refresh_value;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda�o ci si� uzdrowi� przyjaci�.\n\r", ch );
		return;
	}

	heal_value = number_range( 40, 50 ) + level / 2;
	if ( number_range( 0, luck ) < 3 ) heal_value /= 2;
	if ( number_range( 0, luck ) > 15 ) heal_value *= ( number_range( 100, 110 ) ) / 100;

	//refreshowac bedzie tylko healing ring rzucony przez druida
	refresh_value = number_range( 10, 25 ) + level / 3;
	if ( number_range( 0, luck ) < 3 ) refresh_value /= 2;

	for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
	{
		if ( is_same_group( ch, gch ) )
		{
			members++;

			if ( ( ch->class == CLASS_CLERIC ) && ( gch->hit == get_max_hp( gch ) ) )
				members--;
		}
	}

	if ( members > 8 )
	{
		heal_value /= 3;
		refresh_value /= 3;
	}
	else if ( members > 6 )
	{
		heal_value /= 2;
		refresh_value /= 2;
	}
	else if ( members > 4 )
	{
		heal_value -= heal_value / 4;
		refresh_value -= refresh_value / 4;
	}

	for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
		if ( is_same_group( ch, gch ) )
		{
			heal_char( ch, gch, heal_value, FALSE );
			send_to_char( "Czujesz przyjemne mrowienie sk�ry.\n\r", gch );

			if ( ch->class == CLASS_DRUID )
			{
				if ( gch->move != gch->max_move )
				{
					gch->move = UMIN( gch->move + refresh_value, gch->max_move );

					if ( gch->max_move == gch->move )
						send_to_char( "Zm�czenie ca�kowicie ust�puje.\n\r", gch );
					else
						send_to_char( "Czujesz jak zm�czenie powoli ust�puje.\n\r", gch );
				}
			}
		}
}

void spell_divine_shield( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af, *paf, *paff;
	bool ok = FALSE;
	int holy_dur_mod = 0, holy_lev_mod = 0;

	if ( IS_EVIL( victim ) )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "$N nie jest godne opieki bog�w.", ch, NULL, victim, TO_CHAR );
				break;
			case 1:
				act( "$N nie jest godny opieki bog�w.", ch, NULL, victim, TO_CHAR );
				break;
			default :
				act( "$N nie jest godna opieki bog�w.", ch, NULL, victim, TO_CHAR );
				break;
		}
		return;
	}

	if ( IS_AFFECTED( victim, AFF_RESIST_ACID ) ||
	     IS_AFFECTED( victim, AFF_RESIST_COLD ) ||
	     IS_AFFECTED( victim, AFF_RESIST_FIRE ) ||
	     IS_AFFECTED( victim, AFF_RESIST_LIGHTNING ) ||
	     IS_AFFECTED( victim, AFF_RESIST_FEAR ) ||
	     IS_AFFECTED( victim, AFF_RESIST_NEGATIVE ) )
	{
		send_to_char( "Nie uda�o ci si� wezwa� tarczy bog�w.\n\r", ch );
		return;
	}

	   if ( ( paff = affect_find( ch->affected, gsn_prayer_last )) != NULL)
    {
         if( paff->modifier < 24)
         {
             //bonusy tylko gdy modlimy sie regularnie
             if( (paf = affect_find( ch->affected, gsn_holy_pool)) != NULL)
             {
                 if (paf->modifier > 25 && number_percent() < URANGE(15, paf->modifier/2, 30))
                 {
                     holy_dur_mod += 2;
                     paf->modifier -= 2;
                 }

                 if (paf->modifier > 50 && number_percent() < 30)
                 {
                     holy_lev_mod += 5;
                     paf->modifier -= 2;
                 }

                 if (paf->modifier > 70 && number_percent() < 30)
                 {
                     holy_lev_mod += 10;
                     paf->modifier -= 5;
                 }
             }
         }
          if( (paf = affect_find( ch->affected, gsn_holy_pool)) != NULL)
          {
              if (-(paf->modifier) > 30 ) holy_lev_mod -= 5;
              if (-(paf->modifier) > 75 )
              {
                send_to_char( "Nie jeste� godny by prosi� bog�w o pomoc.\n\r", ch );
		        return;
              }

          }


    }

	af.where = TO_AFFECTS;
	af.type = sn;
	af.location = APPLY_RESIST;

	if ( !IS_AFFECTED( victim, AFF_RESIST_ACID ) && ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 3 ) )
	{
		af.level = 10 + holy_lev_mod + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 10 ) ? 1 : 0 ) + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_WIS ) ) > 15 ) ? 2 : 0 );
		af.duration = holy_dur_mod + URANGE( 1, level / 6, 4 ) + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 10 ) ? 1 : 0 ); af.rt_duration = 0;
		af.modifier = RESIST_ACID;
		af.bitvector = &AFF_RESIST_ACID;
		affect_to_char( victim, &af, NULL, TRUE );
		ok = TRUE;
	}

	if ( !IS_AFFECTED( victim, AFF_RESIST_COLD ) && ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 3 ) )
	{
		af.level = 10 + holy_lev_mod + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 10 ) ? 1 : 0 ) + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_WIS ) ) > 15 ) ? 2 : 0 );
		af.duration = holy_dur_mod + URANGE( 1, level / 6, 4 ) + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 10 ) ? 1 : 0 ); af.rt_duration = 0;
		af.modifier = RESIST_COLD;
		af.bitvector = &AFF_RESIST_COLD;
		affect_to_char( victim, &af, NULL, TRUE );
		ok = TRUE;
	}

	if ( !IS_AFFECTED( victim, AFF_RESIST_FIRE ) && ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 3 ) )
	{
		af.level = 10 + holy_lev_mod + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 10 ) ? 1 : 0 ) + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_WIS ) ) > 15 ) ? 2 : 0 );
		af.duration = holy_dur_mod + URANGE( 1, level / 6, 4 ) + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 10 ) ? 1 : 0 ); af.rt_duration = 0;
		af.modifier = RESIST_FIRE;
		af.bitvector = &AFF_RESIST_FIRE;
		affect_to_char( victim, &af, NULL, TRUE );
		ok = TRUE;
	}

	if ( !IS_AFFECTED( victim, AFF_RESIST_LIGHTNING ) && ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 3 ) )
	{
		af.level = 10 + holy_lev_mod + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 10 ) ? 1 : 0 ) + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_WIS ) ) > 15 ) ? 2 : 0 );
		af.duration = holy_dur_mod + URANGE( 1, level / 6, 4 ) + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 10 ) ? 1 : 0 ); af.rt_duration = 0;
		af.modifier = RESIST_ELECTRICITY;
		af.bitvector = &AFF_RESIST_LIGHTNING;
		affect_to_char( victim, &af, NULL, TRUE );
		ok = TRUE;
	}

	if ( !IS_AFFECTED( victim, AFF_RESIST_FEAR ) && ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 3 ) )
	{
		af.level = 10 + holy_lev_mod + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 10 ) ? 1 : 0 ) + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_WIS ) ) > 15 ) ? 2 : 0 );
		af.duration = holy_dur_mod + URANGE( 1, level / 6, 4 ) + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 10 ) ? 1 : 0 ); af.rt_duration = 0;
		af.modifier = RESIST_FEAR;
		af.bitvector = &AFF_RESIST_FEAR;
		affect_to_char( victim, &af, NULL, TRUE );
		ok = TRUE;
	}

	if ( !IS_AFFECTED( victim, AFF_RESIST_NEGATIVE ) && ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 2 ) )
	{
		af.level = 20 + level + holy_lev_mod + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 10 ) ? 1 : 0 ) + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_WIS ) ) > 15 ) ? 2 : 0 );
		af.duration = holy_dur_mod + URANGE( 2, level / 6, 5 ) + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 10 ) ? 1 : 0 ); af.rt_duration = 0;
		af.modifier = RESIST_NEGATIVE;
		af.bitvector = &AFF_RESIST_NEGATIVE;
		affect_to_char( victim, &af, NULL, TRUE );
		ok = TRUE;
	}

	if ( ok )
	{
        if (holy_lev_mod > 0 || holy_dur_mod > 0) send_to_char( "Czujesz jak zgromadzona w tobie �wi�ta moc uwalnia si�, wzmacniaj�c bosk� tarcz�.\n\r", ch );
        if (holy_lev_mod < 0 ) send_to_char( "Czujesz jak cz�� zgromadzonej energi ucieka ci bezpowrotnie, a boska tarcza staje si� mniej stabilna.\n\r", ch );

        if ( ch == victim )
		{
			act( "$n otacza si� ochronn�, bosk� tarcz�.", ch, NULL, victim, TO_ROOM );
			send_to_char( "Otaczasz si� ochronn�, bosk� tarcz�.\n\r", victim );
		}
		else
		{
			act( "$n otacza $C ochronn�, bosk� tarcz�.", ch, NULL, victim, TO_NOTVICT );
			act( "$n otacza ci� ochronn�, bosk� tarcz�.", ch, NULL, victim, TO_VICT );
			act( "Otaczasz ochronn�, bosk� tacz� $C.", ch, NULL, victim, TO_CHAR );
		}
	}
	else
	{
		send_to_char( "Nie uda�o ci si�, a energia zakl�cia zosta�a rozproszona.\n\r", ch );
	}
	return;
}


void spell_lore_undead( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ),mod = 5 + number_range(1,5);
	char buf[ MAX_STRING_LENGTH ];

	if (IS_NPC(ch)) return;

	if ( !IS_NPC(victim) )
	{
		act( "$N nie wygl�da na martwiaka.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( !is_undead(victim) )
	{
		act( "$N nie wygl�da na martwiaka.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( victim->fighting )
	{
		act( "$N jest w trakcie walki, wi�c nie za bardzo dasz rad� przy�o�y� r�k�.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "W ostatniej chwili rozpraszasz swoj� uwag�.\n\r", ch );
		return;
	}

	if (ch->class == CLASS_PALADIN)
	{
		// paladyni z racji nienawi�ci do martwiak�w maj� mniejsze penalty
		mod /= 2;
	}

	if (victim->level - UMAX(3,mod) > ch->level)
	{
		act( "Nie dasz rady pozna� w�a�ciwo�ci $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	sprintf( buf, "{r%s{x\n\r", victim->name );
	send_to_char( buf, ch );

	sprintf( buf, "Rasa: %s.\n\r",race_table[ GET_RACE( victim ) ].name);
	send_to_char( buf, ch );

	switch (victim->sex)
	{
		case SEX_MALE:
			send_to_char( "P�e�: m�czyzna.\n\r", ch );
			break;
		case SEX_FEMALE:
			send_to_char( "P�e�: kobieta.\n\r", ch );
			break;
		default:
			break;
	}

	sprintf( buf, "Wzrost: %d cm.\n\rWaga: %d.%d kg.\n\r", victim->height, victim->weight / 10, victim->weight % 10 );
	send_to_char( buf, ch );

	send_to_char("Wielko��: ", ch);
	switch (victim->size)
	{
		case 0:
			send_to_char("niewielka", ch);
			break;
		case 1:
			send_to_char("ma�a", ch);
			break;
		case 2:
			send_to_char("�rednia", ch);
			break;
		case 3:
			send_to_char("wi�ksza", ch);
			break;
		case 4:
			send_to_char("ogromna", ch);
			break;
		case 5:
			send_to_char("gigantyczna", ch);
			break;
	}
	send_to_char( ".\n\r", ch );

	sprintf( buf, "Punkty �ycia: %d.\n\r", victim->hit);
	send_to_char( buf, ch );

	sprintf( buf,"Poziom: %d.\n\r",victim->level);
	send_to_char( buf, ch );

	if (ch->class == CLASS_CLERIC || ch->class == CLASS_PALADIN)
	{
		sprintf( buf,"Align: %d.\n\r", 	victim->alignment);
		send_to_char( buf, ch );
	}

	if ( ch->class == CLASS_PALADIN )
	{
		sprintf( buf,"Modyfikator trafienia: %d.\n\rModyfikator obra�e�: %d.\n\r",GET_HITROLL( victim ), GET_DAMROLL( victim, NULL ));
		send_to_char( buf, ch);
	}

	if ( IS_NPC( victim ) && victim->pIndexData->new_format )
	{
		sprintf( buf, "Obra�enia: %dd%d  typu:  %s.\n\r",victim->damage[ DICE_NUMBER ], victim->damage[ DICE_TYPE ],attack_table[ victim->dam_type ].noun );
		send_to_char( buf, ch );
	}

	return;
}

void spell_sense_life( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;
	int dur;

	if ( IS_AFFECTED( ch, AFF_SENSE_LIFE ) )
	{
		send_to_char( "Potrafisz ju� przecie� wyczuwa� �ycie.\n\r", ch );
		return;
	}

	dur = level / 2;
	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur; af.rt_duration = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = &AFF_SENSE_LIFE;
	affect_to_char( ch, &af, NULL, TRUE );

	send_to_char( "Przymykasz na chwil� oczy, a kiedy je otwierasz, �wiat zdaje si� eksplodowa� tysi�cem kolor�w.\n\r", ch );
	act( "$n przymyka na chwil� oczy, a kiedy je otwiera, widzisz, �e sta�y si� jasnoz�ote." , ch, NULL, NULL, TO_ROOM );
	return;
}

void spell_spiritual_weapon( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA *obj = NULL;

	int value;
	int weap_type = OBJ_VNUM_SPIRITUAL_HAMMER; /* default: mloteczek */
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

    int skill_max_value = 0, skill_value = 0;

	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nie uda�o ci si� przywo�a� widmowej bro�i.\n\r", ch );
		return;
	}

	if ( get_hand_slots( ch, WEAR_WIELD ) >= 2 )
	{
		send_to_char( "Masz ju� dwie zaj�te r�ce. Zdejmij co�.\n\r", ch );
		return;
	}

	if ( get_eq_char( ch, WEAR_WIELD ) )
	{
		send_to_char( "Nosisz ju� jak�� bro�. Zdejmij j�.\n\r", ch );
		return;
    }

    /*
     * wybieranie najlepszej broni
     */
	//Bronie dodatkowe dla paladyna
	/* sword */
	if ( ch->class == CLASS_PALADIN )
	{
		skill_value = get_skill ( ch, gsn_sword );
		if ( skill_value > skill_max_value )
		{
			skill_max_value = skill_value;
			weap_type = OBJ_VNUM_SPIRITUAL_SWORD;
		}
	}
    /* mace */
    skill_value = get_skill ( ch, gsn_mace );
    if ( skill_value > skill_max_value )
    {
        skill_max_value = skill_value;
        weap_type = OBJ_VNUM_SPIRITUAL_HAMMER;
    }

    /* flail */
    skill_value = get_skill ( ch, gsn_flail );
    if ( skill_value > skill_max_value )
    {
        skill_max_value = skill_value;
        weap_type = OBJ_VNUM_SPIRITUAL_FLAIL;
    }

    /* staff */
    skill_value = get_skill ( ch, gsn_staff );
    if ( skill_value > skill_max_value )
    {
        skill_max_value = skill_value;
        weap_type = OBJ_VNUM_SPIRITUAL_STAFF;
    }

    /* short sword */
    skill_value = get_skill ( ch, gsn_shortsword );
    if ( skill_value > skill_max_value )
    {
        skill_max_value = skill_value;
        weap_type = OBJ_VNUM_SPIRITUAL_SHORTSWORD;
    }
    obj = create_object( get_obj_index( weap_type ), FALSE );
    if ( !obj )
    {
        send_to_char( "Puff ... i nic.\n\r", ch );
        bug( "Brak obiektu w dla czaru spell_spiritual_weapon.", 0 );
        return;
    }

	value = 2 + dice( 2, 4 ) + level / 5;

	if ( number_range( 0, luck ) > 15 ) value += 2;
	if ( number_range( 0, luck ) < 5 )  value /= 3;
	obj->timer = value;

	value = 2 * level;
	if ( number_range( 0, luck ) > 15 ) value = ( value * 9 ) / 10;
	if ( number_range( 0, luck ) < 5 ) value = ( value * 115 ) / 100;
	obj->rent_cost = value;

	value = level / 5;
	if ( number_range( 0, luck ) > 13 ) ++value;
	if ( number_range( 0, luck ) < 3 ) --value;

    /*
     * dodaje bonus za posiadane umiejetnosci w danej broni
     */
    if ( skill_max_value > 50 ) { value++; }
    if ( skill_max_value > 30 ) { value++; }
    if ( skill_max_value > 10 ) { value++; }

	EXT_SET_BIT( obj->extra_flags, ITEM_GLOW );
	EXT_SET_BIT( obj->extra_flags, ITEM_MAGIC );

	switch ( UMAX( 0, value ) )
	{
		case 0:
			obj->value[ 1 ] = 1;
			obj->value[ 2 ] = 4;
			obj->value[ 5 ] = 1;
			obj->value[ 6 ] = 1;
			break;
		case 1:
			obj->value[ 1 ] = 1;
			obj->value[ 2 ] = 5;
			obj->value[ 5 ] = 2;
			obj->value[ 6 ] = 2;
			break;
		case 2:
			obj->value[ 1 ] = 2;
			obj->value[ 2 ] = 4;
			obj->value[ 5 ] = 1;
			obj->value[ 6 ] = 1;
			break;
		case 3:
			obj->value[ 1 ] = 2;
			obj->value[ 2 ] = 4;
			obj->value[ 5 ] = 2;
			obj->value[ 6 ] = 1;
			break;
		case 4:
			obj->value[ 1 ] = 2;
			obj->value[ 2 ] = 4;
			obj->value[ 5 ] = 3;
			obj->value[ 6 ] = 1;
			break;
		case 5:
			obj->value[ 1 ] = 2;
			obj->value[ 2 ] = 4;
			obj->value[ 5 ] = 4;
			obj->value[ 6 ] = 1;
			break;
		case 6:
			obj->value[ 1 ] = 2;
			obj->value[ 2 ] = 4;
			obj->value[ 5 ] = 1;
			obj->value[ 6 ] = 2;
			break;
		case 7:
			obj->value[ 1 ] = 2;
			obj->value[ 2 ] = 4;
			obj->value[ 5 ] = 1;
			obj->value[ 6 ] = 3;
			break;
		case 8:
			obj->value[ 1 ] = 2;
			obj->value[ 2 ] = 4;
			obj->value[ 5 ] = 2;
			obj->value[ 6 ] = 2;
			break;
		case 9:
			obj->value[ 1 ] = 2;
			obj->value[ 2 ] = 4;
			obj->value[ 5 ] = 2;
			obj->value[ 6 ] = 3;
            break;
		default:
            /*
             * zaladuje sie tylko jezeli:
             * - szczescie +1
             * - poziom > 29
             * - poziom broni > 50%
             */
			obj->value[ 1 ] = 2;
			obj->value[ 2 ] = 4;
			obj->value[ 5 ] = 3;
			obj->value[ 6 ] = 3;
			break;
    }

    /*
     * logowanie jaka bron zostala stworzona
     */
    sprintf
        (
         log_buf,
         "[%d] spell_spiritual_weapon: %s (%d), skill: %d, weapon: %d (%d)",
         ch->in_room ? ch->in_room->vnum : 0,
         IS_NPC( ch )? ch->short_descr : ch->name,
         ch->level,
         skill_max_value,
         obj->value[0],
         value
        );
    log_string( log_buf );

	obj_to_char( obj, ch );
	equip_char( ch, obj, WEAR_WIELD, TRUE );

	act( "W d�oni $z pojawia si� widmowa bro�.", ch, NULL, NULL, TO_ROOM );
	send_to_char( "W twojej d�oni pojawia si� widmowa bro�.\n\r", ch );
	return;
}

void spell_holy_bolt( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	OBJ_DATA *obj;
	int dam, luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda�o ci si� uderzenia boskiej mocy.\n\r", ch );
		return;
    }

    if ( !is_undead( victim ) )
    {
        send_to_char( "�wi�ta boska moc tego czaru dzia�a tylko na nieumar�ych.\n\r", ch );
        return;
    }
	dam = number_range( 20, 30 ) + dice( level, 3 );
	dam = luck_dam_mod( ch, dam );

	if ( IS_GOOD( ch ) )
	{
		        act( "Promie� bia�ego �wiat�a trafia prosto w $C.", ch, NULL, victim, TO_CHAR );
				act( "Promie� bia�ego �wiat�a trafia ci� prosto w pier�.", ch, NULL, victim, TO_VICT );
				act( "Promie� bia�ego �wiat�a trafia posto w $C.", ch, NULL, victim, TO_NOTVICT );
	}
	if ( IS_EVIL( ch ) )
	{
		        act( "Promie� czerwonego �wiat�a trafia prosto w $C.", ch, NULL, victim, TO_CHAR );
				act( "Promie� czerwonego �wiat�a trafia ci� prosto w pier�.", ch, NULL, victim, TO_VICT );
				act( "Promie� czerwonego �wiat�a trafia posto w $C.", ch, NULL, victim, TO_NOTVICT );
	}
	if ( IS_NEUTRAL( ch) )
	{
		       act( "Promie� niebieskiego �wiat�a trafia prosto w $C.", ch, NULL, victim, TO_CHAR );
				act( "Promie� niebieskiego �wiat�a trafia ci� prosto w pier�.", ch, NULL, victim, TO_VICT );
				act( "Promie� niebieskiego �wiat�a trafia posto w $C.", ch, NULL, victim, TO_NOTVICT );
	}
	 if( victim->hit - (dam*(100-victim->resists[RESIST_HOLY]))/100 < -10 )
    {
    	act( "W jasnym rozb�ysku �wiat�a po $b zostaje tylko kupka popio�u.", victim, NULL, NULL, TO_ROOM );
    	act( "Czujesz okropny b�l gdy, boska moc wypala ci� wypieraj�c negatywn� energi� z ka�dego kawa�ka twojego nieumar�ego cia�a...", victim, NULL, NULL, TO_CHAR );
     	obj = create_object( get_obj_index( OBJ_VNUM_ASH ), FALSE );
	obj->timer = UMAX(3,number_percent()/10);
	obj_to_room( obj, ch->in_room );
     }

	spell_damage( ch, victim, dam, sn, DAM_HOLY, FALSE );
	return;
}

/* Kod napisany przez Viguda */
void spell_mass_cure_light( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    /* max. 5 os�b, czasem mniej, zawsze zawiera cel, o ile jest on w grupie */
    CHAR_DATA *group[ 5 ] = { NULL };

    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    CHAR_DATA *gch;
    unsigned int i, max_heal;

    if ( number_range( 0, LUCK_BASE_MOD + get_curr_stat( ch, STAT_LUC ) ) < 6 )
    {
	send_to_char( "Nie uda�o ci si�.\n\r", ch );
	return;
    }

    /* ogranicz do (level / 6) ludzi, ale nie mniej niz 1 i nie wiecej niz ilu
       sie zmiesci w tablicy "group" */
    max_heal = URANGE(1, level / 6, sizeof group / sizeof group[ 0 ]);

      /* zawsze leczymy cel czaru */
      heal_char( ch, victim, 6 + dice( 2, 5 ) + URANGE(7, level, 35), FALSE ); // 16-50
      level -= 3;

      /* orygina� zawsze leczy� victima i 4 osoby, w sumie max. 5 os�b. Je�li
         victim zosta� tutaj uleczony, to dobieramy 4 osoby, je�li nie - szukamy
	 w sumie pi�ciu */
      max_heal--;

      
    /* do grupy leczonych we� ile si� da */
    i = 0;
    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	if ( !is_same_group( gch, victim ) || gch == victim )
	    continue;

	group[ i ] = gch;
	if ( ++i >= max_heal )
	  break;
    }

    /* je�li kandydat�w jest wi�cej, to sprawd� czy kto� nie zas�uguje na
       leczenie bardziej ni� obecni w grupie do leczenia */
    for ( ; gch; gch = gch->next_in_room )
    {
      if ( !is_same_group( gch, victim ) || gch == victim )
	  continue;

      for ( i = 0; i < max_heal; i++ )
	if ( group[ i ] && GET_PERCENT_HP( group[ i ] ) > GET_PERCENT_HP( gch ) )
	{
	  group[ i ] = gch;
	  break;
	}
    }

    /* tasowanie (Fisher-Yates) - r�wniejsze szanse na mocniejsze leczenie. Bez
       tego, najmocniejsze leczenie dostaj� ludzie z pocz�tku ch->in_room, czyli
       najprawdopodobniej w kolejno�ci wej�cia do pomieszczenia */
    unsigned int r;
    for ( i = max_heal - 1; i > 0; i-- )
    {
      r = number_range( 0, i );
      victim = group[ i ];
      group[ i ] = group[ r ];
      group[ r ] = victim;
    }
    
    /* p�tla lecz�ca nasz ma�y szpital */
    for ( i = 0; i < max_heal; i++ )
    {
      if ( !group[ i ] )
	continue;

      heal_char( ch, group[ i ], 6 + dice( 1, 5 ) + URANGE(7, level, 35), FALSE ); // 16-50
      level -= 3;
    }
	return;
}

/* Opis dzia�ania przy mass cure light */
void spell_mass_cure_moderate( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *group[ 5 ] = { NULL };

    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    CHAR_DATA *gch;
    unsigned int i, max_heal;

    if ( number_range( 0, LUCK_BASE_MOD + get_curr_stat( ch, STAT_LUC ) ) < 7 )
    {
	send_to_char( "Nie uda�o ci si�.\n\r", ch );
	return;
    }

    max_heal = URANGE(1, level / 6, sizeof group / sizeof group[ 0 ]);

	  heal_char( ch, victim, 15 + dice( 3, 6 ) + URANGE(5, level, 35), FALSE ); // 23-68
      level -= 3;
      max_heal--;
      
    i = 0;
    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	if ( !is_same_group( gch, victim ) || gch == victim )
	    continue;

	group[ i ] = gch;
	if ( ++i >= max_heal )
	  break;
    }

    for ( ; gch; gch = gch->next_in_room )
    {
      if ( !is_same_group( gch, victim ) || gch == victim )
	  continue;

      for ( i = 0; i < max_heal; i++ )
	if ( group[ i ] && GET_PERCENT_HP( group[ i ] ) > GET_PERCENT_HP( gch ) )
	{
	  group[ i ] = gch;
	  break;
	}
    }

    unsigned int r;
    for ( i = max_heal - 1; i > 0; i-- )
    {
      r = number_range( 0, i );
      victim = group[ i ];
      group[ i ] = group[ r ];
      group[ r ] = victim;
    }
    
    for ( i = 0; i < max_heal; i++ )
    {
      if ( !group[ i ] )
	continue;

	  heal_char( ch, group[ i ], 15 + dice( 3, 6 ) + URANGE(5, level, 35), FALSE ); // 23-68
      level -= 3;
    }
	return;
}

/* Opis dzia�ania przy mass cure light */
void spell_mass_cure_serious( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *group[ 5 ] = { NULL };

    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    CHAR_DATA *gch;
    unsigned int i, max_heal;

    if ( number_range( 0, LUCK_BASE_MOD + get_curr_stat( ch, STAT_LUC ) ) < 8 )
    {
	send_to_char( "Nie uda�o ci si�.\n\r", ch );
	return;
    }

    max_heal = URANGE(1, level / 6, sizeof group / sizeof group[ 0 ]);

      heal_char( ch, victim, 20 + dice( 4, 6 ) + URANGE(10, (level*2)/3, 50), FALSE ); // 34-94
      level -= 3;
      max_heal--;
      
    i = 0;
    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	if ( !is_same_group( gch, victim ) || gch == victim )
	    continue;

	group[ i ] = gch;
	if ( ++i >= max_heal )
	  break;
    }

    for ( ; gch; gch = gch->next_in_room )
    {
      if ( !is_same_group( gch, victim ) || gch == victim )
	  continue;

      for ( i = 0; i < max_heal; i++ )
	if ( group[ i ] && GET_PERCENT_HP( group[ i ] ) > GET_PERCENT_HP( gch ) )
	{
	  group[ i ] = gch;
	  break;
	}
    }

    unsigned int r;
    for ( i = max_heal - 1; i > 0; i-- )
    {
      r = number_range( 0, i );
      victim = group[ i ];
      group[ i ] = group[ r ];
      group[ r ] = victim;
    }
    
    for ( i = 0; i < max_heal; i++ )
    {
      if ( !group[ i ] )
	continue;

      heal_char( ch, group[ i ], 20 + dice( 4, 6 ) + URANGE(10, (level*2)/3, 50), FALSE ); // 34-94
      level -= 3;
    }
	return;
}

/* Opis dzia�ania przy mass cure light */
void spell_mass_cure_critical( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *group[ 5 ] = { NULL };

    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    CHAR_DATA *gch;
    unsigned int i, max_heal;

    if ( number_range( 0, LUCK_BASE_MOD + get_curr_stat( ch, STAT_LUC ) ) < 9 )
    {
	send_to_char( "Nie uda�o ci si�.\n\r", ch );
	return;
    }

    max_heal = URANGE(1, level / 6, sizeof group / sizeof group[ 0 ]);

	  heal_char( ch, victim, 30 + dice( 6, 6 ) + URANGE(10, level*2, 65), FALSE ); // 46-131 
      level -= 3;
      max_heal--;
      
    i = 0;
    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
	if ( !is_same_group( gch, victim ) || gch == victim )
	    continue;

	group[ i ] = gch;
	if ( ++i >= max_heal )
	  break;
    }

    for ( ; gch; gch = gch->next_in_room )
    {
      if ( !is_same_group( gch, victim ) || gch == victim )
	  continue;

      for ( i = 0; i < max_heal; i++ )
	if ( group[ i ] && GET_PERCENT_HP( group[ i ] ) > GET_PERCENT_HP( gch ) )
	{
	  group[ i ] = gch;
	  break;
	}
    }

    unsigned int r;
    for ( i = max_heal - 1; i > 0; i-- )
    {
      r = number_range( 0, i );
      victim = group[ i ];
      group[ i ] = group[ r ];
      group[ r ] = victim;
    }
    
    for ( i = 0; i < max_heal; i++ )
    {
      if ( !group[ i ] )
	continue;

	  heal_char( ch, group[ i ], 35 + dice( 6, 6 ) + URANGE(10, level*2, 65), FALSE ); // 51-136 
      level -= 3;
    }
	return;
}

/***
  * Deathward - ochrona przed �mierci�,
  * daje nam immunitet na efekty i czary kt�re by nas odrazu u�mierci�y
  * takie jak np. pwk, finger of death czy decapitation.
 ***/


void spell_deathward( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( IS_AFFECTED( victim, AFF_DEATHWARD ) )
	{
		if ( victim == ch )
			send_to_char( "Jeste� ju� chronion<&y/a/e> przed czarami �mierci.\n\r", ch );
		else
		{
			switch ( victim->sex )
			{
				case 0:
					act( "$N jest ju� chronione przed czarami �mierci.", ch, NULL, victim, TO_CHAR );
					break;
				case 2:
					act( "$N jest ju� chroniona przed czarami �mierci.", ch, NULL, victim, TO_CHAR );
					break;
				default :
					act( "$N jest ju� chroniony przed czarami �mierci.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( ch != victim )
			act( "Nie uda�o ci si� ochroni� $Z przed czarami �mierci.", ch, NULL, victim, TO_CHAR );
		else
			send_to_char( "Nie uda�o ci si� ochroni� siebie przed czarami �mierci.\n\r", ch );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 20 + level / 3; af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_DEATHWARD;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Czujesz jak otacza ci� boska ochrona przed czarami nag�ej �mierci.\n\r", victim );
	if ( ch != victim ) act( "Otaczasz $Z bosk� ochron� przed czarami nag�ej �mierci.", ch, NULL, victim, TO_CHAR );

	return;
}

void spell_resist_negative( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ), mod, dur;

	if ( IS_AFFECTED( victim, AFF_RESIST_NEGATIVE ) || IS_AFFECTED( victim, AFF_ENERGY_SHIELD ) )
	{
		if ( victim == ch )
			send_to_char( "Nie mo�esz by� bardziej chronion<&y/a/e>.\n\r", ch );
		else
		{
			switch ( ch->sex )
			{
				case 0:
					act( "$N nie mo�e by� bardziej chronione.", ch, NULL, victim, TO_CHAR );
					break;
				case 2:
					act( "$N nie mo�e by� bardziej chroniona.", ch, NULL, victim, TO_CHAR );
					break;
				default :
					act( "$N nie mo�e by� bardziej chroniony.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( ch == victim )
        {
            send_to_char( "Nie uda�o ci si� zwi�kszy� swojej odporno�ci na negatywn� energi�.\n\r", ch );
        }
        else
        {
            act( "Nie uda�o ci si� zwi�kszy� odporno�ci $Z na negatywn� energi�.", ch, NULL, victim, TO_CHAR );
        }
        return;
	}

	mod = 30+level/2;
	dur = 4 + level / 6;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = mod;
	af.duration = dur; af.rt_duration = 0;
	af.location = APPLY_RESIST;
	af.modifier = RESIST_NEGATIVE;
	af.bitvector = &AFF_RESIST_NEGATIVE;
	affect_to_char( victim, &af, NULL, TRUE );

	switch ( victim->sex )
	{
		case 0:
			send_to_char( "Czujesz, �e teraz jeste� bardziej odporne na negatywn� energi�.\n\r", victim );
			act( "$n jest teraz bardziej odporne na negatywn� energi�.", victim, NULL, NULL, TO_ROOM );
			break;
		case 2:
			send_to_char( "Czujesz, �e teraz jeste� bardziej odporna na negatywn� energi�.\n\r", victim );
			act( "$n jest teraz bardziej odporna na negatywn� energi�.", victim, NULL, NULL, TO_ROOM );
			break;
		default :
			send_to_char( "Czujesz, �e teraz jeste� bardziej odporny na negatywn� energi�.\n\r", victim );
			act( "$n jest teraz bardziej odporny na negatywn� energi�.", victim, NULL, NULL, TO_ROOM );
			break;
	}
	
	return;
}

void spell_mass_protection_evil( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	CHAR_DATA * gch;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ), protection_count = 0;

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( ch != victim )
			act( "Nie uda�o ci si� ochroni� grupy $Z przed z�em.", ch, NULL, victim, TO_CHAR );
		else
			send_to_char( "Nie uda�o ci si� ochroni� grupy przed z�em.\n\r", ch );
		return;
	}

	/* Zaczynamy massowa�! */	
	protection_count = URANGE(3, level/4, 10);		
	
	for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
	{
		if ( !is_same_group( gch, ch ) || IS_AFFECTED( gch, AFF_PROTECT_GOOD ) || IS_AFFECTED( gch, AFF_PROTECT_EVIL ) || IS_EVIL( gch ) )
		continue;
		
		if ( protection_count < 1 )
		return;
		
	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 20 + level / 3; af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_PROTECT_EVIL;
	affect_to_char( gch, &af, NULL, TRUE );

	switch ( gch->sex )
		{
		case 0:
			send_to_char( "Czujesz si� powi�zane z dobrem.\n\r", gch );
			break;
		case 2:
			send_to_char( "Czujesz si� powi�zana z dobrem.\n\r", gch );
			break;
		default :
			send_to_char( "Czujesz si� powi�zany z dobrem.\n\r", gch );
			break;
		}
	if ( ch != gch )
		switch ( gch->sex )
		{
			case 0:
				act( "$N jest chronione przed z�em.", ch, NULL, gch, TO_CHAR );
				break;
			case 2:
				act( "$N jest chroniona przed z�em.", ch, NULL, gch, TO_CHAR );
				break;
			default :
				act( "$N jest chroniony przed z�em.", ch, NULL, gch, TO_CHAR );
				break;
		}
		
	}
  return;
}

void spell_mass_protection_good( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	CHAR_DATA * gch;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ), protection_count = 0;

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( ch != victim )
			act( "Nie uda�o ci si� ochroni� grupy $Z przed dobrem.", ch, NULL, victim, TO_CHAR );
		else
			send_to_char( "Nie uda�o ci si� ochroni� grupy przed dobrem.\n\r", ch );
		return;
	}

	/* Zaczynamy massowa�! */	
	protection_count = URANGE(3, level/4, 10);		
	
	for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
	{
		if ( !is_same_group( gch, ch ) || IS_AFFECTED( gch, AFF_PROTECT_GOOD ) || IS_AFFECTED( gch, AFF_PROTECT_EVIL ) || IS_GOOD( gch ) )
		continue;
		
		if ( protection_count < 1 )
		return;
		
	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 20 + level / 3; af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_PROTECT_GOOD;
	affect_to_char( gch, &af, NULL, TRUE );

	switch ( gch->sex )
	{
		case 0:
			send_to_char( "Czujesz si� powi�zane ze z�em.\n\r", gch );
			break;
		case 2:
			send_to_char( "Czujesz si� powi�zana ze z�em.\n\r", gch );
			break;
		default :
			send_to_char( "Czujesz si� powi�zany ze z�em.\n\r", gch );
			break;
	}
	if ( ch != gch )
		switch ( gch->sex )
		{
			case 0:
				act( "$N jest chronione przed dobrem.", ch, NULL, gch, TO_CHAR );
				break;
			case 2:
				act( "$N jest chroniona przed dobrem.", ch, NULL, gch, TO_CHAR );
				break;
			default :
				act( "$N jest chroniony przed dobrem.", ch, NULL, gch, TO_CHAR );
				break;
		}		
	}	
	return;
 }

void spell_mass_resist_negative( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	CHAR_DATA * gch;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ), mod, dur, protection_count = 0;

	
	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( ch == victim )
        {
            send_to_char( "Nie uda�o ci si� zwi�kszy� odporno�ci swojej grupy na negatywn� energi�.\n\r", ch );
        }
        else
        {
            act( "Nie uda�o ci si� zwi�kszy� odporno�ci $Z na negatywn� energi�.", ch, NULL, victim, TO_CHAR );
        }
        return;
	}
	
	mod = 30+level/2;
	dur = 4 + level / 6;
	/* Zaczynamy massowa�! */	
	protection_count = URANGE(2, level/6, 6);		
	
	for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
	{
		if ( !is_same_group( gch, ch ) || IS_AFFECTED( gch, AFF_RESIST_NEGATIVE ) || IS_AFFECTED( gch, AFF_ENERGY_SHIELD ) )
		continue;
		
		if ( protection_count < 1 )
		return;
		
	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = mod;
	af.duration = dur; af.rt_duration = 0;
	af.location = APPLY_RESIST;
	af.modifier = RESIST_NEGATIVE;
	af.bitvector = &AFF_RESIST_NEGATIVE;
	affect_to_char( gch, &af, NULL, TRUE );

	switch ( gch->sex )
	{
		case 0:
			send_to_char( "Czujesz, �e teraz jeste� bardziej odporne na negatywn� energi�.\n\r", gch );
			break;
		case 2:
			send_to_char( "Czujesz, �e teraz jeste� bardziej odporna na negatywn� energi�.\n\r", gch );
			break;
		default :
			send_to_char( "Czujesz, �e teraz jeste� bardziej odporny na negatywn� energi�.\n\r", gch );
			break;
	}
	if ( ch != gch )
	switch ( gch->sex )
	{
		case 0:
			act( "$n jest teraz bardziej odporne na negatywn� energi�.", gch, NULL, gch, TO_CHAR );
			break;
		case 2:
			act( "$n jest teraz bardziej odporna na negatywn� energi�.", gch, NULL, gch, TO_CHAR );
			break;
		default :
			act( "$n jest teraz bardziej odporny na negatywn� energi�.", gch, NULL, gch, TO_CHAR );
			break;
	}
	}
	return;
}

void spell_mass_aid( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	CHAR_DATA * gch;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ), duration, protection_count = 0;

	
		if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( victim == ch ) send_to_char( "Nie uda�o ci si� otoczy� grupy ochronn� bosk� moc�.\n\r", ch );
		else act( "Nie uda�o ci si� sprowadzi� ochronnej boskiej mocy na grup�.", ch, NULL, victim, TO_CHAR );
		return;
	}
	
	
	/* Zaczynamy massowa�! */	
	protection_count = URANGE(2, level/4, 8);		
		
	for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
	{
		if ( !is_same_group( gch, ch ) || is_affected( gch, gsn_aid ) )
		continue;
		
		if ( protection_count < 1 )
		return;	
		
	af.where = TO_AFFECTS;
	af.type = gsn_aid;
	af.level = level;
	af.duration = UMAX( 8, duration ); af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 10 + ( ch->level * 2 ) ;
	af.bitvector = &AFF_AID;
	affect_to_char( victim, &af, NULL, TRUE );

	act( "Na $c sp�ywa ochronna boska moc.", gch, NULL, NULL, TO_ROOM );
	if ( gch == ch ) send_to_char( "Sam<&/a/o> powodujesz, �e sp�ywa na ciebie ochronna boska moc.\n\r", ch );
	else act( "Za po�rednictwem $z sp�ywa na ciebie ochronna boska moc.", ch, NULL, gch, TO_VICT );
	}

	return;
}

