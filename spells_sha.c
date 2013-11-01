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
 * KILLER MUD is copyright 1999-2010 Killer MUD Staff (alphabetical)   *
 *                                                                     *
 * Jaromir Klisz         (jklisz@gmail.com              ) [Drake     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: spells_sha.c 110 2013-07-18 08:34:31Z gurthg $
 * $HeadURL: svn+ssh://svn@killer.mud.pl/killer/branches/current/src/spells_sha.c $
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
#include "lang.h"
#include "music.h"
#include "tables.h"

/**
 *
 * Czary szamana
 *
 */

extern char * target_name;
CHAR_DATA *get_char_area( CHAR_DATA *ch, char *argument );

bool  saves_dispel	args( ( int dis_level, int spell_level, int duration ) );
bool  check_dispel	args( ( int dis_level, CHAR_DATA *victim, int sn ) );
bool  check_shaman_invoke args( ( CHAR_DATA *ch ) );
void  set_fighting	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	raw_kill	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
bool	check_blink	args( (CHAR_DATA *victim, CHAR_DATA *ch, bool unik) );
bool	remove_obj	args( ( CHAR_DATA *ch, int iWear, bool fReplace ) );

/** Trzy zasady w tworzeniu czar�w szamana:
 *
 *  1) Moc czar�w zale�y tylko od umiej�no�ci 'Invoke Spirit', 3% wyszkolenia umiej�tno�ci powinno dawa� bonusy mniej
 *     wi�cej r�wne 1 poziomowi gracza.
 *  Przykad:
 *            int  spirit_power = get_skill(ch, gsn_invoke_spirit)/3;
 *
 *  2) Szansa na udanie si� czaru wywo�ywana jest funkcj� check_shaman_invoke (I nie jest zale�na od szcz�cia).
 *     Wywo�ywanie funkcji check_shaman_invoke znajduje si� w pliku magic.c
 *  Przyk�ad:
 *           if ( check_shaman_invoke ( ch ) == TRUE )
 *           {
 *           <Tutaj piszemy kod czaru>
 *           }
 *            else
 *           {
 *	        	  send_to_char( "Nie uda�o ci si� przywo�a� duch�w.\n\r", ch );
 *		        return;
 *	          }
 *
 *  3) Nigdy nie �am zasady pierwszej i drugiej.
 *
 *  Drake dnia pa�skiego 1 czerwca 2009 roku.
 */

//Czar osobisty kt�ry umo�liwia szamanowi widzenie w ciemno�ciach.
void spell_spirit_of_vision( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	int  spirit_power = get_skill(ch, gsn_invoke_spirit)/3;
	int duration, af_level = spirit_power / 2;

	if ( IS_AFFECTED( ch, AFF_ANCESTORS_WISDOM ) )
	{
     spirit_power += 5;
	}

   duration = 2 + spirit_power / 4;

	if ( IS_AFFECTED( victim, AFF_DARK_VISION ) )
	{
		send_to_char( "Przecie� widzisz w ciemno�ci.\n\r", ch );
		return;
	}

      if ( check_shaman_invoke ( ch ) == TRUE )
      {
        af.where = TO_AFFECTS;
	     af.type = sn;
	     af.level = af_level;
	     af.duration = duration; af.rt_duration = 0;
	     af.location = APPLY_NONE;
	     af.modifier = 0;
	     af.bitvector = &AFF_DARK_VISION;
	     affect_to_char( victim, &af, NULL, TRUE );

	     act( "Oczy $z zaczynaj� lekko �wieci� zielonym blaskiem.", ch, NULL, NULL, TO_ROOM );
		  send_to_char( "Twoje oczy zaczynaj� lekko �wieci� zielonym blaskiem.\n\r", ch );
      }
      else
      {
        send_to_char( "Nie uda�o ci si� przywo�a� duch�w.\n\r", ch );
	     return;
      }
}

/*Czar umozliwia �atwiejsz� kontrol� nad duchami, jest jednym z tych kt�re szaman ca�y czas powinien mie� na sobie.
  Chyba, �e jest na tyle pot�ny, by nie korzysta� z opieki przodk�w.*/
void spell_subdue_spirits( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	int  spirit_power = get_skill(ch, gsn_invoke_spirit)/3;
	int duration, af_level = spirit_power / 2;

	if ( IS_AFFECTED( ch, AFF_ANCESTORS_WISDOM ) )
	{
     spirit_power += 5;
	}

   duration = 2 + spirit_power / 4;

	if ( IS_AFFECTED( victim, AFF_SUBDUE_SPIRITS ) )
	{
   	send_to_char( "Posiadasz ju� przychylno�� duch�w.\n\r", ch );
		return;
	}

      if ( check_shaman_invoke ( ch ) == TRUE )
      {
        af.where = TO_AFFECTS;
	     af.type = sn;
	     af.level = af_level;
	     af.duration = duration; af.rt_duration = 0;
	     af.location = APPLY_NONE;
	     af.modifier = 0;
	     af.bitvector = &AFF_SUBDUE_SPIRITS;
	     affect_to_char( victim, &af, NULL, TRUE );

	     act( "Dooko�a $z zaczynaj� unosi� si� widmowe postacie.", ch, NULL, NULL, TO_ROOM );
		  send_to_char( "Przywo�ujesz swoje duchy opieku�cze, kt�rych widmowe sylwetki zaczynaj� unosi� si� niezauwa�alnie dooko�a ciebie.\n\r", ch );
      }
      else
      {
        send_to_char( "Nie uda�o ci si� zyska� przychylno�ci duch�w.\n\r", ch );
	     return;
      }
}
/*Czar powoduje, �e rzucaj�cy zakl�cie nie zostawia �lad�w, dodatkowo zmniejsza kary do mv za chodzenie
  po g�rach, stepach, jaskiniach, lodowcu i zaspach. */
void spell_spirit_walk( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	int  spirit_power = get_skill(ch, gsn_invoke_spirit)/3;
	int duration, af_level = spirit_power / 2;

	if ( IS_AFFECTED( ch, AFF_ANCESTORS_WISDOM ) )
	{
     spirit_power += 5;
	}

   duration = 1 + spirit_power / 5;

	if ( IS_AFFECTED( victim, AFF_SPIRIT_WALK ) )
	{
   	send_to_char( "Nie mo�esz bardziej nie zostawia� �lad�w.\n\r", ch );
		return;
	}

      if ( check_shaman_invoke ( ch ) == TRUE )
      {
        af.where = TO_AFFECTS;
	     af.type = sn;
	     af.level = af_level;
	     af.duration = duration; af.rt_duration = 0;
	     af.location = APPLY_NONE;
	     af.modifier = 0;
	     af.bitvector = &AFF_SPIRIT_WALK;
	     affect_to_char( victim, &af, NULL, TRUE );

		  send_to_char( "Niczym duch, przestajesz zostawia� na ziemi jakiekolwiek �lady.\n\r", ch );
      }
      else
      {
        send_to_char( "Nie uda�o ci si� przywo�a� duch�w.\n\r", ch );
	     return;
      }
}

//Czar przyspieszaj�cy zabli�nianie si� ran. Nie kumuluje si� z efektem bandarzy.
void spell_healing_salve( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	int  spirit_power = get_skill(ch, gsn_invoke_spirit)/3;
	int  hp_percent, duration;

	    if ( ch->fighting )
    {
        send_to_char( "Skup si� lepiej na walce.\n\r", ch );
        return;
    }

    if ( is_undead(victim) || IS_SET(victim->form, FORM_CONSTRUCT) )
    {
        send_to_char( "To si� nie uda.\n\r", ch );
        return;
    }

    if ( is_affected( victim, gsn_bandage ) && !is_affected(victim,gsn_bleeding_wound ) )
    {
        if ( ch == victim )
            send_to_char( "Twoje rany s� opatrzone, nie dasz rady rady bardziej przy�pieszy� ich leczenia.\n\r", ch );
        else
            print_char( ch, "Rany %s s� ju� opatrzone, nie dasz rady bardziej przy�pieszy� ich leczenia.\n\r", victim->name2 );
        return;
    }

    hp_percent = 100 * victim->hit / UMAX( 1, get_max_hp(victim) );

    if ( hp_percent > 95 && !is_affected(victim,gsn_bleeding_wound) )
    {
        print_char( ch, "Nie dasz rady bardziej przy�pieszy� gojenia ran.\n\r" );
        return;
    }

	if ( IS_AFFECTED( ch, AFF_ANCESTORS_WISDOM ) )
	{
     spirit_power += 5;
	}

     duration = 3 + URANGE(0, spirit_power/5, 5);

	if ( IS_AFFECTED(ch, AFF_ANCESTORS_FAVOR ) )
	{
     duration = 3 + URANGE(0, spirit_power/5, 7);
	}

   if ( check_shaman_invoke ( ch ) == TRUE )
   {
        af.where = TO_AFFECTS;
        af.type = sn;
        af.level = spirit_power/3;
        af.duration = duration;
        af.rt_duration = 0;
        af.location = APPLY_NONE;
	     af.bitvector = &AFF_HEALING_SALVE;
	     affect_to_char( victim, &af, NULL, TRUE );

        if (ch == victim)
        {
            print_char( ch, "Twoje rany zaczynaj� piec, czujesz �e b�d� goi� si� szybciej.\n\r" );
        }
        else
        {
            if ( victim->position > POS_SLEEPING )
            {
                act( "Przyspieszasz gojenie ran $C.", ch, NULL, victim, TO_CHAR );
                act( "Twoje rany zaczynaj� piec, czujesz, �e b�d� goi� si� szybciej.", ch, NULL, victim, TO_VICT );
            }
            else if ( victim->position == POS_SLEEPING)
            {
                act( "Przyspieszasz gojenie ran $C.", ch, NULL, victim, TO_CHAR );
            }
            else
                act( "Przyspieszasz gojenie ran $C.", ch, NULL, victim, TO_CHAR );
        }

        if( is_affected( victim, gsn_bleeding_wound ) )
        {
            if ( skill_table[ gsn_bleeding_wound ].msg_off )
            {
                send_to_char( skill_table[ gsn_bleeding_wound ].msg_off, victim );
                send_to_char( "\n\r", victim );
            }
            affect_strip( victim, gsn_bleeding_wound );
        }
   }
   else
      {
        send_to_char( "Nie uda�o ci si� przywo�a� duch�w.\n\r", ch );
	     return;
      }
}

/*Piecz�� s�abo�ci - kl�twa os�abiaj�ca si�� uderze� wroga. Szaman poch�ania cz�� mocy wroga.
  Wr�g mo�e posiada� na sobie tylko jedn� kl�twe. */
void spell_seal_of_weakness( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int duration;
	int spirit_power = get_skill(ch, gsn_invoke_spirit)/3;

	if ( ch == victim )
		{
			   send_to_char( "Nie dasz rady tego zrobi�.\n\r", ch );
			   return;
		}

	if ( is_affected( victim, 519 ) || //seal of weakness
	     is_affected( victim, 530 ) || //seal of atrocity
	     is_affected( victim, 531 ) || //seal of despair
	     is_affected( victim, 532 ) )  //seal of doom
	{
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

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		act( "Nie uda�o ci si� przekln�� $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( IS_AFFECTED( ch, AFF_ANCESTORS_WISDOM ) )
	{
     spirit_power += 5;
	}

	duration = 5 + spirit_power / 6;

   if ( check_shaman_invoke ( ch ) == TRUE )
   {
	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = spirit_power/3;
	af.duration = duration; af.rt_duration = 0;
	af.location = APPLY_DAMROLL;
	af.modifier = -(spirit_power/6);
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, "Zosta�<&e�/a�/o�> os�abion<&y/a/e>", TRUE );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = spirit_power/3;
	af.duration =  2; af.rt_duration = 0;
	af.location  = APPLY_DAMROLL;
	af.modifier  = 1;
	affect_to_char( ch, &af, NULL, FALSE  );

		act( "$n celuje w $C wyci�gni�t� r�k�.", ch, NULL, victim, TO_NOTVICT );
		act( "Czujesz jak si�a odp�ywa z twych r�k.", ch, NULL, victim, TO_VICT );
		act( "Celujesz w $C wyci�gni�t� r�k� i poch�aniasz jego si��.", ch, NULL, victim, TO_CHAR );
	return;
  }
  else
  {
        send_to_char( "Nie uda�o ci si� przywo�a� duch�w.\n\r", ch );
		return;
  }
}

//Pierwszy z dar�w przodk�w - wiedza przodk�w zwi�ksza si�e wszystkich innych szama�skich czar�w.
void spell_ancestors_wisdom( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	int  spirit_power = get_skill(ch, gsn_invoke_spirit)/3;
	int  af_level = spirit_power / 2;

	if ( is_affected( victim, 520 ) || //Sn: 520 Spell: ancestors wisdom
	     is_affected( victim, 521 ) ||
	     is_affected( victim, 522 ) ||
	     is_affected( victim, 523 )  ||
	     is_affected( victim, 524 ) )
	{
   	act( "Przywo�a�e� ju� b�ogos�awie�stwo przodk�w.", ch, NULL, victim, TO_CHAR );
		return;
	}

      if ( check_shaman_invoke ( ch ) == TRUE )
      {
        af.where = TO_AFFECTS;
	     af.type = sn;
	     af.level = af_level;
	     af.duration = 8; af.rt_duration = 0;
	     af.location = APPLY_NONE;
	     af.modifier = 0;
	     af.bitvector = &AFF_ANCESTORS_WISDOM;
	     affect_to_char( victim, &af, NULL, TRUE );

		  send_to_char( "Przywo�ujesz duchy przodk�w, kt�re skupiaj� si� dooko�a ciebie szeptaj�c ci do ucha zapomniane sekrety.\n\r", ch );
      }
      else
      {
        send_to_char( "Nie uda�o ci si� przywo�a� duch�w przodk�w.\n\r", ch );
	     return;
      }
}

//Drugi z darow przodkow - protekcja pozwala zwiekszyc odpornosci szamana na wiele negatywnych efektow.
void spell_ancestors_protection( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	if ( is_affected( victim, 520 ) || //Sn: 520 Spell: ancestors wisdom
	     is_affected( victim, 521 ) ||
	     is_affected( victim, 522 ) ||
	     is_affected( victim, 523 )  ||
	     is_affected( victim, 524 ) )
	{
   	act( "Przywo�a�e� ju� b�ogos�awie�stwo przodk�w.", ch, NULL, victim, TO_CHAR );
		return;
	}


      if ( check_shaman_invoke ( ch ) == TRUE )
      {
	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = get_skill(ch, gsn_invoke_spirit)/9;
	af.duration = 8;
	af.rt_duration = 0;
	af.location = APPLY_RESIST;

		af.modifier = RESIST_PIERCE;
		af.bitvector = &AFF_ANCESTORS_PROTECTION;
		affect_to_char( victim, &af, NULL, TRUE );

      af.modifier    = RESIST_SLASH;
		af.bitvector = &AFF_ANCESTORS_PROTECTION;
		affect_to_char( victim, &af, NULL, TRUE );

      af.modifier    = RESIST_BASH;
		af.bitvector = &AFF_ANCESTORS_PROTECTION;
		affect_to_char( victim, &af, NULL, TRUE );

      af.modifier    = RESIST_FIRE;
		af.bitvector = &AFF_ANCESTORS_PROTECTION;
		affect_to_char( victim, &af, NULL, TRUE );

      af.modifier    = RESIST_ELECTRICITY;
		af.bitvector = &AFF_ANCESTORS_PROTECTION;
		affect_to_char( victim, &af, NULL, TRUE );

      af.modifier    = RESIST_COLD;
		af.bitvector = &AFF_ANCESTORS_PROTECTION;
		affect_to_char( victim, &af, NULL, TRUE );

      af.modifier    = RESIST_ACID;
		af.bitvector = &AFF_ANCESTORS_PROTECTION;
		affect_to_char( victim, &af, NULL, TRUE );

      af.modifier    = RESIST_POISON;
		af.bitvector = &AFF_ANCESTORS_PROTECTION;
		affect_to_char( victim, &af, NULL, TRUE );

      af.modifier    = RESIST_NEGATIVE;
		af.bitvector = &AFF_ANCESTORS_PROTECTION;
		affect_to_char( victim, &af, NULL, TRUE );

      af.modifier    = RESIST_MENTAL;
		af.bitvector = &AFF_ANCESTORS_PROTECTION;
		affect_to_char( victim, &af, NULL, TRUE );

      af.modifier    = RESIST_HOLY;
		af.bitvector = &AFF_ANCESTORS_PROTECTION;
		affect_to_char( victim, &af, NULL, TRUE );

      af.modifier    = RESIST_SOUND;
		af.bitvector = &AFF_ANCESTORS_PROTECTION;
		affect_to_char( victim, &af, NULL, TRUE );

		  send_to_char( "Przywo�ujesz duchy przodk�w, kt�re skupiaj� si� dooko�a ciebie chroni�c ci� przed zagro�eniem.\n\r", ch );
      }
      else
      {
        send_to_char( "Nie uda�o ci si� przywo�a� duch�w przodk�w.\n\r", ch );
	     return;
      }
}

//Trzeci z dar�w przodk�w - zwi�ksza d�ugo�� czar�w leczniczych rzucanych przez nasz� posta�.
void spell_ancestors_favor( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	int  spirit_power = get_skill(ch, gsn_invoke_spirit)/3;
	int  af_level = spirit_power / 2;

	if ( is_affected( victim, 520 ) || //Sn: 520 Spell: ancestors wisdom
	     is_affected( victim, 521 ) ||
	     is_affected( victim, 522 ) ||
	     is_affected( victim, 523 )  ||
	     is_affected( victim, 524 ) )
	{
   	act( "Przywo�a�e� ju� b�ogos�awie�stwo przodk�w.", ch, NULL, victim, TO_CHAR );
		return;
	}

      if ( check_shaman_invoke ( ch ) == TRUE )
      {
        af.where = TO_AFFECTS;
	     af.type = sn;
	     af.level = af_level;
	     af.duration = 8; af.rt_duration = 0;
	     af.location = APPLY_NONE;
	     af.modifier = 0;
	     af.bitvector = &AFF_ANCESTORS_FAVOR;
	     affect_to_char( victim, &af, NULL, TRUE );

		  send_to_char( "Przywo�ujesz duchy przodk�w i wr�cz czujesz ca�ym sob� przep�ywaj�c� dooko�a ciebie �yciodajn� energi�.\n\r", ch );
      }
      else
      {
        send_to_char( "Nie uda�o ci si� przywo�a� duch�w przodk�w.\n\r", ch );
	     return;
      }
}

//Czwarty z dar�w przodk�w - wizja przodk�w pozwala nam zobaczy� to, co do tej pory by�o ukryte przed wzrokiem.
void spell_ancestors_vision( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	int  spirit_power = get_skill(ch, gsn_invoke_spirit)/3;
	int  af_level = spirit_power / 2;

	if ( is_affected( victim, 520 ) || //Sn: 520 Spell: ancestors wisdom
	     is_affected( victim, 521 ) ||
	     is_affected( victim, 522 ) ||
	     is_affected( victim, 523 )  ||
	     is_affected( victim, 524 ) )
	{
   	act( "Przywo�a�e� ju� b�ogos�awie�stwo przodk�w.", ch, NULL, victim, TO_CHAR );
		return;
	}

      if ( check_shaman_invoke ( ch ) == TRUE )
      {

   //Stripujemy detecta jak ma, nie m�wi�c o tym - nie chcemy by si� dublowa�.
   if ( IS_AFFECTED( victim, AFF_DETECT_INVIS ) )
   affect_strip( victim, 35 );
   //J/W tyle �e detect hidden.
	if ( IS_AFFECTED( victim, AFF_DETECT_HIDDEN ) )
   affect_strip( victim, 34 );

   af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 8;
	af.rt_duration = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = &AFF_DETECT_INVIS;
	affect_to_char( victim, &af, NULL, FALSE );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 8;
	af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_DETECT_HIDDEN;
	affect_to_char( victim, &af, NULL, FALSE );

   af.where = TO_AFFECTS;
	af.type = sn;
	af.level = af_level;
	af.duration = 8; af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_ANCESTORS_VISION;
	affect_to_char( victim, &af, NULL, TRUE );

		  send_to_char( "Przywo�ujesz duchy przodk�w i otwierasz ponownie oczy na �wiat, na rzeczy kt�re by�y dotychczas ukryte i niewidzialne.\n\r", ch );
      }
      else
      {
        send_to_char( "Nie uda�o ci si� przywo�a� duch�w przodk�w.\n\r", ch );
	     return;
      }
}

/*Pi�ty z dar�w przodk�w - furia przodk�w. Daje szanse na dodatkowy atak (zale�n� od skilla invoke - 20-40% -
  ale blokuje uciekanie z walki na ca�y czas trwania (8 tick�w).*/
void spell_ancestors_fury( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	int  spirit_power = get_skill(ch, gsn_invoke_spirit)/3;
	int  af_level = spirit_power / 2;

	if ( is_affected( victim, 520 ) || //Sn: 520 Spell: ancestors wisdom
	     is_affected( victim, 521 ) ||
	     is_affected( victim, 522 ) ||
	     is_affected( victim, 523 )  ||
	     is_affected( victim, 524 ) )
	{
   	act( "Przywo�a�e� ju� b�ogos�awie�stwo przodk�w.", ch, NULL, victim, TO_CHAR );
		return;
	}

      if ( check_shaman_invoke ( ch ) == TRUE )
      {
        af.where = TO_AFFECTS;
	     af.type = sn;
	     af.level = af_level;
	     af.duration = 8; af.rt_duration = 0;
	     af.location = APPLY_NONE;
	     af.modifier = 0;
	     af.bitvector = &AFF_ANCESTORS_FURY;
	     affect_to_char( victim, &af, NULL, TRUE );

		  send_to_char( "Przywo�ujesz duchy przodk�w, czujesz narastaj�cy w twych �y�ach gniew.\n\r", ch );
      }
      else
      {
        send_to_char( "Nie uda�o ci si� przywo�a� duch�w przodk�w.\n\r", ch );
	     return;
      }
}
//Spirit shackle - hold na niematerialne widma i przybysz�w z innych sfer.
void spell_spirit_shackle( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int duration, spirit_power = get_skill(ch, gsn_invoke_spirit)/3;;


	/*sprawdzanie bez powiadomienia bledu*/
	if ( !IS_SET( race_table[ GET_RACE( victim ) ].type , OUTSIDER )
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

    // stripujemy invisa je�eli target jest graczem
    if (!IS_NPC(victim))
    {
        strip_invis(ch, TRUE, TRUE);
    }


	duration = 1 + ( spirit_power / 10 );

 if ( check_shaman_invoke ( ch ) == TRUE )
    {
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
   else
   {
        send_to_char( "Nie uda�o ci si� przywo�a� duch�w przodk�w.\n\r", ch );
	     return;
   }
}

//Dismiss outsider - odsy�anie istot z innych plan�w i widm: dla maga, druida i szamana.
void spell_dismiss_outsider( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	CHAR_DATA * vmaster;
	int chance, hp_bonus, spirit_power = get_skill(ch, gsn_invoke_spirit)/3;;;

	DEBUG_INFO( "do_cast:spell_dismiss_outsider:1");

	if ( victim == NULL )
	{
		send_to_char( "Kogo chcesz odes�a�?", ch);
		return;
	}
	else if ( victim == ch )
	{
		send_to_char( "No co ty, sam<&/a/o> siebie chcesz odes�a�?", ch);
		return;
	}

	DEBUG_INFO( "do_cast:spell_dismiss_outsider:2");

	vmaster = victim->master;

	if ( vmaster == NULL )
		vmaster = victim;

	if ( !IS_SET( race_table[ victim->race ].type, OUTSIDER ) )
	{
		act( "$N nie jest pozasferowcem.", ch, NULL, victim, TO_CHAR );
		return;
	}
	if ( !EXT_IS_SET( victim->act, ACT_NO_EXP ) || !IS_NPC( victim ) )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "$N nie zosta�o przywo�ane.", ch, NULL, victim, TO_CHAR );
				break;
			case 1:
				act( "$N nie zosta� przywo�any.", ch, NULL, victim, TO_CHAR );
				break;
			case 2:
				act( "$N nie zosta�a przywo�ana.", ch, NULL, victim, TO_CHAR );
				break;
		}
		return;
	}

	if ( vmaster == ch )
	{
		chance = URANGE(80,80+number_range(1,15),95);
	}
	else
	{
		chance = 55;
		chance += ( get_curr_stat_deprecated( ch, STAT_LUC ) - 10 ) / 2;
		chance -= ( get_curr_stat_deprecated( victim, STAT_LUC ) - 10 ) / 2;
		if ( ch->class == CLASS_SHAMAN )
		{ chance += spirit_power * 4; }
		else
		{ chance += level * 4; }
		chance -= victim->level / 2;
		chance -= vmaster->level * 3;
		chance -= get_curr_stat_deprecated( vmaster, STAT_WIS ) - 10;
		chance += get_curr_stat_deprecated( ch, STAT_WIS ) - 10;
		hp_bonus = 50 - ( 100 * victim->hit / UMAX( get_max_hp(victim), 1 ) );
		chance += hp_bonus / 2;
		if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_CONSECRATE ) && IS_GOOD( vmaster ) && IS_EVIL( ch ) ) chance -= 20;
		if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_CONSECRATE ) && IS_EVIL( vmaster ) && IS_GOOD( ch ) ) chance += 20;
		if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_DESECRATE ) && IS_GOOD( vmaster ) && IS_EVIL( ch ) ) chance += 20;
		if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_DESECRATE ) && IS_EVIL( vmaster ) && IS_GOOD( ch ) ) chance -= 20;
		if ( !IS_NPC( ch ) )
		{
			if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
			{
				chance += chance/5;
			}
		}
		chance = URANGE( 5, chance, 75 );
	}

	if( ch->class == CLASS_DRUID )
	{
		act( "Koncentrujesz si� wzywaj�c moce natury by ci� wspomog�y, zamykasz oczy na chwil� po czym otwieraj�c je kierujesz moc w kierunku $Z.", ch, NULL, victim, TO_CHAR );
		act( "$n koncentruje si� przez chwil� po czym wymawia s�owa w dziwnym dialekcie. Zamyka na chwil� oczy po czym otwieraj�c je wypuszcza ze swych r�k promienie zielonej energii, kt�re opl�tuj� $C.", ch, NULL, victim, TO_ROOM );

		if ( number_percent() < chance )
		{
			act( "$n znika w rozb�ysku �wiat�a!", victim, NULL, NULL, TO_ALL );
			extract_char( victim, TRUE );
		}
		else
			act( "Sylwetka $z migocze przez chwil�, jednak nic si� nie dzieje.", victim, NULL, NULL, TO_ALL );
		return;
	}
   else if ( ch->class == CLASS_SHAMAN && check_shaman_invoke ( ch ) == TRUE )
   {
		act( "Koncentrujesz si� pr�buj�c odes�a� niepokornego ducha z powrotem tam gdzie jego miejsce, wskazujesz na $Z.", ch, NULL, victim, TO_CHAR );
		act( "$n koncentruje si� przez chwil� po czym wskazuje na $C.", ch, NULL, victim, TO_ROOM );

		if ( number_percent() < chance )
		{
			act( "$n znika w rozb�ysku �wiat�a!", victim, NULL, NULL, TO_ALL );
			extract_char( victim, TRUE );
		}
		else
			act( "Sylwetka $z migocze przez chwil�, jednak nic si� nie dzieje.", victim, NULL, NULL, TO_ALL );
		return;
	}
	else
	{
        send_to_char( "Nie uda�o ci si� przywo�a� duch�w przodk�w.\n\r", ch );
	     return;
	}

	act( "Koncentruj�c si� przywo�ujesz wielkie, staro�ytne moce, kt�re kr��� wok� twej ofiary przykuwaj�c j� do ziemi.", ch, NULL, victim, TO_CHAR );
	act( "$n przywo�uje wielkie, staro�ytne moce, kt�re kr��� wok� $s ofiary przykuwaj�c j� do ziemi.", ch, NULL, victim, TO_ROOM );

	DEBUG_INFO( "do_cast:spell_dismiss_outsider:4");

	if ( number_percent() < chance )
	{
		act( "W nag�ym rozb�ysku �wiat�a $n znika.", victim, NULL, NULL, TO_ALL );
		extract_char( victim, TRUE );
	}
	else
	{
		act( "Sylwetka $z migocze przez chwil�, jednak nic si� nie dzieje.", victim, NULL, NULL, TO_ALL );
	}

	return;
}
// Spirit of Life - czar zwi�kszajacy regeneracj� celu
void spell_spirit_of_life( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	int  spirit_power = get_skill(ch, gsn_invoke_spirit)/3;
	int duration;

   if ( is_undead(victim) || IS_SET(victim->form, FORM_CONSTRUCT) )
   {
        send_to_char( "To si� nie uda.\n\r", ch );
        return;
   }

	if ( IS_AFFECTED( ch, AFF_ANCESTORS_WISDOM ) )
	{
     spirit_power += 5;
	}

     duration = number_range(1, 2);

	if ( IS_AFFECTED( ch, AFF_ANCESTORS_FAVOR ) )
	{
     duration = number_range(2, 4);
	}

   if ( check_shaman_invoke ( ch ) == TRUE )
      {
        af.where = TO_AFFECTS;
	     af.type = sn;
	     af.level = spirit_power/2;
	     af.duration = duration; af.rt_duration = 0;
	     af.location = APPLY_NONE;
	     af.modifier = 0;
	     af.bitvector = &AFF_SPIRIT_OF_LIFE;
	     affect_to_char( victim, &af, NULL, TRUE );

		  send_to_char( "Czujesz lecznic� energie rozlewaj�c� si� po twoim ciele.\n\r", ch );
      }
      else
      {
        send_to_char( "Nie uda�o ci si� przywo�a� duch�w.\n\r", ch );
	     return;
      }
}

// Breath of life - czar znacznie zwi�kszaj�cy regeneracje celu, opr�cz tego ma szans� usun�� efekt zatrucia lub choroby.
void spell_breath_of_life( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	int  spirit_power = get_skill(ch, gsn_invoke_spirit)/3;
	int duration, af_level = spirit_power/2;

	if ( is_undead(victim) || IS_SET(victim->form, FORM_CONSTRUCT) )
	{
		send_to_char( "To si� nie uda.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( ch, AFF_ANCESTORS_WISDOM ) )
	{
		spirit_power += 5;
	}

	duration = number_range(1, 2);

	if ( IS_AFFECTED( ch, AFF_ANCESTORS_FAVOR ) )
	{
		duration = number_range(2, 4);
	}

	if ( check_shaman_invoke ( ch ) == TRUE )
	{
		af.where = TO_AFFECTS;
		af.type = sn;
		af.level = af_level;
		af.duration = duration; af.rt_duration = 0;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = &AFF_BREATH_OF_LIFE;
		affect_to_char( victim, &af, NULL, TRUE );

		send_to_char( "Czujesz pot�n� i relaksuj�c� moc wype�niaj�c� ca�e twoje cia�o.\n\r", ch );

		if ( is_affected( victim, gsn_poison ) && number_percent() > (100 - spirit_power) )
		{
			affect_strip( victim, gsn_poison );
			send_to_char( "Trucizna przestaje kr��y� w twojej krwi.\n\r", victim );
			act( "$n wygl�da o wiele lepiej.", victim, NULL, NULL, TO_ROOM );
		}

		if ( is_affected( victim, gsn_plague ) && number_percent() > (100 - spirit_power) )
		{
			affect_strip( victim, gsn_plague );
			send_to_char( "Objawy choroby cofaj� si�.\n\r", victim );
			act( "$n wygl�da o wiele lepiej.", victim, NULL, NULL, TO_ROOM );
		}


	}
	else
	{
		send_to_char( "Nie uda�o ci si� przywo�a� duch�w.\n\r", ch );
		return;
	}
}

void spell_spiritual_guidance( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	int  spirit_power = get_skill(ch, gsn_invoke_spirit)/3;
	int duration, af_level = spirit_power / 2;

	if ( IS_AFFECTED( ch, AFF_ANCESTORS_WISDOM ) )
	{
     spirit_power += 5;
	}

   duration = 1 + spirit_power / 5;

	if ( IS_AFFECTED( victim, AFF_SPIRITUAL_GUIDANCE ) )
	{
	  if ( victim == ch )
	 {
	   send_to_char( "$N posiada juz opiek� duch�w.\n\r", ch);
		return;
	 }
	 else
	 {
	   send_to_char( "Posiadasz ju� na sobie opiek� duch�w.\n\r", ch );
		return;
	 }
	}

      if ( check_shaman_invoke ( ch ) == TRUE )
      {
        af.where = TO_AFFECTS;
	     af.type = sn;
	     af.level = af_level;
	     af.duration = duration; af.rt_duration = 0;
	     af.location = APPLY_NONE;
	     af.modifier = 0;
	     af.bitvector = &AFF_SPIRITUAL_GUIDANCE;
	     affect_to_char( victim, &af, NULL, TRUE );

		  send_to_char( "Czujesz, �e duchy zaczynaj� ci sprzyja� prowadz�c pewniej tw� d�o� w walce.\n\r", victim );
		  send_to_char( "Sprowadzasz opiek� duch�w.\n\r", ch );
      }
      else
      {
        send_to_char( "Nie uda�o ci si� przywo�a� duch�w.\n\r", ch );
	     return;
      }
}


/*Piecz�� okrucie�stwa - obcina skille wroga o 1/3. Narazie wy��czam bo buguje muda.
  Wr�g mo�e posiada� na sobie tylko jedn� kl�twe.
void spell_seal_of_atrocity( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int duration;
	int spirit_power = get_skill(ch, gsn_invoke_spirit)/3;

	if ( ch == victim )
		{
			   send_to_char( "Nie dasz rady tego zrobi�.\n\r", ch );
			   return;
		}

	if ( is_affected( victim, 519 ) || //seal of weakness
	     is_affected( victim, 530 ) || //seal of atrocity
	     is_affected( victim, 531 ) || //seal of despair
	     is_affected( victim, 532 ) )  //seal of doom
	{
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

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		act( "Nie uda�o ci si� przekln�� $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( IS_AFFECTED( ch, AFF_ANCESTORS_WISDOM ) )
	{
     spirit_power += 3;
	}

	duration = 5 + spirit_power / 6;

   if ( check_shaman_invoke ( ch ) == TRUE )
   {
	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = spirit_power/3;
	af.duration = duration; af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = -(spirit_power/6);
	af.bitvector = &AFF_SEAL_OF_ATROCITY;
	affect_to_char( victim, &af, "Twoje ruchy s� bardziej niezdarne niz zwykle", TRUE );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = spirit_power/3;
	af.duration =  2; af.rt_duration = 0;
	af.location  = APPLY_HITROLL;
	af.modifier  = 1;
	affect_to_char( ch, &af, NULL, FALSE  );

		act( "$n celuje w $C wyci�gni�t� r�k�.", ch, NULL, victim, TO_NOTVICT );
		act( "Czujesz jak pewno�� odp�ywa z twych r�k.", ch, NULL, victim, TO_VICT );
		act( "Celujesz w $C wyci�gni�t� r�k� i poch�aniasz je<&go/j/go> pewno�� w pos�ugiwania si� umiej�tno�ciami.", ch, NULL, victim, TO_CHAR );
	return;
  }
  else
  {
        send_to_char( "Nie uda�o ci si� przywo�a� duch�w.\n\r", ch );
		return;
  }
}

Piecz�� desperacji - blokuje mo�liwo�� zadawania krytycznych cios�w wrogowi. Wy��czam bo buguje muda.
  Wr�g mo�e posiada� na sobie tylko jedn� kl�twe.
void spell_seal_of_despair( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int duration;
	int spirit_power = get_skill(ch, gsn_invoke_spirit)/3;

	if ( ch == victim )
		{
			   send_to_char( "Nie dasz rady tego zrobi�.\n\r", ch );
			   return;
		}

	if ( is_affected( victim, 519 ) || //seal of weakness
	     is_affected( victim, 530 ) || //seal of atrocity
	     is_affected( victim, 531 ) || //seal of despair
	     is_affected( victim, 532 ) )  //seal of doom
	{
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

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		act( "Nie uda�o ci si� przekln�� $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( IS_AFFECTED( ch, AFF_ANCESTORS_WISDOM ) )
	{
     spirit_power += 3;
	}

	duration = 5 + spirit_power / 6;

   if ( check_shaman_invoke ( ch ) == TRUE )
   {
	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = spirit_power/3;
	af.duration = duration; af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = -(spirit_power/6);
	af.bitvector = &AFF_SEAL_OF_DESPAIR;
	affect_to_char( victim, &af, "Przestajesz widzie� ods�oni�te miejsa na ciele przeciwnik�w", TRUE );

		act( "$n celuje w $C wyci�gni�t� r�k�.", ch, NULL, victim, TO_NOTVICT );
		act( "Czujesz jak sylwetka twego wroga rozmywa ci si� w oczach.", ch, NULL, victim, TO_VICT );
		act( "Celujesz w $C wyci�gni�t� r�k� i ukrywasz wszystkie s�abe punkty przed je<&go/j/go> wzrokiem.", ch, NULL, victim, TO_CHAR );
	return;
  }
  else
  {
        send_to_char( "Nie uda�o ci si� przywo�a� duch�w.\n\r", ch );
		return;
  }
}
*/

/*Piecz�� zag�ady - pozbawia wroga losowego zmys�u.
  Wr�g mo�e posiada� na sobie tylko jedn� kl�twe. */
void spell_seal_of_doom( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int duration;
	int spirit_power = get_skill(ch, gsn_invoke_spirit)/3;

	if ( ch == victim )
		{
			   send_to_char( "Nie dasz rady tego zrobi�.\n\r", ch );
			   return;
		}

	if ( is_affected( victim, 519 ) || //seal of weakness
	     is_affected( victim, 530 ) || //seal of atrocity
	     is_affected( victim, 531 ) || //seal of despair
	     is_affected( victim, 532 ) )  //seal of doom
	{
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

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		act( "Nie uda�o ci si� przekln�� $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}


	if ( IS_AFFECTED( ch, AFF_ANCESTORS_WISDOM ) )
	{
     spirit_power += 5;
	}

	duration = 3 + spirit_power / 6;

   if ( check_shaman_invoke ( ch ) == TRUE )
   {

	switch ( number_range (1,3))//Losowanie kt�ry zmys�
	{
    case 1://o�lepienie
            if ( IS_AFFECTED( victim, AFF_BLIND ) || IS_AFFECTED(victim,AFF_PERFECT_SENSES )  )
            {
               send_to_char( "Duchy odpowiedzia�y na twoje wezwanie, ale nic si� nie dzieje.\n\r", ch );
               return;
            }
            else
            {
             af.where = TO_AFFECTS;
             af.type = sn;
             af.level = spirit_power/3;
             af.location = APPLY_NONE;
             af.modifier = 0;
	          af.duration = duration;
	          af.rt_duration = 0;
	          af.bitvector = &AFF_BLIND;
	          affect_to_char( victim, &af, "Zosta�e� o�lepiony", TRUE );

             act( "$n celuje w $C wyci�gni�t� r�k�.", ch, NULL, victim, TO_NOTVICT );
             act( "Czujesz jak �wiat�o opuszcza twe oczy.", ch, NULL, victim, TO_VICT );
             act( "Celujesz w $C wyci�gni�t� r�k� i pozbawiasz <&go/j�/je> wzroku.", ch, NULL, victim, TO_CHAR );
             return;
             break;
            }
      case 2://Uciszenie
            if ( IS_AFFECTED( victim, AFF_SILENCE ) || IS_AFFECTED(victim,AFF_PERFECT_SENSES )  )
            {
               send_to_char( "Duchy odpowiedzia�y na twoje wezwanie, ale nic si� nie dzieje.\n\r", ch );
               return;
            }
           else
            {
             af.where = TO_AFFECTS;
             af.type = sn;
             af.level = spirit_power/3;
             af.location = APPLY_NONE;
             af.modifier = 0;
	          af.duration = duration;
	          af.rt_duration = 0;
	          af.bitvector = &AFF_SILENCE;
	          affect_to_char( victim, &af, "Zosta�e� uciszony", TRUE );

             act( "$n celuje w $C wyci�gni�t� r�k�.", ch, NULL, victim, TO_NOTVICT );
             act( "Czujesz �e �adne s�owo nie przeci�nie ci si� przez gard�o.", ch, NULL, victim, TO_VICT );
             act( "Celujesz w $C wyci�gni�t� r�k� i pozbawiasz <&go/j�/je> mowy.", ch, NULL, victim, TO_CHAR );
             return;
             break;
            }
      case 3://Og�uszenie
            if ( IS_AFFECTED( victim, AFF_DEAFNESS ) || IS_AFFECTED(victim,AFF_PERFECT_SENSES )  )
            {
               send_to_char( "Duchy odpowiedzia�y na twoje wezwanie, ale nic si� nie dzieje.\n\r", ch );
               return;
            }
           else
            {
             af.where = TO_AFFECTS;
             af.type = sn;
             af.level = spirit_power/3;
             af.location = APPLY_NONE;
             af.modifier = 0;
	          af.duration = duration;
	          af.rt_duration = 0;
	          af.bitvector = &AFF_DEAFNESS;
	          affect_to_char( victim, &af, "Zosta�e� og�uszony", TRUE );

             act( "$n celuje w $C wyci�gni�t� r�k�.", ch, NULL, victim, TO_NOTVICT );
             act( "Twoje uszy przestaj� s�ysze� cokolwiek.", ch, NULL, victim, TO_VICT );
             act( "Celujesz w $C wyci�gni�t� r�k� i pozbawiasz <&go/j�/je> s�uchu.", ch, NULL, victim, TO_CHAR );
             return;
             break;
            }
       default: break;
     }
   }
  else
  {
        send_to_char( "Nie uda�o ci si� przywo�a� duch�w.\n\r", ch );
		return;
  }
}

