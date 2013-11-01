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
 * Brosig Micha�         (brohacz@gmail.com             ) [Brohacz   ] *
 * Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor     ] *
 * Koper Tadeusz         (jediloop@go2.pl               ) [Garloop   ] *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Skrzetnicki Krzysztof (gtener@gmail.com              ) [Tener     ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: fight.c 127 2013-07-27 14:23:52Z drake $
 * $HeadURL: svn+ssh://svn@killer.mud.pl/killer/branches/current/src/fight.c $
 *
 */
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "recycle.h"
#include "monk_kit.h"
#include "tables.h"
#include "progs.h"
#include "clans.h"
#include "magic.h"
#include "shapeshifting.h"
#include "projects.h"
#include "mount.h"
#include "money.h"
#include "paladin_auras.h"

//#define INFO
/*
 * Local functions.
 */

void		op_fight_trigger args( (CHAR_DATA *ch));
void		check_assist	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void		check_killer	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool		check_parry	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam, OBJ_DATA *parowana ) );
void		dam_message	 args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt, bool immune ) );
void		death_cry	args( ( CHAR_DATA *ch ) );
void		group_gain	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool		mob_can_attack	args( ( CHAR_DATA * ch ) );
bool		is_safe		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void		make_corpse	args( ( CHAR_DATA *ch ) );
void		one_hit		args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt, bool second ) );
void		mob_hit		args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void		raw_kill	args( ( CHAR_DATA *victim , CHAR_DATA *ch ) );
void		set_fighting	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void		disarm		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
OBJ_DATA*	randomize_item	args( (int points) );
OBJ_DATA* generate_random_spellbook args ( ( CHAR_DATA *ch, int counter ) );
sh_int		get_caster	args( (CHAR_DATA *ch) );
DECLARE_DO_FUN(	do_dismount	);
void		extract_dead_player	args( ( CHAR_DATA *ch ) );
void		update_obj_cond		args( (CHAR_DATA *ch, OBJ_DATA *weapon, int base_dam, int dam, CHAR_DATA *victim));
SPELL_MSG*	msg_lookup		args( (sh_int sn) );
int		 	weapon_skill_mod	args( (CHAR_DATA *ch, bool primary, bool hitr) );
bool		check_stone_skin	args( (CHAR_DATA *ch, CHAR_DATA *victim, int dam_type, int *dam, OBJ_DATA *weapon) );
bool		check_mirror_image	args( (CHAR_DATA *victim, CHAR_DATA *ch) );
bool		check_blink	args( (CHAR_DATA *victim, CHAR_DATA *ch, bool unik) );
bool		still_fighting		args( (CHAR_DATA *ch) );
int			compute_tohit		args( (CHAR_DATA *ch, bool primary) );
void		raw_damage		args( (CHAR_DATA *ch,CHAR_DATA *victim,int dam ) );
int		 	check_magic_attack	args( (CHAR_DATA *ch, CHAR_DATA *vch, OBJ_DATA *weapon) );
bool		mp_onehit_trigger	args( ( CHAR_DATA *mob, CHAR_DATA *victim ) );
bool		op_onehit_trigger	args( (OBJ_DATA *obj, CHAR_DATA *ch) );
void		delevel			args( ( CHAR_DATA *ch, bool hide ) );
bool		mp_hit_trigger	args( ( CHAR_DATA *mob, CHAR_DATA *victim ) );
bool		op_hit_trigger	args( (OBJ_DATA *obj, CHAR_DATA *ch) );

void		check_player_death	args( ( CHAR_DATA *ch, CHAR_DATA *killer) );
void		generate_onehit_msg	args( (CHAR_DATA *ch, CHAR_DATA *victim, int dam,int dt,bool immune ) );
void		reorganize_mobile_group args( (CHAR_DATA *mob) );
void		onehit_kill_check	args( (CHAR_DATA *mob, CHAR_DATA *ch) );
bool		check_increase_wounds	args( (CHAR_DATA *victim, int dam_type, int *dam) );
void		check_armor_spell	args( (CHAR_DATA *victim, int dam) );
int		 	parry_mod		args( (OBJ_DATA *weapon) );
bool		special_damage_message	args( (CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *wield, int dam, int dt, bool immune) );
void		check_berserk_rescue	args( (CHAR_DATA * ch) );
bool		check_dodge		args( ( CHAR_DATA *ch, CHAR_DATA *victim, int ac, int th_roll, int thac0, bool critic ) );
bool		check_shield_block( CHAR_DATA *ch, CHAR_DATA *victim, int ac, int th_roll, int thac0, bool critic, int * dam );
void		update_death_statistic	args( (CHAR_DATA *victim, CHAR_DATA *ch) );
void		check_special_unarmed_hit	args( (CHAR_DATA *ch, CHAR_DATA *victim, int base_dam) );
void		one_hit_monk			args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void		strip_fight_affects		args( ( CHAR_DATA * ch ) );
void		check_defensive_spells		args( (CHAR_DATA * ch, CHAR_DATA * victim ) );
void		remove_mirror		args( (CHAR_DATA * ch, CHAR_DATA * victim, int how_many ) );
void		all_artefact_from_char  args( (CHAR_DATA *ch)  );
bool 		form_check( CHAR_DATA *ch, CHAR_DATA *vch, OBJ_DATA* obj, sh_int sn );
int 		get_backstabbed_dam args ( ( CHAR_DATA * ch, CHAR_DATA * victim, int old_dam ) );
bool		check_illusion		args( (CHAR_DATA *ch ) );
int			check_critical_hit args( ( CHAR_DATA *ch, CHAR_DATA *victim, int *diceroll, int *dt, OBJ_DATA *wield,int *base_dam, int old_dam, int *twohander_fight_style, int *weapon_mastery, bool *critical ) );
int         one_hit_check_mastery args ( ( CHAR_DATA *ch, CHAR_DATA *victim, int gsn, int check_improve_level ) );
bool check_noremovable_affects args ( ( AFFECT_DATA *aff ) );

int calculate_final_damage( CHAR_DATA *ch, CHAR_DATA *victim, int dam);

/* GADANIE MOBOW */
void mob_chat_fight args ( ( CHAR_DATA *ch ) );

/* part droper */
void part_dropper_wraper args ( ( CHAR_DATA *ch ) ) ;

/*
 * Control the fights going on.
 * Called periodically by update_handler.
 */

bool strip_illusion( CHAR_DATA *ch )
{
	OBJ_DATA *obj, *obj_next;

	if ( !IS_NPC(ch) ) return FALSE;

	act( "$n rozp�ywa si� w powietrzu.", ch, NULL, NULL, TO_ROOM );

	/* itemy co je trzymala iluzja opadaja na ziemie*/
	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	{
		OBJ_NEXT_CONTENT( obj, obj_next );

		if ( IS_OBJ_STAT( obj, ITEM_NODROP ) )
        {
			continue;
        }

		if ( obj->wear_loc != WEAR_NONE )
        {
			unequip_char( ch, obj );
        }

		obj_from_char( obj );
		act( "$p upada na ziemi�.", ch, obj, NULL, TO_ROOM );
		obj_to_room( obj, ch->in_room );

		if ( IS_OBJ_STAT( obj, ITEM_MELT_DROP ) )
		{
			switch ( number_range( 1, 3 ) )
			{
				case 1:
					act( "$p rozp�ywa si� w powietrzu.", ch, obj, NULL, TO_ROOM );
					break;
				case 2:
					act( "$p znika bez �ladu.", ch, obj, NULL, TO_ROOM );
					break;
				default:
					act( "$p rozp�ywa si� pozostawiaj�c tylko chmur� dymu.", ch, obj, NULL, TO_ROOM );
					break;
			}
		}
		else if ( obj->item_type == ITEM_POTION && ( obj->material == 11 || obj->material == 15 ) )
		{
			switch ( number_range( 1, 3 ) )
			{
				case 1:
					act( "$p toczy si� przez chwil�, po czym rozpryskuj� si� na kwa�eczki.", ch, obj, NULL, TO_ROOM );
					break;
				case 2:
					act( "$p p�ka z hukiem i rozpryskuje si� dooko�a.", ch, obj, NULL, TO_ROOM );
					break;
				default:
					act( "$p rozpryskuje si� na kawa�eczki.", ch, obj, NULL, TO_ROOM );
					break;
			}
		}
	}

    long int copper = money_count_copper_all( ch );
	if ( copper > 0 )
	{
		obj_to_room( create_money( ch->copper, ch->silver, ch->gold, ch->mithril ), ch->in_room );
		if ( copper > 1 ) act( "Kupka monet upada na ziemi�.", ch, NULL, NULL, TO_ROOM );
		else act( "Jedna moneta upada na ziemi�.", ch, NULL, NULL, TO_ROOM );
        money_reset_character_money( ch );
	}

	return TRUE;
}

void violence_update( void )
{
	CHAR_DATA * ch;
	CHAR_DATA *ch_next;
	CHAR_DATA *victim;
	AFFECT_DATA *paf, *af, *af_do_counta, *dazzling;
	AFFECT_DATA *paf_next;
	OBJ_DATA *obj, *weapon, *obj_next;
	CHAR_DATA *tch;
	bool found_impale = FALSE;
	int af_count = 0;

	/* petla dla normali */
	for ( ch = char_list; ch != NULL; ch = ch_next )
	{
		ch_next	= ch->next;

		/* nie walczymy z supermobem ! */
		DEBUG_INFO( "viol_update:supermob_stuff" );
		if ( IS_NPC( ch ) && ch->fighting && ch->pIndexData->vnum == 3 )
		{
			stop_fighting( ch, TRUE );
			ch->hunting = NULL;
			ch->hit = get_max_hp(ch);
			continue;
		}
		
		//stripowanie sneak_invis, moby zorientowaly sie ze gracz stoi w lokacji
        EXT_REMOVE_BIT( ch->affected_by, AFF_SNEAK_INVIS ); 

		/* moby z summonow znikaja, jak nie maja pana, wylaczajac sytuacje - summowali a nie mogli kontrolowac
		   (act aggressive) albo jak stracili pana po czarku repayment (czy huntuje pc)*/
		DEBUG_INFO( "viol_update:mob_noexp_extract" );
   	//Drake: By� tu jeszcze && !IS_AFFECTED(ch,AFF_CHARM), ale po wyj�ciu z gry w karczmie charm na mobie nie znika, wi�c i sam mob nie znika.
		if ( IS_NPC( ch ) && EXT_IS_SET( ch->act, ACT_NO_EXP ) && !EXT_IS_SET( ch->act, ACT_AGGRESSIVE )  && !ch->master && ch->pIndexData->vnum != MOB_VNUM_FETCH )
		{
			if( ( ch->hunting && !IS_NPC( ch->hunting ) ) || ( ch->fighting && !IS_NPC( ch->fighting ) ) )
				;
			else
			{
				stop_fighting( ch, TRUE );

				if( IS_NPC(ch) && ( ch->pIndexData->vnum == MOB_VNUM_GOLEM ||
							ch->pIndexData->vnum == MOB_VNUM_LIVEOAK ||
							ch->pIndexData->vnum == MOB_VNUM_BARKGUARD ||
							ch->pIndexData->vnum == MOB_VNUM_WILDTHORN ||
							ch->pIndexData->vnum == MOB_VNUM_BEAR ||
							ch->pIndexData->vnum == MOB_VNUM_WOLF ||

							EXT_IS_SET( ch->act, ACT_RAISED ) ||
							( ch->pIndexData->vnum > 10 && ch->pIndexData->vnum < 20 ) || //ghule, ghasty
							( ch->pIndexData->vnum > 3000 && ch->pIndexData->vnum < 3064 ) ||
							( ch->pIndexData->vnum > 10904 && ch->pIndexData->vnum < 10910 ) ) )//rozne moby z summonow
				{
					for ( obj = ch->carrying; obj != NULL; obj = obj_next )
					{
						OBJ_NEXT_CONTENT( obj, obj_next );

						if ( IS_OBJ_STAT( obj, ITEM_NODROP ) || IS_OBJ_STAT( obj, ITEM_ROT_DEATH ))
							continue;

						if ( obj->wear_loc != WEAR_NONE )
							unequip_char( ch, obj );

						obj_from_char( obj );

						act( "$p upada na ziemie.", ch, obj, NULL, TO_ROOM );
						obj_to_room( obj, ch->in_room );

						if ( IS_OBJ_STAT( obj, ITEM_MELT_DROP ) )
						{
							switch ( number_range( 1, 3 ) )
							{
								case 1:
									act( "$p rozp�ywa si� w powietrzu.", ch, obj, NULL, TO_ROOM );
									break;
								case 2:
									act( "$p znika bez �ladu.", ch, obj, NULL, TO_ROOM );
									break;
								default:
									act( "$p rozp�ywa si� pozostawiaj�c tylko chmur� dymu.", ch, obj, NULL, TO_ROOM );
									break;
							}
							/*artefact*/
							if ( is_artefact( obj ) ) extract_artefact( obj );
							if ( obj->contains ) extract_artefact_container( obj );
							extract_obj( obj );
						}
						else if ( obj->item_type == ITEM_POTION && ( obj->material == 11 || obj->material == 15 ) )
						{
							switch ( number_range( 1, 3 ) )
							{
								case 1:
									act( "$p toczy si� przez chwil�, po czym rozpryskuj� si� na kwa�eczki.", ch, obj, NULL, TO_ROOM );
									break;
								case 2:
									act( "$p p�ka z hukiem i rozpryskuje si� dooko�a.", ch, obj, NULL, TO_ROOM );
									break;
								default:
									act( "$p rozpryskuje si� na kawa�eczki.", ch, obj, NULL, TO_ROOM );
									break;
							}
							/*artefact*/
							if ( is_artefact( obj ) ) extract_artefact( obj );
							if ( obj->contains ) extract_artefact_container( obj );
							extract_obj( obj );
						}
					}

                    long int copper = money_count_copper_all( ch );
					if ( copper > 0 )
					{
						obj_to_room( create_money( ch->copper, ch->silver, ch->gold, ch->mithril ), ch->in_room );
                        append_file_format_daily
                            (
                             ch,
                             MONEY_LOG_FILE,
                             "-> S: %d %d (%d), B: %d %d (%d) - quit yes",
                             copper, 0, -copper,
                             ch->bank,
                             ch->bank,
                             0
                            );
						if ( copper > 1 ) act( "Kupka monet upada na ziemi�.", ch, NULL, NULL, TO_ROOM );
						else act( "Jedna moneta upada na ziemi�.", ch, NULL, NULL, TO_ROOM );
                        money_reset_character_money( ch );
					}
				}

				if ( ch->in_room )
				{
					if( EXT_IS_SET( ch->act, ACT_RAISED ) )
						act( "$n rozsypuje si� w proch.", ch, NULL, NULL, TO_ROOM );
					else
							strip_illusion( ch );
				}
				extract_char( ch, TRUE );
			}
		}

		DEBUG_INFO( "viol_update:mirrorfall_strip" );
		if( is_affected(ch,gsn_mirrorfall ) )
			affect_strip(ch, gsn_mirrorfall);

		DEBUG_INFO( "viol_update:impale" );
		if( is_affected(ch,gsn_glorious_impale) )
		{
			if( affect_find(ch->affected,gsn_glorious_impale)->modifier == 0 )
			{
				if( !get_eq_char(ch,WEAR_WIELD) || get_eq_char(ch,WEAR_WIELD)->value[0] != WEAPON_SPEAR )
				{
					affect_strip(ch,gsn_glorious_impale);
				}
			}
			else
			{
				for( tch = ch->in_room->people; tch; tch = tch->next_in_room )
				{
					if( tch->fighting == ch && is_affected(tch,gsn_glorious_impale) && get_eq_char(tch,WEAR_WIELD) && get_eq_char(tch,WEAR_WIELD)->value[0] == WEAPON_SPEAR )
						++found_impale;
				}
				if( !found_impale )
					affect_strip(ch,gsn_glorious_impale);
			}
		}

		DEBUG_INFO( "viol_update:overload" );
		/* opadanie na ziemi�, je�li jest si� przeci��onym */
		if ( (!ch->mount && get_carry_weight( ch ) > can_carry_w( ch ) && IS_AFFECTED(ch, AFF_FLYING) && !IS_NPC (ch)) ||
				( !ch->mount && get_carry_weight( ch ) > can_carry_w( ch ) && IS_AFFECTED(ch, AFF_FLOAT) && !IS_NPC (ch)))
		{
			act("$n pod wp�ywem ci�aru opada na ziemi�.",ch,NULL,NULL,TO_ROOM);
			send_to_char( "Pod wp�ywem obci��enia opadasz na ziemi�.\n\r", ch );
			affect_strip(ch, gsn_fly);
			affect_strip(ch, gsn_float);
		}

		DEBUG_INFO( "viol_update:regen" );
		/* regeneracja co runde walki */
		if ( IS_AFFECTED( ch, AFF_REGENERATION ) && ch->hit < get_max_hp(ch) )
		{
			AFFECT_DATA * paf_regen;
			int min, max, level;
            bool check;

			paf_regen = affect_find( ch->affected, gsn_regenerate );

			if ( paf_regen )
				level = paf_regen->level;
			else
				level = 2;

			switch ( level )
			{
				case 1:
				case 2:
					min = 1;max = 2;break;
				case 3:
					min = 1;max = 3;break;
				case 4:
					min = 2;max = 4;break;
				case 5:
					min = 2;max = 5;break;
				case 6:
					min = 3;max = 6;break;
				case 7:
					min = 3;max = 7;break;
				case 8:
					min = 4;max = 7;break;
				default:
					min = 4;max = 8;break;
			}
			update_pos( ch );
		}

		//sprawdzenie heat/chill metal
		for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
		{
			if ( obj->wear_loc == WEAR_NONE )
				continue;

			if ( affect_find( obj->affected, gsn_heat_metal ) )
			{
				int dam = 0;

				switch ( obj->wear_loc )
				{
					case WEAR_FEET: dam = dice( 1, 4 );	break;
					case WEAR_LEGS: dam = dice( 2, 4 );	break;
					case WEAR_ARMS: dam = dice( 1, 6 );	break;
					case WEAR_HANDS: dam = dice( 1, 4 );	break;
					case WEAR_BODY: dam = dice( 2, 6 );	break;
					case WEAR_HEAD: dam = dice( 2, 4 );	break;
					case WEAR_SHIELD: dam = dice( 1, 4 );	break;
					case WEAR_WIELD: dam = dice( 1, 4 );	break;
					case WEAR_SECOND: dam = dice( 1, 4 );	break;
					default: break;
				}

				switch ( obj->wear_loc )
				{
					case WEAR_FEET:
					case WEAR_LEGS:
					case WEAR_HANDS:
					case WEAR_ARMS:
						act( "$p pal� twoj� sk�r�!", ch, obj, NULL, TO_CHAR );
						act( "$p pali sk�r� $z!", ch, obj, NULL, TO_ROOM );
						break;
					default:
						act( "$p pali twoj� sk�r�!", ch, obj, NULL, TO_CHAR );
						act( "$p pali sk�r� $z!", ch, obj, NULL, TO_ROOM );
						break;
				}

				spell_damage( ch, ch, dam, gsn_heat_metal, DAM_FIRE , FALSE );

				if (!IS_AFFECTED(ch, AFF_PARALYZE))
				{
					if ( !IS_OBJ_STAT(obj, ITEM_NOREMOVE) && IS_NPC( ch ))
					{
						if (obj->item_type == ITEM_ARMOR )
							act("$n j�czy i w wielkim po�piechu zdejmuje $h!", ch,obj,NULL,TO_ROOM);
						else if (obj->item_type == ITEM_WEAPON || obj->item_type == ITEM_SHIELD )
							act("$p wypada $x z d�oni parz�c j�.", ch,obj,NULL,TO_ROOM);
						unequip_char( ch, obj );
					}
				}
				else if (number_percent() > 50)
				{
					for ( af = ch->affected; af != NULL; af = af->next )
					{
						if ( af->bitvector == &AFF_PARALYZE )
						{
							affect_strip(ch,af->type);
							if (!IS_NPC( ch ) )
								send_to_char( "Z powodu b�lu, odzyskujesz kontrol� nad swoim cia�em\n\r", ch );
						}
					}
					act("Z powodu b�lu, $n odzyskuje kontrol� nad swoim cia�em.", ch, NULL, NULL, TO_ROOM);
				}

				if ( !ch->in_room )
					return;
			}
			else if ( affect_find( obj->affected, gsn_chill_metal ) )
			{
				int dam = 0;

				switch ( obj->wear_loc )
				{
					case WEAR_FEET: dam = dice( 1, 4 );	break;
					case WEAR_LEGS: dam = dice( 2, 4 );	break;
					case WEAR_ARMS: dam = dice( 1, 6 );	break;
					case WEAR_HANDS: dam = dice( 1, 4 );	break;
					case WEAR_BODY: dam = dice( 2, 6 );	break;
					case WEAR_HEAD: dam = dice( 2, 4 );	break;
					case WEAR_SHIELD: dam = dice( 1, 4 );	break;
					case WEAR_WIELD: dam = dice( 1, 4 );	break;
					case WEAR_SECOND: dam = dice( 1, 4 );	break;
					default: break;
				}

				switch ( obj->wear_loc )
				{
					case WEAR_FEET:
					case WEAR_LEGS:
					case WEAR_HANDS:
					case WEAR_ARMS:
						act( "$p mro�� na ko�� twoje cia�o!", ch, obj, NULL, TO_CHAR );

						if ( ch->sex == 2 )
							act( "$p mro�� na ko�� jej cia�o!", ch, obj, NULL, TO_ROOM );
						else
							act( "$p mro�� na ko�� jego cia�o!", ch, obj, NULL, TO_ROOM );
						break;

					default:
						act( "$p mrozi na ko�� twoje cia�o!", ch, obj, NULL, TO_CHAR );

						if ( ch->sex == 2 )
							act( "$p mrozi na ko�� jej cia�o!", ch, obj, NULL, TO_ROOM );
						else
							act( "$p mrozi na ko�� jego cia�o!", ch, obj, NULL, TO_ROOM );
						break;
				}


				spell_damage( ch, ch, dam, gsn_heat_metal, DAM_COLD, FALSE );

				if ( !ch->in_room )
					return;
			}
		}

		DEBUG_INFO( "viol_update:underwater_update" );
        /* dziwne rzeczy pod woda*/

        if
            (
             !IS_NPC( ch ) &&
             ch->in_room &&
             IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_UNDERWATER ) &&
             !IS_AFFECTED( ch, AFF_WATERBREATH ) &&
             !is_undead( ch ) &&
             !IS_IMMORTAL( ch )
            )
        {
			int oxygen;

			ch->pcdata->oxygen++;

			oxygen = UMAX( 0, ch->pcdata->oxygen - 3 - ( get_curr_stat_deprecated( ch, STAT_CON ) / 6 ) );

			//jesli zejdzie,to oxygen = 6, zeby ladny tekst wyskoczyl
			if ( ch->hit + 11 < oxygen * get_max_hp(ch) / 10 )
				oxygen = 6;

			switch ( oxygen )
			{
				case 0:
					break;
				case 1:
					send_to_char( "Czujesz narastaj�cy b�l w klatce piersiowej, zaczyna brakowa� ci powietrza.\n\r", ch );
					switch ( ch->sex )
					{
						case 0:
							act( "$n sinieje na twarzy, chyba brakuje temu powietrza.", ch, NULL, NULL, TO_ROOM );
							break;
						case 1:
							act( "$n sinieje na twarzy, chyba brakuje mu powietrza.", ch, NULL, NULL, TO_ROOM );
							break;
						default :
							act( "$n sinieje na twarzy, chyba brakuje jej powietrza.", ch, NULL, NULL, TO_ROOM );
							break;
					}
					break;
				case 2:
				case 3:
					send_to_char( "Czujesz narastaj�cy b�l w klatce piersiowej, zaczyna brakowa� ci powietrza.\n\r", ch );
					switch ( ch->sex )
					{
						case 0:
							act( "$n sinieje na twarzy, chyba brakuje temu powietrza.", ch, NULL, NULL, TO_ROOM );
							break;
						case 1:
							act( "$n sinieje na twarzy, chyba brakuje mu powietrza.", ch, NULL, NULL, TO_ROOM );
							break;
						default :
							act( "$n sinieje na twarzy, chyba brakuje jej powietrza.", ch, NULL, NULL, TO_ROOM );
							break;
					}
					damage( ch, ch, oxygen * get_max_hp(ch) / 10, TYPE_UNDEFINED, DAM_NONE, FALSE );
					break;
				case 4:
					send_to_char( "B�l w klatce narasta, musisz jak najszybciej si� wynurzy�.\n\r", ch );
					switch ( ch->sex )
					{
						case 0:
							act( "$n sinieje na twarzy, chyba brakuje temu powietrza.", ch, NULL, NULL, TO_ROOM );
							break;
						case 1:
							act( "$n sinieje na twarzy, chyba brakuje mu powietrza.", ch, NULL, NULL, TO_ROOM );
							break;
						default :
							act( "$n sinieje na twarzy, chyba brakuje jej powietrza.", ch, NULL, NULL, TO_ROOM );
							break;
					}
					damage( ch, ch, oxygen * get_max_hp(ch) / 10, TYPE_UNDEFINED, DAM_NONE, FALSE );
					break;
				case 5:
				case 6:
					send_to_char( "Przed oczami zaczynaj� ci przep�ywa� kolorowe plamy, ogarnia ci� potworne zm�czenie.\n\r", ch );
					switch ( ch->sex )
					{
						case 0:
							act( "$n sinieje na twarzy, chyba brakuje temu powietrza.", ch, NULL, NULL, TO_ROOM );
							break;
						case 1:
							act( "$n sinieje na twarzy, chyba brakuje mu powietrza.", ch, NULL, NULL, TO_ROOM );
							break;
						default :
							act( "$n sinieje na twarzy, chyba brakuje jej powietrza.", ch, NULL, NULL, TO_ROOM );
							break;
					}
					damage( ch, ch, oxygen * get_max_hp(ch) / 10, TYPE_UNDEFINED, DAM_NONE, FALSE );
					break;
				default:
					send_to_char( "Przestajesz cokolwiek widzie�, nie czujesz ju� b�lu. To juz chyba koniec.\n\r", ch );
					act( "$n zamyka oczy i nieruchomieje. Z $s otwartych ust unosza si� ku g�rze b�belki powietrza.", ch, NULL, NULL, TO_ROOM );
					damage( ch, ch, get_max_hp(ch) + 12, TYPE_UNDEFINED, DAM_NONE, FALSE );
					break;
			}

			if ( !ch->in_room )
				continue;
		}

		DEBUG_INFO( "viol_update:hallucinations" );
		if ( IS_AFFECTED( ch, AFF_HALLUCINATIONS_POSITIVE ) && number_range(1,20) == 1 )
		{
			if ( ch->fighting != NULL )
			{
				switch( number_range(1,10))
				{
					case 1:
						send_to_char( "Dok�adnie widzisz ka�dy ruch przeciwnika.\n\r", ch );
						act("$n wyprowadza nadwyraz trafione ciosy.",ch,NULL,NULL,TO_ROOM);
						break;
					case 2:
						act("$N puchnie i rusza si� coraz wolniej.",ch,NULL,ch->fighting,TO_CHAR);
						act("$n bije coraz celniej.",ch,NULL,ch->fighting,TO_ROOM);
						break;
					case 3:
						act("�wiat�o wok� $Z blednie, widzisz dok�adnie ka�dy $S kontur.",ch,NULL,ch->fighting,TO_CHAR);
						act("$n nienaturalnie wytrzeszcza oczy.",ch,NULL,ch->fighting,TO_ROOM);
						break;
					case 4:
						act("$N ci� nienawidzi!",ch,NULL,ch->fighting,TO_CHAR);
						act("$n mamrocze co� pod nosem.",ch,NULL,ch->fighting,TO_ROOM);
						break;
					case 5:
						act("Czujesz narastaj�c� nienawi�� do $Z.",ch,NULL,ch->fighting,TO_CHAR);
						act("$n warczy gro�nie.",ch,NULL,ch->fighting,TO_ROOM);
						break;
					case 6:
						act("$N to tw�j najwi�kszy wr�g!",ch,NULL,ch->fighting,TO_CHAR);
						act("$n coraz bardziej anga�uje si� w walk�.",ch,NULL,ch->fighting,TO_ROOM);
						break;
					case 7:
						act("Z ust $Z wyp�ywa czarna ma�.",ch,NULL,ch->fighting,TO_CHAR);
						act("$n cofa si� z wyrazem obrzydzenia na twarzy.",ch,NULL,ch->fighting,TO_ROOM);
						break;
					case 8:
						act("$N �mieje si� z ciebie podle.",ch,NULL,ch->fighting,TO_CHAR);
						act("$n wpada w sza�!",ch,NULL,ch->fighting,TO_ROOM);
						break;
					case 9:
						act("Orientujesz si�, �e $N to dzikie zwierz�!",ch,NULL,ch->fighting,TO_CHAR);
						act("$n be�kocze co� g�o�no.",ch,NULL,ch->fighting,TO_ROOM);
						break;
					case 10:
						act("Po swej prawicy zauwa�asz siebie walcz�c<&ego/�/e> z $V.",ch,NULL,ch->fighting,TO_CHAR);
						act("$n nerwowo przewraca oczami.",ch,NULL,ch->fighting,TO_ROOM);
						break;
				}
			}
			else
			{
				switch(number_range(1,8))
				{
					case 1:
						if ( !IS_OUTSIDE( ch ) || IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_NOWEATHER ) )
						{
							send_to_char( "Niebo przykrywaj� czarne burzowe chmury.\n\r", ch );
							act("$n spogl�da ze strachem na niebo.",ch,NULL,NULL,TO_ROOM);
							break;
						}
					case 2:
						send_to_char( "Dooko�a ciebie zaczynaj� pe�za� jadowite w�e!\n\r", ch );
						act("$n patrzy na ziemi� i wydaje z siebie j�k obrzydzenia.",ch,NULL,NULL,TO_ROOM);
						break;
					case 3:
						send_to_char( "Powoli zaczynasz zapada� si� w ziemi�.\n\r", ch );
						act("$n mamrocze co� pod nosem tupi�c g�o�no.",ch,NULL,NULL,TO_ROOM);
						break;
					case 4:
						if (( weapon = get_eq_char( ch, WEAR_WIELD )) != NULL )
						{
							act("$p warczy na ciebie!", ch, weapon, NULL, TO_CHAR );
							act("$n patrzy przera�onym wzrokiem przed siebie.",ch,NULL,NULL,TO_ROOM);
							break;
						}
					case 5:
						victim = get_random_char( ch, FALSE );
						if ( victim != NULL )
						{
							act("Spogl�dasz na $C i czujesz nienawi��!", ch, NULL, victim, TO_CHAR );
							act("$n patrzy na ciebie z nienawi�ci�.", ch, NULL, victim, TO_VICT );
							act("$n patrzy z nienawi�ci� na $C.", ch, NULL, victim, TO_NOTVICT);
							break;
						}
					case 6:
						send_to_char( "Robi si� zimno i nieprzyjemnie.\n\r", ch );
						act("$n trz�sie si� z zimna.",ch,NULL,NULL,TO_ROOM);
						break;
					case 7:
						send_to_char( "Nagle stajesz w p�omieniach! Palisz si�!\n\r", ch );
						act("$n rzuca si� na ziemie i zaczyna si� turla�.",ch,NULL,NULL,TO_ROOM);
						break;
					case 8:
						send_to_char( "Zastanawiasz si� nad sensem swojego marnego �ycia.\n\r", ch );
						act("$n patrzy przed siebie bezbarwnym wzrokiem.",ch,NULL,NULL,TO_ROOM);
						break;
				}
			}
		}
		else if ( IS_AFFECTED( ch, AFF_HALLUCINATIONS_NEGATIVE ) && number_range(1,20) == 1 )
		{
			if ( ch->fighting != NULL )
			{
				switch(number_range(1,8))
				{
					case 1:
						act("$N znika i pojawia si� w innym miejscu! Ale �mieszne!", ch, NULL, ch->fighting, TO_CHAR );
						act("$n nagle wybucha �miechem.",ch,NULL,NULL,TO_ROOM);
						break;
					case 2:
						act("$N wyrastaj� dodatkowe r�ce!", ch, NULL, ch->fighting, TO_CHAR );
						act("$n kiwa g�ow� na boki �miej�c si� g�upio.",ch,NULL,NULL,TO_ROOM);
						break;
					case 3:
						act("$N dwoi si� i troi!", ch, NULL, ch->fighting, TO_CHAR );
						act("$n rozgl�da si� z wyrazem zdenerwowania na twarzy.",ch,NULL,NULL,TO_ROOM);
						break;
					case 4:
						if ( !IS_OUTSIDE( ch ) || IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_NOWEATHER ) )
						{
							act("$N robi przysiad, wyskok i znika gdzie� mi�dzy ob�okami. Nagle orientujesz si�, �e stoi obok ciebie!", ch, NULL, ch->fighting, TO_CHAR );
							act("$n z wyrazem os�upienia gapi si� na niebo.",ch,NULL,NULL,TO_ROOM);
							break;
						}
					case 5:
						act("Zabawa z $V jest taka przyjemna!", ch, NULL, ch->fighting, TO_CHAR );
						act("$n klaszcze w d�onie szczerz�c z�by.",ch,NULL,NULL,TO_ROOM);
						break;
					case 6:
						act("$N skacze jak wiewi�rka!", ch, NULL, ch->fighting, TO_CHAR );
						act("$n kiwa g�ow� w g�r� i w d�.",ch,NULL,NULL,TO_ROOM);
						break;
					case 7:
						act("$n robi salto w powietrzu.", ch, NULL, ch->fighting, TO_CHAR );
						act("$n patrzy przed siebie z niedowierzaniem.",ch,NULL,NULL,TO_ROOM);
						break;
					case 8:
						act("$N nagle zapada si� pod ziemi�, aby po chwili wyskoczy� w innym miejscu.", ch, NULL, ch->fighting, TO_CHAR );
						act("$n rozgl�da si� po ziemi.",ch,NULL,NULL,TO_ROOM);
						break;
				}
			}
			else
			{
				switch(number_range(1,8))
				{
					case 1:
						if ( !IS_OUTSIDE( ch ) || IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_NOWEATHER ) )
						{
							send_to_char( "Na niebie przelatuje pi�kny szkar�atno fioletowy smok!\n\r", ch );
							act("$n spogl�da z g�upawym u�miechem na niebo.",ch,NULL,NULL,TO_ROOM);
							break;
						}
					case 2:
						send_to_char( "Dooko�a ciebie zaczynaj� biega� weso�o �liczne czerwone zaj�czki!\n\r", ch );
						act("$n patrzy na ziemi� i �mieje si� nie wiadomo z czego.",ch,NULL,NULL,TO_ROOM);
						break;
					case 3:
						send_to_char( "Nagle zaczynasz si� wznosi� nad okolic�! Ty latasz! Jak pi�knie!\n\r", ch );
						act("$n zaczyna macha� r�koma na�laduj�c lot ptaka.",ch,NULL,NULL,TO_ROOM);
						break;
					case 4:
						if (( weapon = get_eq_char( ch, WEAR_WIELD )) != NULL )
						{
							act("$p u�miecha si� weso�o!", ch, weapon, NULL, TO_CHAR );
							act("$n patrzy bezbarwnym wzrokiem przed siebie usmiechaj�c si� szeroko.",ch,NULL,NULL,TO_ROOM);
							break;
						}
					case 5:
						victim = get_random_char( ch, FALSE );
						if ( victim != NULL )
						{
							switch (victim->sex)
							{
								case 0:
									act("Spogl�dasz na $C i widzisz jakie jest kochane!", ch, NULL, victim, TO_CHAR );
									break;
								case 1:
									act("Spogl�dasz na $C i widzisz jaki jest kochany!", ch, NULL, victim, TO_CHAR );
									break;
								default:
									act("Spogl�dasz na $C i widzisz jaka jest kochana!", ch, NULL, victim, TO_CHAR );
									break;
							}

							act("$n u�miecha si� dziwnie patrz�c na ciebie.", ch, NULL, victim, TO_VICT );
							act("$n u�miecha si� dziwnie patrz�c na $C.", ch, NULL, victim, TO_NOTVICT);
							break;
						}
					case 6:
						send_to_char( "Jest ci ciep�o i przyjemnie!\n\r", ch );
						act("$n u�miecha si� b�ogo.",ch,NULL,NULL,TO_ROOM);
						break;
					case 7:
						send_to_char( "Dooko�a ciebie wsz�dzie le�� srebrne monety! B�dziesz bogat<&y/a/e>!\n\r", ch );
						act("$n rzuca si� na ziemie i zaczyna czego� szuka� �miej�c si� g�o�no.",ch,NULL,NULL,TO_ROOM);
						break;
					case 8:
						send_to_char( "Jeste� ma�ym, czerwonym kwiatuszkiem na �rodku zielonej ��ki.\n\r", ch );
						act("$n patrzy przed siebie bezbarwnym wzrokiem.",ch,NULL,NULL,TO_ROOM);
						break;
				}
			}
		}

		DEBUG_INFO( "viol_update:demon_aura" );
		if( is_affected( ch, gsn_demon_aura ) )
		{
			for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
			{
                if (tch->position > POS_SLEEPING)
                {
                    if( tch == ch)
                    {
                        if ( number_range(1,50) == 1 )
                        {
                            switch( number_range(1,6))
                            {
                                case 1:
                                    send_to_char( "Czujesz jak otaczaj�ca ci� aura oblepia w przyjemny, ciep�y spos�b ca�e twoje cia�o, masz wra�enie, jakby� by<&�/�a/�o> zanurzon<&y/a/e> w zbiorniku wype�nionym �wie�� krwi�.\n\r", tch );
                                    break;
                                case 2:
                                    send_to_char( "Otaczaj�ca ci� aura s�abnie na chwil�, tylko po to aby otoczy� ci� ze zdwojon� moc�.\n\r",tch);
                                    break;
                                case 3:
                                    send_to_char( "Wydaje ci si�, �e dzi�ki otaczaj�cej ci� aurze widzisz wi�cej i lepiej ni� normalnie. Tak�e pozosta�e zmys�y wyostrzy�y si� nieznacznie.\n\r", tch );
                                    break;
                                case 4:
                                    send_to_char( "Krew przep�ywaj�ca przez twoje cia�o za spraw� aury zaczyna p�yn�� szybciej. Twoje �y�y pulsuj�, a ca�e cia�o zaczyna wr�cz emanowa� ciep�em.\n\r", tch );
                                    break;
                                case 5:
                                    send_to_char( "Przez chwile twoje oczy zalewa krew... nie, to tylko z�udzenie. Jednak ogl�danie �wiata przez krwaw� zas�on� by�o mi�ym wydarzeniem.\n\r", tch );
                                    break;
                                case 6:
                                    send_to_char( "Czujesz jak twoje mi�nie zaczynaj� same si� napina�, pulsowa� delikatnie i wida� napr�one na nich �y�y.\n\r", tch );
                                    break;
                            }
                        }
                    }
                    else if( is_same_group(ch,tch) && number_range(1,40) == 1 )
                    {
                        if ( IS_EVIL(tch))
                        {
                            switch(number_range(1,5))
                            {
                                case 1:
                                    if(ch->sex == 2)
                                        act( "Widzisz jak $c otacza pi�kna krwistoczerwona, demoniczna aura, kt�ra czyni j� majestatyczn� i gro�n� postaci�.", ch, NULL, tch, TO_VICT );
                                    else
                                        act( "Widzisz jak $c otacza pi�kna krwistoczerwona, demoniczna aura, kt�ra czyni go majestatyczn� i gro�n� postaci�.", ch, NULL, tch, TO_VICT );
                                    break;
                                case 2:
                                    act( "Z podziwem przygl�dasz si� przez chwil� $x, a w szczeg�lno�ci $s fascynuj�cej, pulsuj�cej aurze z�a.", ch, NULL, tch, TO_VICT );
                                    break;
                                case 3:
                                    if(ch->sex == 2)
                                        act( "Widz�c mroczn� aur� otaczaj�c� $c jeste� pew<&ien/na/ne>, �e posz<&ed�by�/�aby�/�oby�> za ni� na koniec �wiata!", ch, NULL, tch, TO_VICT );
                                    else
                                        act( "Widz�c mroczn� aur� otaczaj�c� $c jeste� pew<&ien/na/ne>, �e posz<&ed�by�/�aby�/�oby�> za nim na koniec �wiata!", ch, NULL, tch, TO_VICT );
                                    break;
                                case 4:
                                    if(ch->sex == 2)
                                        act( "Wydaje ci si�, �e $n wygl�da jak prawdziwa demonica, za spraw� otaczaj�cej j� mrocznej aury, rozpalaj�cej w twoim sercu z�owieszcze ognie z�a.", ch, NULL, tch, TO_VICT );
                                    else
                                        act( "Wydaje ci si�, �e $n wygl�da jak prawdziwy demon, za spraw� otaczaj�cej go mrocznej aury, rozpalaj�cej w twoim sercu z�owieszcze ognie z�a.", ch, NULL, tch, TO_VICT );
                                    break;
                                case 5:
                                    act( "Czujesz jak lepkie wici demonicznej aury $z oplataj� ci� delikatnie i dziel� si� z tob� cz�ci� swojej mocy! ", ch, NULL, tch, TO_VICT );
                                    break;
                            }
                        }
                        else if ( IS_GOOD(tch))
                        {
                            switch(number_range(1,5))
                            {
                                case 1:
                                    if(ch->sex == 2)
                                        act( "Z przera�eniem obserwujesz jak $n smieje si� z�owieszczo delektuj�c si� otaczaj�c� j� krwistoczerwon� aur�!", ch, NULL, tch, TO_VICT );
                                    else
                                        act( "Z przera�eniem obserwujesz jak $n smieje si� z�owieszczo delektuj�c si� otaczaj�c� go krwistoczerwon� aur�!", ch, NULL, tch, TO_VICT );
                                    break;
                                case 2:
                                    act( "Czujesz jak jakie� z�e moce odrywaj� si� od otaczaj�cej $c po�wiaty i pr�buj� zapu�ci� si� wewn�trz twojej duszy. Czujesz jak wype�niaj� ci� ohydne uczucia trwogi, zw�tpienia, strachu, gniewu i nienawi�ci! Brrr...", ch, NULL, tch, TO_VICT );
                                    break;
                                case 3:
                                    if(ch->sex == 2)
                                        act( "Mdli ci� na sam� my�l o tym, �e obok ciebie stoi $n, przes�czona krwi� i �mierci�, otoczona demoniczn� aur� b�lu, strachu i nienawi�ci.", ch, NULL, tch, TO_VICT );
                                    else
                                        act( "Mdli ci� na sam� my�l o tym, �e obok ciebie stoi $n, przes�czony krwi� i �mierci�, otoczony demoniczn� aur� b�lu, strachu i nienawi�ci.", ch, NULL, tch, TO_VICT );
                                    break;
                                case 4:
                                    if(ch->sex == 2)
                                        act( "Wydaje ci si� jakby $n patrzy�a na ciebie z ogromn� pogard� i nienawisci�, dok�adnie tak, jakby chcia�a przenikn�� i wyssa� z ciebie dusz�. A mo�e to tylko z�udzenie?", ch, NULL, tch, TO_VICT );
                                    else
                                        act( "Wydaje ci si� jakby $n patrzy� na ciebie z ogromn� pogard� i nienawisci�, dok�adnie tak, jakby chcia� przenikn�� i wyssa� z ciebie dusz�. A mo�e to tylko z�udzenie?", ch, NULL, tch, TO_VICT );
                                    break;
                                case 5:
                                    if(ch->sex == 2)
                                        act( "Zaczynasz w�tpi� w to, czy po��czenie si� z $v by�o dobrym pomys�em. Otaczaj�ca j� aura jest bez w�tpienia przesi�kni�ta �mierci� i z�em, a ona sama nie mo�e mie� �adnych dobrych intencji.", ch, NULL, tch, TO_VICT );
                                    else
                                        act( "Zaczynasz w�tpi� w to, czy po��czenie si� z $v by�o dobrym pomys�em. Otaczaj�ca go aura jest bez w�tpienia przesi�kni�ta �mierci� i z�em, a on sam nie mo�e mie� �adnych dobrych intencji.", ch, NULL, tch, TO_VICT );
                                    break;
                            }
                        }
                        else
                        {
                            switch(number_range(1,5))
                            {
                                case 1:
                                    if(ch->sex == 2)
                                        act( "Spogl�dasz oboj�tnie na emanuj�c� mrokiem $z, kt�ra z powazn�, acz wype�nion� gniewem twarz� wydaje si� by� naprawd� pot�n� osob�.", ch, NULL, tch, TO_VICT );
                                    else
                                        act( "Spogl�dasz oboj�tnie na emanuj�cego mrokiem $z, kt�ry z powazn�, acz wype�nion� gniewem twarz� wydaje si� by� naprawd� pot�n� osob�.", ch, NULL, tch, TO_VICT );
                                    break;
                                case 2:
                                    if(ch->sex == 2)
                                        act( "Zaczynasz w�tpi�, czy $n jest dobr� towarzyszk� wyprawy, bowiem otaczaj�ca j� krwawa aura �wiadczy o tym, �e raczej nie mo�na jej ufa�.", ch, NULL, tch, TO_VICT );
                                    else
                                        act( "Zaczynasz w�tpi�, czy $n jest dobrym towarzyszem wyprawy, bowiem otaczaj�ca go krwawa aura �wiadczy o tym, �e raczej nie mo�na mu ufa�.", ch, NULL, tch, TO_VICT );
                                    break;
                                case 3:
                                    act( "Bez wi�kszego wra�enia obserwujesz k�tem oka otaczaj�c� $c kristoczerwon�, demoniczn� aur�.", ch, NULL, tch, TO_VICT );
                                    break;
                                case 4:
                                    act( "Czujesz jak otaczaj�ca $c demoniczna aura zapuszcza si� wewn�trz twojej duszy, pr�buj�c zasia� w niej strach i zw�tpienie.", ch, NULL, tch, TO_VICT );
                                    break;
                                case 5:
                                    if(ch->sex == 2)
                                        act( "Z gniewem obserwujesz jak $n z rado�ci� delektuje si� otaczaj�c� j� demoniczn� aur�. Masz wra�enie jakby jakie� pierwotne instynkty pr�bowa�y przej�� kontrol� nad twoim umys�em, jednak udaje ci si� zachowa� spok�j.", ch, NULL, tch, TO_VICT );
                                    else
                                        act( "Z gniewem obserwujesz jak $n z rado�ci� delektuje si� otaczaj�c� go demoniczn� aur�. Masz wra�enie jakby jakie� pierwotne instynkty pr�bowa�y przej�� kontrol� nad twoim umys�em, jednak udaje ci si� zachowa� spok�j.", ch, NULL, tch, TO_VICT );
                                    break;
                            }
                        }
                    }
                    continue;
                }
            }
        }

		DEBUG_INFO( "viol_update:check_fight" );
		/* jesli nie walczy albo nie jest w roomie: zdejmij flage
		   plus sprawdz czy dalej walczy-zdejmij berserk*/
		if ( ( victim = ch->fighting ) == NULL || ch->in_room == NULL )
		{
			DEBUG_INFO( "viol_update:check_fight(1)" );
			strip_fight_affects( ch );
			DEBUG_INFO( "viol_update:check_fight(2)" );
			continue;
		}

		DEBUG_INFO( "viol_update:check_fight(3)" );
		if ( victim )
		{
			if ( ch->position == POS_STANDING )
				ch->position = POS_FIGHTING;
			if ( victim->position == POS_STANDING )
				victim->position = POS_FIGHTING;
		}

		DEBUG_INFO( "viol_update:check_fight(4)" );
		if ( ( IS_NPC( ch ) && EXT_IS_SET( ch->act, ACT_MONK ) ) ||
				( !IS_NPC( ch ) && ch->class == CLASS_MONK ) )
			ch->num_attacks = 0;

		DEBUG_INFO( "viol_update:confusion" );
		//confusion
		paf = affect_find( ch->affected, gsn_confusion );
		if ( paf && paf->duration > 0 && number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) < number_range( 0, paf->modifier ) )
		{
			switch ( number_bits( 1 ) )
			{
				case 0:
					victim = get_random_char( ch, FALSE );
					if ( victim && !is_safe( ch, victim ) && ch != victim && victim != ch->fighting )
					{
						paf->duration = UMAX( 0, paf->duration - 1 ); paf->rt_duration = 0;
						ch->fighting = victim;
						send_to_char( "Czujesz si� troch� zdezorientowan<&y/a/e> i zaczynasz atakowa� kogo� innego!\n\r", ch );
						act( "$n przez chwil� wygl�da na $t i po chwili zaczyna atakowa� kogo� innego!",
								ch, ch->sex == 2 ? "zdezorientowan�" : "zdezorientowanego", victim, TO_NOTVICT );
					}
					else
						victim = ch->fighting;
					break;
				case 1:
					if ( ( weapon = get_eq_char( ch, WEAR_WIELD ) ) != NULL && !IS_OBJ_STAT( weapon, ITEM_NOREMOVE ) )
					{
						paf->duration = UMAX( 0, paf->duration - 1 ); paf->rt_duration = 0;
						send_to_char( "Czujesz si� troch� zdezorientowan<&y/a/e> i wypuszczasz bro� z d�oni!\n\r", ch );
						act( "$n przez chwil� wygl�da na $t i po chwili wypuszcza bro� z d�oni!",
								ch, ch->sex == 2 ? "zdezorientowan�" : "zdezorientowanego", victim, TO_NOTVICT );
						obj_from_char( weapon );
						if ( IS_OBJ_STAT( weapon, ITEM_NODROP ) || IS_OBJ_STAT( weapon, ITEM_INVENTORY ) )
							obj_to_char( weapon, ch );
						else
							obj_to_room( weapon, ch->in_room );
					}
					break;
			}
		}

		DEBUG_INFO( "viol_update:berserk" );
		if( !IS_NPC(ch) && is_affected(ch,gsn_berserk ) && get_skill(ch,gsn_berserk) < number_range(-10,40) )
		{
			if( number_range(0,2*(get_curr_stat_deprecated(ch,STAT_INT) + get_curr_stat_deprecated(ch,STAT_WIS))) == 0 )
			{
                bool allow_attack = TRUE;
				victim = get_random_char( ch, FALSE );
                if(
                        victim
                        && ch->fighting != victim
                        && victim != ch
                        && !is_affected( victim, gsn_sanctuary )
                  )
                {
                    /**
                     * moby ucz�ce oraz ci�ko pracuj�ce nie mog� by� tak traktowane, prawda?
                     */
                    if (
                            IS_NPC( victim)
                            && (
                                EXT_IS_SET( victim->act, ACT_PRACTICE )
                                || victim->pIndexData->pShop != NULL
                               )
                       )
                    {
                        allow_attack = FALSE;
                    }
                    if ( allow_attack )
                    {
                        act( "Niespodziewanie, w twojej duszy budzi si� jaki� prymitywny instynkt. Pragniesz mordowa�, zabija�, sieka�! Rzucasz si� z impetem na stoj�c� najbli�ej ciebie osob�.", ch, NULL, victim, TO_CHAR );
                        act( "Nagle dzieje si� co� niespodziewanego. $n zaczyna sapa� dziko, zdaje si� p�on�� jak�� prymitwyn� si��, i nagle rzuca si� na ciebie!", ch, NULL, victim, TO_VICT );
                        act( "Nie mo�esz uwierzy� w�asnym oczom! $n zaczyna zachowywa� si� nad wyraz dziko i w morderczym szale rzuca si� na $C.", ch, NULL, victim, TO_NOTVICT );
                        stop_fighting( ch, FALSE );
                        multi_hit( ch, victim, TYPE_UNDEFINED );
                    }
				}
			}
		}

		DEBUG_INFO( "viol_update:check_fight2" );
		if ( ( victim = ch->fighting ) == NULL )
		{
			if ( !still_fighting( ch ) )
			{
				strip_fight_affects( ch );
				continue;
			}
		}

		DEBUG_INFO( "viol_update:strip_dazzling_flash" );
		if( victim && is_affected( victim, gsn_dazzling_flash) && affect_find( victim->affected, gsn_dazzling_flash)->level == 0 &&
			ch && is_affected( ch, gsn_dazzling_flash) &&
			!IS_AFFECTED( victim, AFF_PERFECT_SENSES) && ( !IS_AFFECTED(victim,AFF_DETECT_INVIS) || IS_AFFECTED(ch,AFF_NONDETECTION)) )
		{
			for( dazzling = ch->affected ; dazzling; dazzling = dazzling->next )
			{
				if( dazzling && dazzling->type == gsn_dazzling_flash && dazzling->level == 1 && affect_find( victim->affected, gsn_dazzling_flash)->modifier == dazzling->modifier )
				{
					affect_remove( ch, dazzling );
					affect_strip( victim, gsn_dazzling_flash );
					act("$N materializuje si� nagle.", victim, NULL, ch, TO_CHAR );
					act("Dostrzegasz zaskoczenie w oczach $z, twoje zakl�cie przesta�o dzia�a�.", victim, NULL, ch, TO_VICT );
					break;
				}
			}
		}

		DEBUG_INFO( "viol_update:multi_hit" );
		if ( !can_move( ch ) && !can_move( victim ) )
			stop_fighting( ch, FALSE );

		if ( IS_AWAKE( ch ) && ch->in_room == victim->in_room )
		{
			if ( !IS_SET( ch->fight_data, FIGHT_DISARM ) &&
					!IS_SET( ch->fight_data, FIGHT_PARRY ) &&
					!IS_SET( ch->fight_data, FIGHT_CASTING ) &&
					!IS_SET( ch->fight_data, FIGHT_DODGE ) )
				multi_hit( ch, victim, TYPE_UNDEFINED );

			if ( ch->in_room && ch->wait < 4 )
				ch->wait = 4;
		}
		else
			stop_fighting( ch, FALSE );


		DEBUG_INFO( "viol_update:check_fight3" );
		if ( ( victim = ch->fighting ) == NULL )
		{
			if ( !still_fighting( ch ) )
			{
				strip_fight_affects( ch );
				continue;
			}
		}
		/*
		 * Fun for the whole family!
		 */


		DEBUG_INFO( "viol_update:check_assist" );
		check_assist( ch, victim );

		DEBUG_INFO( "viol_update:percent_hp_trig" );
		if ( IS_NPC( ch ) )
		{
			if ( HAS_TRIGGER( ch, TRIG_FIGHT ) )
				mp_percent_trigger( ch, victim, NULL, NULL, &TRIG_FIGHT );
			if ( HAS_TRIGGER( ch, TRIG_HPCNT ) )
				mp_hprct_trigger( ch, victim );
		}

		DEBUG_INFO( "viol_update:obj_fight_trig" );
		op_fight_trigger( ch );


		DEBUG_INFO( "viol_update:check_affect" );
		for ( paf = ch->affected; paf != NULL; paf = paf_next )
		{
			paf_next	= paf->next;

			if ( skill_table[ paf->type ].round_hour )
			{
				if ( paf->duration > 0 )
				{
					paf->duration--;
				}
				else if ( paf->duration < 0 )
					;
				else
				{
					if ( paf_next == NULL
							|| paf_next->type != paf->type
							|| paf_next->duration > 0 )
					{
						if ( paf->type > 0 && skill_table[ paf->type ].msg_off )
						{
							for( af_do_counta = ch->affected; af_do_counta; af_do_counta = af_do_counta->next )
							{
								if ( af_do_counta->type == paf->type )
									++af_count;
							}
/*							if( af_count == 1 ) narazie wywalam bo nie dziala i nie mam pojecia dlaczego
							{*/
								send_to_char( skill_table[ paf->type ].msg_off, ch );
								send_to_char( "\n\r", ch );
//							}
						}
						if ( paf->type == gsn_glorious_impale )
						{
							if( paf->modifier )
							{
								if( ch->fighting && is_affected( ch->fighting, gsn_glorious_impale ) && IS_AFFECTED( ch, AFF_PARALYZE ) && get_eq_char( ch->fighting,WEAR_WIELD) && get_eq_char( ch->fighting, WEAR_WIELD)->value[0] == WEAPON_SPEAR )
								{
									act( "$n gwa�townie wyrywa $h z twojego cia�a powi�kszaj�c jeszcze ran�.", ch->fighting, get_eq_char(ch->fighting,WEAR_WIELD), ch, TO_VICT );
									act( "$n gwa�townie wyrywa $h z cia�a $Z powi�kszaj�c jeszcze ran�.", ch->fighting, get_eq_char(ch->fighting,WEAR_WIELD), ch, TO_NOTVICT );
								}
							}
							else
							{
								if( ch->fighting && is_affected( ch->fighting, gsn_glorious_impale ) && IS_AFFECTED( ch->fighting, AFF_PARALYZE ) && get_eq_char( ch,WEAR_WIELD) && get_eq_char( ch, WEAR_WIELD)->value[0] == WEAPON_SPEAR )
								{
									act( "Gwa�townie wyrywasz $h z cia�a $Z powi�kszaj�c jeszcze ran�.", ch, get_eq_char( ch,WEAR_WIELD), ch->fighting, TO_CHAR );
									damage( ch, ch->fighting, 2*(dice(get_eq_char(ch,WEAR_WIELD)->value[1],get_eq_char(ch,WEAR_WIELD)->value[2])+get_eq_char(ch,WEAR_WIELD)->value[6]+GET_DAMROLL( ch, get_eq_char(ch,WEAR_WIELD))+GET_SKILL_DAMROLL( ch, get_eq_char(ch,WEAR_WIELD))),gsn_glorious_impale, DAM_PIERCE, FALSE );
								}
							}
						}
					}
					affect_remove( ch, paf );
				}
			}
		}
	}


	DEBUG_INFO( "viol_update:other_loop" );
	/* petla dla parowcow i reszty*/
	for ( ch = char_list; ch != NULL; ch = ch_next )
	{
		ch_next	= ch->next;

		DEBUG_INFO( "viol_update:check_casting" );

		if ( IS_SET( ch->fight_data, FIGHT_CASTING ) )
		{
			REMOVE_BIT( ch->fight_data, FIGHT_CASTING );
			continue;
		}

		DEBUG_INFO( "viol_update:berserk_rescue" );
		/* rescue automatyczny dla berserkerow nie tankujacych */
		check_berserk_rescue( ch );

		DEBUG_INFO( "viol_update:check_fight4" );
		if ( ( victim = ch->fighting ) == NULL ||
				ch->in_room == NULL ||
				victim->in_room == NULL ||
				!( IS_SET( ch->fight_data, FIGHT_PARRY ) ||
					IS_SET( ch->fight_data, FIGHT_DISARM ) ||
					IS_SET( ch->fight_data, FIGHT_DODGE ) ) )
		{
			if ( !still_fighting( ch ) )
				strip_fight_affects( ch );

			continue;
		}

		DEBUG_INFO( "viol_update:multi_hit(2)" );
		if ( IS_AWAKE( ch ) && ch->in_room == victim->in_room )
		{
			multi_hit( ch, victim, TYPE_UNDEFINED );

			if ( ch->wait < 4 )
				ch->wait = 4;
		}
		else
			stop_fighting( ch, FALSE );

		DEBUG_INFO( "viol_update:check_fight_last" );
		if ( ( victim = ch->fighting ) == NULL )
		{
			if ( !still_fighting( ch ) )
			{
				strip_fight_affects( ch );
				continue;
			}
		}

		/*
		 * Fun for the whole family!
		 */


		DEBUG_INFO( "viol_update:check_assist(2)" );
		check_assist( ch, victim );

		DEBUG_INFO( "viol_update:percent_hp_trig(2)" );
		if ( IS_NPC( ch ) )
		{
			if ( HAS_TRIGGER( ch, TRIG_FIGHT ) )
				mp_percent_trigger( ch, victim, NULL, NULL, &TRIG_FIGHT );
			if ( HAS_TRIGGER( ch, TRIG_HPCNT ) )
				mp_hprct_trigger( ch, victim );
		}

		DEBUG_INFO( "viol_update:obj_fight_trig" );
		op_fight_trigger( ch );

		DEBUG_INFO( "viol_update:rem_fight_data_flags" );
		REMOVE_BIT( ch->fight_data, FIGHT_PARRY );
		REMOVE_BIT( ch->fight_data, FIGHT_DISARM );
		REMOVE_BIT( ch->fight_data, FIGHT_DODGE );
	}

	return;
}

/* for auto assisting */
void check_assist( CHAR_DATA *ch, CHAR_DATA *victim )
{
	CHAR_DATA * rch, *rch_next;

	if ( !ch || !victim )
		return;

	for ( rch = ch->in_room->people; rch != NULL; rch = rch_next )
	{
		rch_next = rch->next_in_room;
		DEBUG_INFO( "void:check_assist(1)" );

		if ( number_bits(1) == 0 )
			continue;

		DEBUG_INFO( "void:check_assist(2)" );
		if ( IS_NPC( rch) && IS_AWAKE( rch )
				&& rch->fighting == NULL && can_move( rch )
				&& !IS_AFFECTED( rch, AFF_CHARM ) && mob_can_attack( rch ) )
		{
			DEBUG_INFO( "void:check_assist(3)" );
			if ( !IS_NPC( ch ) && EXT_IS_SET( rch->off_flags, OFF_ASSIST_PLAYERS ) )
			{
				DEBUG_INFO( "void:check_assist(4)" );
				if ( !IS_NPC( victim ) )
				{
					continue;
				}
				rch->position = POS_STANDING;
				switch(number_range(0,2))
				{
					case 1:
						do_function( rch, &do_emote, "w��cza si� do walki!" );
						break;
					case 2:
						do_function( rch, &do_emote, "anga�uje si� w wir walki!" );
						break;
					default:
						do_function( rch, &do_emote, "przy��cza si� do walki!" );
						break;
				}
				multi_hit( rch, victim, TYPE_UNDEFINED );
				continue;
			}
			else if ( IS_NPC( ch ) && !IS_AFFECTED( ch, AFF_CHARM ) )
			{
				DEBUG_INFO( "void:check_assist(5)" );
				if ( EXT_IS_SET( rch->off_flags, OFF_ASSIST_ALL )
						|| ( rch->group && rch->group == ch->group )
						|| ( GET_RACE( rch ) == GET_RACE( ch ) && EXT_IS_SET( rch->off_flags, OFF_ASSIST_RACE ) )
						|| ( EXT_IS_SET( rch->off_flags, OFF_ASSIST_ALIGN ) && IS_SAME_ALIGN( rch, ch ) )
						|| ( rch->pIndexData->vnum == ch->pIndexData->vnum && EXT_IS_SET( rch->off_flags, OFF_ASSIST_VNUM ) ) )
				{
					CHAR_DATA * vch;
					CHAR_DATA *target;
					int number;


					target = NULL;
					number = 0;
					DEBUG_INFO( "void:check_assist(6)" );
					for ( vch = ch->in_room->people; vch; vch = vch->next )
					{
						if ( can_see( rch, vch )
								&& is_same_group( vch, victim )
								&& number_range( 0, number ) == 0 )
						{
							target = vch;
							number++;
						}
					}

					DEBUG_INFO( "void:check_assist(7)" );
					if ( target != NULL )
					{

						rch->position = POS_STANDING;
						switch(number_range(0,2))
						{
							case 1:
								do_function( rch, &do_emote, "w��cza si� do walki!" );
								break;
							case 2:
								do_function( rch, &do_emote, "anga�uje si� w wir walki!" );
								break;
							default:
								do_function( rch, &do_emote, "przy��cza si� do walki!" );
								break;
						}
						multi_hit( rch, target, TYPE_UNDEFINED );
					}
				}
			}
		}
	}
}

/*
 * Do one group of attacks.
 */
void monk_multi_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
	if ( ch->position < POS_SITTING || !can_move( ch ) || ch->wait_char )
		return;
	DEBUG_INFO( "multi_hit:one_hit" );

	for ( ; ch->num_attacks < max_monk_hits( ch ); ch->num_attacks++ )
	{
		one_hit_monk( ch, victim, dt );

		if ( ch->fighting != victim )
			return;
	}


	/* haste daje 50% na dodatkowy atak z prawej reki */
	if ( (IS_AFFECTED( ch, AFF_HASTE ) && number_percent() < 50 ) || is_affected(ch, 300 ) )
		one_hit_monk( ch, victim, dt );

	return;
}

/*Do one group of attacks*/
void multi_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    AFFECT_DATA *aff, *abide, *abide2;
    CHAR_DATA *tch;
    OBJ_DATA * wield, * second, * spear;
    char msg[MAX_STRING_LENGTH];
    bool stop = FALSE, check, check_dual_wielding = FALSE;
    int vertical_skill, weapon_mastery, crowd = 0;
    int cleave_count, remembered_hp = victim->hit; //do rozplatania, jezeli cos nie zrani to sie zatrzymuje
    int speed = 0, speed_multiplier = 100, weapon_skill, i = 1;
    /*Bazowa szybko�� w walce jest 0 (definiuje dodatkowe ataki).
      Bazowny mno�nik pr�dko�ci to 100 (czyli 100%). */

    // Na wszelki wypadek ustawiamy pierwszy znak na 0
    msg[0] = '\0';

    if ( ch->position < POS_SITTING || !can_move( ch ) || ch->wait_char )
    {
        return;
    }
    /**
     * moby ucz�ce oraz ci�ko pracuj�ce nie mog� by� tak traktowane, prawda?
     */
    if (
            IS_NPC( victim)
            && (
                EXT_IS_SET( victim->act, ACT_PRACTICE )
                || victim->pIndexData->pShop != NULL
               )
       )
    {
        return;
    }

    //rzut na confusion
    aff = affect_find( ch->affected, gsn_confusion );
    if ( aff && aff->duration > 0 && number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > number_range( 0, aff->modifier ) )
    {
        switch(number_range(0,3))
        {
            case 1:
                send_to_char( "Czujesz si� dziwnie, bardzo dziwnie, w�a�ciwie nie wiesz o co chodzi.\n\r", ch );
                break;
            case 2:
                send_to_char( "Ca�y �wiat staje na g�owie, a ty czujesz si� nieco dziwnie.\n\r", ch );
                break;
            case 3:
                send_to_char( "Nagle tracisz orientacje i nie wiesz co jest czym.\n\r", ch );
                break;
            default:
                send_to_char( "Czujesz si� troch� zdezorientowan<&y/a/e>.\n\r", ch );
                break;
        }
        aff->duration = UMAX( 0, aff->duration - 1 ); aff->rt_duration = 0;
        return;
    }

    DEBUG_INFO( "multi_hit:monk_hit" );

    if ( ( IS_NPC( ch ) && EXT_IS_SET( ch->act, ACT_MONK ) ) ||
            ( !IS_NPC( ch ) && ch->class == CLASS_MONK ) )
    {
        monk_multi_hit( ch, victim, dt );
        return;
    }

    if ( IS_NPC( ch ) )
    {
        mob_chat_fight( ch );
        mob_hit( ch, victim, dt );
        return;
    }

    DEBUG_INFO( "multi_hit:glorious_impale" );
    if( is_affected( ch, gsn_glorious_impale ) )
    {
        if( !ch->fighting )
        {
            affect_strip( ch, gsn_glorious_impale );
        }
        else if ( !IS_AFFECTED( ch->fighting, AFF_PARALYZE ) || !is_affected( ch->fighting, gsn_glorious_impale ) )
        {
            act( "Gwa�townie wyrywasz $h z cia�a $Z powi�kszaj�c jeszcze ran�.", ch, get_eq_char( ch,WEAR_WIELD), ch->fighting, TO_CHAR );
            act( "$n gwa�townie wyrywa $h z twojego cia�a powi�kszaj�c jeszcze ran�.", ch, get_eq_char( ch,WEAR_WIELD), ch->fighting, TO_VICT );
            act( "$n gwa�townie wyrywa $h z cia�a $Z powi�kszaj�c jeszcze ran�.", ch, get_eq_char( ch,WEAR_WIELD), ch->fighting, TO_NOTVICT );
            affect_strip( ch->fighting, gsn_glorious_impale );
            damage( ch, ch->fighting, 2*(dice(get_eq_char(ch,WEAR_WIELD)->value[1],get_eq_char(ch,WEAR_WIELD)->value[2])+get_eq_char(ch,WEAR_WIELD)->value[6]+GET_DAMROLL( ch, get_eq_char(ch,WEAR_WIELD))+GET_SKILL_DAMROLL( ch, get_eq_char(ch,WEAR_WIELD))),gsn_glorious_impale, DAM_PIERCE, FALSE );
        }
        else
            return;
    }

    abide = affect_find( ch->affected, gsn_weapon_wrench );

    DEBUG_INFO( "multi_hit:trick_cyclone" );
    if( get_eq_char( ch, WEAR_WIELD ) && get_eq_char( ch, WEAR_SECOND ) && check_trick( ch, victim, SN_TRICK_CYCLONE ) && !abide)
        return;

    spear = get_eq_char(ch, WEAR_WIELD );
    DEBUG_INFO( "multi_hit:trick_dragon_strike" );
    if( spear && spear->item_type == ITEM_WEAPON && spear->value[0] == WEAPON_SPEAR && IS_WEAPON_STAT(spear,WEAPON_TWO_HANDS) && check_trick( ch, victim, SN_TRICK_DRAGON_STRIKE ) && !abide)
        return;

    spear = get_eq_char(ch, WEAR_WIELD );
    DEBUG_INFO( "multi_hit:trick_ravaging_orb" );
    if( spear && spear->item_type == ITEM_WEAPON && spear->value[0] == WEAPON_FLAIL && IS_WEAPON_STAT(spear,WEAPON_TWO_HANDS) && check_trick( ch, victim, SN_TRICK_RAVAGING_ORB ) && !abide )
        return;

    DEBUG_INFO( "multi_hit:one_hit" );

    if( IS_SET( ch->fight_data, FIGHT_HTH_CHARGE ))
        REMOVE_BIT( ch->fight_data, FIGHT_HTH_CHARGE );
    else
    {
        if( !abide || abide->modifier == -1 || (abide->modifier == 1 && abide->level == 51 ) )
            one_hit( ch, victim, dt, FALSE );
        else
        {
            for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
            {
                if( (abide2 = affect_find(tch->affected, gsn_weapon_wrench ) ) != NULL && abide2->modifier != 1 && tch != ch )
                {
                    print_char( ch, "Pr�bujesz zada� cios %s %s, ale %s owini�ty wok� twej broni uniemo�lwia ci to.\n\r", victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr );
                    print_char( victim, "%s pr�buje zada� ci cios %s, ale %s owini�ty wok� %s broni uniemo�lwia %s to.\n\r", ch->name, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                    sprintf( msg, "%s pr�buje zada� cios %s %s, ale %s owini�ty wok� %s broni uniemo�lwia %s to.", ch->name, victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                    act( msg, ch, NULL, victim, TO_NOTVICT );
                }
            }
            act( "Nie jeste� w stanie wykona� ataku $j.", ch, get_eq_char( ch, WEAR_WIELD ), NULL, TO_CHAR );
        }
    }

    /* jesli z pierwszego ataku zszedl, koniec */
    if ( ch->fighting != victim )
        return;

    if( is_affected( ch, gsn_glorious_impale ) )
    {
        if( !ch->fighting || !IS_AFFECTED( ch->fighting, AFF_PARALYZE ) || !is_affected( ch->fighting, gsn_glorious_impale ) )
            affect_strip( ch, gsn_glorious_impale );
        else
            return;
    }

    DEBUG_INFO( "multi_hit:dualwield_hit" );
    check = FALSE;
    if ( get_eq_char ( ch, WEAR_SECOND ) && get_eq_char(ch,WEAR_SECOND)->item_type == ITEM_WEAPON &&
            (
             check
             || ch->class == CLASS_THIEF
             || ch->class == CLASS_WARRIOR
             || ch->class == CLASS_PALADIN
             || ch->class == CLASS_BARD
             || ch->class == CLASS_BLACK_KNIGHT
             || ch->class == CLASS_BARBARIAN
             || ch->class == CLASS_SHAMAN
             ||
             (
              ch->class == CLASS_DRUID
              && get_eq_char( ch, WEAR_WIELD)
              && get_eq_char( ch, WEAR_WIELD)->value[0] == WEAPON_CLAWS
              && get_eq_char( ch,WEAR_SECOND)->value[0] == WEAPON_CLAWS
             )
            )
       )
    {
        if( !abide || abide->modifier == 0 || (abide->modifier == 1 && abide->level != 51 ) )
            one_hit( ch, victim, dt, TRUE );
        else
        {
            for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
            {
                if( (abide2 = affect_find(tch->affected, gsn_weapon_wrench ) ) != NULL && abide2->modifier != 1 && tch != ch )
                {
                    print_char( ch, "Pr�bujesz zada� cios %s %s, ale %s owini�ty wok� twej broni uniemo�lwia ci to.\n\r", victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr );
                    print_char( victim, "%s pr�buje zada� cios cios %s, ale %s owini�ty wok� %s broni uniemo�lwia %s to.\n\r", ch->name, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                    sprintf( msg, "%s pr�buje zada� cios %s %s, ale %s owini�ty wok� %s broni uniemo�lwia %s to.", ch->name, victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                    act( msg, ch, NULL, victim, TO_NOTVICT );
                }
            }
            act( "Nie jeste� w stanie wykona� ataku $j.", ch, get_eq_char( ch, WEAR_SECOND ), NULL, TO_CHAR );
        }
    }

    if ( ch->fighting != victim )
        return;

    if( is_affected( ch, gsn_glorious_impale ) )
    {
        if( !ch->fighting || !IS_AFFECTED( ch->fighting, AFF_PARALYZE ) || !is_affected( ch->fighting, gsn_glorious_impale ) )
            affect_strip( ch, gsn_glorious_impale );
        else
            return;
    }

    DEBUG_INFO( "multi_hit:vertical_slash" );

    /**
     * wezmij bronie
     */
    wield = get_eq_char(ch,WEAR_WIELD);
    second = get_eq_char(ch,WEAR_SECOND);

    //i tutaj vertical slash black knighta. Wczesniejszy atak musial trafic
    if ( wield && IS_SET( ch->fight_data, FIGHT_VERTICAL_SLASH ) && victim->hit != remembered_hp )
    {
        REMOVE_BIT( ch->fight_data, FIGHT_VERTICAL_SLASH );
        if ( IS_WEAPON_STAT( wield, WEAPON_TWO_HANDS ) &&
                wield->value[0] != WEAPON_DAGGER &&
                wield->value[0] != WEAPON_WHIP &&
                wield->value[0] != WEAPON_MACE &&
                wield->value[0] != WEAPON_STAFF &&
                wield->value[0] != WEAPON_SPEAR &&
                wield->value[0] != WEAPON_FLAIL &&
                wield->value[0] != WEAPON_SHORTSWORD &&
                wield->value[0] != WEAPON_CLAWS )
        {
            vertical_skill = get_skill( ch, gsn_vertical_slash );
            vertical_skill += (get_curr_stat_deprecated(ch,STAT_DEX)-10)*2;
            vertical_skill -= (get_curr_stat_deprecated(victim,STAT_DEX)-10)*2;
            if ( IS_AFFECTED( ch, AFF_SLOW ) ) vertical_skill /= 4;
            if ( IS_AFFECTED( ch, AFF_HASTE ) )	vertical_skill += 10;

            switch ( victim->size )//dla size tiny i giant nigdy nie wejdzie
            {
                case SIZE_SMALL:
                    vertical_skill -= 10;
                    break;
                case SIZE_LARGE:
                    vertical_skill -= 10;
                    break;
                case SIZE_HUGE:
                    vertical_skill -= 25;
                    break;
                default:
                    break;
            }

            if( victim->size == SIZE_TINY )
            {
                if( victim->sex == 2 )
                    act( "$N jest za ma�a, by pr�bowa� na niej takich sztuczek.", ch, NULL, victim, TO_CHAR );
                else
                    act( "$N jest za ma�y, by pr�bowa� na nim takich sztuczek.", ch, NULL, victim, TO_CHAR );
            }
            else if ( victim->size == SIZE_GIANT )
            {
                if( victim->sex == 2 )
                    act( "$N jest zbyt wielka, by pr�bowa� na niej takich sztuczek.", ch, NULL, victim, TO_CHAR );
                else
                    act( "$N jest zbyt wielki, by pr�bowa� na nim takich sztuczek.", ch, NULL, victim, TO_CHAR );
            }
            else if ( number_percent() < vertical_skill || !can_move(victim))
            {
                switch( number_range(1,5))
                {
                    case 1:
                        act( "Po zadaniu ciosu czujesz, �e lekko oszo�omiony przeciwnik nie jest gotowy na �adn� obron�, co w spos�b bezwzgl�dny wykorzystujesz wyprowadzaj�c natychmiast kolejne ci�cie, tym razem pionowo z do�u w g�r�.", ch, wield, victim, TO_CHAR );
                        act( "Cios $z lekko ci� oszo�omi�, co $e w spos�b bezwzgl�dny wykorzystuje wyprowadzaj�c natychmiast kolejne ci�cie, tym razem pionowo z do�u w g�r�.", ch, wield, victim, TO_VICT );
                        act( "Cios $z lekko oszo�omi� $C, co zostaje w spos�b bezwzgl�dny wykorzystane wyprowadzeniem natychmiast kolejnego ci�cie, tym razem pionowo z do�u w g�r�.", ch, wield, victim, TO_NOTVICT );
                        break;
                    case 2:
                        act( "Poprawiasz szybko sw�j chwyt na r�koje�ci $f i momentalnie tniesz ponownie z do�u w g�r�.", ch, wield, victim, TO_CHAR );
                        act( "$n poprawia szybko sw�j chwyt na r�koje�ci $f i momentalnie tnie ci� ponownie z do�u w g�r�.", ch, wield, victim, TO_VICT );
                        act( "$n poprawia szybko sw�j chwyt na r�koje�ci $f i momentalnie tnie ponownie z do�u w g�r�.", ch, wield, victim, TO_NOTVICT );
                        break;
                    case 3:
                        act( "Wykorzystuj�c impet pierwszego ciosu i natychmiast tniesz po tej samej lini co poprzednio prowadz�c ostrze swojej broni ku g�rze.", ch, wield, victim, TO_CHAR );
                        act( "$n wykorzystuj�c impet pierwszego ciosu natychmiast tnie ci� po tej samej lini co poprzednio prowadz�c ostrze swojej broni ku g�rze.", ch, wield, victim, TO_VICT );
                        act( "$n wykorzystuj�c impet pierwszego ciosu natychmiast tnie po tej samej lini co poprzednio prowadz�c ostrze swojej broni ku g�rze.", ch, wield, victim, TO_NOTVICT );
                        break;
                    case 4:
                        act( "Zaskoczywszy przeciwnika zupe�nie pierwszym ciosem z g�ry do do�u, wyprowadzasz kolejne pot�ne ci�cie w lini pionowej w odwrotnym kierunku.", ch, wield, victim, TO_CHAR );
                        act( "Zaskoczywszy ci� zupe�nie pierwszym ciosem z g�ry do do�u, $n wyprowadza kolejne pot�ne ci�cie w lini pionowej w odwrotnym kierunku.", ch, wield, victim, TO_VICT );
                        act( "Zaskoczywszy $C zupe�nie pierwszym ciosem z g�ry do do�u, $n wyprowadza kolejne pot�ne ci�cie w lini pionowej w odwrotnym kierunku.", ch, wield, victim, TO_NOTVICT );
                        break;
                    case 5:
                        act( "Udaje ci si� po mistrzowsku wykorzysta� sprzyjaj�c� sytuacj� po zadaniu pierwszego ciosu aby w �lad za nim uderzy� po raz kolejny tn�c wroga od st�p do g�owy.", ch, wield, victim, TO_CHAR );
                        act( "$x udaje si� wykorzysta� sprzyjaj�c� sytuacj� po zadaniu pierwszego ciosu aby w �lad za nim uderzy� ci� po raz kolejny tn�c od st�p do g�owy.", ch, wield, victim, TO_VICT );
                        act( "$x udaje si� wykorzysta� sprzyjaj�c� sytuacj� po zadaniu pierwszego ciosu aby w �lad za nim uderzy� $C po raz kolejny tn�c od st�p do g�owy.", ch, wield, victim, TO_NOTVICT );
                        break;
                }

                one_hit( ch, victim, gsn_vertical_slash , FALSE );
                check_improve( ch, victim, gsn_vertical_slash, TRUE, 40 );
            }
            else if( number_percent() > URANGE( 50, get_curr_stat_deprecated(ch,STAT_DEX)*4 + vertical_skill/3, 95) )
            {
                switch( number_range(1,5))
                {
                    case 1:
                        act( "Zaatakowawszy ponownie po wyprowadzeniu pierwszego ciosu stwierdzasz ze zgroz�, �e przeciwnik w ostatniej chwili zd�zy� si� uchyli�. Twoja bro� tnie powietrze, a ty sa<&m/ma/mo> lecisz do przodu na spotkanie gruntu.", ch, wield, victim, TO_CHAR );
                        act( "Wykonujesz g�adki unik przed niezgrabnym atakiem $z, $s bro� tnie powietrze i leci $e do przodu na spotkanie gruntu.", ch, wield, victim, TO_VICT );
                        act( "$N wykonuje g�adki unik przed niezgrabnym atakiem $z, $s bro� tnie powietrze i leci $e do przodu na spotkanie gruntu.", ch, wield, victim, TO_NOTVICT );
                        break;
                    case 2:
                        act( "Pr�bujesz zmieni� chwyt na r�koje�ci swej broni, jednak robisz to tak nieudolnie, �e wypada ci ona z r�k. Szybko pr�bujesz naprawi� sytuacje i przerywasz p�ynny ruch, kt�ry wykonywa�<&e�/a�/o�>, dzi�ki czemu udaje ci si� natychmiast z�apa� $h, ale kosztem utraty r�wnowagi.", ch, wield, victim, TO_CHAR );
                        act( "$n pr�buje zmieni� chwyt na r�koje�ci swej broni, jednak robi to tak nieudolnie, �e wypada ona z $s r�k. Szybko pr�buje $e naprawi� sytuacje i przerywa p�ynny ruch, kt�ry mia� ci pewnie zrobi� krzywd�. Udaje si� $o natychmiast z�apa� $h, ale kosztem utraty r�wnowagi.", ch, wield, victim, TO_VICT );
                        act( "$n pr�buje zmieni� chwyt na r�koje�ci swej broni, jednak robi to tak nieudolnie, �e wypada ona z $s r�k. Szybko pr�buje $e naprawi� sytuacje i przerywa p�ynny ruch, kt�ry mia� pewnie zrobi� krzywd� $X. Udaje si� $o natychmiast z�apa� $h, ale kosztem utraty r�wnowagi.", ch, wield, victim, TO_NOTVICT );
                        break;
                    case 3:
                        act( "Po udanym pierwszym ciosie wydawa�o ci si�, �e zadanie kolejnego momentalnie po pierwszym nie b�dzie trudne. Sztuka ta jednak ci� przeros�a i w trakcie zamachu straci�<&e�/a�/o�> r�wnowag� wywracaj�c si� na ziemie.", ch, wield, victim, TO_CHAR );
                        act( "Po udanym pierwszym ciosie $x wydawa�o si�, �e zadanie ci kolejnego momentalnie po pierwszym nie b�dzie trudne. Na szcz�cie sztuka ta przeros�a $s umiej�tno�ci i w trakcie zamachu r�wnowaga zosta�a stracona, przez co $n le�y teraz na ziemi.", ch, wield, victim, TO_VICT );
                        act( "Po udanym pierwszym ciosie $x wydawa�o si�, �e zadanie kolejnego momentalnie po pierwszym nie b�dzie trudne. Sztuka ta przeros�a jednak $s umiej�tno�ci i w trakcie zamachu r�wnowaga zosta�a stracona, przez co $n le�y teraz na ziemi.", ch, wield, victim, TO_NOTVICT );
                        break;
                    case 4:
                        act( "Kiedy twoje pierwsze ci�cie wyprowadzone znad g�owy przynios�o wyj�tkowo dobry efekt pr�bujesz powt�rzy� cios, tym razem bior�c zamach od do�u. Pechowo jednak zahaczasz ko�cem $f o pod�o�e, co ca�kowicie wyprowadza ci� z rytmu i r�wnowagi. Upadasz na ziemi�.", ch, wield, victim, TO_CHAR );
                        if( ch->sex == 2 )
                        {
                            act( "$n pr�buje powt�rzy� cios, tym razem bior�c zamach od do�u. Na szcz�cie jednak zahacza ko�cem $f o pod�o�e, co ca�kowicie wyprowadza j� z rytmu i r�wnowagi. Upada ona na ziemi�.", ch, wield, victim, TO_VICT );
                            act( "$n pr�buje powt�rzy� cios, tym razem bior�c zamach od do�u, zahacza jednak ko�cem $f o pod�o�e, co ca�kowicie wyprowadza j� z rytmu i r�wnowagi. Upada ona na ziemi�.", ch, wield, victim, TO_NOTVICT );
                        }
                        else
                        {
                            act( "$n pr�buje powt�rzy� cios, tym razem bior�c zamach od do�u. Na szcz�cie jednak zahacza ko�cem $f o pod�o�e, co ca�kowicie wyprowadza go z rytmu i r�wnowagi. Upada on na ziemi�.", ch, wield, victim, TO_VICT );
                            act( "$n pr�buje powt�rzy� cios, tym razem bior�c zamach od do�u, zahacza jednak ko�cem $f o pod�o�e, co ca�kowicie wyprowadza go z rytmu i r�wnowagi. Upada on na ziemi�.", ch, wield, victim, TO_NOTVICT );
                        }
                        break;
                    case 5:
                        act( "Zadawszy pierwszy cios wpadasz w dziki sza�, istny amok, i nie my�l�c zbyt du�o natychmiast chcesz powt�rzy� poprzedni wyczyn, jednak w swym zapami�taniu wypadasz z rytmu i ostatecznie walisz si� na ziemi� jak d�ug<&i/a/ie>.", ch, wield, victim, TO_CHAR );
                        act( "Zadawszy pierwszy cios $n wpada w dziki sza�, istny amok, i nie my�l�c zbyt du�o natychmiast chce powt�rzy� poprzedni wyczyn, na szcz�cie w swym zapami�taniu wypada z rytmu i ostatecznie wali si� na ziemi�.", ch, wield, victim, TO_VICT );
                        act( "Zadawszy pierwszy cios $n wpada w dziki sza�, istny amok, i nie my�l�c zbyt du�o natychmiast chce powt�rzy� poprzedni wyczyn, jednak w swym zapami�taniu wypada z rytmu i ostatecznie wali si� na ziemi�.", ch, wield, victim, TO_NOTVICT );
                        break;
                }

                if ( ch->position > POS_SITTING )
                    ch->position = POS_SITTING;
                check_improve( ch, victim, gsn_vertical_slash, FALSE, 50 );
            }
            else
            {
                switch( number_range(1,5))
                {
                    case 1:
                        act( "Przez moment wydaje ci si�, �e przeciwnik nie zd�zy� pozbiera� si� po pierwszym ciosie i pr�bujesz wyprowadzi� natychmiast kolejny, $E jednak uchyla si� w ostatniej chwili.", ch, wield, victim, TO_CHAR );
                        act( "$x przez moment wydaje si�, �e nie zd�zy�<&e�/a�/o�> pozbiera� si� po pierwszym ciosie i pr�buje wyprowadzi� natychmiast kolejny, ty jednak uchylasz si� bez k�opotu.", ch, wield, victim, TO_VICT );
                        act( "$n pr�buje wyprowadzi� natychmiast kolejny cios, $N jednak uchyla si� bez k�opotu.", ch, wield, victim, TO_NOTVICT );
                        break;
                    case 2:
                        act( "Przepe�nion<&y/a/e> dzikim sza�em po perfekcyjnym pierwszym ci�ciu w euforii starasz si� bezzw�ocznie zaatakowa� po raz kolejny, jednak pud�ujesz haniebnie.", ch, wield, victim, TO_CHAR );
                        act( "Po perfekcyjnym pierwszym ci�ciu $n w euforii stara si� bezzw�ocznie zaatakowa� ci� po raz kolejny, jednak pud�uje haniebnie.", ch, wield, victim, TO_VICT );
                        act( "Po perfekcyjnym pierwszym ci�ciu $n w euforii stara si� bezzw�ocznie zaatakowa� $C po raz kolejny, jednak pud�uje haniebnie.", ch, wield, victim, TO_NOTVICT );
                        break;
                    case 3:
                        act( "Wyprowadziwszy silne ci�cie znad g�owy starasz si� poprawi� sw�j chwyt na r�koje�ci $f, ta jednak wymyka ci si� na chwil� z r�k i tracisz kilka chwil, kt�re mog�<&e�/a�/o�> wykorzysta� na zadanie kolejnego ataku.", ch, wield, victim, TO_CHAR );
                        act( "$n wyprowadziwszy silne ci�cie znad g�owy stara si� poprawi� sw�j chwyt na r�koje�ci $f, ta jednak wymyka si� na chwil� z $s r�k i traci $e kilka chwil, kt�re mog�y zosta� wykorzysta� na zadanie ci kolejnego ciosu.", ch, wield, victim, TO_VICT );
                        act( "$n wyprowadziwszy silne ci�cie znad g�owy stara si� poprawi� sw�j chwyt na r�koje�ci $f, ta jednak wymyka si� na chwil� z $s r�k i traci $e kilka chwil, kt�re mog�y zosta� wykorzysta� na zadanie $X kolejnego ciosu.", ch, wield, victim, TO_NOTVICT );
                        break;
                    case 4:
                        act( "Starasz si� wyprowadzi� kolejny atak po zadaniu pierwszego, jednak przeciwnik przewidzia� twoje zamiary i w sprytny spos�b odskakuje lekko na bok.", ch, wield, victim, TO_CHAR );
                        if( ch->sex != 2 )
                        {
                            act( "$n stara� si� wyprowadzi� kolejny atak po zadaniu pierwszego, jednak przewidzia�<&e�/a�/o�> $s zamiary i w sprytny spos�b odskakujesz lekko na bok.", ch, wield, victim, TO_VICT );
                            act( "$n stara� si� wyprowadzi� kolejny atak po zadaniu pierwszego, jednak $N w sprytny spos�b odskakuje lekko na bok.", ch, wield, victim, TO_NOTVICT );
                        }
                        else
                        {
                            act( "$n stara�a si� wyprowadzi� kolejny atak po zadaniu pierwszego, jednak przewidzia�<&e�/a�/o�> $s zamiary i w sprytny spos�b odskakujesz lekko na bok.", ch, wield, victim, TO_VICT );
                            act( "$n stara�a si� wyprowadzi� kolejny atak po zadaniu pierwszego, jednak $N w sprytny spos�b odskakuje lekko na bok.", ch, wield, victim, TO_NOTVICT );
                        }
                        break;
                    case 5:
                        act( "Po wyprowadzeniu ciosu masz ochot� ponowi� atak, jednak wahasz si� o jedn� chwil� za d�ugo i sprzyjaj�ca sytuacja mija.", ch, wield, victim, TO_CHAR );
                        act( "Po wyprowadzeniu ciosu $n ma ochot� ponowi� atak, jednak waha si� o jedn� chwil� za d�ugo i sprzyjaj�ca sytuacja mija.", ch, wield, victim, TO_VICT );
                        act( "Po wyprowadzeniu ciosu $n ma ochot� ponowi� atak, jednak waha si� o jedn� chwil� za d�ugo i sprzyjaj�ca sytuacja mija.", ch, wield, victim, TO_NOTVICT );
                        break;
                }
                check_improve( ch, victim, gsn_vertical_slash, FALSE, 50 );
            }
        }
    }

    if ( ch->fighting != victim )
        return;

    DEBUG_INFO( "multi_hit: Additional_attacks" );
    // Drake: Generalnie teraz to moja dzia�ka muahahahaha

    /* Drake: Wy�aczam na razie.
    //Tylko klasy kwalifikuj�ce si� mniej lub bardziej pod 'zbrojnych' zamiast 'caster�w'.
    if (
    ch->class == CLASS_THIEF
    || ch->class == CLASS_WARRIOR
    || ch->class == CLASS_PALADIN
    || ch->class == CLASS_BARD
    || ch->class == CLASS_BLACK_KNIGHT
    || ch->class == CLASS_BARBARIAN
    || ch->class == CLASS_SHAMAN )

     */
    //Najpierw idzie umiej�tno�� pos�ugiwania si� broni�
    //Pierwsza r�ka
    if ( !IS_NPC( ch ) )
    {
        DEBUG_INFO( "Additional_attacks: Skill_modifier" );
        if ( get_eq_char ( ch, WEAR_WIELD ) && get_eq_char(ch,WEAR_WIELD)->item_type == ITEM_WEAPON )
        {
            wield = get_eq_char(ch,WEAR_WIELD);
            if (
             (wield->value[0] == WEAPON_SWORD      && ( weapon_skill = get_skill(ch, gsn_sword)      ) > 0 )||
             (wield->value[0] == WEAPON_DAGGER     && ( weapon_skill = get_skill(ch, gsn_dagger)     ) > 0 )||
             (wield->value[0] == WEAPON_SPEAR      && ( weapon_skill = get_skill(ch, gsn_spear)      ) > 0 )||
             (wield->value[0] == WEAPON_MACE       && ( weapon_skill = get_skill(ch, gsn_mace)       ) > 0 )||
             (wield->value[0] == WEAPON_AXE        && ( weapon_skill = get_skill(ch, gsn_axe)        ) > 0 )||
             (wield->value[0] == WEAPON_FLAIL      && ( weapon_skill = get_skill(ch, gsn_flail)      ) > 0 )||
             (wield->value[0] == WEAPON_WHIP       && ( weapon_skill = get_skill(ch, gsn_whip)       ) > 0 )||
             (wield->value[0] == WEAPON_POLEARM    && ( weapon_skill = get_skill(ch, gsn_polearm)    ) > 0 )||
             (wield->value[0] == WEAPON_STAFF      && ( weapon_skill = get_skill(ch, gsn_staff)      ) > 0 )||
             (wield->value[0] == WEAPON_SHORTSWORD && ( weapon_skill = get_skill(ch, gsn_shortsword) ) > 0 )||
             (wield->value[0] == WEAPON_CLAWS      && ( weapon_skill = get_skill(ch, gsn_claws)      ) > 0 )
            )
            {
                speed += weapon_skill /5 ; //Od 0-20 dodatkowego speeda od wyszkolenia broni.
            }
        }

        //Opcjonalnie: Druga r�ka
        if ( get_eq_char ( ch, WEAR_SECOND ) && get_eq_char(ch,WEAR_SECOND)->item_type == ITEM_WEAPON )
        {
            check_dual_wielding = TRUE;
            if (
                    (second->value[0] == WEAPON_SWORD      && ( weapon_skill = get_skill(ch, gsn_sword)      ) > 0 )||
                    (second->value[0] == WEAPON_DAGGER     && ( weapon_skill = get_skill(ch, gsn_dagger)     ) > 0 )||
                    (second->value[0] == WEAPON_SPEAR      && ( weapon_skill = get_skill(ch, gsn_spear)      ) > 0 )||
                    (second->value[0] == WEAPON_MACE       && ( weapon_skill = get_skill(ch, gsn_mace)       ) > 0 )||
                    (second->value[0] == WEAPON_AXE        && ( weapon_skill = get_skill(ch, gsn_axe)        ) > 0 )||
                    (second->value[0] == WEAPON_FLAIL      && ( weapon_skill = get_skill(ch, gsn_flail)      ) > 0 )||
                    (second->value[0] == WEAPON_WHIP       && ( weapon_skill = get_skill(ch, gsn_whip)       ) > 0 )||
                    (second->value[0] == WEAPON_POLEARM    && ( weapon_skill = get_skill(ch, gsn_polearm)    ) > 0 )||
                    (second->value[0] == WEAPON_STAFF      && ( weapon_skill = get_skill(ch, gsn_staff)      ) > 0 )||
                    (second->value[0] == WEAPON_SHORTSWORD && ( weapon_skill = get_skill(ch, gsn_shortsword) ) > 0 )||
                    (second->value[0] == WEAPON_CLAWS      && ( weapon_skill = get_skill(ch, gsn_claws)      ) > 0 )
               )
            {
                speed += weapon_skill /5 ; //Od 0-20 dodatkowego speeda od wyszkolenia broni.
            }
            speed /= 2; //Gdy u�ywamy dw�ch broni to sumujemy szybko�� wyszkolenia obu broni i dzielimy przez dwa.
        }

        //Sprawdzanie dexa - generalnie niezr�czniakom dzi�kujemy
        speed +=  get_curr_stat(ch, STAT_DEX) / 5  ; // do 42 dodatkowego speeda od zr�czno�ci.

        //Sprawdzanie masterki - pierwsza r�ka (max +6)
        if ( get_eq_char ( ch, WEAR_WIELD ) && get_eq_char(ch,WEAR_WIELD)->item_type == ITEM_WEAPON )
        {
            wield = get_eq_char(ch,WEAR_WIELD);
            if (
             (wield->value[0] == WEAPON_SWORD      && ( weapon_mastery = get_skill(ch, gsn_sword_mastery)      ) > 0 )||
             (wield->value[0] == WEAPON_DAGGER     && ( weapon_mastery = get_skill(ch, gsn_dagger_mastery)     ) > 0 )||
             (wield->value[0] == WEAPON_SPEAR      && ( weapon_mastery = get_skill(ch, gsn_spear_mastery)      ) > 0 )||
             (wield->value[0] == WEAPON_MACE       && ( weapon_mastery = get_skill(ch, gsn_mace_mastery)       ) > 0 )||
             (wield->value[0] == WEAPON_AXE        && ( weapon_mastery = get_skill(ch, gsn_axe_mastery)        ) > 0 )||
             (wield->value[0] == WEAPON_FLAIL      && ( weapon_mastery = get_skill(ch, gsn_flail_mastery)      ) > 0 )||
             (wield->value[0] == WEAPON_WHIP       && ( weapon_mastery = get_skill(ch, gsn_whip_mastery)       ) > 0 )||
             (wield->value[0] == WEAPON_POLEARM    && ( weapon_mastery = get_skill(ch, gsn_polearm_mastery)    ) > 0 )||
             (wield->value[0] == WEAPON_STAFF      && ( weapon_mastery = get_skill(ch, gsn_staff_mastery)      ) > 0 )||
             (wield->value[0] == WEAPON_SHORTSWORD && ( weapon_mastery = get_skill(ch, gsn_shortsword_mastery) ) > 0 )
            )
            {
                speed += weapon_mastery / 15; //Masterke mo�emy mie� jedn�, wi�� nie ma potrzeby dzieli�.
            }
        }
        else if ( get_eq_char ( ch, WEAR_SECOND ) && get_eq_char(ch,WEAR_SECOND)->item_type == ITEM_WEAPON )
        {
            second = get_eq_char(ch,WEAR_SECOND);
            if (
             (second->value[0] == WEAPON_SWORD      && ( weapon_mastery = get_skill(ch, gsn_sword)      ) > 0 )||
             (second->value[0] == WEAPON_DAGGER     && ( weapon_mastery = get_skill(ch, gsn_dagger)     ) > 0 )||
             (second->value[0] == WEAPON_SPEAR      && ( weapon_mastery = get_skill(ch, gsn_spear)      ) > 0 )||
             (second->value[0] == WEAPON_MACE       && ( weapon_mastery = get_skill(ch, gsn_mace)       ) > 0 )||
             (second->value[0] == WEAPON_AXE        && ( weapon_mastery = get_skill(ch, gsn_axe)        ) > 0 )||
             (second->value[0] == WEAPON_FLAIL      && ( weapon_mastery = get_skill(ch, gsn_flail)      ) > 0 )||
             (second->value[0] == WEAPON_WHIP       && ( weapon_mastery = get_skill(ch, gsn_whip)       ) > 0 )||
             (second->value[0] == WEAPON_POLEARM    && ( weapon_mastery = get_skill(ch, gsn_polearm)    ) > 0 )||
             (second->value[0] == WEAPON_STAFF      && ( weapon_mastery = get_skill(ch, gsn_staff)      ) > 0 )||
             (second->value[0] == WEAPON_SHORTSWORD && ( weapon_mastery = get_skill(ch, gsn_shortsword) ) > 0 )
            )
            {
                speed += weapon_mastery / 20;
            }
        }

        //Dodatek dla szamanow pod wp�ywem dzia�ania Ancestor's Fury (max +12)
        if (ch->class == CLASS_SHAMAN && IS_AFFECTED ( ch, AFF_ANCESTORS_FURY))
        {
            speed +=  get_skill(ch, gsn_invoke_spirit)/8;
        }

        if ( IS_AFFECTED( ch, AFF_SLOW ) )
        {
            speed /= 2;
        }

        // Kara dla 'caster�w', �eby nie bili za dobrze.
        if (
                ch->class == CLASS_MAG
                || ch->class == CLASS_CLERIC
                || ch->class == CLASS_DRUID
                )
        {
            speed /= 2;
        }

        // Lekka kara dla z�odzieji. Ot, �eby nie mieli za dobrze.
        if ( ch->class == CLASS_THIEF )
        {
            speed -= speed/8;
        }

        // Przewr�ceni s� wolniejsi.
        if ( ch->position == POS_SITTING )
        {
            speed -= speed/3;
        }

        // Ludzie z tarczami s� wolniejsi.
        if ( get_eq_char( victim, WEAR_SHIELD ) != NULL )
        {
            speed -= speed/10;
        }

        /**
         * gruba dopalka od haste'a
         */
        if ( is_affected( ch, skill_lookup( "haste" ) ) )
        {
            speed *= 3;
            speed /= 2;
        }

        speed_multiplier = 100;

        DEBUG_INFO( "Additional_attacks: weight_modifier" );
        //Waga broni/12 zmniejsza szybko�� o 1%, ka�dy punkt si�y powy�ej 18 zwi�ksza szybko�� o 1%.
        if ( wield && wield->item_type == ITEM_WEAPON )
        {
            DEBUG_INFO( "Additional_attacks: weight_modifier: 1" );
            speed_multiplier -= wield->weight / 12;
            if ( second && second->item_type == ITEM_WEAPON )
            {
                DEBUG_INFO( "Additional_attacks: weight_modifier: 2" );
                speed_multiplier -= second->weight/10;
            }
        }
        speed_multiplier += get_curr_stat( ch, STAT_STR ) / 10;
        DEBUG_INFO( "Additional_attacks: weight_modifier: 5" );

        //Lets Start this Opera Sh*t! - odpalamy p�tle

        speed = ( speed * UMIN(speed_multiplier, 100) ) / 100;

        if ( ch->pcdata->wiz_conf & W6 )
        {
            print_char( ch, "Speed - po odliczeniu: %d\n\r", speed );
            print_char( ch, "Speed - modyfikator: %d\n\r", speed_multiplier );
        }

        DEBUG_INFO( "Additional_attacks: for_loop" );
        for ( i = 1; i < 4; i++ ) //Max 4 ataki, albo jeden nieudany.
        {
            //Generalnie to sprawdzamy czy wyprowadzi� dodatkowy atak
            if (number_percent() < URANGE (0, speed + (number_range(0, get_curr_stat_deprecated(ch, STAT_LUC)) - number_range(0, get_curr_stat_deprecated(victim, STAT_LUC))), 95))
            {
							//Sprawdzamy czy u�ywa tarczy. Jak tak, to szansa, �e walnie tarcz�.
							if( get_eq_char( ch, WEAR_SHIELD ) != NULL && number_percent() < 40 )
              one_hit_shield( ch, victim, dt );
                //Sprawdzamy czy u�ywa dw�ch broni, je�li tak to dajemy 60% na atak z pierwszej �apy, jak nie wejdzie to wyprowadzamy atak z drugiej �apy.
                else if ( check_dual_wielding )
                {
                    if (number_percent() < 60 )
                        if( !abide || abide->modifier == -1 || (abide->modifier == 1 && abide->level == 51 ) )
                            one_hit( ch, victim, dt , FALSE );
                        else
                        {
                            for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
                            {
                                if( (abide2 = affect_find(tch->affected, gsn_weapon_wrench ) ) != NULL && abide2->modifier != 1 && tch != ch )
                                {
                                    print_char( ch, "Pr�bujesz zada� cios %s %s, ale %s owini�ty wok� twej broni uniemo�lwia ci to.\n\r", victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr );
                                    print_char( victim, "%s pr�buje zada� ci cios %s, ale %s owini�ty wok� %s broni uniemo�lwia %s to.\n\r", ch->name, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                                    sprintf( msg, "%s pr�buje zada� cios %s %s, ale %s owini�ty wok� %s broni uniemo�lwia %s to.", ch->name, victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                                    act( msg, ch, NULL, victim, TO_NOTVICT );
                                }
                            }
                            act( "Nie jeste� w stanie wykona� $j.", ch, get_eq_char( ch, WEAR_WIELD ), NULL, TO_CHAR );
                        }

                    else
                        if( !abide || abide->modifier == 0 || (abide->modifier == 1 && abide->level != 51 ) )
                            one_hit( ch, victim, dt , TRUE );
                        else
                        {
                            for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
                            {
                                if( (abide2 = affect_find(tch->affected, gsn_weapon_wrench ) ) != NULL && abide2->modifier != 1 && tch != ch )
                                {
                                    print_char( ch, "Pr�bujesz zada� cios %s %s, ale %s owini�ty wok� twej broni uniemo�lwia ci to.\n\r", victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr );
                                    print_char( victim, "%s pr�buje zada� ci cios %s, ale %s owini�ty wok� %s broni uniemo�lwia %s to.\n\r", ch->name, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                                    sprintf( msg, "%s pr�buje zada� cios %s %s, ale %s owini�ty wok� %s broni uniemo�lwia %s to.", ch->name, victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                                    act( msg, ch, NULL, victim, TO_NOTVICT );
                                }
                            }
                            act( "Nie jeste� w stanie wykona� ataku $j.", ch, get_eq_char( ch, WEAR_SECOND ), NULL, TO_CHAR );
                        }

                }
                else if( !abide || abide->modifier == -1 || (abide->modifier == 1 && abide->level == 51 ) )
                        one_hit( ch, victim, dt , FALSE );
                    else
                    {
                        for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
                        {
                            if( (abide2 = affect_find(tch->affected, gsn_weapon_wrench ) ) != NULL && abide2->modifier != 1 && tch != ch )
                            {
                                print_char( ch, "Pr�bujesz zada� cios %s %s, ale %s owini�ty wok� twej broni uniemo�lwia ci to.\n\r", victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr );
                                print_char( victim, "%s pr�buje zada� ci cios %s, ale %s owini�ty wok� %s broni uniemo�lwia %s to.\n\r", ch->name, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                                sprintf( msg, "%s pr�buje zada� cios %s %s, ale %s owini�ty wok� %s broni uniemo�lwia %s to.", ch->name, victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                                act( msg, ch, NULL, victim, TO_NOTVICT );
                            }
                        }
                        act( "Nie jeste� w stanie wykona� $j.", ch, get_eq_char( ch, WEAR_WIELD ), NULL, TO_CHAR );
                    }

                //Odejmujemy speeda po wyprowadzonym ataku, je�li speed jest mniejszy ni� 0 to dajemy na 0.
                //Je�li nie ma wroga to dodatkowo ko�czymy p�tle, by ataki nie sz�y w pr�nie.
                speed -= speed/4+8;
                if (speed < 0) speed = 0;
                if ( ch->fighting != victim ) i = 4;
                if ( ch->pcdata->wiz_conf & W6 )
                {
                    print_char( ch, "Speed: %d\n\r", speed );
                }
            }
            else
                //Je�li mu si� nie uda�o setujemy i na 4, koncz�c p�tle.
            {
                i = 4;
            }
        }
    }
    DEBUG_INFO( "multi_hit:haste_hit" );
    /* haste daje szane na dodatkowy atak z losowej reki */
    if ( ( IS_AFFECTED( ch, AFF_HASTE ) && number_percent() < 50 ) || is_affected(ch, 300 ) )
    {
        if( get_eq_char( ch, WEAR_SHIELD ) != NULL && number_percent() < 50 )
            one_hit_shield( ch, victim, dt );
        else if( get_eq_char( ch, WEAR_SECOND ) != NULL && number_percent() < 50 )
        {
            if( !abide || abide->modifier == 0 || (abide->modifier == 1 && abide->level != 51 ) )
                one_hit( ch, victim, dt, TRUE );
            else
            {
                for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
                {
                    if( (abide2 = affect_find(tch->affected, gsn_weapon_wrench ) ) != NULL && abide2->modifier != 1 && tch != ch )
                    {
                        print_char( ch, "Pr�bujesz zada� cios %s %s, ale %s owini�ty wok� twej broni uniemo�lwia ci to.\n\r", victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr );
                        print_char( victim, "%s pr�buje zada� ci cios %s, ale %s owini�ty wok� %s broni uniemo�lwia %s to.\n\r", ch->name, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                        sprintf( msg, "%s pr�buje zada� cios %s %s, ale %s owini�ty wok� %s broni uniemo�lwia %s to.", ch->name, victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                        act( msg, ch, NULL, victim, TO_NOTVICT );
                    }
                }
                act( "Nie jeste� w stanie wykona� ataku $j.", ch, get_eq_char( ch, WEAR_SECOND ), NULL, TO_CHAR );
            }
        }
        else
        {
            if( !abide || abide->modifier == -1 || (abide->modifier == 1 && abide->level == 51 ) )
                one_hit( ch, victim, dt, FALSE );
            else
            {
                for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
                {
                    if( (abide2 = affect_find(tch->affected, gsn_weapon_wrench ) ) != NULL && abide2->modifier != 1 && tch != ch )
                    {
                        print_char( ch, "Pr�bujesz zada� cios %s %s, ale %s owini�ty wok� twej broni uniemo�lwia ci to.\n\r", victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr );
                        print_char( victim, "%s pr�buje zada� ci cios %s, ale %s owini�ty wok� %s broni uniemo�lwia %s to.\n\r", ch->name, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                        sprintf( msg, "%s pr�buje zada� cios %s %s, ale %s owini�ty wok� %s broni uniemo�lwia %s to.", ch->name, victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                        act( msg, ch, NULL, victim, TO_NOTVICT );
                    }
                }
                act( "Nie jeste� w stanie wykona� ataku $j.", ch, get_eq_char( ch, WEAR_WIELD ), NULL, TO_CHAR );
            }
        }
    }

    if ( ch->fighting != victim )
    {
        return;
    }

    if( is_affected( ch, gsn_glorious_impale ) )
    {
        if( !ch->fighting || !IS_AFFECTED( ch->fighting, AFF_PARALYZE ) || !is_affected( ch->fighting, gsn_glorious_impale ) )
        {
            affect_strip( ch, gsn_glorious_impale );
        }
        else
        {
            return;
        }
    }

    /* w sektorach naturalnych druidzi maja mala szanse na dodatkowy atak, na 31 levie +1% szansy */
    if ( ( ch->class == CLASS_DRUID ) && IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_DRUID ))
    {
        if ( number_range(0,1000) < ( ch->level + (ch->level / LEVEL_HERO)*10) )
        {
            send_to_char( "{gCzujesz nag�y przyp�yw si�y i korzystaj�c z niego zadajesz jeszcze jeden cios.{x\n\r", ch );
            if( get_eq_char( ch, WEAR_SECOND ) != NULL && get_eq_char( ch, WEAR_SECOND )->item_type == ITEM_WEAPON && number_percent() < 50 )
            {
                if( !abide || abide->modifier == 0 || (abide->modifier == 1 && abide->level != 51 ) )
                    one_hit( ch, victim, dt, TRUE );
                else
                {
                    for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
                    {
                        if( (abide2 = affect_find(tch->affected, gsn_weapon_wrench ) ) != NULL && abide2->modifier != 1 && tch != ch )
                        {
                            print_char( ch, "Pr�bujesz zada� cios %s %s, ale %s owini�ty wok� twej broni uniemo�lwia ci to.\n\r", victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr );
                            print_char( victim, "%s pr�buje zada� ci cios %s, ale %s owini�ty wok� %s broni uniemo�lwia %s to.\n\r", ch->name, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                            sprintf( msg, "%s pr�buje zada� cios %s %s, ale %s owini�ty wok� %s broni uniemo�lwia %s to.", ch->name, victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                            act( msg, ch, NULL, victim, TO_NOTVICT );
                        }
                    }
                    act( "Nie jeste� w stanie wykona� ataku $j.", ch, get_eq_char( ch, WEAR_SECOND ), NULL, TO_CHAR );
                }
            }
            else
            {
                if( !abide || abide->modifier == -1 || (abide->modifier == 1 && abide->level == 51 ) )
                    one_hit( ch, victim, dt, FALSE );
                else
                {
                    for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
                    {
                        if( (abide2 = affect_find(tch->affected, gsn_weapon_wrench ) ) != NULL && abide2->modifier != 1 && tch != ch )
                        {
                            print_char( ch, "Pr�bujesz zada� cios %s %s, ale %s owini�ty wok� twej broni uniemo�lwia ci to.\n\r", victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr );
                            print_char( victim, "%s pr�buje zada� ci cios %s, ale %s owini�ty wok� %s broni uniemo�lwia %s to.\n\r", ch->name, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                            sprintf( msg, "%s pr�buje zada� cios %s %s, ale %s owini�ty wok� %s broni uniemo�lwia %s to.", ch->name, victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                            act( msg, ch, NULL, victim, TO_NOTVICT );
                        }
                    }
                    act( "Nie jeste� w stanie wykona� ataku $j.", ch, get_eq_char( ch, WEAR_WIELD ), NULL, TO_CHAR );
                }
            }
        }
    }

    if ( ch->fighting != victim )
    {
        return;
    }

    DEBUG_INFO( "multi_hit:feat_tarczownik_hit" );

    if ( ch->fighting != victim )
    {
        return;
    }

    DEBUG_INFO( "multi_hit:trick_staff" );
    wield = get_eq_char(ch,WEAR_WIELD);

    if ( wield &&
            wield->item_type == ITEM_WEAPON &&
            wield->value[0] == WEAPON_STAFF &&
            IS_WEAPON_STAT( wield, WEAPON_TWO_HANDS ))
        check_trick(ch, victim, SN_TRICK_STAFF_SWIRL );

    DEBUG_INFO( "multi_hit:cleave_hit" );
    // rozp�atanie black knighta
    // tutaj nie ma od trika weapon wrench, bo w takim wypadku atak z cleave nie jest mozliwy
    wield = get_eq_char(ch,WEAR_WIELD);
    if ( wield && IS_SET( ch->fight_data, FIGHT_CLEAVE ) && victim->hit != remembered_hp )
    {
        REMOVE_BIT( ch->fight_data, FIGHT_CLEAVE );
        if ( IS_WEAPON_STAT( wield, WEAPON_TWO_HANDS ) &&
                wield->value[0] != WEAPON_DAGGER &&
                wield->value[0] != WEAPON_WHIP &&
                wield->value[0] != WEAPON_MACE &&
                wield->value[0] != WEAPON_STAFF &&
                wield->value[0] != WEAPON_SPEAR &&
                wield->value[0] != WEAPON_FLAIL &&
                wield->value[0] != WEAPON_SHORTSWORD &&
                wield->value[0] != WEAPON_CLAWS )
        {

            cleave_count = 0;

            for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
                if ( tch->fighting == ch || ch->fighting == tch )
                    crowd++;

            if( crowd == 1 )
            {
                for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
                {
                    if ( tch->fighting == ch && ch->fighting == tch && IS_AFFECTED( ch->fighting, AFF_MIRROR_IMAGE ) && is_affected(ch->fighting, gsn_mirror_image) )
                    {
                        while( ( cleave_count == 0 || cleave_count * 24 < get_skill(ch,gsn_cleave ) ) && is_affected(ch->fighting, gsn_mirror_image) )
                        {
                            switch( dice(1,10))
                            {
                                case 1:
                                    act( "Obracasz si� szybko i wyprowadzasz gwa�towny cios w innego oponenta!", ch, NULL, tch, TO_CHAR );
                                    act( "$n obraca si� szybko i wyprowadza gwa�towny cios w twoim kierunku!", ch, NULL, tch, TO_VICT );
                                    act( "$n obraca si� szybko i wyprowadza gwa�towny cios w innego oponenta!", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 2:
                                    act( "Odchylasz si� lekko zarazem atakuj�c niespodziewanie kolejnego wroga!", ch, NULL, tch, TO_CHAR );
                                    act( "$n odchyla si� lekko zarazem atakuj�c ci� niespodziewanie!", ch, NULL, tch, TO_VICT );
                                    act( "$n odchyla si� lekko zarazem atakuj�c niespodziewanie kolejnego wroga!", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 3:
                                    act( "Wyprowadzasz mocny cios w stoj�cego obok przeciwnika.", ch, NULL, tch, TO_CHAR );
                                    act( "$n wyprowadza mocny cios w twoim kierunku.", ch, NULL, tch, TO_VICT );
                                    act( "$n wyprowadza mocny cios w stoj�cego obok przeciwnika.", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 4:
                                    act( "Robisz delikatny zw�d i rzucasz si� na innego rywala!", ch, NULL, tch, TO_CHAR );
                                    act( "$n robi delikatny zw�d i rzuca si� na ciebie!", ch, NULL, tch, TO_VICT );
                                    act( "$n robi delikatny zw�d i rzuca si� na innego rywala!", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 5:
                                    act( "Markujesz jedno mocne uderzenie po czym wyprowadzasz rzeczywisty cios w innego przeciwnika!", ch, NULL, tch, TO_CHAR );
                                    act( "$n markuje jedno mocne uderzenie po czym wyprowadza rzeczywisty cios w twoim kierunku!", ch, NULL, tch, TO_VICT );
                                    act( "$n markuje jedno mocne uderzenie po czym wyprowadza rzeczywisty cios w innego przeciwnika!", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 6:
                                    act( "Bierzesz szeroki zamach i jednocze�nie atakujesz innego wroga!", ch, NULL, tch, TO_CHAR );
                                    act( "$n bierze szeroki zamach i jednocze�nie atakuje ciebie!", ch, NULL, tch, TO_VICT );
                                    act( "$n bierze szeroki zamach i jednocze�nie atakuje innego wroga!", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 7:
                                    act( "Przez chwile obserwujesz sytuacje dooko�a ciebie, a gdy przeciwnicy ustawiaj� si� blisko siebie pr�bujesz jednym mocnym ciosem zrani� wi�cej ni� jednego.", ch, NULL, tch, TO_CHAR );
                                    act( "$n przez chwile obserwuje sytuacje dooko�a, a gdy $s przeciwnicy ustawiaj� si� blisko siebie wyprowadza jedno zamaszyste ci�cie pr�buj�c zrani� wi�cej ni� jednego.", ch, NULL, tch, TO_VICT );
                                    act( "$n przez chwile obserwuje sytuacje dooko�a, a gdy $s przeciwnicy ustawiaj� si� blisko siebie wyprowadza jedno zamaszyste ci�cie pr�buj�c zrani� wi�cej ni� jednego.", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 8:
                                    act( "Wykonujesz zamaszysty p�obr�t atakuj�c przy okazji kolejn� ofiar�!", ch, NULL, tch, TO_CHAR );
                                    act( "$n wykonuje zamaszysty p�obr�t atakuj�c ciebie przy okazji!", ch, NULL, tch, TO_VICT );
                                    act( "$n wykonuje zamaszysty p�obr�t atakuj�c przy okazji kolejn� ofiar�!", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 9:
                                    act( "Odczekawszy chwil� wykorzystujesz sprzyjaj�cy moment aby za jednym razem zaatakowa� wi�cej ni� jednego nieprzyjaciela!", ch, NULL, tch, TO_CHAR );
                                    act( "$n odczekawszy chwil� wykorzystuje sprzyjaj�cy moment aby za jednym razem zaatakowa� wi�cej ni� jednego nieprzyjaciela!", ch, NULL, tch, TO_VICT );
                                    act( "$n odczekawszy chwil� wykorzystuje sprzyjaj�cy moment aby za jednym razem zaatakowa� wi�cej ni� jednego nieprzyjaciela!", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 10:
                                    act( "Robisz zdecydowany zw�d markuj�c seri� cios�w w jednego przeciwnika, jednak w ostatniej chwili odskakujesz i atakujesz innego rywala!", ch, NULL, tch, TO_CHAR );
                                    act( "$n robi zdecydowany zw�d markuj�c seri� cios�w w jednego przeciwnika, jednak w ostatniej chwili odskakuje i atakuje ci�!", ch, NULL, tch, TO_VICT );
                                    act( "$n robi zdecydowany zw�d markuj�c seri� cios�w w jednego przeciwnika, jednak w ostatniej chwili odskakuje i atakuje innego rywala!", ch, NULL, tch, TO_NOTVICT );
                                    break;
                            }

                            act( "Tw�j cios ze �wistem przebija si� przez cia�o $Z, kt�re nie stawi�o �adnego oporu.", ch, NULL, tch, TO_CHAR );
                            act( "Cios $z ze �wistem przebija si� przez twoje lustrzane odbicie, kt�re znika zaraz potem.", ch, NULL, tch, TO_VICT );
                            act( "Cios $z ze �wistem przebija si� przez cia�o $Z, kt�re nie stawi�o �adnego oporu.", ch, NULL, tch, TO_NOTVICT );
                            remove_mirror( ch, tch, 1 );

                            ++cleave_count;
                        }
                    }
                }
            }
            else
            {
                for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
                {
                    if ( tch->fighting == ch && ( ch->fighting != tch || ( IS_AFFECTED( ch->fighting, AFF_MIRROR_IMAGE ) && is_affected(ch->fighting, gsn_mirror_image) ) ) )
                    {
                        if( !stop && ( cleave_count == 0 || cleave_count * 24 < get_skill(ch,gsn_cleave ) ) )
                        {
                            switch( dice(1,10))
                            {
                                case 1:
                                    act( "Obracasz si� szybko i wyprowadzasz gwa�towny cios w innego oponenta!", ch, NULL, tch, TO_CHAR );
                                    act( "$n obraca si� szybko i wyprowadza gwa�towny cios w twoim kierunku!", ch, NULL, tch, TO_VICT );
                                    act( "$n obraca si� szybko i wyprowadza gwa�towny cios w innego oponenta!", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 2:
                                    act( "Odchylasz si� lekko zarazem atakuj�c niespodziewanie kolejnego wroga!", ch, NULL, tch, TO_CHAR );
                                    act( "$n odchyla si� lekko zarazem atakuj�c ci� niespodziewanie!", ch, NULL, tch, TO_VICT );
                                    act( "$n odchyla si� lekko zarazem atakuj�c niespodziewanie kolejnego wroga!", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 3:
                                    act( "Wyprowadzasz mocny cios w stoj�cego obok przeciwnika.", ch, NULL, tch, TO_CHAR );
                                    act( "$n wyprowadza mocny cios w twoim kierunku.", ch, NULL, tch, TO_VICT );
                                    act( "$n wyprowadza mocny cios w stoj�cego obok przeciwnika.", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 4:
                                    act( "Robisz delikatny zw�d i rzucasz si� na innego rywala!", ch, NULL, tch, TO_CHAR );
                                    act( "$n robi delikatny zw�d i rzuca si� na ciebie!", ch, NULL, tch, TO_VICT );
                                    act( "$n robi delikatny zw�d i rzuca si� na innego rywala!", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 5:
                                    act( "Markujesz jedno mocne uderzenie po czym wyprowadzasz rzeczywisty cios w innego przeciwnika!", ch, NULL, tch, TO_CHAR );
                                    act( "$n markuje jedno mocne uderzenie po czym wyprowadza rzeczywisty cios w twoim kierunku!", ch, NULL, tch, TO_VICT );
                                    act( "$n markuje jedno mocne uderzenie po czym wyprowadza rzeczywisty cios w innego przeciwnika!", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 6:
                                    act( "Bierzesz szeroki zamach i jednocze�nie atakujesz innego wroga!", ch, NULL, tch, TO_CHAR );
                                    act( "$n bierze szeroki zamach i jednocze�nie atakuje ciebie!", ch, NULL, tch, TO_VICT );
                                    act( "$n bierze szeroki zamach i jednocze�nie atakuje innego wroga!", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 7:
                                    act( "Przez chwile obserwujesz sytuacje dooko�a ciebie, a gdy przeciwnicy ustawiaj� si� blisko siebie pr�bujesz jednym mocnym ciosem zrani� wi�cej ni� jednego.", ch, NULL, tch, TO_CHAR );
                                    act( "$n przez chwile obserwuje sytuacje dooko�a, a gdy $s przeciwnicy ustawiaj� si� blisko siebie wyprowadza jedno zamaszyste ci�cie pr�buj�c zrani� wi�cej ni� jednego.", ch, NULL, tch, TO_VICT );
                                    act( "$n przez chwile obserwuje sytuacje dooko�a, a gdy $s przeciwnicy ustawiaj� si� blisko siebie wyprowadza jedno zamaszyste ci�cie pr�buj�c zrani� wi�cej ni� jednego.", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 8:
                                    act( "Wykonujesz zamaszysty p�obr�t atakuj�c przy okazji kolejn� ofiar�!", ch, NULL, tch, TO_CHAR );
                                    act( "$n wykonuje zamaszysty p�obr�t atakuj�c ciebie przy okazji!", ch, NULL, tch, TO_VICT );
                                    act( "$n wykonuje zamaszysty p�obr�t atakuj�c przy okazji kolejn� ofiar�!", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 9:
                                    act( "Odczekawszy chwil� wykorzystujesz sprzyjaj�cy moment aby za jednym razem zaatakowa� wi�cej ni� jednego nieprzyjaciela!", ch, NULL, tch, TO_CHAR );
                                    act( "$n odczekawszy chwil� wykorzystuje sprzyjaj�cy moment aby za jednym razem zaatakowa� wi�cej ni� jednego nieprzyjaciela!", ch, NULL, tch, TO_VICT );
                                    act( "$n odczekawszy chwil� wykorzystuje sprzyjaj�cy moment aby za jednym razem zaatakowa� wi�cej ni� jednego nieprzyjaciela!", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 10:
                                    act( "Robisz zdecydowany zw�d markuj�c seri� cios�w w jednego przeciwnika, jednak w ostatniej chwili odskakujesz i atakujesz innego rywala!", ch, NULL, tch, TO_CHAR );
                                    act( "$n robi zdecydowany zw�d markuj�c seri� cios�w w jednego przeciwnika, jednak w ostatniej chwili odskakuje i atakuje ci�!", ch, NULL, tch, TO_VICT );
                                    act( "$n robi zdecydowany zw�d markuj�c seri� cios�w w jednego przeciwnika, jednak w ostatniej chwili odskakuje i atakuje innego rywala!", ch, NULL, tch, TO_NOTVICT );
                                    break;
                            }

                            remembered_hp = tch->hit;
                            if( tch != ch->fighting )
                            {
                                if( check_trick( ch, tch, SN_TRICK_THOUSANDSLAYER ) )
                                {
                                    SET_BIT( ch->fight_data, FIGHT_THOUSANDSLAYER );
                                    one_hit( ch, tch, gsn_cleave, FALSE );
                                }
                                else
                                    one_hit( ch, tch, gsn_cleave, FALSE );
                            }
                            else
                            {
                                act( "Tw�j cios ze �wistem przebija si� przez cia�o $Z, kt�re nie stawi�o �adnego oporu.", ch, NULL, tch, TO_CHAR );
                                act( "Cios $z ze �wistem przebija si� przez twoje lustrzane odbicie, kt�re znika zaraz potem.", ch, NULL, tch, TO_VICT );
                                act( "Cios $z ze �wistem przebija si� przez cia�o $Z, kt�re nie stawi�o �adnego oporu.", ch, NULL, tch, TO_NOTVICT );
                                remove_mirror( ch, tch, 1 );
                                remembered_hp -= 1;//by nie przerwalo
                            }
                            ++cleave_count;
                            if( remembered_hp == tch->hit )
                            {
                                stop = TRUE;
                            }

                        }
                    }
                }
                if( IS_SET( ch->fight_data, FIGHT_THOUSANDSLAYER ) )
                {
                    act( "Na zako�czenie zadajesz jeszcze jeden cios swojemu przeciwnikowi.", ch, NULL, victim, TO_CHAR );
                    act( "Na zako�czenie $n zadaje ci jeszcze jeden cios.", ch, NULL, victim, TO_VICT );
                    act( "Na zako�czenie $n zadaje $Z jeszcze jeden cios.", ch, NULL, victim, TO_NOTVICT );
                    one_hit( ch, victim, gsn_cleave, FALSE );
                    REMOVE_BIT( ch->fight_data, FIGHT_THOUSANDSLAYER );
                }
            }
        }
    }

    return;
}

/* procedure for all mobile attacks */
void mob_hit ( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    AFFECT_DATA *abide, *abide2;
    CHAR_DATA * vch, *vch_next, *tch;
    bool slowed = FALSE, stop = FALSE;
    char *msg = (char*)calloc(512, sizeof(char));
    OBJ_DATA *weapon = NULL, *wield = NULL;
    int remembered_hp = victim->hit, vertical_skill, cleave_count, crowd = 0;

    DEBUG_INFO( "mob_hit:zero_attack" );
    if ( EXT_IS_SET( ch->off_flags, OFF_ZERO_ATTACK ) )
    {
        return;
    }

    DEBUG_INFO( "mob_hit:abide:1" );
    abide = affect_find( ch->affected, gsn_weapon_wrench );

	DEBUG_INFO( "mob_hit:area_attack" );
	/* Area attack -- BALLS nasty! */
	/*****************************************/
	if ( EXT_IS_SET( ch->off_flags, OFF_AREA_ATTACK ) )
	{
		for ( vch = ch->in_room->people; vch != NULL; vch = vch_next )
		{
			vch_next = vch->next_in_room;
			if ( ( vch != ch && vch->fighting == ch ) )
            {
				one_hit( ch, vch, dt , FALSE );
            }
		}
	}
	else
	{
		if( !abide || abide->modifier == -1 || (abide->modifier == 1 && abide->level == 51 ) )
        {
			one_hit( ch, victim, dt, FALSE );
        }
		else
		{
			for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
			{
				if( (abide2 = affect_find(tch->affected, gsn_weapon_wrench ) ) != NULL && abide2->modifier != 1 && tch != ch )
				{
					print_char( ch, "Pr�bujesz zada� cios %s %s, ale %s owini�ty wok� twej broni uniemo�lwia ci to.\n\r", victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr );
					print_char( victim, "%s pr�buje zada� ci cios %s, ale %s owini�ty wok� %s broni uniemo�lwia %s to.\n\r", ch->name, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
					sprintf( msg, "%s pr�buje zda� ci cios %s %s, ale %s owini�ty wok� %s broni uniemo�lwia %s to.", ch->name, victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
					act( msg, ch, NULL, victim, TO_NOTVICT );
				}
			}
			act( "Nie jeste� w stanie wykona� ataku $j.", ch, get_eq_char( ch, WEAR_WIELD ), NULL, TO_CHAR );
		}
	}

	if ( ch->fighting != victim )
    {
		return;
    }

	DEBUG_INFO( "mob_hit:second_weapon" );
	/* druga bro� */
	if ( ( weapon = get_eq_char ( ch, WEAR_SECOND ) ) != NULL &&
			( EXT_IS_SET( ch->act, ACT_WARRIOR ) ||
			  EXT_IS_SET( ch->act, ACT_PALADIN ) ||
			  EXT_IS_SET( ch->act, ACT_BARBARIAN ) ||
			  EXT_IS_SET( ch->act, ACT_BARD ) ||
			  EXT_IS_SET( ch->act, ACT_BLACK_KNIGHT ) ||
           EXT_IS_SET( ch->act, ACT_SHAMAN ) ||
			  EXT_IS_SET( ch->act, ACT_THIEF ) ) )
	{
		if( !abide || abide->modifier == 0 || (abide->modifier == 1 && abide->level != 51 ) )
        {
			one_hit( ch, victim, dt, TRUE );
        }
		else
		{
			for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
			{
				if( (abide2 = affect_find(tch->affected, gsn_weapon_wrench ) ) != NULL && abide2->modifier != 1 && tch != ch )
				{
					print_char( ch, "Pr�bujesz zada� cios %s %s, ale %s owini�ty wok� twej broni uniemo�lwia ci to.\n\r", victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr );
					print_char( victim, "%s pr�buje zada� cios cios %s, ale %s owini�ty wok� %s broni uniemo�lwia %s to.\n\r", ch->name, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
					sprintf( msg, "%s pr�buje zada� cios %s %s, ale %s owini�ty wok� %s broni uniemo�lwia %s to.", ch->name, victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
					act( msg, ch, NULL, victim, TO_NOTVICT );
				}
			}
			act( "Nie jeste� w stanie wykona� ataku $j.", ch, get_eq_char( ch, WEAR_SECOND ), NULL, TO_CHAR );
		}
	}

	if ( ch->fighting != victim )
    {
		return;
    }

	wield = get_eq_char(ch,WEAR_WIELD);
	//i tutaj vertical slash black knighta. Wczesniejszy atak musial trafic
	if ( wield && IS_SET( ch->fight_data, FIGHT_VERTICAL_SLASH ) && victim->hit != remembered_hp )
	{
		REMOVE_BIT( ch->fight_data, FIGHT_VERTICAL_SLASH );
		if ( IS_WEAPON_STAT( wield, WEAPON_TWO_HANDS ) &&
				wield->value[0] != WEAPON_DAGGER &&
				wield->value[0] != WEAPON_WHIP &&
				wield->value[0] != WEAPON_MACE &&
				wield->value[0] != WEAPON_STAFF &&
				wield->value[0] != WEAPON_SPEAR &&
				wield->value[0] != WEAPON_FLAIL &&
				wield->value[0] != WEAPON_SHORTSWORD &&
				wield->value[0] != WEAPON_CLAWS )
		{
			vertical_skill = get_skill( ch, gsn_vertical_slash );
			vertical_skill += (get_curr_stat_deprecated(ch,STAT_DEX)-10)*2;
			vertical_skill -= (get_curr_stat_deprecated(victim,STAT_DEX)-10)*2;
			if ( IS_AFFECTED( ch, AFF_SLOW ) ) vertical_skill /= 4;
			if ( IS_AFFECTED( ch, AFF_HASTE ) )	vertical_skill += 10;

			switch ( victim->size )//dla size tiny i giant nigdy nie wejdzie
			{
				case SIZE_SMALL:
					vertical_skill -= 10;
					break;
				case SIZE_LARGE:
					vertical_skill -= 10;
					break;
				case SIZE_HUGE:
					vertical_skill -= 25;
					break;
				default:
					break;
			}

			if( victim->size == SIZE_TINY )
			{
				if( victim->sex == 2 )
					act( "$N jest za ma�a, by pr�bowa� na niej takich sztuczek.", ch, NULL, victim, TO_CHAR );
				else
					act( "$N jest za ma�y, by pr�bowa� na nim takich sztuczek.", ch, NULL, victim, TO_CHAR );
			}
			else if ( victim->size == SIZE_GIANT )
			{
				if( victim->sex == 2 )
					act( "$N jest zbyt wielka, by pr�bowa� na niej takich sztuczek.", ch, NULL, victim, TO_CHAR );
				else
					act( "$N jest zbyt wielki, by pr�bowa� na nim takich sztuczek.", ch, NULL, victim, TO_CHAR );
			}
			else if ( number_percent() < vertical_skill || !can_move(victim))
			{
				switch( number_range(1,5))
				{
					case 1:
						act( "Po zadaniu ciosu czujesz, �e lekko oszo�omiony przeciwnik nie jest gotowy na �adn� obron�, co w spos�b bezwzgl�dny wykorzystujesz wyprowadzaj�c natychmiast kolejne ci�cie, tym razem pionowo z do�u w g�r�.", ch, wield, victim, TO_CHAR );
						act( "Cios $z lekko ci� oszo�omi�, co $e w spos�b bezwzgl�dny wykorzystuje wyprowadzaj�c natychmiast kolejne ci�cie, tym razem pionowo z do�u w g�r�.", ch, wield, victim, TO_VICT );
						act( "Cios $z lekko oszo�omi� $C, co zostaje w spos�b bezwzgl�dny wykorzystane wyprowadzeniem natychmiast kolejnego ci�cie, tym razem pionowo z do�u w g�r�.", ch, wield, victim, TO_NOTVICT );
						break;
					case 2:
						act( "Poprawiasz szybko sw�j chwyt na r�koje�ci $f i momentalnie tniesz ponownie z do�u w g�r�.", ch, wield, victim, TO_CHAR );
						act( "$n poprawia szybko sw�j chwyt na r�koje�ci $f i momentalnie tnie ci� ponownie z do�u w g�r�.", ch, wield, victim, TO_VICT );
						act( "$n poprawia szybko sw�j chwyt na r�koje�ci $f i momentalnie tnie ponownie z do�u w g�r�.", ch, wield, victim, TO_NOTVICT );
						break;
					case 3:
						act( "Wykorzystuj�c impet pierwszego ciosu i natychmiast tniesz po tej samej lini co poprzednio prowadz�c ostrze swojej broni ku g�rze.", ch, wield, victim, TO_CHAR );
						act( "$n wykorzystuj�c impet pierwszego ciosu natychmiast tnie ci� po tej samej lini co poprzednio prowadz�c ostrze swojej broni ku g�rze.", ch, wield, victim, TO_VICT );
						act( "$n wykorzystuj�c impet pierwszego ciosu natychmiast tnie po tej samej lini co poprzednio prowadz�c ostrze swojej broni ku g�rze.", ch, wield, victim, TO_NOTVICT );
						break;
					case 4:
						act( "Zaskoczywszy przeciwnika zupe�nie pierwszym ciosem z g�ry do do�u, wyprowadzasz kolejne pot�ne ci�cie w lini pionowej w odwrotnym kierunku.", ch, wield, victim, TO_CHAR );
						act( "Zaskoczywszy ci� zupe�nie pierwszym ciosem z g�ry do do�u, $n wyprowadza kolejne pot�ne ci�cie w lini pionowej w odwrotnym kierunku.", ch, wield, victim, TO_VICT );
						act( "Zaskoczywszy $C zupe�nie pierwszym ciosem z g�ry do do�u, $n wyprowadza kolejne pot�ne ci�cie w lini pionowej w odwrotnym kierunku.", ch, wield, victim, TO_NOTVICT );
						break;
					case 5:
						act( "Udaje ci si� po mistrzowsku wykorzysta� sprzyjaj�c� sytuacj� po zadaniu pierwszego ciosu aby w �lad za nim uderzy� po raz kolejny tn�c wroga od st�p do g�owy.", ch, wield, victim, TO_CHAR );
						act( "$x udaje si� wykorzysta� sprzyjaj�c� sytuacj� po zadaniu pierwszego ciosu aby w �lad za nim uderzy� ci� po raz kolejny tn�c od st�p do g�owy.", ch, wield, victim, TO_VICT );
						act( "$x udaje si� wykorzysta� sprzyjaj�c� sytuacj� po zadaniu pierwszego ciosu aby w �lad za nim uderzy� $C po raz kolejny tn�c od st�p do g�owy.", ch, wield, victim, TO_NOTVICT );
						break;
				}

				one_hit( ch, victim, gsn_vertical_slash , FALSE );
				check_improve( ch, victim, gsn_vertical_slash, TRUE, 50 );
			}
			else if( number_percent() > URANGE( 50, get_curr_stat_deprecated(ch,STAT_DEX)*4 + vertical_skill/3, 95) )
			{
				switch( number_range(1,5))
				{
					case 1:
						act( "Zaatakowawszy ponownie po wyprowadzeniu pierwszego ciosu stwierdzasz ze zgroz�, �e przeciwnik w ostatniej chwili zd�zy� si� uchyli�. Twoja bro� tnie powietrze, a ty sa<&m/ma/mo> lecisz do przodu na spotkanie gruntu.", ch, wield, victim, TO_CHAR );
						act( "Wykonujesz g�adki unik przed niezgrabnym atakiem $z, $s bro� tnie powietrze i leci $e do przodu na spotkanie gruntu.", ch, wield, victim, TO_VICT );
						act( "$N wykonuje g�adki unik przed niezgrabnym atakiem $z, $s bro� tnie powietrze i leci $e do przodu na spotkanie gruntu.", ch, wield, victim, TO_NOTVICT );
						break;
					case 2:
						act( "Pr�bujesz zmieni� chwyt na r�koje�ci swej broni, jednak robisz to tak nieudolnie, �e wypada ci ona z r�k. Szybko pr�bujesz naprawi� sytuacje i przerywasz p�ynny ruch, kt�ry wykonywa�<&e�/a�/o�>, dzi�ki czemu udaje ci si� natychmiast z�apa� $h, ale kosztem utraty r�wnowagi.", ch, wield, victim, TO_CHAR );
						act( "$n pr�buje zmieni� chwyt na r�koje�ci swej broni, jednak robi to tak nieudolnie, �e wypada ona z $s r�k. Szybko pr�buje $e naprawi� sytuacje i przerywa p�ynny ruch, kt�ry mia� ci pewnie zrobi� krzywd�. Udaje si� $o natychmiast z�apa� $h, ale kosztem utraty r�wnowagi.", ch, wield, victim, TO_VICT );
						act( "$n pr�buje zmieni� chwyt na r�koje�ci swej broni, jednak robi to tak nieudolnie, �e wypada ona z $s r�k. Szybko pr�buje $e naprawi� sytuacje i przerywa p�ynny ruch, kt�ry mia� pewnie zrobi� krzywd� $X. Udaje si� $o natychmiast z�apa� $h, ale kosztem utraty r�wnowagi.", ch, wield, victim, TO_NOTVICT );
						break;
					case 3:
						act( "Po udanym pierwszym ciosie wydawa�o ci si�, �e zadanie kolejnego momentalnie po pierwszym nie b�dzie trudne. Sztuka ta jednak ci� przeros�a i w trakcie zamachu straci�<&e�/a�/o�> r�wnowag� wywracaj�c si� na ziemie.", ch, wield, victim, TO_CHAR );
						act( "Po udanym pierwszym ciosie $x wydawa�o si�, �e zadanie ci kolejnego momentalnie po pierwszym nie b�dzie trudne. Na szcz�cie sztuka ta przeros�a $s umiej�tno�ci i w trakcie zamachu r�wnowaga zosta�a stracona, przez co $n le�y teraz na ziemi.", ch, wield, victim, TO_VICT );
						act( "Po udanym pierwszym ciosie $x wydawa�o si�, �e zadanie kolejnego momentalnie po pierwszym nie b�dzie trudne. Sztuka ta przeros�a jednak $s umiej�tno�ci i w trakcie zamachu r�wnowaga zosta�a stracona, przez co $n le�y teraz na ziemi.", ch, wield, victim, TO_NOTVICT );
						break;
					case 4:
						act( "Kiedy twoje pierwsze ci�cie wyprowadzone znad g�owy przynios�o wyj�tkowo dobry efekt pr�bujesz powt�rzy� cios, tym razem bior�c zamach od do�u. Pechowo jednak zahaczasz ko�cem $f o pod�o�e, co ca�kowicie wyprowadza ci� z rytmu i r�wnowagi. Upadasz na ziemi�.", ch, wield, victim, TO_CHAR );
						if( ch->sex == 2 )
						{
							act( "$n pr�buje powt�rzy� cios, tym razem bior�c zamach od do�u. Na szcz�cie jednak zahacza ko�cem $f o pod�o�e, co ca�kowicie wyprowadza j� z rytmu i r�wnowagi. Upada ona na ziemi�.", ch, wield, victim, TO_VICT );
							act( "$n pr�buje powt�rzy� cios, tym razem bior�c zamach od do�u, zahacza jednak ko�cem $f o pod�o�e, co ca�kowicie wyprowadza j� z rytmu i r�wnowagi. Upada ona na ziemi�.", ch, wield, victim, TO_NOTVICT );
						}
						else
						{
							act( "$n pr�buje powt�rzy� cios, tym razem bior�c zamach od do�u. Na szcz�cie jednak zahacza ko�cem $f o pod�o�e, co ca�kowicie wyprowadza go z rytmu i r�wnowagi. Upada on na ziemi�.", ch, wield, victim, TO_VICT );
							act( "$n pr�buje powt�rzy� cios, tym razem bior�c zamach od do�u, zahacza jednak ko�cem $f o pod�o�e, co ca�kowicie wyprowadza go z rytmu i r�wnowagi. Upada on na ziemi�.", ch, wield, victim, TO_NOTVICT );
						}
						break;
					case 5:
						act( "Zadawszy pierwszy cios wpadasz w dziki sza�, istny amok, i nie my�l�c zbyt du�o natychmiast chcesz powt�rzy� poprzedni wyczyn, jednak w swym zapami�taniu wypadasz z rytmu i ostatecznie walisz si� na ziemi� jak d�ug<&i/a/ie>.", ch, wield, victim, TO_CHAR );
						act( "Zadawszy pierwszy cios $n wpada w dziki sza�, istny amok, i nie my�l�c zbyt du�o natychmiast chce powt�rzy� poprzedni wyczyn, na szcz�cie w swym zapami�taniu wypada z rytmu i ostatecznie wali si� na ziemi�.", ch, wield, victim, TO_VICT );
						act( "Zadawszy pierwszy cios $n wpada w dziki sza�, istny amok, i nie my�l�c zbyt du�o natychmiast chce powt�rzy� poprzedni wyczyn, jednak w swym zapami�taniu wypada z rytmu i ostatecznie wali si� na ziemi�.", ch, wield, victim, TO_NOTVICT );
						break;
				}

				if ( ch->position > POS_SITTING )
					ch->position = POS_SITTING;
				check_improve( ch, victim, gsn_vertical_slash, FALSE, 50 );
			}
			else
			{
				switch( number_range(1,5))
				{
					case 1:
						act( "Przez moment wydaje ci si�, �e przeciwnik nie zd�zy� pozbiera� si� po pierwszym ciosie i pr�bujesz wyprowadzi� natychmiast kolejny, $E jednak uchyla si� w ostatniej chwili.", ch, wield, victim, TO_CHAR );
						act( "$x przez moment wydaje si�, �e nie zd�zy�<&e�/a�/o�> pozbiera� si� po pierwszym ciosie i pr�buje wyprowadzi� natychmiast kolejny, ty jednak uchylasz si� bez k�opotu.", ch, wield, victim, TO_VICT );
						act( "$n pr�buje wyprowadzi� natychmiast kolejny cios, $N jednak uchyla si� bez k�opotu.", ch, wield, victim, TO_NOTVICT );
						break;
					case 2:
						act( "Przepe�nion<&y/a/e> dzikim sza�em po perfekcyjnym pierwszym ci�ciu w euforii starasz si� bezzw�ocznie zaatakowa� po raz kolejny, jednak pud�ujesz haniebnie.", ch, wield, victim, TO_CHAR );
						act( "Po perfekcyjnym pierwszym ci�ciu $n w euforii stara si� bezzw�ocznie zaatakowa� ci� po raz kolejny, jednak pud�uje haniebnie.", ch, wield, victim, TO_VICT );
						act( "Po perfekcyjnym pierwszym ci�ciu $n w euforii stara si� bezzw�ocznie zaatakowa� $C po raz kolejny, jednak pud�uje haniebnie.", ch, wield, victim, TO_NOTVICT );
						break;
					case 3:
						act( "Wyprowadziwszy silne ci�cie znad g�owy starasz si� poprawi� sw�j chwyt na r�koje�ci $f, ta jednak wymyka ci si� na chwil� z r�k i tracisz kilka chwil, kt�re mog�<&e�/a�/o�> wykorzysta� na zadanie kolejnego ataku.", ch, wield, victim, TO_CHAR );
						act( "$n wyprowadziwszy silne ci�cie znad g�owy stara si� poprawi� sw�j chwyt na r�koje�ci $f, ta jednak wymyka si� na chwil� z $s r�k i traci $e kilka chwil, kt�re mog�y zosta� wykorzysta� na zadanie ci kolejnego ciosu.", ch, wield, victim, TO_VICT );
						act( "$n wyprowadziwszy silne ci�cie znad g�owy stara si� poprawi� sw�j chwyt na r�koje�ci $f, ta jednak wymyka si� na chwil� z $s r�k i traci $e kilka chwil, kt�re mog�y zosta� wykorzysta� na zadanie $X kolejnego ciosu.", ch, wield, victim, TO_NOTVICT );
						break;
					case 4:
						act( "Starasz si� wyprowadzi� kolejny atak po zadaniu pierwszego, jednak przeciwnik przewidzia� twoje zamiary i w sprytny spos�b odskakuje lekko na bok.", ch, wield, victim, TO_CHAR );
						if( ch->sex != 2 )
						{
							act( "$n stara� si� wyprowadzi� kolejny atak po zadaniu pierwszego, jednak przewidzia�<&e�/a�/o�> $s zamiary i w sprytny spos�b odskakujesz lekko na bok.", ch, wield, victim, TO_VICT );
							act( "$n stara� si� wyprowadzi� kolejny atak po zadaniu pierwszego, jednak $N w sprytny spos�b odskakuje lekko na bok.", ch, wield, victim, TO_NOTVICT );
						}
						else
						{
							act( "$n stara�a si� wyprowadzi� kolejny atak po zadaniu pierwszego, jednak przewidzia�<&e�/a�/o�> $s zamiary i w sprytny spos�b odskakujesz lekko na bok.", ch, wield, victim, TO_VICT );
							act( "$n stara�a si� wyprowadzi� kolejny atak po zadaniu pierwszego, jednak $N w sprytny spos�b odskakuje lekko na bok.", ch, wield, victim, TO_NOTVICT );
						}
						break;
					case 5:
						act( "Po wyprowadzeniu ciosu masz ochot� ponowi� atak, jednak wahasz si� o jedn� chwil� za d�ugo i sprzyjaj�ca sytuacja mija.", ch, wield, victim, TO_CHAR );
						act( "Po wyprowadzeniu ciosu $n ma ochot� ponowi� atak, jednak waha si� o jedn� chwil� za d�ugo i sprzyjaj�ca sytuacja mija.", ch, wield, victim, TO_VICT );
						act( "Po wyprowadzeniu ciosu $n ma ochot� ponowi� atak, jednak waha si� o jedn� chwil� za d�ugo i sprzyjaj�ca sytuacja mija.", ch, wield, victim, TO_NOTVICT );
						break;
				}
				check_improve( ch, victim, gsn_vertical_slash, FALSE, 45 );
			}
		}
	}

	if ( ch->fighting != victim )
		return;

	DEBUG_INFO( "mob_hit:berserk" );
	if ( is_affected( ch, gsn_berserk ) && number_percent() < 40 )
	{
		if( get_eq_char( ch, WEAR_SECOND ) != NULL && number_percent() < 50 )
		{
			if( !abide || abide->modifier == 0 || (abide->modifier == 1 && abide->level != 51 ) )
				one_hit( ch, victim, dt, TRUE );
			else
			{
				for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
				{
					if( (abide2 = affect_find(tch->affected, gsn_weapon_wrench ) ) != NULL && abide2->modifier != 1 && tch != ch )
					{
						print_char( ch, "Pr�bujesz zada� cios %s %s, ale %s owini�ty wok� twej broni uniemo�lwia ci to.\n\r", victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr );
						print_char( victim, "%s pr�buje zada� ci cios %s, ale %s owini�ty wok� %s broni uniemo�lwia %s to.\n\r", ch->name, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
						sprintf( msg, "%s pr�buje zada� cios %s %s, ale %s owini�ty wok� %s broni uniemo�lwia %s to.", ch->name, victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
						act( msg, ch, NULL, victim, TO_NOTVICT );
					}
				}
				act( "Nie jeste� w stanie wykona� ataku $j.", ch, get_eq_char( ch, WEAR_SECOND ), NULL, TO_CHAR );
			}
		}
		else
		{
			if( !abide || abide->modifier == -1 || (abide->modifier == 1 && abide->level == 51 ) )
				one_hit( ch, victim, dt, FALSE );
			else
			{
				for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
				{
					if( (abide2 = affect_find(tch->affected, gsn_weapon_wrench ) ) != NULL && abide2->modifier != 1 && tch != ch )
					{
						print_char( ch, "Pr�bujesz zada� cios %s %s, ale %s owini�ty wok� twej broni uniemo�lwia ci to.\n\r", victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr );
						print_char( victim, "%s pr�buje zada� ci cios %s, ale %s owini�ty wok� %s broni uniemo�lwia %s to.\n\r", ch->name, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
						sprintf( msg, "%s pr�buje zada� cios %s %s, ale %s owini�ty wok� %s broni uniemo�lwia %s to.", ch->name, victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
						act( msg, ch, NULL, victim, TO_NOTVICT );
					}
				}
				act( "Nie jeste� w stanie wykona� ataku $j.", ch, get_eq_char( ch, WEAR_WIELD ), NULL, TO_CHAR );
			}
		}
	}

	if ( is_affected( ch, gsn_wardance ) && number_percent() < 50 )
	{
		if( get_eq_char( ch, WEAR_SECOND ) != NULL && number_percent() < 50 )
		{
			if( !abide || abide->modifier == 0 || (abide->modifier == 1 && abide->level != 51 ) )
				one_hit( ch, victim, dt, TRUE );
			else
			{
				for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
				{
					if( (abide2 = affect_find(tch->affected, gsn_weapon_wrench ) ) != NULL && abide2->modifier != 1 && tch != ch )
					{
						print_char( ch, "Pr�bujesz zada� cios %s %s, ale %s owini�ty wok� twej broni uniemo�lwia ci to.\n\r", victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr );
						print_char( victim, "%s pr�buje zada� ci cios %s, ale %s owini�ty wok� %s broni uniemo�lwia %s to.\n\r", ch->name, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
						sprintf( msg, "%s pr�buje zada� cios %s %s, ale %s owini�ty wok� %s broni uniemo�lwia %s to.", ch->name, victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
						act( msg, ch, NULL, victim, TO_NOTVICT );
					}
				}
				act( "Nie jeste� w stanie wykona� ataku $j.", ch, get_eq_char( ch, WEAR_SECOND ), NULL, TO_CHAR );
			}
		}
		else
		{
			if( !abide || abide->modifier == -1 || (abide->modifier == 1 && abide->level == 51 ) )
				one_hit( ch, victim, dt, FALSE );
			else
			{
				for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
				{
					if( (abide2 = affect_find(tch->affected, gsn_weapon_wrench ) ) != NULL && abide2->modifier != 1 && tch != ch )
					{
						print_char( ch, "Pr�bujesz zada� cios %s %s, ale %s owini�ty wok� twej broni uniemo�lwia ci to.\n\r", victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr );
						print_char( victim, "%s pr�buje zada� ci cios %s, ale %s owini�ty wok� %s broni uniemo�lwia %s to.\n\r", ch->name, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
						sprintf( msg, "%s pr�buje zada� cios %s %s, ale %s owini�ty wok� %s broni uniemo�lwia %s to.", ch->name, victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
						act( msg, ch, NULL, victim, TO_NOTVICT );
					}
				}
				act( "Nie jeste� w stanie wykona� ataku $j.", ch, get_eq_char( ch, WEAR_WIELD ), NULL, TO_CHAR );
			}
		}
	}

	if ( ch->fighting != victim )
		return;

	DEBUG_INFO( "mob_hit:haste/fast/slow" );
	if ( ( IS_AFFECTED( ch, AFF_HASTE ) || EXT_IS_SET( ch->off_flags, OFF_FAST ) ) && !IS_AFFECTED( ch, AFF_SLOW ) )
	{
		if (( !IS_AFFECTED(ch,AFF_CHARM ) || number_percent() < 50 ) || is_affected(ch, 300 ))
		{
			if( get_eq_char( ch, WEAR_SECOND ) != NULL && number_percent() < 50 )
			{
				if( !abide || abide->modifier == 0 || (abide->modifier == 1 && abide->level != 51 ) )
					one_hit( ch, victim, dt, TRUE );
				else
				{
					for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
					{
						if( (abide2 = affect_find(tch->affected, gsn_weapon_wrench ) ) != NULL && abide2->modifier != 1 && tch != ch )
						{
							print_char( ch, "Pr�bujesz zada� cios %s %s, ale %s owini�ty wok� twej broni uniemo�lwia ci to.\n\r", victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr );
							print_char( victim, "%s pr�buje zada� ci cios %s, ale %s owini�ty wok� %s broni uniemo�lwia %s to.\n\r", ch->name, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
							sprintf( msg, "%s pr�buje zada� cios %s %s, ale %s owini�ty wok� %s broni uniemo�lwia %s to.", ch->name, victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
							act( msg, ch, NULL, victim, TO_NOTVICT );
						}
					}
					act( "Nie jeste� w stanie wykona� ataku $j.", ch, get_eq_char( ch, WEAR_SECOND ), NULL, TO_CHAR );
				}
			}
			else
			{
				if( !abide || abide->modifier == -1 || (abide->modifier == 1 && abide->level == 51 ) )
					one_hit( ch, victim, dt, FALSE );
				else
				{
					for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
					{
						if( (abide2 = affect_find(tch->affected, gsn_weapon_wrench ) ) != NULL && abide2->modifier != 1 && tch != ch )
						{
							print_char( ch, "Pr�bujesz zada� cios %s %s, ale %s owini�ty wok� twej broni uniemo�lwia ci to.\n\r", victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr );
							print_char( victim, "%s pr�buje zada� ci cios %s, ale %s owini�ty wok� %s broni uniemo�lwia %s to.\n\r", ch->name, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
							sprintf( msg, "%s pr�buje zada� cios %s %s, ale %s owini�ty wok� %s broni uniemo�lwia %s to.", ch->name, victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
							act( msg, ch, NULL, victim, TO_NOTVICT );
						}
					}
					act( "Nie jeste� w stanie wykona� ataku $j.", ch, get_eq_char( ch, WEAR_WIELD ), NULL, TO_CHAR );
				}
			}
		}
	}

	DEBUG_INFO( "mob_hit:razorblade_hands/beast_claws" );
	if ( IS_AFFECTED( ch, AFF_RAZORBLADED ) ||
			IS_AFFECTED( ch, AFF_BEAST_CLAWS ) )
		one_hit( ch, victim, dt , FALSE );

	/* ilosci atakow nie dzialaja jesli uzywa broni - anticharmies stuff */
	if ( weapon )
		return;

	if ( IS_AFFECTED( ch, AFF_SLOW ) )
		slowed = TRUE;

	if ( ch->fighting != victim )
		return;


	DEBUG_INFO( "mob_hit:multi_attacks" );
	if ( EXT_IS_SET( ch->off_flags, OFF_ONE_HALF_ATTACK ) )
	{
		if ( number_percent() < 50 && !slowed )
		{
			one_hit( ch, victim, dt , FALSE );

			if ( ch->fighting != victim )
				return;
		}

	}
	else if ( EXT_IS_SET( ch->off_flags, OFF_TWO_ATTACK ) )
	{
		if ( !slowed || number_percent() < 50 )
		{
			one_hit( ch, victim, dt , FALSE );

			if ( ch->fighting != victim )
				return;
		}
	}
	else if ( EXT_IS_SET( ch->off_flags, OFF_THREE_ATTACK ) )
	{
		if ( !slowed )
		{
			one_hit( ch, victim, dt , FALSE );
			if ( ch->fighting != victim )
				return;
		}
		one_hit( ch, victim, dt , FALSE );
		if ( ch->fighting != victim )
			return;
	}
	else if ( EXT_IS_SET( ch->off_flags, OFF_FOUR_ATTACK ) )
	{
		if ( !slowed )
		{
			one_hit( ch, victim, dt , FALSE );
			if ( ch->fighting != victim )
				return;
		}

		one_hit( ch, victim, dt , FALSE );
		if ( ch->fighting != victim )
			return;

		one_hit( ch, victim, dt , FALSE );
		if ( ch->fighting != victim )
			return;
	}

	if ( ch->fighting != victim )
		return;

	// rozp�atanie black knighta
	wield = get_eq_char(ch,WEAR_WIELD);
	if ( !slowed && wield && IS_SET( ch->fight_data, FIGHT_CLEAVE ) && victim->hit != remembered_hp )
	{
		REMOVE_BIT( ch->fight_data, FIGHT_CLEAVE );
		if ( IS_WEAPON_STAT( wield, WEAPON_TWO_HANDS ) &&
				wield->value[0] != WEAPON_DAGGER &&
				wield->value[0] != WEAPON_WHIP &&
				wield->value[0] != WEAPON_MACE &&
				wield->value[0] != WEAPON_STAFF &&
				wield->value[0] != WEAPON_SPEAR &&
				wield->value[0] != WEAPON_FLAIL &&
				wield->value[0] != WEAPON_SHORTSWORD &&
				wield->value[0] != WEAPON_CLAWS
			 )
		{

			cleave_count = 0;

			for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
				if ( tch->fighting == ch || ch->fighting == tch )
					crowd++;

			if( crowd == 1 )
			{
				for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
				{
					if ( tch->fighting == ch && ch->fighting == tch && IS_AFFECTED( ch->fighting, AFF_MIRROR_IMAGE ) && is_affected(ch->fighting, gsn_mirror_image) )
					{
						while( ( cleave_count == 0 || cleave_count * 24 < get_skill(ch,gsn_cleave ) ) && is_affected(ch->fighting, gsn_mirror_image) )
						{
							switch( dice(1,10))
							{
								case 1:
									act( "Obracasz si� szybko i wyprowadzasz gwa�towny cios w innego oponenta!", ch, NULL, tch, TO_CHAR );
									act( "$n obraca si� szybko i wyprowadza gwa�towny cios w twoim kierunku!", ch, NULL, tch, TO_VICT );
									act( "$n obraca si� szybko i wyprowadza gwa�towny cios w innego oponenta!", ch, NULL, tch, TO_NOTVICT );
									break;
								case 2:
									act( "Odchylasz si� lekko zarazem atakuj�c niespodziewanie kolejnego wroga!", ch, NULL, tch, TO_CHAR );
									act( "$n odchyla si� lekko zarazem atakuj�c ci� niespodziewanie!", ch, NULL, tch, TO_VICT );
									act( "$n odchyla si� lekko zarazem atakuj�c niespodziewanie kolejnego wroga!", ch, NULL, tch, TO_NOTVICT );
									break;
								case 3:
									act( "Wyprowadzasz mocny cios w stoj�cego obok przeciwnika.", ch, NULL, tch, TO_CHAR );
									act( "$n wyprowadza mocny cios w twoim kierunku.", ch, NULL, tch, TO_VICT );
									act( "$n wyprowadza mocny cios w stoj�cego obok przeciwnika.", ch, NULL, tch, TO_NOTVICT );
									break;
								case 4:
									act( "Robisz delikatny zw�d i rzucasz si� na innego rywala!", ch, NULL, tch, TO_CHAR );
									act( "$n robi delikatny zw�d i rzuca si� na ciebie!", ch, NULL, tch, TO_VICT );
									act( "$n robi delikatny zw�d i rzuca si� na innego rywala!", ch, NULL, tch, TO_NOTVICT );
									break;
								case 5:
									act( "Markujesz jedno mocne uderzenie po czym wyprowadzasz rzeczywisty cios w innego przeciwnika!", ch, NULL, tch, TO_CHAR );
									act( "$n markuje jedno mocne uderzenie po czym wyprowadza rzeczywisty cios w twoim kierunku!", ch, NULL, tch, TO_VICT );
									act( "$n markuje jedno mocne uderzenie po czym wyprowadza rzeczywisty cios w innego przeciwnika!", ch, NULL, tch, TO_NOTVICT );
									break;
								case 6:
									act( "Bierzesz szeroki zamach i jednocze�nie atakujesz innego wroga!", ch, NULL, tch, TO_CHAR );
									act( "$n bierze szeroki zamach i jednocze�nie atakuje ciebie!", ch, NULL, tch, TO_VICT );
									act( "$n bierze szeroki zamach i jednocze�nie atakuje innego wroga!", ch, NULL, tch, TO_NOTVICT );
									break;
								case 7:
									act( "Przez chwile obserwujesz sytuacje dooko�a ciebie, a gdy przeciwnicy ustawiaj� si� blisko siebie pr�bujesz jednym mocnym ciosem zrani� wi�cej ni� jednego.", ch, NULL, tch, TO_CHAR );
									act( "$n przez chwile obserwuje sytuacje dooko�a, a gdy $s przeciwnicy ustawiaj� si� blisko siebie wyprowadza jedno zamaszyste ci�cie pr�buj�c zrani� wi�cej ni� jednego.", ch, NULL, tch, TO_VICT );
									act( "$n przez chwile obserwuje sytuacje dooko�a, a gdy $s przeciwnicy ustawiaj� si� blisko siebie wyprowadza jedno zamaszyste ci�cie pr�buj�c zrani� wi�cej ni� jednego.", ch, NULL, tch, TO_NOTVICT );
									break;
								case 8:
									act( "Wykonujesz zamaszysty p�obr�t atakuj�c przy okazji kolejn� ofiar�!", ch, NULL, tch, TO_CHAR );
									act( "$n wykonuje zamaszysty p�obr�t atakuj�c ciebie przy okazji!", ch, NULL, tch, TO_VICT );
									act( "$n wykonuje zamaszysty p�obr�t atakuj�c przy okazji kolejn� ofiar�!", ch, NULL, tch, TO_NOTVICT );
									break;
								case 9:
									act( "Odczekawszy chwil� wykorzystujesz sprzyjaj�cy moment aby za jednym razem zaatakowa� wi�cej ni� jednego nieprzyjaciela!", ch, NULL, tch, TO_CHAR );
									act( "$n odczekawszy chwil� wykorzystuje sprzyjaj�cy moment aby za jednym razem zaatakowa� wi�cej ni� jednego nieprzyjaciela!", ch, NULL, tch, TO_VICT );
									act( "$n odczekawszy chwil� wykorzystuje sprzyjaj�cy moment aby za jednym razem zaatakowa� wi�cej ni� jednego nieprzyjaciela!", ch, NULL, tch, TO_NOTVICT );
									break;
								case 10:
									act( "Robisz zdecydowany zw�d markuj�c seri� cios�w w jednego przeciwnika, jednak w ostatniej chwili odskakujesz i atakujesz innego rywala!", ch, NULL, tch, TO_CHAR );
									act( "$n robi zdecydowany zw�d markuj�c seri� cios�w w jednego przeciwnika, jednak w ostatniej chwili odskakuje i atakuje ci�!", ch, NULL, tch, TO_VICT );
									act( "$n robi zdecydowany zw�d markuj�c seri� cios�w w jednego przeciwnika, jednak w ostatniej chwili odskakuje i atakuje innego rywala!", ch, NULL, tch, TO_NOTVICT );
									break;
							}

							act( "Tw�j cios ze �wistem przebija si� przez cia�o $Z, kt�re nie stawi�o �adnego oporu.", ch, NULL, tch, TO_CHAR );
							act( "Cios $z ze �wistem przebija si� przez twoje lustrzane odbicie, kt�re znika zaraz potem.", ch, NULL, tch, TO_VICT );
							act( "Cios $z ze �wistem przebija si� przez cia�o $Z, kt�re nie stawi�o �adnego oporu.", ch, NULL, tch, TO_NOTVICT );
							remove_mirror( ch, tch, 1 );

							++cleave_count;
						}
					}
				}
			}
			else
			{
				for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
				{
					if ( tch->fighting == ch && ( ch->fighting != tch || ( IS_AFFECTED( ch->fighting, AFF_MIRROR_IMAGE ) && is_affected(ch->fighting, gsn_mirror_image) ) ) )
					{
						if( !stop && ( cleave_count == 0 || cleave_count * 24 < get_skill(ch,gsn_cleave ) ) )
						{
							switch( dice(1,10))
							{
								case 1:
									act( "Obracasz si� szybko i wyprowadzasz gwa�towny cios w innego oponenta!", ch, NULL, tch, TO_CHAR );
									act( "$n obraca si� szybko i wyprowadza gwa�towny cios w twoim kierunku!", ch, NULL, tch, TO_VICT );
									act( "$n obraca si� szybko i wyprowadza gwa�towny cios w innego oponenta!", ch, NULL, tch, TO_NOTVICT );
									break;
								case 2:
									act( "Odchylasz si� lekko zarazem atakuj�c niespodziewanie kolejnego wroga!", ch, NULL, tch, TO_CHAR );
									act( "$n odchyla si� lekko zarazem atakuj�c ci� niespodziewanie!", ch, NULL, tch, TO_VICT );
									act( "$n odchyla si� lekko zarazem atakuj�c niespodziewanie kolejnego wroga!", ch, NULL, tch, TO_NOTVICT );
									break;
								case 3:
									act( "Wyprowadzasz mocny cios w stoj�cego obok przeciwnika.", ch, NULL, tch, TO_CHAR );
									act( "$n wyprowadza mocny cios w twoim kierunku.", ch, NULL, tch, TO_VICT );
									act( "$n wyprowadza mocny cios w stoj�cego obok przeciwnika.", ch, NULL, tch, TO_NOTVICT );
									break;
								case 4:
									act( "Robisz delikatny zw�d i rzucasz si� na innego rywala!", ch, NULL, tch, TO_CHAR );
									act( "$n robi delikatny zw�d i rzuca si� na ciebie!", ch, NULL, tch, TO_VICT );
									act( "$n robi delikatny zw�d i rzuca si� na innego rywala!", ch, NULL, tch, TO_NOTVICT );
									break;
								case 5:
									act( "Markujesz jedno mocne uderzenie po czym wyprowadzasz rzeczywisty cios w innego przeciwnika!", ch, NULL, tch, TO_CHAR );
									act( "$n markuje jedno mocne uderzenie po czym wyprowadza rzeczywisty cios w twoim kierunku!", ch, NULL, tch, TO_VICT );
									act( "$n markuje jedno mocne uderzenie po czym wyprowadza rzeczywisty cios w innego przeciwnika!", ch, NULL, tch, TO_NOTVICT );
									break;
								case 6:
									act( "Bierzesz szeroki zamach i jednocze�nie atakujesz innego wroga!", ch, NULL, tch, TO_CHAR );
									act( "$n bierze szeroki zamach i jednocze�nie atakuje ciebie!", ch, NULL, tch, TO_VICT );
									act( "$n bierze szeroki zamach i jednocze�nie atakuje innego wroga!", ch, NULL, tch, TO_NOTVICT );
									break;
								case 7:
									act( "Przez chwile obserwujesz sytuacje dooko�a ciebie, a gdy przeciwnicy ustawiaj� si� blisko siebie pr�bujesz jednym mocnym ciosem zrani� wi�cej ni� jednego.", ch, NULL, tch, TO_CHAR );
									act( "$n przez chwile obserwuje sytuacje dooko�a, a gdy $s przeciwnicy ustawiaj� si� blisko siebie wyprowadza jedno zamaszyste ci�cie pr�buj�c zrani� wi�cej ni� jednego.", ch, NULL, tch, TO_VICT );
									act( "$n przez chwile obserwuje sytuacje dooko�a, a gdy $s przeciwnicy ustawiaj� si� blisko siebie wyprowadza jedno zamaszyste ci�cie pr�buj�c zrani� wi�cej ni� jednego.", ch, NULL, tch, TO_NOTVICT );
									break;
								case 8:
									act( "Wykonujesz zamaszysty p�obr�t atakuj�c przy okazji kolejn� ofiar�!", ch, NULL, tch, TO_CHAR );
									act( "$n wykonuje zamaszysty p�obr�t atakuj�c ciebie przy okazji!", ch, NULL, tch, TO_VICT );
									act( "$n wykonuje zamaszysty p�obr�t atakuj�c przy okazji kolejn� ofiar�!", ch, NULL, tch, TO_NOTVICT );
									break;
								case 9:
									act( "Odczekawszy chwil� wykorzystujesz sprzyjaj�cy moment aby za jednym razem zaatakowa� wi�cej ni� jednego nieprzyjaciela!", ch, NULL, tch, TO_CHAR );
									act( "$n odczekawszy chwil� wykorzystuje sprzyjaj�cy moment aby za jednym razem zaatakowa� wi�cej ni� jednego nieprzyjaciela!", ch, NULL, tch, TO_VICT );
									act( "$n odczekawszy chwil� wykorzystuje sprzyjaj�cy moment aby za jednym razem zaatakowa� wi�cej ni� jednego nieprzyjaciela!", ch, NULL, tch, TO_NOTVICT );
									break;
								case 10:
									act( "Robisz zdecydowany zw�d markuj�c seri� cios�w w jednego przeciwnika, jednak w ostatniej chwili odskakujesz i atakujesz innego rywala!", ch, NULL, tch, TO_CHAR );
									act( "$n robi zdecydowany zw�d markuj�c seri� cios�w w jednego przeciwnika, jednak w ostatniej chwili odskakuje i atakuje ci�!", ch, NULL, tch, TO_VICT );
									act( "$n robi zdecydowany zw�d markuj�c seri� cios�w w jednego przeciwnika, jednak w ostatniej chwili odskakuje i atakuje innego rywala!", ch, NULL, tch, TO_NOTVICT );
									break;
							}

							remembered_hp = tch->hit;
							if( tch != ch->fighting )
								one_hit( ch, tch, gsn_cleave, FALSE );
							else
							{
								act( "Tw�j cios ze �wistem przebija si� przez cia�o $Z, kt�re nie stawi�o �adnego oporu.", ch, NULL, tch, TO_CHAR );
								act( "Cios $z ze �wistem przebija si� przez twoje lustrzane odbicie, kt�re znika zaraz potem.", ch, NULL, tch, TO_VICT );
								act( "Cios $z ze �wistem przebija si� przez cia�o $Z, kt�re nie stawi�o �adnego oporu.", ch, NULL, tch, TO_NOTVICT );
								remove_mirror( ch, tch, 1 );
								remembered_hp -= 1;//by nie przerwalo
							}
							++cleave_count;
							if( remembered_hp == tch->hit ) stop = TRUE;
						}
					}
				}
			}
		}
	}


	return;
}

char specdam_text_ch[MAX_INPUT_LENGTH];
char specdam_text_victim[MAX_INPUT_LENGTH];
char specdam_text_room[MAX_INPUT_LENGTH];
/*
 * Calculating bonus from spec weapon damages
 */
int calculate_spec_damage( CHAR_DATA *ch, CHAR_DATA *vch, OBJ_DATA *weapon, int spec_target )
{
	SPEC_DAMAGE *specdam;
	int bonus = 0;
	bool check1, check2;

	for ( specdam = weapon->spec_dam; specdam; specdam = specdam->next )
	{
	        if ( specdam->target_type != spec_target )
		  continue;
		switch ( specdam->type )
		{
			/*			case SD_TYPE_MAGIC:
						if ( number_percent() <= specdam->chance )
						{
						tmp = check_resist( vch, specdam->param1, specdam->bonus );
						if ( tmp <= 0 )
						continue;
						bonus += tmp;
						switch ( specdam->param1 )
						{
						case DAM_FIRE:
						sprintf( specdam_text_ch,	 "Przypalasz $j cia�o $Z." );
						sprintf( specdam_text_victim, "$n przypala ci sk�r� $j." );
						sprintf( specdam_text_room,   "$n przypala $j cia�o $Z." );
						break;
						}
						}
						break;*/
			case SD_TYPE_RACE:
				if ( !vch )
					break;
				if ( ( specdam->param1 == SD_PARAM_ANY || GET_RACE(vch) == specdam->param1 ) &&
						( specdam->param2 == SD_PARAM_ANY ||  GET_RACE(ch) == specdam->param2 ) &&
						( number_percent() <= specdam->chance ) )
					bonus += specdam->bonus;
				break;
			case SD_TYPE_ALIGN:
				check1 = check2 = FALSE;
				if ( vch && (( specdam->param1 == SD_PARAM_ANY ) ||
						( specdam->param1 == 0 && IS_EVIL( vch ) ) ||
						( specdam->param1 == 1 && IS_GOOD( vch ) ) ||
						( specdam->param1 == 2 && IS_NEUTRAL( vch ) )) )
					check1 = TRUE;
				if ( ( specdam->param2 == SD_PARAM_ANY ) ||
						( specdam->param2 == 0 && IS_EVIL( ch ) ) ||
						( specdam->param2 == 1 && IS_GOOD( ch ) ) ||
						( specdam->param2 == 2 && IS_NEUTRAL( ch ) ) )
					check2 = TRUE;

				if ( check1 && check2 && number_percent() <= specdam->chance )
					bonus += specdam->bonus;

				break;
			case SD_TYPE_SIZE:
				if ( ( specdam->param1 == SD_PARAM_ANY || ( vch && (vch->size == specdam->param1) ) ) &&
						( specdam->param2 == SD_PARAM_ANY ||  ch->size == specdam->param2 ) &&
						( number_percent() <= specdam->chance ) )
					bonus += specdam->bonus;
				break;
			case SD_TYPE_SELFSTAT:
				if ( num_eval( get_curr_stat_deprecated( ch, specdam->param1 ), specdam->param2, specdam->param3 ) &&
						number_percent() <= specdam->chance )
					bonus += specdam->bonus;
				break;
			case SD_TYPE_TARGETSTAT:
				if ( !vch )
					break;
				if ( num_eval( get_curr_stat_deprecated( vch, specdam->param1 ), specdam->param2, specdam->param3 ) &&
						number_percent() <= specdam->chance )
					bonus += specdam->bonus;
				break;
			case SD_TYPE_CLASS:
				if ( !IS_NPC( ch ) && ch->class == specdam->param1 && (number_percent() <= specdam->chance) )
					bonus += specdam->bonus;
				break;
			case SD_TYPE_ACT:
				if ( vch && IS_NPC( vch ) && ext_flags_same( ext_flags_intersection( vch->act, specdam->ext_param ), specdam->ext_param ) && (number_percent() <= specdam->chance) )
					bonus += specdam->bonus;
				break;
		}
	}
	return bonus;
}

void weapon_dispeller( CHAR_DATA *ch, CHAR_DATA *victim, int base_dam )
{
	AFFECT_DATA * aff;
	AFFECT_DATA * aff_next;
	OBJ_DATA * obj;
	OBJ_DATA * obj_next;
	AFFECT_DATA	* spells_found[ 12 ];
	int spells_found_count = 0, i, gsn;
	bool greater = FALSE;

	i = number_percent();
	if (i > 20)
	{
		return;
	}

	if (i <= 5)
    {
		greater = TRUE;
    }

	if (!greater)
    {
		for ( i = 0; i < 12; i++ )
        {
			spells_found[ i ] = 0;
        }
		for ( aff = victim->affected; aff; aff = aff->next )
		{
			/* tylko te z tablicy spelli, nie wiem czy mo�e si� zdarzyc inaczej */
			if ( aff->type <= 0 || aff->type >= MAX_SKILL )
				continue;
			/* tylko spelle */
			if ( skill_table[ aff->type ].spell_fun == spell_null )
				continue;

			if ( aff->level > 40 ||
			     aff->type == gsn_energy_drain ||
			     aff->type == gsn_on_smoke ||
			     aff->type == gsn_feeblemind ||
			     aff->type == gsn_bleeding_wound ||
			     aff->type == gsn_life_transfer ||
			     aff->type == gsn_energy_strike ||
			     aff->type == 329 || //razorblade hands
			     aff->type == 331 ) //beast claws
				continue;

			spells_found[ spells_found_count++ ] = aff;

			if ( spells_found_count > 11 )
				break;
		}

		if ( spells_found_count == 0 )
		{
			return ;
		}

		i = number_range( 0, spells_found_count - 1 );

		if ( ch->level + number_range( 1, 20 ) <= 12 + spells_found[ i ] ->level )
		{
			return ;
		}

		gsn = spells_found[ i ] ->type;
		affect_strip( victim, gsn );

		if ( skill_table[ gsn ].msg_off )
		{
			send_to_char( skill_table[ gsn ].msg_off, victim );
			send_to_char( "\n\r", victim );
		}
	}

	else

	{

//	if (greater) {
	    for ( aff = victim->affected; aff; aff = aff_next )
		{
			aff_next = aff->next;


			//spelle resistowe maja % w polu level, czyli niedaloby sie ich zdispelowac przez to. Juz lepiej by sie dalo niezaleznie od lewa
			if( aff->level > 40 &&
				aff->type != gsn_resist_fire &&
				aff->type != 246 &&//stom shell
				aff->type != gsn_resist_lightning &&
				aff->type != gsn_resist_cold &&
				aff->type != 143 &&//resist acid
				aff->type != gsn_brave_cloak &&//resist acid
				aff->type != 178 &&//resist elements
				aff->type != 179 &&//lesser magic resist
				aff->type != gsn_steel_scarfskin &&
				aff->type != gsn_energy_shield )
				continue;

			affect_remove( victim, aff );
			spells_found_count ++;

			if ( skill_table[aff->type].msg_off )
			{
				send_to_char( skill_table[aff->type].msg_off, victim );
				send_to_char( "\n\r", victim );
			}

//		affect_strip( victim, aff->type );
		}
		for ( obj = victim->carrying; obj != NULL; obj = obj_next )
		{
			OBJ_NEXT_CONTENT( obj, obj_next );

			if ( obj->pIndexData->vnum == OBJ_VNUM_LIGHT_BALL || obj->pIndexData->vnum == OBJ_VNUM_GOOD_BALL || obj->pIndexData->vnum == OBJ_VNUM_NEUTRAL_BALL || obj->pIndexData->vnum == OBJ_VNUM_EVIL_BALL )
			{
				act( "Kula �wiat�a rozpada si� na kawa�eczki.", ch, NULL, NULL, TO_ALL );
				extract_obj(obj);
			}

			if ( obj->pIndexData->vnum == OBJ_VNUM_FLAMEBLADE )
			{
				act( "P�on�ce ostrze ga�nie z lekkim sykiem i znika.", ch, NULL, NULL, TO_ALL );
				extract_obj(obj);
			}

			if ( obj->pIndexData->vnum == OBJ_VNUM_EYE_OF_VISION )
			{
				act( "Oko widzenia zamyka si� i znika.", ch, NULL, NULL, TO_ALL );
				extract_obj(obj);
			}

            if (
                    FALSE
                    || obj->pIndexData->vnum == OBJ_VNUM_HOLY_HAMMER
                    || obj->pIndexData->vnum == OBJ_VNUM_HOLY_FLAIL
                    || obj->pIndexData->vnum == OBJ_VNUM_SPIRITUAL_STAFF
                    || obj->pIndexData->vnum == OBJ_VNUM_SPIRITUAL_FLAIL
                    || obj->pIndexData->vnum == OBJ_VNUM_SPIRITUAL_SHORTSWORD
                    || obj->pIndexData->vnum == OBJ_VNUM_SPIRITUAL_HAMMER
					|| obj->pIndexData->vnum == OBJ_VNUM_SPIRITUAL_SWORD
                    || obj->pIndexData->vnum == OBJ_VNUM_GOODBARRY
                    || obj->pIndexData->vnum == OBJ_VNUM_MUSHROOM
               )
			{
				act( "$p migocze przez chwil� i znika.", ch, obj, NULL, TO_ALL );
				extract_obj(obj);
			}

			if ( obj->pIndexData->vnum == OBJ_VNUM_FLOATING_SKULL || obj->pIndexData->vnum == OBJ_VNUM_CREATE_SYMBOL_EVIL || obj->pIndexData->vnum == OBJ_VNUM_CREATE_SYMBOL_NEUTRAL || obj->pIndexData->vnum == OBJ_VNUM_CREATE_SYMBOL_GOOD )
			{
				act( "$p migocze przez chwil� i rozpda si� na kawa�eczki.", ch, obj, NULL, TO_ALL );
				extract_obj(obj);
			}

			if ( obj->pIndexData->vnum == OBJ_VNUM_SHADOW_DAGGER || obj->pIndexData->vnum == OBJ_VNUM_SHADOW_SHORT_SWORD || obj->pIndexData->vnum == OBJ_VNUM_SHADOW_SHORT_SWORD_VORPAL || obj->pIndexData->vnum == OBJ_VNUM_SHADOW_SHORT_SWORD_DISPELLER )
			{
				act( "Ostrze cienia migocze przez chwil� i znika.", ch, NULL, NULL, TO_ALL );
				extract_obj(obj);
			}

			if ( obj->pIndexData->vnum == OBJ_VNUM_FALSE_RAZORBLADE || obj->pIndexData->vnum == OBJ_VNUM_NORMAL_RAZORBLADE || obj->pIndexData->vnum == OBJ_VNUM_SHARP_RAZORBLADE || obj->pIndexData->vnum == OBJ_VNUM_VORPAL_RAZORBLADE || obj->pIndexData->vnum == OBJ_VNUM_FALSE_CLAW || obj->pIndexData->vnum == OBJ_VNUM_NORMAL_CLAW || obj->pIndexData->vnum == OBJ_VNUM_SHARP_CLAW || obj->pIndexData->vnum == OBJ_VNUM_VORPAL_CLAW )
			{
				act( "Twa d�o� powraca do naturalnego stanu.", victim, NULL, NULL, TO_CHAR );
				act( "D�o� $z powraca do naturalnego stanu.", victim, NULL, NULL, TO_ROOM );
				extract_obj(obj);
			}

			if ( obj->pIndexData->vnum == OBJ_VNUM_SPIRITUAL_ARMOR_HEAD || obj->pIndexData->vnum == OBJ_VNUM_SPIRITUAL_ARMOR_BODY || obj->pIndexData->vnum == OBJ_VNUM_SPIRITUAL_ARMOR_LEGS || obj->pIndexData->vnum == OBJ_VNUM_SPIRITUAL_ARMOR_ARMS || obj->pIndexData->vnum == OBJ_VNUM_SPIRITUAL_ARMOR_HANDS || obj->pIndexData->vnum == OBJ_VNUM_SPIRITUAL_ARMOR_FEET || obj->pIndexData->vnum == OBJ_VNUM_DISC )
			{
				if ( obj->liczba_mnoga )
					act( "$p migocz� przez chwil� i rozpadaj� si�.", victim, obj, NULL, TO_ALL );
				else
					act( "$p migocze przez chwil� i rozpada si�.", victim, obj, NULL, TO_ALL );
				extract_obj(obj);
			}
		}

		if (spells_found_count)
		{
			act( "Rozpraszasz wszystkie zakl�cia wp�ywaj�ce na $C.", ch, NULL, victim, TO_CHAR );
			act( "$n rozprasza wszystkie wp�ywaj�ce na ciebie zakl�cia.", ch, NULL, victim, TO_VICT );
			act( "$n rozprasza wszystkie zakl�cia wp�ywaj�ce na $C.", ch, NULL, victim, TO_NOTVICT );
		}
	}
	if ( ( !victim->mount && get_carry_weight( victim ) > can_carry_w( victim ) && IS_AFFECTED( victim, AFF_FLYING ) ) ||
	     ( ( !victim->mount && get_carry_weight( victim ) > can_carry_w( victim ) && IS_AFFECTED( victim, AFF_FLOAT ) ) ) )
	{
		act( "$n pod wp�ywem ci�aru opada na ziemi�.", victim, NULL, NULL, TO_ROOM );
		send_to_char( "Pod wp�ywem obci��enia opadasz na ziemi�.\n\r", victim );
		affect_strip( victim, gsn_fly );
		affect_strip( victim, gsn_float );
	}
	return;
}


/*
 * Hit one guy once.
 */
char fight_text_ch[MAX_INPUT_LENGTH];
char fight_text_victim[MAX_INPUT_LENGTH];
char fight_text_room[MAX_INPUT_LENGTH];
int to_hit_bonus;
int to_dam_bonus;
bool accident = FALSE;
/* do_one_hit*/
void one_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt, bool second )
{
	OBJ_DATA * wield;
	OBJ_DATA * obj, *obj_next, *damned_armor, *damned_weapon;
	CHAR_DATA *tch;
	AFFECT_DATA *paf;
	AFFECT_DATA af;
	int holy_mod1 = 0, holy_mod2 = 0;
	int victim_ac;
	int thac0;
	int dam, base_dam = 0, unarmed_base_dam = 0;
	int diceroll;
	int sn = -1, skill;
	int dam_type;
	bool result = TRUE, immune = FALSE, critical = FALSE, prog_mod = FALSE, dreadful = FALSE, check;
	int twohander_fight_style = 0;
	AFFECT_DATA *scarfskin, *damned, bleeding_wound, *defense_curl;
	int weapon_mastery = 0, overwhelming_skill = 0, over_chance = 0, over_hap;
	OBJ_DATA *body;
	OBJ_DATA *arms;
	OBJ_DATA *legs;
	OBJ_DATA *hands;
	OBJ_DATA *feet;
	OBJ_DATA *head;
	OBJ_DATA *bron;

	DEBUG_INFO( "one_hit:checks" );
	if ( victim == ch || ch == NULL || victim == NULL )
		return;

	if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
		return;

	if ( check_blink( ch, victim, FALSE ) )
	{
		send_to_char( "Pr�bujesz wyprowadzi� cios, jednak przeskakujesz na chwilk� do innego planu egzystencji i przeciwnik znika ci z oczu.\n\r", ch );
		return;
	}

    if ( !second )
    {
        wield = get_eq_char( ch, WEAR_WIELD );
    }
    else
    {
        wield = get_eq_char( ch, WEAR_SECOND );
    }

	DEBUG_INFO( "onehit:twohander_fighting" );
	if ( (!second && wield && IS_WEAPON_STAT( wield, WEAPON_TWO_HANDS)))
	{
		twohander_fight_style = get_skill( ch, gsn_twohander_fighting );

		if ( twohander_fight_style > 0 )
			check_improve( ch, victim, gsn_twohander_fighting, TRUE, 180 );

		if( get_skill( ch, gsn_two_hands_fighting ) > 0 )
			check_improve( ch, victim, gsn_two_hands_fighting, TRUE, 210 );
	}

	DEBUG_INFO( "onehit:dual_fighting" );
	if ( ( second && wield && get_eq_char( ch, WEAR_WIELD ) ) ||
			( !second && wield && get_eq_char( ch, WEAR_SECOND ) ) )
	{

		if((ch->class == CLASS_WARRIOR ||
					ch->class == CLASS_BARBARIAN ||
					ch->class == CLASS_BARD ||
					ch->class == CLASS_PALADIN ) &&
				get_skill( ch, gsn_two_weapon_fighting ) )
		{
			check_improve( ch, victim, gsn_two_weapon_fighting, TRUE, 210 );
		}
	}

    //uczenie sie i podstawianie masterow
    OBJ_DATA * weapon = NULL;
    if (
           ( !second && ( weapon = get_eq_char( ch, WEAR_WIELD  ) ) ) ||
           (  second && ( weapon = get_eq_char( ch, WEAR_SECOND ) ) )
       )
    {
        switch ( ch->class )
        {
            case CLASS_BLACK_KNIGHT:
                switch ( weapon->value[0] )
                {
                    case WEAPON_AXE:
                        weapon_mastery = one_hit_check_mastery( ch, victim, gsn_axe_mastery, 16 );
                        break;
                    case WEAPON_POLEARM:
                        weapon_mastery = one_hit_check_mastery( ch, victim, gsn_polearm_mastery, 16 );
                        break;
                    case WEAPON_SWORD:
                        weapon_mastery = one_hit_check_mastery( ch, victim, gsn_sword_mastery, 16 );
                        break;
                    case WEAPON_WHIP:
                        weapon_mastery = one_hit_check_mastery( ch, victim, gsn_whip_mastery, 16 );
                        break;
                }
                break;
            case CLASS_PALADIN:
                switch ( weapon->value[0] )
                {
                    case WEAPON_FLAIL:
                        weapon_mastery = one_hit_check_mastery( ch, victim, gsn_flail_mastery, 16 );
                        break;
                    case WEAPON_MACE:
                        weapon_mastery = one_hit_check_mastery( ch, victim, gsn_mace_mastery, 16 );
                        break;
                    case WEAPON_SWORD:
                        weapon_mastery = one_hit_check_mastery( ch, victim, gsn_sword_mastery, 16 );
                        break;
                }
                break;
            case CLASS_THIEF:
                if ( weapon->value[0] == WEAPON_DAGGER )
                {
                    weapon_mastery = one_hit_check_mastery( ch, victim, gsn_dagger_mastery, 16 );
                }
                break;
            case CLASS_WARRIOR:
                switch ( weapon->value[0] )
                {
                    case WEAPON_AXE:
                        weapon_mastery = one_hit_check_mastery( ch, victim, gsn_axe_mastery, 16 );
                        break;
                    case WEAPON_DAGGER:
                        weapon_mastery = one_hit_check_mastery( ch, victim, gsn_dagger_mastery, 16 );
                        break;
                    case WEAPON_FLAIL:
                        weapon_mastery = one_hit_check_mastery( ch, victim, gsn_flail_mastery, 16 );
                        break;
                    case WEAPON_MACE:
                        weapon_mastery = one_hit_check_mastery( ch, victim, gsn_mace_mastery, 16 );
                        break;
                    case WEAPON_POLEARM:
                        weapon_mastery = one_hit_check_mastery( ch, victim, gsn_polearm_mastery, 16 );
                        break;
                    case WEAPON_SHORTSWORD:
                        weapon_mastery = one_hit_check_mastery( ch, victim, gsn_shortsword_mastery, 16 );
                        break;
                    case WEAPON_SPEAR:
                        weapon_mastery = one_hit_check_mastery( ch, victim, gsn_spear_mastery, 16 );
                        break;
                    case WEAPON_STAFF:
                        weapon_mastery = one_hit_check_mastery( ch, victim, gsn_staff_mastery, 16 );
                        break;
                    case WEAPON_SWORD:
                        weapon_mastery = one_hit_check_mastery( ch, victim, gsn_sword_mastery, 16 );
                        break;
                    case WEAPON_WHIP:
                        weapon_mastery = one_hit_check_mastery( ch, victim, gsn_whip_mastery, 16 );
                        break;
                }
                break;
        }
    }

	if ( dt == TYPE_UNDEFINED )
	{
		dt = TYPE_HIT;
		if ( wield != NULL && wield->item_type == ITEM_WEAPON )
			dt += wield->value[ 3 ];
		else
			dt += ch->dam_type;
	}

	if ( dt < TYPE_HIT )
	{
		if ( wield != NULL )
		{
			switch ( wield->value[ 0 ] )
			{
				case WEAPON_SWORD: dam_type = DAM_SLASH;	break;
				case WEAPON_MACE: dam_type = DAM_BASH;	break;
				case WEAPON_DAGGER: dam_type = DAM_PIERCE;	break;
				case WEAPON_AXE: dam_type = DAM_SLASH;	break;
				case WEAPON_SPEAR: dam_type = DAM_PIERCE;	break;
				case WEAPON_FLAIL: dam_type = DAM_BASH;	break;
				case WEAPON_WHIP: dam_type = DAM_SLASH;	break;
				case WEAPON_POLEARM: dam_type = DAM_SLASH;	break;
				case WEAPON_STAFF: dam_type = DAM_BASH;	break;
				case WEAPON_SHORTSWORD: dam_type = DAM_SLASH;	break;
				case WEAPON_CLAWS: dam_type = DAM_SLASH; break;
				default: dam_type = DAM_BASH;	break;
			}
			//dam_type = attack_table[wield->value[3]].damage;
		}
		else
        {
            dam_type = attack_table[ ch->dam_type ].damage;
        }
    }
    else
    {
        dam_type = attack_table[ dt - TYPE_HIT ].damage;
    }

    if ( dam_type == -1 )
    {
        dam_type = DAM_BASH;
    }

	DEBUG_INFO( "onehit:get_weapon_skill" );
	sn = get_weapon_sn( ch, !second );
	skill = get_weapon_skill( ch, sn );


	DEBUG_INFO( "onehit:get_AC" );
	switch ( dam_type )
	{
		case( DAM_PIERCE ) : victim_ac = GET_AC( victim, AC_PIERCE ) / 10;	break;
		case( DAM_BASH ) : victim_ac = GET_AC( victim, AC_BASH ) / 10;	break;
		case( DAM_SLASH ) : victim_ac = GET_AC( victim, AC_SLASH ) / 10;	break;
		default: victim_ac = GET_AC( victim, AC_EXOTIC ) / 10;	break;
	};

#ifdef INFO
    print_char( ch, "victim_ac: %d\n\r", victim_ac );
#endif

    DEBUG_INFO( "onehit:ac_modifiers" );
    if ( !can_see( ch, victim ) )
    {
        victim_ac -= 4;
#ifdef INFO
        print_char( ch, "victim_ac (can_see): %d\n\r", victim_ac );
#endif
    }

	if ( !IS_EVIL(ch) && victim->class == CLASS_BLACK_KNIGHT && ( damned_armor = get_eq_char(victim,WEAR_BODY)) != NULL )
	{
		for ( damned = damned_armor->affected; damned != NULL; damned = damned->next )
			if ( damned->type == gsn_damn_armor )
            {
                victim_ac -= URANGE( 1, damned->level/12 +  get_skill(victim,gsn_damn_armor)/33, 4 );
				break;
			}
	}

	if ( victim->position < POS_FIGHTING )
    {
		victim_ac += 3;
    }

	if ( victim->position < POS_RESTING )
    {
		victim_ac += 6;
    }
#ifdef INFO
    print_char( ch, "victim_ac (position): %d\n\r", victim_ac );
#endif

    if ( IS_SET( victim->fight_data, FIGHT_CASTING ) )
    {
        victim_ac += 3;
#ifdef INFO
        print_char( ch, "victim_ac (FIGHT_CASTING): %d\n\r", victim_ac );
#endif
    }

    if ( IS_SET( victim->fight_data, FIGHT_VERTICAL_SLASH ) )
    {
        victim_ac += 6;
#ifdef INFO
        print_char( ch, "victim_ac (FIGHT_VERTICAL_SLASH): %d\n\r", victim_ac );
#endif
    }

    if ( victim->daze > 0 )
    {
        victim_ac += 4;
#ifdef INFO
        print_char( ch, "victim_ac (daze): %d\n\r", victim_ac );
#endif
    }

    /**
     * aura of protection
     */
    if ( has_aura( victim, FALSE ) )
    {
        if ( get_aura_sn( victim ) == 462 )
        {
            victim_ac += get_aura_modifier( victim );
#ifdef INFO
            print_char( ch, "victim_ac (aura of protection): %d\n\r", victim_ac );
#endif
        }
    }

	DEBUG_INFO( "onehit:compute_thac0" );
	thac0 = compute_tohit( ch, !second );

	if ( wield )
	{
		if ( dt == gsn_charge )
			thac0 -= 2;
		else if ( dt == gsn_backstab )
        {
            thac0 -= URANGE( 4, get_skill(ch,gsn_backstab)/10, 10 );
            /**
             * bonus ze sztyletu z flaga WEAPON_HEARTSEEKER
             */
            if ( !second && IS_SET( wield->value[ 4 ], WEAPON_HEARTSEEKER ) )
            {
                thac0 -= 2;
            }
        }
		else if ( dt == gsn_smite || dt == gsn_smite_good )
			thac0 -= 2;
		else if ( dt == gsn_stun )
			thac0 += 4;
		else if ( dt == gsn_cleave )
			thac0 -= 3;//bo koles nie jest przygotowany na atak z twojej strony
		else if ( dt == gsn_vertical_slash )
			thac0 += 2;
	}
	DEBUG_INFO( "onehit:thac0_conse/desecrate_mod" );
	if (EXT_IS_SET( ch->in_room->room_flags, ROOM_CONSECRATE))
	{
		if( IS_EVIL(ch))
			thac0 += number_range(1,3);
		if( IS_GOOD(ch))
			thac0 -= number_range(1,3);
	}

	if (EXT_IS_SET( ch->in_room->room_flags, ROOM_DESECRATE))
	{
		if( IS_EVIL(ch))
			thac0 -= number_range(1,3);
		if( IS_GOOD(ch))
			thac0 += number_range(1,3);
	}

	//premie i kary za rozmiary
	DEBUG_INFO( "onehit:thac0_size_mod" );
	//tu jak jest wiekszy - kara za bicie w malego
	if ( ch->size - victim->size == 1 )
		thac0 += 2;
	else if ( ch->size - victim->size == 2 )
		thac0 += 3;
	else if ( ch->size - victim->size == 3 )
		thac0 += 4;
	else if ( ch->size - victim->size >= 4 )
		thac0 += 5;

	//jak jest mniejszy - poczatkowo premia, ale jak jest za duzy to tez kara
	if ( ch->size - victim->size == -1 )
		thac0 -= 2;
	else if ( ch->size - victim->size == -2 )
		;
	else if ( ch->size - victim->size == -3 )
		thac0 += 2;
	else if ( ch->size - victim->size <= -4 )
		thac0 += 4;

	if ( !IS_EVIL(victim) && ch->class == CLASS_BLACK_KNIGHT && ( damned_weapon = get_eq_char(ch,WEAR_WIELD)) != NULL )
	{
		for ( damned = damned_weapon->affected; damned != NULL; damned = damned->next )
			if ( damned->type == gsn_damn_weapon )
			{
				if ( IS_GOOD( victim ) && !IS_AFFECTED( victim, AFF_PROTECT_EVIL ))
					thac0 -= URANGE( 1, damned->level/10 +  get_skill(victim,gsn_damn_weapon)/24, 6 );
				else
					thac0 -= URANGE( 1, damned->level/12 +  get_skill(victim,gsn_damn_weapon)/33, 4 );
				break;
			}
	}

	//goody w grupce z death knightem z demon aura maja minusy do trafien
	for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
	{
		if( IS_GOOD(ch) && is_affected(tch,gsn_demon_aura) && is_same_group(ch,tch))
        {
			thac0 += URANGE( 1, get_skill(tch,gsn_demon_aura)/16, 6);
        }
	}
	tch = NULL;

	if ( is_affected(ch,gsn_magic_hands) && !get_eq_char(ch,WEAR_WIELD) )
		thac0 -= URANGE(1, ch->level/5, 5);

	DEBUG_INFO( "onehit:trig_onehit" );
	to_hit_bonus = 0;
	to_dam_bonus = 0;
	fight_text_ch[ 0 ] = '\0';
	fight_text_victim[ 0 ] = '\0';
	fight_text_room[ 0 ] = '\0';

	/*onehit trigger for mobiles*/
	if ( !wield && IS_NPC( ch ) && HAS_TRIGGER( ch, TRIG_ONEHIT ) )
		prog_mod = mp_onehit_trigger( ch, victim );
	else if ( wield && HAS_OTRIGGER( wield, TRIG_ONEHIT ) )
		prog_mod = op_onehit_trigger( wield, ch );

	if ( !victim->in_room )
		return;

	//mo�e nie mie� ju� broni (prog onehit)
    if ( !second )
    {
        wield = get_eq_char( ch, WEAR_WIELD );
    }
    else
    {
        wield = get_eq_char( ch, WEAR_SECOND );
    }

    if ( prog_mod && to_hit_bonus != 0 )
    {
        thac0 -= to_hit_bonus;
    }

	check_defensive_spells( ch, victim );

    if ( ch->position < POS_RESTING || !ch->in_room )
    {
        return;
    }

	while ( ( diceroll = number_bits( 5 ) ) >= 20 );

	/* standardowe rzeczy przeklejone z damage(...)*/

	DEBUG_INFO( "onehit:standard_checks" );
	if ( victim->position == POS_DEAD )
		return;

	if ( victim != ch )
	{
		DEBUG_INFO( "onehit:is_safe" );
		if ( is_safe( ch, victim ) )
			result = FALSE;

		DEBUG_INFO( "onehit:trig_kill" );
		if ( victim->position > POS_STUNNED )
		{
			if ( victim->fighting == NULL )
			{
				set_fighting( victim, ch );

				if ( IS_NPC( victim ) && HAS_TRIGGER( victim, TRIG_KILL ) )
					mp_percent_trigger( victim, ch, NULL, NULL, &TRIG_KILL );

				if ( !ch->in_room )
					return;

				//mo�e nie mie� ju� broni (prog kill)
				if ( !second )
					wield = get_eq_char( ch, WEAR_WIELD );
				else
					wield = get_eq_char( ch, WEAR_SECOND );
			}
		}

		DEBUG_INFO( "onehit:set_fighting" );
		if ( ch->position > POS_STUNNED )
		{
			if ( ch->fighting == NULL )
				set_fighting( ch, victim );
		}

		DEBUG_INFO( "onehit:charm_stop_follower" );
		if ( victim->master == ch && !accident )
		{
			if( IS_NPC( victim ) && IS_AFFECTED(victim,AFF_CHARM) )
			{
				act( "Wi� mi�dzy tob� a $V zostaje brutalnie przerwana!", ch, NULL, victim, TO_CHAR );
				raw_damage( ch, ch, number_range( ch->level/2, ch->level+ch->level/2 ) );
				if ( ch->position <= POS_STUNNED )
					return;
			}
			stop_follower( victim );
		}

		DEBUG_INFO( "onehit:strip_hide/invis" );
		if ( IS_AFFECTED( ch, AFF_HIDE ) )
			affect_strip( ch, gsn_hide );

		strip_invis( ch, TRUE, TRUE );

		DEBUG_INFO( "onehit:start_hating" );
		if ( IS_NPC( victim ) && !is_fearing( victim, ch ) && !accident && !is_hating( victim, ch ) )
		{
			if ( !EXT_IS_SET( victim->act, ACT_SENTINEL ) )
            {
				start_hunting( victim, ch );
            }
			start_hating( victim, ch );
		}
	}
	/* koniec standardowych rzeczy*/

#ifdef INFO
    print_char( ch, "diceroll: %d\n\r", diceroll );
#endif
    /**
     * aura of precision
     */
    if ( has_aura( ch, FALSE ) )
    {
        if ( get_aura_sn( ch ) == 463 )
        {
            diceroll += get_aura_modifier( ch );
#ifdef INFO
            print_char( ch, "DAM + aura of precision: %d\n\r", diceroll );
#endif
        }
    }

    /**
     * WEAPON_HEARTSEEKER
     */
    DEBUG_INFO( "onehit:WEAPON_HEARTSEEKER" );
    if ( wield && IS_SET( wield->value[ 4 ], WEAPON_HEARTSEEKER ) )
    {
        diceroll += 1;
#ifdef INFO
        print_char( ch, "WEAPON_HEARTSEEKER: %d\n\r", diceroll );
#endif
    }

    /**
     * WEAPON_UNBALANCED
     */
    DEBUG_INFO( "onehit:WEAPON_UNBALANCED" );
    if ( wield && IS_SET( wield->value[ 4 ], WEAPON_UNBALANCED ) )
    {
        if ( number_percent() < 15 )
        {
            diceroll = 0;
        }
#ifdef INFO
        print_char( ch, "WEAPON_UNBALANCED: %d\n\r", diceroll );
#endif
    }

    DEBUG_INFO( "onehit:miss" );
    /*czesc kodu przy nietrafieniu kogostam*/
    if ( ( diceroll == 0 || ( can_move( victim ) && diceroll != 19 && diceroll < thac0 - victim_ac ) ) )
    {
        if ( !special_damage_message( ch, victim, wield, 0, dt, FALSE ) )
        {
            generate_onehit_msg( ch, victim, 0, 0, FALSE );
        }
        if ( sn != -1 )
        {
            check_improve( ch, victim, sn, FALSE, 200 );
        }
        return;
    }
	/* koniec czesci kodu przy nie-trafieniu kogostam */
	
	DEBUG_INFO( "onehit:trick_weapon_wrench" );
	if( wield &&
		wield->value[0] == WEAPON_WHIP &&
		( get_eq_char( victim, WEAR_WIELD) || get_eq_char( victim, WEAR_SECOND ) ) )
	{
		if( second )
			SET_BIT( ch->fight_data, FIGHT_TRICK_WIELDSECOND );

		if( !is_affected( victim, gsn_weapon_wrench ) && !is_affected( ch, gsn_weapon_wrench ) && check_trick( ch, victim, SN_TRICK_WEAPON_WRENCH ) )
		{
			if( IS_SET( ch->fight_data, FIGHT_TRICK_WIELDSECOND ) )
				REMOVE_BIT( ch->fight_data, FIGHT_TRICK_WIELDSECOND );
			return;
		}

		if( IS_SET( ch->fight_data, FIGHT_TRICK_WIELDSECOND ) )
			REMOVE_BIT( ch->fight_data, FIGHT_TRICK_WIELDSECOND );
	}

	DEBUG_INFO( "onehit:mob_hitting_without_weapon" );
	/* mobile wali z reki */
	if ( IS_NPC( ch ) && wield == NULL )
	{
		dam = dice( ch->damage[ DICE_NUMBER ], ch->damage[ DICE_TYPE ] ) + ch->damage[ DICE_BONUS ];

		unarmed_base_dam = dam;

		/* critical mobilesa*/
		if ( diceroll == 19 )
		{
			dam += dice( ch->damage[ DICE_NUMBER ], ch->damage[ DICE_TYPE ] ) + ch->damage[ DICE_BONUS ];
			critical = TRUE;
		}
	}
	else
	{
		DEBUG_INFO( "onehit:hitting_with_weapon" );
		/* jesli konkretna umiejetnosc broni to sprawdzamy czy si� nauczyl czegos*/
		if ( sn != -1 )
        {
			check_improve( ch, victim, sn, TRUE, 200 );
        }

		/* jesli uzywa jakiegos narzedzia*/
		if ( wield != NULL )
		{
			dam = dice( UMIN( wield->value[ 1 ], 8 ), UMIN( wield->value[ 2 ], 20 ) ) + UMIN( wield->value[ 6 ], 6 );
			dam += calculate_spec_damage( ch, victim, wield, SD_TARGET_DAM );
			base_dam = dam;

#ifdef INFO
			print_char( ch, "BASE_DAM:%d\n\r", base_dam );
#endif

			/* sharpness! */
			if ( IS_WEAPON_STAT( wield, WEAPON_SHARP ) && number_percent() <= ( skill / 2 ) )
			{
				dam += dam / 4;
#ifdef INFO
				print_char( ch, "+SHARPNESS: %d\n\r", dam );
#endif
			}

			if ( IS_WEAPON_STAT( wield, WEAPON_VORPAL ) )
			{
				if ( number_percent() <= ( skill / 2 ) )
					dam += dam / 3;
				else
					dam += dam / 4;
#ifdef INFO
				print_char( ch, "+VORPAL: %d\n\r", dam );
#endif
			}

			if ( IS_SET( ch->fight_data, FIGHT_MIGHTY_BLOW ) )
			{
				dam += 4;
				if( number_range(0, 100 - get_skill(ch,gsn_mighty_blow) ) < 5 ) dam += 8; //crit
#ifdef INFO
				print_char( ch, "+MIGHTY_BLOW: %d\n\r", dam );
#endif
				send_to_char( "{5Bierzesz pot�ny zamach.{x\n\r", ch );
				act( "{5$n z pot�nym zamachem rzuca si� na ciebie.{x", ch, victim, victim, TO_VICT );
				act( "{5$n z pot�nym zamachem rzuca si� na $C.{x", ch, victim, victim, TO_NOTVICT );
				REMOVE_BIT( ch->fight_data, FIGHT_MIGHTY_BLOW );
			}
			else if ( IS_SET( ch->fight_data, FIGHT_POWER_STRIKE ) )
			{
				dam += 8;
				if( number_range(0, 100 - get_skill(ch,gsn_power_strike) ) < 5 ) dam += 16; //crit
#ifdef INFO
				print_char( ch, "+POWER_STRIKE: %d\n\r", dam );
#endif
				print_char( ch, "{5Wznosisz %s nad g�ow� i wyprowadzasz pot�ny atak.{x\n\r", wield->name4 );
				act( "{5$n wznosi $h nad g�ow� i atakuje ci� z potworn� si��.{x", ch, wield, victim, TO_VICT );
				act( "{5$n wznosi $h nad g�ow� i wyprowadza pot�ny atak.{x", ch, wield, victim, TO_NOTVICT );
				REMOVE_BIT( ch->fight_data, FIGHT_POWER_STRIKE );
			}
			else if ( IS_SET( ch->fight_data, FIGHT_CRITICAL_STRIKE ) )
			{
				if( wield->value[0] == WEAPON_AXE &&
						4*(ch->height)/3 >= victim->height &&
						IS_SET( victim->parts, PART_HEAD ) &&
						( victim->resists[ RESIST_MAGIC_WEAPON ] == 0 || check_magic_attack( ch, victim, wield ) >= victim->resists[ RESIST_MAGIC_WEAPON ] ) &&
						check_trick( ch, victim, SN_TRICK_DECAPITATION ) )
					return;

				dam += 3;
				if( number_range(0, 100 - get_skill(ch,gsn_critical_strike) ) < 5 ) dam += 6; //crit
				send_to_char( "{5Z dono�nym okrzykiem atakujesz z ca�ych si�.{x\n\r", ch );
				act( "{5$n z dono�nym okrzykiem wyprowadza morderczy cios.{x", ch, victim, victim, TO_VICT );
				act( "{5$n z dono�nym okrzykiem zadaje $X morderczy cios.{x", ch, victim, victim, TO_NOTVICT );
				//poza tym symuluje krytyka
			}
			else if ( IS_SET( ch->fight_data, FIGHT_OVERWHELMING_STRIKE ) && IS_WEAPON_STAT(wield,WEAPON_TWO_HANDS ))
			{
				dam += 3 + get_skill(ch,gsn_overwhelming_strike)/10;
				if( number_range(0, 100 - get_skill(ch,gsn_overwhelming_strike) ) < 5 ) dam += 10; //crit
				//no i tutaj walimy czeka na trik dreadful strike, jak zaskoczy to pomijamy opis ktory jest w funkcji triku
				//ale dam dalej naliczamy, ktory normalnie zadamy, + obrazenia od tormenta + jakis bonusik
				if( !is_undead( victim ) &&
					!is_affected(ch,gsn_torment ) &&
					get_eq_char(ch,WEAR_WIELD) &&
					check_trick( ch, victim, SN_TRICK_DREADFUL_STRIKE ) )
				{
					dreadful = TRUE;
				}
				else
				{
					act( "{5Wk�adaj�c w ten cios wszystkie swe si�y rzucasz si� na $C trzymaj�c $h w morderczym u�cisku.{x", ch, get_eq_char( ch, WEAR_WIELD ), victim, TO_CHAR );
					act( "{5$n z potworn� si�� naciera na ciebie trzymaj�c $h w morderczym u�cisku.{x", ch, get_eq_char( ch, WEAR_WIELD ), victim, TO_VICT );
					act( "{5$n z potworn� si�� naciera na $C trzymaj�c $h w morderczym u�cisku.{x", ch, get_eq_char( ch, WEAR_WIELD ), victim, TO_NOTVICT );
				}
				//poza tym bedzie troche nizej szansa na wywrocenie
			}
			else if ( IS_SET( ch->fight_data, FIGHT_THOUSANDSLAYER ) && IS_WEAPON_STAT(wield,WEAPON_TWO_HANDS ))
			{
				dam += get_skill(ch,gsn_overwhelming_strike)/17.5;
				act( "{5Wk�adaj�c w ten cios wszystkie swe si�y rzucasz si� na $C trzymaj�c $h w morderczym u�cisku.{x", ch, get_eq_char( ch, WEAR_WIELD ), victim, TO_CHAR );
				act( "{5$n z potworn� si�� naciera na ciebie trzymaj�c $h w morderczym u�cisku.{x", ch, get_eq_char( ch, WEAR_WIELD ), victim, TO_VICT );
				act( "{5$n z potworn� si�� naciera na $C trzymaj�c $h w morderczym u�cisku.{x", ch, get_eq_char( ch, WEAR_WIELD ), victim, TO_NOTVICT );
				//poza tym bedzie troche nizej szansa na wywrocenie
				//poza tym symuluje krytyka
			}

			/* critical */
			dam = check_critical_hit( ch, victim, &diceroll, &dt, wield, &base_dam, dam, &twohander_fight_style, &weapon_mastery, &critical );

#ifdef INFO
            if( critical )
            {
                print_char( ch, "CRITICAL: %d\n\r", dam );
            }
#endif

	 		/* modyfikator obrazen ze wzgledu na uszkodzenie broni */
			//niezbyt pikne
			if ( wield->condition > 89 )
				;
			else if ( wield->condition > 84 )
				dam = UMAX( ( ( 95 * dam ) / 100 ), 1 );
			else if ( wield->condition > 74 )
				dam = UMAX( ( ( 90 * dam ) / 100 ), 1 );
			else if ( wield->condition > 64 )
				dam = UMAX( ( ( 70 * dam ) / 100 ), 1 );
			else if ( wield->condition > 44 )
				dam = UMAX( ( ( 65 * dam ) / 100 ), 1 );
			else
				dam = UMAX( dam / 2, 1 );

#ifdef INFO
			print_char( ch, "DAM -WEAPON_CONDITION: %d\n\r", dam );
#endif
		}
		else
		{
			dam = number_range( 1, 4 );
			base_dam = dam;
			/* damage z reki: 1-4 */
		}
	}

	if ( prog_mod && to_dam_bonus != 0 )
		dam += to_dam_bonus;

	//bonus do walki bronia
	if ( IS_NPC( ch ) && wield )
		dam += ch->weapon_damage_bonus;

	if ( !IS_EVIL(victim) && ch->class == CLASS_BLACK_KNIGHT && ( damned_weapon = get_eq_char(ch,WEAR_WIELD)) != NULL )
	{
		for ( damned = damned_weapon->affected; damned != NULL; damned = damned->next )
		{
			if ( damned->type == gsn_damn_weapon )
			{
				if( IS_GOOD(victim) && !IS_AFFECTED( victim, AFF_PROTECT_EVIL ))
					dam += URANGE( 1, damned->level/10 +  get_skill(victim,gsn_damn_weapon)/24, 6 );
				else
					dam += URANGE( 1, damned->level/12 +  get_skill(victim,gsn_damn_weapon)/33, 4 );
				break;
			}
		}
	}

	//dobzi slabiej bija jak sa w grupce z death knightem z demoniczna aura
	for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
	{
		if( IS_GOOD(ch) && is_affected(tch,gsn_demon_aura) && is_same_group(ch,tch))
			dam -= URANGE( 1, get_skill(tch,gsn_demon_aura)/24, 4);
	}

	//dodajemy premie za weapon_skille, weapon_mastery i style walki bronia dwureczna (przed beesem)
	dam += GET_SKILL_DAMROLL( ch, wield );

#ifdef INFO
	print_char( ch, "DAM +SKILL_DAMROLL: %d\n\r", dam );
#endif

	DEBUG_INFO( "onehit:backstab_mod" );

	/* cios w plecy dam + pierdoly ze specjalnymi rasami*/
	if ( dt == gsn_backstab && wield != NULL )
	{
		dam = get_backstabbed_dam( ch, victim, dam );
	}

	DEBUG_INFO( "onehit:conse/desecrate_mod" );
	if (EXT_IS_SET( ch->in_room->room_flags, ROOM_CONSECRATE))
	{
		if( IS_EVIL(ch))
			dam -= number_range(1,3);
		if( IS_GOOD(ch))
			dam += number_range(1,3);
	}

	if (EXT_IS_SET( ch->in_room->room_flags, ROOM_DESECRATE))
	{
		if( IS_EVIL(ch))
			dam += number_range(1,3);
		if( IS_GOOD(ch))
			dam -= number_range(1,3);
	}

	DEBUG_INFO( "onehit:GET_DAMROLL_mod" );
	/* dodajemy reszte dodatkowych obrazen (po beesie) */
	dam += GET_DAMROLL( ch, wield );

#ifdef INFO
	print_char( ch, "DAM +DAMROLL: %d\n\r", dam );
#endif

	DEBUG_INFO( "onehit:charge_mod" );
	/* bonus do obrazen: critical + bonus (mnozony damroll) */
	if ( dt == gsn_charge && wield )
	{
		switch ( wield->value[ 0 ] )
		{
			case WEAPON_SWORD:
				dam += ( 20 + skill / 4 + get_skill( ch, gsn_charge ) ) * dam / 100;
				break;
			case WEAPON_AXE:
				dam += ( 16 + skill / 4 + get_skill( ch, gsn_charge ) ) * dam / 100;
				break;
			case WEAPON_POLEARM:
				dam += ( 21 + skill / 4 + get_skill( ch, gsn_charge ) ) * dam / 100;
				break;
			case WEAPON_SPEAR:
				dam += ( 25 + skill / 4 + get_skill( ch, gsn_charge ) ) * dam / 100;
				break;
			case WEAPON_MACE:
				dam += ( 15 + skill / 4 + get_skill( ch, gsn_charge ) ) * dam / 100;
				break;
			case WEAPON_STAFF:
				dam += ( 10 + skill / 4 + get_skill( ch, gsn_charge ) ) * dam / 100;
				break;
			case WEAPON_FLAIL:
				dam += ( 22 + skill / 4 + get_skill( ch, gsn_charge ) ) * dam / 100;
				break;
			default:
				dam += ( 5 + skill / 4 + get_skill( ch, gsn_charge ) ) * dam / 100;
				break;
		}

#ifdef INFO
		print_char( ch, "DAM +CHARGE: %d\n\r", dam );
#endif

	}
	else if ( ( dt == gsn_smite && wield ) || ( dt == gsn_smite_good && wield ) )
		{
	    int level_bonus = 0;
        //send_to_char( "Jestesmy w fight.c. \n\r", ch );
        dam += ( ch->level + UMAX(0,(get_curr_stat_deprecated( ch, STAT_CHA )-15)*4 ));

        //holy_prayer
        if ( ( paf = affect_find( ch->affected, gsn_prayer_last )) != NULL ) //jesli ma wlaczonego skilla
        {
            if ( paf->modifier < 30 ) //jesli sie ostatnio modlil
            {
                 if ( ( paf = affect_find( ch->affected, gsn_holy_pool )) != NULL ) //jesli znaleziono pule
                 {
                    if((paf->modifier > 50) && (number_percent() < (paf->modifier)/3) ) // sacred
                    {
                    //send_to_char( "{5Wzmocnienie z tytu�u holy prayer.{x\n\r", ch );
                     if ( IS_WEAPON_STAT( wield, WEAPON_FLAMING) || IS_WEAPON_STAT( wield, WEAPON_FLAMING_BURST) || IS_WEAPON_STAT( wield, WEAPON_FROST) ||
                     		  IS_WEAPON_STAT( wield, WEAPON_SHOCKING) || IS_WEAPON_STAT( wield, WEAPON_SACRED ) || EXT_IS_SET(wield->extra_flags, ITEM_NOMAGIC ) ||
                     		  IS_WEAPON_STAT( wield, WEAPON_TOXIC ) || IS_WEAPON_STAT( wield, WEAPON_FROST_BURST) || IS_WEAPON_STAT( wield, WEAPON_SHOCKING_BURST) ||
                     		  IS_WEAPON_STAT( wield, WEAPON_HOLY) || IS_WEAPON_STAT( wield, WEAPON_TOXIC_BURST) || IS_WEAPON_STAT( wield, WEAPON_NEGATIVE) ||
                     		  IS_WEAPON_STAT( wield, WEAPON_UNHOLY))
					  {
					  }
	                  else
                      {
                        //send_to_char( "{5Bron nie ma flagi, mozna dac sacred.{x\n\r", ch );

                            holy_mod1 = paf->modifier/4;
                            holy_mod2 = paf->modifier/4;
                            af.where = TO_WEAPON;
                         	af.type = sn;
                          	af.level = holy_mod1;
                           	af.duration = holy_mod2;
                      //    af.rt_duration = 10;
                            af.rt_duration = 0;
                          	af.location = WEAPON_SACRED;
                          	af.modifier = 0;
                          	af.bitvector = &AFF_NONE;
                         	affect_to_obj( wield, &af );
                            send_to_char( "Twoje rami� otacza jasna po�wiata, kt�ra nast�pnie wnika w tw� bro�!\n\r", ch );
                            act( "Bro� $z wydaje si� przez chwil� rozb�yska� niezwyk�ym �wiat�em!", ch, NULL, NULL, TO_ROOM );
                           paf->modifier -= 7;

                      }

                      if (paf->modifier > 30 && number_percent() < paf->modifier/3)
                      {

                        level_bonus = 3;
                        dam += dam/3;
                        send_to_char( "Czujesz jak twe cia�o rozpiera pot�ga �wiat�a, wk�adasz niesamowit� si�� w cios!\n\r", ch );
                        act( "$n wydaje si� przez chwil� rozb�yska� niezwyk�ym �wiat�em!", ch, NULL, NULL, TO_ROOM );
                        //send_to_char( "{5Bron ma juz jakas flage.{x\n\r", ch );
                        paf->modifier -= 7;

                      }
                      if (paf->modifier < 0 && number_percent() < (-paf->modifier))
                      {

                        level_bonus = -2;
                        dam -= dam/3;


                      }

                       if (paf->modifier < 0 && number_percent() < (-paf->modifier))
                      {

                        level_bonus = -2;
                        dam -= dam/3;

                      }
                      if(level_bonus < 0)
                      {
                      send_to_char( "Twe nieczyste sumienie przeszkadza ci w zebraniu si�!\n\r", ch );
                      act( "$n wydaje si� wacha� przy zadaniu ciosu.", ch, NULL, NULL, TO_ROOM );
                      }
                    }
                 }//koniec sprawdzania puli
            }//koniec sprawdzania modlitwy
        }//koniec wlaczonego skilla
        //koniec holy prayera

		//smite moze duzo slabszych dobic jednym strzalem
		//send_to_char( "Sprawdzamy czy mozemy zabic jednym strzalem. \n\r", ch );
        if ( ( victim->level + 10 < ch->level + level_bonus ) && ((victim->hit - dam)*100)/victim->max_hit < UMAX( get_skill(ch,gsn_smite),get_skill(ch,gsn_smite_good) )*( IS_WEAPON_STAT(wield,WEAPON_TWO_HANDS) ? 3 : 2 )/4 )
        {
            dam += victim->hit - dam + 12;
        }


#ifdef INFO
		print_char( ch, "DAM +SMITE: %d\n\r", dam );
#endif
	}

	DEBUG_INFO( "onehit:bashed_mod" );
	/* jesli koles nie stoi */
	if ( victim->position < POS_FIGHTING )
	{
		/* nieprzytomy/spiacy obrazenia razy 2, reszta +25%*/
		if ( !IS_AWAKE( victim ) )
			dam *= 2;
		else
			dam += dam / 4;
#ifdef INFO
		print_char( ch, "DAM + POSITION_BONUS: %d\n\r", dam );
#endif

	}

	DEBUG_INFO( "onehit:palladin_align_mod" );
    if ( ( paf = affect_find( ch->affected, gsn_holy_pool )) != NULL )
    {
             holy_mod1 = 0;
             holy_mod2 = 0;
             if(IS_EVIL(victim))
             {
                 if(paf->modifier > 50 && number_percent() < paf->modifier/10)
                 {
                     holy_mod1 += dam/3;
                     if(number_percent() < 40)
                     {
                         paf->modifier -= 1;
                     }
                 }

                 if(paf->modifier > 0 && number_percent() < paf->modifier/5)
                 {
                     holy_mod1 += dam/3;
                     if(number_percent() < 30)
                     {
                         paf->modifier -= 1;
                     }

                 }
             }
             /*
              * rasy zwierzece wykluczamy
              */
             else if ( !IS_SET( race_table[ GET_RACE(victim) ].type, ANIMAL ) )
             {
                 if(paf->modifier < 0 && number_percent() < (-paf->modifier/5))
                 {
                     holy_mod1 -= dam/3;

                 }

                 if(paf->modifier < -50 && number_percent() < (-paf->modifier/5))
                 {
                     holy_mod1 -= dam/3;

                 }
             }

             if(holy_mod1 > 0)
             {
                          switch ( number_range( 1, 9 ) )
					{
						case 1:
							act( "Wznosisz r�ke do ciosu i czujesz, �e to jest TO, to jest Twoje przeznaczenie! Uderzasz z potworn� si�a.", ch, NULL, victim, TO_CHAR );
							break;
						case 2:
							act( "Czujesz jak twe ramie wype�nia Boska moc, uderzasz $C z ca�ych si�.", ch, NULL, victim, TO_CHAR );
							break;
						case 3:
							act( "Czujesz jak z�o niemal kapie z $Z, pewno�� i� jest on z�y wype�nia ci� Gniewem Sprawiedliwych.", ch, NULL, victim, TO_CHAR );
							break;
						case 4:
							act( "Masz wizj� p�on�cej wioski w kt�rej to niewinnych morduj� takie potwory jak $N! To napawa ci� nadludzk� si�a z kt�ra atakujesz $C.", ch, NULL, victim, TO_CHAR );
							break;
						case 5:
							act( "Walcz�c z $V czujesz jak wiele zale�y od twojego sukcesu, nie zawiedziesz niewinnych, nie porzucisz �lub�w, nie poddasz tej walki!", ch, NULL, victim, TO_CHAR );
							break;
						case 6:
							act( "Wype�nia ci� niesamowita si�a, walka ze s�ugami Ciemno�ci to twoja specjalno��!", ch, NULL, victim, TO_CHAR );
							break;
						case 7:
							act( "Wiesz, �e tysi�ce ludzi liczy na to, i� ty i tobie podobni zwyci�ycie. Z pewno�ci� ich nie zawiedziesz.", ch, NULL, victim, TO_CHAR );
							break;
						case 8:
							act( "Wiedz�c, �e $N jest z�y, atakujesz z ca�ych si�.", ch, NULL, victim, TO_CHAR );
							break;
						case 9:
							act( "Czujesz jak Bogwie �wiat�o�ci wype�niaj� ci� wielk� si��.", ch, NULL, victim, TO_CHAR );
							break;
					}
					switch ( number_range( 1, 6 ) )
					{
						case 1:
							act( "Widzisz jak $n z b�yskiem uniesienia w oczach wyprowadza pot�ny cios.", ch, NULL, victim, TO_NOTVICT );
							break;
						case 2:
							act( "Nie wiesz co wst�pi�o w $c, ten cios by� naprawd� pot�ny.", ch, NULL, victim, TO_NOTVICT );
							break;
						case 3:
							act( "Wydaje ci si�, �e dla $z ca�y �wiat przesta� istnie�, walczy jakby istnia� tylko $N.", ch, NULL, victim, TO_NOTVICT );
							break;
						case 4:
							act( "Ciosy $z zostawiaj� za sob� �wiec�c� smuge a ich si�a jest niewyobra�alna.", ch, NULL, victim, TO_NOTVICT );
							break;
						case 5:
							act( "Niemal wsp�czujesz $X, gdy $n z natchnionym wyrazem twarzy rzuca sie na $C i atakuje go z potworn� si��.", ch, NULL, victim, TO_NOTVICT );
							break;
						case 6:
							act( "$n atakuje tak jakby to by�a walka �ycia, ciosy s� pewne i bardzo silne.", ch, NULL, victim, TO_NOTVICT );
							break;
					}


             }

             if(holy_mod1 < 0)
             {
               	switch ( number_range( 1, 11 ) )
				{
					case 1:
						act( "Czujesz jak przez umys� przebiegaj� ci obrazy z �ycia legendarnych Paladyn�w, co sob� reprezentowali, co chcieli zrobi�, ONI nie atakowaliby $Z!", ch, NULL, victim, TO_CHAR );
						break;
					case 2:
						act( "Gdy wznosisz r�k� do ciosu czujesz si� podle, przecie� tylko niegodny cn�t Paladyna zbir wznosi r�ke na niewinnych!", ch, NULL, victim, TO_CHAR );
						break;
					case 3:
						act( "Tracisz ochot� do walki. Wiesz, �e $N nie jest z�y, czujesz to ca�ym swoim cia�em, co ty wog�le wyprawiasz ?!?", ch, NULL, victim, TO_CHAR );
						break;
					case 4:
						act( "Przed oczami staje ci obraz kobiety w b�yszcz�cej zbroi z dwoma �wietlistymi m�otami w r�kach. Spogl�da nagle na ciebie, jej oczy zachodz� �zami i opuszcza g�ow�. Czujesz si� naprawde podle.", ch, NULL, victim, TO_CHAR );
						break;
					case 5:
						act( "Twe d�onie dr�� gdy atakujesz $C z trudem utrzymujesz bro� w r�ku.", ch, NULL, victim, TO_CHAR );
						break;
					case 6:
						act( "Targaj� tob� wyrzuty sumienia, jak to si� sta�o, �e walczysz z $V ? Z pewno�ci� mo�na by�o tego unikn�c!", ch, NULL, victim, TO_CHAR );
						break;
					case 7:
						act( "S�yszysz tysi�ce szept�w, nasilaj� si� w twojej g�owie, ka�dy z nich krzyczy - Morderca!", ch, NULL, victim, TO_CHAR );
						break;
					case 8:
						act( "Anielski spiew dobiega z oddali, czujesz jak ogarnia ci� fala dobra i mocy. Wznosisz r�k� na $C i nagle spiewy cichn� a moc odp�ywa od ciebie... czujesz si� bardzo �le.", ch, NULL, victim, TO_CHAR );
						break;
					case 9:
						act( "Czujesz �e walka z $V nie jest dobrym pomys�em, przecie� nie do tego ca�e �ycie si� przygotowywa�e�, czy tak post�puj� str�e Dobra i Porz�dku ?!?", ch, NULL, victim, TO_CHAR );
						break;
					case 10:
						act( "Czujesz si� bardzo �le walcz�c z $V.", ch, NULL, victim, TO_CHAR );
						break;
					case 11:
						act( "Ca�a ta walka nie ma sensu, na Bog�w co ty wyprawiasz?", ch, NULL, victim, TO_CHAR );
						break;
				}
				switch ( number_range( 1, 4 ) )
				{
					case 1:
						act( "Widzisz jak $n wacha si� zadaj�c cios, wyra�nie nie mo�e si� skoncentrowa�.", ch, NULL, victim, TO_NOTVICT );
						break;
					case 2:
						act( "Widzisz jak d�onie $z dr�� gdy walczy z $V.", ch, NULL, victim, TO_NOTVICT );
						break;
					case 3:
						act( "$n rozgl�da si� dooko�a jakby poszukuj�c wyj�cia z tej niewygodnej dla siebie walki.", ch, NULL, victim, TO_NOTVICT );
						break;
					case 4:
						act( "Wida�, �e $x ta walka nie odpowiada, najwyra�niej wstrzymuje sie od ciosu. ", ch, NULL, victim, TO_NOTVICT );
						break;
				}


             }
             dam += holy_mod1;
        }//koniec wplywu holy prayer


#ifdef INFO
	print_char( ch, "DAM + PAL_ALIGN_BONUS: %d\n\r", dam );
#endif

	//rozplatanie - spada sila w kolejnych przeciwnikow
	if( dt == gsn_cleave && !IS_SET( ch->fight_data, FIGHT_THOUSANDSLAYER ))
	{
		dam -= dam/4;
	}

	/* sparowany atak??*/
	if ( dt != gsn_backstab && dt != gsn_charge && !dreadful )
	{
		DEBUG_INFO( "onehit:parry" );
		if ( check_parry( ch, victim, dam, wield ) )
			return;

		DEBUG_INFO( "onehit:dodge" );
		/* po dodge, jest szansa ze cios pojdzie w kogos obok */
		if ( check_dodge( ch, victim, victim_ac , diceroll, thac0, critical ) )
		{
			CHAR_DATA * vch = NULL;

			if ( number_percent() < 15 )
			{
				for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
					if ( vch != ch && vch->fighting == victim )
						break;

				if ( vch )
				{
					accident = TRUE;
					one_hit( ch, vch, dt, second );
					accident = FALSE;
					return;
				}
			}

			return;
		}

		if ( check_shield_block( ch, victim, victim_ac , diceroll, thac0, critical, &dam  ) )
			return;
	}

	/* jesli trafilismy to obrazenia minimum 1*/
	if ( dam < 1 )
    {
		dam = 1;
    }
    /**
     * aura of battle lust
     */
    if ( has_aura( ch, FALSE ) )
    {
        if ( get_aura_sn( ch ) == 465 )
        {
            dam += get_aura_modifier( ch );
#ifdef INFO
            print_char( ch, "DAM + aura of battle lust: %d\n\r", dam );
#endif
        }
    }

    /**
     * ogolnie koniec, teraz obsluga roznych rzeczy,
     * na poczatek zmniejszamy obrazenia wojownika
     */
   /* if ( !IS_NPC( ch ) && ch->class == CLASS_WARRIOR && dam > 3 )
    {
        dam *= 3;
        dam /= 4;
    }
   */

	if ( dam > 70 && !IS_NPC( ch ) && !IS_IMMORTAL( ch ) && dt == TYPE_UNDEFINED )
	{
		bugf( "Gracz: %s co� podejrzanie mocno bije z %s [%d]!", ch->name,
				( wield ? wield->short_descr : "reki" ),
				( wield ? wield->pIndexData->vnum : 0 ) );
	}

	DEBUG_INFO( "onehit:blink" );
	if ( check_blink( victim, ch, TRUE ) && dt != gsn_bash && !dreadful && dt != gsn_slam )
	{
		act( "Kiedy tw�j cios ju� ma trafi� w $C, $S sylwetka chwilowo znika.", ch, NULL, victim, TO_CHAR );
		act( "Kiedy cios $z ma ju� w ciebie trafi� znikasz na chwilk� z tego planu egzystencji.", ch, NULL, victim, TO_VICT );
		act( "Kiedy cios $z ma ju� trafi� w $C, $S sylwetka chwilowo znika.", ch, NULL, victim, TO_NOTVICT );
		return;
	}

	DEBUG_INFO( "onehit:mirror_image" );
	if ( !dreadful && dt != gsn_crush && dt != gsn_bash && check_mirror_image( victim, ch ) && dt != gsn_slam )
	{
		print_char( ch, "Tw�j cios trafia, %s znika.\n\r", PERS( victim, ch ) );
		print_char( victim, "%s trafia jedno z twoich lustrzanych odbi�.\n\r", PERS( ch, victim ) );
		act( "$n trafia jedno z lustrzanych odbi� $Z.", ch, NULL, victim, TO_NOTVICT );
		return;
	}

	DEBUG_INFO( "onehit:soul" );
	if ( IS_AFFECTED( victim, AFF_SOUL ) )
	{
		print_char( ch, "Tw�j cios trafia, uwolniona dusza ulatuje do swojego wymiaru.\n\r", PERS( victim, ch ) );
		print_char( victim, "%s trafia ci�, ulatujesz do swojego wymiaru.\n\r", PERS( ch, victim ) );
		act( "$n trafia, dusza $Z ulatuje do swojego wymiaru.", ch, NULL, victim, TO_NOTVICT );

		/* itemy co je trzymala dusza opadaja na ziemie*/
		for ( obj = victim->carrying; obj != NULL; obj = obj_next )
		{
			OBJ_NEXT_CONTENT( obj, obj_next );

			if ( IS_OBJ_STAT( obj, ITEM_NODROP ) )
				continue;
			if ( obj->wear_loc != WEAR_NONE )
				unequip_char( victim, obj );

			obj_from_char( obj );
			act( "$p upada na ziemi�.", victim, obj, NULL, TO_ROOM );
			obj_to_room( obj, victim->in_room );

			if ( IS_OBJ_STAT( obj, ITEM_MELT_DROP ) )
			{
				switch ( number_range( 1, 3 ) )
				{
					case 1:
						act( "$p rozp�ywa si� w powietrzu.", ch, obj, NULL, TO_ROOM );
						break;
					case 2:
						act( "$p znika bez �ladu.", ch, obj, NULL, TO_ROOM );
						break;
					default:
						act( "$p rozp�ywa si� pozostawiaj�c tylko chmur� dymu.", ch, obj, NULL, TO_ROOM );
						break;
				}
			}
			else if ( obj->item_type == ITEM_POTION && ( obj->material == 11 || obj->material == 15 ) )
			{
				switch ( number_range( 1, 3 ) )
				{
					case 1:
						act( "$p toczy si� przez chwil�, po czym rozpryskuj� si� na kwa�eczki.", ch, obj, NULL, TO_ROOM );
						break;
					case 2:
						act( "$p p�ka z hukiem i rozpryskuje si� dooko�a.", ch, obj, NULL, TO_ROOM );
						break;
					default:
						act( "$p rozpryskuje si� na kawa�eczki.", ch, obj, NULL, TO_ROOM );
						break;
				}
			}
		}
		extract_char( victim, TRUE );
		return;
	}

	DEBUG_INFO( "onehit:steel_scarfskin" );
		if( is_affected( victim, gsn_steel_scarfskin ) && !dreadful &&
				dam <= number_range( 6, UMAX(14,affect_find(victim->affected,gsn_steel_scarfskin)->level ) ) &&
				( !wield || ( wield && !IS_WEAPON_STAT(wield, WEAPON_SHARP) && !IS_WEAPON_STAT(wield, WEAPON_VORPAL) ) ) )
		{
			scarfskin = affect_find( victim->affected, gsn_steel_scarfskin );

			if ( scarfskin->modifier > 0 )
			{
				scarfskin->modifier -= dam;
				dam = 0;

				if ( wield )
				{
					print_char( ch, "Twoja bro� odbija si� jakby� uderzy�%s w stal.\n\r", ch->sex == 2 ? "a" : ch->sex == 0 ? "o" : "" );
					print_char( victim, "%s trafia cie, jednak %s bro� odskakuje jakby uderzy�%s w stal.\n\r", PERS( ch, victim ), ch->sex == 2 ? "jej" : ch->sex == 0 ? "tego" : "jego", ch->sex == 2 ? "a" : ch->sex == 0 ? "o" : "" );
					switch ( victim->sex )
					{
						case 0:
							act( "$n trafia $C, jednak $s bro� odskakuje jakby $N by�o ze stali.", ch, NULL, victim, TO_NOTVICT );
							break;
						case 1:
							act( "$n trafia $C, jednak $s bro� odskakuje jakby $N by� ze stali.", ch, NULL, victim, TO_NOTVICT );
							break;
						default :
							act( "$n trafia $C, jednak $s bro� odskakuje jakby $N by�a ze stali.", ch, NULL, victim, TO_NOTVICT );
							break;
					}
				}
				else
				{
					print_char( ch, "Tw�j cios jest nieskuteczny, jakby� uderzy�%s w stal.\n\r", ch->sex == 2 ? "a" : ch->sex == 0 ? "o" : "" );
					print_char( victim, "%s trafia cie, jednak %s cios jest nieskuteczny.\n\r", PERS( ch, victim ), ch->sex == 2 ? "jej" : "jego", ch->sex == 2 ? "uderzy�a" : "uderzy�" );
					act( "$n trafia $C, jednak $s cios jest nieskuteczny.", ch, NULL, victim, TO_NOTVICT );
				}
				return;
			}
		}

	DEBUG_INFO( "onehit:stone_skin" );
	if ( check_stone_skin( ch, victim, dam_type, &dam, wield ) && !dreadful )
	{
		if ( dam == 0 )
		{
			if ( wield )
			{
				print_char( ch, "Twoja bro� odbija si� jakby� uderzy�%s w kamie�.\n\r", ch->sex == 2 ? "a" : ch->sex == 0 ? "o" : "" );
				print_char( victim, "%s trafia cie, jednak %s bro� odskakuje jakby uderzy�%s w kamie�.\n\r", PERS( ch, victim ), ch->sex == 2 ? "jej" : ch->sex == 0 ? "tego" : "jego", ch->sex == 2 ? "a" : ch->sex == 0 ? "o" : "" );
				switch ( victim->sex )
				{
					case 0:
						act( "$n trafia $C, jednak $s bro� odskakuje jakby $N by�o z kamienia.", ch, NULL, victim, TO_NOTVICT );
						break;
					case 1:
						act( "$n trafia $C, jednak $s bro� odskakuje jakby $N by� z kamienia.", ch, NULL, victim, TO_NOTVICT );
						break;
					default :
						act( "$n trafia $C, jednak $s bro� odskakuje jakby $N by�a z kamienia.", ch, NULL, victim, TO_NOTVICT );
						break;
				}
			}
			else
			{
				print_char( ch, "Tw�j cios jest nieskuteczny, jakby� uderzy�%s w kamie�.\n\r", ch->sex == 2 ? "a" : ch->sex == 0 ? "o" : "" );
				print_char( victim, "%s trafia cie, jednak %s cios jest nieskuteczny.\n\r", PERS( ch, victim ), ch->sex == 2 ? "jej" : "jego", ch->sex == 2 ? "uderzy�a" : "uderzy�" );
				act( "$n trafia $C, jednak $s cios jest nieskuteczny.", ch, NULL, victim, TO_NOTVICT );
			}
			return;
		}
		else
		{
			if ( wield )
			{
				print_char( ch, "Twoja bro� przebija si� przez kamienn� skor� pokrywaj�c� %s.\n\r", PERS2( victim, ch ) );
				print_char( victim, "Bro� %s przebija si� przez twoj� kamienn� sk�r�.\n\r", PERS2( ch, victim ) );
				act( "Bro� $z trafia $C przebijaj�c si� przez kammien� sk�r�.", ch, NULL, victim, TO_NOTVICT );
			}
			else
			{
				print_char( ch, "Tw�j cios przebija si� przez kamienn� sk�r� pokrywaj�c� %s.\n\r", PERS4( victim, ch ) );
				print_char( victim, "Cios %s przebija si� przez twoj� kamienn� sk�r�.\n\r", PERS2( ch, victim ) );
				act( "Cios $z trafia $C przebijaj�c si� przez kamienn� sk�r�.", ch, NULL, victim, TO_NOTVICT );
			}
		}
	}

	DEBUG_INFO( "onehit:magic_weapon" );
	if ( victim->resists[ RESIST_MAGIC_WEAPON ] > 0 &&
			check_magic_attack( ch, victim, wield ) < victim->resists[ RESIST_MAGIC_WEAPON ] )
	{
		if( check_magic_attack( ch, victim, wield ) == 0 && IS_AFFECTED(victim, AFF_RESIST_MAGIC_WEAPON))
		{
			;
		}
		else
		{
			if( !dreadful )//jak bk udaza z trika dreadful strike to atak jest umagiczniony negativem (zuzywa tormenta)
			{
                if((check_magic_attack( ch, victim, wield ) + 1) == victim->resists[ RESIST_MAGIC_WEAPON ] && check_magic_attack( ch, victim, wield ) > 0)
                {
                  dam /= 3; //Raszer - jesli mamy bron o slabszym o 1 umagicznieniu niz potrzebowalismy na bossa, to zadajemy polowe normalnych obrazen
                }else
                {
				immune = TRUE;
				dam = 0;
                }
			}
		}
	}

	if ( immune || dam == 0 )
	{
		print_char( ch, "Tw�j atak jest nieskuteczny, cios nic %s nie robi.\n\r", PERS3( victim, ch ) );
		print_char( victim, "Atak %s jest nieskuteczny, %s cios niczego ci nie robi.\n\r", PERS2( ch, victim ), ( ch->sex == 2 ) ? "jej" : "jego" );
		act( "$n trafia, jednak $s cios jest nieskuteczny.", ch, NULL, victim, TO_NOTVICT );
		return;
	}

	DEBUG_INFO( "onehit:check_resist" );
	dam = check_resist( victim, dam_type, dam );

#ifdef INFO
	print_char( ch, "DAM -RESISTS: %d\n\r", dam );
#endif

	if ( immune || dam == 0 )
	{
		print_char( ch, "Tw�j atak jest nieskuteczny, cios nic %s nie robi.\n\r", PERS3( victim, ch ) );
		print_char( victim, "Atak %s jest nieskuteczny, %s cios niczego ci nie robi.\n\r", PERS2( ch, victim ), ch->sex == 2 ? "jej" : "jego" );
		act( "Atak $z nie robi wrazenia na $B, $s cios jest nieskuteczny.", ch, NULL, victim, TO_NOTVICT );
	


			

		if ( wield )
		{
			if ( ch->fighting == victim && number_percent() < 5 && IS_WEAPON_STAT( wield, WEAPON_FLAMING_BURST ) )
			{
				dam = UMIN(wield->value[6], wield->value[5]) * number_range( 8, 15 ) + number_range(30, 45);
				dam = UMAX(dam, number_range(30, 45)); //Dla broni z ujemnymi v5 i v6
				act( "Fala ognia wylatuje z $j i trafia w $N sprawiaj�c, ze staje przez chwil� w p�omieniach.", ch, wield, victim, TO_CHAR );
				act( "$N staje na kr�tk� chwil� w p�omieniach kiedy dostaje ognista fal� z $j $z.", ch, wield, victim, TO_NOTVICT );
				act( "$p uderza w ciebie fal� p�omieni.", ch, wield, victim, TO_VICT );
				fire_effect( victim, 1, dam, TARGET_CHAR );
				damage( ch, victim, dam, 0, DAM_FIRE, FALSE );
			}
			else if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_FLAMING || IS_WEAPON_STAT( wield, WEAPON_FLAMING_BURST ) ) ) 
			{
				dam = number_range( base_dam / 4, base_dam / 2 );
				dam = UMAX( dam, 1 );
				act( "$N parzy si� przy zetkni�ciu z twoja $j.", ch, wield, victim, TO_CHAR );
				act( "$N parzy si� przy zetkni�ciu z $j $z.", ch, wield, victim, TO_NOTVICT );
				act( "$p lekko cie przysma�a.", ch, wield, victim, TO_VICT );
				fire_effect( ( void * ) victim, 1, dam, TARGET_CHAR );
				damage( ch, victim, dam, 0, DAM_FIRE, FALSE );
			}
			if ( ch->fighting == victim && number_percent() < 5 && IS_WEAPON_STAT( wield, WEAPON_FROST_BURST ) )
			{
				dam = UMIN(wield->value[6], wield->value[5]) * number_range( 8, 15 ) + number_range(30, 45);
				dam = UMAX(dam, number_range(30, 45)); //Dla broni z ujemnymi v5 i v6
				act( "$p rozb�yskuje zimnem i mro��ny podmuch uderza w $c.", victim, wield, NULL, TO_ROOM );
				act( "Mro�ny podmuch wiatru z $f mrozi ci� do ko�ci.", ch, wield, victim, TO_VICT );
				cold_effect( victim, 1, dam, TARGET_CHAR );
				damage( ch, victim, dam, 0, DAM_COLD, FALSE );
			}
			else if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_FROST ) || IS_WEAPON_STAT( wield, WEAPON_FROST_BURST ) )
			{
				dam = number_range( base_dam / 4, base_dam / 2 );
				dam = UMAX( dam, 1 );
				act( "$p zamra�a $c.", victim, wield, NULL, TO_ROOM );
				act( "Zimny dotyk $f zamienia ci� w bry�� lodu.", ch, wield, victim, TO_VICT );
				cold_effect( victim, 1, dam, TARGET_CHAR );
				damage( ch, victim, dam, 0, DAM_COLD, FALSE );
			}
			if ( ch->fighting == victim && number_percent() < 5 && IS_WEAPON_STAT( wield, WEAPON_SHOCKING_BURST ) )
			{
				dam = UMIN(wield->value[6], wield->value[5]) * number_range( 8, 15 ) + number_range(30, 45);
				dam = UMAX(dam, number_range(30, 45)); //Dla broni z ujemnymi v5 i v6
				act( "W nag�bym rozb�ysku ��wiat�a $f pora�a $c moc� piorun�w.", victim, wield, NULL, TO_ROOM );
				act( "$p pora�a ci� olbrzymi� moc� piorun�w.", victim, wield, NULL, TO_CHAR );
				shock_effect( victim, 1, dam, TARGET_CHAR );
				damage( ch, victim, dam, 0, DAM_LIGHTNING, FALSE );
			}
			else if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_SHOCKING ) || IS_WEAPON_STAT( wield, WEAPON_SHOCKING_BURST ) )
			{
				dam = number_range( base_dam / 4, base_dam / 2 );
				dam = UMAX( dam, 1 );
				act( "B�yskawica z $f trafia $c wywo�uj�c u niego niekontrolowane drgawki.", victim, wield, NULL, TO_ROOM );
				act( "$p aplikuje ci seri� elektrowstrzas�w.", victim, wield, NULL, TO_CHAR );
				shock_effect( victim, 1, dam, TARGET_CHAR );
				damage( ch, victim, dam, 0, DAM_LIGHTNING, FALSE );
			}
			if ( ch->fighting == victim && number_percent() < 5 && IS_WEAPON_STAT( wield, WEAPON_TOXIC_BURST ) )
			{
				dam = UMIN(wield->value[6], wield->value[5]) * number_range( 8, 15 ) + number_range(30, 45);
				dam = UMAX(dam, number_range(30, 45)); //Dla broni z ujemnymi v5 i v6
				act( "Pulsuj�c moc� $f trafia kwasowym pociskiem w $c.", victim, wield, NULL, TO_ROOM );
				act( "Kwasowy pocisk z $f trafia ci� i pali!", victim, wield, NULL, TO_CHAR );
				acid_effect( victim, 1, dam, TARGET_CHAR );
				damage( ch, victim, dam, 0, DAM_ACID, FALSE );
			}
			else if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_TOXIC ) || IS_WEAPON_STAT( wield, WEAPON_TOXIC_BURST ) )
			{
				dam = number_range( base_dam / 4, base_dam / 2 );
				dam = UMAX( dam, 1 );
				act( "Pokrywaj�cy $h kwas wy�era sk�r� $z.", victim, wield, NULL, TO_ROOM );
				act( "Pokrywaj�cy $h kwas wy�era tw� sk�r�!", victim, wield, NULL, TO_CHAR );
				acid_effect( victim, 1, dam, TARGET_CHAR );
				damage( ch, victim, dam, 0, DAM_ACID, FALSE );
			}
			if ( ch->fighting == victim && number_percent() < 5 && IS_WEAPON_STAT( wield, WEAPON_SONIC_BURST ) )
			{
				dam = UMIN(wield->value[6], wield->value[5]) * number_range( 8, 15 ) + number_range(30, 45);
				dam = UMAX(dam, number_range(30, 45)); //Dla broni z ujemnymi v5 i v6
			    act( "W nag�ym rozb�ysku moc� $f pora�a $c fal� dzwi�kow�.", victim, wield, NULL, TO_ROOM );
			    act( "Pulsuj�c moc� $f pora�a ci� prawdziw� kakofoni� fal dzwi�kowych.", victim, wield, NULL, TO_CHAR );
				sound_effect( victim, 1, dam, TARGET_CHAR );
				damage( ch, victim, dam, 0, DAM_SOUND, FALSE );
			}
			else if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_RESONANT ) || IS_WEAPON_STAT( wield, WEAPON_SONIC_BURST ) )
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

			if ( ch->fighting == victim && number_percent() < 5 && IS_WEAPON_STAT( wield, WEAPON_HOLY ) )
			{
				dam = UMIN(wield->value[6], wield->value[5]) * number_range( 4, 12 ) + number_range(40, 60);
				act( "Roz�byskuj�c pot�n� ��wi�t� moc� $f trafia w $c.", victim, wield, NULL, TO_ROOM );
				act( "Rozb�yskuj�c pot�n� ��wi�t� moc� $f trafia ci� prosto w pier�.", victim, wield, NULL, TO_CHAR );
				dam = UMAX(dam, number_range(20, 35)); //Dla broni z ujemnymi v5 i v6
				damage( ch, victim, dam, 0, DAM_HOLY, FALSE );
				if( IS_NPC(victim) && victim->pIndexData->vnum == MOB_VNUM_AVATAR && ch->fighting == victim)
				{
					act( "{5W kr�tkim rozb�ysku �wiat�a $n zamienia si� w kupk� dymi�cego popio�u.{x", victim, NULL, NULL, TO_ROOM );
					raw_damage( ch, victim, 20 + get_max_hp(victim) );
				}
			}
			else if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_SACRED ) || IS_WEAPON_STAT( wield, WEAPON_HOLY ) )
			{
				dam = number_range( base_dam / 4, base_dam / 2 );
				dam = UMAX( dam, 1 );
				act( "Niewielki strumie� �wi�tej mocy z $f trafia w $c.", victim, wield, NULL, TO_ROOM );
				act( "Niewielki strumie� �wi�tej mocy z $f trafia ci� prosto w pier�.", victim, wield, NULL, TO_CHAR );
				damage( ch, victim, dam, 0, DAM_HOLY, FALSE );
				if( IS_NPC(victim) && victim->pIndexData->vnum == MOB_VNUM_AVATAR )
				{
					act( "{5W kr�tkim rozb�ysku �wiat�a $n zamienia si� w kupk� dymi�cego popio�u.{x", victim, NULL, NULL, TO_ROOM );
					raw_damage( ch, victim, 20 + get_max_hp(victim) );
				}
			}

			if ( ch->fighting == victim && number_percent() < 5 && IS_WEAPON_STAT( wield, WEAPON_UNHOLY ) )
			{
				dam = UMIN(wield->value[6], wield->value[5]) * number_range( 4, 12 ) + number_range(35, 45);
				act( "Pot�na z�a moc wylatuje z $f i trafia w $c.", victim, wield, NULL, TO_ROOM );
				act( "Pot�na z�a moc promienuj�ca z $f otacza i pr�buje ci� zniszczy�.", victim, wield, NULL, TO_CHAR );
				dam = UMAX(dam, number_range(20, 35)); //Dla broni z ujemnymi v5 i v6				
				damage( ch, victim, dam, 0, DAM_NEGATIVE, FALSE );
			}			
			else if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_NEGATIVE ) || IS_WEAPON_STAT( wield, WEAPON_UNHOLY ) )
			{
				dam = number_range( base_dam / 4, base_dam / 2 );
				dam = UMAX( dam, 1 );
				act( "Ciemnoczerwona aura z $f otacza przez chwil� $c.", victim, wield, NULL, TO_ROOM );
				act( "Ciemnoczerwona aura z $f otacza i rani ci�.", victim, wield, NULL, TO_CHAR );
				damage( ch, victim, dam, 0, DAM_NEGATIVE, FALSE );
			}

			if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_DISPEL ) )
			{
				weapon_dispeller(ch, victim, base_dam);
			}
			return;
		}
		else if ( IS_NPC( ch ) && ch->attack_flags != 0 )
			check_special_unarmed_hit( ch, victim, unarmed_base_dam );

		return;
	}

	DEBUG_INFO( "onehit:increase_wounds" );
	/* spell zwiekszajacy obrazenia slash/pierce/bash */
	check_increase_wounds( victim, dam_type, &dam );

#ifdef INFO
	print_char( ch, "DAM +INCREASE_WOUNDS: %d\n\r", dam );
#endif

	DEBUG_INFO( "onehit:troll_power" );
	if ( IS_AFFECTED( victim, AFF_TROLL ) && victim->hit > 0 && victim->hit - dam < 0 )
	{
		dam = victim->hit + 1;
		create_event( EVENT_TROLL_POWER_REGEN, number_range( 3, 5 ) * PULSE_VIOLENCE, victim, NULL, 0 );
	}

	DEBUG_INFO( "onehit:damage_reduction" );

	if ( is_affected(victim,gsn_demon_aura))
	{
        int mod = 20;
		if( IS_GOOD(ch))
        {
			dam -= get_skill(victim,gsn_demon_aura) / mod;
        }
		else if ( !IS_EVIL(ch))
        {
			dam -= get_skill(victim,gsn_demon_aura)/30;
        }
	}

	if ( is_affected(victim,gsn_defense_curl))
	{
		defense_curl = affect_find(victim->affected,gsn_defense_curl );
		defense_curl->modifier -= UMIN( defense_curl->level, dam );
		dam -= UMIN( defense_curl->level, dam );
		if( defense_curl->modifier <= 0 )
		{
			affect_remove( victim, defense_curl );
			act( "Otaczaj�ca ci� ochronna spirala zwija si�, a po chwili zanika.", victim,NULL,NULL,TO_CHAR);
		}
	}

	if ( is_affected( victim, gsn_damage_reduction ) && dam > 0 )
	{
		AFFECT_DATA * pAff;

		if ( ( pAff = affect_find( victim->affected, gsn_damage_reduction ) ) != NULL )
		{
			if ( pAff->level <= 16 )
				dam -= 1;
			else if ( pAff->level <= 20 )
				dam -= 2;
			else if ( pAff->level <= 25 )
				dam -= 3;
			else if ( pAff->level <= 30 )
				dam -= 4;
			else
				dam -= 5;

			dam = UMAX( dam, 1 );
		}
#ifdef INFO
		print_char( ch, "DAM - DAMAGE_REDUCTION: %d\n\r", dam );
#endif
	}

#ifdef INFO
	print_char( ch, "FINAL DAMAGE: %d\n\r", dam );
#endif

	DEBUG_INFO( "onehit:trick_entwine" );
	//tutaj zobaczymy, czy bijac z whipa nie wlazl trik entwine
	if( wield &&
		wield->value[0] == WEAPON_WHIP &&
		victim->position > POS_SITTING &&
		IS_SET(victim->parts, PART_LEGS ) )
	{
		if( second )
			SET_BIT( ch->fight_data, FIGHT_TRICK_WIELDSECOND );

        check_trick( ch, victim, SN_TRICK_ENTWINE );

		if( IS_SET( ch->fight_data, FIGHT_TRICK_WIELDSECOND ) )
			REMOVE_BIT( ch->fight_data, FIGHT_TRICK_WIELDSECOND );
	}

	DEBUG_INFO( "onehit:trick_crushing_mace" );
	if( wield && wield->value[0] == WEAPON_MACE )
	{
		if( second )
        {
			SET_BIT( ch->fight_data, FIGHT_TRICK_WIELDSECOND );
        }

        check_trick( ch, victim, SN_TRICK_CRUSHING_MACE );

		if( IS_SET( ch->fight_data, FIGHT_TRICK_WIELDSECOND ) )
        {
			REMOVE_BIT( ch->fight_data, FIGHT_TRICK_WIELDSECOND );
        }

		if( IS_SET( ch->fight_data, FIGHT_CRUSHING_MACE_DAM ) )
		{
			dam *= 2;
			REMOVE_BIT( ch->fight_data, FIGHT_CRUSHING_MACE_DAM );
		}
	}

    DEBUG_INFO( "onehit:trick_bleed" );
    if
        (
         wield
         && !IS_WEAPON_STAT( wield, WEAPON_TWO_HANDS )
         && !second
         && ( wield->value[0] == WEAPON_SWORD || wield->value[0] == WEAPON_SHORTSWORD )
         && !is_affected( victim, gsn_bleed )
         && victim->size <= SIZE_LARGE
         && ( IS_SET( victim->form, FORM_WARM ) || IS_SET( victim->form, FORM_COLD_BLOOD ) )
        )
        {
            if( second )
                SET_BIT( ch->fight_data, FIGHT_TRICK_WIELDSECOND );

            check_trick( ch, victim, SN_TRICK_BLEED );

            if( IS_SET( ch->fight_data, FIGHT_TRICK_WIELDSECOND ) )
            {
                REMOVE_BIT( ch->fight_data, FIGHT_TRICK_WIELDSECOND );
            }
        }

	DEBUG_INFO( "onehit:trick_thigh_jab" );
	if( wield &&
		wield->value[0] == WEAPON_DAGGER &&
		!is_affected(victim, gsn_thigh_jab ) &&
		victim->size <= SIZE_LARGE &&
		victim->size >= SIZE_SMALL &&
		IS_SET(victim->parts, PART_LEGS ) )
	{
		if( second )
			SET_BIT( ch->fight_data, FIGHT_TRICK_WIELDSECOND );

		check_trick( ch, victim, SN_TRICK_THIGH_JAB );

		if( IS_SET( ch->fight_data, FIGHT_TRICK_WIELDSECOND ) )
        {
			REMOVE_BIT( ch->fight_data, FIGHT_TRICK_WIELDSECOND );
        }
	}

	DEBUG_INFO( "onehit:dam_mesg" );
	/* walimy opis ciosu*/
	if( wield &&
		wield->value[0] == WEAPON_SPEAR &&
		IS_WEAPON_STAT( wield, WEAPON_TWO_HANDS ) &&
		victim->size < SIZE_HUGE &&
		victim->size > SIZE_TINY &&
		check_trick( ch, victim, SN_TRICK_GLORIOUS_IMPALE ) )
	{
		if( victim->sex == SEX_FEMALE )
		{
			act( "Trafiasz $C prosto w brzuch, $N wisi nadziana na $h.", ch, wield, victim, TO_CHAR );
			act( "$n trafia ci� prosto w brzuch, wisisz nadziana na $h.", ch, wield, victim, TO_VICT );
			act( "$n trafia $C prosto w brzuch, $N wisi nadziana na $h.", ch, wield, victim,TO_NOTVICT );
		}
		else if( victim->sex == SEX_MALE )
		{
			act( "Trafiasz $C prosto w brzuch, $N wisi nadziany na $h.", ch, wield, victim, TO_CHAR );
			act( "$n trafia ci� prosto w brzuch, wisisz nadziany na $h.", ch, wield, victim, TO_VICT );
			act( "$n trafia $C prosto w brzuch, $N wisi nadziany na $h.", ch, wield, victim,TO_NOTVICT );
		}
		else
		{
			act( "Trafiasz $C prosto w brzuch, $N wisi nadziane na $h.", ch, wield, victim, TO_CHAR );
			act( "$n trafia ci� prosto w brzuch, wisisz nadziane na $h.", ch, wield, victim, TO_VICT );
			act( "$n trafia $C prosto w brzuch, $N wisi nadziane na $h.", ch, wield, victim,TO_NOTVICT );
		}
		dam *= 3;
	}
	else if ( dreadful )//opis z trika dreadful strike jest w kodzie trika
		;
	else if ( !special_damage_message( ch, victim, wield, dam, dt, FALSE ) )
		dam_message( ch, victim, dam, dt, immune );

	DEBUG_INFO( "onehit:raw_damage" );
	raw_damage( ch, victim, dam );

	//trick divine impact
	if( dt == gsn_smite || dt == gsn_smite_good )
    {
        check_trick( ch, victim, SN_TRICK_DIVINE_IMPACT );
	}

	/* koniec ciekawostek dla umierajacych*/
	//noo moze nie do konca, jak koles z featem do berka kogos upioerdoli
	//a jego grupka dalej walczy to robi automatycznie assista
	if ( ch->fighting != victim || !victim->in_room )
    {
        if ( IS_SET( ch->fight_data, FIGHT_OVERWHELMING_STRIKE ) )
        {
            REMOVE_BIT( ch->fight_data, FIGHT_OVERWHELMING_STRIKE );
        }
        return;
    }

	DEBUG_INFO( "onehit:check_armor" );
	/* dla tych co przezyli, sprawdzamy armora */
	check_armor_spell( victim, dam );

	//overwhelming strike reszta bajerow
	DEBUG_INFO( "onehit:overwhelming_strike" );
	if ( wield && (IS_SET( ch->fight_data, FIGHT_OVERWHELMING_STRIKE ) || IS_SET( ch->fight_data, FIGHT_THOUSANDSLAYER ) )&& IS_WEAPON_STAT( wield, WEAPON_TWO_HANDS ) )
	{
		overwhelming_skill = get_skill(ch, gsn_overwhelming_strike );

		//i tutaj mechanizm wag z basha, troche utrudniony
		overwhelming_skill -= 6 * URANGE( 0, get_curr_stat_deprecated( victim, STAT_CON ) - 18, 10 );

		if ( victim->weight > 0 && ch->weight > 0 )
		{
			if ( ch->weight > victim->weight )
			{
				overwhelming_skill += 5 * ch->weight / victim->weight;
			}
			else
			{
				if ( ch->weight < victim->weight / 2 )
					overwhelming_skill = UMIN( overwhelming_skill, 5 );
				else
					overwhelming_skill = overwhelming_skill * ch->weight / victim->weight;
			}
		}

		/* speed */
		if ( EXT_IS_SET( ch->off_flags, OFF_FAST ) || IS_AFFECTED( ch, AFF_HASTE ) )
			overwhelming_skill += 15;

		if ( EXT_IS_SET( victim->off_flags, OFF_FAST ) || IS_AFFECTED( victim, AFF_HASTE ) )
			overwhelming_skill -= 15;

		if ( dam < number_range( 27, 35 ) )
			overwhelming_skill -= ( 40 - dam )*2;

		if ( victim->position <= POS_SITTING )
			overwhelming_skill = 0;

		overwhelming_skill = URANGE( 0, overwhelming_skill, 95 );

		if ( number_percent() < overwhelming_skill / 2 && !IS_AFFECTED(victim,AFF_STABILITY) && form_check( ch, victim, NULL, gsn_bash ) && dt != gsn_slam)
		{
			act( "$n uderza z potworn� si��, $N przewraca sie.", ch, NULL, victim, TO_NOTVICT );
			act( "$N trafia cie z potworn� si��, probujesz z�apa� rownowag� jednak przewracasz sie.", victim, wield, ch, TO_CHAR );
			print_char( ch, "W�o�y�%s w ten cios sporo energii, %s przewraca si� na ziemi�.\n\r", ch->sex == 2 ? "a�" : "e�", PERS( victim, ch ) );
			WAIT_STATE( victim, 2 * PULSE_VIOLENCE );
			victim->position = POS_SITTING;
		}

		//z obuchowej broni moze dac daze_state
		if (!( IS_AFFECTED( victim, AFF_PARALYZE ) || IS_AFFECTED( victim, AFF_DAZE ) ))
		{
			if ( wield->value[0] == WEAPON_MACE || wield->value[0] == WEAPON_FLAIL || wield->value[0] == WEAPON_STAFF )
			{
				if ( number_percent() < get_skill(ch,gsn_overwhelming_strike) && !stat_throw(victim, STAT_CON ) && !stat_throw(victim, STAT_WIS ) )
				{
					DAZE_STATE( victim, get_skill(ch,gsn_overwhelming_strike)/2 );
					send_to_char( "Dostajesz zawrot�w g�owy!\n\r", victim );
					if( victim->sex == 2 )
						act( "$n wygl�da na lekko oszo�omion�.", victim, NULL, NULL, TO_ROOM );
					else
						act( "$n wygl�da na lekko oszo�omionego.", victim, NULL, NULL, TO_ROOM );
				}
			}
		}

		//z klujacej broni moze dziurawic
		if ( wield->value[0] == WEAPON_SPEAR && !is_undead(victim) && ( IS_SET( victim->form, FORM_WARM ) || IS_SET( victim->form, FORM_COLD_BLOOD ) ) )
		{
			//tutaj szansa na dziurke typu instant kill
			if ( victim->level < ch->level/3 ||
					( victim->level-1 <= ch->level/3 && IS_WEAPON_STAT(wield, WEAPON_SHARP ) ) ||
					( victim->level <= ch->level/2 && IS_WEAPON_STAT(wield, WEAPON_VORPAL ) ) )
			{
				if( number_percent() < get_skill(ch,gsn_overwhelming_strike ) && !stat_throw(victim,STAT_CON ) )
				{
					make_blood(victim);
					act( "$p przebija na wylot korpus $z wywo�uj�c fontann� krwii z rozerwanych t�tnic. Cia�o $z osuwa si� bezw�adnie na ziemi�.", victim, wield, NULL, TO_ROOM );
					act( "$p przebija na wylot tw�j korpus wywo�uj�c fontann� krwii z rozerwanych t�tnic. Osuwasz si� na ziemi� i umierasz.", victim, wield, NULL, TO_CHAR );
					raw_damage( ch, victim, victim->hit + 12 );
					return;
				}
			}//tutaj kill dobijajacy rannego
			else if ( ch->level - number_range( 4, 7 ) > victim->level )
			{
				over_chance = get_curr_stat_deprecated( ch, STAT_STR ) - 10;
				over_chance *= 2;
				if( get_curr_stat_deprecated(ch,STAT_DEX) > 20 ) over_chance += URANGE( 1, (get_curr_stat_deprecated(ch,STAT_DEX)-20)/2, 4 );
				if( get_curr_stat_deprecated(victim,STAT_DEX) > 20 ) over_chance -= URANGE( 1, (get_curr_stat_deprecated(victim,STAT_DEX)-20)/2, 4 );
				if( IS_WEAPON_STAT( wield, WEAPON_SHARP ) ) over_chance += 8;
				if( IS_WEAPON_STAT( wield, WEAPON_VORPAL ) ) over_chance += 15;

				over_hap = ( victim->hit * 100 )/get_max_hp(victim);

				if( over_hap < over_chance && !stat_throw(victim,STAT_CON) && number_percent() < get_skill(ch,gsn_overwhelming_strike ) )
				{
					make_blood(victim);
					act( "$p przebija na wylot korpus $z wywo�uj�c fontann� krwii z rozerwanych t�tnic. Cia�o $z osuwa si� bezw�adnie na ziemi�.", victim, wield, NULL, TO_ROOM );
					act( "$p przebija na wylot tw�j korpus wywo�uj�c fontann� krwii z rozerwanych t�tnic. Osuwasz si� na ziemi� i umierasz.", victim, wield, NULL, TO_CHAR );
					raw_damage( ch, victim, victim->hit + 12 );
					return;
				}
			}
			else if( number_percent() < get_skill(ch,gsn_overwhelming_strike ) && !is_affected(victim,gsn_bleeding_wound ) )
			{//a tutaj bleeding wound
				act( "G��bokie d�gni�cie zadane $j pozostawia na ciele $z krwawi�c� ran�!", victim, wield, NULL, TO_ROOM );
				act( "G��bokie d�gni�cie zadane $j pozostawia na twoim ciele krwawi�c� ran�!", victim, wield, NULL, TO_CHAR );
				bleeding_wound.where = TO_AFFECTS;
				bleeding_wound.type = gsn_bleeding_wound;
				bleeding_wound.level = 50;
				bleeding_wound.duration = URANGE( 3, (30-get_curr_stat_deprecated(victim,STAT_CON))/2, 10 );
				bleeding_wound.rt_duration = 0;
				bleeding_wound.location = APPLY_NONE;
				bleeding_wound.modifier = 0;
				bleeding_wound.bitvector = &AFF_NONE;
				affect_to_char(	victim,	&bleeding_wound, NULL, TRUE );
			}
		}

		//a tutaj crushowanie sprzetu, tylko duzo slabsze niz z do_crush

		body = get_eq_char( victim, WEAR_BODY );
		arms = get_eq_char( victim, WEAR_ARMS );
		legs = get_eq_char( victim, WEAR_LEGS );
		hands = get_eq_char( victim, WEAR_HANDS );
		feet = get_eq_char( victim, WEAR_FEET );
		head = get_eq_char( victim, WEAR_HEAD );
		bron = get_eq_char( victim, WEAR_WIELD );

		switch (dice(1,10))
		{
			case 1:
			case 2:
			case 3:
				if ( body == NULL || IS_OBJ_STAT(body, ITEM_UNDESTRUCTABLE) )
					break;
				if ( body->material == 11 || body->material == 15 || body->material == 31 )
				{
					destroy_obj( body->carried_by, body );
					break;
				}
				body->condition -= URANGE( 1, dam/5, 15 );
				if (body->condition < 1 )
				{
					destroy_obj( body->carried_by, body );
				}
				break;
			case 4:
				if ( feet == NULL || IS_OBJ_STAT(feet, ITEM_UNDESTRUCTABLE) )
					break;
				if ( feet->material == 11 || feet->material == 15 || feet->material == 31 )
				{
					destroy_obj( feet->carried_by, feet );
					break;
				}
				feet->condition -= URANGE( 1, dam/5, 15 );
				if (feet->condition < 1 )
				{
					destroy_obj( feet->carried_by, feet );
				}
				break;
			case 5:
				if ( head == NULL || IS_OBJ_STAT(head, ITEM_UNDESTRUCTABLE) )
					break;
				if ( head->material == 11 || head->material == 15 || head->material == 31 )
				{
					destroy_obj( head->carried_by, head );
					break;
				}
				head->condition -= URANGE( 1, dam/5, 15 );
				if (head->condition < 1 )
				{
					destroy_obj( head->carried_by, head );
				}
				break;
			case 6:
			case 7:
				if ( arms == NULL || IS_OBJ_STAT(arms, ITEM_UNDESTRUCTABLE))
					break;
				if ( arms->material == 11 || arms->material == 15 || arms->material == 31 )
				{
					destroy_obj( arms->carried_by, arms );
					break;
				}
				arms->condition -= URANGE( 1, dam/5, 15 );
				if (arms->condition < 1 )
				{
					destroy_obj( arms->carried_by, arms );
				}
				break;
			case 8:
			case 9:
				if ( legs == NULL || IS_OBJ_STAT(legs, ITEM_UNDESTRUCTABLE) )
					break;
				if ( legs->material == 11 || legs->material == 15 || legs->material == 31 )
				{
					destroy_obj( legs->carried_by, legs );
					break;
				}
				legs->condition -= URANGE( 1, dam/5, 15 );
				if (legs->condition < 1 )
				{
					destroy_obj( legs->carried_by, legs );
				}
				break;
			case 10:
				if ( hands == NULL || IS_OBJ_STAT(hands, ITEM_UNDESTRUCTABLE) )
					break;
				if ( hands->material == 11 || hands->material == 15 || hands->material == 31 )
				{
					destroy_obj( hands->carried_by, hands );
					break;
				}
				hands->condition -= URANGE( 1, dam/5, 15 );
				if (hands->condition < 1 )
				{
					destroy_obj( hands->carried_by, hands );
				}
				break;
		}

		REMOVE_BIT( ch->fight_data, FIGHT_OVERWHELMING_STRIKE );
	}

	DEBUG_INFO( "onehit:power_charge" );
	/* wyjatkowo mocne pierdolniecie chargem kladzie kolesia na glebie */
	if ( dt == gsn_charge && wield && !IS_AFFECTED(victim,AFF_STABILITY) )
    {
        //Raszer - zmniejszenie szansy na wywalenie charge z skill/2 do skill/3
        if ( victim->position >= POS_FIGHTING &&
                ( IS_SET( race_table[ GET_RACE(victim) ].type, PERSON ) || IS_SET( race_table[ GET_RACE(victim) ].type, ANIMAL ) ) &&
                number_percent() < get_skill( ch, gsn_charge ) / 3 &&
                ( victim->size <= ch->size ) )
        {
            act( "$n uderza z potworn� si��, $N przewraca sie.", ch, wield, victim, TO_NOTVICT );
            act( "$N trafia cie z potworn� si��, probujesz z�apa� rownowag� jednak przewracasz sie.", victim, wield, ch, TO_CHAR );
            print_char( ch, "Wlozyl%s w ten cios sporo energii, %s przewraca si� na ziemi�.\n\r", ch->sex == 2 ? "a�" : "e�", PERS( victim, ch ) );
            check_trick( ch, victim, SN_TRICK_STRUCKING_WALLOP );
            victim->position = POS_SITTING;
            WAIT_STATE( victim, 1.5 * PULSE_VIOLENCE );
        }
    }
	else if ( dt == gsn_stun && dam > 0 )
	{
		int daze_pulse = 1;

		if ( IS_NPC( ch ) )
			daze_pulse = URANGE( 2, 2 + ch->level / 7, 7 );
		else
		{
			if ( ch->class == CLASS_PALADIN || ch->class == CLASS_WARRIOR || ch->class == CLASS_SHAMAN )
				daze_pulse = number_range( 1, URANGE( 1, 1 + get_skill( ch, gsn_stun ) / 30, 4 ) );
			else if ( ch->class == CLASS_CLERIC )
				daze_pulse = number_range( 1, URANGE( 1, get_skill( ch, gsn_stun ) / 40, 2 ) );

			if ( critical )
				daze_pulse++;
		}

		if ( IS_AFFECTED( victim, AFF_BERSERK ) )
		{
			//szansa taka jaki dam, ale na polowe czasu
			if ( number_percent() < dam )
			{
				daze_pulse = UMAX( 1, daze_pulse / 2 );
				DAZE_STATE( victim, daze_pulse * PULSE_VIOLENCE );
			}
		}
		else
			DAZE_STATE( victim, daze_pulse * PULSE_VIOLENCE );
	}

	//mo�e nie mie� ju� broni (trick strucking_wallop)
	if ( !second )
		wield = get_eq_char( ch, WEAR_WIELD );
	else
		wield = get_eq_char( ch, WEAR_SECOND );

	DEBUG_INFO( "onehit:update_obj_condition" );
	if ( result )
		update_obj_cond( ch, wield, base_dam, dam, victim );

	//mo�e nie mie� ju� broni jak si� rozpad�a
	if ( !second )
		wield = get_eq_char( ch, WEAR_WIELD );
	else
		wield = get_eq_char( ch, WEAR_SECOND );

	DEBUG_INFO( "onehit:spell_draining_hands" );
	if ( ch->fighting == victim && is_affected(ch,gsn_draining_hands) && !is_undead( victim ) && !IS_SET( victim->form, FORM_CONSTRUCT ) )
	{
		dam = number_range( base_dam / 4, base_dam / 2 );
		if (dam>0)
		{
			act( "Tw�j dotyk wysysa �ycie z $Z.", ch, NULL, victim, TO_CHAR );
			act( "Czujesz jak dotyk $z wysysa z ciebie energi�.", ch, NULL, victim, TO_VICT );
			act( "Dotyk $z wysysa �ycie z $Z.", ch, NULL, victim, TO_NOTVICT );
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

	DEBUG_INFO( "onehit:funky_weapon" );
	/* but do we have a funky weapon? */
	if ( result && wield != NULL )
	{
		int dam;

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
                if (wield->gender < GENDER_ZENSKI) {
                    act( "$p wysysa �ycie z $z.", victim, wield, NULL, TO_ROOM );
                    act( "Czujesz jak $p wysysa z ciebie energi�.", victim, wield, NULL, TO_CHAR );
                }
                else {
                    act( "$p wysysaj� �ycie z $z.", victim, wield, NULL, TO_ROOM );
                    act( "Czujesz jak $p wysysaj� z ciebie energi�.", victim, wield, NULL, TO_CHAR );
                }
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
	
		
		if ( ch->fighting == victim && number_percent() < 5 && IS_WEAPON_STAT( wield, WEAPON_FLAMING_BURST ) )
		{
				dam = UMIN(wield->value[6], wield->value[5]) * number_range( 8, 15 ) + number_range(30, 45);
				dam = UMAX(dam, number_range(30, 45)); //Dla broni z ujemnymi v5 i v6
				act( "Fala ognia wylatuje z $j i trafia w $N sprawiajac, ze staje przez chwil� w p�omieniach.", ch, wield, victim, TO_CHAR );
				act( "$N staje na krotk� chwil� w p�omieniach kiedy dostaje ognista fal� z $j $z.", ch, wield, victim, TO_NOTVICT );
				act( "$p uderza w ciebie fal� p�omieni.", ch, wield, victim, TO_VICT );
				fire_effect( victim, 1, dam, TARGET_CHAR );
				damage( ch, victim, dam, 0, DAM_FIRE, FALSE );
		}
		else if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_FLAMING ) || IS_WEAPON_STAT( wield, WEAPON_FLAMING_BURST ) )
		{
			dam = number_range( base_dam / 4, base_dam / 2 );
			dam = UMAX( dam, 1 );
			act( "$p lekko przysma�a $C.", ch, wield, victim, TO_CHAR );
			act( "Parzysz si� przy zetkni�ciu z $j.", ch, wield, victim, TO_VICT );
			act( "$N parzy si� przy zetkni�ciu z $j.", ch, wield, victim, TO_NOTVICT );
			fire_effect( ( void * ) victim, 1, dam, TARGET_CHAR );
			damage( ch, victim, dam, 0, DAM_FIRE, FALSE );
		}

		if ( ch->fighting == victim && number_percent() < 5 && IS_WEAPON_STAT( wield, WEAPON_FROST_BURST ) )
		{
				dam = UMIN(wield->value[6], wield->value[5]) * number_range( 8, 15 ) + number_range(30, 45);
				dam = UMAX(dam, number_range(30, 45)); //Dla broni z ujemnymi v5 i v6
				act( "$p rozb�yskuje zimnem i mro��ny podmuch uderza w $c.", victim, wield, NULL, TO_ROOM );
				act( "Mro�ny podmuch wiatru z $f mrozi ci� do ko�ci.", ch, wield, victim, TO_VICT );
				cold_effect( victim, 1, dam, TARGET_CHAR );
				damage( ch, victim, dam, 0, DAM_COLD, FALSE );
		}
		else if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_FROST ) || IS_WEAPON_STAT( wield, WEAPON_FROST_BURST ) )
		{
			dam = number_range( base_dam / 4, base_dam / 2 );
			dam = UMAX( dam, 1 );
			act( "$p zamra�a $c.", victim, wield, NULL, TO_ROOM );
			act( "Zimny dotyk $f zamienia ci� w bry�� lodu.", victim, wield, NULL, TO_CHAR );
			cold_effect( victim, 1, dam, TARGET_CHAR );
			damage( ch, victim, dam, 0, DAM_COLD, FALSE );
		}

		if ( ch->fighting == victim && number_percent() < 5 && IS_WEAPON_STAT( wield, WEAPON_SHOCKING_BURST ) )
		{
				dam = UMIN(wield->value[6], wield->value[5]) * number_range( 8, 15 ) + number_range(30, 45);
				dam = UMAX(dam, number_range(30, 45)); //Dla broni z ujemnymi v5 i v6
				act( "W nag�bym rozb�ysku �wiat�a $f pora�a $c moc� piorun�w.", victim, wield, NULL, TO_ROOM );
				act( "$p pora�a ci� olbrzymi� moc� piorun�w.", victim, wield, NULL, TO_CHAR );
				shock_effect( victim, 1, dam, TARGET_CHAR );
				damage( ch, victim, dam, 0, DAM_LIGHTNING, FALSE );
		}
		else if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_SHOCKING ) || IS_WEAPON_STAT( wield, WEAPON_SHOCKING_BURST ) )
		{
			dam = number_range( base_dam / 4, base_dam / 2 );
			dam = UMAX( dam, 1 );
			act( "B�yskawica z $f trafia $c wywo�uj�c u niego niekontrolowane drgawki.", victim, wield, NULL, TO_ROOM );
			act( "$p aplikuje ci seri� elektrowstrzas�w.", victim, wield, NULL, TO_CHAR );
			shock_effect( victim, 1, dam, TARGET_CHAR );
			damage( ch, victim, dam, 0, DAM_LIGHTNING, FALSE );
		}

		if ( ch->fighting == victim && number_percent() < 5 && IS_WEAPON_STAT( wield, WEAPON_TOXIC_BURST ) )
		{
				dam = UMIN(wield->value[6], wield->value[5]) * number_range( 8, 15 ) + number_range(30, 45);
				dam = UMAX(dam, number_range(30, 45)); //Dla broni z ujemnymi v5 i v6
				act( "Pulsuj�c moc� $f trafia kwasowym pociskiem w $c.", victim, wield, NULL, TO_ROOM );
				act( "Kwasowy pocisk z $f trafia ci� i pali!", victim, wield, NULL, TO_CHAR );
				acid_effect( victim, 1, dam, TARGET_CHAR );
				damage( ch, victim, dam, 0, DAM_ACID, FALSE );
		}
		else if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_TOXIC ) || IS_WEAPON_STAT( wield, WEAPON_TOXIC_BURST ) )
		{
			dam = number_range( base_dam / 4, base_dam / 2 );
			dam = UMAX( dam, 1 );
			act( "Pokrywaj�cy $h kwas wy�era sk�r� $z.", victim, wield, NULL, TO_ROOM );
			act( "Pokrywaj�cy $h kwas wy�era tw� sk�r�!", victim, wield, NULL, TO_CHAR );
			acid_effect( victim, 1, dam, TARGET_CHAR );
			damage( ch, victim, dam, 0, DAM_ACID, FALSE );
		}

		if ( ch->fighting == victim && number_percent() < 5 && IS_WEAPON_STAT( wield, WEAPON_SONIC_BURST ) )
		{
				dam = UMIN(wield->value[6], wield->value[5]) * number_range( 8, 15 ) + number_range(30, 45);
				dam = UMAX(dam, number_range(30, 45)); //Dla broni z ujemnymi v5 i v6
			    act( "W nag�ym rozb�ysku moc $f pora�a $c fal� dzwi�k�w.", victim, wield, NULL, TO_ROOM );
			    act( "Pulsuj�ca moc $f pora�a ci� prawdziw� kakofoni� fal dzwi�kowych.", victim, wield, NULL, TO_CHAR );
				sound_effect( victim, 1, dam, TARGET_CHAR );
				damage( ch, victim, dam, 0, DAM_SOUND, FALSE );
		}
		else if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_RESONANT ) || IS_WEAPON_STAT( wield, WEAPON_SONIC_BURST ) )
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
		if ( ch->fighting == victim && number_percent() < 5 && IS_WEAPON_STAT( wield, WEAPON_HOLY )  )
		{
				dam = UMIN(wield->value[6], wield->value[5]) * number_range( 4, 12 ) + number_range(40, 60);
				act( "Rozb�yskuj�c pot�na �wi�ta moc $f trafia w $c.", victim, wield, NULL, TO_ROOM );
				act( "Rozb�yskuj�c pot�n� ��wi�t� moc� $f trafia ci� prosto w pier�.", victim, wield, NULL, TO_CHAR );
				dam = UMAX(dam, number_range(20, 35)); //Dla broni z ujemnymi v5 i v6
				damage( ch, victim, dam, 0, DAM_HOLY, FALSE );
				if( IS_NPC(victim) && victim->pIndexData->vnum == MOB_VNUM_AVATAR )
				{
					act( "{5W kr�tkim rozb�ysku �wiat�a $n zamienia si� w kupk� dymi�cego popio�u.{x", victim, NULL, NULL, TO_ROOM );
					raw_damage( ch, victim, 20 + get_max_hp(victim) );
				}
		}
		else if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_SACRED ) || IS_WEAPON_STAT( wield, WEAPON_HOLY ) )
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
		
			if ( ch->fighting == victim && number_percent() < 5 && IS_WEAPON_STAT( wield, WEAPON_UNHOLY ) )
			{
				dam = UMIN(wield->value[6], wield->value[5]) * number_range( 4, 12 ) + number_range(35, 50);
				act( "Pot�na z�a moc wylatuje z $f i trafia w $c.", victim, wield, NULL, TO_ROOM );
				act( "Pot�na z�a moc promienuj�ca z $f otacza i pr�buje ci� zniszczy�.", victim, wield, NULL, TO_CHAR );
				dam = UMAX(dam, number_range(20, 35)); //Dla broni z ujemnymi v5 i v6				
				damage( ch, victim, dam, 0, DAM_NEGATIVE, FALSE );
			}			
			else if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_NEGATIVE ) || IS_WEAPON_STAT( wield, WEAPON_UNHOLY ) )
			{
				dam = number_range( base_dam / 4, base_dam / 2 );
				dam = UMAX( dam, 1 );
				act( "Ciemnoczerwona aura z $f dotyka $c.", victim, wield, NULL, TO_ROOM );
				act( "Ciemnoczerwona aura z $f otacza i rani ci�.", victim, wield, NULL, TO_CHAR );
				damage( ch, victim, dam, 0, DAM_NEGATIVE, FALSE );
			}

		if ( ch->fighting == victim && !is_affected(victim,gsn_bleeding_wound ) && !is_undead(victim) &&
				( IS_SET( victim->form, FORM_WARM ) || IS_SET( victim->form, FORM_COLD_BLOOD ) ) &&
				( IS_WEAPON_STAT( wield, WEAPON_SHARP ) || IS_WEAPON_STAT( wield, WEAPON_VORPAL ) || IS_WEAPON_STAT( wield, WEAPON_INJURIOUS ) ) )
		{
			//szansa na zranienie
			if( IS_WEAPON_STAT( wield, WEAPON_SHARP ) && !stat_throw( victim, STAT_CON ) && number_percent() <= 15 )
			{
				act( "Cios zadany $j pozostawia na ciele $z krwawi�c� ran�!", victim, wield, NULL, TO_ROOM );
				act( "Cios zadany $j pozostawia na twoim ciele krwawi�c� ran�!", victim, wield, NULL, TO_CHAR );
				bleeding_wound.where = TO_AFFECTS;
				bleeding_wound.type = gsn_bleeding_wound;
				bleeding_wound.level = 50;
				bleeding_wound.duration = number_range(2,4);
				bleeding_wound.rt_duration = 0;
				bleeding_wound.location = APPLY_NONE;
				bleeding_wound.modifier = 0;
				bleeding_wound.bitvector = &AFF_NONE;
				affect_to_char(	victim,	&bleeding_wound, NULL, TRUE );
			}
			else if( IS_WEAPON_STAT( wield, WEAPON_VORPAL ) && !stat_throw( victim, STAT_CON ) && number_percent() <= 35 )
			{
				act( "Cios zadany $j pozostawia na ciele $z krwawi�c� ran�!", victim, wield, NULL, TO_ROOM );
				act( "Cios zadany $j pozostawia na twoim ciele krwawi�c� ran�!", victim, wield, NULL, TO_CHAR );
				bleeding_wound.where = TO_AFFECTS;
				bleeding_wound.type = gsn_bleeding_wound;
				bleeding_wound.level = 50;
				bleeding_wound.duration = number_range(3,5);
				bleeding_wound.rt_duration = 0;
				bleeding_wound.location = APPLY_NONE;
				bleeding_wound.modifier = 0;
				bleeding_wound.bitvector = &AFF_NONE;
				affect_to_char(	victim,	&bleeding_wound, NULL, TRUE );
			}
			else if( IS_WEAPON_STAT( wield, WEAPON_INJURIOUS ) && number_percent() <= 25 )
			{
				act( "Cios zadany $j pozostawia na ciele $z krwawi�c� ran�!", victim, wield, NULL, TO_ROOM );
				act( "Cios zadany $j pozostawia na twoim ciele krwawi�c� ran�!", victim, wield, NULL, TO_CHAR );
				bleeding_wound.where = TO_AFFECTS;
				bleeding_wound.type = gsn_bleeding_wound;
				bleeding_wound.level = 50;
				bleeding_wound.duration = number_range(7,10);
				bleeding_wound.rt_duration = 0;
				bleeding_wound.location = APPLY_NONE;
				bleeding_wound.modifier = 0;
				bleeding_wound.bitvector = &AFF_NONE;
				affect_to_char(	victim,	&bleeding_wound, NULL, TRUE );
			}
		}

		if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_DISPEL ) )
		{
			weapon_dispeller(ch, victim, base_dam);
		}

	}

    /**
     * WEAPON_WICKED
     */
    if ( wield && IS_SET( wield->value[ 4 ], WEAPON_WICKED ) )
    {
        print_char( ch, "{r%s{m wysysa z ciebie odrobin� �ycia.{x\n\r", capitalize( wield->short_descr ) );
        raw_damage( ch, ch, number_range( 1, 3 ) );
    }

	if ( result && IS_NPC( ch ) && ch->attack_flags != 0 )
		check_special_unarmed_hit( ch, victim, unarmed_base_dam );

	if ( !wield && IS_NPC( ch ) && HAS_TRIGGER( ch, TRIG_HIT ) )
		mp_hit_trigger( ch, victim );
	else if ( wield && HAS_OTRIGGER( wield, TRIG_HIT ) )
		op_hit_trigger( wield, ch );

	tail_chain( );

    /**
     * drop parts
     */
    part_dropper_wraper( victim );

	return;
}

/*
 * Inflict damage from a hit (do_damage).
 */
bool damage( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt, int dam_type, bool show )
{
	OBJ_DATA * obj, *obj_next;
	bool immune = FALSE, check;

	save_debug_info("fight.c => damage", NULL, NULL, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );


	if ( dam < 0 )
	{
		victim->hit = UMIN( get_max_hp(victim), victim->hit - dam );
		update_pos( victim );
		return TRUE;
	}

	if ( victim->position == POS_DEAD )
		return FALSE;

	if ( victim != ch )
	{
		if ( is_safe( ch, victim ) )
			return FALSE;

		if ( victim->position > POS_STUNNED )
		{
			if ( victim->fighting == NULL )
			{
				//ZMIANA
				if ( !can_move( victim ) &&
						( ch->fighting == victim || ch->fighting == NULL ) )
					set_fighting( victim, ch );

				if ( can_move( victim ) )
					set_fighting( victim, ch );

				if ( IS_NPC( victim ) && HAS_TRIGGER( victim, TRIG_KILL ) )
					mp_percent_trigger( victim, ch, NULL, NULL, &TRIG_KILL );
			}
		}

		if ( ch->position > POS_STUNNED )
		{
			if ( ch->fighting == NULL )
				set_fighting( ch, victim );
		}


		if ( victim->master == ch )
		{
			if( IS_NPC( victim ) && IS_AFFECTED(victim,AFF_CHARM) )
			{
				act( "Wi� mi�dzy tob� a $V zostaje brutalnie przerwana!", ch, NULL, victim, TO_CHAR );
				raw_damage( ch, ch, number_range( ch->level/2, ch->level+ch->level/2 ) );
				if ( ch->position <= POS_STUNNED )
					return FALSE;
			}
			stop_follower( victim );
		}
	}

	/*
	 * Inviso attacks ... not.
	 */
	if ( IS_AFFECTED( ch, AFF_HIDE ) )
		affect_strip( ch, gsn_hide );

	strip_invis( ch, TRUE, TRUE );

	if ( IS_NPC( victim ) && !is_fearing( victim, ch ) )
	{
		if ( !EXT_IS_SET( victim->act, ACT_SENTINEL ) )
			start_hunting( victim, ch );

		start_hating( victim, ch );
	}

	if ( dam == 0 )
	{
		if ( show )
			dam_message( ch, victim, dam, dt, immune );

		return FALSE;
	}

	if ( ( dam_type == DAM_BASH ||
				dam_type == DAM_PIERCE ||
				dam_type == DAM_SLASH ) &&
			victim->resists[ RESIST_MAGIC_WEAPON ] > 0 && !IS_AFFECTED( victim, AFF_RESIST_MAGIC_WEAPON))
	{
		if ( (dt == gsn_kick && get_eq_char( ch, WEAR_FEET ) != NULL && IS_OBJ_STAT( get_eq_char( ch, WEAR_FEET ), ITEM_MAGIC) && victim->resists[ RESIST_MAGIC_WEAPON ] <= (get_curr_stat_deprecated(ch,STAT_STR)-6)/10 && dice( 2, get_curr_stat_deprecated(ch,STAT_STR)) > 14  ))
        {
			;
        }
		else
		{
			immune = TRUE;
			dam = 0;

			if ( show )
				dam_message( ch, victim, dam, dt, immune );
			return FALSE;
		}
	}

	immune = FALSE;

	if ( dt != gsn_bash && dt != gsn_crush && dt != gsn_slam && dt != gsn_trip )
	{
		immune = check_blink( victim, ch, TRUE );

		if( immune )
		{
			if ( show )
			dam_message( ch, victim, dam, dt, immune );

			act( "Sylwetka chwilowo znika unikaj�c twego ataku.", ch, NULL, victim, TO_CHAR );
		act(	 "Znikasz na chwilk� z tego planu egzystencji unikaj�c ataku $z.\n\r", ch, NULL, victim, TO_VICT );
			act( "Sylwetka $Z chwilowo znika, unikaj�c ataku $z.", ch, NULL, victim, TO_NOTVICT );
			return FALSE;
		}
	}

	if ( dam_type != DAM_NONE && dam_type != DAM_SOUND && dt != gsn_bash && dt != gsn_crush && dt != gsn_turn && dt != gsn_slam && dt != gsn_trip )
	{
		immune = check_mirror_image( victim, ch );

		if ( immune )
		{
			if ( show )
				dam_message( ch, victim, dam, dt, immune );

			print_char( ch, "Lustrzane odbicie %s migocze przez chwil� i znika.\n\r", PERS4( victim, ch ) );
			send_to_char( "Jedno z twoich lustrzanych odbi� migocze przez chwil� i znika.\n\r", victim );
			act( "Lustrzane odbicie $Z migocze przez chwil� i znika.", ch, NULL, victim, TO_NOTVICT );
			return FALSE;
		}
	}

	immune = IS_AFFECTED( victim, AFF_ILLUSION );

	if ( immune )
	{
		if ( show )
			dam_message( ch, victim, dam, dt, immune );

		print_char( ch, "Tw�j cel rozp�ywa si� w powietrzu i znika.\n\r", PERS4( victim, ch ) );
		send_to_char( "Rozp�ywasz si� w powietrzu.\n\r", victim );
		act( "$N rozp�ywa si� w powietrzu.", ch, NULL, victim, TO_NOTVICT );

		/* itemy co je trzymala iluzja opadaja na ziemie*/
		for ( obj = victim->carrying; obj != NULL; obj = obj_next )
		{
			OBJ_NEXT_CONTENT( obj, obj_next );

			if ( IS_OBJ_STAT( obj, ITEM_NODROP ) )
				continue;
			if ( obj->wear_loc != WEAR_NONE )
				unequip_char( victim, obj );

			obj_from_char( obj );
			act( "$p upada na ziemi�.", victim, obj, NULL, TO_ROOM );
			obj_to_room( obj, victim->in_room );

			if ( IS_OBJ_STAT( obj, ITEM_MELT_DROP ) )
			{
				switch ( number_range( 1, 3 ) )
				{
					case 1:
						act( "$p rozp�ywa si� w powietrzu.", ch, obj, NULL, TO_ROOM );
						break;
					case 2:
						act( "$p znika bez �ladu.", ch, obj, NULL, TO_ROOM );
						break;
					default:
						act( "$p rozp�ywa si� pozostawiaj�c tylko chmur� dymu.", ch, obj, NULL, TO_ROOM );
						break;
				}
			}
			else if ( obj->item_type == ITEM_POTION && ( obj->material == 11 || obj->material == 15 ) )
			{
				switch ( number_range( 1, 3 ) )
				{
					case 1:
						act( "$p toczy si� przez chwil�, po czym rozpryskuj� si� na kwa�eczki.", ch, obj, NULL, TO_ROOM );
						break;
					case 2:
						act( "$p p�ka z hukiem i rozpryskuje si� dooko�a.", ch, obj, NULL, TO_ROOM );
						break;
					default:
						act( "$p rozpryskuje si� na kawa�eczki.", ch, obj, NULL, TO_ROOM );
						break;
				}
			}
		}
		extract_char( victim, TRUE );

		return FALSE;
	}

	immune = IS_AFFECTED( victim, AFF_SOUL );

	if ( immune )
	{
		if ( show )
			dam_message( ch, victim, dam, dt, immune );

		print_char( ch, "Uwalniasz dusz� %s, kt�ra ulatuje do swojego wymiaru\r", PERS4( victim, ch ) );
		send_to_char( "Ulatujesz do swojego wymiaru.\n\r", victim );
		act( "$n uwalnia dusz� $Z, kt�ra ulatuje do swojego wymiaru.", ch, NULL, victim, TO_NOTVICT );

		/* itemy co je trzymala dusza opadaja na ziemie*/
		for ( obj = victim->carrying; obj != NULL; obj = obj_next )
		{
			OBJ_NEXT_CONTENT( obj, obj_next );

			if ( IS_OBJ_STAT( obj, ITEM_NODROP ) )
				continue;
			if ( obj->wear_loc != WEAR_NONE )
				unequip_char( victim, obj );

			obj_from_char( obj );
			act( "$p upada na ziemi�.", victim, obj, NULL, TO_ROOM );
			obj_to_room( obj, victim->in_room );

			if ( IS_OBJ_STAT( obj, ITEM_MELT_DROP ) )
			{
				switch ( number_range( 1, 3 ) )
				{
					case 1:
						act( "$p rozp�ywa si� w powietrzu.", ch, obj, NULL, TO_ROOM );
						break;
					case 2:
						act( "$p znika bez �ladu.", ch, obj, NULL, TO_ROOM );
						break;
					default:
						act( "$p rozp�ywa si� pozostawiaj�c tylko chmur� dymu.", ch, obj, NULL, TO_ROOM );
						break;
				}
			}
			else if ( obj->item_type == ITEM_POTION && ( obj->material == 11 || obj->material == 15 ) )
			{
				switch ( number_range( 1, 3 ) )
				{
					case 1:
						act( "$p toczy si� przez chwil�, po czym rozpryskuj� si� na kwa�eczki.", ch, obj, NULL, TO_ROOM );
						break;
					case 2:
						act( "$p p�ka z hukiem i rozpryskuje si� dooko�a.", ch, obj, NULL, TO_ROOM );
						break;
					default:
						act( "$p rozpryskuje si� na kawa�eczki.", ch, obj, NULL, TO_ROOM );
						break;
				}
			}
		}

		extract_char( victim, TRUE );

		return FALSE;
	}

	/* Stone skin - wstawka
	 *
	 */
	immune = check_stone_skin( NULL, victim, dam_type, &dam, NULL );

	if ( dam == 0 )
	{
		if ( show )
			dam_message( ch, victim, dam, dt, immune );

		return FALSE;
	}

	dam = check_resist( victim, dam_type, dam );

	if ( dam == 0 )
	{
		immune = TRUE;

		if ( show )
			dam_message( ch, victim, dam, dt, immune );

		return FALSE;
	}

	if ( IS_AFFECTED( victim, AFF_TROLL ) && victim->hit > 0 && victim->hit - dam < 0 )
	{
		dam = victim->hit + 1;

		create_event( EVENT_TROLL_POWER_REGEN, number_range( 3, 5 ) * PULSE_VIOLENCE, victim, NULL, 0 );
	}

	if ( show )
    {
		dam_message( ch, victim, dam, dt, immune );
    }
	
	raw_damage( ch, victim, dam );

	return TRUE;
}

bool mob_can_attack( CHAR_DATA * ch )
{
	if ( !IS_NPC( ch ) )
		return FALSE;

	if ( ch->pIndexData->pShop || ch->pIndexData->pRepair )
		return FALSE;

	return TRUE;
}

bool is_safe( CHAR_DATA *ch, CHAR_DATA *victim )
{
	if ( ch != victim && is_affected( ch, gsn_sanctuary ) )
		affect_strip( ch, gsn_sanctuary );

	if ( ch->master && IS_AFFECTED( ch, AFF_CHARM ) && is_affected( ch->master, gsn_sanctuary ) )
		affect_strip( ch->master, gsn_sanctuary );

	if ( victim->in_room == NULL || ch->in_room == NULL )
		return TRUE;

	if ( victim->fighting == ch || victim == ch )
		return FALSE;

	if ( IS_IMMORTAL( ch ) && ch->level > LEVEL_IMMORTAL )
		return FALSE;

	if ( EXT_IS_SET( victim->in_room->room_flags, ROOM_SAFE ) )
		return TRUE;

	if ( IS_AFFECTED( victim, AFF_SANCTUARY ) )
		return TRUE;

	if ( IS_AFFECTED( victim, AFF_FORCE_FIELD ) )
		return TRUE;

	/* killing mobiles */
	if ( IS_NPC( victim ) )
	{
		if ( victim->pIndexData->pShop != NULL )
		{
			send_to_char( "Mo�e lepiej by� co� kupi�<&/a/o>?\n\r", ch );
			return TRUE;
		}

		if ( victim->pIndexData->pRepair != NULL )
		{
			send_to_char( "Mo�e lepiej by� co� naprawi�<&/a/o>?\n\r", ch );
			return TRUE;
		}

		/* no killing healers, trainers, etc */
		if ( EXT_IS_SET( victim->act, ACT_PRACTICE ) || EXT_IS_SET( victim->act, ACT_REWARD ) )
		{
			send_to_char( "Naprawd� nie ma co juz zabija�?\n\r", ch );
			return TRUE;
		}
	}
	return FALSE;
}

bool is_safe_spell( CHAR_DATA *ch, CHAR_DATA *victim, bool area )
{
	save_debug_info("fight.c => is_safe_spell", NULL, NULL, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );
	if ( ch != victim && is_affected( ch, gsn_sanctuary ) )
		affect_strip( ch, gsn_sanctuary );

	if ( victim->in_room == NULL || ch->in_room == NULL )
		return TRUE;

	if ( victim == ch && area )
		return TRUE;

	if ( victim->fighting == ch || victim == ch )
		return FALSE;

	if ( IS_IMMORTAL( ch ) && ch->level > LEVEL_IMMORTAL && !area )
		return FALSE;

	if ( EXT_IS_SET( victim->in_room->room_flags, ROOM_SAFE ) )
		return TRUE;

	if ( IS_AFFECTED( victim, AFF_SANCTUARY ) )
		return TRUE;

	/* killing mobiles */
	if ( IS_NPC( victim ) )
	{
		if ( victim->pIndexData->pShop != NULL )
			return TRUE;

		if ( EXT_IS_SET( victim->act, ACT_PRACTICE ) )
			return TRUE;

		if ( IS_NPC( ch ) )
		{
			/* area effect spells do not hit other mobs */
			if ( area && !is_same_group( victim, ch->fighting ) )
				return TRUE;
		}
	}
	/* killing players */
	else
	{
		if ( area && IS_IMMORTAL( victim ) && victim->level > LEVEL_IMMORTAL )
			return TRUE;

		/* NPC doing the killing */
		if ( IS_NPC( ch ) )
		{
			/* charmed mobs and pets cannot attack players while owned */
			if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master != NULL
					&& ch->master->fighting != victim )
				return TRUE;

		}

		/* player doing the killing */
		else
		{
			;
		}

	}
	return FALSE;
}

/*
 * See if an attack justifies a KILLER flag.
 */
void check_killer( CHAR_DATA *ch, CHAR_DATA *victim )
{
	// zdejmujemy atakuj�cemu sanctuary
	if ( is_affected( ch, gsn_sanctuary ) )
	{
		affect_strip( ch, gsn_sanctuary );
	}

	// zdejmujemy zaatakowanemu healing sleep
	if ( is_affected( victim, gsn_healing_sleep ) )
	{
		affect_strip( victim, gsn_healing_sleep );
	}

    // logowanie atak�w
    if (IS_NPC( ch ) && ch->master )
    {
        sprintf( log_buf, "[%d] attacker: %s (%d), defender (%s): %s (%d), master %s (%d)",
                victim->in_room ? victim->in_room->vnum : 0,
                ( IS_NPC( ch ) ? ch->short_descr : ch->name ),
                ch->level,
                ( IS_NPC( victim ) )? "npc":"pc",
                ( IS_NPC( victim ) ? victim->short_descr : victim->name ),
                victim->level,
                ch->master->name,
                ch->master->level
               );
    }
    else
    {
        sprintf( log_buf, "[%d] attacker: %s (%d), defender (%s): %s (%d)",
                victim->in_room ? victim->in_room->vnum : 0,
                ( IS_NPC( ch ) ? ch->short_descr : ch->name ),
                ch->level,
                ( IS_NPC( victim ) )? "npc":"pc",
                ( IS_NPC( victim ) ? victim->short_descr : victim->name ),
                victim->level
               );
    }
    log_string( log_buf );
	return;
}

/*
 * Check for parry.
 */
bool check_parry( CHAR_DATA *ch, CHAR_DATA *victim, int dam, OBJ_DATA *parowana )
{
	int parry_skill, dex_mod = 0, chance = 0, percent;
	int max_dam = 0, prim_skill = 0, offhand_skill = 0, i = 0;
	OBJ_DATA *prim = NULL, *offhand = NULL;
    bool check;

	if (!IS_SET( victim->fight_data, FIGHT_PARRY ) )
	{
        return FALSE;
	}

	if ( !IS_AWAKE( victim ) ||
			!can_move( victim ) ||
			!can_see( victim, ch ) )
    {
		return FALSE;
    }

	parry_skill = get_skill( victim, gsn_parry );

	/*bez broni nie da rady*/
	prim = get_eq_char( victim, WEAR_WIELD );
	offhand = get_eq_char( victim, WEAR_SECOND );

	if ( !prim && !offhand )
	{
		return FALSE;
	}

	dex_mod = (2*(get_curr_stat_deprecated( victim, STAT_DEX )-10))/3;
	chance = parry_skill / 3;

	if( is_affected( ch, gsn_wardance ))
	{
		chance += get_skill(ch, gsn_wardance)/10;
	}

	if ( prim )
	{
		max_dam = dice( prim->value[ 1 ], prim->value[ 2 ] ) + prim->value[ 6 ];
		max_dam += GET_SKILL_DAMROLL( victim, prim );
		prim_skill = get_weapon_skill( victim, get_weapon_sn( victim, TRUE ) );

		if ( offhand )
        {
            offhand_skill = get_weapon_skill( victim, get_weapon_sn( victim, FALSE ) );
            chance += ( parry_mod( prim ) + parry_mod( offhand ) ) / 2;
            chance += dex_mod;

            max_dam += ( dice( offhand->value[ 1 ], offhand->value[ 2 ] ) + offhand->value[ 6 ] ) / 2;
            max_dam += GET_SKILL_DAMROLL( victim, offhand )/2;
            chance += ( prim_skill + offhand_skill ) / 5;
        }
		else
		{
			chance += prim_skill / 3;
			chance += parry_mod( prim );
			chance += dex_mod;
		}

	}
	else if ( offhand )
    {
        offhand_skill = get_weapon_skill( victim, get_weapon_sn( victim, FALSE ) );
        max_dam += ( dice( offhand->value[ 1 ], offhand->value[ 2 ] ) + offhand->value[ 6 ] ) / 2;
        max_dam += GET_SKILL_DAMROLL( victim, offhand )/2;
        chance += offhand_skill / 5;
        chance += parry_mod( offhand );
        chance += dex_mod;
    }

	max_dam += URANGE( 0, 2*(get_curr_stat_deprecated(victim,STAT_DEX)-17)/3,7);

	if ( chance > number_range( 50, 70 ) && victim->level >= ch->level )
    {
		chance -= ( victim->level + 3 - ch->level )*2;
    }

	//i tutaj od rozmiarow - nie sparujesz trzepniecia lapy smoka, ktora jest od ciebie dwa razy wieksza
	if ( victim->size + 2 < ch->size )
    {
		return FALSE;
    }
	else if ( victim->size + 1 < ch->size )
	{
		if( victim->size == SIZE_SMALL )
        {
			chance = URANGE( 3, 3*chance/4, 75 );
        }
		else
        {
			chance = URANGE( 3, chance/2, 45 );
        }
	}

	chance = URANGE(3,chance,95);

	//tutaj trick weapon wrench osoby ktora jest parowana, niezaleznie od tego czy parry wejdzie czy nie
	//jak trik wejdzie to parry nie wejdzie
	//szansa na wejscie trika 10 razy wieksza niz przy zwyklym ciosie
	if( parowana &&	parowana->item_type == ITEM_WEAPON && parowana->value[0] == WEAPON_WHIP )
	{
		//nawet jak trik nie wejdzie, to bicz naprawde w cholere trudno sparowac
		chance /= 4;

		if( parowana == get_eq_char(victim,WEAR_SECOND) )
			SET_BIT( victim->fight_data, FIGHT_TRICK_WIELDSECOND );

		for( i = 0; i < 10; ++i )//10 razy wieksza szansa, czyli dziesiec razy testuje trika, czyli 25%
		{
			if( !is_affected(victim, gsn_weapon_wrench) && !is_affected(ch, gsn_weapon_wrench) && check_trick( victim, ch, SN_TRICK_WEAPON_WRENCH ) )
			{
				if( IS_SET( victim->fight_data, FIGHT_TRICK_WIELDSECOND ) )
					REMOVE_BIT( victim->fight_data, FIGHT_TRICK_WIELDSECOND );
				return FALSE;
			}
		}

		if( IS_SET( victim->fight_data, FIGHT_TRICK_WIELDSECOND ) )
			REMOVE_BIT( victim->fight_data, FIGHT_TRICK_WIELDSECOND );
	}

#ifdef INFO
	print_char( victim, "Max_dam = %d\n\r", max_dam );
	print_char( victim, "Chance to parry = %d\n\r", chance );
#endif

	if ( max_dam < 3 * dam / 4 )
	{
		return FALSE;
	}

	percent = number_percent();
	if ( percent > chance )
	{
		return FALSE;
	}

	// teraz przeliczamy to i normalizujemy, tak �eby widzie� "si��" parowania
	// i na tej podstawie obliczymy jak "skuteczne" by�o parowanie, dzi�ki
	// czemu zr�znicujemy komunikaty na "s�abe", "normalne" i "silne" parowanie.

	// najpierw normalizacja wyniku w zakresie 0-100, opieramy si� na tym, �e
	// chance >= percent (wynika to z warunku, kt�ry zwraca false), nadpisujemy
	// zmienn� chance, poniewa� nie b�dzie ona ju� w tej funkcji potrzebna

	chance = (percent * 100) / chance;

	// nast�pnie wk�adamy warunki i opisy

	if (chance > 21 ) // tutaj to co jest "mocarnym sparowaniem")
	{
		switch(number_range(0,4))
		{
			case 1:
				act( "{5Bez �adnego problemu parujesz atak $z.{x", ch, NULL, victim, TO_VICT );
				act( "{5$N bez �adnego problemu paruje tw�j atak.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$N bez �adnego problemu paruje atak $z.{x", ch, NULL, victim, TO_NOTVICT );
				break;
			case 2:
				act( "{5Bez wysi�ku parujesz atak $z.{x", ch, NULL, victim, TO_VICT );
				act( "{5$N bez wysi�ku paruje tw�j atak.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$N bez wysi�ku paruje atak $z.{x", ch, NULL, victim, TO_NOTVICT );
				break;
			case 3:
				act( "{5Bez k�opotu parujesz atak $z.{x", ch, NULL, victim, TO_VICT );
				act( "{5$N bez k�opotu paruje tw�j atak.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$N bez k�opotu paruje atak $z.{x", ch, NULL, victim, TO_NOTVICT );
				break;
			case 4:
				act( "{5Z gracj� parujesz atak $z.{x", ch, NULL, victim, TO_VICT );
				act( "{5$N z gracj� paruje tw�j atak.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$N z gracj� paruje atak $z.{x", ch, NULL, victim, TO_NOTVICT );
				break;
			default:
				act( "{5Z gracj� parujesz atak $z.{x", ch, NULL, victim, TO_VICT );
				act( "{5$N z gracj� paruje tw�j atak.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$N z gracj� paruje atak $z.{x", ch, NULL, victim, TO_NOTVICT );
		}
	}
	else if (chance < 76) // sparowanie s�abe
	{
		switch(number_range(0,5))
		{
			case 1:
				act( "{5Prawie w ostatniej chwili parujesz atak $z.{x", ch, NULL, victim, TO_VICT );
				act( "{5Prawie w ostatniej chwili $N paruje tw�j atak.{x", ch, NULL, victim, TO_CHAR );
				act( "{5Prawie w ostatniej chwili $N paruje atak $z.{x", ch, NULL, victim, TO_NOTVICT );
				break;
			case 2:
				act( "{5Nadzwyczajnym wysi�kiem parujesz atak $z.{x", ch, NULL, victim, TO_VICT );
				act( "{5$N nadzwyczajnym wysi�kiem paruje tw�j atak.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$N nadzwyczajnym wysi�kiem paruje atak $z.{x", ch, NULL, victim, TO_NOTVICT );
				break;
			case 3:
				act( "{5Ledwie udaje ci si� sparowa� atak $z.{x", ch, NULL, victim, TO_VICT );
				act( "{5$N ledwo paruje tw�j atak.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$N ledwo paruje atak $z.{x", ch, NULL, victim, TO_NOTVICT );
				break;
			case 4:
				act( "{5Przypadkowo parujesz atak $z.{x", ch, NULL, victim, TO_VICT );
				act( "{5$N przypadkowo paruje tw�j atak.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$N przypadkowo paruje atak $z.{x", ch, NULL, victim, TO_NOTVICT );
				break;
			case 5:
				act( "{5Z trudem parujesz atak $z.{x", ch, NULL, victim, TO_VICT );
				act( "{5$N z trudem paruje tw�j atak.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$N z trudem paruje atak $z.{x", ch, NULL, victim, TO_NOTVICT );
				break;
			default:
				act( "{5Z trudem parujesz atak $z.{x", ch, NULL, victim, TO_VICT );
				act( "{5$N z trudem paruje tw�j atak.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$N z trudem paruje atak $z.{x", ch, NULL, victim, TO_NOTVICT );
				break;
		}
	}
	else // sparowanie normalne
	{
		switch(number_range(0,4))
		{
			case 1:
				act( "{5Zwinnie parujesz atak $z.{x", ch, NULL, victim, TO_VICT );
				act( "{5$N zwinnie paruje tw�j atak.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$N zwinnie paruje atak $z.{x", ch, NULL, victim, TO_NOTVICT );
				break;
			case 2:
				act( "{5Bez problemu parujesz atak $z.{x", ch, NULL, victim, TO_VICT );
				act( "{5$N bez problemu paruje tw�j atak.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$N bez problemu paruje atak $z.{x", ch, NULL, victim, TO_NOTVICT );
				break;
			case 3:
				act( "{5Zr�cznie parujesz atak $z.{x", ch, NULL, victim, TO_VICT );
				act( "{5$N zr�cznie paruje tw�j atak.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$N zr�cznie paruje atak $z.{x", ch, NULL, victim, TO_NOTVICT );
				break;
			case 4:
				act( "{5P�ynnie parujesz atak $z.{x", ch, NULL, victim, TO_VICT );
				act( "{5$N p�ynnie paruje tw�j atak.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$N p�ynnie paruje atak $z.{x", ch, NULL, victim, TO_NOTVICT );
				break;
			default:
				act( "{5Zr�cznie parujesz atak $z.{x", ch, NULL, victim, TO_VICT );
				act( "{5$N z zaskakuj�c� zr�czno�ci� paruje tw�j atak.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$N z zaskakuj�c� zr�czno�ci� paruje atak $z.{x", ch, NULL, victim, TO_NOTVICT );
		}
	}
    check_trick( victim, ch, SN_TRICK_RIPOSTE );
	return TRUE;
}

/*
 * Set position of a victim.
 */
void update_pos( CHAR_DATA *victim )
{
    if (victim->position == POS_FIGHTING)
    {
        if (victim->fighting == NULL || victim->in_room != victim->fighting->in_room )
        {
            victim->position = POS_STANDING;
        }
    }

	if ( victim->hit > 0 )
	{
		if ( victim->position <= POS_STUNNED )
		{

			victim->position = POS_SITTING;
			//do_function(victim, &do_rest, "");
			return;
		}

		return;
	}

	if ( victim->hit <= -11 )
	{
		victim->position = POS_DEAD;
		return;
	}

	if ( victim->hit <= -6 ) victim->position = POS_MORTAL;
	else if ( victim->hit <= -3 ) victim->position = POS_INCAP;
	else victim->position = POS_STUNNED;

	return;
}

/*
 * Start fights.
 */
void set_fighting( CHAR_DATA *ch, CHAR_DATA *victim )
{
	if ( !IS_NPC( ch ) && ch->pcdata->mind_in )
	{
		send_to_char( "Czujesz, �e twoje cia�o znajduje si� w niebezpiecze�stwie. ", ch );
		mind_move_room( ch, NULL );
	}

	if ( ch->fighting != NULL )
	{
		return;
	}

	if ( ( IS_NPC( ch ) && ch->pIndexData->vnum == 3 ) || ( IS_NPC( victim ) && victim->pIndexData->vnum == 3 ) )
	{
		return;
	}

	if ( IS_AFFECTED( ch, AFF_SLEEP ) )
	{
		affect_strip( ch, gsn_sleep );
	}

	if ( IS_AFFECTED( ch, AFF_RECUPERATE ) )
	{
		affect_strip( ch, gsn_recuperate );
	}

	if( IS_AFFECTED(victim, AFF_MEDITATION) )
	{
		affect_strip( ch, gsn_meditation );
	}

	if (  can_move( ch ) )
	{
		if ( ch->position == POS_SITTING || ch->position == POS_RESTING )
			act( "$n wstaje.", ch, NULL, NULL, TO_ROOM );
		ch->position = POS_FIGHTING;
	}
	else if ( ch->position == POS_SLEEPING )
	{
		act( "$n budzi si� i siada.", ch, NULL, NULL, TO_ROOM );
		act_new( "$N brutalnie przerywa tw�j sen.", ch, NULL, victim, TO_CHAR, POS_SLEEPING );
		ch->position = POS_SITTING;
	}

	ch->fighting = victim;

	return;
}

/*
 * Stop fights.
 */
void stop_fighting( CHAR_DATA *ch, bool fBoth )
{
	CHAR_DATA * fch;

	/* heh, kurwa nie wiem co tu si� dzieje...
	 * jesli jest fBoth = FALSE to po chuj przegladac wszystkie
	 * postacie na liscie...wystarczy tego kolesia z argumentu
	 * wykorzystac...jakis kretyn to pisal
	 */
	ch->fighting = NULL;

	if ( ch->position >= POS_RESTING )
	{
		if ( ch->position == POS_SITTING ||  ch->position == POS_RESTING )
	if(IS_NPC(ch))	act( "$n wstaje.", ch, NULL, NULL, TO_ROOM );//Raszer, tutaj tez powinien byc warunek, 31.05.2009
				if(IS_NPC(ch)) ch->position = ch->default_pos; //Raszer, wylaczenie automatycznego wstawania po walce
    //		act( "$n wstaje.", ch, NULL, NULL, TO_ROOM );
	//	ch->position	= IS_NPC( ch ) ? ch->default_pos : POS_STANDING;
	}


	update_pos( ch );

	if ( !fBoth )
		return;

	for ( fch = char_list; fch != NULL; fch = fch->next )
		if ( fch->fighting == ch )
		{
			fch->fighting	= NULL;

			if ( fch->position >= POS_RESTING )
			{
				if ( fch->position == POS_SITTING ||  fch->position == POS_RESTING )
				if(IS_NPC(fch))	act( "$n wstaje.", fch, NULL, NULL, TO_ROOM );//Raszer, tutaj tez powinien byc warunek, 31.05.2009
				if(IS_NPC(fch)) fch->position = fch->default_pos; //Raszer, wylaczenie automatycznego wstawania po walce
				fch->fighting = NULL;
			}

			update_pos( fch );
		}
	return;
}

/*
 * Make a corpse out of a character.
 */
void make_corpse( CHAR_DATA *ch )
{
    char buf[ MAX_STRING_LENGTH ];
    OBJ_DATA *corpse = NULL;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    OBJ_DATA *random_obj = NULL;
    char *name;
    int corpse_vnum;
    bool custom_body = FALSE;
    int i;
    OBJ_DATA *random_obj_spellbook;

    if ( ch->ss_data )
    {
        SHAPESHIFT_DATA* ss;
        ss = do_reform_silent ( ch );
        free_shape_data( ss );
        ch->ss_data = NULL;
    }

	if ( IS_NPC( ch ) )
    {
		name = ch->name2;
		corpse_vnum = ch->pIndexData->corpse_vnum;

		if ( EXT_IS_SET( ch->act, ACT_RAND_ITEMS ) && number_percent() < ch->level / 2 )
        {
			random_obj = randomize_item( UMIN( ch->level / 5, 7 ) );
        }

		if ( corpse_vnum > 0 && get_obj_index( corpse_vnum ) )
		{
			corpse = create_object( get_obj_index( corpse_vnum ), FALSE );
			custom_body = TRUE;
		}
		else if ( corpse_vnum == 0 )
        {
			corpse = create_object( get_obj_index( OBJ_VNUM_CORPSE_NPC ), FALSE );
        }
		else
        {
			corpse = NULL;
        }

        if ( corpse )
        {
            corpse->cost  = 0;
            corpse->timer = number_range( 4, 8 );
            /**
             * slow rot
             */
            if ( IS_SET( corpse->value[ 5 ], SLOW_ROT ) )
            {
                corpse->timer *= number_range( 10, 50 );
                corpse->timer += number_percent();
            }
            /**
             * no rot
             */
            if ( IS_SET( corpse->value[ 5 ], NO_ROT ) )
            {
                corpse->timer = 0;
            }
            /**
             * ustaw wage, jezeli nie jest ona zablokowana przez ustawienia
             * uwaga na kompatybilnosc wagi
             */
            if ( !IS_SET( corpse->value[ 5 ], STATIC_WEIGHT ) )
            {
                corpse->weight = 2*ch->weight;
            }

            /* do robienia szkieletkow i innego szajsu*/
            if ( corpse->item_type == ITEM_CORPSE_NPC && !EXT_IS_SET( ch->act, ACT_RAISED ))
            {
                corpse->value[ 0 ] = ch->pIndexData->vnum;
            }
            corpse->value[ 2 ] = ch->pIndexData->level;
        }

        /*
         * dorzucanie randomowych ksiag
         */
        i = UMAX( 1, ch->level/12 );
        while ( i > 0 )
        {
            random_obj_spellbook = generate_random_spellbook ( ch, i-- );
            if ( random_obj_spellbook == NULL )
            {
                continue;
            }
            if ( corpse )
            {
                switch ( corpse->item_type )
                {
                    case ITEM_CORPSE_NPC:
                    case ITEM_CORPSE_PC:
                    case ITEM_CONTAINER:
                        obj_to_obj( random_obj_spellbook, corpse );
                        break;
                    default:
                        obj_to_room( random_obj_spellbook, ch->in_room );
                        break;
                }
            }
            else
            {
                obj_to_room( random_obj_spellbook, ch->in_room );
            }
        }

        /**
         * przeniesienie kasy
         */
        long int copper = money_count_copper_all( ch );
        if ( copper > 0 )
        {
            if ( corpse )
            {
                switch ( corpse->item_type )
                {
                    case ITEM_CORPSE_NPC:
                    case ITEM_CORPSE_PC:
                    case ITEM_CONTAINER:
                        obj_to_obj( create_money( ch->copper, ch->silver, ch->gold, ch->mithril ), corpse );
                        break;
                    default:
                        obj_to_room( create_money( ch->copper, ch->silver, ch->gold, ch->mithril ), ch->in_room );
                        break;
                }
                /*
                 * dorzucanie przedmiotow randomowych celowo zapiete tylko dla mobow ze srebrem
                 */
                if ( random_obj )
                    switch ( corpse->item_type )
                    {
                        case ITEM_CORPSE_NPC:
                        case ITEM_CORPSE_PC:
                        case ITEM_CONTAINER:
                            obj_to_obj( random_obj, corpse );
                            break;
                        default:
                            obj_to_room( random_obj, ch->in_room );
                            break;
                    }
            }
            else
            {
                obj_to_room( create_money( ch->copper, ch->silver, ch->gold, ch->mithril ), ch->in_room );
                if ( random_obj )
                {
                    obj_to_room( random_obj, ch->in_room );
                }
            }
            money_reset_character_money( ch );
        }
    }
	else
    {
        /*artefact*/
        //all_artefact_from_char( ch );

        name           = ch->name2;
        corpse         = create_object( get_obj_index( OBJ_VNUM_CORPSE_PC ), FALSE );
        corpse->weight = 2 * ch->weight;
        corpse->timer  = number_range( 40, 65 );
        corpse->owner  = NULL;

        //rellik: na potrzeby wycinania ucha tutaj przechowuj� imi� w�a�ciciela cia�a i ras�
        corpse->hidden_description = strdup(ch->name2);

        corpse->value[ 0 ] = ch->level;
        corpse->value[ 1 ] = 0;
        corpse->value[ 2 ] = 0;
        corpse->value[ 3 ] = 0;
        corpse->value[ 4 ] = 0;
        corpse->value[ 5 ] = 0;
        corpse->value[ 6 ] = GET_RACE( ch );

        corpse->cost = 0;

        long int copper = money_count_copper_all( ch );
        if ( copper > 1 )
        {
            obj_to_obj( create_money( ch->copper, ch->silver, ch->gold, ch->mithril ), corpse );
            append_file_format_daily
                (
                 ch,
                 MONEY_LOG_FILE,
                 "-> S: %ld %ld (%ld) /%d/%d/%d/%d/, B: %ld %ld (%ld) - �mier�",
                 copper,
                 0,
                 -copper,
                 ch->copper,
                 ch->silver,
                 ch->gold,
                 ch->mithril,
                 money_count_copper_all( ch ),
                 money_count_copper_all( ch ),
                 0
                );
            money_reset_character_money ( ch );
        }
    }

	if ( corpse && !custom_body )
	{
		sprintf( buf, corpse->short_descr, name );
		free_string( corpse->short_descr );
		corpse->short_descr = str_dup( buf );

		sprintf( buf, corpse->description, name );
		free_string( corpse->description );
		corpse->description = str_dup( buf );
	}

	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	{
		bool floating = FALSE;

		OBJ_NEXT_CONTENT( obj, obj_next );

		if ( obj->wear_loc == WEAR_FLOAT )
			floating = TRUE;

		/*artefact*/
		if ( is_artefact( obj ) && !IS_NPC( ch ) && !IS_IMMORTAL( ch ) )
			artefact_from_char( obj, ch );
		obj_from_char( obj );

		if ( obj->item_type == ITEM_POTION )
			obj->timer = number_range( 500, 1000 );

		if ( obj->item_type == ITEM_SCROLL )
			obj->timer = number_range( 1000, 2500 );

		if ( IS_OBJ_STAT( obj, ITEM_ROT_DEATH ) )
		{
			if ( obj->contains != NULL )
			{
				OBJ_DATA * in, *in_next;

				act( "$p wyparowuje, pozostawiaj�c swoj� zawarto�� wsz�dzie dooko�a.",
						ch, obj, NULL, TO_ROOM );

				for ( in = obj->contains; in != NULL; in = in_next )
				{
					in_next = in->next_content;
					obj_from_obj( in );
					obj_to_room( in, ch->in_room );
				}
			}
			else
				act( "$p wyparowuje.", ch, obj, NULL, TO_ROOM );

			/*artefact*/
			if ( is_artefact( obj ) ) extract_artefact( obj );
			if ( obj->contains ) extract_artefact_container( obj );
			extract_obj( obj );
			continue;
		}

		EXT_REMOVE_BIT( obj->extra_flags, ITEM_VIS_DEATH );

		/* tylko mobiki */
		if ( IS_NPC( ch ) && obj->item_type == ITEM_SPELLBOOK )
		{
			int x, tmp;
			bool out = FALSE;

			for ( tmp = 2; tmp < 7; tmp++ )
				if ( obj->value[ 2 ] != 0 )
				{
					out = TRUE;
					break;
				}

			if ( !out )
				for ( tmp = 2, x = 0; x < 16; x++ )
				{
					if ( ch->pIndexData->spells[ x ] == 0 )
						break;

					if ( number_percent() < 50 )
					{
						obj->value[ tmp ] = ch->pIndexData->spells[ x ];
						tmp++;

						if ( tmp > 6 )
							break;
					}
				}
		}
		/* Ciala wypadaja na ziemie z pojemnikow... */
		if ( obj->item_type == ITEM_CONTAINER || obj->item_type == ITEM_CORPSE_PC || obj->item_type == ITEM_CORPSE_NPC )
		{
			if ( obj->contains != NULL )
			{
				OBJ_DATA * in, *in_next;
				for ( in = obj->contains; in != NULL; in = in_next )
				{
					in_next = in->next_content;
					if ( in->item_type == ITEM_CORPSE_PC || in->item_type == ITEM_CORPSE_NPC )
					{
						obj_from_obj( in );
						obj_to_room( in, ch->in_room );
						act( "$p upada na ziemi�.", ch, in, NULL, TO_ROOM );
					}
				}
			}
		}

		if ( IS_OBJ_STAT( obj, ITEM_INVENTORY ) )
		{
			/*artefact*/
			if ( is_artefact( obj ) ) extract_artefact( obj );
			if ( obj->contains ) extract_artefact_container( obj );
			extract_obj( obj );
		}
		else if ( floating )
		{
			obj_to_room( obj, ch->in_room );
			act( "$p upada na ziemi�.", ch, obj, NULL, TO_ROOM );
		}
		else if ( obj->item_type == ITEM_CORPSE_PC || obj->item_type == ITEM_CORPSE_NPC )
		{
			obj_to_room( obj, ch->in_room );
			act( "$p upada na ziemi�.", ch, obj, NULL, TO_ROOM );
		}
		else
		{
			if ( !corpse )
				obj_to_room( obj, ch->in_room );
			else
			{
				switch ( corpse->item_type )
				{
					case ITEM_CORPSE_NPC:
					case ITEM_CORPSE_PC:
					case ITEM_CONTAINER:
						obj_to_obj( obj, corpse );
						break;
					default:
						obj_to_room( obj, ch->in_room );
						break;
				}
			}
		}
	}


	if ( corpse )
		obj_to_room( corpse, ch->in_room );

	return;
}

/*
 * Improved Death_cry contributed by Diavolo.
 */
void death_cry( CHAR_DATA *ch )
{
	ROOM_INDEX_DATA * was_in_room;
	char *msg;
	int door;

	if ( IS_NPC( ch ) )
		msg = "Powietrze przecina rozdzieraj�cy krzyk, chyba ktos w�a�nie umar�...";
	else
		msg = "Ryk b�lu prawie rani twoje uszy, zdaje si�, �e kto� lub co� ponios�o smier�...";


	was_in_room = ch->in_room;
	for ( door = 0; door <= 5; door++ )
	{
		EXIT_DATA *pexit;

		if ( ( pexit = was_in_room->exit[ door ] ) != NULL
				&& pexit->u1.to_room != NULL
				&& pexit->u1.to_room != was_in_room )
		{
			ch->in_room = pexit->u1.to_room;
			act( msg, ch, NULL, NULL, TO_ROOM );
		}
	}
	ch->in_room = was_in_room;
	return;
}

void skill_loss( CHAR_DATA *ch, int handicap )
{
	int sn;
	int i;
	int wis_or_int = UMAX( get_curr_stat_deprecated( ch, STAT_WIS ), get_curr_stat_deprecated( ch, STAT_INT ) );

	for ( sn = 0; sn < MAX_SKILL; sn++ )
		if ( ( skill_table[sn].name != NULL )
				&&  ( ch->pcdata->learned[sn] > 0 ) && ( ch->pcdata->learning_rasz[sn] > 0 ) )
		{
			for ( i = 0; i < handicap; i++ )
				if ( ( ch->pcdata->learning_rasz[sn] * handicap ) + number_range( 20, 280 ) > wis_or_int * 10 )
				{
					ch->pcdata->learning_rasz[sn] = UMAX( 0, ch->pcdata->learning_rasz[sn]-1 );
					if ( ch->pcdata->learning_rasz[sn] < 0 )
						log_string( "skill_loss: bug z learning" );
				}
		}
	return;
}
void raw_kill( CHAR_DATA *victim, CHAR_DATA *ch )
{
	int i;
	AFFECT_DATA *aff, *aff_next, noob_new, *noob_old;
	bool has_silver = FALSE;
	OBJ_DATA *head, *head_next;
	char *name;
	char buf[ MAX_STRING_LENGTH ];

    /**
     * logowanie zabitych bossow
     */
    if ( EXT_IS_SET( victim->act, ACT_BOSS ) )
    {
        sprintf( buf, "[%d][boss death] %s (%d) killed by %s (%d) (%d) w %s.",
					victim->in_room ? victim->in_room->vnum : 0,
					victim->name,
					victim->level,
					( IS_NPC( ch ) ? ch->short_descr : ch->name ),
					ch->level,
					( IS_NPC( ch ) ? -1 : ch->class ),
					victim->in_room ? victim->in_room->name : "unknown"
			       );
        log_string( buf );
    }

	if ( victim->copper > 0 || victim->silver > 0 || victim->gold > 0 || victim->mithril > 0 )
    {
		has_silver = TRUE;
    }

	//tutaj robimy flage noob_killer by zliczala ile noobow zabil
	if( !IS_NPC(victim) && !IS_NPC(ch) && victim->level <= LEVEL_NEWBIE && ch->level > victim->level )
	{
		if( is_affected(ch,gsn_noob_killer) && ( noob_old = affect_find( ch->affected, gsn_noob_killer ) ) != NULL )
		{
			++noob_old->modifier;
			noob_old->duration = 10080; //im wi�cej, tym lepiej...tydzien jest ok
			noob_old->rt_duration = 0;
#ifdef NOOB_STAT_REMOVER
			//Rysand - walka z chamstwem - tymczasowe ci�cie stat�w
			if(number_range(0, 100) < (5 * (noob_old->modifier - 3)))
			{
				AFFECT_DATA af;
				af.where = TO_AFFECTS;
				af.type = gsn_none;
				af.level= 50;
				af.duration = 60*24;
				af.rt_duration = 0;
				af.location = number_range(1, 5);
				af.modifier = -noob_old->modifier; //ile noob�w zabi� taki minus do stata
				af.bitvector = &AFF_NONE;
				affect_to_char( ch, &af, NULL, TRUE );

				send_to_char( "Czujesz, �e dosi�gn�� ci� gniew Bog�w!\n\r", ch );
			}
#endif
		}
		else
		{
			noob_new.where = TO_AFFECTS;
			noob_new.type = gsn_noob_killer;
			noob_new.level = 50;
			noob_new.duration = 100;
			noob_new.rt_duration = 0;
			noob_new.location = APPLY_NONE;
			noob_new.modifier = 1;
			noob_new.bitvector = &AFF_NONE;
			affect_to_char( ch, &noob_new, NULL, FALSE );
		}
	}

	BOUNTY_DATA *bounty;
	//tutaj robimy trophy od bounty
	if ( !IS_NPC(ch) && !IS_NPC(victim) && victim->in_room && ( bounty = search_bounty ( victim->name ) ) != NULL )
	{
		name = victim->name2;

		head = create_object( get_obj_index( OBJ_VNUM_SEVERED_HEAD ), FALSE );
		head->timer	= number_range( 40, 70 );

		sprintf( buf, head->short_descr, name );
		free_string( head->short_descr );
		head->short_descr = str_dup( buf );

		sprintf( buf, "g�owy %s", victim->name2 );
		free_string( head->name2 );
		head->name2 = str_dup( buf );

		sprintf( buf, "g�owie %s", victim->name2 );
		free_string( head->name3 );
		head->name3 = str_dup( buf );

		sprintf( buf, "g�ow� %s", victim->name2 );
		free_string( head->name4 );
		head->name4 = str_dup( buf );

		sprintf( buf, "g�ow� %s", victim->name2 );
		free_string( head->name5 );
		head->name5 = str_dup( buf );

		sprintf( buf, "g�owie %s", victim->name2 );
		free_string( head->name6 );
		head->name6 = str_dup( buf );

		sprintf( buf, head->description, name );
		free_string( head->description );
		head->description = str_dup( buf );
		head->item_type = ITEM_TROPHY;
		head->value[0] = bounty->value;
		rem_bounty( &misc.bounty_list, victim->name );
		obj_to_room( head, victim->in_room );

		//i tutaj czyszczonko coby przypadkiem nie zrobilo dwoch glow, ajkby juz jakas byla z make head
		for( head = victim->in_room->contents; head; head = head_next )
		{
			head_next = head->next_content;
			if( head->item_type == ITEM_TRASH && head->pIndexData->vnum == OBJ_VNUM_SEVERED_HEAD && head->value[5] == -17 )
				extract_obj( head );
		}
	}

	DEBUG_INFO( "raw_kill:group_gain" );
	group_gain( ch, victim );
	DEBUG_INFO( "raw_kill:stop_fighting" );
	stop_fighting( victim, TRUE );
	DEBUG_INFO( "raw_kill:death_cry" );
	death_cry( victim );
	DEBUG_INFO( "raw_kill:make_corpse" );
	make_corpse( victim );
	DEBUG_INFO( "raw_kill:clear_mem" );
	clear_mem( victim );
	DEBUG_INFO( "raw_kill:death_stats" );
	update_death_statistic( victim, ch );

	if ( IS_NPC( victim ) && !IS_NPC( ch ) && has_silver )
	{
		if ( EXT_IS_SET( ch->act, PLR_AUTOGOLD ) )
        {
			do_function( ch, &do_get, "monety cia�o" );
        }
	}

	if ( IS_NPC( victim ) )
	{
		DEBUG_INFO( "raw_kill:reorganize_group" );
		reorganize_mobile_group( victim );
		victim->pIndexData->killed++;
		kill_table[ URANGE( 0, victim->level, MAX_LEVEL - 1 ) ].killed++;
		DEBUG_INFO( "raw_kill:extract:pre" );
		extract_char( victim, TRUE );
		DEBUG_INFO( "raw_kill:extract:post" );
		return;
	}

	DEBUG_INFO( "raw_kill:remove_affects" );
	/* zdejmuje wszystkie affecty, oprocz zwiazanych ze smiercia*/
    for ( aff = victim->affected; aff; aff = aff_next )
    {
        aff_next = aff->next;
        if ( check_noremovable_affects( aff ) )
        {
            continue;
        }
        affect_remove( victim, aff );
    }

	DEBUG_INFO( "raw_kill:reseting_char" );
	for ( i = 0;i < MAX_COUNTER;i++ )
		victim->counter[ i ] = 0;

	for ( i = 0;i < MAX_VECT_BANK;i++ )
		victim->affected_by[ i ] = 0;

	for ( i = 0;i < MAX_LANG;i++ )
		victim->pcdata->lang_mod[ i ] = 0;

	for ( i = 0;i < MAX_SKILL;i++ )
		victim->pcdata->learn_mod[ i ] = 0;

	ext_flags_copy( ext_flags_sum( victim->affected_by, ext_flag_value( affect_flags, race_table[ GET_RACE(victim) ].aff ) ), victim->affected_by );

	for ( i = 0; i < 4; i++ )
		victim->armor[ i ] = 100;

	victim->position	= POS_RESTING;
	victim->hit	= UMAX( 1, victim->hit );
	victim->move	= victim->max_move / 2;
	victim->fight_data	= 0;
	check_player_death( victim, ch );
	DEBUG_INFO( "raw_kill:extract_dead_players" );
	extract_dead_player( victim );
	return;
}

void dam_message( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt, bool immune )
{
	char buf1[ 256 ], buf2[ 256 ], buf3[ 256 ];
	const char *vs;
	const char *vp;
	const char *attack;
	char punct;
	int perfect_self;

	dam = calculate_final_damage( ch, victim, dam );

	if ( !IS_NPC(ch) )
	{
		dam *= 3;
		dam /= 2;
 	}

	save_debug_info("fight.c => dam_message", NULL, NULL, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );

	if( IS_AFFECTED(ch,AFF_DEFLECT_WOUNDS)) dam /= 2;

	if ( ch == NULL || victim == NULL )
		return;

	if (is_affected( ch, gsn_perfect_self))
		perfect_self = number_range(5,30);
	else
		perfect_self = 0;

	if ( dt > 0 && dt < MAX_SKILL )
	{
		SPELL_MSG * msg;

		msg = msg_lookup( dt );
		if ( msg )
		{
			if ( immune )
			{
				act( msg->caster[ 4 ], ch, NULL, victim, TO_CHAR );
				act( msg->around[ 4 ], ch, NULL, victim, TO_ROOM );
			}
			else
			{
				if ( ch == victim )
				{
					if ( ch->hit - dam < -11 )
					{
						act_new( msg->caster[ 2 ], ch, NULL, victim, TO_CHAR, POS_DEAD );
						act( msg->around[ 2 ], ch, NULL, victim, TO_ROOM );
					}
					else
					{
						act_new( msg->caster[ 0 ], ch, NULL, victim, TO_CHAR, POS_DEAD );
						act( msg->around[ 0 ], ch, NULL, victim, TO_ROOM );
					}
				}
				else
				{
					if ( victim->hit - dam < -11 )
					{
						act( msg->caster[ 3 ], ch, NULL, victim, TO_CHAR );
						act_new( msg->victim[ 3 ], ch, NULL, victim, TO_VICT, POS_DEAD );
						act( msg->around[ 3 ], ch, NULL, victim, TO_NOTVICT );
					}
					else
					{
						act( msg->caster[ 1 ], ch, NULL, victim, TO_CHAR );
						act_new( msg->victim[ 1 ], ch, NULL, victim, TO_VICT, POS_DEAD );
						act( msg->around[ 1 ], ch, NULL, victim, TO_NOTVICT );
					}
				}
			}
			return;
		}
		else if ( special_damage_message( ch, victim, NULL, dam, dt, immune ) )
		{
			return;
		}
	}

	if ( dam + perfect_self == 0 )
	{
		vs = "chybiasz"; vp = "chybia";
	}
	else if ( dam + perfect_self <= 2 )
	{
		vs = "siniaczysz"; vp = "siniaczy";
	}
	else if ( dam + perfect_self <= 6 )
	{
		vs = "muskasz"; vp = "muska";
	}
	else if ( dam + perfect_self <= 10 )
	{
		vs = "ledwie ranisz"; vp = "ledwie rani";
	}
	else if ( dam + perfect_self <= 14 )
	{
		vs = "lekko ranisz"; vp = "lekko rani";
	}
	else if ( dam + perfect_self <= 18 )
	{
		vs = "ranisz"; vp = "rani";
	}
	else if ( dam + perfect_self <= 22 )
	{
		vs = "mocno ranisz"; vp = "mocno rani";
	}
	else if ( dam + perfect_self <= 26 )
	{
		vs = "dotkliwie ranisz"; vp = "dotkliwie rani";
	}
	else if ( dam + perfect_self <= 30 )
	{
		vs = "powa�nie ranisz"; vp = "powa�nie rani";
	}
	else if ( dam + perfect_self <= 34 )
	{
		vs = "masakrujesz"; vp = "masakruje";
	}
	else if ( dam + perfect_self <= 38 )
	{
		vs = "rozpruwasz"; vp = "rozpruwa";
	}
	else if ( dam + perfect_self <= 44 )
	{
		vs = "dewastujesz"; vp = "dewastuje";
	}
	else if ( dam + perfect_self <= 50 )
	{
		vs = "grzmocisz"; vp = "grzmoci";
	}
	else if ( dam + perfect_self <= 55 )
	{
		vs = "niszczysz"; vp = "niszczy";
	}
	else if ( dam + perfect_self <= 60 )
	{
		vs = "NISZCZYSZ"; vp = "NISZCZY";
	}
	else if ( dam + perfect_self <= 67 )
	{
		vs = "DRUZGOCZESZ"; vp = "DRUZGOCZE";
	}
	else if ( dam + perfect_self <= 75 )
	{
		vs = "ROZPRUWASZ"; vp = "ROZPRUWA";
	}
	else if ( dam + perfect_self <= 84 )
	{
		vs = "ROZRYWASZ"; vp = "ROZRYWA";
	}
	else if ( dam + perfect_self <= 100 )
	{
		vs = "ROZBEBESZASZ"; vp = "ROZBEBESZA";
	}
	else if ( dam + perfect_self <= 115 )
	{
		vs = "DEKAPITUJESZ"; vp = "DEKAPITUJE";
	}
	else if ( dam + perfect_self <= 130 )
	{
		vs = "EKSTYRPUJESZ"; vp = "EKSTYRPUJE";
	}
	else if ( dam + perfect_self <= 145 )
	{
		vs = "ANIHILUJESZ"; vp = "ANIHILUJE";
	}
	else if ( dam + perfect_self <= 200 )
	{
		vs = "U�MIERCASZ"; vp = "U�MIERCA";
	}
	else
	{
		vs = "UNICESTWIASZ"; vp = "UNICESTWIA";
	}

	punct = ( dam <= 24 ) ? '.' : '!';

	if ( dt == TYPE_HIT )
	{
		if ( ch == victim )
		{
			sprintf( buf1, "{3$n{3 %s {3sam siebie%c{x", vp, punct );
			sprintf( buf2, "{2%s sam siebie %c{x", vs, punct );
		}
		else
		{
			sprintf( buf1, "{3$n {3%s $C{3%c{x", vp, punct );
			sprintf( buf2, "{2%s $C{2%c{x", capitalize( vs ), punct );
			sprintf( buf3, "{4$n {4%s cie%c{x", vp, punct );
		}
	}
	else
	{
		if ( dt >= 0 && dt < MAX_SKILL )
			attack	= skill_table[ dt ].noun_damage;
		else if (( dt >= TYPE_HIT
				&& dt < TYPE_HIT + MAX_DAMAGE_MESSAGE ) )
			attack	= attack_table[ dt - TYPE_HIT ].noun;
		else
		{
			bug( "Dam_message: bad dt %d.", dt );
			dt = TYPE_HIT;
			attack = attack_table[ 0 ].name;
		}

		if ( immune )
		{
			if ( ch == victim )
			{
				switch ( ch->sex )
				{
				case 0:
					sprintf( buf1, "{3$n jest odporne na sw�j w�asny czar %s.{x", attack );
					sprintf( buf2, "{2Co za szcz�cie, jeste� na to odporne.{x" );
					break;
				case 1:
					sprintf( buf1, "{3$n jest odporny na sw�j w�asny czar %s.{x", attack );
					sprintf( buf2, "{2Co za szcz�cie, jeste� na to odporny.{x" );
					break;
				default:
					sprintf( buf1, "{3$n jest odporna na sw�j w�asny czar %s.{x", attack );
					sprintf( buf2, "{2Co za szcz�cie, jeste� na to odporna.{x" );
					break;
				}
			}
			else
			{
				sprintf( buf1, "{3%s $z nic nie robi $X!{x", capitalize( attack ) );
				sprintf( buf2, "{2%s nic nie robi $X!{x", capitalize( attack ) );
				sprintf( buf3, "{4%s $z nic ci nie robi.{x", capitalize( attack ) );
			}
		}
		else
		{
			if ( ch == victim )
			{
				sprintf( buf1, "{3%s $z {3%s $m%c{x", capitalize( attack ), vp, punct );
				sprintf( buf2, "{2Twoje %s %s cie%c{x", attack, vp, punct );
			}
			else
			{
				sprintf( buf1, "{3%s $z {3%s $C%c{x", capitalize( attack ), vp, punct );
				sprintf( buf2, "{2Twoje %s %s $C{2%c{x", attack, vp, punct );
				sprintf( buf3, "{4%s $z {4%s cie%c{x", capitalize( attack ), vp, punct );
			}
		}
	}

	if ( ch == victim )
	{
		act( buf1, ch, NULL, NULL, TO_ROOM );
		act( buf2, ch, NULL, NULL, TO_CHAR );
	}
	else
	{
		act( buf1, ch, NULL, victim, TO_NOTVICT );
		act( buf2, ch, NULL, victim, TO_CHAR );
		act( buf3, ch, NULL, victim, TO_VICT );
	}

	return;
}

/*
 * Disarm a creature.
 * Caller must check for successful attack.
 */
void disarm( CHAR_DATA *ch, CHAR_DATA *victim )
{
	OBJ_DATA * obj;
	AFFECT_DATA *paf;

	if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL )
		obj = get_eq_char( victim, WEAR_SECOND );

	if ( obj == NULL ) return;


	if ( IS_OBJ_STAT( obj, ITEM_NOREMOVE ) )
	{
		act( "{5$S bro� nawet nie drgnie!{x", ch, NULL, victim, TO_CHAR );
		act( "{5$n pr�buje ci� rozbroi�, ale twoja bro� nawet nie drgn�a!{x",
				ch, NULL, victim, TO_VICT );
		act( "{5$n pr�buje rozbroi� $C, ale mu si� nie udaje.{x", ch, NULL, victim, TO_NOTVICT );
		return;
	}

	act( "{5$n ROZBRAJA ci�! Twoja bro� upada na ziemi�!{x",
			ch, NULL, victim, TO_VICT );
	char buf[ 256 ];
	sprintf(buf, "{5Zr�cznym ruchem rozbrajasz $C, wytr�caj�c %s z r�k!{x", can_see_obj(ch, obj) ? obj->name4 : "co�");
	act( buf, ch, NULL, victim, TO_CHAR );
	act( "{5$n szybkim i zr�cznym ruchem rozbraja $C!{x", ch, NULL, victim, TO_NOTVICT );
	WAIT_STATE( victim, PULSE_VIOLENCE );

	/*artefact*/
	if ( is_artefact( obj ) && !IS_NPC( victim ) && !IS_IMMORTAL( victim ) )
		artefact_from_char( obj, victim );
	obj_from_char( obj );
	if ( IS_OBJ_STAT( obj, ITEM_NODROP ) || IS_OBJ_STAT( obj, ITEM_INVENTORY ) )
	{
		/*artefact*/
		if ( is_artefact( obj ) && !IS_NPC( victim ) && !IS_IMMORTAL( victim ) )
			artefact_to_char( obj, victim );
		obj_to_char( obj, victim );
	}
	else
	{
		obj_to_room( obj, victim->in_room );
		if ( IS_NPC( victim ) && victim->wait == 0 && can_see_obj( victim, obj ) )
			get_obj( victim, obj, NULL );
	}

	if( (paf = affect_find(ch->affected,gsn_weapon_wrench) ) )
	{
		affect_remove(ch,paf);
	}

	return;
}

void do_kill( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	CHAR_DATA *victim;

	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Kogo zabi�?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{
		send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
#ifdef ENABLE_WAIT_AFTER_MISS
		WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
#endif
		return;
	}

	if ( victim == ch )
	{
		send_to_char( "Targasz si� na swoje �ycie!\n\r", ch );
		multi_hit( ch, ch, TYPE_UNDEFINED );
		return;
	}

	if ( is_safe( ch, victim ) )
		return;

	if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master == victim )
	{
		act( "$N jest twoim ukochanym mistrzem.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( ch->position == POS_FIGHTING )
	{
		send_to_char( "Starasz si� jak tylko mo�esz!\n\r", ch );
		return;
	}
	WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
	check_killer( ch, victim );
	multi_hit( ch, victim, TYPE_UNDEFINED );
	return;
}

void do_flee( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA * was_in;
    ROOM_INDEX_DATA *now_in;
    CHAR_DATA *victim;
    int attempt;

    if ( ( victim = ch->fighting ) == NULL )
    {
        if ( ch->position == POS_FIGHTING )
        {
            ch->position = POS_STANDING;
        }
        send_to_char( "Z nikim nie walczysz.\n\r", ch );
        return;
    }

    if ( ch->position < POS_FIGHTING )
    {
        send_to_char( "Najpierw wsta�!\n\r", ch );
        return;
    }

    if ( IS_AFFECTED( ch, AFF_PARALYZE ) )
    {
        send_to_char( "Nie mo�esz si� ruszy�.\n\r", ch );
        return;
    }

    if ( IS_AFFECTED( ch, AFF_BERSERK ) )
    {
        send_to_char( "Lepiej skup si� na walce.\n\r", ch );
        return;
    }

    if( IS_AFFECTED(ch, AFF_ENTANGLE ) )
    {
        send_to_char( "Ro�liny opl�tuj� twe nogi, nie mo�esz si� ruszy�!\n\r", ch );
        return;
    }

    if( IS_AFFECTED(ch, AFF_WEB ) )
    {
        send_to_char( "Magiczna paj�czyna sklei�a twe nogi, nie mo�esz si� ruszy�!\n\r", ch );
        return;
    }

    if ( IS_AFFECTED( ch, AFF_HALLUCINATIONS_POSITIVE ) && number_range(1,8) != 1 )
    {
        switch(number_range(1,7))
        {
            case 1:
                send_to_char( "Ucieka�? ale po co? kiedy� i tak zginiesz... To wszystko nie ma sensu...\n\r", ch );
                break;
            case 2:
                send_to_char( "�ciany... dooko�a ciebie �ciany... Coraz bli�ej... Zaraz ci� zgniot�.. Nie masz gdzie uciec!\n\r", ch );
                break;
            case 3:
                send_to_char( "Wielki jak stodo�a ogr zagradza ci drog� machaj�c ogromn� maczug�!\n\r", ch );
                break;
            case 4:
                send_to_char( "Zap�akany duch twej mamusi ukazuje ci si� i prosi, by� do niego do��czy<&�/�a/�o>, bo jemu tak smutno... B�dziesz walczy� do ko�ca!\n\r", ch );
                break;
            case 5:
                send_to_char( "Ogarnia ci� taki �al, �e nagle odechciewa ci si� ucieka�.\n\r", ch );
                break;
            case 6:
                send_to_char( "�wiat jest tak niemo�ebnie okr�tny, �e nie ma dok�d i po co ucieka�.\n\r", ch );
                break;
            case 7:
                send_to_char( "Muzyka stali przera�a ci� do szpiku ko�ci i nie dasz rady teraz uciec.\n\r", ch );
                break;
        }
        return;
    }

    if ( IS_AFFECTED( ch, AFF_HALLUCINATIONS_NEGATIVE ) && number_range(1,8) != 1 )
    {
        switch(number_range(1,7))
        {
            case 1:
                send_to_char( "Ale tu jest tak fajnie.. Nie masz ochoty nigdzie i��.\n\r", ch );
                break;
            case 2:
                send_to_char( "Ju� masz zamiar uciec, ale zauwa�asz �licznego motylka i zaczynasz za nim biec.\n\r", ch );
                break;
            case 3:
                send_to_char( "Jeste� zbyt zaj�t<&y/a/e> zbieraniem kwiatk�w.\n\r", ch );
                break;
            case 4:
                send_to_char( "Czemu mamy przerywa� zabaw�? Ten du�y pan jest taki milutki...\n\r", ch );
                break;
            case 5:
                send_to_char( "Doskonale si� bawisz, wi�c wcale tego nie chcesz.\n\r", ch );
                break;
            case 6:
                send_to_char( "Przecie� jest tutaj tak fajnie, dok�d i po co ucieka�?\n\r", ch );
                break;
            case 7:
                send_to_char( "Muzyka stali wci�ga ci� doszcz�tnie i nie chcesz sobie st�d i��.\n\r", ch );
                break;
        }
        return;
    }

    // Gurthg: 2005-09-07, Nico, wielki czarnych twierdzi, �e to pomo�e, bo torment, to wielka dopa�a
    // Kainti 2005 09 08 - To mialo blokowac flee tylko z walki, w ktorej zostal uzyty, a nie tak, ze gwiazka
    // przy tormencie blokuje flee wogole. Poprawiam.
    //	if ( is_affected(ch,gsn_torment ))
    if ( IS_SET( ch->fight_data, FIGHT_TORMENT_NOFLEE ) )
    {
        send_to_char( "Nie chcesz ucieka�, chcesz zabija� ...\n\r", ch );
        return;
    }

    //Drake: Szama�ski czar 'Ancestors Fury' blokuje uciekanie z walki przez ca�y czas swojego trwania. =D
    if ( IS_AFFECTED( ch, AFF_ANCESTORS_FURY) )
    {
        send_to_char( "Furia przodk�w wype�niaj�ca twoje �y�y domaga si� krwi!\n\r", ch );
        return;
    }

    was_in = ch->in_room;
    for ( attempt = 0; attempt < 6; attempt++ )
    {
        EXIT_DATA *pexit;
        int door;

        door = number_door( );
        if ( ( pexit = was_in->exit[ door ] ) == 0
                || pexit->u1.to_room == NULL
                || IS_SET( pexit->exit_info, EX_CLOSED )
                || IS_SET( pexit->exit_info, EX_HIDDEN )
                || IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_UNDERWATER )
                || IS_SET( pexit->exit_info, EX_SECRET )
                || number_range( 0, ch->daze ) != 0
                || ( IS_AFFECTED(ch, AFF_LOOP) && number_range( 0, 12 ) != 0 )
                || IS_SET( pexit->exit_info, EX_NO_FLEE )
                || ( IS_NPC( ch )
                    && ( EXT_IS_SET( pexit->u1.to_room->room_flags, ROOM_NO_MOB )
                        || IS_SET( pexit->exit_info, EX_NO_MOB ) ) ) )
        {
            continue;
        }

        if ( is_affected(ch,gsn_glorious_impale) && get_eq_char(ch,WEAR_WIELD) && get_eq_char(ch,WEAR_WIELD)->value[0] == WEAPON_SPEAR )
        {
            affect_strip( ch, gsn_glorious_impale );
            if ( ch->fighting && IS_AFFECTED( ch->fighting, AFF_PARALYZE) && is_affected(ch->fighting, gsn_glorious_impale ) )
            {
                act( "Gwa�townie wyrywasz $h z cia�a $Z powi�kszaj�c jeszcze ran�.", ch, get_eq_char( ch,WEAR_WIELD), ch->fighting, TO_CHAR );
                act( "$n gwa�townie wyrywa $h z twojego cia�a powi�kszaj�c jeszcze ran�.", ch, get_eq_char( ch,WEAR_WIELD), ch->fighting, TO_VICT );
                act( "$n gwa�townie wyrywa $h z cia�a $Z powi�kszaj�c jeszcze ran�.", ch, get_eq_char( ch,WEAR_WIELD), ch->fighting, TO_NOTVICT );
                affect_strip( ch->fighting, gsn_glorious_impale );
                damage( ch, ch->fighting, 2*(dice(get_eq_char(ch,WEAR_WIELD)->value[1],get_eq_char(ch,WEAR_WIELD)->value[2])+get_eq_char(ch,WEAR_WIELD)->value[6]+GET_DAMROLL( ch, get_eq_char(ch,WEAR_WIELD))+GET_SKILL_DAMROLL( ch, get_eq_char(ch,WEAR_WIELD))),gsn_glorious_impale, DAM_PIERCE, FALSE );
            }
        }

        move_char( ch, door, FALSE, NULL );
        if ( ( now_in = ch->in_room ) == was_in )
        {
            continue;
        }

        ch->in_room = was_in;

        switch(number_range(1,11))
        {
            case 1:
                act( "$n wycofuje si� i wieje!", ch, NULL, NULL, TO_ROOM );
                break;
            case 2:
                act( "$n wycofuje si� z tego miejsca!", ch, NULL, NULL, TO_ROOM );
                break;
            case 3:
                act( "$n ucieka st�d!", ch, NULL, NULL, TO_ROOM );
                break;
            case 4:
                act( "$n bierze nogi za pas!", ch, NULL, NULL, TO_ROOM );
                break;
            case 5:
                act( "$n daje dyla!", ch, NULL, NULL, TO_ROOM );
                break;
            case 6:
                act( "$n pierzcha stad!", ch, NULL, NULL, TO_ROOM );
                break;
            case 7:
                act( "$n orientuje si� w sytuacji i czmycha niepostrze�enie!", ch, NULL, NULL, TO_ROOM );
                break;
            case 8:
                act( "$n wycofuje si� i uchodzi!", ch, NULL, NULL, TO_ROOM );
                break;
            case 9:
                act( "$n wycofuje si� i umyka!", ch, NULL, NULL, TO_ROOM );
                break;
            case 10:
                act( "$n po kr�tkiej ocenie sytuacji ulatnia si� st�d!", ch, NULL, NULL, TO_ROOM );
                break;
            case 11:
            default:
                act( "$n wycofuje si� i ucieka!", ch, NULL, NULL, TO_ROOM );
                break;
        }

        ch->in_room = now_in;

        if ( !IS_NPC( ch ) )
        {
            send_to_char( "Uciekasz z walki!\n\r", ch );
            if ( ( ch->class == CLASS_THIEF ) && ( number_percent() < 3 * ( ch->level / 2 ) ) )
            {
                send_to_char( "Wycofujesz si� bezpiecznie.\n\r", ch );
            }
            else
            {
                send_to_char( "Tracisz troszk� punkt�w do�wiadczenia.\n\r", ch );
                gain_exp( ch, - number_range( 5, 20 ), FALSE );
            }
        }

        if ( IS_NPC( ch ) && ch->hunting )
        {
            ch->hunting = NULL;
            start_fearing( ch, ch->fighting );

        }

        stop_fighting( ch, TRUE );
        return;
    }

    send_to_char( "WPADASZ W PANIK�! Nie uda�o ci si� uciec!\n\r", ch );
    return;
}

void do_disarm( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA * victim;
    OBJ_DATA *obj;
    OBJ_DATA *wield, *second;
    int chance, ch_weapon, vict_weapon, ch_vict_weapon, mastery_bonus = 0;

    if ( ( chance = get_skill( ch, gsn_disarm ) ) == 0 )
    {
        send_to_char( "Nie znasz si� na tym.\n\r", ch );
        return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
        send_to_char( "Z nikim nie walczysz.\n\r", ch );
        return;
    }

    /*nie rozbroi sie golymi rekoma*/
    if ( get_eq_char( ch, WEAR_WIELD ) == NULL )
    {
        send_to_char( "Musisz trzyma� bro� je�li chcesz kogo� rozbroi�.\n\r", ch );
        return;
    }

    if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL &&
            ( obj = get_eq_char( victim, WEAR_SECOND ) ) == NULL )
    {
        send_to_char( "Tw�j przeciwnik nie ma �adnej broni.\n\r", ch );
        return;
    }

    if( is_affected(victim,gsn_glorious_impale) && !affect_find(victim->affected,gsn_glorious_impale)->modifier && get_eq_char(victim,WEAR_WIELD) && get_eq_char(victim,WEAR_WIELD)->value[0] == WEAPON_SPEAR )
    {
        send_to_char( "To si� nie uda.\n\r", ch );
        return;
    }

    if ( IS_NPC( victim ) )
        chance /= 2;
    else
        chance /= 3;

    /* find weapon skills */
    ch_weapon = get_weapon_skill( ch, get_weapon_sn( ch, TRUE ) );
    vict_weapon = get_weapon_skill( victim, get_weapon_sn( victim, TRUE ) );
    ch_vict_weapon = get_weapon_skill( ch, get_weapon_sn( victim, TRUE ) );

    chance += ch_weapon/6;

    /*no i powiedzmy jakis plus jesil masz 2  bronie*/
    if ( get_eq_char( ch, WEAR_SECOND ) != NULL )
    {
        chance += 8;
    }

    /*oraz bonusik dla whip'ow*/
    /*i niewielki dla short swordow*/
    wield = get_eq_char( ch, WEAR_WIELD );
    second = get_eq_char( ch, WEAR_SECOND );

    if ( wield->value[ 0 ] == WEAPON_WHIP && second && second->value[ 0 ] == WEAPON_WHIP )
    {
        chance += 15;
    }
    else if ( wield->value[ 0 ] == WEAPON_WHIP )
    {
        chance += 10;
    }
    else if ( second && second->value[ 0 ] == WEAPON_WHIP )
    {
        chance += 5;
    }

    if ( wield->value[ 0 ] == WEAPON_SHORTSWORD && second && second->value[ 0 ] == WEAPON_SHORTSWORD )
    {
        chance += 5;
    }
    else if ( wield->value[ 0 ] == WEAPON_SHORTSWORD )
    {
        chance += 3;
    }
    else if ( second && second->value[ 0 ] == WEAPON_SHORTSWORD )
    {
        chance += 2;
    }

    /* jak disarmujacy umie walczyc bronia przeciwnika, to dobrze :) */
    if( ch_vict_weapon > vict_weapon )
    {
        chance += ( ch_vict_weapon - vict_weapon ) / 4;
    }
    else
    {
        chance -= ( vict_weapon - ch_vict_weapon ) / 4;
    }

    /**
     * bonus od masterki
     */
    switch ( wield->value[ 0 ] )
    {
        case WEAPON_SWORD:
            mastery_bonus = get_skill( ch, gsn_sword_mastery );
            break;
        case WEAPON_DAGGER:
            mastery_bonus = get_skill( ch, gsn_dagger_mastery );
            break;
        case WEAPON_SPEAR:
            mastery_bonus = get_skill( ch, gsn_spear_mastery );
            break;
        case WEAPON_MACE:
            mastery_bonus = get_skill( ch, gsn_mace_mastery );
            break;
        case WEAPON_AXE:
            mastery_bonus = get_skill( ch, gsn_axe_mastery );
            break;
        case WEAPON_FLAIL:
            mastery_bonus = get_skill( ch, gsn_flail_mastery );
            break;
        case WEAPON_WHIP:
            mastery_bonus = get_skill( ch, gsn_whip_mastery );
            break;
        case WEAPON_POLEARM:
            mastery_bonus = get_skill( ch, gsn_polearm_mastery );
            break;
        case WEAPON_STAFF:
            mastery_bonus = get_skill( ch, gsn_staff_mastery );
            break;
        case WEAPON_SHORTSWORD:
            mastery_bonus = get_skill( ch, gsn_shortsword_mastery );
            break;
    }
    if ( mastery_bonus > 0 )
    {
        if ( second && wield->value[ 0 ] == second->value[0] )
        {
            mastery_bonus *= 4;
            mastery_bonus /= 3;
        }
        chance += mastery_bonus/10;
    }

    /* dex disarmujacego vs. strength disarmowanego */
    chance += (get_curr_stat_deprecated( ch, STAT_DEX )-10)*2;
    chance -= (get_curr_stat_deprecated( victim, STAT_STR )-10)*2;

    /* roznica leveli */
    chance += ch->level;
    chance -= victim->level;

    if( IS_WEAPON_STAT( obj, WEAPON_TWO_HANDS ) )
    {
        chance -= chance/4;
    }

    chance = URANGE( 4, chance, 95 );

    if( IS_NPC(victim ) && EXT_IS_SET( victim->off_flags, OFF_DISARMPROOF ) )
    {
        chance = 0;
    }

    /* and now the attack */
    if ( chance && number_percent() < chance )
    {
        WAIT_STATE( ch, skill_table[ gsn_disarm ].beats );
        disarm( ch, victim );
        SET_BIT( ch->fight_data, FIGHT_DISARM );
        check_improve( ch, NULL, gsn_disarm, TRUE, 50 );
    }
    else
    {
        WAIT_STATE( ch, skill_table[ gsn_disarm ].beats );
        act( "{5Nie uda�o ci si� rozbroi� $Z.{x", ch, NULL, victim, TO_CHAR );
        act( "{5$n pr�buje ci� rozbroi�, bez skutku.{x", ch, NULL, victim, TO_VICT );
        switch ( ch->sex )
        {
            case 0:
                act( "{5$n pr�buje rozbroi� $C, ale temu si� nie udaje.{x", ch, NULL, victim, TO_NOTVICT );
                break;
            case 1:
                act( "{5$n pr�buje rozbroi� $C, ale mu si� nie udaje.{x", ch, NULL, victim, TO_NOTVICT );
                break;
            default :
                act( "{5$n pr�buje rozbroi� $C, ale jej si� nie udaje.{x", ch, NULL, victim, TO_NOTVICT );
                break;
        }
        check_improve( ch, NULL, gsn_disarm, FALSE, 55 );
    }
    check_killer( ch, victim );
    return;
}

void do_surrender( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA * mob;

	if ( ( mob = ch->fighting ) == NULL )
	{
		send_to_char( "Z nikim nie walczysz!\n\r", ch );
		return;
	}


	act( "Rzucasz si� na kolana i b�agasz $C o lito��!", ch, NULL, mob, TO_CHAR );
	WAIT_STATE( ch, 14 );
	if ( IS_AFFECTED( ch, AFF_SILENCE ) )
	{
		act( "$n pada na kolana w pokornym ge�cie poddania!", ch, NULL, mob, TO_VICT );
		act( "$n rzuca si� na kolana przed $V w pokornym ge�cie poddania!", ch, NULL, mob, TO_NOTVICT );
	}
	else
	{
		act( "$n pada na kolana i b�aga o lito��!", ch, NULL, mob, TO_VICT );
		act( "$n rzuca si� na kolana i b�aga $C o lito��!", ch, NULL, mob, TO_NOTVICT );
	}
	//stop_fighting( ch, TRUE );

	if ( !IS_NPC( ch ) && IS_NPC( mob )
			&& ( !HAS_TRIGGER( mob, TRIG_SURR )
				|| !mp_percent_trigger( mob, ch, NULL, NULL, &TRIG_SURR ) ) )
	{
		act( "$N nie zwraca na to uwagi!", ch, NULL, mob, TO_CHAR );
		multi_hit( mob, ch, TYPE_UNDEFINED );
	}
	else if ( !IS_NPC( ch ) && !IS_NPC( mob ) && !EXT_IS_SET( mob->act, PLR_ACCEPTSURR ) )
	{
		act( "$N nie zwraca na to uwagi!", ch, NULL, mob, TO_CHAR );
		multi_hit( mob, ch, TYPE_UNDEFINED );
	}
	else if ( IS_NPC(ch) && IS_NPC(mob) && ch->master != mob->master )
	{
		multi_hit( mob, ch, TYPE_UNDEFINED );
	}
	else
	{
		stop_fighting( ch, TRUE );
	}
	return;
}

void do_sla( CHAR_DATA *ch, char *argument )
{
	send_to_char( "If you want to SLAY, spell it out.\n\r", ch );
	return;
}

void do_slay( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA * victim;
	char arg[ MAX_INPUT_LENGTH ];

	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Slay whom?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{
		send_to_char( "They aren't here.\n\r", ch );
		return;
	}

	if ( ch == victim )
	{
		send_to_char( "U�miercasz sa<&m/ma/me> siebie!\n\r", ch );
		raw_kill( victim, ch );
		return;
	}

	if ( !IS_NPC( victim ) && victim->level >= get_trust( ch ) )
	{
		send_to_char( "You failed.\n\r", ch );
		return;
	}

	act( "{1U�miercasz $M z zimn� krwi�!{x", ch, NULL, victim, TO_CHAR );
	act( "{1$n u�mierca ci� z zimn� krwi�!{x", ch, NULL, victim, TO_VICT );
	act( "{1$n u�mierca $C z zimn� krwi�!{x", ch, NULL, victim, TO_NOTVICT );
	raw_kill( victim, ch );
	return;
}

void update_obj_cond( CHAR_DATA *ch, OBJ_DATA *weapon, int base_dam, int dam, CHAR_DATA *victim )
{
    int material = 0;
    if ( weapon && weapon->item_type == ITEM_WEAPON && !IS_OBJ_STAT( weapon, ITEM_UNDESTRUCTABLE ) )
    {
        /* bazowa szansa prz kazdym uderzeniu*/
        int chance = 2;
        int x;
        int hardness = 0;

        material = UMAX( 0, weapon->material );
        hardness = UMIN( 95, material_table[ material ].hardness );

        for ( x = 0; material_table[ x ].name; x++ )
        {
            if ( x == material )
            {
                break;
            }
        }

        if ( material > x )
        {
            return;
        }

        chance += ( 100 - hardness ) / 10;

        /* jesli uderzenie bylo wieksze niz 3/4 maksa obrazen z broni szansa wieksza*/
        if ( base_dam > ( 75 * ( weapon->value[ 1 ] * weapon->value[ 2 ] + weapon->value[ 6 ] ) / 100 ) )
        {
            chance += number_range(1,3);
        }

        if ( EXT_IS_SET( weapon->extra_flags, ITEM_NOREPAIR ) )
        {
            chance--;
        }

        if ( is_artefact( weapon ) && number_percent() > 20 )
        {
            chance--;
        }

        if ( chance > 0 && number_percent() < chance && number_percent() > hardness  )
        {
            weapon->condition -= ( number_percent() > hardness )? number_range( 1, 3 ) : 1;
            if ( weapon->condition < 1 )
            {
                /*artefact*/
                if ( is_artefact( weapon ) ) extract_artefact( weapon );
                destroy_obj( weapon->carried_by, weapon );
                return;
            }
        }
    }

    if ( victim && dam > 0 )
    {
        int where = -1, chance, x;
        OBJ_DATA *armor;

        switch ( number_range( 1 , 20 ) )
        {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
                where = WEAR_SHIELD;
                if ( get_eq_char( victim, where ) )
                    break;
            case 6:
            case 7:
            case 8:
            case 9:
            case 10: where = WEAR_BODY;break;
            case 11:
            case 12:
            case 13: where = WEAR_ARMS;break;
            case 14:
            case 15:
            case 16: where = WEAR_LEGS;break;
            case 17:
            case 18: where = WEAR_HEAD;break;
            case 19:
            case 20: where = WEAR_HANDS;break;
            default: break;
        }

        if ( where != -1 )
        {
            armor = get_eq_char( victim, where );
            if ( armor && !IS_OBJ_STAT( armor, ITEM_UNDESTRUCTABLE ) && ( armor->item_type == ITEM_ARMOR || armor->item_type == ITEM_CLOTHING ) )
            {
                chance = dam / 2;

                material = UMAX( 0, armor->material );

                for ( x = 0; material_table[ x ].name; x++ )
                    if ( x == material )
                        break;

                if ( material > x )
                    return;

                if ( is_artefact( armor ) )
                {
                    chance /= 2;
                }

                if ( number_percent() < chance &&
                        number_percent() > material_table[ armor->material ].hardness )
                {
                    armor->condition -= ( number_percent() > material_table[ armor->material ].hardness )? number_range( 1, 3 ) : 1;
                    if ( armor->condition < 1 )
                    {
                        /*artefact*/
                        if ( is_artefact( armor ) ) extract_artefact( armor );
                        destroy_obj( armor->carried_by, armor );
                        return;
                    }
                }
            }
        }
    }

    return;
}

SPELL_MSG* msg_lookup(sh_int sn)
{
	SPELL_MSG *tmp;

	for(tmp=spellmsg_list;tmp;tmp=tmp->next)
		if(tmp->sn==sn)
			return tmp;

	return NULL;
}

static sh_int skill_weapon_mod_table[MAX_CLASS][2][6]=
{
	{//Mag
		{ -2, 0,  1,  1,  2,  2 },
		{ -1, 0,  0,  1,  1,  2 }
	},

	{//Kleryk
		{ -1,  0,  1,  1,  2,  3 },
		{ -1,  0,  1,  2,  2,  3 }
	},

	{//Zlodziej
		{ -1,  0,  1,  2,  3,  3 },
		{ -1,  0,  1,  2,  2,  3 }
	},

	{//Wojownik
		{  0,  1,  2,  3,  4,  5 },
		{ -1,  0,  1,  2,  3,  3 }
	},

	{//Paladyn
		{  0,  0,  2,  2,  3,  3 },
		{ -1,  0,  1,  2,  2,  3 }
	},

	{//Druid
		{ -1,  0,  1,  1,  2,  3 },
		{ -1,  0,  1,  2,  2,  3 }
	},

	{//Barbarzynca
		{ -1,  0,  1,  1,  2,  2 },
		{ -1,  1,  2,  3,  4,  4 }
	},

	{//Mnich
		{  0,  1,  2,  3,  4,  5 },
		{ -2,  1,  1,  1,  2,  3 }
	},

	{//Bard
		{ -1,  0,  1,  2,  3,  3 },
		{ -1,  0,  1,  2,  2,  2 }
	},

	{//Czarny rycerz
		{  0,  0,  2,  2,  3,  3 },
		{ -1,  0,  1,  2,  2,  3 }
	},

	{//Szaman
		{ -1,  0,  1,  1,  2,  3 },
		{ -1,  0,  1,  2,  2,  3 }
	},

};

int weapon_skill_mod( CHAR_DATA *ch, bool primary, bool hitr )
{
	sh_int sn, skill, val = -1;

	if ( !ch )
		return 0;

	if ( ( IS_NPC( ch ) && EXT_IS_SET( ch->act, ACT_MONK ) ) ||
			( !IS_NPC( ch ) && ch->class == CLASS_MONK ) )
	{
		skill = get_skill( ch, gsn_unarmed_strike );

		if ( skill >= 100 )
			val = 5;
		else if ( skill >= 85 )
			val = 4;
		else if ( skill >= 65 )
			val = 3;
		else if ( skill >= 45 )
			val = 2;
		else if ( skill >= 25 )
			val = 1;
		else if ( skill >= 15 )
			;
		else
			val = 0;

		if ( val < 0 )
			return 0;

		if ( hitr )
			return skill_weapon_mod_table[ ch->class ][ 0 ][ val ];
		else
			return skill_weapon_mod_table[ ch->class ][ 1 ][ val ];
	}

	sn = get_weapon_sn( ch, primary );

	if ( sn < 0 )
		return 0;

	skill = get_weapon_skill( ch, sn );

	if ( primary )
	{
		if( IS_WEAPON_STAT( get_eq_char( ch, WEAR_WIELD ), WEAPON_TWO_HANDS ) )
		{
			skill += get_skill( ch, gsn_two_hands_fighting );
			skill /= 2;
		}
	}

	if ( skill >= 100 )
		val = 5;
	else if ( skill >= 85 )
		val = 4;
	else if ( skill >= 65 )
		val = 3;
	else if ( skill >= 45 )
		val = 2;
	else if ( skill >= 25 )
		val = 1;
	else if ( skill >= 15 )
		;
	else
		val = 0;

	if ( val < 0 )
		return 0;

	if ( hitr )
		return skill_weapon_mod_table[ ch->class ][ 0 ][ val ];
	else
		return skill_weapon_mod_table[ ch->class ][ 1 ][ val ];
};

bool check_stone_skin( CHAR_DATA *ch, CHAR_DATA *victim, int dam_type, int *dam, OBJ_DATA *weapon )
{
	AFFECT_DATA *stone = NULL;
	int d;
	bool ret = FALSE;

	if ( !IS_AFFECTED( victim, AFF_STONE_SKIN ) || !is_affected( victim, gsn_stone_skin ) )
		return FALSE;

	if ( dam_type != DAM_BASH && dam_type != DAM_PIERCE && dam_type != DAM_SLASH )
		return FALSE;

	d = *dam;

	stone = affect_find( victim->affected, gsn_stone_skin );

	if ( !stone )
		return FALSE;

	if ( stone->modifier > 0 )
	{
		if( !IS_NPC(victim) && victim->pcdata->mage_specialist == 1 )
		{
			if ( ( weapon && IS_WEAPON_STAT( weapon, WEAPON_VORPAL ) ) ||
					( ch && IS_NPC( ch ) && IS_SET( ch->attack_flags, WEAPON_VORPAL ) ) )
				d = d / 2;
			else if ( d > 70 )
				d = d / 3;
			else
			{
				d = 0;
				ret = TRUE;
			}
		}
		else
		{
			if ( ( weapon && IS_WEAPON_STAT( weapon, WEAPON_VORPAL ) ) ||
					( ch && IS_NPC( ch ) && IS_SET( ch->attack_flags, WEAPON_VORPAL ) ) )
				;
			else if ( d > 50 )
				d = d / 2;
			else
			{
				d = 0;
				ret = TRUE;
			}
		}
	}

	stone->modifier--;

	if ( stone->modifier <= 0 )
	{
		if ( stone->type > 0 && skill_table[ stone->type ].msg_off )
		{
			send_to_char( skill_table[ stone->type ].msg_off, victim );
			send_to_char( "\n\r", victim );
		}

		affect_remove( victim, stone );
		affect_strip( victim, gsn_stone_skin );
	}

	*dam = d;
	return ret;
}


bool check_increase_wounds( CHAR_DATA *victim, int dam_type, int *dam )
{
	AFFECT_DATA * paf, *spell = NULL;
	int d, level;

	if ( !IS_AFFECTED( victim, AFF_INCREASE_WOUNDS ) )
		return FALSE;

	if ( dam_type != DAM_BASH &&
			dam_type != DAM_PIERCE &&
			dam_type != DAM_SLASH )
		return FALSE;

	d = *dam;

	for ( paf = victim->affected; paf != NULL; paf = paf->next )
		if ( paf->type == gsn_increase_wounds )
		{
			spell = paf;
			break;
		}

	/* jesli ma flage, to tylko obrazenia +25% */
	if ( !spell )
		level = 25;
	else
	{
		level = spell->modifier;

		spell->duration--;

		if ( spell->duration < 0 )
		{
			if ( spell->type > 0 && spell->type < MAX_SKILL && skill_table[ spell->type ].msg_off )
			{
				send_to_char( skill_table[ spell->type ].msg_off, victim );
				send_to_char( "\n\r", victim );
			}

			affect_remove( victim, spell );
		}
	}

	d += ( ( d * level ) / 100 );
	*dam = d;
	return TRUE;
}

bool check_mirror_image( CHAR_DATA *victim, CHAR_DATA *ch )
{
	AFFECT_DATA *paf, *mirror = NULL;
	AFFECT_DATA aff;
	bool ret = FALSE;
	int chance;
	int cyferka_bijacego = 0, cyferka_omirrorowanego = 0;

	if ( IS_AFFECTED( ch, AFF_PIERCING_SIGHT ) )
		return FALSE;

	if ( !IS_AFFECTED( victim, AFF_MIRROR_IMAGE ) || ch == victim )
		return FALSE;

	for ( paf = victim->affected; paf != NULL; paf = paf->next )
		if ( paf->type == gsn_mirror_image )
		{
			mirror = paf;
			break;
		}

	if ( !mirror )
		return FALSE;

	chance = mirror->modifier;

	cyferka_bijacego += number_range( 1, get_curr_stat_deprecated( ch, STAT_LUC ) );
	cyferka_bijacego += number_range( 1, get_curr_stat_deprecated( ch, STAT_WIS ) );
	cyferka_bijacego += number_range( 1, get_curr_stat_deprecated( ch, STAT_INT ) );
	cyferka_bijacego += number_range( 1, ch->level );

	cyferka_omirrorowanego += number_range( 1, get_curr_stat_deprecated( victim, STAT_LUC ) );
	cyferka_omirrorowanego += number_range( 1, get_curr_stat_deprecated( victim, STAT_INT ) );
	cyferka_omirrorowanego += number_range( 1, mirror->level );
	cyferka_omirrorowanego += chance;

	if( !IS_NPC(victim) && victim->class == CLASS_MAG && victim->pcdata->mage_specialist == 5 )
		cyferka_omirrorowanego += number_range( 5, 8 );

	if ( cyferka_bijacego > cyferka_omirrorowanego )
		return FALSE;

	if ( mirror->modifier > 0 )
	{
		print_char( ch, "Jedno z lustrzanych odbi� %s znikn�o.\n\r", PERS2( victim, ch ) );
		send_to_char( "Jedno z twoich odbi� znikn�o.\n\r", victim );

		if ( is_affected(victim, gsn_mirrorfall) )
			affect_strip(victim, gsn_mirrorfall);
		aff.where	= TO_AFFECTS;
		aff.type	= gsn_mirrorfall;
		aff.level	= 1;
		aff.duration	= 1;
		aff.rt_duration = 0;
		aff.bitvector = &AFF_NONE;
		aff.location	= APPLY_NONE;
		aff.modifier	= 0;
		affect_to_char( victim, &aff, NULL, FALSE );

		ret = TRUE;
	}

	mirror->modifier--;


	if ( mirror->modifier <= 0 )
	{
		if ( mirror->type > 0 && skill_table[ mirror->type ].msg_off )
		{
			send_to_char( skill_table[ mirror->type ].msg_off, victim );
			send_to_char( "\n\r", victim );
		}

		affect_remove( victim, mirror );
	}

	return ret;
}

bool check_blink( CHAR_DATA *victim, CHAR_DATA *ch, bool unik )
{
	AFFECT_DATA * paf, *blink = NULL;

	if ( !is_affected( victim, gsn_blink ) || ch == victim )
		return FALSE;

	for ( paf = victim->affected; paf != NULL; paf = paf->next )
		if ( paf->type == gsn_blink )
		{
			blink = paf;
			break;
		}

	if ( !blink )
		return FALSE;

	if ( unik )
	{
		if ( number_percent() < blink->modifier )
			return TRUE;
	}
	else
	{
		if ( number_percent() < blink->level )
			return TRUE;
	}

	return FALSE;
}

bool still_fighting( CHAR_DATA *ch )
{
	CHAR_DATA * vch;

	if ( !ch->in_room )
		return FALSE;

	if ( ch->fighting )
		return TRUE;

	if ( ch->position < POS_RESTING )
		return FALSE;

	for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
		if ( vch->fighting && vch->fighting == ch )
		{
			ch->fighting = vch;
			return TRUE;
		}

	return FALSE;
}


/*
 * Inflict magic damage from a spell. */
bool spell_damage( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int sn, int dam_type, bool show )
{
	OBJ_DATA * obj, *obj_next, *wand;
	AFFECT_DATA *defense_curl;
	bool immune, wand_pierwszy_val = FALSE;
	int dam_mod = 0, dam_base = dam;

	// ~15% wi�cej obra�e� ze spelli w mobki.
	if  ( !IS_NPC( ch ) && IS_NPC( victim ) )
		dam += dam/7;

	//i tutaj nowe rozdzki, troche tego kodu jest :/
	wand = get_eq_char( ch,WEAR_HOLD );
	if( !IS_NPC(ch) && wand != NULL && wand->item_type == ITEM_WAND )
	{
		if( wand->value[0] == WAND_DAM_PERCENT )
		{
			wand_pierwszy_val = TRUE;
			if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_GOOD ) )
			{
				if( IS_GOOD( ch ) )
					dam_mod += (wand->value[1]*dam)/100;
				else
					dam_mod -= (wand->value[1]*dam)/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_NEUTRAL ) )
			{
				if( IS_NEUTRAL( ch ) )
					dam_mod += (wand->value[1]*dam)/100;
				else
					dam_mod -= (wand->value[1]*dam)/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_EVIL ) )
			{
				if( IS_EVIL( ch ) )
					dam_mod += (wand->value[1]*dam)/100;
				else
					dam_mod -= (wand->value[1]*dam)/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_MAG ) )
			{
				if( ch->class == CLASS_MAG )
					dam_mod += (wand->value[1]*dam)/100;
				else
					dam_mod -= (wand->value[1]*dam)/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_CLERIC ) )
			{
				if( ch->class == CLASS_CLERIC )
					dam_mod += (wand->value[1]*dam)/100;
				else
					dam_mod -= (wand->value[1]*dam)/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_DRUID ) )
			{
				if( ch->class == CLASS_DRUID )
					dam_mod += (wand->value[1]*dam)/100;
				else
					dam_mod -= (wand->value[1]*dam)/100;
			}
			else
				dam_mod += (wand->value[1]*dam)/100;
		}
		else if( wand->value[2] == WAND_DAM_PERCENT )
		{
			if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_GOOD ) )
			{
				if( IS_GOOD( ch ) )
					dam_mod += (wand->value[3]*dam)/100;
				else
					dam_mod -= (wand->value[3]*dam)/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_NEUTRAL ) )
			{
				if( IS_NEUTRAL( ch ) )
					dam_mod += (wand->value[3]*dam)/100;
				else
					dam_mod -= (wand->value[3]*dam)/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_EVIL ) )
			{
				if( IS_EVIL( ch ) )
					dam_mod += (wand->value[3]*dam)/100;
				else
					dam_mod -= (wand->value[3]*dam)/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_MAG ) )
			{
				if( ch->class == CLASS_MAG )
					dam_mod += (wand->value[3]*dam)/100;
				else
					dam_mod -= (wand->value[3]*dam)/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_CLERIC ) )
			{
				if( ch->class == CLASS_CLERIC )
					dam_mod += (wand->value[3]*dam)/100;
				else
					dam_mod -= (wand->value[3]*dam)/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_DRUID ) )
			{
				if( ch->class == CLASS_DRUID )
					dam_mod += (wand->value[3]*dam)/100;
				else
					dam_mod -= (wand->value[3]*dam)/100;
			}
			else
				dam_mod += (wand->value[3]*dam)/100;
		}
		else if( wand->value[4] == WAND_DAM_PERCENT )
		{
			if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_GOOD ) )
			{
				if( IS_GOOD( ch ) )
					dam_mod += (wand->value[5]*dam)/100;
				else
					dam_mod -= (wand->value[5]*dam)/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_NEUTRAL ) )
			{
				if( IS_NEUTRAL( ch ) )
					dam_mod += (wand->value[5]*dam)/100;
				else
					dam_mod -= (wand->value[5]*dam)/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_EVIL ) )
			{
				if( IS_EVIL( ch ) )
					dam_mod += (wand->value[5]*dam)/100;
				else
					dam_mod -= (wand->value[5]*dam)/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_MAG ) )
			{
				if( ch->class == CLASS_MAG )
					dam_mod += (wand->value[5]*dam)/100;
				else
					dam_mod -= (wand->value[5]*dam)/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_CLERIC ) )
			{
				if( ch->class == CLASS_CLERIC )
					dam_mod += (wand->value[5]*dam)/100;
				else
					dam_mod -= (wand->value[5]*dam)/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_DRUID ) )
			{
				if( ch->class == CLASS_DRUID )
					dam_mod += (wand->value[5]*dam)/100;
				else
					dam_mod -= (wand->value[5]*dam)/100;
			}
			else
				dam_mod += (wand->value[5]*dam)/100;
		}

		if ( dam_mod != 0 && !IS_NPC(ch) )
		{
			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_0 ) && IS_SET( skill_table[ sn ].school, school_table[ 0 ].flag ) )
			{
				if ( !wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_1 ) && IS_SET( skill_table[ sn ].school, school_table[ 1 ].flag ) )
			{
				if ( !wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_2 ) && IS_SET( skill_table[ sn ].school, school_table[ 2 ].flag ) )
			{
				if ( !wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_3 ) && IS_SET( skill_table[ sn ].school, school_table[ 3 ].flag ) )
			{
				if ( !wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_4 ) && IS_SET( skill_table[ sn ].school, school_table[ 4 ].flag ) )
			{
				if ( !wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_5 ) && IS_SET( skill_table[ sn ].school, school_table[ 5 ].flag ) )
			{
				if ( !wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_6 ) && IS_SET( skill_table[ sn ].school, school_table[ 6 ].flag ) )
			{
				if ( !wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_7 ) && IS_SET( skill_table[ sn ].school, school_table[ 7 ].flag ) )
			{
				if ( !wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_0 ) && !IS_SET( skill_table[ sn ].school, school_table[ 0 ].flag ) )
			{
				if ( wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_1 ) && !IS_SET( skill_table[ sn ].school, school_table[ 1 ].flag ) )
			{
				if ( wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_2 ) && !IS_SET( skill_table[ sn ].school, school_table[ 2 ].flag ) )
			{
				if ( wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_3 ) && !IS_SET( skill_table[ sn ].school, school_table[ 3 ].flag ) )
			{
				if ( wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_4 ) && !IS_SET( skill_table[ sn ].school, school_table[ 4 ].flag ) )
			{
				if ( wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_5 ) && !IS_SET( skill_table[ sn ].school, school_table[ 5 ].flag ) )
			{
				if ( wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_6 ) && !IS_SET( skill_table[ sn ].school, school_table[ 6 ].flag ) )
			{
				if ( wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_7 ) && !IS_SET( skill_table[ sn ].school, school_table[ 7 ].flag ) )
			{
				if ( wand_pierwszy_val )
					dam_mod = 0;
			}
		}

		dam += dam_mod;
		dam_mod = 0;

		wand_pierwszy_val = FALSE;
		if( wand->value[0] == WAND_DAM_VALUE )
		{
			wand_pierwszy_val = TRUE;
			if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_GOOD ) )
			{
				if( IS_GOOD( ch ) )
					dam_mod += wand->value[1];
				else
					dam_mod -= wand->value[1];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_NEUTRAL ) )
			{
				if( IS_NEUTRAL( ch ) )
					dam_mod += wand->value[1];
				else
					dam_mod -= wand->value[1];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_EVIL ) )
			{
				if( IS_EVIL( ch ) )
					dam_mod += wand->value[1];
				else
					dam_mod -= wand->value[1];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_MAG ) )
			{
				if( ch->class == CLASS_MAG )
					dam_mod += wand->value[1];
				else
					dam_mod -= wand->value[1];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_CLERIC ) )
			{
				if( ch->class == CLASS_CLERIC )
					dam_mod += wand->value[1];
				else
					dam_mod -= wand->value[1];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_DRUID ) )
			{
				if( ch->class == CLASS_DRUID )
					dam_mod += wand->value[1];
				else
					dam_mod -= wand->value[1];
			}
			else
				dam_mod += wand->value[1];
		}
		else if( wand->value[2] == WAND_DAM_VALUE )
		{
			if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_GOOD ) )
			{
				if( IS_GOOD( ch ) )
					dam_mod += wand->value[3];
				else
					dam_mod -= wand->value[3];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_NEUTRAL ) )
			{
				if( IS_NEUTRAL( ch ) )
					dam_mod += wand->value[3];
				else
					dam_mod -= wand->value[3];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_EVIL ) )
			{
				if( IS_EVIL( ch ) )
					dam_mod += wand->value[3];
				else
					dam_mod -= wand->value[3];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_MAG ) )
			{
				if( ch->class == CLASS_MAG )
					dam_mod += wand->value[3];
				else
					dam_mod -= wand->value[3];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_CLERIC ) )
			{
				if( ch->class == CLASS_CLERIC )
					dam_mod += wand->value[3];
				else
					dam_mod -= wand->value[3];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_DRUID ) )
			{
				if( ch->class == CLASS_DRUID )
					dam_mod += wand->value[3];
				else
					dam_mod -= wand->value[3];
			}
			else
				dam_mod += wand->value[3];
		}
		else if( wand->value[4] == WAND_DAM_VALUE )
		{
			if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_GOOD ) )
			{
				if( IS_GOOD( ch ) )
					dam_mod += wand->value[5];
				else
					dam_mod -= wand->value[5];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_NEUTRAL ) )
			{
				if( IS_NEUTRAL( ch ) )
					dam_mod += wand->value[5];
				else
					dam_mod -= wand->value[5];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_EVIL ) )
			{
				if( IS_EVIL( ch ) )
					dam_mod += wand->value[5];
				else
					dam_mod -= wand->value[5];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_MAG ) )
			{
				if( ch->class == CLASS_MAG )
					dam_mod += wand->value[5];
				else
					dam_mod -= wand->value[5];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_CLERIC ) )
			{
				if( ch->class == CLASS_CLERIC )
					dam_mod += wand->value[5];
				else
					dam_mod -= wand->value[5];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_DRUID ) )
			{
				if( ch->class == CLASS_DRUID )
					dam_mod += wand->value[5];
				else
					dam_mod -= wand->value[5];
			}
			else
				dam_mod += wand->value[5];
		}

		if ( dam_mod != 0 && !IS_NPC(ch) )
		{
			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_0 ) && IS_SET( skill_table[ sn ].school, school_table[ 0 ].flag ) )
			{
				if ( !wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_1 ) && IS_SET( skill_table[ sn ].school, school_table[ 1 ].flag ) )
			{
				if ( !wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_2 ) && IS_SET( skill_table[ sn ].school, school_table[ 2 ].flag ) )
			{
				if ( !wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_3 ) && IS_SET( skill_table[ sn ].school, school_table[ 3 ].flag ) )
			{
				if ( !wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_4 ) && IS_SET( skill_table[ sn ].school, school_table[ 4 ].flag ) )
			{
				if ( !wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_5 ) && IS_SET( skill_table[ sn ].school, school_table[ 5 ].flag ) )
			{
				if ( !wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_6 ) && IS_SET( skill_table[ sn ].school, school_table[ 6 ].flag ) )
			{
				if ( !wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_7 ) && IS_SET( skill_table[ sn ].school, school_table[ 7 ].flag ) )
			{
				if ( !wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_0 ) && !IS_SET( skill_table[ sn ].school, school_table[ 0 ].flag ) )
			{
				if ( wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_1 ) && !IS_SET( skill_table[ sn ].school, school_table[ 1 ].flag ) )
			{
				if ( wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_2 ) && !IS_SET( skill_table[ sn ].school, school_table[ 2 ].flag ) )
			{
				if ( wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_3 ) && !IS_SET( skill_table[ sn ].school, school_table[ 3 ].flag ) )
			{
				if ( wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_4 ) && !IS_SET( skill_table[ sn ].school, school_table[ 4 ].flag ) )
			{
				if ( wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_5 ) && !IS_SET( skill_table[ sn ].school, school_table[ 5 ].flag ) )
			{
				if ( wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_6 ) && !IS_SET( skill_table[ sn ].school, school_table[ 6 ].flag ) )
			{
				if ( wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_7 ) && !IS_SET( skill_table[ sn ].school, school_table[ 7 ].flag ) )
			{
				if ( wand_pierwszy_val )
					dam_mod = 0;
			}
		}
	}
	dam += dam_mod;

	if( dam <= 0 )
		dam = 1;

	if ( victim->position == POS_DEAD )
		return FALSE;

	if ( victim != ch )
	{
		if ( is_safe( ch, victim ) )
			return FALSE;


		if ( victim->position > POS_STUNNED )
		{
			if ( victim->fighting == NULL )
			{
				set_fighting( victim, ch );

				if ( IS_NPC( victim ) && HAS_TRIGGER( victim, TRIG_KILL ) )
					mp_percent_trigger( victim, ch, NULL, NULL, &TRIG_KILL );
			}
		}

		if ( victim->master == ch )
		{
			if( IS_NPC( victim ) && IS_AFFECTED(victim,AFF_CHARM) )
			{
				act( "Wi� mi�dzy tob� a $V zostaje brutalnie przerwana!", ch, NULL, victim, TO_CHAR );
				raw_damage( ch, ch, number_range( ch->level/2, ch->level+ch->level/2 ) );
				if ( ch->position <= POS_STUNNED )
					return FALSE;
			}
			stop_follower( victim );
		}
	}

	/*
	 * Inviso attacks ... not.
	 */
	if ( IS_AFFECTED( ch, AFF_HIDE ) )
		affect_strip( ch, gsn_hide );

	strip_invis( ch, TRUE, TRUE );

	if ( IS_NPC( victim ) && !is_fearing( victim, ch ) )
	{
		if ( !EXT_IS_SET( victim->act, ACT_SENTINEL ) )
			start_hunting( victim, ch );

		start_hating( victim, ch );
	}

	immune = check_blink( victim, ch, TRUE );

	if( !immune )
		immune = check_mirror_image( victim, ch );

	if ( dam == 0 || immune )
	{
		if ( show )
			dam_message( ch, victim, dam, sn, immune );

		return FALSE;
	}

	immune = IS_AFFECTED( victim, AFF_ILLUSION );

	if ( immune )
	{
		if ( show )
			dam_message( ch, victim, dam, sn, immune );

		print_char( ch, "Tw�j cel rozp�ywa si� w powietrzu i znika.\n\r", PERS4( victim, ch ) );
		send_to_char( "Rozp�ywasz si� w powietrzu.\n\r", victim );
		act( "$N rozp�ywa si� w powietrzu.", ch, NULL, victim, TO_NOTVICT );

		/* itemy co je trzymala iluzja opadaja na ziemie*/
		for ( obj = victim->carrying; obj != NULL; obj = obj_next )
		{
			OBJ_NEXT_CONTENT( obj, obj_next );

			if ( IS_OBJ_STAT( obj, ITEM_NODROP ) )
				continue;
			if ( obj->wear_loc != WEAR_NONE )
				unequip_char( victim, obj );

			obj_from_char( obj );
			act( "$p upada na ziemi�.", victim, obj, NULL, TO_ROOM );
			obj_to_room( obj, victim->in_room );

			if ( IS_OBJ_STAT( obj, ITEM_MELT_DROP ) )
			{
				switch ( number_range( 1, 3 ) )
				{
					case 1:
						act( "$p rozp�ywa si� w powietrzu.", ch, obj, NULL, TO_ROOM );
						break;
					case 2:
						act( "$p znika bez �ladu.", ch, obj, NULL, TO_ROOM );
						break;
					default:
						act( "$p rozp�ywa si� pozostawiaj�c tylko chmur� dymu.", ch, obj, NULL, TO_ROOM );
						break;
				}
			}
			else if ( obj->item_type == ITEM_POTION && ( obj->material == 11 || obj->material == 15 ) )
			{
				switch ( number_range( 1, 3 ) )
				{
					case 1:
						act( "$p toczy si� przez chwil�, po czym rozpryskuj� si� na kwa�eczki.", ch, obj, NULL, TO_ROOM );
						break;
					case 2:
						act( "$p p�ka z hukiem i rozpryskuje si� dooko�a.", ch, obj, NULL, TO_ROOM );
						break;
					default:
						act( "$p rozpryskuje si� na kawa�eczki.", ch, obj, NULL, TO_ROOM );
						break;
				}
			}
		}

		extract_char( victim, TRUE );

		return FALSE;
	}

	immune = IS_AFFECTED( victim, AFF_SOUL );

	if ( immune )
	{
		if ( show )
			dam_message( ch, victim, dam, sn, immune );
		print_char( ch, "Uwalniasz dusz� %s, kt�ra ulatuje do swojego wymiaru\r", PERS4( victim, ch ) );
		send_to_char( "Ulatujesz do swojego wymiaru.\n\r", victim );
		act( "$n uwalnia dusz� $Z, kt�ra ulatuje do swojego wymiaru.", ch, NULL, victim, TO_NOTVICT );

		/* itemy co je trzymala dusza opadaja na ziemie*/
		for ( obj = victim->carrying; obj != NULL; obj = obj_next )
		{
			OBJ_NEXT_CONTENT( obj, obj_next );

			if ( IS_OBJ_STAT( obj, ITEM_NODROP ) )
				continue;
			if ( obj->wear_loc != WEAR_NONE )
				unequip_char( victim, obj );

			obj_from_char( obj );
			act( "$p upada na ziemi�.", victim, obj, NULL, TO_ROOM );
			obj_to_room( obj, victim->in_room );

			if ( IS_OBJ_STAT( obj, ITEM_MELT_DROP ) )
			{
				switch ( number_range( 1, 3 ) )
				{
					case 1:
						act( "$p rozp�ywa si� w powietrzu.", ch, obj, NULL, TO_ROOM );
						break;
					case 2:
						act( "$p znika bez �ladu.", ch, obj, NULL, TO_ROOM );
						break;
					default:
						act( "$p rozp�ywa si� pozostawiaj�c tylko chmur� dymu.", ch, obj, NULL, TO_ROOM );
						break;
				}
			}
			else if ( obj->item_type == ITEM_POTION && ( obj->material == 11 || obj->material == 15 ) )
			{
				switch ( number_range( 1, 3 ) )
				{
					case 1:
						act( "$p toczy si� przez chwil�, po czym rozpryskuj� si� na kwa�eczki.", ch, obj, NULL, TO_ROOM );
						break;
					case 2:
						act( "$p p�ka z hukiem i rozpryskuje si� dooko�a.", ch, obj, NULL, TO_ROOM );
						break;
					default:
						act( "$p rozpryskuje si� na kawa�eczki.", ch, obj, NULL, TO_ROOM );
						break;
				}
			}
		}

		extract_char( victim, TRUE );

		return FALSE;
	}

	immune = check_stone_skin( NULL, victim, dam_type, &dam, NULL );

	if ( dam == 0 || immune )
	{
		if ( show )
			dam_message( ch, victim, dam, sn, immune );

		return FALSE;
	}

	dam = check_resist( victim, dam_type, dam );

	if ( dam == 0 )
	{
		if ( show )
			dam_message( ch, victim, dam, sn, TRUE );

		return FALSE;
	}

	if ( IS_AFFECTED( victim, AFF_TROLL ) && victim->hit > 0 && victim->hit - dam < 0 )
	{
		dam = victim->hit + 1;
		create_event( EVENT_TROLL_POWER_REGEN, number_range( 3, 5 ) * PULSE_VIOLENCE, victim, NULL, 0 );
	}

	DEBUG_INFO( "spell_damage:damage_reduction" );

	if ( is_affected(victim,gsn_demon_aura))
	{
		if( IS_GOOD(ch))
        {
            dam -= get_skill(victim,gsn_demon_aura)/6;
        }
		else if ( !IS_EVIL(ch))
        {
			dam -= get_skill(victim,gsn_demon_aura)/12;
        }
	}

	if( dam < 0 ) dam = 1;

	if ( is_affected(victim,gsn_defense_curl))
	{
		defense_curl = affect_find(victim->affected,gsn_defense_curl );
		defense_curl->modifier -= UMIN( defense_curl->level, dam );
		dam -= UMIN( defense_curl->level, dam );
		if( defense_curl->modifier <= 0 )
		{
			affect_remove( victim, defense_curl );
			act( "Otaczaj�ca ci� ochronna spirala zwija si�, a po chwili zanika.", victim,NULL,NULL,TO_CHAR);
		}
	}

	if ( is_affected( victim, gsn_damage_reduction ) && dam > 0 )
	{
		AFFECT_DATA * pAff;

		if ( ( pAff = affect_find( victim->affected, gsn_damage_reduction ) ) != NULL )
		{
			if ( pAff->level <= 16 )
				dam -= 1;
			else if ( pAff->level <= 20 )
				dam -= 2;
			else if ( pAff->level <= 25 )
				dam -= 3;
			else if ( pAff->level <= 30 )
				dam -= 4;
			else
				dam -= 5;

			dam = UMAX( dam, 1 );
		}
#ifdef INFO
		print_char( ch, "DAM - DAMAGE_REDUCTION: %d\n\r", dam );
#endif

	}

	if ( show )
    {
		dam_message( ch, victim, dam, sn, immune );
    }

    /**
     * logowanie wilkosci obrazen
     */
    append_file_format_daily
        (
         ch,
         DAM_LOG_FILE,
         "%s/%d/%s victim: %s/%d spell: %s/%s/%d/%d/%d",
         /* ch */
         IS_NPC(ch) ? "mob":"pc",
         ch->level,
         IS_NPC( ch ) ? "-" : class_table[ ch->class ].name,
         /* victim */
         IS_NPC( victim ) ? strip_colour_codes( victim->short_descr ) : victim->name,
         victim->level,
        /* spell */
         skill_table[ sn ].name,
         attack_table[ dam_type ].name,
         dam_base,
         dam_mod,
         dam
        );
		
	raw_damage( ch, victim, dam );
	return TRUE;
}


int compute_tohit( CHAR_DATA *ch, bool primary )
{
	int thac0_00, thac0_32, thac0;
	OBJ_DATA *wield;
	int sn;
	int fight_style_skill = 0;
    bool check;

	if ( primary )
		wield = get_eq_char( ch, WEAR_WIELD );
	else
		wield = get_eq_char( ch, WEAR_SECOND );

	sn = get_weapon_sn( ch, TRUE );

	if ( IS_NPC( ch ) )
	{
		thac0_00 = 20;
		if ( EXT_IS_SET( ch->act, ACT_WARRIOR ) )
			thac0_32 = 0;
		if ( EXT_IS_SET( ch->act, ACT_MONK ) )
			thac0_32 = 0;
		else if ( EXT_IS_SET( ch->act, ACT_PALADIN ) )
			thac0_32 = 0;
		else if ( EXT_IS_SET( ch->act, ACT_BLACK_KNIGHT ) )
			thac0_32 = 0;
		else if ( EXT_IS_SET( ch->act, ACT_BARBARIAN ) )
			thac0_32 = 0;
		else if ( EXT_IS_SET( ch->act, ACT_THIEF ) )
			thac0_32 = 4;
		else if ( EXT_IS_SET( ch->act, ACT_BARD ) )
			thac0_32 = 4;
		else if ( EXT_IS_SET( ch->act, ACT_CLERIC ) )
			thac0_32 = 4;
		else if ( EXT_IS_SET( ch->act, ACT_MAGE ) )
			thac0_32 = 8;
		else if ( EXT_IS_SET( ch->act, ACT_SHAMAN ) )
			thac0_32 = 4;
		else
			thac0_32 = 2;
	}
	else
	{
		thac0_00 = class_table[ ch->class ].thac0_00;
		thac0_32 = class_table[ ch->class ].thac0_32;
	}

	thac0 = interpolate( ch->level, thac0_00, thac0_32 );

	thac0 -= GET_HITROLL( ch );

	if ( wield )
		thac0 -= UMIN( wield->value[ 5 ], 6 );

	//lata na dagger_mastery
	if ( ( IS_NPC( ch ) && EXT_IS_SET( ch->act, ACT_THIEF ) ) ||
			( !IS_NPC( ch ) && ch->class == CLASS_THIEF ) )
	{
		OBJ_DATA *weapon;

		if ( ( weapon = get_eq_char( ch, WEAR_WIELD ) ) != NULL &&
				weapon->value[ 0 ] == 2 &&
				( weapon = get_eq_char( ch, WEAR_SECOND ) ) != NULL &&
				weapon->value[ 0 ] == 2 )
			fight_style_skill = get_skill( ch, gsn_dagger_mastery );
		else
			fight_style_skill = 0;

		if ( !primary )
        {
            thac0 += 1 + 8 * ( 100 - fight_style_skill ) / 100;
		}
		else
		{
			if ( get_eq_char( ch, WEAR_SECOND ) != NULL )
			{
				thac0 += 4 * ( 100 - fight_style_skill ) / 100;
			}
		}
	}
	else if ( ( IS_NPC( ch ) && EXT_IS_SET( ch->act, ACT_BLACK_KNIGHT ) ) ||
			( !IS_NPC( ch ) && ch->class == CLASS_BLACK_KNIGHT ) )
	{
		OBJ_DATA *weapon;

		if ( ( weapon = get_eq_char( ch, WEAR_WIELD ) ) != NULL &&
				weapon->value[ 0 ] == WEAPON_WHIP &&
				( weapon = get_eq_char( ch, WEAR_SECOND ) ) != NULL &&
				weapon->value[ 0 ] == WEAPON_WHIP )
			fight_style_skill = get_skill( ch, gsn_whip_mastery );
		else
			fight_style_skill = 0;

		if ( ( weapon = get_eq_char( ch, WEAR_WIELD ) ) != NULL &&
				weapon->value[ 0 ] == WEAPON_CLAWS &&
				( weapon = get_eq_char( ch, WEAR_SECOND ) ) != NULL &&
				weapon->value[ 0 ] == WEAPON_CLAWS )
			fight_style_skill = (get_skill( ch, gsn_claws )*2)/3;
		else
			fight_style_skill = 0;

		if ( !primary )
        {
            thac0 += 1 + 8 * ( 100 - fight_style_skill ) / 100;
		}
		else
		{
			if ( get_eq_char( ch, WEAR_SECOND ) != NULL )
			{
				thac0 += 4 * ( 100 - fight_style_skill ) / 100;
			}
		}
	}
	else if ( ( IS_NPC( ch ) && EXT_IS_SET( ch->act, ACT_BARBARIAN ) ) ||
			( !IS_NPC( ch ) && ch->class == CLASS_BARBARIAN ) ||
			( IS_NPC( ch ) && EXT_IS_SET( ch->act, ACT_BARD ) ) ||
			( !IS_NPC( ch ) && ch->class == CLASS_BARD ) )
	{
		fight_style_skill = get_skill( ch, gsn_two_weapon_fighting );
		fight_style_skill += UMIN( (get_curr_stat_deprecated(ch,STAT_DEX)-18)*3, 0 );

		if ( !primary )
		{
			thac0 += 1 + 8 * ( 100 - fight_style_skill/3 ) / 100;
		}
		else
		{
			if ( get_eq_char( ch, WEAR_SECOND ) != NULL )
			{
				thac0 += 4 * ( 100 - fight_style_skill/2 ) / 100;
			}
			else if ( wield && ( IS_WEAPON_STAT( wield, WEAPON_TWO_HANDS) ))
			{
				fight_style_skill = get_skill( ch, gsn_twohander_fighting );
				thac0 -= 3 * ( fight_style_skill ) / 100;
			}
		}
	}
	else
	{
		fight_style_skill = get_skill( ch, gsn_two_weapon_fighting );
		fight_style_skill += UMIN( (get_curr_stat_deprecated(ch,STAT_DEX)-15)*3, 0 );
		if ( !primary )
		{
			thac0 += 1 + 8 * ( 100 - fight_style_skill ) / 100;
		}
		else
		{
			if ( get_eq_char( ch, WEAR_SECOND ) != NULL )
			{
				thac0 += 4 * ( 100 - fight_style_skill ) / 100;
			}
			else if ( wield && ( IS_WEAPON_STAT( wield, WEAPON_TWO_HANDS) ))
			{
				fight_style_skill = get_skill( ch, gsn_twohander_fighting );
				thac0 -= 3 * ( fight_style_skill ) / 100;
			}
		}
	}

	thac0 -= weapon_skill_mod( ch, primary, TRUE );

	if ( !IS_NPC( ch ) && ch->condition[ COND_SLEEPY ] == 0 )
		thac0 += 4;

	if ( IS_SET( ch->fight_data, FIGHT_DISARM ) )
		thac0 += 4;

	if ( IS_SET( ch->fight_data, FIGHT_DODGE ) )
		thac0 += 2;

	if ( IS_SET( ch->fight_data, FIGHT_PARRY ) )
		thac0 += 6;

	if ( IS_SET( ch->fight_data, FIGHT_MIGHTY_BLOW ) )
		thac0 += 4;

	if ( IS_SET( ch->fight_data, FIGHT_POWER_STRIKE ) )
		thac0 += 8;

	if ( IS_SET( ch->fight_data, FIGHT_CRITICAL_STRIKE ) )
		thac0 += 10;

	if ( IS_SET( ch->fight_data, FIGHT_OVERWHELMING_STRIKE ) )
		thac0 += get_curr_stat_deprecated(ch,STAT_STR) > 21 ? 8 : 6;

	if ( ch->daze > 0 )
		thac0 += 6;

	if(ch->level == 1)
		thac0 = URANGE(0, thac0, number_range(10, 15));
	return thac0;
}

int calculate_final_damage( CHAR_DATA *ch, CHAR_DATA *victim, int dam)
{
  if ( IS_AFFECTED(victim,AFF_DEFLECT_WOUNDS ) && dam > 0 )
    dam /= 2;

  /* nowe modyfikatory obra�e�, dla wersji > 2.104 */
  /* Og�lnie: PvM 50%, reszta bez zmian. */
  /*
  if ( !IS_NPC(ch) && IS_NPC(victim) )
    {
      dam /= 2;
    }
	*/
  return dam;
}

/* do_raw_damage */
void raw_damage( CHAR_DATA *ch, CHAR_DATA *victim, int dam )
{
    int i;
    AFFECT_DATA *paf;

    if( check_illusion( victim ) )
    {
        return;
    }

    if ( victim->position == POS_DEAD || !victim->in_room )
    {
        return;
    }

    DEBUG_INFO( "raw_damage:gain_exp_from_damage" );

    /* exp za robienie szkody*/
    if ( !IS_NPC( ch ) && ch != victim && dam > 0 )
    {
        int xval;
        dam = UMIN( victim->hit + 11, dam );
        xval = number_range( dam * 1, dam * 3 );
        xval = UMIN( number_range(100, 300), xval );
        gain_exp( ch, xval, TRUE );
    }

    if ( IS_AFFECTED(victim,AFF_DEFLECT_WOUNDS ) && dam > 0 )
        dam /= 2;

    //gracz vs. mob mnozone x150%
    if ( !IS_NPC(ch) )
    {
        dam *=DAMAGE_MODIFIER_PLAYER_VS_MOB;
        //dam *= 3;
        //dam /= 2;
    }


    DEBUG_INFO( "raw_damage:negative_dam" );
    if ( dam < 0 )
    {
        victim->hit = UMIN( get_max_hp(victim), victim->hit - dam );
        return;
    }

    victim->hit -= dam;

    DEBUG_INFO( "raw_damage:mount1" );

    if ( victim->mount && dam > number_range(5, 8) )//coby nie spadali po byle gownie
    {
        do_dismount_body(victim);
        send_to_char( "Tracisz rownowag� i spadasz z wierzchowca.\n\r", victim );
        act( "$n traci r�wnowag� i spada z wierzchwca.", victim, NULL, NULL, TO_ROOM );
        victim->position = POS_SITTING;
        WAIT_STATE( victim, 6 );
    }
    else if ( victim->mounting )
    {
        strip_invis( victim->mounting, TRUE, TRUE );

        switch ( victim->mounting->position )
        {
            case POS_DEAD:
                act( "Trup $z zostaje zrzucony przez $N na ziemi�.", victim, NULL, victim->mounting, TO_NOTVICT );
                break;
            case POS_MORTAL:
            case POS_INCAP:
            case POS_STUNNED:
                send_to_char( "Osuwasz si� ze swojego wierzchowca.\n\r", victim->mounting );
                act( "$n zrzuca $C na ziemi�.", victim, NULL, victim->mounting, TO_NOTVICT );
                break;
            case POS_SLEEPING:
                act( "$z spada z $Z na ziemi�.", victim, NULL, victim->mounting, TO_NOTVICT );
                break;
            case POS_FIGHTING:
                send_to_char( "Tw�j wierzchowiec zrzuca ci� w ferworze walki na ziemi�.\n\r", victim->mounting );
                act( "$n zrzuca w ferworze walki $C na ziemi�.", victim, NULL, victim->mounting, TO_NOTVICT );
                break;
            case POS_RESTING:
            case POS_SITTING:
            case POS_STANDING:
            default :
                send_to_char( "Tw�j wierzchowiec zrzuca ci� na ziemi�.\n\r", victim->mounting );
                act( "$n zrzuca $C na ziemi�.", victim, NULL, victim->mounting, TO_NOTVICT );
                break;
        }
        victim->mounting->position = POS_SITTING;
        WAIT_STATE( victim->mounting, 6 );
        do_dismount_body(victim);
    }

    /* zapisz obra�enia wi�ksze ni� 100 */
    if (dam > 100)
    {
        sprintf( log_buf, "[%d][damlog] %s (%d) zadano obra�enia <%d> przez %s (%d) (%d) w %s.",
                victim->in_room ? victim->in_room->vnum : 0,
                victim->name,
                victim->level,
                dam,
                ( IS_NPC( ch ) ? ch->short_descr : ch->name ),
                ch->level,
                ( IS_NPC( ch ) ? -1 : ch->class ),
                victim->in_room ? victim->in_room->name : "unknown"
               );
        log_string( log_buf );
    }

    /* zapisz ostatnie dam do countera */
    for ( i = 2; i > 0; i--)
    {
        victim->counter[ i ] = victim->counter[ i-1 ];
    }

    victim->counter[ 0 ] = dam;

    DEBUG_INFO( "raw_damage:wait_char" );
    if ( victim->wait_char )
    {
        if ( victim->wait >= 0 )
            victim->wait = skill_table[ victim->wait_char->sn ].beats;

        free_pwait_char( victim->wait_char );
        victim->wait_char = NULL;
        print_char( victim, "Straci�%s� koncentracj�.\n\r", victim->sex == 2 ? "a" : victim->sex == 1 ? "e" : "o" );
    }

    /* immortal wiadomo */
    if ( !IS_NPC( victim )
            && victim->level >= LEVEL_IMMORTAL
            && victim->hit < 1 )
    {
        victim->hit = 1;
    }

    //holy prayer, szansa na "revive" przy prawie pelnej puli, revive czysci pule
    if ( !IS_NPC( victim ) && victim->hit < 1 )
    {
        if ( ( paf = affect_find( victim->affected, gsn_prayer_last )) != NULL )
        {
            if(paf->modifier < 20)
            {
                if ( ( paf = affect_find( victim->affected, gsn_holy_pool )) != NULL)
                {
                    if(paf->modifier > 80 && number_percent() < (paf->modifier/4))
                    {

                        victim->hit = get_max_hp(victim)/2;
                        paf->modifier = 0;
                        victim->position = POS_STANDING;
                        switch ( victim->sex )
                        {
                            case 0:
                                send_to_char( "�mierelnie ranne, trac�c si�y, osuwasz si� powoli na ziemi�. Gdy twoja twarz ma uderzy� o ziemi�, czujesz jak zgromadzona w tobie �wi�ta moc wzburza si� i rezonuje, otaczaj�c twoje rany. Z niedowierzaniem patrzysz na to co si� dzieje. Na chwil� czas zdaje si� przesta� istnie�. Gdy otrz�sujesz si� z zdziwienia, wszystko wraca do normy, a ty orientujesz si�, �e znowu stoisz na nogach, twoje najpowa�niejsze rany si� zasklpei�y, a wype�niaj�ca ci� �wi�ta moc wygas�a.\n\r", victim );
                                act( "Widzisz jak $n osuwa si� nieprzytomnie na kolana, gdy nagle jego cia�o wybucha niesamowitym, o�lepiaj�cym �wiat�em! Gdy otwierasz oczy, zauwa�asz, �e cz�� ran $z si� zasklepi�a, a otaczaj�ce go niezwyk�e �wiat�o zupe�nie wygas�o.", victim, NULL, NULL, TO_ROOM );
                                break;
                            case 1:
                                send_to_char( "�mierelnie ranny, trac�c si�y, osuwasz si� powoli na ziemi�. Gdy twoja twarz ma uderzy� o ziemi�, czujesz jak zgromadzona w tobie �wi�ta moc wzburza si� i rezonuje, otaczaj�c twoje rany. Z niedowierzaniem patrzysz na to co si� dzieje. Na chwil� czas zdaje si� przesta� istnie�. Gdy otrz�sujesz si� z zdziwienia, wszystko wraca do normy, a ty orientujesz si�, �e znowu stoisz na nogach, twoje najpowa�niejsze rany si� zasklpei�y, a wype�niaj�ca ci� �wi�ta moc wygas�a.\n\r", victim );
                                act( "Widzisz jak $n osuwa si� nieprzytomnie na kolana, gdy nagle jego cia�o wybucha niesamowitym, o�lepiaj�cym �wiat�em! Gdy otwierasz oczy, zauwa�asz, �e cz�� ran $z si� zasklepi�a, a otaczaj�ce go niezwyk�e �wiat�o zupe�nie wygas�o.", victim, NULL, NULL, TO_ROOM );
                                break;
                            default :
                                send_to_char( "�mierelnie ranna, trac�c si�y, osuwasz si� powoli na ziemi�. Gdy twoja twarz ma uderzy� o ziemi�, czujesz jak zgromadzona w tobie �wi�ta moc wzburza si� i rezonuje, otaczaj�c twoje rany. Z niedowierzaniem patrzysz na to co si� dzieje. Na chwil� czas zdaje si� przesta� istnie�. Gdy otrz�sujesz si� z zdziwienia, wszystko wraca do normy, a ty orientujesz si�, �e znowu stoisz na nogach, twoje najpowa�niejsze rany si� zasklpei�y, a wype�niaj�ca ci� �wi�ta moc wygas�a.\n\r", victim );
                                act( "Widzisz jak $n osuwa si� nieprzytomnie na kolana, gdy nagle jej cia�o wybucha niesamowitym, o�lepiaj�cym �wiat�em! Gdy otwierasz oczy, zauwa�asz, �e cz�� ran $z si� zasklepi�a, a otaczaj�ce j� niezwyk�e �wiat�o zupe�nie wygas�o.", victim, NULL, NULL, TO_ROOM );
                                break;
                        }
                    }


                }
            }
        }
    }



    //koniec holy prayer

    DEBUG_INFO( "raw_damage:update_pos" );
    update_pos( victim );

    switch ( victim->position )
    {
        case POS_MORTAL:
            if ( IS_NPC( victim ) && IS_SET( victim->form, FORM_CONSTRUCT ) )
            {
                switch ( victim->sex )
                {
                    case 0:
                        act( "$n jest unieruchomione.", victim, NULL, NULL, TO_ROOM );
                        break;
                    case 1:
                        act( "$n jest unieruchomiony.", victim, NULL, NULL, TO_ROOM );
                        break;
                    default :
                        act( "$n jest unieruchomiona.", victim, NULL, NULL, TO_ROOM );
                        break;
                }
                break;
            }

            switch ( victim->sex )
            {
                case 0:
                    act( "$n jest �miertelnie ranne, umrze je�li nikt temu nie pomo�e.", victim, NULL, NULL, TO_ROOM );
                    send_to_char( "Jeste� �miertelnie ranne, umrzesz je�li nikt ci nie pomo�e.\n\r", victim );
                    break;
                case 1:
                    act( "$n jest �miertelnie ranny, umrze je�li nikt mu nie pomo�e.", victim, NULL, NULL, TO_ROOM );
                    send_to_char( "Jeste� �miertelnie ranny, umrzesz je�li nikt ci nie pomo�e.\n\r", victim );
                    break;
                default :
                    act( "$n jest �miertelnie ranna, umrze je�li nikt jej nie pomo�e.", victim, NULL, NULL, TO_ROOM );
                    send_to_char( "Jeste� �miertelnie ranna, umrzesz je�li nikt ci nie pomo�e.\n\r", victim );
                    break;
            }
            break;

        case POS_INCAP:
            if ( IS_NPC( victim ) && IS_SET( victim->form, FORM_CONSTRUCT ) )
            {
                switch ( victim->sex )
                {
                    case 0:
                        act( "$n jest unieruchomione.", victim, NULL, NULL, TO_ROOM );
                        break;
                    case 1:
                        act( "$n jest unieruchomiony.", victim, NULL, NULL, TO_ROOM );
                        break;
                    default :
                        act( "$n jest unieruchomiona.", victim, NULL, NULL, TO_ROOM );
                        break;
                }
                break;
            }

            switch ( victim->sex )
            {
                case 0:
                    act( "$n jest unieruchomione i umrze je�li temu nikt nie pomo�e.", victim, NULL, NULL, TO_ROOM );
                    send_to_char( "Jeste� unieruchomione i umrzesz je�li ci nikt nie pomo�e.\n\r", victim );
                    break;
                case 1:
                    act( "$n jest unieruchomiony i umrze je�li mu nikt nie pomo�e.", victim, NULL, NULL, TO_ROOM );
                    send_to_char( "Jeste� unieruchomiony i umrzesz je�li ci nikt nie pomo�e.\n\r", victim );
                    break;
                default :
                    act( "$n jest unieruchomiona i umrze je�li jej nikt nie pomo�e.", victim, NULL, NULL, TO_ROOM );
                    send_to_char( "Jeste� unieruchomiona i umrzesz je�li ci nikt nie pomo�e.\n\r", victim );
                    break;
            }
            break;

        case POS_STUNNED:
            if ( IS_NPC( victim ) && IS_SET( victim->form, FORM_CONSTRUCT ) )
            {
                switch ( victim->sex )
                {
                    case 0:
                        act( "$n jest unieruchomione.", victim, NULL, NULL, TO_ROOM );
                        break;
                    case 1:
                        act( "$n jest unieruchomiony.", victim, NULL, NULL, TO_ROOM );
                        break;
                    default :
                        act( "$n jest unieruchomiona.", victim, NULL, NULL, TO_ROOM );
                        break;
                }
                break;
            }

            switch ( victim->sex )
            {
                case 0:
                    act( "$n jest oszo�omione, ale prawdopodobnie wyjdzie z tego.", victim, NULL, NULL, TO_ROOM );
                    send_to_char( "Jeste� oszo�omione, ale prawdopodobnie wyjdziesz z tego.\n\r", victim );
                    break;
                case 1:
                    act( "$n jest oszo�omiony, ale prawdopodobnie wyjdzie z tego.", victim, NULL, NULL, TO_ROOM );
                    send_to_char( "Jeste� oszo�omiony, ale prawdopodobnie wyjdziesz z tego.\n\r", victim );
                    break;
                default :
                    act( "$n jest oszo�omiona, ale prawdopodobnie wyjdzie z tego.", victim, NULL, NULL, TO_ROOM );
                    send_to_char( "Jeste� oszo�omiona, ale prawdopodobnie wyjdziesz z tego.\n\r", victim );
                    break;
            }
            break;

        case POS_DEAD:

            /* zeby triger DEAD odpalal si� wczesniej */
            if ( IS_NPC( victim ) && HAS_TRIGGER( victim, TRIG_DEATH ) )
            {
                victim->position = POS_STANDING;
                mp_percent_trigger( victim, ch, NULL, NULL, &TRIG_DEATH );
                victim->position = POS_DEAD;
            }
            else
            {
                if ( !( IS_NPC( victim ) && IS_SET( victim->form, FORM_CONSTRUCT ) ) )
                {
                    act( "$n nie �yje!!", victim, 0, 0, TO_ROOM );
                    switch ( victim->sex )
                    {
                        case 0:
                            act( "$n pada na ziemi�... MARTWE.", victim, NULL, NULL, TO_ROOM );
                            break;
                        case 1:
                            act( "$n pada na ziemi�... MARTWY.", victim, NULL, NULL, TO_ROOM );
                            break;
                        default :
                            act( "$n pada na ziemi�... MARTWA.", victim, NULL, NULL, TO_ROOM );
                            break;
                    }
                }
                else
                {
                    act( "$n rozpada si� na kawa�eczki.", victim, 0, 0, TO_ROOM );
                }
            }
            send_to_char( "Nie �yjesz, co za pech!!!\n\r\n\r", victim );
            break;

        default:
            if ( dam > get_max_hp(victim) / 4 )
            {
                send_to_char( "{RTO naprawd� bola�o!{x\n\r", victim );
                if( dam > 35 && number_percent() > con_app[get_curr_stat_deprecated(victim,STAT_CON)].shock )
                {
                    DAZE_STATE( victim, URANGE( 12, (dam - (get_max_hp(victim)/4)) + ( ( (MAX_STAT_VALUE - get_curr_stat_deprecated(victim,STAT_CON)/2)/11 ) ), 48) );
                    send_to_char( "B�l nie do wytrzymania chwilowo parali�uje twoje mi�nie!\n\r", victim );
                    act( "$n wydaje z siebie pe�en b�lu ryk! To musia�o bole�...", victim, NULL, NULL, TO_ROOM );
                }
            }
            if ( victim->hit < get_max_hp(victim) / 5 )
            {
                send_to_char( "{RPowa�nie KRWAWISZ{x!\n\r", victim );
            }

            if ( !IS_NPC( victim )
                    && victim->hit > 0
                    && victim->hit <= victim->wimpy
                    && victim->wait == 0 )
            {
                do_flee( victim, "" );
            }

            break;
    }

    DEBUG_INFO( "raw_damage:one_hit_kill_check" );
    /* zeby grupkowicze zalapali ze im kogos sprzatneli */
    if ( IS_NPC( victim ) )
        onehit_kill_check( victim, ch );

    if ( !IS_AWAKE( victim ) )
        stop_fighting( victim, FALSE );

    DEBUG_INFO( "raw_damage:pos_dead_stuff" );
    if ( victim->position == POS_DEAD )
    {
        //group_gain( ch, victim );
        if ( !IS_NPC( victim ) && victim != ch )
        {
            sprintf( log_buf, "[%d] [player death] %s (%d) zosta�%s zabity przez %s (%d) w %s.",
                    victim->in_room ? victim->in_room->vnum : 0,
                    victim->name,
                    victim->level,
                    victim->sex == 2 ? "a" : victim->sex == 1 ? "" : "o",
                    ( IS_NPC( ch ) ? ch->short_descr : ch->name ),
                    ch->level,
                    victim->in_room ? victim->in_room->name : "unknown"
                   );
            log_string( log_buf );
        }
        /**
         * prepare wiznet string
         */
        sprintf( log_buf, "[%d] %s%s (%d) rozwalon%s przez %s (%d) w %s.",
                victim->in_room ? victim->in_room->vnum : 0,
                IS_NPC( victim )? "NPC ":"",
                IS_NPC( victim )? victim->short_descr : victim->name,
                victim->level,
                victim->sex == 2 ? "a" : victim->sex == 1 ? "y" : "e",
                ch->name4,
                ch->level,
                victim->in_room ? victim->in_room->name : "unknown"
               );
        if ( IS_NPC( victim ) )
        {
            wiznet( log_buf, NULL, NULL, WIZ_MOBDEATHS, 0, 0 );
        }
        else
        {
            wiznet( log_buf, NULL, NULL, WIZ_DEATHS, 0, 0 );
        }

        raw_kill( victim, ch );
        return;
    }

    return;
}

/*
 * sprawdza czy powinien stracic expa, ile traci i czy nie traci tez poziomu
 * dodatkowo, nadawany affect 'player death' po smierci
 */

void check_player_death( CHAR_DATA *ch, CHAR_DATA *killer )
{
	int exp_loss, szanse = - MAX_LEVEL, scinka;
	AFFECT_DATA *taff, aff;
	const int ROZNICA_LEV = 4; //r�nica lev powy�ej kt�rej mniej nas boli zgon
	const int LEVEL_PROTECTED = 18; //poziom do kt�rego mniej nas boli zgon

	if ( IS_NPC( ch ) || ch->level == 1 || IS_IMMORTAL( ch ) )
    {
        return;
    }

//Brohacz: poprawia w plikach postaci, bo na ostatnim levelu exp rosl przy padzie
	if ( ch->level == LEVEL_HERO && ch->exp > exp_per_level( ch, 30 ) )
		ch->exp = exp_per_level( ch, 30 );

    /**
     * 2008-05-10 - do po�owy LEVEL_NEWBIE - nie traci si� expa za zgon
     */
	if ( ch->level < LEVEL_NEWBIE / 2 )
		return;

	/* Tener: je�eli zabi� mob na charmie, to prawdziwym killerem jest jego master */
	if ( IS_AFFECTED( killer, AFF_CHARM ) && killer->master )
	   killer = killer->master;

	//rellik: kary za zgon, obliczanie roznicy pomiedzy graczami pozwoli uzale�ni� kary od niej
	if ( !IS_NPC( killer ) ) szanse = killer->level - ch->level;

    if ( !is_affected( ch, gsn_playerdeathlearnpenalty ) )
    {
        aff.where       = TO_AFFECTS;
        aff.type        = gsn_playerdeathlearnpenalty;
        aff.level       = 0;
        aff.duration    = 2 + dice(2, 4);
        aff.rt_duration = 2 * aff.duration;
        aff.location    = APPLY_NONE;
        aff.modifier    = 0;
        aff.bitvector   = &AFF_NONE;
        affect_to_char( ch, &aff, NULL, FALSE );
    }

	if ( is_affected( ch, gsn_playerdeath ) )
	{
/*		if ( IS_NPC( killer ) )
			exp_loss = ( number_range( 18, 30 ) * ( exp_per_level( ch, ch->level ) - exp_per_level( ch, ch->level - 1 ) ) ) / 100;
		else
			exp_loss = ( number_range( 10, 22 ) * ( exp_per_level( ch, ch->level ) - exp_per_level( ch, ch->level - 1 ) ) ) / 100;
*/

		// testowe zmniejszenie traconego expa!
		// Gurthg: 2007-05-09
	//Brohacz: BUGFIX: go�� na 31 levelu ma exp obliczany z expa potrzebnego z levelu 30 na 31
	if ( ch->level == LEVEL_HERO )
		exp_loss = ( exp_per_level( ch, 30 ) - exp_per_level( ch, 29 ) ) / 6;
	else
		exp_loss = ( exp_per_level( ch, ch->level ) - exp_per_level( ch, ch->level - 1 ) ) / 10;

        if ( ch->level <  LEVEL_IMMORTAL)
        {
            ch->exp -= exp_loss;
        }

        if ( exp_per_level( ch, ch->level - 1 ) > ch->exp )
        {
            delevel( ch, FALSE );
        }

		if ( ( taff = affect_find( ch->affected, gsn_playerdeath ) ) == NULL )
			return;

		switch ( taff->level )
		{
			/* teraz juz mozna zdelevelowac */
			//rellik: kary za zgon, drugi zgonik no...
		case 0:
		case 1:
			if ( ch->level < LEVEL_PROTECTED || szanse > ROZNICA_LEV )
			{
				taff->level = 2;
			} else {
				taff->level = 3;
			}
			taff->duration += ( 1 * 60 ); //rellik: kary za zgon, drugi zgon przed�u�a dzia�anie afekt�w, dodamy godzink�
			taff->rt_duration += ( 2 * 60 ); //rellik: i dorzucamy jeszcze godzink� (mo�e by� poza gr�)
			skill_loss( ch, 1 );
			aff.where	= TO_AFFECTS;
			aff.type	= gsn_playerdeathpenalty;
			aff.level	= 0;
			aff.duration	= ( 1 * 58 ); //rellik: godzink� na mudzie, aby zgubi� II zgon (zostanie odpracowanie reszt� z pierwszego)
			aff.rt_duration = ( 2 * 59 ); //rellik: i 2 godzinki czasu oboj�tnie czy w grze (czyli 2 godziny z tego jedn� mo�e by� poza gr�)
			aff.bitvector = &AFF_NONE;
			aff.location	= APPLY_LUC;
			aff.modifier	= -3;
			affect_to_char( ch, &aff, "II zgon", FALSE );
			aff.location = number_range( APPLY_STR, APPLY_DEX );
			aff.modifier = -number_range( 0, 2 );
			affect_to_char( ch, &aff, "II zgon", FALSE );
			return;
			/* tu tez mozna zdelevelowac, i co gorsza
			* dodawane sa minusy do str, dex, con
			*/
		case 2:
		case 3:
			//rellik: kary za zgon, po trzecim zgonie ju� os�abiamy
			if ( ch->level < LEVEL_PROTECTED || szanse > ROZNICA_LEV )
			{
				scinka = 2;
			} else {
				scinka = 3;
			}
			taff->level = 4;
			taff->duration += ( 1 * 60 ); //rellik: dok�adamy kolejn� godzink� gry
			taff->rt_duration += ( 4 * 60 ); //rellik: i ... sady�ci ... 3 godziny, kt�re mo�e wykorzysta� na pisanie do nas za�ale� na d�ugo�� trwania kar
			aff.where	= TO_AFFECTS;
			aff.type	= gsn_playerdeathpenalty;
			aff.level	= 0;
			aff.duration	= ( 1 * 60 ); //rellik: jeszcze jedn� godzink�
			aff.rt_duration = ( 4 * 59 ); //rellik: tak tak... 3 godziny z kary mo�e po�wi�ci� na granie inn�  postaci�
			aff.bitvector = &AFF_NONE;
			aff.location	= APPLY_STR;
			aff.modifier	= -scinka;
			affect_to_char( ch, &aff, "III zgon", FALSE );
			aff.location	= APPLY_DEX;
			aff.modifier	= -scinka;
			affect_to_char( ch, &aff, "III zgon", FALSE );
			aff.location	= APPLY_CON;
			aff.modifier	= -scinka;
			affect_to_char( ch, &aff, "III zgon", FALSE );
			aff.location	= APPLY_LUC;
			aff.modifier	= -scinka;
			affect_to_char( ch, &aff, "III zgon", FALSE );
			aff.location	= APPLY_INT;
			aff.modifier	= -scinka;
			affect_to_char( ch, &aff, "III zgon", FALSE );
			aff.location	= APPLY_WIS;
			aff.modifier	= -scinka;
			affect_to_char( ch, &aff, "III zgon", FALSE );
			aff.location	= APPLY_CHA;
			aff.modifier	= -scinka;
			affect_to_char( ch, &aff, "III zgon", FALSE );
			skill_loss( ch, 2 );
			return;
		default:
			return;
		}
		return;
	}
	else
	{
		/* pierwszy zgon od conajmniej x godzin
		  	exp spada gora do 1 potrzebnej na tym poziomie
		*/
		//rellik: kary za zgon, spadek luck
		aff.where	= TO_AFFECTS;
		aff.type	= gsn_playerdeath;
		if ( ch->level < LEVEL_PROTECTED || szanse > ROZNICA_LEV )
		{
			aff.level = 0;
		} else {
			aff.level	= 1;
		}
		if ( IS_NPC( killer ) && ch->level == LEVEL_HERO ) //Brohacz: zmniejsza bezkarnosc pierwszego pada od moba na 31 levelu
		{
			aff.duration	= (1 * 90); //1,5 godzinki w stanie zwi�kszonego zagro�enia, �e trafi si� drugi zgon...
			aff.rt_duration = (4 * 60); //2,5 godzinki mo�e przesiedzie� poza mudem
		}
		else
		{
			aff.duration	= (1 * 60); //1 godzinka w stanie zwi�kszonego zagro�enia, �e trafi si� drugi zgon...
			aff.rt_duration = (2 * 60); //drug� mo�e przesiedzie� poza mudem
		}
		aff.bitvector = &AFF_NONE;
		aff.location	= APPLY_LUC;
		aff.modifier	= -3;
		affect_to_char( ch, &aff, "I zgon", FALSE );

		//rellik: kary za zgon, wywali�em strat� '+' przy pierwszym zgonie

/*		if ( IS_NPC( killer ) )
			exp_loss = ( number_range( 13, 25 ) * ( exp_per_level( ch, ch->level ) - exp_per_level( ch, ch->level - 1 ) ) ) / 100;
		else
			exp_loss = ( number_range( 8, 15 ) * ( exp_per_level( ch, ch->level ) - exp_per_level( ch, ch->level - 1 ) ) ) / 100;
*/

		// testowe zmniejszenie traconego expa!
		// Gurthg: 2007-05-09
	//Brohacz: BUGFIX: go�� na 31 levelu ma exp obliczany z expa potrzebnego z levelu 30 na 31
        if ( ch->level == LEVEL_HERO )
        {
            exp_loss = ( exp_per_level( ch, 30 ) - exp_per_level( ch, 29 ) ) / 6;
        }
        else
        {
            exp_loss = ( exp_per_level( ch, ch->level ) - exp_per_level( ch, ch->level - 1 ) ) / 10;
        }

        if ( ch->level <  LEVEL_IMMORTAL)
        {
            ch->exp = UMAX( exp_per_level( ch, ch->level - 1 ), ch->exp - exp_loss );
        }

        return;
    }

    return;
}

void generate_onehit_msg( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt, bool immune )
{
	char buf[ 256 ];
	const char *ch_msg = "";
	const char *victim_msg = "";
	const char *room_msg = "";
	const char *attack;

	if ( ch == NULL || victim == NULL )
		return;

	if ( immune )
	{
		/* jakis tekscik wygenerowac*/
		return;
	}

	if ( dam == 0 )
	{
		switch ( number_range( 0, 6 ) )
		{
			case 0:
				ch_msg = "{mPr�bujesz wyprowadzi� cios, ale chybiasz $C haniebnie.{x";
				victim_msg = "{m$n pr�buje wyprowadzi� cios, ale chybia cie haniebnie.{x";
				room_msg = "{m$n pr�buje wyprowadzi� cios, ale chybia $C haniebnie.{x";
				break;
			case 1:
				ch_msg = "{mAtakujesz, ale w ostatniej chwili tracisz rownowag� i chybiasz $C.{x";
				victim_msg = "{m$n atakuje ci�, ale w ostatniej chwili traci rownowag� i chybia.{x";
				room_msg = "{m$n atakuje $C, ale w ostatniej chwili traci rownowag� i chybia.{x";
				break;
			case 2:
				ch_msg = "{mChybiasz $C.{x";
				victim_msg = "{m$n chybia ci�.{x";
				room_msg = "{m$n chybia $C.{x";
				break;
			case 3:
				ch_msg = "{mNiestety mimo ca�ego wysi�ku w�o�onego w uderzenie chybiasz $C.{x";
				victim_msg = "{m$n wk�ada sporo wysi�ku w ten cios, jednak chybia ci�.{x";
				room_msg = "{m$n wk�ada sporo wysi�ku w ten cios, jednak chybia $C.{x";
				break;
			case 4:
				ch_msg = "{mTwoje uderzenie przecina z sykiem powietrze chybiaj�c $C o w�os.{x";
				victim_msg = "{mUderzenie $z przecina z sykiem powietrze chybiaj�c ci� o w�os.{x";
				room_msg = "{mUderzenie $z przecina z sykiem powietrze chybiaj�c $C o w�os.{x";
				break;
			case 5:
				ch_msg = "{mBalansujesz cia�em pr�buj�c trafi� $C, ale chybiasz.{x";
				victim_msg = "{m$n balansuje cia�em pr�buj�c ci� trafi�, ale chybia.{x";
				room_msg = "{m$n balansuje cia�em pr�buj�c trafi� $C, ale chybia.{x";
				break;
			case 6:
				ch_msg = "{mPr�bujesz waln�� $C, ale mijasz si� ze swoim celem.{x";
				victim_msg = "{m$n pr�buje ci� waln��, ale mija si� z tob�.{x";
				room_msg = "{m$n pr�buje waln�� $C, ale mija si� ze swoim celem.{x";
				break;
		}
		act( ch_msg, ch, NULL, victim, TO_CHAR );
		act( victim_msg, ch, NULL, victim, TO_VICT );
		act( room_msg, ch, NULL, victim, TO_NOTVICT );
		return;
	}
	else if ( dam <= 3 )
	{
		switch ( number_range( 0, 1 ) )
		{
			case 0:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s cie i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
			case 1:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ci� i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
		}
	}
	else if ( dam <= 7 )
	{
		switch ( number_range( 0, 1 ) )
		{
			case 0:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ci� i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
			case 1:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ci� i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
		}
	}
	else if ( dam <= 11 )
	{
		switch ( number_range( 0, 1 ) )
		{
			case 0:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ci� i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
			case 1:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ci� i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
		}
	}
	else if ( dam <= 15 )
	{
		switch ( number_range( 0, 1 ) )
		{
			case 0:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ci� i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
			case 1:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ci� i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
		}
	}
	else if ( dam <= 19 )
	{
		switch ( number_range( 0, 1 ) )
		{
			case 0:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ci� i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
			case 1:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ci� i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
		}
	}
	else if ( dam <= 23 )
	{
		switch ( number_range( 0, 1 ) )
		{
			case 0:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ci� i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
			case 1:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ci� i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
		}
	}
	else if ( dam <= 27 )
	{
		switch ( number_range( 0, 1 ) )
		{
			case 0:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ci� i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
			case 1:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ci� i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
		}
	}
	else if ( dam <= 31 )
	{
		switch ( number_range( 0, 1 ) )
		{
			case 0:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ci� i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
			case 1:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ci� i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
		}
	}
	else if ( dam <= 35 )
	{
		switch ( number_range( 0, 1 ) )
		{
			case 0:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ci� i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
			case 1:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ci� i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
		}
	}
	else if ( dam <= 39 )
	{
		switch ( number_range( 0, 1 ) )
		{
			case 0:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ci� i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
			case 1:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ci� i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
		}
	}
	else if ( dam <= 45 )
	{
		switch ( number_range( 0, 1 ) )
		{
			case 0:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ci� i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
			case 1:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ci� i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
		}
	}
	else if ( dam <= 50 )
	{
		switch ( number_range( 0, 1 ) )
		{
			case 0:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ci� i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
			case 1:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ci� i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
		}
	}
	else if ( dam <= 60 )
	{
		switch ( number_range( 0, 1 ) )
		{
			case 0:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ci� i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
			case 1:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ci� i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
		}
	}
	else if ( dam <= 75 )
	{
		switch ( number_range( 0, 1 ) )
		{
			case 0:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ci� i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
			case 1:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ci� i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
		}
	}
	else
		return;

	if ( dt >= TYPE_HIT && dt < TYPE_HIT + MAX_DAMAGE_MESSAGE )
		attack	= attack_table[ dt - TYPE_HIT ].noun;
	else
	{
		bug( "Dam_message: bad dt %d.", dt );
		attack = attack_table[ 0 ].name;
	}

	sprintf( buf, ch_msg, attack );
	act( buf, ch, NULL, victim, TO_CHAR );
	sprintf( buf, victim_msg, attack );
	act( buf, ch, NULL, victim, TO_VICT );
	sprintf( buf, room_msg, attack );
	act( buf, ch, NULL, victim, TO_NOTVICT );
	return;
}

/* zeby grupka zalapala atak po ktorym ktos z grupki zejdzie */
void onehit_kill_check( CHAR_DATA *mob, CHAR_DATA *ch )
{
	CHAR_DATA * vch;
	save_debug_info("fight.c => onehit_kill_check", NULL, NULL, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );
	if ( mob == ch )
		return;

	for ( vch = mob->in_room->people; vch; vch = vch->next_in_room )
		if ( vch != mob && is_same_group( vch, mob ) && !vch->fighting && IS_NPC(vch) && !IS_AFFECTED(vch, AFF_CHARM ) )
			vch->fighting = ch;
	return;
}

void reorganize_mobile_group( CHAR_DATA *mob )
{
	CHAR_DATA * group = NULL, *vch;

	/* najpierw sprawdzamy czy ktos mial leader na mob */
	for ( vch = mob->in_room->people; vch; vch = vch->next_in_room )
	{
		if ( vch->leader == mob )
		{
			if ( mob->leader )
			{
				vch->leader = mob->leader;
				vch->master = mob->master;
			}
			else
			{
				if ( !group )
				{
					group = vch;
					continue;
				}
				else
				{
					vch->leader = group;
					vch->master = group;
				}
			}
		}
	}
}

/* zamiast makra */
int GET_AC( CHAR_DATA *ch, int type )
{
	OBJ_DATA /** weapon,*/ *shield;
	AFFECT_DATA *paf;
	int armor = 100;
	//int twohander_style = 0;

	/* dla eterycznego trzeba przeliczac AC ciut inaczej
	   bazowo AC 1 plus zrecznosc i czarki
	   nie licza si� bonusy na gratach...z lenistwa
	 */
	if ( IS_AFFECTED( ch, AFF_ETHEREAL_ARMOR ) )
	{
		armor = 10;
	}
	else if ( is_affected( ch, gsn_steel_scarfskin ) )
	{
 	        armor = UMIN( armor, 30 );
	}
	else if ( IS_AFFECTED( ch, AFF_ARMOR ) )
	{
	        armor = UMIN( armor, 60 );
	}
	else if ( IS_AFFECTED( ch, AFF_IMMOLATE ) )
	{
                armor = UMIN( armor, 95 );

		for ( paf = ch->affected; paf; paf = paf->next )
			if ( paf->type == gsn_immolate )
			{
				armor -= paf->modifier;
				break;
			}
	}

	for ( paf = ch->affected; paf; paf = paf->next )
	   if ( paf->location == APPLY_AC )
	      armor += paf->modifier;

	armor = armor < ch->armor[ type ] ? armor : ch->armor[ type ];

	if ( ( shield = get_eq_char( ch, WEAR_SHIELD ) ) != NULL )
	{
		if(	shield->item_type == ITEM_SHIELD )
		{
			//rellik, przerabiam bo troch� sztywne by�o (tarcza ac 4 i 5 by�y takie same), [20080507]
			armor -= URANGE( 10, shield->value[1]*7, 40 ) ;
		}
        else if ( shield->item_type == ITEM_WEAPON )
        {
			armor -= 10;
		}
	}

	armor += ( IS_AWAKE( ch ) ? dex_app[ get_curr_stat_deprecated( ch, STAT_DEX ) ].defensive : 0 );

	/*
	   if ( ( weapon = get_eq_char( ch, WEAR_WIELD ) ) != NULL &&
	   IS_WEAPON_STAT( weapon, WEAPON_TWO_HANDS) &&
	   ( twohander_style = get_skill( ch, gsn_twohander_fighting ) ) > 0 )
	   armor -= ( ch->class == CLASS_BARBARIAN ? 40 : 30 ) * twohander_style / 100;
	 */

	if ( ( IS_NPC( ch ) && EXT_IS_SET( ch->act, ACT_MONK ) ) ||
			( !IS_NPC( ch ) && ch->class == CLASS_MONK ) )
		armor -= 10 * wis_app[ get_curr_stat_deprecated( ch, STAT_WIS ) ].mod;

	return armor;
}

void check_armor_spell( CHAR_DATA *victim, int dam )
{
	AFFECT_DATA * paf;
	if ( !IS_AFFECTED( victim, AFF_ARMOR ) )
		return;

	for ( paf = victim->affected; paf; paf = paf->next )
		if ( paf->bitvector == &AFF_ARMOR &&
				dam > paf->modifier )
		{
			if ( skill_table[ paf->type ].msg_off && skill_table[ paf->type ].msg_off != '\0' )
			{
				send_to_char( skill_table[ paf->type ].msg_off, victim );
				send_to_char( "\n\r", victim );
			}

			affect_remove( victim, paf );
			break;
		}


	return;
}

void make_blood( CHAR_DATA *ch )
{
	OBJ_DATA *obj;

	for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
		if ( obj->pIndexData->vnum == OBJ_VNUM_SPILLED_BLOOD )
			break;

	if ( !obj)
	{
		obj = create_object( get_obj_index( OBJ_VNUM_SPILLED_BLOOD ), FALSE );
		obj->timer	= number_range( 12, 16 );
		obj->value[0] = 5;
		obj->value[1] = 5;
		obj_to_room( obj, ch->in_room );
	}
	else
	{
		obj->timer += number_range( 2, 5 );
		obj->timer = UMIN( 16, obj->timer );
		obj->value[0] += 5;
		obj->value[1] += 5;
		obj->value[0] = UMIN( 40, obj->value[0] );
		obj->value[1] = UMIN( obj->value[1], obj->value[0] );
	}
	return;
}

void make_brains( CHAR_DATA *ch )
{
	OBJ_DATA *obj;

	obj = create_object( get_obj_index( OBJ_VNUM_BRAINS ), FALSE);
	obj->timer	= number_range( 12, 24 );
	obj_to_room( obj, ch->in_room );
}

/* kretynstwo do robiebia glowki w przypadku dekapitacji*/
void make_head( CHAR_DATA *ch )
{
	OBJ_DATA * obj;
	char buf[ MIL ], *name;

	name = ch->name2;

	obj = create_object( get_obj_index( OBJ_VNUM_SEVERED_HEAD ), FALSE );
	obj->timer	= number_range( 4, 7 );

	sprintf( buf, obj->short_descr, name );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );

	sprintf( buf, "g�owy %s", ch->name2 );
	free_string( obj->name2 );
	obj->name2 = str_dup( buf );

	sprintf( buf, "g�owie %s", ch->name2 );
	free_string( obj->name3 );
	obj->name3 = str_dup( buf );

	sprintf( buf, "g�ow� %s", ch->name2 );
	free_string( obj->name4 );
	obj->name4 = str_dup( buf );

	sprintf( buf, "g�ow� %s", ch->name2 );
	free_string( obj->name5 );
	obj->name5 = str_dup( buf );

	sprintf( buf, "g�owie %s", ch->name2 );
	free_string( obj->name6 );
	obj->name6 = str_dup( buf );


	sprintf( buf, obj->description, name );
	free_string( obj->description );
	obj->description = str_dup( buf );
	obj->item_type = ITEM_TRASH;
	obj->value[5] = -17;//by nie bylo dwoch glow w przypadku bounty hunting
	obj_to_room( obj, ch->in_room );
}

void make_leg( CHAR_DATA *ch )
{
	OBJ_DATA * obj;
	char buf[ MIL ], *name;

	name = ch->name2;

	obj = create_object( get_obj_index( OBJ_VNUM_SLICED_LEG ), FALSE );
	obj->timer	= number_range( 4, 7 );

	sprintf( buf, obj->short_descr, name );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );

	sprintf( buf, "nogi %s", ch->name2 );
	free_string( obj->name2 );
	obj->name2 = str_dup( buf );

	sprintf( buf, "nodze %s", ch->name2 );
	free_string( obj->name3 );
	obj->name3 = str_dup( buf );

	sprintf( buf, "nog� %s", ch->name2 );
	free_string( obj->name4 );
	obj->name4 = str_dup( buf );

	sprintf( buf, "nog� %s", ch->name2 );
	free_string( obj->name5 );
	obj->name5 = str_dup( buf );

	sprintf( buf, "nodze %s", ch->name2 );
	free_string( obj->name6 );
	obj->name6 = str_dup( buf );


	sprintf( buf, obj->description, name );
	free_string( obj->description );
	obj->description = str_dup( buf );
	obj->item_type = ITEM_TRASH;
	obj_to_room( obj, ch->in_room );
}

void make_arm( CHAR_DATA *ch )
{
	OBJ_DATA * obj;
	char buf[ MIL ], *name;

	name = ch->name2;

	obj = create_object( get_obj_index( OBJ_VNUM_SLICED_ARM ), FALSE );
	obj->timer	= number_range( 4, 7 );

	sprintf( buf, obj->short_descr, name );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );

	sprintf( buf, "r�ki %s", ch->name2 );
	free_string( obj->name2 );
	obj->name2 = str_dup( buf );

	sprintf( buf, "r�ce %s", ch->name2 );
	free_string( obj->name3 );
	obj->name3 = str_dup( buf );

	sprintf( buf, "r�k� %s", ch->name2 );
	free_string( obj->name4 );
	obj->name4 = str_dup( buf );

	sprintf( buf, "r�k� %s", ch->name2 );
	free_string( obj->name5 );
	obj->name5 = str_dup( buf );

	sprintf( buf, "r�ce %s", ch->name2 );
	free_string( obj->name6 );
	obj->name6 = str_dup( buf );


	sprintf( buf, obj->description, name );
	free_string( obj->description );
	obj->description = str_dup( buf );
	obj->item_type = ITEM_TRASH;
	obj_to_room( obj, ch->in_room );
}

int parry_mod( OBJ_DATA *weapon )
{
	if ( !weapon || weapon->item_type != ITEM_WEAPON )
    {
		return 0;
    }

	switch ( weapon->value[ 0 ] )
	{
		case WEAPON_SWORD: return 10;
		case WEAPON_SHORTSWORD: return 5;
		case WEAPON_AXE:
					if ( IS_SET( weapon->value[ 4 ], WEAPON_TWO_HANDS ) )
						return -10;
					else
						return -50;
		case WEAPON_DAGGER:
		case WEAPON_CLAWS: return -20;
		case WEAPON_SPEAR:
		case WEAPON_POLEARM:
		case WEAPON_STAFF:
				    if ( IS_SET( weapon->value[ 4 ], WEAPON_TWO_HANDS ) )
					    return 10;
				    else
					    return -30;
		case WEAPON_MACE: return -40;
		case WEAPON_FLAIL: return -60;
		case WEAPON_WHIP: return -60;
		default: return 0;
	}
	return 0;
}

bool special_damage_message(CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *wield, int dam, int dt, bool immune)
{
	save_debug_info("fight.c => special_damage_message", NULL, NULL, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );
	if ( dt != gsn_backstab &&
			dt != gsn_charge &&
			dt != gsn_kick &&
			dt != gsn_stun &&
			dt != gsn_trip &&
			dt != gsn_smite &&
			dt != gsn_smite_good &&
			dt != gsn_crush &&
			dt != gsn_tail)
		return FALSE;

	if( IS_AFFECTED(ch,AFF_DEFLECT_WOUNDS)) dam /= 2;

	//lata, jesli backstab idzie z damage, wtedy nie podaje ptr broni
	if ( !wield )
		wield = get_eq_char( ch, WEAR_WIELD );

	/* KICK TEXT */
	if ( dt == gsn_kick )
	{
		if ( immune )
		{
			act( "{5Odsuwasz si� lekko od $Z i kopiesz $M tak mocno jak mo�esz, jednak $E nawet tego nie zauwa�a.{x", ch, NULL, victim, TO_CHAR );
			act( "{5$n odsuwa si� lekko od ciebie i kopie ci� tak mocno jak tylko mo�e, jednak nawet tego nie zauwa�asz.{x", ch, NULL, victim, TO_VICT );
			act( "{5$n odsuwa si� od $Z i kopie tak mocno jak tylko mo�e, jednak $E nawet tego nie zauwa�a.{x", ch, NULL, victim, TO_NOTVICT );
			return TRUE;
		}
		else if ( dam <= 0 )
		{
			switch ( number_range( 0, 3 ) )
			{
				case 0:
					act( "{5Bierzesz zamach i pr�bujesz kopn�� $C, ale wyra�nie ci to nie wychodzi, chybiasz $M chyba o p� metra.{x", ch, NULL, victim, TO_CHAR );
					act( "{5$x wyra�nie nie wychodzi kopniak, chybia ci� o jakies p� metra.{x", ch, NULL, victim, TO_VICT );
					act( "{5$x wyra�nie nie wychodzi kopniak, chybia $C o jakie� po� metra.{x", ch, NULL, victim, TO_NOTVICT );
					break;

				case 1:
					act( "{5Wyprowadzasz prostego kopniaka, w tym czasie jednak $N robi lekki unik i cios ze�lizguje si� po $S ciele.{x", ch, NULL, victim, TO_CHAR );
					act( "{5$n wyprowadza kopniaka, lekki unik jednak wystarcza, aby potencjalnie gro�ny cios ze�lizn�� ci si� bez szwanku po ciele.{x", ch, NULL, victim, TO_VICT );
					act( "{5$n wyprowadza kopniaka, lekki unik jednak wystarcza, aby potencjalnie gro�ny cios ze�lizn�� si� bez szwanku po ciele $Z.{x", ch, NULL, victim, TO_NOTVICT );
					break;

				case 2:
					act( "{5Pr�bujesz kopn�� $C, ale tracisz na sekund� r�wnowag� i chybiasz haniebnie.{x", ch, NULL, victim, TO_CHAR );
					act( "{5$n pr�buje ci� kopn��, ale traci na sekund� r�wnowag� i chybia haniebnie.{x", ch, NULL, victim, TO_VICT );
					act( "{5$n pr�buje kopn�� $C, ale traci na sekund� r�wnowag� i chybia haniebnie.{x", ch, NULL, victim, TO_NOTVICT );
					break;

				case 3:
					act( "{5Nie wiesz jak ci si� to uda�o, ale pr�buj�c kopn�� $C prawie $t obr�t o p� osi.{x", ch, ch->sex == 2 ? "zrobi�a�" : "zrobi�e�", victim, TO_CHAR );
					act( "{5$n robi co� przedziwnego, do kopniaka bierze tak t�gi zamach, �e kiedy nie trafia obraca si� o p� osi i staje do ciebie na u�amek sekundy plecami.{x", ch, NULL, victim, TO_VICT );
					act( "{5$n robi co� przedziwnego, do kopniaka bierze tak t�gi zamach, �e kiedy nie trafia obraca si� o p� osi i staje do $Z na u�amek sekundy plecami.{x", ch, NULL, victim, TO_NOTVICT );
					break;
			}
			return TRUE;
		}
        else if ( victim->hit - dam < -11 )
        {
            if ( !( IS_NPC( victim ) && IS_SET( victim->form, FORM_CONSTRUCT ) ) )
            {
                switch ( number_range( 0, 2 ) )
                {
                    case 0:
                        act( "{5Pr�bujesz wyprowadzi� kopniak, zatrzymujesz na chwil� nog� w powietrzu po czym uderzasz $C. Cios by� skuteczny jak diabli, $N pada na ziemi�, rz�zi chwil� i umiera.{x", ch, NULL, victim, TO_CHAR );
                        act( "{5$n pr�buje chyba wyprowadzi� kopniak, zawiesza jednak na chwil� noge w powietrzu jakby $t zamiar po czym kopie ci� prosto w krta�. �wiat wiruje ci przed oczyma, padasz na ziemi� i umierasz.{x", ch, ch->sex == 2 ? "zmieni�a" : "zmieni�", victim, TO_VICT );
                        act( "{5$n pr�buje chyba wyprowadzi� kopniak, zawiesza jednak na chwil� nog� w powietrzu jakby $t zamiar po czym kopie $C. Cios okazuje si� niezwykle skuteczny, $N pada na ziemi�, rz�zi chwil� i umiera.{x", ch, ch->sex == 2 ? "zmieni�a" : "zmieni�", victim, TO_NOTVICT );
                        break;

                    case 1:
                        act( "{5Kopiesz $C wk�adaj�c w to ca�� si��. $N pada na ziemi� zupe�nie oszo�omion$R, dostrzegasz tylko przez moment gasn�ce w $Q �ycie, a ju� po sekundzie jest po wszystkim.{x", ch, NULL, victim, TO_CHAR );
                        act( "{5$n kopie ci� pot�nie, przewracasz si� na ziemi� i tracisz oddech, oczy zachodz� ci mg��, umierasz.{x", ch, NULL, victim, TO_VICT );
                        act( "{5$n kopie $C wk�adaj�c w to chyba ca�� si��. $N pada zupe�nie oszo�omion$R, dostrzegasz tylko przez moment gasn�ce w $Q �ycie, a ju� po sekundzie jest po wszystkim.{x" , ch, NULL, victim, TO_NOTVICT );
                        break;

                    case 2:
                        act( "{5$N pr�buje robi� jakie� uniki, dostaje jednak solidnego kopniaka nie bed�c na to w og�le $t. S�ycha� dono�ny chrupot po czym $N pada na ziemi�, zwija si� chwil� z b�lu i nieruchomieje.{x", ch, victim->sex == 2 ? "przygotowana" : "przygotowany", victim, TO_CHAR );
                        act( "{5Dostajesz pot�nego kopniaka od $z prosto w splot s�oneczny, czujesz chrupot p�kaj�cych �eber, chyba jakie� przebija twoje serce... umierasz w jednej chwili.{x", ch, NULL, victim, TO_VICT );
                        act( "{5$N pr�buje robi� uniki, nawet mu to wychodzi, po chwili jednak dostaje od $c naprawd� solidnego kopniaka, s�ycha� tylko dono�ny chrupot p�kaj�cych ko�ci, wali si� bez czucia na ziemi� i umiera.{x", ch, NULL, victim, TO_NOTVICT );
                        break;
                }
            }
            else
            {
                        act( "{5$N dostaje solidnego kopniaka. S�ycha� dono�ny chrupot po czym $N rozpada si� na kawlki.{x", ch, NULL, victim, TO_ROOM );
                        act( "{5$N pr�buje robi� uniki, nawet mu to wychodzi, po chwili jednak dostaje od $c naprawd� solidnego kopniaka, s�ycha� tylko dono�ny chrupot p�kaj�cych ko�ci, wali si� bez czucia na ziemi� i umiera.{x", ch, NULL, victim, TO_NOTVICT );
                act( "$n rozpada sie na kawleczki", victim, 0, 0, TO_ROOM );
            }
			return TRUE;
		}
		else
		{
			if (victim->position < POS_FIGHTING )
			{
				act( "{5Kopiesz $C z ca�ej si�y.{x",   ch, NULL, victim, TO_CHAR );
				act( "{5$n kopie ci� z ca�ej si�y.{x", ch, NULL, victim, TO_VICT );
				act( "{5$n kopie $C z ca�ej si�y.{x",  ch, NULL, victim, TO_NOTVICT );
			}
			else
			{
				switch ( number_range( 0, 3 ) )
				{
					case 0:
						act( "{5Wyczekujesz odpowiedniego momentu i kopiesz $C z ca�ej si�y.{x", ch, NULL, victim, TO_CHAR );
						switch (victim->sex)
						{
							case 0:
								act( "{5$x udaje si� znale�� odpowiedni moment, kiedy jeste� ods�oni�te i kopie ci� z ca�ej si�y.{x", ch, NULL, victim, TO_VICT );
								act( "{5$x udaje si� znale�� odpowiedni moment, kiedy $N jest ods�oni�te i kopie $M z ca�ej si�y.{x", ch, NULL, victim, TO_NOTVICT );
								break;
							case 1:
								act( "{5$x udaje si� znale�� odpowiedni moment, kiedy jeste� ods�oni�ty i kopie ci� z ca�ej si�y.{x", ch, NULL, victim, TO_VICT );
								act( "{5$x udaje si� znale�� odpowiedni moment, kiedy $N jest ods�oni�ty i kopie $M z ca�ej si�y.{x", ch, NULL, victim, TO_NOTVICT );
								break;
							case 2:
							default:
								act( "{5$x udaje si� znale�� odpowiedni moment, kiedy jeste� ods�oni�ta i kopie ci� z ca�ej si�y.{x", ch, NULL, victim, TO_VICT );
								act( "{5$x udaje si� znale�� odpowiedni moment, kiedy $N jest ods�oni�ta i kopie $M z ca�ej si�y.{x", ch, NULL, victim, TO_NOTVICT );
								break;
						}
						break;

					case 1:
						act( "{5Odchodzisz lekko od $Z i kopiesz $M tak mocno jak tylko mo�esz.{x", ch, NULL, victim, TO_CHAR );
						act( "{5$n odchodzi lekko od ciebie i kopie ci� tak mocno jak tylko mo�e.{x", ch, NULL, victim, TO_VICT );
						act( "{5$n odchodzi lekko od $Z i kopie $M tak mocno jak tylko mo�e.{x", ch, NULL, victim, TO_NOTVICT );
						break;

					case 2:
						act( "{5Czekasz chwil� a� $N si� troch� ods�oni i kiedy to si� staje kopiesz $M.{x", ch, NULL, victim, TO_CHAR );
						act( "{5$n chyba na co� czeka, przez przypadek ods�aniasz si� i po chwili dostajesz $s solidnym kopniakiem.{x", ch, victim->sex == 2 ? "pocz�stowana" : "pocz�stowany", victim, TO_VICT );
						act( "{5$n obserwuje uwa�nie $C jakby na co� czekaj�c i solidnie $M kopie kiedy $t si� ods�ania.{x", ch, victim->sex == 2 ? "tamta" : "tamten", victim, TO_NOTVICT );
						break;

					case 3:
						act( "{5Ooo, udaje ci si� wyprowadzi� niez�y cios, kopiesz $C z p�obrotu. Efektownie, efektownie, no no.{x", ch, NULL, victim, TO_CHAR );
						act( "{5Na twoje nieszcz�cie $x wychodzi ca�kiem niez�y cios. Niespecjalnie wa�ne jest jak $o si� to uda�o, istotne jest solidne trafienie z p�obrotu w z�by. Ouuuuuuu���!!{x", ch, NULL, victim, TO_VICT );
						act( "{5$x udaje si� wyprowadzi� naprawd� niez�y, efektowny cios. Trafia $C wykopem z p�obrotu.{x", ch, wield, victim, TO_NOTVICT );
						break;
				}
			}
			return TRUE;
		}
	}
	/* BACKSTAB OPISY */
	else if ( dt == gsn_backstab && wield )
	{
		if ( immune )
		{
			act( "{5Zachodzisz $C od ty�u i wbijasz $O $p na wysoko�ci bark�w prawie po r�koje��, jednak $E nie reaguje na to.{x", ch, wield, victim, TO_CHAR );
			act( "{5$n zachodzi ci� od ty�u i wbija ci $p w plecy, jednak nie reagujesz na to.{x", ch, wield, victim, TO_VICT );
			act( "{5$n zachodzi $C od ty�u i wbija $O $p w plecy, jednak $E nie reaguje na to.{x", ch, wield, victim, TO_NOTVICT );
			return TRUE;
		}
		else if ( dam <= 0 )
		{//opisy specjalne dla potworow o duzym cielsku (ale nie bardzo duzym like smoki)
			if( !str_cmp( race_table[ GET_RACE(victim) ].name, "wywerna" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "hydra" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "mantykora" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "smok" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "bazyliszek" ))
			{
				act( "{5Staraj�c si� zachowa� ostro�no�� podchodzisz od ty�u do $Z. Niestety, z t� ostro�no�ci� nie do ko�ca si� uda�o, niedosz�a ofiara zauwa�a ci� kontem �lepia, odwraca si� i atakuje.{x", ch, wield, victim, TO_CHAR );
				act( "{5$n staraj�c si� zachowa� wszelkie �rodki ostro�no�ci podchodzi od ty�u to $Z. Z t� ostro�no�ci� jednak nie do ko�ca $m wysz�o, niedosz�a ofiara zauwa�a $s kontem �lepia, odwraca si� i atakuje.{x", ch, wield, victim, TO_NOTVICT );
				return TRUE;
			}

			switch ( number_range( 0, 3 ) )
			{
				case 0:
					act( "{5Bezszelestnie zakradasz si� do $Z, chyba jednak nie uwa�asz nadto dobrze. $N dostrzega ci�, uchyla si� i unika twojego ciosu.{x", ch, wield, victim, TO_CHAR );
					act( "{5Dostrzegasz $c zakrad�jacego si� w twoim kierunku z jakim� ostrzem w r�ku, robisz b�yskawiczny unik i $s cios tnie powietrze.{x", ch, wield, victim, TO_VICT );
					act( "{5Widzisz jak $n zakrada si� do $Z z jakim� ostrzem w r�ku, $t jednak robi b�yskawiczny unik i cios tnie powietrze.{x", ch, victim->sex == 2 ? "ta" : "ten", victim, TO_NOTVICT );
					break;

				case 1:
					act( "{5Podchodzisz do $Z od ty�u i pr�bujesz $M d�gn�� $j, jednak nie do�� mocno go chwytasz i ostrze niegro�nie ze�lizguje si� po $S plecach.{x", ch, wield, victim, TO_CHAR );
					act( "{5Otrzymujesz cios w plecy, jednak ostrze ze�lizguje ci si� tylko po nich. Co za szcz�cie, $n tym razem nie $t.{x", ch, ch->sex == 2 ? "trafi�a" : "trafi�", victim, TO_VICT );
					act( "{5$n podchodzi do $Z od ty�u, zamierza si� z jakims ostrzem do ciosu i zadaje go. �le chyba jednak trzyma $h, bo ostrze niegro�nie ze�lizguje si� po plecach $Z.{x", ch, wield, victim, TO_NOTVICT );
					break;

				case 2:
					act( "{5Zachodzisz $C po ma�ym �uku od ty�u, robisz wypad i pr�bujesz d�gn�� $j, jednak trafiasz $M tylko lekko w rami�.{x", ch, wield, victim, TO_CHAR );
					act( "{5Czujesz dra�ni�cie w lewe rami�, kto� pr�bowa� chyba d�gn�� ci� w�asnie w plecy. Odwracasz lekko g�ow� i dostrzegasz $c.{x", ch, wield, victim, TO_VICT );
					act( "{5$n zachodzi $C po ma�ym �uku od ty�u, robi wypad i pr�buje d�gn�� $M $j, jednak trafia tylko w lewe rami�.{x", ch, wield, victim, TO_NOTVICT );
					break;

				case 3:
					act( "{5W kilku skokach pr�bujesz pokona� odleg�o�� do $Z i d�gn�� $M $j, jednak potykasz si� i zamiast tego wpadasz $O na plecy.{x", ch, wield, victim, TO_CHAR );
					switch( NOPOL(wield->name5[0]) )
					{
						case 'Z':
						case 'S':
							act( "{5S�yszysz kroki za plecami a po chwili kto� wpada na ciebie. Odwracasz si� i dostrzegasz $c ze $j w r�ku. Tym razem nie uda�o si� $o zada� ciosu.{x", ch, wield, victim, TO_VICT );
							act( "{5Widzisz jak $n w paru skokach podbiega do $Z, ze $j w r�ku chc�c $M d�gn��. Jednak potyka si� i zamiast tego wpada $O na plecy.{x", ch, wield, victim, TO_NOTVICT );
							break;
						default:
							act( "{5S�yszysz kroki za plecami a po chwili kto� wpada na ciebie. Odwracasz si� i dostrzegasz $c z $j w r�ku. Tym razem nie uda�o si� $o zada� ciosu.{x", ch, wield, victim, TO_VICT );
							act( "{5Widzisz jak $n w paru skokach podbiega do $Z, z $j w r�ku chc�c $M d�gn��. Jednak potyka si� i zamiast tego wpada $O na plecy.{x", ch, wield, victim, TO_NOTVICT );
							break;
					}
					break;
			}
			return TRUE;
		}
		else if ( victim->hit - dam < -11 )
		{
			if ( victim->position <= POS_SLEEPING )
			{
				act( "{5Podchodzisz do $Z, �ciskasz mocno $p i z ca�ej si�y wbijasz swoj� bro� w $S plecy po sam� rekoje��. Cia�o $Z wypr�a si� i po chwili nieruchomieje.{x", ch, wield, victim, TO_CHAR );
				act( "{5Przeszywa ci� straszny b�l. Przez sparali�owany nim m�zg przebija si� jedna my�l. Kto� chyba d�gn�� ci� w plecy. Nie starcza ci czasu �eby pomy�le� o czym� wi�cej. Umierasz.{x", ch, wield, victim, TO_VICT );
				act( "{5$n podchodzi do $Z �ciskaj�c mocno $p i z ca�ej si�y wbija swoj� bro� w $S plecy po sam� r�koje��. Cia�o $Z wypr�a si� i po chwili nieruchomieje.{x", ch, wield, victim, TO_NOTVICT );
				return TRUE;
			}

			if ( IS_SET( race_table[ GET_RACE(victim) ].type, ANIMAL ) )
			{
				act( "{5Zakradasz si� od ty�u do $Z i d�gasz tam, gdzie jak s�dzisz znajduje si� serce. Przera�liwe charczenie towarzyszy ostatnim podrygom $Z. W cokolwiek trafi�e�, cios okaza� si� zab�jczy.{x", ch, wield, victim, TO_CHAR );
				act( "{5$n zakrada si� od ty�u do $Z i mocno d�ga. Ostatnim podrygom $Z towarzyszy przera�liwe charczenie. W cokolwiek $n trafi�, cios okaza� si� zab�jczy.{x", ch, wield, victim, TO_NOTVICT );
				return TRUE;
			}
			//opisy specjalne dla humanoidalnych potworow duzych rozmiarow
			if( !str_cmp( race_table[ GET_RACE(victim) ].name, "ogr" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "troll" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "umberkolos" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "yeti" ))
			{
				if ( ch->size == SIZE_SMALL ||
						ch->size == SIZE_TINY )
				{
					act( "{5Wyj�tkowo dobrze uda�o ci si� zgra� swoje ruchy. Pojawiasz si� za ci�ko ju� rannym przeciwnikiem i z ca�ej si�y wbijasz mu $h w prawe udo, po czym pomagaj�c sobie drug� r�k� przeci�gasz otrze w d� rozrywaj�c �ci�gna kolana i mi�nie �ydki. Cios ten dope�ni� los $Z, wali si� $E na ziemi� i wykrwawia na �mier�.{x", ch, wield, victim, TO_CHAR );
					act( "{5$n pojawia si� b�yskawicznie za ci�ko ju� rannym przeciwnikiem i z ca�ej si�y wbija mu $h w prawe udo, po czym pomagaj�c sobie drug� r�k� przeci�ga ostrze w d� rozrywaj�c kolana i mi�nie �ydki. Cios ten dope�ni� los $Z, wali si� $E na ziemi� i wykrwawia na �mier�.{x" , ch, wield, victim, TO_NOTVICT );
				}
				else
				{
					act( "{5Zakradasz si� do $Z od ty�u zupe�nie nie zauwazon<&y/a/e>. Z satysfakcj� przystajesz na chwil�, �ciskasz mocno r�koje�� $f, wyci�gasz r�k� w g�r� by si�gn�� wysoko�ci serca potwora i wbijasz w $S plecy sw�j or� tak g��boko jak tylko mo�esz. $N krztusi si� jaki� czas, po czym pada g�ucho jak spr�chnia�a k�oda na ziemi�.{x", ch, wield, victim, TO_CHAR );
					act( "{5$n zakrada si� cichutko do $Z, przystaje na chwil�, wyci�ga r�k� w g�r� i d�ga z ca�ej si�y w $S plecy na wysoko�ci serca potwora. $N charczy przez chwil� i nagle w zupe�nej ciszy osuwa si� martw$R na ziemi�.{x" , ch, wield, victim, TO_NOTVICT );
				}
				return TRUE;
			}
			//opisy specjalne dla potworow o duzym cielsku (ale nie bardzo duzym like smoki)
			if( !str_cmp( race_table[ GET_RACE(victim) ].name, "wywerna" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "hydra" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "smok" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "mantykora" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "bazyliszek" ))
			{
				act( "{5Ostro�nie podchodzisz do zaj�tego teraz czym� innym potwora, bierzesz zamach i wykorzystuj�c ca�� sw� si�� wbijasz $h w jego cielsko. Musia�<&e�/a�>o�> trafi� w jaki� czu�y punkt $Z, gdy� wydaje $E z siebie przera�liwy kwik, a z niewielkiej rany bucha fontanna krwi. Monstrum jeszcze przez chwil� t�ucze swym opas�ym cielskiem o ziemi� i stara si� wyra�nie zrobi� ci krzywdn�, lecz nic z tego nie wychodzi. Po chwili �lepia $Z gasn� na wieki.{x", ch, wield, victim, TO_CHAR );
				act( "{5$n ostro�nie podchodzi do zaj�tego teraz czym� innym potwora, bierze zamach i wykorzystuj�c ca�� sw� si�� wbija $h w jego cielsko. Widocznie or� wbi� si� w jaki� czu�y punkt $Z, gdy� wydaje $E z siebie przera�liwy kwik, a z niewielkiej rany bucha fontanna krwi. Monstrum jeszcze przez chwil� t�ucze swym opas�ym cielskiem o ziemi� i stara sie wyra�nie zrobi� $x krzywd�, lecz gasn�ce powoli �lepia �wiadcz�, �e nic z tego nie wyjdzie.{x", ch, wield, victim, TO_NOTVICT );
				return TRUE;
			}

			switch ( number_range( 0, 3 ) )
			{
				case 0:
					act( "{5Wyj�tkowo dobrze uda�o ci si� zgra� swoje ruchy. Pojawiasz si� za plecami $Z jak b�yskawica i trafiasz $M $j mi�dzy �opatki. Po chwili $S zwiotcza�e cia�o osuwa si� po tobie na ziemi�.{x", ch, wield, victim, TO_CHAR );
					act( "{5Niespodziewanie otrzymujesz bardzo silny cios w plecy. �wiat wiruje ci przed oczyma. Dostrzegasz ostatnim spojrzeniem czyj�� twarz... tttooo chhhhhybbbaa $n. Hrrhrhrh...{x", ch, wield, victim, TO_VICT );
					act( "{5$N nagle otrzymuje od $z bardzo silne d�gniecie $j w plecy i osuwa si� powoli po $s ramionach ostatnim spojrzeniem dostrzegaj�c twarz zab�jcy.{x" , ch, wield, victim, TO_NOTVICT );
					break;

				case 1:
					act( "{5Zakradasz si� do $Z od ty�u zupe�nie nie zauwazon$r. Z satysfakcj� przystajesz na chwil�, �ciskasz mocno r�koje�� $f i d�gasz $M w plecy na wysoko�ci serca. $N pada g�ucho jak spr�chnia�a k�oda na ziemi�.{x", ch, wield, victim, TO_CHAR );
					act( "{5Przeszywa ci� straszny b�l. Przez sparali�owany nim m�zg przebija si� jedna my�l. Kto� chyba d�gn�� ci� w plecy. Nie starcza ci czasu �eby pomy�le� o czym� wi�cej. Osuwasz si� martw$R na ziemi�.{x", ch, wield, victim, TO_VICT );
					switch (ch->sex)
					{
						case 0:
							act( "{5$n zakrada si� nie zauwa�one do $Z, przystaje na chwil� i d�ga z ca�ej si�y w plecy. $N charczy przez chwil� i nagle w zupe�nej ciszy osuwa si� martw$R na ziemi�.{x" , ch, wield, victim, TO_NOTVICT );
							break;
						case 2:
							act( "{5$n zakrada si� nie zauwa�ona do $Z, przystaje na chwil� i d�ga z ca�ej si�y w plecy. $N charczy przez chwil� i nagle w zupe�nej ciszy osuwa si� martw$R na ziemi�.{x" , ch, wield, victim, TO_NOTVICT );
							break;
						default:
							act( "{5$n zakrada si� nie zauwa�ony do $Z, przystaje na chwil� i d�ga z ca�ej si�y w plecy. $N charczy przez chwil� i nagle w zupe�nej ciszy osuwa si� martw$R na ziemi�.{x" , ch, wield, victim, TO_NOTVICT );
							break;
					}
					break;

				case 2:
					act( "{5Podbiegasz w kilku skokach do $Z nie zwracaj�c uwagi na to, �e $E dostrzeg$U ci� przez rami�. Wbijasz $O z ca�ej si�y $p w plecy. $N �apie ci� jedn� r�k� za g�ow� a drug� rozdrapuje ci twarz. Osuwa si� powoli i po chwili ju� nie �yje.{x", ch, wield, victim, TO_CHAR );
					act( "{5K�tem oka dostrzegasz jak $n podbiega w kilku skokach do ciebie trzymaj�c $p w r�ku. Zaczynasz si� odwraca� w $s stron�, ale nie zd��asz. $n z ca�ej si�y wbija ci ostrze w plecy. �apiesz go jedn� r�k� za g�ow� a drug� rozdrapujesz $o twarz, jednak na nic wi�cej nie masz ju� si�y. Padasz na ziemi� i umierasz!{x", ch, wield, victim, TO_VICT );
					act( "{5Dostrzegasz jak $n podbiega w kilku skokach do $Z trzymaj�c $p w r�ku. $N zaczyna si� odwraca� w $s stron�, ale nie zd��a. $n z ca�ej si�y wbija $O ostrze w plecy. $N jeszcze �apie $z jedn� r�ka za g�ow� a drug� rozdrapuje $o twarz, jednak na nic wi�cej nie ma si�y. Pada na ziemi� i umiera!{x" , ch, wield, victim, TO_NOTVICT );
					break;

				case 3:
					act( "{5W paru sprawnych, kocich ruchach zakradasz si� do $Z i ustawiasz si� za $S plecami. Szybko wyci�gasz $p i mierz�c moment d�gasz tu� przy szyi, wbijaj�c ostrze prawie po sam� r�koje��. $N harczy, toczy troch� piany z ust i umiera u twoich st�p.{x", ch, wield, victim, TO_CHAR );
					act( "{5Z realnego �wiata wyrywa ci� potworny, przeszywaj�cy od st�p do g��w b�l. Kto� d�gn�� ci� pot�nie w okolicach szyi jakim� ostrym narz�dziem. Harczysz tocz�c pian� z ust i osuwasz si�. Kiedy padasz na ziemi� twoje gasn�ce oczy dostrzegaj� twarz $z.{x", ch, wield, victim, TO_VICT );
					act( "{5$n paroma szybkimi, kocimi ruchami zbli�a si� do $Z i ustawia si� za $S plecami. Wyci�ga szybko $p i mierz�c przez moment d�ga tu� przy szyi, wbijaj�c ostrze prawie po sam� r�koje��. $N harczy, toczy troch� piany z ust i oddaje ducha u stop $z.{x", ch, wield, victim, TO_NOTVICT );
					break;
			}
			return TRUE;
		}
		else
		{

			if ( IS_SET( race_table[ GET_RACE(victim) ].type, ANIMAL ) )
			{
				act( "{5Zakradasz si� od ty�u do $Z i d�gasz tam, gdzie jak s�dzisz znajduje si� serce.{x", ch, wield, victim, TO_CHAR );
				act( "{5$n zakrada si� od ty�u do $Z i mocno d�ga.{x", ch, wield, victim, TO_NOTVICT );
				return TRUE;
			}
			//opisy specjalne dla humanoidalnych potworow duzych rozmiarow
			if( !str_cmp( race_table[ GET_RACE(victim) ].name, "ogr" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "troll" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "umberkolos" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "yeti" ))
			{
				if ( ch->size == SIZE_SMALL ||
						ch->size == SIZE_TINY )
				{
					switch(number_range(1,2))
					{
						case 1:
							act( "{5Staraj�c si� porusza� jak najciszej zachodzisz $C od ty�u, a gdy $E nie zauwa�a twojej obecno�ci, wbijasz $h g��boko w $S nog� uszkadzaj�c staw kolanowy. $N wydaje z siebie cichy i �a�osny j�k b�lu, a $S ko�czyna z lekkim chrupotem zgina si� wbrew woli w�a�ciciela, kt�ry ze st�kiem l�duje na ziemi.{x", ch, wield, victim, TO_CHAR );
							act( "{5$n staraj�c si� porusza� jak najciszej zachodzi $C od ty�u, a gdy $E nie zauwa�a $s obecno�ci, wbija $e $h g��boko w $S nog� uszkadzaj�c staw kolanowy. $N wydaje z siebie cichy i �a�osny j�k b�lu, a $S ko�czyna z lekkim chrupotem zgina si� wbrew woli w�a�ciciela, kt�ry ze st�kaniem l�duje na ziemi.{x", ch, wield, victim, TO_NOTVICT );
							WAIT_STATE( victim, 18 ); //1,5 pulse violenca
							victim->position = POS_SITTING;
							break;
						case 2:
							act( "{5Delikatnie skradasz si� do $Z, $E nawet nie podejrzewa, i� mozesz by� za $S plecami. Jednak $S nag�y krok w ty� powoduje, i� o ma�o co nie zosta�<&by�/aby�/oby�> przygniecion<&y/a/e>. Na szcz�cie sprawny odskok ujawnia doskona�y punkt do uderzenia! Nie namy�laj�c si� d�ugo, chwytasz $h w obie r�ce i pot�nym pchni�ciem wbijasz ostrze tu� poni�ej kolana bestii. S�yszysz ohydny mlask i zgrzyt ko�ci gdy $h zg��bia si� w ciele. Wyszarpujesz bro� i cudem unikasz drugiej nogi zmierzaj�cej na spotkanie z twoj� twarz�, po czym w kilku sprawnych skokach wycofujesz si� na bezpieczn� odleg�o��.{x", ch, wield, victim, TO_CHAR );
							act( "{5$n delikatnie skrada si� do $Z, $E widocznie nie podejrzewa, i� mo�e $e znajdowac si� tu� za $S plecami. Nagle $N wykonuje nag�y krok w ty� prawie przygniataj�c $c, $e jednak wykonuje sprawny odskok i chwytaj�c $h w obie r�ce wykonuje mordercze pchni�cie. S�yszysz ohydny mlask i zgrzyt ko�ci gdy $h zg��bia si� w ciele. $n sprawnym ruchem wyszarpuje bro� i odskakuje na bezpieczn� odleg�o��.{x", ch, wield, victim, TO_NOTVICT );
							break;
					}
				}
				else
				{
					switch(number_range(1,2))
					{
						case 1:
							act( "{5Cichym truchtem podbiegasz do niczego nie spodziewaj�cego si� potwora, mierzysz jego plecy bystrym wzrokiem i z nieprawdopodobn� pr�dko�ci� wyskakujesz lekko i wbijasz $h obok kr�gos�upa w miejscu, w kt�rym powinny byc p�uca. Nie trafiasz doskonale, jednak ryk bestii i $p zg�ebiaj�cy si� bez oporu pokazuj�, i� jednak trafi�<&e�/a�/o�> w co� cz�ego. Wyci�gasz bro� i odskakujesz na bezpieczn� odleg�o��.{x", ch, wield, victim, TO_CHAR );
							act( "{5$n cichym truchtem podbiega do niczego nie spodziewaj�cego si� potwora, mierzy jego plecy bystrym wzrokiem i z nieprawdopodobn� pr�dko�ci� wyskakuje lekko i wbija $h obok kr�gos�upa w miejscu, w kt�ym powinny by� p�uca. Nie trafia doskonale, jednak ryk bestii i or� zag��biaj�cy si� bez oporu pokazuje, i� jednak trafiono $C w co� czu�ego. $n wyci�ga $h i odskakuje na bezpieczn� odleg�o��.{x" , ch, wield, victim, TO_NOTVICT );
							break;
						case 2:
							act( "{5Szybkim i ostro�nym krokiem zblizasz sie do $Z, mierzysz $S plecy bacznym spojrzeniem w poszukiwaniu s�abego punktu, kt�ry po chwili odnajdujesz. Wyci�gasz r�ce i z ca�ej si�y wbijasz $h w... po�ladek $Z! Z $S gardzieli wydobywa si� okropny ryk b�lu, kt�ry rozgleg� si� w okolicy. Jednak ty juz dawno odskoczy�<&e�/a�/o�> na bezpieczn� odleg�o��.{x", ch, wield, victim, TO_CHAR );
							act( "{5$n szybkim i ostro�nym krokiem zbli�a si� do $z i mierzy $S plecy bacznym spojrzeniem. Nagle wyci�ga $e r�ce i z ca�ej si�y wbija $h w... po�ladek $Z! Z gardzieli ofiary wydobywa si� okropny ryk b�lu, kt�ry rozlega si� po okolicy.{x", ch, wield, victim, TO_NOTVICT );
							break;
					}
				}
				return TRUE;
			}
			//opisy specjalne dla potworow o duzym cielsku (ale nie bardzo duzym like smoki)
			if( !str_cmp( race_table[ GET_RACE(victim) ].name, "wywerna" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "hydra" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "mantykora" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "smok" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "bazyliszek" ))
			{
				act( "{5Cichutko przebieraj�c n�kami zakradasz si� za $C, staraj�c si� pozosta� niezauwa�on<&ym/�/e>. Zauwa�aj�c dogodn� okazj� jednym susem podskakujesz do spas�ego cielska potwora i wbijasz w nie $h najg��biej jak tylko potrafisz. Ostrze twej broni przecina mi�siste cia�o $Z jakby to by�o mas�o, po czym z chlupotem wyskakuje, ca�e w krwi.{x",ch, wield, victim, TO_CHAR );
				act( "{5$n cichutko przebieraj�c n�kami zakrada si� za $C. Wypatruj�c dogodn� okazj� jednym susem doskakuje do spas�ego cielska potwora i g��boko w nie wbija $h. Ostrze $s broni przecina mi�site cia�o $Z, jakby by�o one z mas�a, poczym z chlupotem wyskakuje, ca�e we krwi.{x", ch, wield, victim, TO_NOTVICT );
				return TRUE;
			}

			switch ( number_range( 0, 3 ) )
			{
				case 0:
					act( "{5Zachodzisz $C od ty�u i wbijasz $O $p na wysoko�ci bark�w prawie po r�koje��.{x", ch, wield, victim, TO_CHAR );
					act( "{5Czujesz przez zaledwie chwil� ciep�o czyjego� oddechu za sob� po czym czujesz rozdzieraj�cy b�l przechodz�cy przez twoje cia�o, kt�ry prawie pozbawia ci� przytomno�ci. Przez g�ow� przechodzi ci strz�p my�li: SZTYLET!{x", ch, wield, victim, TO_VICT );
					act( "{5$n zachodzi $C od ty�u i wbija $O $p w plecy. Przez u�amek chwili widzisz jak $N zamiera z b�lu.{x", ch, wield, victim, TO_NOTVICT );
					break;
				case 1:
					act( "{5Zakradasz si� cicho do $Z i kiedy jeste� ju� tu� za $S plecami bierzesz lekki zamach i d�gasz $M $j pod lew� �opatk�.{x", ch, wield, victim, TO_CHAR );
					act( "{5Wydaje ci sie, �e kto� jest za tob�, po czym s�yszysz cichy syk przecinanego czym� powietrza i czujesz uderzenie ostrza w okolicach �opatki. Cooo za B�L!{x", ch, wield, victim, TO_VICT );
					act( "{5$n bardzo szybko zakrada si� do $Z i kiedy jest ju� za $Y, nie trac�c czasu d�ga $M pod lew� �opatk�.{x" , ch, wield, victim, TO_NOTVICT );
					break;
				case 2:
					act( "{5W kilku szybkich krokach podbiegasz do $Z i wbijasz $O $p kilka palc�w poni�ej szyi, o szeroko�� kciuka mijaj�c kr�gos�up.{x", ch, wield, victim, TO_CHAR );
					act( "{5Sylwetka $z miga za twoimi plecami. Kto� doskakuje do ciebie i wpycha ci zimne ostrze troch� poni�ej szyi. Przez moment chwiejesz si� na nogach z trudem �api�c r�wnowag�.{x", ch, wield, victim, TO_VICT );
					act( "{5$n doskakuje w paru krokach do $Z i wbija $O jakie� ostrze troch� poni�ej szyi. Widzisz jak $N chwieje si� chwil�, ale - o dziwo - nie osuwa si� na ziemi�.{x", ch, wield, victim, TO_NOTVICT );
					break;
				case 3:
					act( "{5Pr�bujesz cicho podkra�� si� do $Z, jednak nie do ko�ca ci si� to udaje. Robisz troch� ha�asu i $E ci� zauwa�a, jednak jest ju� za p�no na reakcj�. Tw�j $p l�duje mi�dzy $S �opatkami niemal w tym samym momencie.{x", ch, wield, victim, TO_CHAR );
					act( "{5K�tem oka dostrzegasz jak $n zakrada si� tu� za twoimi plecami z czym� w r�ku. Pr�bujesz instynktownie si� uchyli�, ale jest ju� za p�no. Ostrze $f l�duje mi�dzy twymi �opatkami.{x", ch, wield, victim, TO_VICT );
					act( "{5$n zakrada si� za plecami $Z, robi troch� ha�asu i zostaje przez $A w por� dostrze�on$r. Jest ju� jednak za p�no na jakielowiek uniki, $p $z l�duje mi�dzy �opatkami $Z.{x", ch, wield, victim, TO_NOTVICT );
					break;
			}
			return TRUE;
		}
		return TRUE;
	}

	/* CHARGE OPISY */
	else if ( dt == gsn_charge && wield )
	{

		if ( immune )
		{
			act( "{5$p ze �wistem przecina powietrze, trafia $C jednak $E nawet tego nie zauwa�a.{x", ch, wield, victim, TO_CHAR );
			act( "{5$p $z ze �wistem przecina powietrze, trafia ci� jednak nawet tego nie zauwa�asz.{x", ch, wield, victim, TO_VICT );
			act( "{5$p $z ze �wistem przecina powietrze, trafia $C jednak $E nawet tego nie zauwa�a.{x", ch, wield, victim, TO_NOTVICT );
		}

		switch ( wield->value[ 0 ] )
		{
			case WEAPON_AXE:
				if (!str_cmp( race_table[ GET_RACE(victim) ].name, "zombi" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "kryptowiec" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "upiorny rycerz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "ghul" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "ghast" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "mumia" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "mumia wi�ksza" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "wampir" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Wpatrujesz si� intensywnie w sylwetk� przeciwnika. Bierzesz pot�ny zamach i z ca�ych si� nacierasz na $C, jednak przeciwnik jest zbyt szybki, b�yskawicznie uchyla si� od twojego ciosu z szyderczym warkotem. Ostrze $f o centymetry ca�e mija cel poci�gaj�c ci� za sob�. Bogowie! Ten b��d m�g� kosztowa� ci� �ycie.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak $z kieruje swoj� bro� na przeciwnika wraz z pot�nym zamachem. $N b�yskawicznym ruchem wykonuje unik, a cios atakuj�cego jest tak pot�ny, �e poci�ga go za sob�. $n o ma�o nie nadziewa si� na atak wroga.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Warcz�c z w�ciek�o�ci przez zaci�ni�te z�by przez zmru�one oczy wpatrujesz si� w $C. Zaciskaj�c z nienawi�ci palce na stylisku $f wznosisz go nad g�ow�. Nie dbaj�c o w�asne bezpiecze�stwo z ca�ych swoich si� tniesz prosto w czaszk� przeciwnika, $p wbija si� z g�o�nym chrz�stem w cia�o nieumar�ego prawie go rozpo�awiaj�c. $N opada z bolesnym i �miertelnym skrzekiem. Fontanna cuchn�cej cieczy i oderwane kawa�ki galaretowatego cia�a unosz� si� w powietrze gdy wyszarpujesz sw� bro�.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n wyprowadza w�ciek�y, morderczy cios zaciskaj�c palce na stylisku $f a� do bia�o�ci. Tak pot�ne uderzenia nale�� do rzadko�ci. Z zachwytem spogl�dasz jak �miertelne r�bni�cie rozrywa cuchn�ce cia�o $Z prawie na dwie cz�ci. �cierwo powoli osuwa si� z bolesnym skrzekiem, a z cia�a tryska fontanna cuchn�cej, t�ustej cieczy.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5�ciskasz mocniej stylisko $f. B�yskawicznie oceniasz odleg�o��, bierzesz zamach i r�biesz z ca�ych si�, $h zatacza szeroki �uk i wbija si� z mokrym mla�ni�ciem g��boko w korpus $Z. Cuchn�ca ciecz wydobywaj�ca si� z martwego cia�a obryzguj�c ciebie i wszystko dooko�a. $N zatacza si� od si�y twojego ciosu skaml�c �a�o�nie. Wydobywasz z siebie ryk w�ciek�o�ci i nacierasz ze zdwojon� si��!{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak $n precyzyjnie ocenia odleg�o�� mi�dzy sob� a $V. Jego $p zatacza �wietlisty �uk i z potworn� si�� zag��bia si� w martwe cia�o przeciwnika. W ostatnim momencie odskakujesz przed bryzgaj�cym, cuchn�cym trupim jadem. Nieumar�y zatacza si� skaml�c pod ciosem $z.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Bez ostrze�enia, b�yskawicznym ruchem kierujesz $p w kierunku $Z. Ostrze tnie powietrze z przera�aj�cym �wistem �mierci i zag��bia si� g��boko w korpus nieumar�ego wyrywaj�c przy tym poka�ny kawa� gnij�cego mi�sa.{x", ch, wield, victim, TO_CHAR );
						act( "{5Patrzysz jak b�yskawicznym ruchem $n kieruje $h w stron� $Z. Ostrze $s broni za �wistem wbija si� w korpus bestii i z mokrym chrz�stem kawa� galaretowego mi�sa odrywa si� z torsu $Z i szybuje w powietrze ods�aniaj�c przegni�e wn�trzno�ci.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (!str_cmp( race_table[ GET_RACE(victim) ].name, "duch" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "banshee" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "cie�" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "widmo" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "zmora" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "upi�r" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Tniesz z ca�ych si� $C wykorzystuj�c chwil� nieuwagi przeciwnika, $p ze �wistem tnie powietrze i opada na kark wroga. Zatapia si� w niematerialne cia�o, jednak nie czujesz �adnego oporu. G�adko przechodzi przez p�przezroczysty, ods�oni�ty kark i w�druje dalej w powietrze nie czyni�c $X jakiejkolwiek krzywdy. Z przera�eniem ws�uchujesz si� w rozbrzmiewaj�cy w twym chorym ze strachu umy�le upiorny, szyderczy chichot.{x", ch, wield, victim, TO_CHAR );
						act( "{5Powietrze przeszywa dono�ny �wist ostrza $f, p�dz�cego w kierunku ods�oni�tego karku $Z. To $n wykorzystuje sytuacj� i atakuje. Bro� zag��bia si� w niematerialnym ciele przeciwnika. Ostrze $f przesz�o g�adko przez grdyk� i poszybowa�o dalej tn�c ze �wistem powietrze. Jedynie szarawa mgie�ka wok� karku $Z dowodzi, �e zetkn�a si� z nim zimna stal. Bardziej odczuwasz ni� s�yszysz przera�aj�cy, upiorny, szyderczy chichot unosz�cy si� w powietrzu.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Szalony, ob��ka�czy wrzask wydobywa si� z twojego gard�a gdy zadajesz cios. Z ca�ych si� wyprowadzasz pot�ne ci�cie, �wiat wiruje przed twoimi oczami. Skupiasz si� tylko na celu. P�ywaj�cy w powietrzu rozmazany wizerunek $Z spoziera na ciebie z nienawi�ci�. Twoja bro� wnika g��boko, bez oporu w klatk� piersiow� przeciwnika. Szerokie smugi jaskrawoczerwonego �wiat�a strzelaj� w powietrze. Upiorne, dono�ne wycie rani twe uczy, gdy wyszarpujesz $h. �wiat�o z sekundy na sekund� staje si� bardziej intensywne i z czasem ciemnoczerwone. $N znika w o�lepiaj�cym rozb�ysku szkar�atnego blasku. Upiorny, pe�en pot�pie�czego b�lu wrzask jeszcze d�ugo unosi si� w powietrzu odbijany echem.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n wrzeszcz�c ob��ka�czo z ca�ych si� wyprowadza pot�ne ci�cie w klatk� piersiow� $Z. Szerokie ostrze $f wnika g��boko w niematerialne cia�o. W powietrze strzelaj� szerokie, jaskrawoczerwone smugi �wiat�a. Twoje uszy wype�nia pe�en b�lu op�ta�czy wrzask $Z, a po�wiata nabiera g��bszego, ciemniejszego koloru. P�przezroczyst� istot� zalewa purpurowe �wiat�o, po czym niknie ona w o�lepiaj�cym rozb�ysku. Mru��c oczy ws�uchujesz si� w upiorne echo szale�czego krzyku.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Wpatrujesz si� w rozmazan�, p�materialn� sylwetk� przeciwnika. Zataczasz $j szerokie kr�gi wyczekuj�c odpowiedniego momentu do zadania ciosu. Nie masz poj�cia czy to gdzie trafisz ma jakiekolwiek znaczenie. Przera�liwe, pe�ne cierpienia j�ki $Z przyprawiaj� ci� o dreszcze. Nie wytrzymuj�c napi�cia, bierzesz szeroki zamach i wyprowadzasz pot�ny cios na o�lep. Szerokie ostrze $f zag��bia si� w p�przezroczyste cia�o i zwalnia. Twe uszy rani przera�aj�cy ryk jakby tysi�ca g�os�w, w panice wyszarpujesz bro�. W astralnej pow�oce przeciwnika widnieje szeroka smuga od kt�rej bije czerwonawa po�wiata.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n wolno podchodzi do $Z wyczekuj�c odpowiedniego momentu do zadania ciosu. W pewnym momencie dr��cymi r�koma bierze pot�ny zamach kieruj�c ostrze $f w niematerialne cia�o. Bro� bezg�o�nie zatapia si� w p�przezroczystej sylwetce i nagle zwalnia jakby trafiaj�c na op�r. Potworny wrzask b�lu i cierpienia rozbrzmiewa po okolicy, jakby tysi�ce garde� krzykn�o w jednej sekundzie. Z szerokiej szramy w ciele $Z bije o�lepiaj�ce, jaskrawoczerwone �wiat�o.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5�ciskaj�c mocno stylisko $f obserwujesz p�ynne ruchy przeciwnika. P�ynie on nad ziemi� prosto w twoim kierunku. Gdy $N znajduje si� dostatecznie blisko ciebie zadajesz pot�ny cios w bok niematerialnej istoty. Potworny j�k cierpienia rozlega si� dono�nie po okolicy, gdy twoje ostrze nacina p�prze�roczysty korpus. Wyszarpuj�c bro� zauwa�asz w boku astralnej istoty szerok� wyrw�, z kt�rej bije smuga czerwonego �wiat�a. Na twarzy $Z wida� wyra�nie nieme cierpienie �wiadcz�ce o skuteczno�ci uderzenia.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n mocno �ciskaj�c stylisko $f czeka a� p�yn�cy w jego kierunku przeciwnik zbli�y si� na dostateczn� odleg�o��. Kiedy to nast�puje wyprowadza pot�ny cios w jego bok. Bro� zag��bia si� w niematerialnym korpusie, pozostawiaj�c za sob� olbrzymi� wyrw�, z kt�rej promieniuje jaskrawy, czerwony strumie� �wiat�a.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (!str_cmp( race_table[ GET_RACE(victim) ].name, "drakolicz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "licz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "szkielet wojownik" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "szkielet" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Wyprowadzasz pot�ne, boczne ci�cie na korpus $Z. �wist powietrza, klekotanie ko�ci i tw�j zduszony warkot rozbrzmiewaj� ci w uszach. Ostrze zatacza szeroki, zamaszysty �uk b�yszcz�c przy tym w�ciekle, $p zbli�a si� z zastraszaj�c� pr�dko�ci� w kierunku celu, jednak dzieje si� co� dziwnego. Pech chcia�, �e rozp�dzona bro� przelatuje z mi�dzy �ebrami przeciwnika nie czyni�c mu �adnej krzywdy. W zdumieniu pr�bujesz odzyska� r�wnowag� odskakuj�c jak najdalej od nacieraj�cego szale�czo przeciwnika.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n warcz�c op�ta�czo bierze szeroki zamach i wyprowadza precyzyjne boczne ci�cie na korpus $Z. Ostrze b�yszczy w�ciekle zataczaj�c szeroki �uk. Z pot�n� si�� $p pruje powietrze, jednak staje si� co� dziwnego. Bro� ze �wistem przelatuje mi�dzy �ebrami $Z nie czyni�c �adnej krzywdy. Z wyrazem w�ciek�o�ci i zdumienia na twarzy $n odskakuje jak najdalej od wroga pr�buj�c odzyska� r�wnowag�.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Rycz�c w�ciekle, tocz�c pian� z ust nacierasz na zaskoczonego przeciwnika. R�biesz z pot�n� si�� swych napi�tych do granic ramion z do�u ku g�rze, ostrze $f z chrupni�ciem trafia w kr�gos�up na wysoko�ci klatki piersiowej $Z posy�aj�c go wysoko w g�r�. Nim $N zd��a opa�� na ziemi� unosisz swoj� zab�jcz� bro� i wyprowadzasz na o�lep b�yskawiczne ci�cie. Z gruchotem �amanych ko�ci ostrze trafia w cel przepo�awiaj�c wroga. Obie cz�ci $Z szybuj� w powietrzu par� metr�w i wal� si� z �oskotem na ziemi�. Zmasakrowany $N w chmurze zmursza�ych, po��k�ych od�amk�w i cuchn�cego, st�ch�ego py�u rozpada si� na tysi�ce kawa�k�w. U twych st�p toczy si� oderwana czaszka, kt�r� mia�d�ysz jednym uderzeniem buta.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n tocz�c pian� w�ciek�o�ci z rozchylonych ust naciera na $C. R�bniecie jest tak pot�ne, �e z g�uchym chrupni�ciem posy�a przeciwnika wysoko w g�r�. $n unosi swoje b�yszcz�ce, �mierciono�ne ostrze i tnie z ca�ych si� w lawiruj�cego w powietrzu przeciwnika, a $p trafia bezb��dnie w cel przepo�awiaj�c go. Obie cz�ci po��k�ego, zmursza�ego o�ywie�ca szybuj� par� metr�w obracaj�c si� we wszystkich kierunkach i wal� si� z �oskotem na pod�o�e. Nieumar�y rozpada si� na tysi�ce cz�ci wywo�uj�c chmur� od�amk�w kostnych i wiekowego, cuchn�cego py�u. $n z suchym trzaskiem wdeptuje w ziemi� tocz�c� si� u jego st�p czaszk�.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Wykrzykuj�c imi� swego boga r�biesz $j z ca�ych si� w czaszk� przeciwnika. Bro� z pr�dko�ci� b�yskawicy opada na g�ow� $Z. Zimne ostrze z suchym chrupotem zag��bia si� w nagiej ko�ci i w�druje od g�ry czaszki, przez ziej�cy pustk� oczod� ko�cz�c sw�j szlak zniszczenia tu� nad g�rn� szcz�k�. Cz�� pustej, zmursza�ej twarzy wroga opada ci�ko na pod�o�e rozbijaj�c si� w drobny mak. $N chwiej�c si� po otrzymanym uderzeniu �widruje ci� z nienawi�ci� ocala�ym oczodo�em i atakuje ze zdwojon� si��.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n wykrzykuj�c z w�ciek�o�ci� jakie� nieznane ci imi� wznosi $h wysoko nad g�ow� i opuszcza z szybko�ci� pioruna w kierunku $Z. B�ysk ostrza i suchy, niemi�y chrupot ko�ci towarzysz� temu pot�nemu r�bni�ciu. Bro� zag��bia si� w zmursza�ej ko�ci tn�c ze zgrzytem pust�, po��k�� twarz $Z. Jej lewa cz�� odrywa si� tu� nad rozwart� w niemym cierpieniu g�rn� szcz�k� i z �oskotem opada na pod�o�e rozpadaj�c si� w mgnieniu oka w drobny mak. $N z widoczn� nienawi�ci� b�yszcz�c� w ocala�ym, zion�cym pustk� oczodole atakuje $c ze zdwojon� sil�.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Powoli zbli�asz si� do przeciwnika �ciskaj�c mocno stylisko $f. $N jest na razie zbyt czujny i nie spostrzegasz luki w jego obronie. Wyczekujesz odpowiedniego momentu do zadania ostatecznego ciosu. Nagle przeciwnik potyka si� i chwieje przez chwil�, pr�buj�c odzyska� r�wnowag� odchyla zmursza�y ko�ciec lekko w ty�. Wykorzystuj�c sytuacj� unosisz $h i tniesz z ca�ych si� na ukos. Bro� z trzaskiem opada na �ebra �ami�c kilka z nich. Kawa�ki ko�ci lec� w powietrze we wszystkich kierunkach, $N odzyskuj�c r�wnowag� w�ciekle rusza w twoim kierunku.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n okr��a ostro�nie $C czekaj�c na odpowiedni moment do ataku. W pewnym momencie $s wr�g potyka i chwieje przez chwil� pr�buj�c odzyska� r�wnowag�. $n b�yskawicznie zapiera si� nogami i wyprowadza szerokie ci�cie na korpus przeciwnika, $s $p zatacza zab�jczy �uk i opada z trzaskiem na dolne �ebra przeciwnika. Kawa�ki odci�tych ko�ci strzelaj� w powietrze w ka�dym z kierunk�w, jednak szkielet odzyskuje r�wnowag� i w�ciekle rusza w kierunku $z wyprowadzaj�c �mierciono�ne ataki.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (IS_SET(race_table[GET_RACE(victim)].type , ANIMAL))
				{
					if ( dam <= 0 )
					{
						act( "{5P�dzisz z przera�liwym wrzaskiem w kierunku przeciwnika z uniesion� broni�. Ostrze $f zatacza nad twoj� g�ow� b�yszcz�ce, szerokie kr�gi. W d�ugich susach, �lizgaj�c si� po pod�o�u zbli�asz si� z ka�d� chwil� do $Z. Wyprowadzasz p�ynne ci�cie na ukos, or� wr�cz frunie w powietrzu rzucaj�c jasne refleksy, jednak $N jest zbyt czujny, $S zwierz�cy instynkt i zab�jczy refleks po raz kolejny ratuj� $S �agro�one �ycie. Odsuwa si� w ostatniej chwili na bezpieczn� odleg�o�� z b�yskiem paniki w �lepiach.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n wrzeszcz�c szale�czo w d�ugich susach zbli�a si� do $Z. Ostrze $f zatacza w powietrzu szerokie, �wietliste kr�gi po czym opada uko�nie w kierunku celu, jednak instynkt i b�yskawiczny refleks ratuj� ofiar� z opresji, $N o w�os unika zr�cznie wymierzonego na korpus r�bni�cia.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Z rykiem w�ciek�o�ci r�biesz z ca�ych si� w korpus przeciwnika. Cios jest tak silny, �e wnika w tkank� g��boko tn�c mi�so, ko�ci i �y�y. Fontanny krwi tryskaj� wysoko zalewaj�c wszystko dooko�a. W powietrze szybuj� rozerwane wn�trzno�ci. $N patrzy na ciebie z wyrzutem z iskr� gasn�cego �ywota w oczach, a ty �miej�c si� jak szalon<&y/a/e> wyrywasz ostrze i z rozdartych t�tnic po raz wt�ry buchaj� rzeki ciep�awej posoki. U twoich st�p le�y ju� teraz kupa por�banego mi�sa w olbrzymiej ka�u�y ciemnoczerwonej krwi. �lepia $Z zachodz� mg�� i gasn�.{x", ch, wield, victim, TO_CHAR );
						act( "{5Przera�aj�cy ryk w�ciek�o�ci wydobywaj�cy si� z gard�a $z unosi si� w powietrze i niesie daleko echem, r�bie $e z ca�ych si� $j w $C. Odg�os p�kaj�cych ko�ci, r�banego mi�sa, rozrywanych �y� i wn�trzno�ci miesza si� z ob��ka�czym okrzykiem. W g�r� strzelaj� gejzery ciep�ej, paruj�cej jeszcze krwi zalewaj�c wszystko strumieniami. Z szale�czym chichotem $n wyrywa sw�j or� ze zmasakrowanego cia�a, u $s st�p widnieje ju� tylko por�bane �cierwo, kt�re drga jeszcze przez chwil� i nieruchomieje.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Chwytasz obur�cz stylisko swej broni i unosisz j� wysoko w g�r�. Bierzesz pot�ny zamach znad g�owy i mierz�c dok�adnie opuszczasz or� z zastraszaj�c� pr�dko�ci� i si��. Ostrze tnie powietrze ze �miertelnym �wistem i opada na cel. $N kul�c si� ze strachu instynktownie pr�buje unikn�� �mierciono�nego ci�cia, jednak na pr�no. Bro� z paskudnym mlaskiem i odg�osem rwanej sk�ry oraz �amanych ko�ci zag��bia si� w cia�o. Ciep�a, lepka posoka chlupie prosto na ciebie oraz na pod�o�e, a powietrze rozdziera przera�liwy kwik.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n unosi sw� bro� wysoko w g�r�, bierze pot�ny zamach i opuszcza j� z zastraszaj�c� si�� na $Z, $E kul�c si� ze strachu pr�buje instynktownie uskoczy� przed morderczym r�bni�ciem. Na pr�no jednak. Ostrze zag��bia si� w $S boku z dono�nym trzaskiem �amanych ko�ci, darcia sk�ry oraz rozrywanego mi�sa. Z rozdygotanego cia�a tryska gejzer krwi zalewaj�c wszystko dooko�a, a powietrze przecina dono�ny, przera�liwy kwik zwierz�cia.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Doskakujesz b�yskawicznie do przeciwnika i wyprowadzasz szybkie, kr�tkie r�bni�cie skierowane w sam �rodek korpusu. Tw�j or� opada za �wistem na zdezorientowany i oszo�omiony cel. Ostrze $f szybuje za �wistem rzucaj�c mordercze refleksy. Jednak b�yskawiczny atak ma r�wnie� swoje s�abe strony, nie zdo�a�<&e�/a�/o�> odpowiednio oceni� odleg�o�ci i nada� odpowiedniego impetu broni. Czubek ostrza zahacza o bok $Z tn�c sk�r�. W tym samym momencie powietrze rozdziera kwikni�cie przera�enia i b�lu. Ciemna posoka chlupie dooko�a barwi�c wszystko na czerwono.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n doskakuje b�yskawicznie do $Z w morderczym ataku. Wyprowadza szybkie, kr�tkie r�bni�cie w korpus $Z chc�c zako�czy� walk� w u�amku sekundy, jednak zbyt ma�a si�a ciosu i �le wymierzona odleg�o�� powoduje, i� jedynie koniec ostrza zahacza o bok $Z tn�c $M sk�r� z odg�osem darcia. W powietrze unosi si� przera�liwe kwikni�cie strachu i b�lu.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
			case WEAPON_SWORD:
				if (!str_cmp( race_table[ GET_RACE(victim) ].name, "zombi" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "kryptowiec" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "upiorny rycerz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "ghul" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "ghast" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "mumia" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "mumia wi�ksza" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "wampir" ))
				{
					if ( dam <= 0 )
					{
						act( "{5P�dzisz wprost na $Z w d�ugich susach wznos�c nad g�ow� $p. Bro� twa ko�ysze si� miarowo w rytm szale�czego biegu. Opuszczasz g�owni� z zastraszaj�c� si��, lekko pod skosem kieruj�c j� wprost w kark przeciwnika, jednak w ostatnim momencie tracisz r�wnowag�. Ostrze przecina ze �wistem powietrze obok o�ywie�ca. Jeste� teraz niebezpiecznie blisko wroga i nie mo�esz z�apa� r�wnowagi tak szybko jakby� chcia<&�/�a/�o>. Wci�gasz w nozdrza od�r, oddalonego od ciebie zaledwie o cale, gnij�cego cia�a, od kt�rego kr�ci ci si� w g�owie jeszcze bardziej.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak w kilku susach $n dopada $Z wznosz�c $h nad g�ow�, jednak zadaj�c ci�cie skierowane widocznie ku szyi przeciwnika traci r�wnowag� i chybia. Pr�buj�c odzyska� r�wnowag� szar�uj�cy z trudno�ci� unika gradu cios�w.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Chwytasz obur�cz $h za r�koje�� niczym sztylet i unosisz nad g�ow�. B�yszcz�ca g�ownia skierowana jest w d�. Ramiona dr�� od wysi�ku, lecz cios ten wymaga ogromnej krzepy. W przegni�ym m�zgu $Z pojawia instynktowna my�l o rych�ym jego ko�cu. Ze �wistem opuszczasz kling�. Wbijasz j� tu� poni�ej karku ozywie�ca z zastraszaj�c� si��, ostrze penetruje cia�o pionowo w d� na wylot przez ca�y korpus. Sztych przechodzi g�adko, z g�o�nym mla�ni�ciem przez pachwin� o�ywie�ca i wbija si� w pod�o�e. Strumienie cuchn�cej cieczy tworz� ogromne ka�u�e nad podryguj�cym jeszcze resztk� nienaturalnego �ycia cia�em. $N wydaje z siebie ostatni przepe�niony b�lem skrzek i kona.{x", ch, wield, victim, TO_CHAR );
						act( "{5Patrzysz jak $n chwyta nienaturalnie $h dzier��c go jak sztylet. B�yskawicznym ruchem unosi bro� do g�ry i jeszcze szybciej opuszcza w d� z zastraszaj�c� si��. Cia�em $Z wstrz�sa nag�y skurcz. G�ownia przesz�a g�adko na wylot z paskudnym, dono�nym chrz�stem wbijaj�c si� w ziemi�. Nadziany jak na pal o�ywieniec wydaje z siebie ostatni pe�en b�lu i rozpaczy skrzek, po czym powoli osuwa si� po ostrzu na ziemi�, prosto w ogromn� ka�u�� w�asnych p�yn�w.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Okr��asz $C �ciskaj�c mocno $h i wyczekujesz odpowiedniego momentu by uderzy�. Przeciwnik sunie w twoim kierunku mechanicznymi, makabrycznymi ruchami wydobywaj�c z martwych p�uc skrzek, po kt�rym przechodz� ci ciarki po grzbiecie. Zbli�aj�c si� do ciebie pochyla si�, aby zada� cios. �ciskaj�c mocniej trzpie� $f odskakujesz o krok i wykonujesz pot�ne pchni�cie. G�ownia rzucaj�c �wietlne refleksy uderza z mordercz� si�� b�yskawicy. Wbijasz w brzuch nieumar�ego szerokie ostrze na ca�y sztych. Z rz꿱cych ust przeciwnika chlupie rzeka przezroczystej cieczy kiedy bezlito�nie przekr�casz mu miecz w ciele i wyszarpujesz go.{x", ch, wield, victim, TO_CHAR );
						act( "{5Patrzysz jak $n szuka luki w obronie przeciwnika. Kiedy $N ods�ania si� by zaatakowa�, $n b�yskawicznie robi krok w ty�, cofa lekko $h, by w nast�pnej sekundzie nadzia� na niego sw� ofiar�. Ostrze wbija si� a� na sztych w brzuch nieumar�ego. Z paszczy o�ywie�ca bucha cuchn�ca jucha, a przera�aj�ce, pe�ne cierpienia charczenie dociera do twych uszu. $n z grymasem szale�stwa na twarzy naciera ponownie.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Nacierasz zdecydowanie na $Z. Bierzesz pot�ny zamach i kierujesz szerok� g�owni� $f z g�ry ku do�owi lekko zakrzywiaj�c tor ci�cia, kt�re przesz�o na ukos przez klatk� piersiow� i rozora�o tu��w o�ywie�ca a� po dolne �ebra. Nieumar�y podtrzymuj�c wyprute wn�trzno�ci, wyj�c niemi�osiernie i chlapi�c cuchn�c� ciecz� zdwoi� w �lepej furii si�� swych atak�w.{x", ch, wield, victim, TO_CHAR );
						act( "{5Spogl�dasz na scen�, kt�ra wywo�uje u ciebie torsje. $n zadaje pot�ne ci�cie $X, kt�re patroszy go niczym prosiaka u rze�nika. Grube w�z�y przegni�ych flak�w o�ywie�ca zwisaj� mu z szerokiej, paskudnej szramy na korpusie. Hektolitry cuchn�cej cieczy bryzga na wszystko wok�, jednak $N nadal stoi chwiejnie na nogach i stawia op�r.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (!str_cmp( race_table[ GET_RACE(victim) ].name, "duch" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "banshee" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "cie�" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "widmo" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "zmora" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "upi�r" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Wyskakujesz do g�ry, celuj�c $j w $C, jednak pech sprawi� i� tw�j cel uchyli� si� i twa bro� wbi�a si� w ziemie tu� obok niematerialnego cia�a $Z.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak $n wyskakuje do g�ry i celuje sw� broni� w $C, jednak cel uchyli� si� i $f $z wbija si� w ziemi�, tu� obok niematerialnego cia�a.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Bierzesz pot�ny rozbieg i z w�ciek�o�ci� rzucasz si� na $C. Twa bro� zatacza kr�gi wzniesiona wysoko i b�yszczy upiornie gdy tw�j rze�nicki cios spada na pr�buj�cego si� os�oni� przeciwnika. Ostrze $f g�adko wchodzi w eteryczne cia�o tn�c je i rozrywaj�c, jego kawa�ki odpadaj� i znikaj� po chwili. Suniesz g�adko, od g�owy, przez r�ce po sam d�. Wstajesz z kl�czk�w i z satysfakcj� patrzysz jak dwie cz�sci $Z rozp�ywaj� si� powoli a pot�piencze j�ki gasn� w niebycie.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n bierze pot�ny rozbieg i z w�ciek�ym rykiem rzuca si� na $C. Ostrze $f zatacza kr�gi, wznie�ione wysoko upiornie b�yszczy gdy rz�nicki cios spada na pr�buj�cego si� os�oni� ducha. Bro� $z g�adko wchodzi w eteryczne cia�o, tn�c je, rozrywaj�c. Odpadaj�ce kawa�ki znikaj� po chwili, roztapiaj� si� od��czone od swego niematerialnego cia�a. Cios przeszed� od g�ry po sam d�. Po chwili $n wstaje i z satysfakcj� patrzy jak powoli rozp�ywaj� si� dwie cz�sci ducha a pot�piencze j�ki cichn� coraz bardziej.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Bierzesz kr�tki rozbieg i odbijasz si� od ziemi. Twa bro� z olbrzymi� pr�dko�ci� trafia w p�materialne cia�o $Z i przeszywaj�c je na wylot jak n� wbija si� w mas�o. $N wydaje z siebie pot�pieczy j�k, jakby si� do czego� szykowa�, jednak szybko wyci�gasz $h z $S powoduj�c wielkie zniszczenia.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n bierze kr�tki rozbieg i odbija si� od ziemi. Jego bro� bezb��dnie trafia w niematerialne cia�o $Z przeszywaj�c je na wylot i czyni�c wielkie zniszczenia. $N wydaje z siebie pot�pienczy j�k i probowa� uderzy� $c, jednak niezd��a si� nawet powa�nie zamachn�� kiedy przeciwnik wyci�ga z sw� bro� rani�c $C jeszcze powa�niej.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Z olbrzymi� pr�dkosci� i dzikim rykiem rzucasz si� na $C, wymachuj�c $j. Przeciwnik pr�bowa� uchyli� si�, lecz twa bro� przyszpili�� go do ziemi z ogromn� si��. Pot�pienczy j�k, jaki wyda� duch sprawi�, �e a� zabola�y ci� uszy. Jednak z g�o�nym warkotem wyszarpno�e� bro� z niematerialnego, szamocz�cego si� bez�adnie cia�a. $N rzuca si� do walki z wsciek�o�ci�.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n z olbrzymi� pr�dkosci� i dzikim rykiem rzuca si� na $C, wymachuj�c dooko�a $j. Unik, jaki pr�bowa� zrobi� cel, nie uda� si� i bro� $z przyszpili�a go do ziemi z wielk� si��. $N wrzasn�� pot�pi��czo, po czym $n sprawnym ruchem wyszarpn�� bro� z szamocz�cego si�, niematerialnego cia�a, kt�re jednak podnios�o si� i rzuci�o do walki z wsciek�osci�. Wszystko sta�o si� w jednej chwili.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (!str_cmp( race_table[ GET_RACE(victim) ].name, "drakolicz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "licz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "szkielet wojownik" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "szkielet" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Bierzesz silny i szeroki zamach zamierzaj�c wykona� pot�ne ci�cie znad g�owy w ods�oni�t� czaszk� $Z. Opuszczasz ze �wistem szerok� kling� $f, prowadz�c j� precyzyjnie ku obranemu celowi. Jednak �liska od potu r�koje�� przekr�ca ci si� w d�oniach i o ma�o co nie wypuszczasz broni z r�k. Ostrze opada z zastraszaj�c� si�� i uderza p�azem w tward� czaszk� powoduj�c zdr�twienie ramion. G�ownia z g�uchym brz�kiem odbija si� od ko�cianej g�owy nie czyni�c przeciwnikowi �adnej krzywdy.{x", ch, wield, victim, TO_CHAR );
						act( "{5Obserwujesz jak $n unosi nad g�ow� b�yszcz�ce ostrze $f w pot�nym zamachu. Kieruje kling� na ods�oni�t� czaszk� $Z ze �wistem tn�c powietrze. Do twych uszu dociera dono�ny, g�uchy brz�k kiedy szeroka g�ownia miecza odskakuje od czaszki nie wyrz�dzaj�c o�ywie�cowi �adnej krzywdy. $n haniebnie uderzy� p�azem, widocznie niedostatecznie mocno z�apana r�koje�� wy�lizgn�a si� i ostrze przekr�ci�o si� w szale�czym locie.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5W szale�czych susach zbli�asz si� do $Z z $j w r�kach. Mijaj�c przeciwnika zataczasz broni� ciasny, morderczy �uk i tniesz z ca�ych si�. Ostrze opada ze �wistem na podstaw� czaszki, odr�bana ko�ciana g�owa szkieletu zatacza szeroki �uk w powietrzu obracaj�c si� wok� w�asnej osi. Nast�pnie przykl�kasz plecami do przeciwnika i wyprowadzasz kolejne mordercze ci�cie wykonuj�c nag�y skr�t cia�a. Tym razem g�ownia z trzaskiem opada na kr�gos�up tu� nad miednic� oddzielaj�c korpus od n�g. Odr�bane cz�ci szkieletu z �oskotem wal� si� na ziemi� w tym samym czasie otaczaj�c tw� nieruchom�, skulon� sylwetk�.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak $n b�yskawicznie zbli�a si� do $Z w szale�czym biegu. Kiedy zr�wnuje si� ze szkieletem g�ownia $s $f zatacza ciasny �uk i opada z zastraszaj�c� si�� na podstaw� czaszki. Odci�ta g�owa $Z szybuje wysoko w powietrze obracaj�c si� wok� w�asnej osi. $n mija przeciwnika i przykl�ka zaraz za nim wyprowadzaj�c kolejne precyzyjne ci�cie. Wykonuj�c nag�y skr�t cia�a przecina kr�gos�up tu� nad miednic� przeciwnika odr�buj�c korpus od n�g. Po�wiartowane cz�ci szkieletu szybuj� przez chwil� w powietrzu i opadaj� r�wnocze�nie z �oskotem wok� nieruchomej sylwetki $z.{x", ch, wield, victim, TO_NOTVICT );
						make_head( victim );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Mocniej �ciskasz $h patrz�c na klekocz�cy przy ka�dym ruchu szkielet. Unosisz b�yskawicznie b�yszcz�ce ostrze w g�r� i mocno tniesz na ukos przez klatk� piersiow� $Z. Zimna stal zgrzyta o suche, zmursza�e ko�ci w piekielnym ha�asie, w powietrze szybuj� odci�te, ostre kawa�ki zmia�d�onych �eber wraz z tumanem st�ch�ego, wiekowego py�u. $N chwieje si� przez chwil� na ko�cistych nogach, a twoja uniesiona ponownie klinga b�yszczy w morderczych refleksach.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak $n unosi wysoko w g�r� $h. Jego ci�cie dosi�ga klatki piersiowej $Z i biegnie na ukos ko�cz�c sw� mordercz� w�dr�wk� po przek�tnej ko�cianego korpusu zadaj�c mordercze obra�enia. Dono�ny zgrzyt przyprawia ci� o zimne dreszcze. W niemym cierpieniu $N odchyla sw� czaszk� w ty� szeroko rozchylaj�c szcz�k�. Ostre kawa�ki po�amanych �eber oraz chmura st�ch�ego py�u w�druj� w powietrze tworz�c mglist� zas�on�. Nieumar�y chwieje si� na ko�cianych nogach pod si�� uderzenia.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Biegn�c w kierunku $Z wykonujesz szale�czy skok, szykuj�c si� do morderczego pchni�cia, potykasz si� jednak i na o�lep zamachujesz si� $j rozpaczliwie pr�buj�c odzyska� r�wnowag�. Ostrze $f, bardziej przez przypadek ni� w zamierzonym ciosie, trafia w miednic� szkieletu zag��biaj�c si� tam z cichym zgrzytem i brz�kiem. Wielki kawa� od�upanej ko�ci z g�uchym �oskotem l�duje na pod�o�u. Wyszarpuj�c poszczerbione ostrze z trudem unikasz b�yskawicznej kontry przeciwnika.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak $n odbija si� i wykonuje szale�czy skok w kierunku $Z. L�duj�c na ziemi stopa atakuj�cego niebezpiecznie wygina si� ku �rodkowi gro��c skr�ceniem kostki. Aby tego unikn�� $n przerzuca ci�ar cia�a na drug� ko�czyn� machaj�c na o�lep mieczem dla lepszego odzyskania r�wnowagi. Ostrze przez przypadek trafia w zmursza�� miednic� zaskoczonego szkieletu, wielki kawa� od�upanej ko�ci z �oskotem opada na pod�o�e. Wyszarpuj�c $h $n z trudem unika w�ciek�ych cios�w $Z.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (IS_SET(victim->form, FORM_CONSTRUCT))
				{
					if ( dam <= 0 )
					{
						act( "{5Z b�yskiem w oku wyprowadzasz pot�ne pchni�cie. Ostrze p�dzi z niebywa�� pr�dko�ci� w kierunku sylwetki $Z. Klinga tn�c powietrze dociera do klatki piersiowej celu, jednak nie by�a to dobra strategia. G�ownia uderza w tors wroga prawie rozpadaj�c si� na kawa�ki i odbija z metalicznym brz�kiem. Cios nie wyrz�dzi� przeciwnikowi praktycznie �adnej szkody. Potrz�saj�c g�ow� zataczasz si� rozpaczliwie jak najdalej od wyci�gni�tych w twym kierunku ramion chc�c unikn�� pot�nego kontrataku.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n wyprowadza mordercze pchniecie w tors przeciwnika. Ostrze tn�c powietrze osi�ga bez problemu sw�j cel, jednak odbija si� nie czyni�c $X �adnej szkody. Si�a uderzenia jest tak mocna, �e ramionami $z wstrz�sa bolesny dreszcz a g�ownia rozpada si� prawie na kawa�ki. Gdyby ostrze trafi�o na organiczne tkanki by�oby praktycznie po pojedynku, jednak tak si� nie sta�o. $n rozpaczliwie zataczaj�c si� jak najdalej od wyci�gni�tych ramion wroga odzyskuje r�wnowag� i rozlu�nia r�ce, aby pozby� si� natarczywego, narastaj�cego w nich odr�twienia.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Z furi� ob��ka�ca nacierasz na $C. W szale�czym biegu kr�cisz m�y�ca nad g�ow� $j i dopadaj�c nieruchomego przeciwnika, po czym w u�amku sekundy tniesz z g�ry z ca�ych si�. Ostrze opada z mia�d��c� si�� na kark wroga i wdziera si� w twarde cia�o ze zgrzytem. Lawina iskier zasypuje twoj� rozw�cieczon� twarz przes�aniaj�c na chwil� widoczno��. Odci�ty �eb wraz z prawym ramieniem i kawa�kiem torsu opada z g�uchym �oskotem na ziemi�, ko�ysze si� chwil� i nieruchomieje. Jednak to ci nie wystarcza. Aby da� upust szale�czej furii tniesz po raz wt�ry. Tym razem ostrze z metalicznym brz�kiem opada na dolne ko�czyny odrywaj�c je z paskudnym zgrzytem na wysoko�ci kolan. Cia�o $Z wali si� na ziemi� i rozpada na kawa�ki.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n z wyrazem szale�stwa i w�ciek�o�ci na twarzy szar�uje na bezdusznego przeciwnika. Uniesiony or� zatacza w powietrzu b�yszcz�ce, mordercze kr�gi, a barbarzy�ca w u�amku sekundy dopada $Z i tnie z ca�ych si�. Klinga $f ze zgrzytem zag��bia si� na ukos w twardym, nienaturalnym karku, a w powietrze strzela lawina iskier poch�aniaj�c obu przeciwnik�w. Opada jednak na czas by ods�oni� wal�cy si� ze stukotem czerep wraz z r�k� i kawa�kiem torsu $Z. $x to jednak nie wystarcza. Z rykiem w�ciek�o�ci tnie ponownie, ostrze opada na dolne ko�czyny $s ofiary odrywaj�c je od reszty zmasakrowanego cia�a. Sylwetka $Z wali si� z g�uchym �oskotem na pod�o�e i kruszy si� na kawa�ki.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5�ciskaj�c or� zbli�asz si� do przeciwnika w b�yskawicznym doskoku, $p zatacza w powietrzu �mierciono�ny �uk i ze �wistem opada na korpus $Z. Ostrze ze zgrzytem sunie po nienaturalnie twardym ciele, rozgrzane odpryski materia�u, z kt�rego jest wykonane strzelaj� w powietrze tworz�c istny grad.{x", ch, wield, victim, TO_CHAR );
						act( "{5Rozp�dzona klinga $f $z zatacza szeroki �uk i ze �wistem opada na korpus $Z. Z g�uchym stukotem ostrze dochodzi celu i ze zgrzytem sunie po nienaturalnym ciele wywo�uj�c grad odprysk�w.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5G�ownia $f zatacza szeroki, �wietlisty �uk opadaj�c z zastraszaj�c� si�� na g�ow� przeciwnika. Ostrze ze �wistem uderza nienaturalny �eb $Z, jednak�e ze�lizguje si� bez szkody po kr�g�o�ciach czerepu wywo�uj�c fontann� iskier. Sztych miecza zsuwaj�c si� z impetem l�duje na prawym ramieniu $Z i zag��bia si� w nienaturalnym ciele. Wyszarpujesz ze zgrzytem or� pot�nym szarpni�ciem i odskakujesz rozpypuj�c wydarte kawa�ki sztucznego cia�a.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n zamierza si� na $C bior�c pot�ny zamach. Ostrze $f zatacza szeroki �uk i opada z zastraszaj�c� si�� na czerep celu, jednak ze�lizguje si� bez szkody po �bie i l�duje na ramieniu zag��biaj�c si� w twardym korpusie. $n odskakuj�c wyszarpuje b�yszcz�ce ostrze z paskudnym zgrzytem w fontannie iskier i wyszarpni�tych kawa�k�w sztucznego cia�a.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (IS_SET(race_table[GET_RACE(victim)].type , ANIMAL))
				{
					if ( dam <= 0 )
					{
						act( "{5Nie czekaj�c ani chwili d�u�ej, wydajesz z siebie g�o�ny okrzyk i rozpoczynasz bieg w stron� $Z, wysoko wznosz�c $h. Na twe nieszcz�cie, �le wyczu�e� sytuacj�, gdy� twej ofierze wystarczy� jedynie lekki unik, by usun�� si� z pola ra�enia. Twoja bro�, spadaj�ca z wielk� pr�dko�ci� tnie powietrze, a sam z trudem utrzymujesz r�wnowag�.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n nie zastanawiaj�c si� nad lepsz� sposobno�ci� wyprowadza szar�� w stron� $Z. Tym razem najwyra�niej odpowiedni moment nie zosta� wyczuty i zwierzak jednym, lekkim unikiem schodzi z pola ra�enia spadaj�cego z impetem miecza, tym samym prawie posy�aj�c $c na ziemi�.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Nuc�c sw� plemienn� pie�� wojenn�, rzucasz si� z okrzykiem na $C, a $p zaczyna wirowa� w zatrwa�aj�cym tempie. Czujesz, jak twe cia�o jest wprost rozdzierane przez wyt�one w nieludzkim wysi�ku mi�nie, a pulsuj�ce �y�y przebijaj� si� przez sk�r�. B�d�c u kresu drogi, kr�cisz nad g�ow� jeszcze ostatniego m�y�ca g�o�no �wiszcz�cym or�em i wymierzasz pot�ny cios w poprzek cia�a zwierzaka. Ostrze przechodzi na wylot, a dwie po��wki opadaj� w powi�kszaj�c� si� ka�u�� krwi, wci�� jeszcze drgaj�c.{x", ch, wield, victim, TO_CHAR );
						act( "{5Z ust $z dobywa si� zapewne pie�� bitewna, w nieznanym ci narzeczu, a $e bierze rozp�d i rzuca si� na $C, wymachuj�c $j w ob��ka�czym tempie. Widoczna teraz doskonale g�ra mi�ni zaci�ni�ta przez sznury niebieskich, pulsuj�cych �y� wydaje si� wprost rozdziera� $s sk�r�. B�d�c u celu zmusza si� do jeszcze jednego nadludzkiego wysi�ku i wymierza pot�ny cios, tn�c w poprzek cia�a $Z. Dwie po��wki wstrz�sane ostatnimi drgawkami, opadaj� w ka�u�� krwi i nieruchomiej�.{x", ch, wield, victim, TO_NOTVICT );
						make_blood(victim);
					}
					else if (number_range(1,2) == 2 && IS_SET( ch->parts, PART_LEGS ) )
					{
						act( "{5Pod�wiadomie wyczuwaj�c odpowiedni moment, poprawiasz chwyt na r�koje�ci $f i rozpoczynasz i�cie sprinterski bieg w stron� nagle ods�aniaj�cego si� zwierzaka. U celu bierzesz g��boki wdech, wydajesz z siebie szale�czy ryk i tniesz precyzyjnie, podcinaj�c �ci�gna jednej z ko�czyn $Z. Tw�j miecz przechodzi przez mi�sie� jak przez mas�o, wi�c udaje ci si� jeszcze wycofa� i uchyli� przed kontratakiem.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n ni st�d, ni zow�d, zaczyna szale�czo wymachiwa� $j, wt�ruj�c mu przy tym pr�dko�ci� pracy n�g. Bacznym wzrokiem wpatruje si� w sylwetk� $Z. Kiedy nagle zwierz� si� ods�ania, dwur�czny or� spada na ko�czyn� zwierzaka i podcina jeden z mi�ni.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Z krzykiem rzucasz si� na ods�oni�te cia�o $Z i starasz si� wymierzy� ko�cz�cy cios wiruj�c� niesamowicie szybko kling� $f, celuj�c w poprzek linii karku. Twa domniemana ofiara robi jednak gwa�towny unik, a miecz spada tylko na jej korpus rozcinaj�c sk�r� i arterie.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n z krzykiem rzuca si� na ods�oni�te cia�o $Z i wymachuj�c szale�czo $f tnie, celuj�c zapewne w kark ofiary. Zwierz� jednak, przewiduj�c $s zamiary, stara si� odskoczy� jak najdalej, co udaje si� tylko po�owicznie. Ostrze spada na korpus $Z rozcinaj�c sk�r� i arterie.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if ( dam <= 0 )
				{
					act( "{5$p ze �wistem przecina powietrze o w�os chybiaj�c $C.{x", ch, wield, victim, TO_CHAR );
					act( "{5$p $z ze �wistem przecina powietrze o w�os ci� chybiaj�c.{x", ch, wield, victim, TO_VICT );
					act( "{5$p $z ze �wistem przecina powietrze o w�os chybiaj�c $C.{x", ch, wield, victim, TO_NOTVICT );
					return TRUE;
				}
				else if ( victim->hit - dam < -11 )
				{
					int death_mesg[ 5 ], i;

					for ( i = 0; i < 5;i++ )
						death_mesg[ i ] = 0;

					i = 1;

					death_mesg[ 0 ] = 0;

					if ( IS_SET( victim->parts, PART_HEAD ) )
						death_mesg[ i++ ] = 1;
					if ( IS_SET( victim->parts, PART_BRAINS ) )
						death_mesg[ i++ ] = 2;
					if ( IS_SET( victim->parts, PART_HEART ) && wield->value[ 0 ] != WEAPON_AXE )
						death_mesg[ i++ ] = 3;
					if ( !IS_NPC( victim ) && victim->size <= SIZE_MEDIUM )
						death_mesg[ i++ ] = 4;

					switch ( death_mesg[ number_range( 0, i - 1 ) ] )
					{
						case 0:
							act( "{5$p ze �wistem spada na $C g��boko wbijaj�c si� w $S cia�o.{x", ch, wield, victim, TO_CHAR );
							act( "{5$p $z ze �wistem spada na ciebie g��boko wbijaj�c si� w twoje cia�o.{x", ch, wield, victim, TO_VICT );
							act( "{5$p $z ze �wistem spada na $C g��boko wbijaj�c si� w $S cia�o.{x", ch, wield, victim, TO_NOTVICT );
							break;

						case 1:
							act( "{5Uderzenie si�ga g�owy $Z, odcinaj�c j� i odrzucaj�c jej zmasakrowane szcz�tki na ziemi�.{x", ch, wield, victim, TO_CHAR );
							act( "{5$n wyprowadza morderczy cios, przez chwil� �wiat wiruje po czym twoje bezg�owe cia�o osuwa si� na ziemi�.{x", ch, wield, victim, TO_VICT );
							act( "{5$p $z ze �wistem spada na g�ow� $Z odcinaj�c j� od reszty cia�a.{x", ch, wield, victim, TO_NOTVICT );
							if ( IS_NPC( victim ) && victim->pIndexData->corpse_vnum < 0 )
								act( "{5Odci�ta g�owa $Z po chwili znika.{x", ch, wield, victim, TO_ROOM );
							else
								make_head( victim );
							break;

						case 2:
							act( "{5Cios wychodzi ci wyj�tkowo dobrze - rozp�atujesz czaszk� $Z i rozchlapujesz $S m�zg.{x", ch, wield, victim, TO_CHAR );
							act( "{5To nie jest twoja szcz�liwa chwila, $n uderza bardzo mocno rozp�atuj�c ci czaszk� i rozchlapuj�c tw�j m�zg.{x", ch, wield, victim, TO_VICT );
							act( "{5To nie by�a szcz�liwa chwila dla $Z. Uderzenie $z rozp�atuje $O czaszk� rozchlapuj�c m�zg.{x", ch, NULL, victim, TO_NOTVICT );
							make_brains(victim);
							break;

						case 3:
							act( "{5Dobrze wyprowadzony sztych nadziewa $C na ostrze przechodz�c przez samo serce.{x", ch, wield, victim, TO_CHAR );
							act( "{5$n wbija ci ostrze swojej broni prawie po r�koje��, przebijaj�c serce.{x", ch, wield, victim, TO_VICT );
							act( "{5$n wbija $p w $C prawie po r�koje�� przeszywaj�c $S serce.{x", ch, wield, victim, TO_NOTVICT );
							break;

						case 4:
							act( "{5Z pe�nego zamachu tniesz $C w po�owie, rozr�buj�c $S cia�o na dwie cz�ci. Poodr�bywane ko�czyny z chlupotem opadaj� na ziemi�, a ciebie zalewa fontanna krwii{x", ch, wield, victim, TO_CHAR );
							act( "{5$n z pe�nego zamachu tnie ci� na wysoko�ci brzucha, rozr�buj�c na p�.{x", ch, wield, victim, TO_VICT );
							act( "{5Zamaszyste ci�cie $z spada na $C, rozr�buj�c $S cia�o na p�. Poodr�bywane ko�czyny z chlupotem opadaj� na ziemi�, a fontanna krwii zalewa $c.{x", ch, NULL, victim, TO_NOTVICT );
							make_arm(victim);
							make_leg(victim);
							make_blood(victim);
							break;
					}
					return TRUE;
				}
				else
				{
					act( "{5$p ze �wistem spada na $C g��boko wbijaj�c si� w $S cia�o.{x", ch, wield, victim, TO_CHAR );
					act( "{5$p $z ze �wistem spada na ciebie g��boko wbijaj�c si� w twoje cia�o.{x", ch, wield, victim, TO_VICT );
					act( "{5$p $z ze �wistem spada na $C g��boko wbijaj�c si� w $S cia�o.{x", ch, wield, victim, TO_NOTVICT );
					return TRUE;
				}
				break;

			case WEAPON_POLEARM:
				if (!str_cmp( race_table[ GET_RACE(victim) ].name, "zombi" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "kryptowiec" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "upiorny rycerz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "ghul" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "ghast" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "mumia" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "mumia wi�ksza" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "wampir" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Bez uprzedniego zastanowienia, pr�bujesz jak najszybciej zada� bolesny cios niepokoj�cemu ci� martwiakowi. Bierzesz du�y rozp�d, wznosisz nad g�ow� $h i wykonuj�c pot�ny zamach, starasz si� trafi� w tors $Z. Niestety, albo wybra�<&e�/a�/o�> sobie z�ego przeciwnika, albo brakuje ci wprawy, albo niezbyt si� do tego ataku przygotowa�e�. Pud�ujesz o dobre p� metra, a na dodatek z�ego walisz si� jak bela na ziemi�. Nie zastanawiaj�c si� d�u�ej podnosisz si� na nogi i kontynuujesz walk�.{x", ch, wield, victim, TO_CHAR );
						act( "{5Dostrzegasz, jak $n bez uprzedniego zastanowienia, pr�buje jak najszybciej zada� cios niepokoj�cemu go, natr�tnemu martwiakowi. Bierze du�y rozp�d, wznosi nad g�ow� $h i wykonuj�c pot�ny zamach, stara si� trafi� w tors $Z. Niestety, brakuje $m wprawy, albo niezbyt si� do tego ataku przyk�ada, bo pud�uje o dobre p� metra, a na dodatek wali si� jak bela na ziemi�. Nie zastanawiaj�c si� d�u�ej podnosi si� na nogi i kontynuuje walk�.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5K�tem oka spogl�dasz na sw� przysz�� ofiar� i �ciskaj�c kurczowo $h rozpoczynasz szale�czy bieg. W twych oczach zapala si� iskra ob��du, z piersi wydobywa si� duszony dot�d skrz�tnie krzyk. Z sekundy na sekundy nabierasz pr�dko�ci, a twe mi�nie zaczynaj� pracowa� z nadludzk� si��. Gdy dobiegasz do $Z skupiasz si� na swoim celu jak tylko mo�esz i z wielkim impetem, wprawnym ruchem $f dokonujesz ostatecznego ciosu i nie trac�c na szybko�ci biegniesz dalej. Momentalnie do twych uszu dobiega piekielny, �miertelny wrzask i dwa g�uche mla�ni�cia. Obracasz si� na pi�cie i dostrzegasz w ka�uz� krwi, �mierdz�ce trupim jadem wn�trzno�ci i cia�o $Z rozpo�owione na dwie cz�ci.{x", ch, wield, victim, TO_CHAR );
						act( "{5Bacznie obserwuj�c ca�e zaj�cie dostrzegasz jak $n �ciskaj�c kurczowo w obu d�oniach $h koncentruje na $B ca�� sw� uwag� i rozpoczyna szale�czy bieg. Widzisz, jak w $s oczach zapala si� iskra ob��du, a do twych uszu dolatuje wydobywaj�cy si� z piersi atakuj�cego przeci�g�y, og�uszaj�cy krzyk. Z sekundy na sekund� nabieraj�c pr�dko�ci skupia $e si� najbardziej jak tylko mo�e na bezb��dnym wykonaniu pot�nej szar�y. W ko�cu, gdy dobiega do swego celu z wielkim impetem, wprawnym ruchem r�ki dokonuje pot�nego, finalnego ciosu i nie trac�c na szybko�ci biegnie dalej. Momentalnie do twych uszu dobiega piekielny, �miertelny wrzask a twym oczom ukazuje si� wprost makabryczny widok. $N chwiej�c si� na nogach, w przeci�gu sekundy dos�ownie traci ca�y korpus, kt�ry z g�uchym pla�ni�ciem opada na ziemi�. Po chwili w jego �lady id� utrzymuj�ce si� dot�d jakim� cudem pionowo nogi.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Trzymaj�c obur�cz $h wydajesz z siebie d�ugi, szale�czy wrzask i rozpoczynasz �w straszliwy bieg, kt�ry wykonuj� wielcy wojownicy gdy pragn� rozpocz�� lub zako�czy� walk� w �wietnym stylu. Twe stopy miarowo, aczkolwiek bardzo szybko uderzaj� o posadzk�, odg�os twego dono�nego warczenia odbija si� zewsz�d dono�nym, g�uchym echem. Wznosisz wysoko nad g�ow� sw�j or� i b�d�c u celu dokonujesz makabrycznego, pot�nego ci�cia. Ostrze $f ze �wistem przecina powietrze i rozkraja cia�o martwiaka z niesamowit� lekko�ci�. $N wydaje potworny wrzask b�lu, a z wielkiej rany bucha �mierdz�ca, przekl�ta krew i trupi jad.{x", ch, wield, victim, TO_CHAR );
						act( "{5Spogl�dasz z ukosa na $c. Trzymaj�c obur�cz $h wydaje $e z siebie d�ugi, szale�czy wrzask i rozpoczyna �w straszliwy bieg, kt�ry wykonuj� wielcy wojownicy, pragn�c rozpocz�� lub zako�czy� walk� w �wietnym stylu. S�yszysz jak $s stopy miarowo, aczkolwiek bardzo szybko uderzaj� o posadzk�, a odg�os okropnego warczenia odbija si� zewsz�d dono�nym, g�uchym echem. Spogl�dasz na� ponownie. Teraz wznosi wysoko nad g�ow� sw�j or� i b�d�c u celu dokonuje makabrycznego, pot�nego ci�cia. Ostrze $s $f ze �wistem przecina powietrze i rozkraja cia�o $Z z niesamowit� lekko�ci�. O�ywieniec wydaje potworny wrzask b�lu.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Wci�� panuj�c nad sytuacj�, dostrzegasz moment, w kt�rym $C w og�le nie obchodzi co teraz robisz. Nie czekaj�c na kolejn� tak� dobr� sposobno��, oceniasz odleg�o�� mi�dzy twoj� osob�, a t� przebrzyd��, mroczn� kreatur� i rozpoczynasz szar��. B�d�c wystarczaj�co blisko przeciwnika, zni�asz si� nieco, po czym wykonujesz efektywny �lizg pomi�dzy jego nogami unosz�c jak najwy�ej $j. S�yszysz, jak twa bro� z ohydnym mlaskiem przebija si� przez nieroz�o�on� jeszcze, �yw� tkank� martwiaka, po czym do twych uszu dochodzi g�o�ne chrupni�cie, �wiadcz�ce o naruszeniu ko�ci u nogi. Ogl�dasz si� za siebie i widzisz, �e $N w miejscu krocza ma poka�nych rozmiar�w dziur�, a jego noga zwisa zaledwie na paru �ci�gnach.{x", ch, wield, victim, TO_CHAR );
						act( "{5Spogl�daj�c na $c i zauwa�asz, i� na $s ustach pojawia si� szeroki u�miech a twarz nieco si� rozja�nia. Widocznie widzi co�, co $m bardzo w tej chwili pasuje... B�yskawicznie, nie czekaj�c na drugi taki moment rozpoczyna szar��. Gdy jest ju� wystarczaj�co blisko przeciwnika, zni�a si� nieco, po czym wykonuje efektowny �lizg pomi�dzy jego nogami z unosz�c jak najwy�ej $j. Po chwili s�yszysz, jak bro� $z z ohydnym mlaskiem przebija si� przez nieroz�o�on� jeszcze, prawie �yw� tkank� $Z, po czym do twych uszu dochodzi g�o�ne chrupni�cie, �wiadcz�ce o naruszeniu ko�ci u nogi. Nagle, dostrzega� jak porz�dny kawa� cia�a $Z, urz�duj�cy dot�d w miejscu krocza oddziela si� od reszty i z g�o�nym plaskiem upada na ziemi�.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (!str_cmp( race_table[ GET_RACE(victim) ].name, "duch" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "banshee" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "cie�" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "widmo" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "zmora" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "upi�r" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Z wyci�gni�t� przed siebie broni� p�dzisz w d�ugich susach w kierunku $Z. Dopadasz do $S sylwetki w kilku szale�czych skokach, unosisz or� i tniesz z ca�ych si�a znad g�owy. Przeciwnik b�yskawicznie uchodzi w bok unikaj�c rozp�dzonego ostrza, kt�re wbija si� g��boko w pod�o�e. Z ca�ych si� szarpiesz drzewce pr�buj�c uwolni� zaklinowan� bro�. Niematerialna sylwetka zbli�a si� do ciebie b�yskawicznie z szale�czym wyciem triumfu rozlegaj�cym si� w twym umy�le. Zataczaj�c si� od og�uszaj�cego wrzasku w ostatniej chwili, resztk� si� wyszarpujesz $h unikaj�c pot�nych cios�w.{x", ch, wield, victim, TO_CHAR );
						act( "{5Patrzysz jak $n w kilku szale�czych skokach dopada $C z $j wysuni�t� przed siebie. Unosi sw�j or� i z szybko�ci� wichru opuszcza ostrze. P�materialny przeciwnik uchodzi b�yskawicznie w bok unikaj�c po��tego ciosu, a rozp�dzona bro� wbija si� z dono�nym, metalicznym brz�kiem w pod�o�e. $n szarpie z ca�ych si� drzewce $f pr�buj�c uwolni� zaklinowany or�.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Wymachuj�c w�ciekle $j ruszasz w stron� $Z, $E cofa si� straciwszy orientacj� pod naporem twojej furii. B�yszcz�ce ostrze ze �wistem opada na p�materialn� szyj� przeciwnika i przechodzi przez ni� bezszelestnie. Odci�ta g�owa unosi si� chwil� nad reszta astralnego cia�a i niknie po chwili w purpurowym rozb�ysku. Niematerialna sylwetka opada powoli na ziemi� wal�c si� na kolana. Z bezg�owego cia�a tryska gejzer czystej, duchowej energii o�lepiaj�c ci�. Potrz�saj�c g�ow� mocniej �ciskasz drzewce $f i tniesz w korpus od g�ry. Przy zetkni�ciu ostrza z cia�em uwalnia si� reszta purpurowej energii i przeciwnik niknie w piekielnych konwulsjach, w jaskrawej, bezg�o�nej eksplozji.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n naciera z furi� na $C, $s $h zatacza �wietliste kr�gi gdy zbli�a si� do cofaj�cego si� przeciwnika. Szerokie ostrze w pot�nym ci�ciu opada na ods�oni�t�, p�materialn� szyj�. Przechodzi przez ni� bezszelestnie odcinaj�c p�przezroczyst� g�ow�. Unosi si� ona przez mgnienie oka nad reszt� cia�a po czym niknie w jasnym, purpurowym rozb�ysku. Niematerialna sylwetka powoli opada na pod�o�e osuwaj�c si� na kolana. Z bezg�owego cia�a tryska gejzer duchowej energii ra��c oczy. Po chwili widzisz jak $n poprawia chwyt swej broni i tnie od g�ry z ca�ych si� nieruch� sylwetk� $Z. Astralne cia�o po zetkni�ciu z rozp�dzonym ostrzem niknie, szarpi�c si� w�ciekle w jaskrawej bezg�o�nej eksplozji upiornego, purpurowego blasku.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Zbli�asz si� szybkimi krokami do przeciwnika mocno �ciskaj�c drzewce swej broni. W ka�dej chwili jeste� gotow<&y/a/e> do zadania decyduj�cego ciosu. Czekasz na odpowiedni moment. Lawiruj�c w powietrzu $N �widruje ci� na wskro� pot�pie�czym, pe�nym b�lu i nienawi�ci wzrokiem. Wyci�ga p�materialne ramiona w twoim kierunku pr�buj�c zada� cios ods�aniaj�c si� przy tym. Korzystaj�c z okazji w mgnieniu oka unosisz $h, bierzesz szeroki zamach i tniesz z ca�ych si� r�wnocze�nie schodz�c z linii ataku przeciwnika. Ostrze zag��bia si� bezszelestnie w p�przezroczystym ciele. W twej czaszce rozbrzmiewa pe�en rozpaczy i cierpienia, niezno�ny wrzask.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak $n i $N �widruj� si� na wskro� pe�nym nienawi�ci wzrokiem. P�materialna sylwetka z gracj� lawiruje tu� nad ziemi� okr��aj�c $c. Przeciwnicy obserwuj� si� nawzajem i wyczekuj� nawzajem odpowiedniej chwili by zaatakowa�. $N nagle rzuca si� z wyci�gni�tymi ramionami na wroga. $n w mgnieniu oka unosi $h i tnie z ca�ych si�. �wiszcz�ce ostrze bezg�o�nie zatapia si� w niematerialnym ciele, a o�lepiaj�cy, czerwony blask wydobywaj�cy si� z rozerwanego korpusu razi twe oczy.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Odskakujesz krok w ty� oddalaj�c si� nieznacznie od $Z. B�d�c poza zasi�giem �mierciono�nych, niematerialnych ramion wykorzystujesz zasi�g swej broni i wyprowadzasz mordercze ci�cie. Ca�a sytuacja trwa zaledwie u�amek sekundy i p�przezroczysty przeciwnik nie ma czasu, aby zej�� z toru ciosu. Szerokie ostrze zahacza o korpus wroga zostawiaj�c g��bokie, w�skie naci�cie na klatce piersiowej. Z rany s�czy si� purpurowa po�wiata tworz�c fantastyczne rozb�yski. Wyj�c z w�ciek�o�ci i b�lu $N rzuca si� w twoim kierunku.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n odskakuje o krok od $Z zwi�kszaj�c dystans. Wykorzystuj�c zasi�g swej broni bierze szeroki zamach i wyprowadza mordercze ci�cie. Ca�a sytuacja trwa zaledwie kilka u�amk�w sekundy. Niematerialny przeciwnik lawiruje w powietrzu uchodz�c nieznacznie w bok, jednak szerokie ostrze zag��bia si� w $S korpusie pozostawiaj�c po sobie g��bokie, szerokie naci�cie. Z klatki piersiowej s�czy si� purpurowa po�wiata, kt�ra niknie po chwili.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (!str_cmp( race_table[ GET_RACE(victim) ].name, "drakolicz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "licz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "szkielet wojownik" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "szkielet" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Starasz si� wykona� $j pot�ny atak celuj�c w korpus $Z, ale $S unik jest szybszy. Odskakuje $E przed ci�ciem i naciera na zza twoich plec�w. Staraj�c si� nie oberwa�, kontynuujesz walk�.{x", ch, wield, victim, TO_CHAR );
						act( "{5K�tem oka widzisz, jak $n wybiega naprzeciw $Z i stara si� wykona� pot�ny atak. Wzniesiona wysoko, wypolerowana bro� b�yszczy si� o�lepiaj�c� jasno�ci�. Niestety, spadaj�c na wrog� muska zaledwie jego rami�, a ten z kolei wymijaj�c zr�cznie jej ostrze zaczyna naciera� na $c od ty�u.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Znajduj�c bardzo wygodn� dla siebie pozycj� na wykonanie pot�nej szar�y bierzesz g��boki oddech i wznosz�c wysoko $h rozpoczynasz natarcie. Stopniowo nabierasz na pr�dko�ci, a twe mi�nie pracuj� coraz szybciej i wydajniej. B�d�c u celu zaskakujesz $C zwinnym unikiem i wykonuj�c efektowny obr�t dokonujesz makabrycznego ci�cia. W przeci�gu sekundy odci�ta czaszka niezorientowanego w sytuacji $Z spada na ziemi� i z g�uchym szcz�kiem toczy si� jeszcze par� metr�w po posadzce, po czym podobnie jak reszta ju� bezu�ytecznych ko�ci zamiera w bezruchu.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n bierze g��boki oddech i rozpoczyna szale�czy bieg. Unikaj�c zr�cznie atak�w, wykonuje efektowne ci�cie $j z p�obrotu. Pusta czaszka, odci�ta od reszty cia�a spada swobodnie w d� i z g�uchym szcz�kiem toczy si� jeszcze par� metr�w po ziemi. Reszta ko��ca z �oskotem wali si� na posadzk� i rozsypuje si� w drobny mak.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Zaciskaj�c mocniej swe d�onie na trzonku $f rozpoczynasz szale�czy bieg i zmuszasz sw�j organizm do wytworzenia dodatkowej dawki adrenaliny. B�d�c u celu bierzesz pot�ny zamach i wyprowadzasz atak skierowany na nogi $Z. S�ycha� trzask nad�amywanych ko�ci i niemi�osierne piszczenie or�a. Tw�j przeciwnik kuleje, a ty ponownie nacierasz.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz, jak $n zaciska najmocniej jak tylko mo�e d�onie na trzonku $f, po czym rozpoczyna szale�czy bieg. B�d�c u celu bierze $e pot�ny zamach i wyprowadza silny atak skierowany na nogi $Z. S�ycha� trzask nad�amywanych ko�ci i niemi�osierne piszczenie or�a.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Bez namys�u, z wielkim zapa�em i bojowym krzykiem rzucasz si� na $C. Pokonuj�c paroma wielkimi susami dziel�c� was odleg�o��, skupiasz si� na celu jak najbardziej tylko mo�esz, a w twych oczach zapala si� iskra ob��du. Bierzesz pot�ny zamach i tniesz jak najsilniej tylko mo�esz korpus $Z, $p wbija si� stosunkowo g��boko pomi�dzy ko�ci i wyszarpuje par� z nich, inne tn�c lub wybijaj�c ze staw�w.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n bez namys�u i z wielkim, bojowym zapa�em rzuca si� na $C. Pokonuj�c paroma susami dziel�c� ich odleg�o�� wznosi wysoko nad g�ow� $h i jak tylko najsilniej mo�e tnie korpus $Z. Jego $p wbija si� stosunkowo g��boko pomi�dzy ko�ci i wyszarpuje par� z nich, inne tn�c lub wybijaj�c ze staw�w.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (IS_SET(victim->form, FORM_CONSTRUCT))
				{
					if ( dam <= 0 )
					{
						act( "{5Szar�ujesz na $C z wyci�gni�t� przed siebie broni�, $E stoi nieruchomo wpatruj�c si� w ciebie bezmy�lnie. Zbli�asz si� do celu z ka�dym krokiem. Nie zatrzymuj�c biegu wykonujesz zab�jcze pchni�cie w klatk� piersiow� nieruchomego wroga, jednak ten unosi w g�r� mocarne rami� odbijaj�c drzewce $f. Ze zduszonym j�kiem, pr�buj�c wyhamowa� szale�czy p�d zderzasz si� ze sztuczn� sylwetk�. Odbijasz si� od przeciwnika i oszo�omion<&y/a/e> zataczasz par� metr�w w ty�.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n p�dzi szale�czo na stoj�cego nieruchomo przeciwnika z wyci�gni�tym przed siebie or�em. Nie zwalniaj�c biegu wyprowadza mordercze pchni�cie skierowane w klatk� piersiow� $Z, $E jednak unosi w g�r� mocarne rami� i z zastraszaj�c� si�a odbija drzewce $f. $n w panice pr�buje os�abi� impet biegu, jednak na pr�no, zderza si� pot�nie ze sztuczn� sylwetk� $Z i odbija par� metr�w w ty�.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Wykorzystuj�c zasi�g $f wyprowadzasz pot�ne ci�cie skierowane w poprzek korpusu przeciwnika. Twoje ramiona napinaj� si� jak postronki kiedy zaciskasz je z potworn� si�� na d�ugim, wy�lizganym drzewcu. Ostrze zatacza szeroki, morderczy �uk b�yszcz�c w�ciekle i opada z �oskotem tu� powy�ej linii bioder $Z. Or� wnika g��boko w sztuczny korpus z dono�nym trzaskiem. Kaskady iskier i od�amk�w strzelaj� g�stymi snopami na wiele metr�w wok� zasypuj�c wszystko, a $p tnie kad�ub z nieprzyjemnym zgrzytem i przechodzi na wylot. Odskakujesz na bezpieczn� odleg�o�� i obserwujesz jak $N l�duje na ziemi z �oskotem i rozsypuje si� na kilkadziesi�t cz�ci.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n wykorzytsuj�c zasi�g $f wyprowadza pot�ne ci�cie na ukos, przez korpus przeciwnika. Ramiona $s napinaj� si� jak postronki kiedy �ciska z ca�ych si� wy�lizgane drzewce. Rozp�dzone ostrze tn�c powietrze ze �wistem z potworn� si�� zag��bia si� w sztuczne cia�o tu� nad lini� bioder $Z. Kaskady iskier i od�amk�w strzelaj� grubymi snopami na wiele metr�w wok�. Or� ze zgrzytem i metalicznym brz�kiem przechodzi na wylot. $n odskakuje na bezpieczn� odleg�o�� i z satysfakcj� obserwuje swoje dzie�o zniszczenia. Kad�ub przeciwnika machaj�c niezdarnie ramionami zsuwa si� powoli wzd�u� linii ci�cia, opada z �oskotem na pod�o�e i rozpada na kilkadziesi�t kawa�k�w. Po chwili, ko�ysz�c si� przez u�amek sekundy, dolne ko�czyny $Z dziel� ten sam los.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Kr�c�c m�y�ca $j nad g�ow� w kilku susach dobiegasz do przeciwnika. Wykonuj�c nag�y skr�t cia�a mijasz go, zapierasz nogami o pod�o�e i stajesz przodem do jego plec�w. Wnosisz sw� bro� wysoko nad g�ow� i wyprowadzasz szale�cze ci�cie prosto mi�dzy �opatki $Z. Uderzeniu towarzyszy og�uszaj�cy, zgrzytliwy �oskot. Snopy iskier i od�amk�w zasypuj� ci twarz i o�lepiaj� na chwil�. Odzyskuj�c wzrok wyszarpujesz ostrze broni, kt�re tkwi�o g��boko w korpusie wroga.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n szale�czo wymachuj�c $j nad g�ow� w d�ugich susach zbli�a si� do $Z. Wykonuj�c nag�y skr�t cia�a mija przeciwnika i staje przodem do jego plec�w. Wznosi sw�j or� w pot�nym ci�ciu. Ostrze b�yszcz�c w�ciekle zatacza szeroki �uk i ze �wistem opada prosto mi�dzy �opatki, a eksplozja iskier i od�amk�w zasypuje sylwetk� $z. Kiedy opadaj�, napastnik wyszarpuje or� z ha�a�liwym zgrzytem.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Wyczekujesz odpowiedniego momentu do zadania ciosu. $N stawiaj�c kolejny oci�a�y krok ods�ania si� na chwil�. Nie czekaj�c d�u�ej b�yskawicznie wyprowadzasz pot�ne pchni�cie w bok przeciwnika. Wyrzucone ostrze tn�c ze �wistem powietrze p�dzi ku przeznaczeniu z ogromn� pr�dko�ci�, jednak przeciwnik w por� zauwa�a zagro�enie i zas�ania si� ramieniem. Or� zag��bia si� w nim z ha�a�liwym zgrzytem przechodz�c na wylot. Wyszarpuj�c bro� tworzysz spor� wyrw� w przedramieniu wroga.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n wyczekuje odpowiedniego momentu do zadania ciosu. Kiedy $N stawia kolejny niezdarny krok, barbarzy�ca z b�yskiem w oku wyprowadza pot�ne pchni�cie w bok wroga. Ostrze tn�c powietrze z zastraszaj�c� si�� w�druje ku przeznaczeniu, jednak $N w por� unosi szerokie rami� w obronnym ge�cie i zas�ania si�. Or� wbija si� w nie ze zgrzytem i przechodzi na wylot. $n wyszarpuj�c bro� tworzy spor� wyrw� w przedramieniu przeciwnika.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if ( dam <= 0 )
				{
					act( "{5Szar�ujesz na $C jednak nie udaje ci si� wyprowadzi� silnego ciosu, drzewce broni wy�lizguje ci si� lekko i z trudem zamachujesz si� i wykonujesz ci�cie.{x", ch, wield, victim, TO_CHAR );
					act( "{5$n szar�uje na ciebie, jednak nie udaje $O si� wyprowadzi� silnego ciosu, z trudem zamachuje si� i wykonuje ci�cie.{x", ch, wield, victim, TO_VICT );
					act( "{5Widzisz jak $n szar�uje na $C, jednak nie udaje $o si� wyprowadzi� silnego ciosu, z trudem zamachuje si� i wykonuje ci�cie.{x", ch, wield, victim, TO_NOTVICT );
					return TRUE;
				}
				else if ( victim->hit - dam < -11 )
				{
					switch ( number_range( 0, 3 ) )
					{
						case 0:
							act( "{5Patrzysz na swojego wroga i wiesz, �e to ju� jego koniec. Nabierasz rozp�du wykonuj�c kilka pe�nych obrot�w. Twoja bro� zaczyna obraca� si� razem z tob�, a gdy zbli�asz si� do $Z bez trudu odcinasz $S g�ow� nie trac�c nawet impetu. Po chwili uspokajasz si�.{x", ch, wield, victim, TO_CHAR );
							act( "{5Dostrzegasz na sobie parali�uj�ce spojrzenie $z, kt�r$r nabiera rozp�du wykonuj�c kilka obrot�w. Czas zwalnia kiedy ostrze przecina najpierw sk�r�, mi�nie i gard�o a ca�e twoje �ycie przelatuje ci przed oczyma. Ostatni� rzecz jak� dostrzegasz to zbli�aj�ca si� ziemi�.{x", ch, wield, victim, TO_VICT );
							act( "{5Widzisz jak $n nabiera rozp�du wykonuj�c kilka pe�nych obrot�w. Gdy zbli�a si� do $Z bez trudu odcina $O g�ow� nie trac�c nawet impetu.{x", ch, wield, victim, TO_NOTVICT );
							if ( IS_NPC( victim ) && victim->pIndexData->corpse_vnum < 0 )
								act( "{5Odci�ta g�owa $Z po chwili znika.{x", ch, wield, victim, TO_ROOM );
							else
								make_head( victim );
							break;

						case 1:
							if ((IS_SET(victim->parts, PART_ARMS) && IS_SET( victim->form, FORM_WARM )) ||
									(IS_SET(victim->parts, PART_ARMS) && IS_SET( victim->form, FORM_COLD_BLOOD )))
							{
								act( "{5Z niesamowit� zwinno�ci� podskakujesz do $Z, wykonujesz pe�ny zamach i trafiasz prosto w $S rami�, ma�o brakowa�o a by�aby to szyja. Jednak�e i taki cios jest skuteczny, ostrze gruchocze ko�ci ramienia $Z, przecina p�uca, �o��dek i w�trob�, a zatrzymuje si� dopiero na miednicy. Odr�bana r�ka $Z upada na ziemi�, a z kikutu ramienia sika krew jak z wodospadu.{x", ch, wield, victim, TO_CHAR );
								act( "{5$n w �miertelnym ta�cu podskakuje do ciebie i wykonuj�c pe�ny zamach trafia prosto w twe rami�, ma�o brakowa�o a by�aby to szyja. Jednak i taki cios wystarcza, by� straci� wszystko, co posiadasz. Robi ci si� ciemno przed oczami, czujesz tylko dziwn� lekko�� po prawej stronie cia�a. Starasz si� zaczerpn�� powietrze, ale jaki� p�yn chyba wype�ni� twe p�uca, gdy� tylko si� krztusisz. Potem ju� nic nie czujesz.{x", ch, wield, victim, TO_VICT );
								act( "{5Widzisz jak $n w szalonym ta�cu podskakuje do $Z i wykonuj�c pe�ny zamach trafia prosto w $S rami�, ma�o brakowa�o a by�aby to szyja. Jednak�e i taki cios jest skuteczny, ostrze gruchocze ko�ci ramienia $Z, przecina p�uca, �o��dek i w�trob�, a zatrzymuje si� dopiero na miednicy. Odr�bana r�ka $Z upada na ziemi�, a z kikutu ramienia sika krew jak z wodospadu..{x", ch, wield, victim, TO_NOTVICT );
								make_arm(victim);
								break;
							}
						case 2:
							if ( IS_SET( victim->form, FORM_WARM ) || IS_SET( victim->form, FORM_COLD_BLOOD ) )
							{
								act( "{5Z niesamowit� zwinno�ci� podskakujesz do $Z, wykonujesz pe�ny zamach rozcinaj�c jedn� z t�tnic. $N upada i w kilka chwil wykrwawia si� na �mier�.{x", ch, wield, victim, TO_CHAR );
								act( "{5$n w �miertelnym ta�cu podskakuje do ciebie i wykonuj�c pe�ny zamach rozcina ci jedn� z t�tnic. Upadasz czuj�c przera�liwy b�l i up�ywaj�c� krew z uda. Chwile wyd�u�aj� si� po czym robi si� ciemno i zimno.{x", ch, wield, victim, TO_VICT );
								act( "{5Widzisz jak $n w szalonym ta�cu podskakuje do $Z i wykonuj�c pe�ny zamach rozcina jedn� z t�tnic. $N upada i w kilka chwil wykrwawia si� na �mier�.{x", ch, wield, victim, TO_NOTVICT );
								make_blood(victim);
								break;
							}
						case 3:
							if ((IS_SET(victim->parts, PART_GUTS)))
							{
								act( "{5Rozp�dzasz si� i wykonujesz wspania�e ci�cie! Brzuch $Z rozrywa si� okazale prezentuj�c ostatni posi�ek nieboszczyka, flaki i inne bli�ej niezidentyfikowane obiekty.{x", ch, wield, victim, TO_CHAR );
								act( "{5Widzisz jak $n rozp�dza si� i wykonuje, na twoje nieszcz�cie, doskona�e ci�cie! Czujesz si� bardzo lekko, powoli odp�ywa od ciebie �wiadomo�� i �ycie...{x", ch, wield, victim, TO_VICT );
								act( "{5Widzisz jak $n rozp�dza si� i wykonuje wspania�e ci�cie! Brzuch $Z rozrywa si� okazale prezentuj�c ostatni posi�ek nieboszczyka, flaki i inne bli�ej niezidentyfikowane obiekty.{x", ch, wield, victim, TO_NOTVICT );
								break;
							}
						default:
								act( "{5Rozp�dzasz si� i wykonujesz wspania�e ci�cie!{x", ch, wield, victim, TO_CHAR );
								act( "{5Widzisz jak $n rozp�dza si� i wykonuje, na twoje nieszcz�cie, doskona�e ci�cie! Czujesz, �e powoli odp�ywa od ciebie �wiadomo�� i �ycie...{x", ch, wield, victim, TO_VICT );
								act( "{5Widzisz jak $n rozp�dza si� i wykonuje wspania�e ci�cie!{x", ch, wield, victim, TO_NOTVICT );
								break;
					}
					return TRUE;
				}
				else
				{
					int types = 1;

					if ( ch->class == CLASS_BARBARIAN && is_affected(ch, gsn_berserk ) )
						types = 2;

					switch ( number_range( 0, types ) )
					{
						case 0:
							act( "{5Szar�ujesz na $C mocno trzymaj�c drzewce $f, w biegu wykonujesz pe�en obr�t i tw�j cios trafia z pot�nym impetem rozrywaj�c $C!{x", ch, wield, victim, TO_CHAR );
							act( "{5$n szar�uje na ciebie, w biegu wykonuje pe�en obr�t i jego cios trafia ci� z pot�nym impetem! Czujesz przera�liwy b�l gdy $p rozrywa twoje cia�o.{x", ch, wield, victim, TO_VICT );
							act( "{5Widzisz jak $n szar�uje na $C wykonuj�c przy tym pe�en obr�t. $s cios trafia z pot�nym impetem rozrywaj�c $C.{x", ch, wield, victim, TO_NOTVICT );
							break;

						case 1:
							act( "{5Szar�ujesz na $C utrzymuj�c $p nad g�ow�, zatrzymujesz si� nagle tu� przed swoim przeciwnikiem wykonuj�c pot�ny zamach! Tw�j cios dewastuje $C trafiaj�c centralnie w klatk� piersiow�.{x", ch, wield, victim, TO_CHAR );
							act( "{5$n szar�uje na ciebie trzymaj�� $p wysoko nad g�ow�, zamachuje si� pot�nie trafiaj�c ci� w klatk� piersiow�! Prawie mdlejesz gdy potworny b�l przechodzi przez twoje cia�o.{x", ch, wield, victim, TO_VICT );
							act( "{5Widzisz jak $n szar�uje na $C trzymaj�c $p wysoko nad g�ow�, zatrzymuje si� nagle przed swoim przeciwnikiem wykonuj�c pot�ny zamach! $s cios trafia i dewastuje $C trafiaj�c w klatk� piersiow�.{x", ch, wield, victim, TO_NOTVICT );
							break;

						case 2:
							act( "{5W kilku dziwacznych podskokach zbli�asz si� do $Z, wykonujesz kr�tki obr�t ca�ym cia�em i precyzyjnie trafiasz $j rozcinaj�c tkanki.{x", ch, wield, victim, TO_CHAR );
							act( "{5$n w kilku dziwacznych podskokach zbli�a si� do ciebie, wykonuje kr�tki obr�t ca�ym cia�em i precyzyjnie trafia ci� rozcinaj�c tkanki!{x", ch, wield, victim, TO_VICT );
							act( "{5Widzisz jak $n zbli�a si� w kilku dziwacznych podskokach do $Z, wykonuje kr�tki obr�t ca�ym cia�em i precyzyjnie trafia $j rozcinaj�c tkanki.{x", ch, wield, victim, TO_NOTVICT );
							break;
					}
					return TRUE;
				}
				break;

			case WEAPON_SPEAR:
				if (!str_cmp( race_table[ GET_RACE(victim) ].name, "zombi" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "kryptowiec" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "upiorny rycerz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "ghul" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "ghast" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "mumia" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "mumia wi�ksza" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "wampir" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Spogl�dasz na miotaj�c� si� sylwetk� przeciwnika zimno kalkuluj�c sytuacj�. Wyczekujesz odpowiedniego momentu, aby zada� mordercze pchni�cie $j. Kiedy tylko nadarza si� okazja d�gasz z ca�ych si� kieruj�c cios w ods�oni�ty brzuch przeciwnika. Ten wykonuje b�yskawiczny skr�t cia�a i tw�j grot mija sw�j cel tn�c ze �wistem powietrze poci�gaj�c ci� za sob�. $N bierze pot�ny zamach i wyrzuca w twym kierunku z potworn� si�� swe przegni�e rami� w mia�d��cym ciosie. Widzisz to jedynie k�tem oka i lekko uchylasz g�ow�, p�dz�c wci�� naprz�d.{x", ch, wield, victim, TO_CHAR );
						act( "{5Patrzysz jak $n w�ciekle d�ga $C wykorzystuj�c luk� w $S obronie. Jednak przeciwnik wykonuje b�yskawiczny unika i odpowiada morderczym ciosem. Tak gwa�towne ruchy powoduj�, �e gubi przy tym p�aty cia�a, przez kt�re prze�wituj� po��k�e ko�ci. Impet niecelnego d�gni�cia $j powoduje, �e nacieraj�cy niezgrabnie sunie naprz�d zr�wnuj�c si� z przeciwnikiem.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Warcz�c w�ciekle bierzesz pot�ny zamach i wykonujesz od do�u pot�ne pchni�cie w brzuch przeciwnika. Grot wchodzi g�adko i wychodzi przez rami� $Z wbijaj�c si� dodatkowo w przekrzywion� g�ow�. Potworne konwulsje wstrz�saj� cia�em o�ywie�ca. Z $S ust wydobywa si� zgrzytliwy, dono�ny, przed�miertny skrzek. Porusza w�ciekle ko�czynami w ostatnich drgawkach. Zapierasz si� z ca�ych si�, unosisz $h w powietrze poci�gaj�c tym samym cuchn�ce �cierwo przeciwnika. Nag�ym, silnym ruchem t�uczesz nim o ziemi�. Uderzenie jest tak mocne, �e przy zetkni�ciu cia�a z pod�o�em s�yszysz d�wi�k p�kaj�cych ko�ci i wn�trzno�ci oraz chrz�st rozrywanych mi�ni i �ci�gien. Zalewa ci� fontanna plugawych trupich sok�w. Twoje wycie triumfu rozlega si� po okolicy.{x", ch, wield, victim, TO_CHAR );
						act( "{5Patrzysz jak $n przykuca i wykonuje �ukowe d�gni�cie skierowane z do�u, ku g�rze. Cios dosi�ga brzucha $Z i przechodzi przez ca�y korpus ko�cz�c sw� mordercz� w�dr�wk� w gnij�cej czaszce. Nieumar�y szarpie si� w ostatnich podrygach. Z jego wykrzywionych b�lem i w�ciek�o�ci� warg wydobywa si� zgrzytliwy skrzek rani�cy uszy. $n wyt�a swe ramiona i unosi $h w powietrze wraz ze �cierwem ofiary. Nagle opuszcza j� z ca�ych si� i t�ucze o pod�o�e. Impet walni�cia jest tak silny, �e dos�ownie mia�d�y martwe cia�o. Fontanna pop�kanych wn�trzno�ci, ko�ci oraz cuchn�cych p�yn�w zalewa dos�ownie wszystko. Z gard�a $z wydobywa si� potworne wycie triumfu.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Warcz�c w�ciekle patrzysz w przegni�� twarz $Z. Zapierasz si� mocno nogami w pod�o�e i wykonujesz b�yskawiczne pchni�cie prosto w klatk� piersiow� przeciwnika. Grot $f wbija si� g��boko z mokrym chrz�stem w toczone zgnilizn� cia�o wroga. Szarpiesz broni� na lewo i prawo dewastuj�c rozmi�k�e wn�trzno�ci o�ywie�ca. Twym morderczym zabiegom towarzyszy przera�liwe wycie. $N miota si� w swym szale�czym cierpieniu chwytaj�c rozszarpuj�ce mu cia�o drzewce. Pr�buje zatrzyma� w�ciekle ta�cz�cy w jego wn�trzno�ciach grot. Zostawiaj�c wrogowi w piersiach ziej�c� jam�, z kt�rej bucha strumie� trupiego jadu wyszarpujesz $h.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak $n staje w lekkim wykroku i z mordercz� si�� oraz precyzj� wykonuje pchniecie $j. Cios trafia w klatk� piersiow� $Z. Grot broni wbija si� z chrz�stem w plugawe cia�o mieszaj�c si� ze skrzekliwym wyciem nieumar�ego. $n szarpie $j na wszystkie strony miotaj�c martwym cia�em przeciwnika. O�ywieniec chwyta drzewce broni chc�c zatrzyma� jej mordercz� w�dr�wk�, na pr�no jednak. W jego korpusie zionie olbrzymia dziura z wystaj�cymi, po�amanymi ko��mi oraz zwisaj�cymi, pop�kanymi wn�trzno�ciami. Atakuj�cy z mokrym mla�ni�ciem wyszarpuje sw� bron i szykuje si� do kolejnego natarcia.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5P�dzisz szale�czo na $C opieraj�c koniec drzewca $f przy prawym biodrze. Spowalnia to nieco bieg, lecz masz przez to lepszy punkt oparcia i mo�esz manewrowa� sw� broni�. Wbijasz grot g��boko w cuchn�ce zgnilizn� cia�o przeciwnika i przeci�gasz go silnym szarpni�ciem w g�r� rozrywaj�c przegni�y korpus o�ywie�ca. Z olbrzymiego rozci�cia wygl�daj� poszarpane wn�trzno�ci oraz chlupie cuchn�ca ciecz. Skrzekliwym warkotem $N daje zna�, �e odczu� ten cios. U�miechaj�c si� dziko chwytasz lu�no sw� bro�.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz szale�czy bieg $z szar�uj�cego na $C. Kiedy z impetem wbija $h w przegni�e cia�o przeciwnika na jego twarzy pojawia si� grymas b�lu. Szeroki grot broni zag��bia si� w brzuchu nieumar�ego. Atakuj�cy szarpie $j kieruj�c j� ku g�rze co powoduje rozleg�e obra�enia na korpusie o�ywie�ca. Ze szramy $Z tryska p�przezroczysta ciecz oraz strzelaj� poszarpane wn�trzno�ci zwisaj�c lu�no. Z gard�a $z wydobywa si� szale�czy �miech.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (!str_cmp( race_table[ GET_RACE(victim) ].name, "duch" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "banshee" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "cie�" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "widmo" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "zmora" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "upi�r" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Wyskakujesz do g�ry, celuj�c $j w $C, jednak pech sprawi� i� tw�j cel uchyli� si� i twa bro� wbi�a si� w ziemie tu� obok niematerialnego cia�a $Z.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak $n wyskakuje do g�ry i celuje sw� broni� w $C, jednak cel uchyli� si� i $f $z wbija si� w ziemi�, tu� obok niematerialnego cia�a.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Bierzesz pot�ny rozbieg i z w�ciek�o�ci� rzucasz si� na $C. Twa bro� zatacza kr�gi wzniesiona wysoko i b�yszczy upiornie gdy tw�j rze�nicki cios spada na pr�buj�cego si� os�oni� przeciwnika. Grot $f g�adko wchodzi w eteryczne cia�o tn�c je i rozrywaj�c, jego kawa�ki odpadaj� i znikaj� po chwili. Suniesz g�adko, od g�owy, przez r�ce po sam d�. Wstajesz z kl�czk�w i z satysfakcj� patrzysz jak dwie cz�sci $Z rozp�ywaj� si� powoli a pot�piencze j�ki gasn� w niebycie.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n bierze pot�ny rozbieg i z w�ciek�ym rykiem rzuca si� na $C. Grot $f zatacza kr�gi, wznie�ione wysoko upiornie b�yszczy gdy rz�nicki cios spada na pr�buj�cego si� os�oni� ducha. Bro� $z g�adko wchodzi w eteryczne cia�o, tn�c je, rozrywaj�c. Odpadaj�ce kawa�ki znikaj� po chwili, roztapiaj� si� od��czone od swego niematerialnego cia�a. Cios przeszed� od g�ry po sam d�. Po chwili $n wstaje i z satysfakcj� patrzy jak powoli rozp�ywaj� si� dwie cz�sci ducha a pot�piencze j�ki cichn� coraz bardziej.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Bierzesz kr�tki rozbieg i odbijasz si� od ziemi. Twa bro� z olbrzymi� pr�dko�ci� trafia w p�materialne cia�o $Z i przeszywaj�c je na wylot jak n� wbija si� w mas�o. $N wydaje z siebie pot�pieczy j�k, jakby si� do czego� szykowa�, jednak szybko wyci�gasz $h z $S powoduj�c wielkie zniszczenia.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n bierze kr�tki rozbieg i odbija si� od ziemi. Jego bro� bezb��dnie trafia w niematerialne cia�o $Z przeszywaj�c je na wylot i czyni�c wielkie zniszczenia. $N wydaje z siebie pot�pienczy j�k i probowa� uderzy� $c, jednak niezd��a si� nawet powa�nie zamachn�� kiedy przeciwnik wyci�ga z sw� bro� rani�c $C jeszcze powa�niej.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Z olbrzymi� pr�dkosci� i dzikim rykiem rzucasz si� na $C, wymachuj�c $j. Przeciwnik pr�bowa� uchyli� si�, lecz twa bro� przyszpili�� go do ziemi z ogromn� si��. Pot�pienczy j�k, jaki wyda� duch sprawi�, �e a� zabola�y ci� uszy. Jednak z g�o�nym warkotem wyszarpno�e� bro� z niematerialnego, szamocz�cego si� bez�adnie cia�a. $N rzuca si� do walki z wsciek�o�ci�.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n z olbrzymi� pr�dkosci� i dzikim rykiem rzuca si� na $C, wymachuj�c dooko�a $j. Unik, jaki pr�bowa� zrobi� cel, nie uda� si� i bro� $z przyszpili�a go do ziemi z wielk� si��. $N wrzasn�� pot�pi��czo, po czym $n sprawnym ruchem wyszarpn�� bro� z szamocz�cego si�, niematerialnego cia�a, kt�re jednak podnios�o si� i rzuci�o do walki z wsciek�osci�. Wszystko sta�o si� w jednej chwili.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (!str_cmp( race_table[ GET_RACE(victim) ].name, "drakolicz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "licz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "szkielet wojownik" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "szkielet" ))
				{
					if ( dam <= 0 )
					{
						act( "{5$N z b�yszcz�c� w pustych oczodo�ach furi� naciera na ciebie. B�yskawicznie blokujesz atak drzewcem $f i wyprowadzasz kontr� z potworn� si��. Grot z szybko�ci� atakuj�cego w�a w�druje w kierunku przeciwnika b�yszcz�c w�ciekle. Natychmiastowy kontratak jest jednak nieprecyzyjny, ma�a ilo�� czasu, aby dok�adnie wycelowa� powoduje, �e bro� muska zmursza�e, po��k�e rami� i ze�lizguje si� z metalicznym brz�kiem nie czyni�c szkody przeciwnikowi.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak $N z furi� naciera na $c wyprowadzaj�c pot�ne ciosy. Barbarzy�ca zas�ania si� drzewcem $f skutecznie blokuj�c b�yskawiczne ataki i w mgnieniu oka wyprowadza kontr�. Grot $s broni z pr�dko�ci� pioruna szybuje tn�c powietrze w kierunku przeciwnika. Jednak b�yskawiczny atak nie jest zbyt dok�adnie wymierzony i $p muska zmursza�e, po��k�e rami� szkieletu i ze�lizguje si� po nim nie wyrz�dzaj�c �adnej szkody.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Biegniesz rozp�dzon<&y/a/e> w kierunku przeciwnika zataczaj�c szerokie kr�gi $j nad g�ow�. W kilku susach dopadasz go i wykonuj�c szeroki �uk podcinasz drzewcem swej broni zmursza�e dolne ko�czyny wroga. $N chwieje si� przez chwil� pr�buj�c odzyska� r�wnowag�, po czym wali si� z �oskotem na pod�o�e. Skaczesz mu na klatk� piersiow� z impetem, mia�d��c przy tym z paskudnym trzaskiem wszystkie �ebra przeciwnika. Unosisz wysoko $h po czym opuszczasz z potworna si��. Szeroki grot opada ze �wistem prosto na czaszk� przebijaj�c j� na wylot. $N drga jeszcze przez chwil�, po czym nieruchomieje.{x", ch, wield, victim, TO_CHAR );
						act( "{5Podziwiasz jak $n z rozp�dem dopada przeciwnika zataczaj�c szerokie kr�gi nad g�ow� $j. B�yskawicznym manewrem $n podcina d�ugim drzewcem broni zmursza�e ko�czyny $Z, a ten chwieje si� przez chwil� pr�buj�c odzyska� r�wnowag�, po czym wali si� z �oskotem na pod�o�e. $n nie czeka ani sekundy. B�yskawicznie skacze na klatk� piersiow� przeciwnika mia�d��c mu przy tym wszystkie �ebra z paskudnym trzaskiem. Unosi wysoko $h i opuszcza j� ze �wistem na po��k�� czaszk�. Szeroki grot z potworn� si�� przebija j� na wylot. $N w ostatnim podrygu pr�buje si� uwolni�, po czym nieruchomieje.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Biegniesz szale�czo �ciskaj�c mocno drzewce swej broni z b�yszcz�cym grotem skierowanym w przeciwnika. $N z nienawi�ci� wpatruje si� w ciebie pr�buj�c zej�� z linii pot�nej szar�y, jednak bez wi�kszego szcz�cia. Sprytnie manewrujesz $f, skutecznie udaremniaj�c przeciwnikowi jak�kolwiek szans� obrony. Szeroki grot z suchym trzaskiem zag��bia si� w �rodek czaszki wroga, tu� nad lini� oczodo��w z potworn� si��. Prawie oderwana od kr�gos�upa, pop�kana ko�ciana g�owa odchyla si� w ty� k�api�c szcz�k� z b�lu. Pot�nym szarpni�ciem, z cichym zgrzytem uwalniasz grot.{x", ch, wield, victim, TO_CHAR );
						act( "{5Spogl�dasz jak $n w szale�czym biegu kieruje $h w stron� przeciwnika. $N pr�buj�c zej�� z toru ataku przemieszcza si� niezdarnie to w lewo, to w prawo. Szar�uj�cy przemy�la� dobrze sw�j atak, tak manewruje broni� oraz cia�em, �e skutecznie udaremnia wrogowi jak�kolwiek szans� obrony. Szeroki, w�ciekle b�yszcz�cy grot z suchym trzaskiem zag��bia si� w samym �rodku ko�ci czo�owej przeciwnika, tu� nad lini� oczodo��w. Prawie oderwana od kr�gos�upa, ko�ciana g�owa k�apie szcz�k� w niemym cierpieniu.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Wymierzasz precyzyjne pchni�cie w kr�gos�up nacieraj�cego na ciebie przeciwnika. Oceniasz odleg�o��, wykonujesz lekki wykrok i bierzesz pot�ny zamach $j. Ju� jeste� gotow<&y/a/e> do zadania �miertelnego ciosu, gdy $n nieoczekiwanie doskakuje do ciebie zbli�aj�c si� na niebezpiecznie ma�� odleg�o��. Wypuszczaj�c z sykiem powietrze, nie maj�c czasu na nic wi�cej, wyprowadzasz pchni�cie na o�lep w kierunku wroga. Szeroki grot z g�uchym trzaskiem zag��bia si� w miednic� przechodz�c na wylot. Odskakuj�c od $Z wyszarpujesz bro� i obserwujesz wybity, szeroki otw�r w kostnym ciele.{x", ch, wield, victim, TO_CHAR );
						act( "{5Obserwujesz jak $n mierzy odleg�o�� mi�dzy sob� a przeciwnikiem, nast�pnie wykonuje lekki wykrok i bierze szeroki zamach zamierzaj�c zada� �miertelne pchni�cie. Jednak $s plany spe�zaj� na niczym, gdy� $N w nag�ym zrywie doskakuje swego wroga. $n na o�lep wyrzuca przed siebie $h, grot z g�uchym trzaskiem zag��bia si� w ods�oni�tej miednicy przeciwnika przechodz�c na wylot.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (IS_SET(victim->form, FORM_CONSTRUCT))
				{
					if ( dam <= 0 )
					{
						act( "{5Wyprowadzasz pot�ny cios kieruj�c sw�j or� z do�u ku g�rze szerokim �ukiem. Grot zataczaj�c �wietlisty p�okr�g szybuje nad ziemi� i wznosi si� z zastraszaj�c� si�� i pr�dko�ci�. Sprawnym szarpni�ciem kierujesz $h w tors przeciwnika. Zaskoczony $N stoi nieruchomo niezdolny do wykonania jakiegokolwiek ruchu. Bro� b�yskawicznie dociera do celu i trze z dono�nym zgrzytem o twardy korpus, jednak grot wzbija tylko kaskady rozgrzanych od�amk�w i ze�lizguje si� z tu�owia wroga, po czym szybuje w g�r� nie zostawiaj�c najmniejszej nawet rysy na twardym ciele.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n kieruj�c sw�j or� z do�u ku g�rze wyprowadza mordercze pchni�cie w tors $Z. �wietlisty grot szybuj�c nad ziemi� zatacza szeroki p�okr�g i wznosi si� w powietrze z zastraszaj�c� pr�dko�ci�. $n sprawnie manewruj�c or�em prowadzi go b�yskawicznie ku przeznaczeniu, $p dociera do celu i zgrzyta o twardy korpus znieruchomia�ego przeciwnika wywo�uj�c kaskady iskier i od�amk�w sztucznego cia�a, jednak grot ze�lizguje si� z klatki piersiowej i w�druje w powietrze nie pozostawiaj�c najmniejszej rysy na ciele $Z.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Z okrzykiem bojowym na ustach wyprowadzasz mordercze pchni�cie z g�ry do do�u. Rozp�dzony grot trafia $Z w brzuch tu� nad kroczem z dono�nym zgrzytem. Fontanny iskier i od�amk�w sztucznego cia�a strzelaj� z twardego korpusu i szybuj� par� metr�w w powietrze gdy or� wbija si� g��boko w twarde cia�o przechodz�c na wylot. Wok� wyrwy pojawiaj� si� liczne rysy i sp�kania. Widocznie trafi�<&e�/a�/o�> w najs�absz� cz�� konstrukcji, bo p�kni�cia z ka�d� sekund� rozszerzaj� si� i rozprzestrzeniaj� praktycznie na ca�e nienaturalne cia�o. W kilka sekund twardy korpus rozpada si� na kilkadziesi�t kawa�k�w. Wszystkie jego cz�ci w jednej sekundzie wal� si� z dono�nym �oskotem na pod�o�e. Ko�ysz�cy si� na ziemi u�amek twarzy $Z patrzy na ciebie lewym okiem, jakby z wyrzutem.{x", ch, wield, victim, TO_CHAR );
						act( "{5Przera�aj�cy okrzyk bojowy wzbija si� dono�nym g�osem w powietrze i odbija echem. $n wyprowadza mordercze pchni�cie $j kieruj�c sw�j or� z nad g�owy w d�. �wiszcz�cy grot wbija si� g��boko tu� nad kroczem w brzuch $Z, z dono�nym zgrzytem wywo�uj�c kaskady iskier i od�amk�w sztucznego materia�u, kt�re wzbijaj� si� wysoko i szybuj� par� metr�w. Wok� powsta�ej wyrwy w twardym korpusie tworz� si� minimalne p�kni�cia i rysy. Prawdopodobnie to najbardziej czu�e miejsce konstrukcji gdy� sp�kania rozszerzaj� si� z ka�d� sekund� i rozprzestrzeniaj� na ca�e nienaturalne cia�o z cichym odg�osem. W kilka chwil sylwetka $Z rozpada si�, a jej cz�ci w jednej chwili wal� si� z �oskotem na pod�o�e. Od�amana lewa strona twarzy $Z patrzy na $z, jakby z wyrzutem, ko�ysz�c si� miarowo.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Z wyci�gni�t� przed siebie broni� nacierasz z ogromnym impetem na przeciwnika. Ko�ysz�cy si� w rytm twego biegu grot $f tnie powietrze z nieprawdopodobn� si��. $N beznami�tnie wpatruj�c si� w ciebie wyci�ga ramiona w obronnym ge�cie, znajdujesz jednak luk� w zas�onie przeciwnika i or� ze �wistem dociera do celu. Szeroki grot z g�uchym odg�osem zag��bia si� w sztucznym korpusie wywo�uj�c kaskady  iskier i od�amk�w.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n naciera z potwornym impetem na $C, $s wyci�gni�ty or� tnie powietrze z przera�liwym �wistem ko�ysz�c si� w rytm krok�w. $N wyci�gaj�c ramiona w obronnym ge�cie beznami�tnie wpatruje si� w szar�uj�c� sylwetk�, kt�ra jednak sprytnie manewruje sw� broni� i szeroki grot z paskudnym, g�uchym j�kiem wbija si� w korpus przeciwnika wywo�uj�c kaskady iskier i od�amk�w.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Zatrzymujesz si� nagle w miejscu i wyprowadzasz b�yskawiczne pchni�cie w brzuch przeciwnika. Wyrzucony do przodu or� mknie w pr�dko�ci� wichru w kierunku sztucznego korpusu, jednak $N wykonuj�c nag�y zryw cia�a schodzi cz�ciowo z linii ciosu. Szeroki grot zachacza o bok celu i przebija si� przez niego. Odzyskuj�c r�wnowag� stwierdzasz z satysfakcj�, �e tw�j atak nie poszed� na marne. W boku przeciwnika widnieje g��boka wyrwa.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n zatrzymuje si� w miejscu i b�yskawicznym ruchem wyrzuca sw�j or� przed siebie wyprowadzaj�c pot�ne pchni�cie w brzuch przeciwnika. Bro� �wiszczy w powietrzu z nieprawdopodobn� intensywno�ci� zbli�aj�c si� do sylwetki $Z. Z niespotykanym u takich istot refleksem, wykonuje $E nag�y zryw cia�a schodz�c cz�ciowo z linii morderczego ciosu. Szeroki grot zgrzyta o bok przeciwnika wywo�uj�c kaskady iskier i od�amk�w po czym ze�lizguje si� i szybuje w powietrze. Odzyskuj�c r�wnowag� $n odskakuje na bezpieczn� odleg�o�� z satysfakcj� obserwuj�c wyrw� w boku $Z.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				if ( dam <= 0 )
				{
					act( "{5Rozp�dzasz si� w kierunku $Z, jednak przez z�e oszacowanie odleg�o�ci wpadasz na $A z trudem wyprowadzaj�c cios.{x", ch, wield, victim, TO_CHAR );
					act( "{5$n odchyla si� do ty�u po czym rusza do przodu z pot�n� szar��, jednak przez z�e oszacowanie odleg�o�ci wpada na ciebie z trudem wyprowadzaj�c cios.{x", ch, wield, victim, TO_VICT );
					act( "{5$n odchyla si� do ty�u po czym rusza w kierunku $Z z pot�n� szar��, jednak przez z�e oszacowanie odleg�o�ci wpada na $A z trudem wyprowadzaj�c cios.{x", ch, wield, victim, TO_NOTVICT );
					return TRUE;
				}
				else if ( victim->hit - dam < -11 )
				{
					int types = 1;

					if ( ch->class == CLASS_BARBARIAN && is_affected(ch, gsn_berserk ) )
						types = 2;

					switch ( number_range( 0, types ) )
					{
						case 0:
							if ( IS_SET( victim->form, FORM_WARM ) || IS_SET( victim->form, FORM_COLD_BLOOD ) )
							{
								act( "{5Twoje pchni�cie by�o wyj�tkowo celne, trafiasz w samo serce $Z! $N spogl�da ostatni raz na ciebie martwym wzrokiem po czym upada na ziemi�. Buchaj�ca z cia�a fontanna {Rkrwi{5 barwi wszystko dooko�a jasn� czerwieni�.{x", ch, wield, victim, TO_CHAR );
								act( "{5$p $z trafia ci� w samo serce! Czujesz przera�liwy ch��d, po czym co� uderza ci� z potworn� si�� w plecy - w ostatnim przeb�ysku my�li dochodzisz do wniosku, �e to ziemia.{x", ch, wield, victim, TO_VICT );
								act( "{5$p $z trafia $C w samo serce! $N chwieje si� przez chwil� po czym upada na ziemi�. Buchaj�ca z cia�a fontanna {Rkrwi{5 barwi wszystko dooko�a jasn� czerwieni�.{x", ch, wield, victim, TO_NOTVICT );
								make_blood( victim );
								break;
							}

						case 1:
							act( "{5To by�o mistrzowskie pchni�cie! $p trafia $C w g�ow� i przechodzi przez oko rozrywaj�c $S czaszk� na dwie cz�ci!{x", ch, wield, victim, TO_CHAR );
							act( "{5Dostrzegasz bardzo szybko zbli�aj�cy si� przedmiot. To $p $z trafia ci� w oko! Czujesz rozrywaj�cy b�l! Jednak chwil� p�niej jest ci ju� niesamowicie lekko i ca�e cierpienie ust�puje.{x", ch, wield, victim, TO_VICT );
							act( "{5$p $z trafia $C w oko! S�yszysz zgrzyt roztrzaskiwanej czaszki, po czym twoim oczom ukazuje si� rozdarta na p� g�owa $Z. Co za ohydny widok!{x", ch, wield, victim, TO_NOTVICT );
							make_brains(victim);
							break;
						case 2:
							act( "{5Nabijasz $C na w��czni�! Gdy grot przebija $S cia�o, $N krzyczy z b�lu! Podnosisz $M wysoko w g�r�, przytrzymujesz przez chwil� ponad swoj� g�ow�, a nast�pnie przerzucasz w ty�. $N umiera tocz�c krwist� pian� z ust.{x", ch, wield, victim, TO_CHAR );
							act( "{5$n nabija ci� na $h! Gdy grot przebija twoje cia�o, czujesz straszliwy b�l! $n podnosi ci� wysoko w g�r�, przytrzymuje przez chwil� ponad swoj� g�ow�, a nast�pnie przerzuca w ty�. Ostatnie co pami�tasz to uderzenie o ziemi�.{x", ch, wield, victim, TO_VICT );
							act( "{5$n nabija $C na $h! $N krzyczy z b�lu gdy grot przebija $S cia�o! $n podnosi $M wysoko w g�r�, przytrzymuje przez chwil� ponad swoj� g�ow�, a nast�pnie przerzuca w ty�. $N umiera tocz�c krew z ust.{x", ch, wield, victim, TO_NOTVICT );
							break;
					}
					return TRUE;
				}
				else
				{
					int types = 1;

					if ( ch->class == CLASS_BARBARIAN && is_affected(ch, gsn_berserk ) )
						types = 2;

					switch ( number_range( 0, types ) )
					{
						case 0:
							act( "{5Wykonujesz precyzyjne pchni�cie, $p g��boko wbija si� w cia�o $Z rozrywaj�c $S tkanki.{x", ch, wield, victim, TO_CHAR );
							act( "{5$n wykonuje precyzyjne pchni�cie, $s $p g��boko wbija si� w twoje cia�o rozrywaj�c tkanki.{x", ch, wield, victim, TO_VICT );
							act( "{5$n wykonuje precyzyjne d�gni�cie, $s $p g��boko wbija si� w cia�o $Z rozrywaj�c tkanki.{x", ch, wield, victim, TO_NOTVICT );
							break;

						case 1:
							act( "{5Wyprowadzasz pot�ne pchni�cie $j w $C, twoja bro� bez trudu wynajduje luk� w obronie $Z i dewastuje $M bezlito�nie!{x", ch, wield, victim, TO_CHAR );
							act( "{5$n wyprowadza w ciebie pot�ne pchni�cie $j. Impet by� tak pot�ny, �e twoje pr�by os�oni�cia si� spe�z�y na niczym. Potworny b�l rozdziera twoje cia�o, gdy $p przebija sk�r�.{x", ch, wield, victim, TO_VICT );
							act( "{5$n wyprowadza pot�ne pchni�cie $j w $C, $s bro� bez trudu wynajduje luk� w obronie $Z i dewastuje $M bezlito�nie!{x", ch, wield, victim, TO_NOTVICT );
							break;

						case 2:
							act( "{5W szalonym ta�cu wykonujesz pchni�cie z p�obrotu w bok $Z. $p bez trudu wbija si� g��boko w $S cia�o szerz�c powa�ne zniszczenia.{x", ch, wield, victim, TO_CHAR );
							act( "{5$n w szalonym ta�cu wykonuje pchni�cie z p�obrotu w tw�j bok. $S $p bez trudu wbija si� w twoje cia�o szerz�c powa�ne zniszczenia.{x", ch, wield, victim, TO_VICT );
							act( "{5$n w szalonym ta�cu wykonuje pchni�cie z p�obrotu w bok $Z. $p $z bez trudu wbija si� g��boko w cia�o $Z szerz�c powa�ne zniszczenia.{x", ch, wield, victim, TO_NOTVICT );
							break;
					}
					return TRUE;
				}
				break;

			case WEAPON_MACE:
				if (!str_cmp( race_table[ GET_RACE(victim) ].name, "zombi" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "kryptowiec" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "upiorny rycerz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "ghul" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "ghast" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "mumia" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "mumia wi�ksza" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "wampir" ))
				{
					if ( dam <= 0 )
					{
						act( "{5S�yszysz mro��cy krew w �y�ach �wist gdy $p opada z zastraszaj�c� szybko�ci� na �eb $Z. W wyobra�ni ju� widzisz rozchlapuj�c� si� wsz�dzie cuchn�c�, trupi� ciecz oraz rozpa�kan� galaretowat� mas� na obuchu $f. Twa bro� opada, jednak czujesz, �e co� posz�o nie tak. T�uczesz w ziemi� mijaj�c szkaradny �eb i nie czyni�c wrogowi najmniejszej krzywdy, musia�<&e�/a�/o�> �le oceni� odleg�o��. Od uderzenia dr�twiej� ci r�ce, lecz nie masz czasu, aby si� nad tym, zastanawia�. Ca�� uwag� po�wi�casz teraz na unikaniu zajad�ych cios�w przeciwnika.{x", ch, wield, victim, TO_CHAR );
						act( "{5Patrzysz na spadaj�cy ze �wistem obuch. Widzisz jak $n zatrzymuje si� w miejscu i wyprowadza pot�ny cios na czaszk� $Z. Jednak, ku twemu zaskoczeniu, $p z g�uchym odg�osem t�ucze w ziemi�. $n prawie pada na kolana przed rozw�cieczonym nieumar�ym, jednak udaje $m si� utrzyma� na nogach.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Z twej piersi wydobywa si� ryk w�ciek�o�ci. Napinasz mi�nie ramion i bierzesz silny zamach. Przez gnij�c� twarz $Z przebieg� jakby cie� zrozumienia, jakby przeczu� zbli�aj�c� si� z ka�d� sekund� zgub�. Obuch twojego $f z zastraszaj�c� si�� opada na czaszk� przeciwnika, jego g�owa z cichym chrupotem rozpada si� na kawa�ki. Z przegni�ych oczodo��w wylewaj� si� ga�ki oczne, odpadaj� p�aty mi�sa oraz pozosta�o�ci rys�w twarzy � policzki, nos, uszy. Z twarzy nieumar�ego zosta�a tylko krwawa papka. Nast�pnie $p zag��bia si� g��boko w korpus przeciwnika dos�ownie wbijaj�c go w pod�o�e. Z cichym mlaskiem bro� ko�czy sw� mordercz� w�dr�wk�.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n szale�czo rycz�c wyprowadza �miertelne uderzenie celuj�c w g�ow� $Z. Cios jest tak pot�ny, �e w chwili uderzenia $f �eb nieumar�ego p�ka jak skorupka jajka. Obuch nie ko�czy na tym swej krwawej w�dr�wki, zag��bia si� od g�ry w szyj�, klatk� piersiow� oraz brzuch przeciwnika. Impet uderzenia mia�d�y wszystko co napotka na swej drodze, nawet przegni�e ko�czyny �ami� si� jak suche patyki po sam� si�� uderzenia. Kiedy pozosta�e narz�dy hamuj� impet uderzenia po przeciwniku w zasadzie nie ma ju� �ladu. Na ziemi zostaje jedynie ka�u�� cuchn�cej, b�belkuj�cej cieczy, pl�tanina flak�w i mi�sa oraz po�amanych ko�ci.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5P�dzisz na z�amanie karku wymachuj�c szale�czo $j. Z szybko�ci� b�yskawicy, w paru skokach dobiegasz do $Z i z pot�nym wymachem opuszczasz obuch swej broni prosto w plecy przeciwnika. Do twych uszu dociera niemi�y d�wi�k strzaskanych ko�ci oraz p�kaj�cych, wydobywaj�cych si� wn�trzno�ci. Z ziej�cej dziury w torsie nieumar�ego wydobywa si� chmura cuchn�cego gazu. Si�a ciosu odrzuca go na par� metr�w, jednak $N szybko otrz�sa si� z otrzymanego uderzenia i szykuje si� do ataku.{x", ch, wield, victim, TO_CHAR );
						act( "{5Spogl�dasz na rozgrywaj�c� si� przed tob� scen�. Widzisz jak $n w paru szybkich susach podbiega do $Z, wymija sw�j cel i skr�tem cia�a wyprowadza pot�ny cios. Z klatki piersiowej nieumar�ego strzela fontanna przegni�ych flak�w oraz wydobywaj� si� cuchn�ce opary zgnilizny. Nieumar�y wydobywa z siebie przera�aj�cy, dono�ny skrzek i sunie po ziemi par� metr�w. Nie przewraca si� jednak, a potrz�saj�c �bem, gubi�c kawa�ki cia�a szykuje si� do ataku.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5�ciskaj�c mocno trzon $f bierzesz pot�ny zamach. Obuch w�druje w powietrzu i z potworn� si�� uderza w rami� $Z. Cios jest tak silny, �e prawie wyrywa r�k� nieumar�emu. Odg�os darcia si� �ci�gien i w��kien mi�ni rozlega si� g�o�no i rani twe uszy, jednak $N chwiej�c si� na nogach rusza do ataku.{x", ch, wield, victim, TO_CHAR );
						act( "{5Patrzysz jak potworny cios $z w�druje w kierunku ramienia $Z. Obuch $f ze �wistem opada na r�k� przeciwnika i prawie mu j� wyrywa. Koszmarny d�wi�k dartej sk�ry, mi�ni oraz �ci�gien rozlega si� po okolicy. $N chwiejnym krokiem rusza jednak do ataku kontynuuj�c walk�.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (!str_cmp( race_table[ GET_RACE(victim) ].name, "drakolicz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "licz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "szkielet wojownik" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "szkielet"))
				{
					if ( dam <= 0 )
					{
						act( "{5Nacierasz b�yskawicznie na przeciwnika. Kierujesz $h z zastraszaj�c� si�� na korpus o�ywie�ca. Bro� ze �wistem tnie powietrze pod��aj�c w nadanym przez ciebie kierunku. $N, patrz�c na ciebie beznami�tnie, stoi w bezruchu jakby nie zdawa� sobie sprawy z gro��cego mu niebezpiecze�stwa. Obuch twej broni o centymetry mija klatk� piersiow� nieruchomego przeciwnika. �le wymierzona odleg�o�� powoduje, �e chybiasz haniebnie, a wr�g wykorzystuj�c sytuacj�, klekocz�c ko��mi przy ka�dym b�yskawicznym ruchu, wyprowadza pot�ne ataki.{x", ch, wield, victim, TO_CHAR );
						act( "{5Spogl�dasz na wpatruj�cych si� w siebie wyzywaj�co przeciwnik�w. $n wyprowadza pot�ny cios kieruj�c $h na klatk� piersiow� wroga. O dziwo $N stoi nieruchomo jak szkaradny, kamienny pos�g szyderczo �widruj�c pustymi oczodo�ami sylwetk� nacieraj�cego $z. Czy�by zmursza�a, wyschni�ta, pozbawiona sk�ry i mi�ni twarz przybra�a kpi�cy wyraz? Wyrzucony w powietrze obuch zatacza szeroki �uk i mija haniebnie cel, w kt�rym pod��a�, poci�gaj�c za sob� dier��cego. Klekocz�c dono�nie ko��mi, przy ka�dym z b�yskawicznych ruch�w szkielet wyprowadza pot�n� kontr�.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Ze w�ciek�ym rykiem wyprowadzasz morderczy cios skierowany w po�yskuj�c� czaszk� przeciwnika. Ten jednak nag�ym zrywem odchyla si� lekko do ty�u. Rozp�dzony obuch $f ze �wistem opada na miednic�. Si�a ciosu jest tak wielka, �e rozpada si� ona w mgnieniu oka z g�uchym odg�osem na tysi�ce drobnych kawa�k�w oddzielaj�c zmursza�e nogi szkieletu od korpusu. Chmura wiekowego, st�ch�ego py�y zas�ania ci widoczno��. Jak przez mg�� spogl�dasz na opadaj�cy, wymachuj�cy w powietrzu r�koma korpus przeciwnika. W jednej sekundzie unosisz bro� z morderczym b�yskiem w oku i opuszczasz j� raz za razem wgniataj�c o�ywie�ca w pod�o�e. Ze $Z nie pozostaje nic opr�cz porozrzucanych wsz�dzie kostnych drobin i ta�cz�cej w powietrzu chmury szarawego py�u. Ocieraj�c pot z czo�a warczysz w�ciekle rozgl�daj�c si� po okolicy.{x", ch, wield, victim, TO_CHAR );
						act( "{5Do twych uszu dociera potworny ryk $Z, szar�uje $e w�ciekle na przeciwnika. Pot�ne walni�cie z zastraszaj�c� pr�dko�ci� opada na ods�oni�t�, po�yskuj�c� czaszk� $Z, jednak o�ywieniec jest zbyt czujny i odchyla si� b�yskawicznie do ty�u, lecz rozp�dzony obuch opada na zmursza�� miednic� nieumar�ego, kt�ra z g�uchym, pustym �oskotem rozpada si� na tysi�ce drobin kostnych oddzielaj�c dolne ko�czyny od korpusu szkieletu. Wzbita przy tym chmura wiekowego py�u zas�ania widoczno��. Kiedy opada widzisz sylwetk� $z t�uk�c� bez opami�tania w le��cy korpus przeciwnika. Kiedy milknie odg�os ostatniego uderzenia po $X zostaje jedynie walaj�ca si� wsz�dzie kupka od�amk�w kostnych.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5W kilku skokach dopadasz przeciwnika. Bierzesz pot�ny zamach i t�uczesz $j z ca�ych si� w ods�oni�ty korpus $Z. Walni�cie dosi�ga �eber powoduj�c seri� zgrzytliwych, suchych trzask�w. Wywo�ujesz istn� burz� kostnych, ostrych jak sztylety od�amk�w i wzniecasz chmur� wiekowego, szarawego py�u. W niemej, ukrytej w pustych oczodo�ach nienawi�ci, $N spogl�da na wybit� w swym boku olbrzymi� wyrw�.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz p�dz�cego $z w kierunku $Z. Dopada wroga w u�amku sekundy, bierze pot�ny zamach i wyprowadza pot�ny cios $j w bok szkieletu. Zderzenie $f z suchymi ko��mi wywo�uje burz� kostnych, ostrych odprysk�w oraz wznieca chmur� wiekowego py�u. Do twych uszu dobiega zgrzytliwy d�wi�k mia�d�onych �eber. W boku przeciwnika widnieje olbrzymia wyrwa.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5P�dzisz ca�ym impetem na $C unosz�c w g�r� $h. Przeciwnik w por� zauwa�a gro��ce mu niebezpiecze�stwo i wyprowadza b�yskawiczny i pot�ny kontratak na tw�j korpus. Robisz szybki unik schodz�c z toru ciosu zmursza�ego, wyschni�tego ramienia. Hamuj�c zaciekle suniesz kilka metr�w po pod�o�u ci�gni�ty impetem szale�czego biegu, opuszczaj�c przy tym bro� dla lepszego zachowania r�wnowagi. Mijaj�c $Z spostrzegasz nadarzaj�c� si� okazj� do wyprowadzenia niezdarnego ciosu. Wykonujesz energiczne, rozpaczliwe pchniecie w bok wroga, po czym do twych uszu dociera pusty odg�os p�kaj�cych, kruchych ko�ci.{x", ch, wield, victim, TO_CHAR );
						act( "{5Patrzysz jak $n p�dzi jak rozjuszony tur w kierunku przeciwnika wysoko unosz�c $h. W por� jednak $N zauwa�a gro��ce mu, straszliwe niebezpiecze�stwo. Wyprowadza b�yskawiczny kontratak na korpus szar�uj�cego, ale $n zgrabnie unika zagro�enia wykonuj�c nag�y skr�t cia�a, opuszczaj�c przy tym bro�. Sunie par� metr�w po pod�o�u pchany impetem zapieraj�c si� rozpaczliwie nogami. Mijaj�c $C wyprowadza opuszczon� broni� rozpaczliwe pchni�cie w �ebra przeciwnika wy�amuj�c wiele z nich.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (!str_cmp( race_table[ GET_RACE(victim) ].name, "duch" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "banshee" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "cie�" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "widmo" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "zmora" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "upi�r" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Bierzesz pot�ny zamach i wyprowadzasz szybkie, pot�ne walni�cie w niematerialn� g�ow� przeciwnika. $N przewiduje jednak tw�j zamiar i wykonuje b�yskawiczny unik w bok unosz�c si� lekko w nad ziemi�. Obuch mija p�przezroczyst� posta� o centymetry t�uk�c g�ucho w pod�o�e. Niematerialna sylwetka pod wp�ywem podmuchu powietrza wywo�anego ciosem zamazuje si� lekko i faluje przed twoimi oczami. Pokonuj�c potworny b�l ramion powoli unosisz ponownie $h. Przez tw�j umys� przebiega niezrozumia�y �liski, cichy, drwi�cy szept w jakim� prastarym j�zyku, kt�rego nie rozumiesz.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n bierze pot�ny zamach i wyprowadza morderczy cios na g�ow� przeciwnika, jednak $N wykonuje b�yskawiczny zryw w bok lawiruj�c lekko nad ziemi�. Rozp�dzony obuch t�ucze z pot�nym, g�uchym �oskotem o centymetry od niematerialnego wroga. Sylwetka przeciwnika rozmazuje si� lekko i faluje pod wp�ywem podmuchu powietrza. $n o ma�o nie upuszczaj�c broni unosi j� ponownie dr��cymi, zdr�twia�ymi d�o�mi z grymasem b�lu na twarzy.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5$N naciera b�yskawicznie na ciebie upiornie wyj�c. Spodziewa�<&e�/a�/o�> si� tego i z zimnym, bezlitosnym u�miechem na twarzy wykonujesz b�yskawiczny skr�t cia�a i ustawiasz si� przodem do plec�w przeciwnika. Z szyderczym rykiem unosisz $h i z w�ciek�o�ci� opuszczasz obuch mi�dzy �opatki wroga. Bro� z potworn� si�� i precyzj� si�ga celu tworz�c w niematerialnym ciele rozleg�� wyrw�. $N odchyla g�ow� w ty� w szale�czym cierpieniu. Z oczu, uszu, rozwartych szeroko ust oraz szerokiej rany w korpusie przeciwnika strzelaj� jaskrawe lance purpurowego �wiat�a. Po chwili niematerialna sylwetka rozp�ywa si� w niemej eksplozji czerwonego blasku. Wok� panuje niezm�cona niczym, b�oga cisza.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak $N naciera b�yskawicznie na $C, jednak $e w u�amku sekundy wykonuje skr�t cia�a ustawiaj�c si� przodem do plec�w przeciwnika i unosi wysoko $h, kieruj�c obuch w sam �rodek niematerialnego korpusu przeciwnika. Bro� ze �wistem opada zgodnie ze swym przeznaczeniem tworz�c olbrzymi� wyrw� w po�przezroczym korpusie. $N odchyla g�ow� w ty� w szale�czym cierpieniu, a z $S ust, uszu, szeroko rozwartych oczu i olbrzymiej rany buchaj� szerokie smugi purpurowego �wiat�a. Po chwili niematerialna sylwetka niknie w niemej eksplozji czerwonego �wiat�a. Wok� panuje grobowa cisza.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Nie okazuj�c strachu kroczysz pewnie �ciskaj�c $h w kierunku $Z. W twym umy�le rozchodzi si� nieustaj�co przera�aj�cy, sycz�cy szept przeciwnika. Na zmarszczonym czole pojawiaj� si� strugi zimnego potu. Musisz to zako�czy�. Pozby� si� o�liz�ych g�os�w rozbrzmiewaj�cych w twym udr�czonym umy�le. Natychmiast! Ze zduszonym j�kiem wyprowadzasz pot�ny cios, rozp�dzony obuch opada ze straszliw� si�� na klatk� piersiow� $Z wydobywaj�c z p�materialnego cia�a szerokie smugi jaskrawoczerwonego �wiat�a. Piekielny szept milknie na chwil�, zast�puje go op�ta�czy wrzask.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n kroczy pewnym krokiem w kierunku $Z mocno zaciskaj�c r�ce na trzonie swej opuszczonej broni. Twarz atakuj�cego ma zatrwa�aj�cy wyraz. Mieszaj� si� na niej w�ciek�o�� i przera�enie. Zbli�aj�c si� do $Z wznosi wysoko w g�r� $h i opuszcza z zastraszaj�c� si�� i szybko�ci� trafiaj�c w p�przezroczyst� klatk� piersiow�. W chwili zetkni�cia si� broni z niematerialnym cia�em spod obucha wydobywaj� si� czerwone lance jaskrawego �wiat�a. Do twego umys�u dociera cie� op�ta�czego wrzasku.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5�widruj�cy m�zg, �liski szept rozbrzmiewaj�cy w twym umy�le przyprawia ci� prawie o ob��d. Czerwone, rozbiegane p�aty w�ciek�o�ci przes�aniaj� ci widoczno��. Jak przez mg�� szale�stwa widzisz unosz�c� si� nad ziemi� rozmazan� sylwetk� przeciwnika. Ze w�ciek�ym rykiem kierujesz obuch $f w brzuch $Z, jednak przeciwnik jakim� sposobem odbija, nie wykonuj�c nawet jednego gestu, rozp�dzon� bro� i cios ze �witem opada na ko�czyn� wroga. Z rozdartej, p�materialnej nogi strzela promie� jaskrawoczerwonego �wiat�a ra��c twe oczy.{x", ch, wield, victim, TO_CHAR );
						act( "{5Spogl�dasz na $n. Kieruje si� $e w stron� $Z na dr��cych nogach. Ze w�ciek�ym rykiem unosi $h i z zastraszaj�c� pr�dko�ci� stara si� uderzy� w brzuch przeciwnika, jednak bro� odbija si� jakby od niewidzialnej tarczy mijaj�c cel i ze �wistem opada na doln� ko�czyn� $Z, tworz�c w niej spor� wyrw�. Z p�przezroczystej nogi strzela smuga jasnego czerwonego �wiat�a i blednie tak szybko jak si� pojawi�a.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (IS_SET(victim->form, FORM_CONSTRUCT))
				{
					if ( dam <= 0 )
					{
						act( "{5Starasz si� wykona� efektown� szar�� na $C. Niestety, przeceni�<&e�/a�/o�> swe mo�liwo�ci i niezbyt dobrze si� przygotowa�<&e�/a�/o�> i $p zaledwie muska powierzchni� $Z, a $E pot�nym ruchem r�ki prawie wytr�ca ci or� z d�oni.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n pr�buje pot�ny cios $X. Bierze du�y rozp�d, wznosi nad g�ow� $h i wyprowadza cios. Niestety, $N okazuje si� by� o niebo szybszy, usuwa si� z trajektorii $f i sprawnym ruchem pot�nej �apy prawie wytr�ca $x bro� z r�ki.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Ruszasz naprz�d, �ciskaj�c obur�cz $h i nie kryj�c przy tym swych intencji. Twe cia�o nabiera rozp�du, w twe serce wlewa si� odwaga, na twych wargach rozbrzmiewa pie�� zwyci�stwa! Gdy dobiegasz do $Z nie zastanawiaj�c si� ani troch� co robisz, bierzesz pot�ny zamach i tw�j $p z niesamowit� pr�dko�ci� uderza w sam �rodek pot�nego torsu wroga. Ten z kolei, wydaj�c og�uszaj�cy, b�bni�cy d�wi�k efektownie rozlatuje si� na kawa�ki, kt�re to znowu rozsypuj� si� w drobny mak.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n z piekielnym wrzaskiem na ustach rusza naprz�d, �ciskaj�c obur�cz $h i nie kryj�c przy ty �adnych intencji. Dostrzegasz, jak $s cia�o nabiera rozp�du, a w oczach pojawia si� ��dza niszczenia. Gdy dobiega do $Z nie zastanawiaj�c si� ani troch� co robi, bierze pot�ny zamach a $s $p z niesamowit� pr�dko�ci� uderza w sam �rodek pot�nego torsu $Z, a $E z kolei, wydaj�c og�uszaj�cy, b�bni�cy d�wi�k, efektownie rozlatuje si� na kawa�ki, kt�re to rozsypuj� si� w drobny mak.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Wci�� nie spuszczaj�c z oczu pot�nej sylwetki $Z dok�adnie planujesz swe uderzenie. �ciskaj�c jak tylko mo�esz najmocniej grub� r�koje�� $f i wykonuj�c efektowny obr�t m��cisz przeciwnika prosto w �rodek plec�w, po czym widzisz jak od jego cielska odpada par� p�at�w dziwnego materia�u. Szybko wycofujesz si� na bezpieczn� odleg�o�� i z nowym zapa�em kontynuujesz walk�.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n gwa�townie wci�ga powietrze i rozpoczyna desperacki bieg. Zauwa�asz, jak nie zpuszczaj�c z pola widzenia $Z planuje dok�adnie pot�n� szar��. Wykonuj�c efektowny obr�t m��ci przeciwnika prosto w �rodek plec�w i z wielk� przyjemno�ci� podziwia, jak od jego cielska odpada par� p�at�w dziwnego materia�u.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Rozpoczynasz szale�czy, desperacki bieg w kierunku $C. Z zai�cie wspania�� zwinno�ci� unikasz cios�w $S monstrualnych r�k i wci�gaj�c gwa�townie powietrze unosz�c $h wysoko w g�r�. Bierzesz seri� pot�nych zamach�w i m��cisz kilkukrotnie w stop� wroga najmocniej jak tylko mo�esz. Po chwili dostrzegasz, jak $n kuleje, a od jego stopy stopniowo odpadaj� zmasakrowane palce.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n trzymaj�c $h nisko przy ziemi biegnie wprost na $C. Nabieraj�c pr�dko�ci doskakuje w �wietnym stylu do nogi przeciwnika i bior�c seri� pot�nych zamach�w m��ci w jego stop� jak tylko mo�e, odbijaj�c od reszty cia�a par� poka�nych wielko�ci paluch�w.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (IS_SET(race_table[GET_RACE(victim)].type , ANIMAL))
				{
					if ( dam <= 0 )
					{
						act( "{5Rzucasz si� z krzykiem na $C i rozpoczynasz szale�czy bieg. Bior�c jednak pot�ny zamach w celu wykonania ko�cowego ciosu, stwierdzasz, �e co� jest nie tak. Nawet nie poczu�<&e�/a�/o�>, jak w czasie biegu zgubi�<&e�/a�/o�> $h! Szybko podnosisz sw� bro� i z wstydem kontynuujesz walk�.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n rzuca si� z krzykiem na $C i rozpoczyna szale�czy bieg. Nagle jednak $p wypada z $s z r�ki... ale c� to? Ta �amaga biegnie dalej! W ko�cu dopiero dostrzega sw�j b��d i wraca po porzucony or�, z wymalowanym na twarzy poczuciem wstydu.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Bierzesz rozp�d i z szale�czym rykiem rzucasz si� na $C, szale�czo wymachuj�c $j. Na twarzy czujesz podmuchy powietrza, wzbudzonego przez or�, gdy ten niebezpiecznie wiruje wok� twej g�owy. Bezbronne wobec przejawu twej agresji zwierz� pr�buje si� uchyli�, lecz jest ju� za p�no. Na $C ze �wistem spada $h, wgniataj�c $S cia�o w ziemi� i przy okazji zamieniaj�c wszystkie organy w zbitk� mi�sa. Krwista posoka obryzguje wszystko dooko�a, a ty z zadowoleniem spogl�dasz na swe dzie�o.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n bierze rozp�d i z szale�czym rykiem rzuca si� na $C, szale�czo wymachuj�c $j. Ciarki przechodz� ci� po plecach, gdy podziwiasz niebezpiecznie wiruj�cy or�, a do twych uszu dochodzi �wist wzbudzonego wiatru. Zwierz�, bezbronne wobec przejawu agresji $z, pr�buje si� uchyli�, lecz jest ju� dla niego za p�no. Obuch $f spada na niego ze �wistem, wgniataj�c cia�o w ziemi� i przy okazji zamieniaj�c wszystkie organy zbitk� mi�sa. Krwista posoka obryzguje wszystko dooko��, ochlapuj�c ci� ca�kowicie.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Dzier��c obur�cz $h, wydobywasz z p�uc bitewny okrzyk i rzucasz si� na $C, a $E, ca�kowicie zaskoczon$R, pr�buje si� jeszcze uchyli�. Na $S nieszcz�cie jeste� o wiele szybsz$r. Wiruj�ca dot�d szale�czo bro� spada z impetem na odkryte cia�o $Z i zwala $S z n�g, dotkliwie przy tym rani�c.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n dzier��c obur�cz $h, wydobywa z p�uc bitewny okrzyk i rzuca si� na $C, a $E, ca�kowicie zaskoczon$R, pr�buje si� jeszcze uchyli�, bez efektu. Wiruj�ca szale�czo bro� z pot�n� si�� spada na $C i zniekszta�ca cz�� cia�a, mia�d��c mi�nie i ko�ci.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Wydobywasz z siebie g�o�ny ryk i zmuszasz swe nogi do szale�czego biegu w stron� $Z, wymachuj�c p�ynnie $j. Sylwetka odkrytego zwierz�cia zbli�a si� z ka�d� sekund�, a ty wci�� nabierasz p�du. U celu bierzesz szeroki rozmach i z ca�ej si�y uderzasz od spodu. Wyrzucon$R w powietrze $N spada par� metr�w dalej z g�uchym �upni�ciem, a do twych uszu dociera ni to pisk, ni to st�kni�cie.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n wydobywa z siebie g�o�ny ryk i zmusza swe nogi do szale�czego biegu w stron� $Z, wymachuj�c p�ynnie $j. Sylwetka odkrytego zwierz�cia zbli�a si� z ka�d� sekund�, a $e wci�� nabiera p�du. U celu zatacza sw� broni� �uk i z ca�ej si�y uderza od spodu. Biedne zwierz� ulatuje w powietrze, spada par� metr�w dalej z g�uchym piskiem i z trudem podnosi si� na nogi.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
			case WEAPON_STAFF:
				if (!str_cmp( race_table[ GET_RACE(victim) ].name, "zombi" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "kryptowiec" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "upiorny rycerz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "ghul" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "ghast" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "mumia" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "mumia wi�ksza" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "wampir" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Stoisz naprzeciw $Z gotuj�c si� do wymierzenia ciosu. Przeciwnik naciera na ciebie z impetem, wymachuj�c rozpadaj�cymi si� ramionami. Uchylasz si� i wymierzasz pot�ny cios w bok o�ywie�ca. Niestety, wr�g jest ostro�ny i chwyta obur�cz koniec $f unieruchamiaj�c go. Szarpiesz z ca�ej si�y pr�buj�c wyrwa� bro� z z�artych zgnilizn� d�oni, jednak nie doceni�<&e�/a�/o�> nadnaturalnej krzepy nieumar�ego. Si�ujesz si� d�u�sz� chwil� z przeciwnikiem, nie przynosi to jednak oczekiwanego rezultatu. Nagle wykonujesz nag�y wykrok. Zapierasz si� nog� o korpus wroga i wyrywasz bro� z cuchn�cych r�k trac�c r�wnowag�. Wydobywaj�c z siebie przeci�g�y skrzek $N w�ciekle naciera na ciebie.{x", ch, wield, victim, TO_CHAR );
						act( "{5Obserwujesz jak $n rzuca si� wyj�c na $C, $e przykuca unikaj�c morderczych ramion o�ywie�ca i wymierza niezdarny cios w ods�oni�ty bok wroga. Jednak nieumar�y zadziwiaj�co zr�cznym i szybkim ruchem chwyta obur�cz p�dz�cy w jego kierunku obuch $f unieruchamiaj�c go. $n rzuca si� w ty� pr�buj�c wyszarpa� swoj� bro�, bezskutecznie. Przeciwnicy szarpi� si� chwil� w niemych zmaganiach. Nagle $n wyrzuca nog� w prz�d, zapiera si� o korpus $Z i pot�nym szarpni�ciem odzyskuje $h.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5P�dzisz szale�czo szar�uj�c na $C ze w�ciek�ym rykiem. W biegu podrzucasz $h, �apiesz obur�cz za jeden z ko�c�w, b�yskawicznie oceniasz odleg�o�� i uderzasz z ca�ych si� w g�ow� przeciwnika. Bro� niebezpiecznie zatrzeszcza�a, ale wytrzyma�a impet uderzenia. W przeciwie�stwie do czaszki o�ywie�ca. Jego g�owa prys�a w jednej sekundzie wyrzucaj�c rozpa�kany m�zg i hektolitry cuchn�cej, g�stej, ��toszarej mazi. Ponownie pierzesz szeroki zamach i t�uczesz oszo�omionego nieumar�ego pot�nie w ods�oni�te plecy. Si�a ciosu odrzuca go na par� metr�w. Wykonujesz pot�ny, b�yskawiczny skok podpieraj�c si� na $j i skaczesz na wroga powalaj�c go ostatecznie na ziemi�. Wbijasz z pot�n� si�� koniec swej broni mi�dzy �opatki przeciwnika przyszpilaj�c i unieruchamiaj�c drgaj�ce jeszcze cia�o. Wszystko to trwa�o niepe�na par� sekund. Wyszarpujesz $h z cuchn�cego �cierwa wyj�c przy tym dono�nie.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak $n p�dzi szale�czym tempem w stron� $Z. Nie zwalniaj�c biegu szar�uj�cy podrzuca $h wysoko w g�r� po czym �apie bro� obur�cz za jeden z ko�c�w i t�ucze od g�ry w czaszk� wroga, kt�ra rozpryskuje si� praktycznie w chwili zetkni�cia si� z broni�. B�yskawicznym ruchem $N odskakuje, bierze zamach i wali z ca�ych si�a w ods�oni�te plecy przeciwnika, kt�rego impet ciosu odrzuca na par� metr�w. Podpieraj�c si� niczym na tyczce atakuj�cy wybija si� wysoko w g�r� i opada prosto na $C zwalaj�c wroga z n�g. $n wymierza mi�dzy �opatki pot�ne pchni�cie i unieruchamia drgaj�ce jeszcze cia�o. Wyrywaj�c bro� barbarzy�ca wyje w szale�czym triumfie.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Obracasz leniwym ruchem sw� bro� w d�oniach okr��aj�c przy tym przeciwnika. Ko�ce $f zataczaj� szerokie kr�gi,  $N pr�buje przedrze� si� przez stworzon� tak zas�on�, jednak bezskutecznie. Nagle, niezdarnym pchni�ciem, pozorujesz atak na brzuch o�ywie�ca. Wr�g uchyla si�, zasadzka si� powiod�a. Pot�nymi, szybkimi, precyzyjnymi ruchami wyprowadzasz seri� mocnych cios�w w ods�oni�te cz�ci rozk�adaj�cego si� cia�a. Uderzenia trafiaj� po kolei w g�ow�, �ebra i dolne ko�czyny przeciwnika. G�sta, cuchn�ca, zielono��ta ma� bryzga przy ka�dym z pot�nych uderze�. S�ycha� trzask �amanych ko�ci. Nieumar�y chwieje si� na nogach pod ciosami wyj�c przera�liwie. Spogl�dasz na skutki swoich zabieg�w i u�miechasz si� z zadowoleniem. Z na wp� zmia�d�onej g�owy wycieka m�zg, z boku wyzieraj� po�amane �ebra, a z kolana sterczy ostra, pozbawiona szpiku ko��. $N utykaj�c rusza w twoj� stron� skrzecz�c w�ciekle.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak $n okr��a powoli przeciwnika zataczaj�c przy tym kr�gi $j tworz�c zas�on� nie do przebycia przed ciosami $Z. Nagle zatrzymuj�c si� w miejscu kieruje jeden z ko�c�w swej broni powolnym, niezdarnym ruchem w kierunku nieumar�ego. $N uchyla si� przez co s�abnie $S obrona. $n ze �wistem wypuszczaj�c powietrze wyprowadza seri� mia�d��cych cios�w. Trafia w g�ow�, kt�ra z paskudnym chrz�stem ods�ania przegni�y m�zg, �ebra oraz nog�. Ze zmia�d�onego boku przez sk�r� przebijaj� si� ostre kawa�ki �eber. Z nogi wystaje strzaskana ko�� piszczelowa. Chrz�stowi �amanych ko�ci towarzyszy upiorne wycie. Przy ka�dym z cios�w z cia�a o�ywie�ca tryskaj� strugi zielono��tej, cuchn�cej ropy. Chwiej�c si� na nogach $N ponownie naciera na $c.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Trzymaj�c obur�cz $h wymierzasz precyzyjny cios w praw� stron� twarzy $Z. Przeciwnik unosi b�yskawicznym ruchem rozpadaj�c� si� r�k� i uderzenie trafia w przedrami� o�ywie�ca. Z cichym zgrzytem ze�lizguje si� po wilgotnej od trupiego jadu ko�ci i trafia w szcz�k� wroga wywo�uj�c pe�en b�lu skrzek oraz mokre odg�osy odrywanego cia�a i wyp�ywaj�cych ga�ek ocznych. Do twych uszu dociera trzask wybijanych z�b�w i �amanej �uchwy. Nieumar�y obraca zmasakrowan� twarz w twoim kierunku. Dolna szcz�ka wisi bezw�adnie na kilku w��knach przegni�ych mi�ni, a oczodo�y �wiec� czarn� pustk�. Z jego gard�a wydobywa si� cichy, w�ciek�y gulgot. Strumie� cuchn�cej posoki bije teraz z jego gard�a bez opami�tania. Nie zastanawiaj�c si� d�u�ej �ciskasz mocniej sw� bro� i czekasz na kontratak.{x", ch, wield, victim, TO_CHAR );
						act( "{5Patrzysz jak $n wyprowadza precyzyjny cios w g�ow� przeciwnika. $N wyrzuca jednak w g�r� przegni�e przedrami� i blokuje uderzenie, $p ze�lizguje si� po wilgotnej od trupiego jadu i ropy ko�ci trafiaj�c w twarz o�ywie�ca. Wywo�uje to fontann� cuchn�cej posoki, trzask �amanych ko�ci oraz mokre mla�ni�cia odrywanego cia�a i wyp�ywaj�cych ga�ek ocznych szybuj�cych w powietrze. Dolna szcz�ka $Z ko�ysze si� bezw�adnie przy ka�dym ruchu rozk�adaj�cej si� g�owy. Puste oczodo�y ra�� bij�c� z nich pustk�. Z gard�a nieumar�ego wyciekaj� galony cuchn�cej ropy. Z cichym, pe�nym w�ciek�o�ci rz�eniem $N rzuca si� na $c.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (!str_cmp( race_table[ GET_RACE(victim) ].name, "duch" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "banshee" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "cie�" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "widmo" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "zmora" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "upi�r" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Chwytaj�c obur�cz za jeden z ko�c�w swego $f bierzesz pot�ny zamach i wyprowadzasz pot�ne walni�cie. Niestety $N b�yskawicznie robi unik i schodzi z toru ciosu, a or� tnie powietrze ze �wistem i uderza g�ucho w pod�o�e tu� obok przeciwnika. Przez twe napi�te ramiona przebiega dreszcz ostrego b�lu i odr�twienia. Na domiar z�ego potykasz si� i walisz na kolana w niezdarnej pr�bie odzyskania r�wnowagi. Turlaj�c si� po ziemi i zrywasz si� na r�wne nogi.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak $n chwyta obur�cz $h, unosi wysoko do g�ry i wyprowadza pot�ne walni�cie na korpus $Z. Niematerialny przeciwnik robi w u�amku sekundy b�yskawiczny unik i pot�ny cios nie trafia. Or� z g�uchym odg�osem t�ucze o pod�o�e. Z wyrazem b�lu na twarzy $n traci r�wnowag� i pada na kolana, po czym turlaj�c si� po ziemi oddala si� od przeciwnika i zrywa na r�wne nogi.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Biegniesz szale�czo w kierunku przeciwnika �ciskaj�c obur�cz $h. Zbli�asz si� do lawiruj�cej w powietrzu postaci $Z z zastraszaj�c� szybko�ci�. Tw�j or� bezg�o�nie zatapia si� w samym �rodku niematerialnej klatce piersiowej przeciwnika przebijaj�c j� na wylot. Wok� zatopionego g��boko w p�przezroczystym ciele or�a tryskaj� gejzery purpurowego �wiat�a. Na pocz�tku w�ska wyrwa w astralnym korpusie rozszerza si� teraz przybieraj�c coraz wi�ksze rozmiary. Jaskrawy blask poch�ania teraz ju� prawie ca�� sylwetk� przeciwnika. Nag�a, bezg�o�na, o�lepiaj�ca �wietlna eksplozja o�lepia ci�. Po chwili po $B nie pozostaje �aden �lad. Unosz�cy si� w powietrzu, piekielny wrzask b�lu r�wnie� si� rozp�ywa.{x", ch, wield, victim, TO_CHAR );
						act( "{5Spogl�dasz na zbli�aj�cych si� do siebie z zastraszaj�c� pr�dko�ci� przeciwnik�w. $n biegnie szale�czo wyci�gaj�c przed siebie sw�j or�, a $N z wyci�gni�tymi ramionami sunie b�yskawicznie nad ziemi� wyj�c szale�czo. Przy zetkni�ciu wrog�w $p zag��bia si� bezszelestnie w niematerialnej klatce piersiowej przebijaj�c j� na wylot. Chwil� p�niej obie sylwetki poch�ania eksplozja purpurowego, o�lepiaj�cego �wiat�a. Gdy odzyskujesz wzrok po $B nie ma ju� najmniejszego �ladu. Potworny ryk b�lu r�wnie� niknie w oddali zostawiaj�c po sobie dreszcz przerazenia.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Z bojowym okrzykiem na ustach rzucasz si� na $C, $E lawiruj�c niepewnie w powietrzu pr�buje uj�� twojej szar�y, jednak nadaremnie. �ciskaj�c obur�cz $h wyprowadzasz obiema ko�cami seri� b�yskawicznych cios�w w g�ow� i korpus przeciwnika. Ka�demu zetkni�ciu or�a z niematerialnym cia�em towarzyszy urywany wrzask rozlegaj�cy si� zgrzytliwie w twej g�owie. Odskakuj�c od $Z pozostawiasz kilkana�cie wyrw w p�przezroczystej pow�oce z kt�rych bij� w�skie smugi czerwonego �wiat�a.{x", ch, wield, victim, TO_CHAR );
						act( "{5Przera�liwy okrzyk wojenny $z rozlega si� dono�nie gdy rzuca si� $e w stron� $Z. P�materialna sylwetka lawiruje niepewnie w powietrzu pr�buj�c zej�� z linii ataku, na pr�no. Zataczaj�c szerokie kr�gi $j $n wyprowadza seri� celnych, mocnych cios�w na g�ow� i korpus przeciwnika. Przy ka�dym bezg�o�nym uderzeniu bardziej czu� ni� s�ycha� urywane wrzaski b�lu. Na p�przezroczystym ciele zauwa�asz kilkana�cie wyrw, z kt�rych bij� w�skie smugi o�lepiaj�cego, czerwonego �wiat�a.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Atakuj�c $C przykucasz na moment i wybijasz si� wysoko w powietrze. �ciskasz obur�cz $h i wyprowadzasz cios z g�ry w g�ow� przeciwnika. Opadasz z ogromn� pr�dko�ci� na p�przezroczyst� sylwetk� z wyci�gni�tym przed siebie or�em. Zbli�aj�c si� do wroga, s�ysz�c jedynie �wist powietrza w uszach wykonujesz kr�tki zamach sw� broni�, lecz $p mija haniebnie wyznaczony cel zahaczaj�c jednak o korpus $Z. Przeciwnik sunie w powietrzu par� metr�w pchany impetem ciosu, a z jego p�materialnego cia�a strzela jaskrawoczerwony strumie� �wiat�a.{x", ch, wield, victim, TO_CHAR );
						act( "{5Obserwujesz jak $n przykuca na chwil�, odbija si� mocno od pod�o�a i wykonuje wysoki, d�ugi skok w kierunku przeciwnika zamierzaj�c zaatakowa� od g�ry. Z zastraszaj�c� pr�dko�ci� zbli�a si� $e do $Z, wyci�ga przed siebie $h i zatacza kr�tki �uk zamierzaj�c uderzy� w g�ow�. Jak na z�o�� or� haniebnie mija wyznaczony cel i zahacza o niematerialny korpus. $N lawiruje w powietrzu par� metr�w przez impet ciosu. Z wyrwy $S w klatce piersiowej strzela snop czerwonego �wiat�a.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (!str_cmp( race_table[ GET_RACE(victim) ].name, "drakolicz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "licz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "szkielet wojownik" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "szkielet" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Wydajesz z siebie og�uszaj�cy krzyk bitewny i nacierasz na $C najagresywniej, jak tylko mo�esz. Ju� nawet planujesz kolejne kroki twego ataku, niestety ko�ciotrup delikatnie schodzi ci z drogi i $p przecina tylko powietrze.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n wydaje z siebie og�uszaj�cy, bitewny krzyk po czym zaciekle naciera na $C, ale $E widz�c to, schodzi z linii szar�y, i $p przecina tylko powietrze.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Bierzesz g��boki oddech i rozpoczynasz szar�� p�dz�c na $C z niesamowit� pr�dko�ci� i wymachuj�c w powietrzu $j. B�d�c u celu wydajesz z siebie og�uszaj�cy okrzyk i wyprowadzasz seri� pot�nych cios�w na czaszk� i korpus przeciwnika. Ostatecznie bierzesz pot�ny zamach i wbijasz z wielk� si�� koniec $f w $S oczod�, od�upuj�c czaszk� od reszty ko��ca i ko�cz�c tym samym zaci�t� walk�.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n bierze g��boki oddech i rozpoczyna pot�n� szar�� p�dz�c na $C z niesamowit� pr�dko�ci� i wymachuj�c w powietrzu $f. B�d�c u celu wydaje og�uszaj�cy okrzyk i wyprowadza seri� pot�nych cios�w na czaszk� i korpus przeciwnika, wbijaj�c finalnie koniec $f w oczod�, od�upuj�c czaszk� od reszty ko��ca i ko�cz�c ju� i tak nieco wyd�u�ony �ywot $Z.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Dzier��c obur�cz $h, utkwiwszy wzrok w przeciwniku, bez najmniejszego zastanowienia rozpoczynasz pot�n� szar��. Biegn�� wprost na $C, wymachujesz szale�czo wok� g�owy sw� broni�. B�d�c u celu bierzesz pot�ny zamach i m��cisz bez opanowania kul�cego si� z b�lu przeciwnika wy�amuj�c jego �ebra.{x", ch, wield, victim, TO_CHAR );
						act( "{5Dostrzegasz, jak $n, dzier��c obur�cz $h, utkwiwszy wzrok w przeciwniku, bez najmniejszego zastanowienia rozpoczyna pot�n� szar��, wymachuj�c szale�czo wok� g�owy sw� broni�. Zmusza swe mi�nie do nadludzkiego wysi�ku i w ko�cu b�d�c u celu bierze pot�ny zamach i m��ci bez opanowania wij�cego si� z b�lu przeciwnika wy�amuj�c jego �ebra.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5�ciskaj�c w obu d�oniach $p i nie spuszczaj�c wzroku z $Z pr�bujesz wykona� pot�ny atak. Biegniesz w stron� bezbronnego w tej sytuacji przeciwnika najszybciej jak tylko mo�esz, a b�d�c ju� wystarczaj�co blisko, bierzesz t�gi zamach i wyprowadzasz seri� mro��cych krew w �y�ach trza�ni�� na korpus przeciwnika. Ka�demu uderzeniu towarzyszy odg�os �amanych �eber i nadkruszanych ko�ci.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n, �ciskaj�c w obu d�oniach $h i nie spuszczaj�c wzroku z $Z pr�buje wykona� pot�ny atak. Biegnie szybko w stron� bezbronnego w tej sytuacji przeciwnika, a gdy znajduje si� ju� wystarczaj�co blisko umarlaka, bierze t�gi zamach i wyprowadza seri� mro��cych krew w �y�ach trza�ni�� skierowanych na korpus przeciwnika. Co chwila s�ycha� nieprzyjemne zgrzyty i trzaski, a stos od�amanych ko�ci spada z gruchotem na ziemi�.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (IS_SET(victim->form, FORM_CONSTRUCT))
				{
					if ( dam <= 0 )
					{
						act( "{5�ciskaj�c $h w d�ugich susach zbli�asz si� do przeciwnika. Z ka�d� sekunda dystans mi�dzy wami maleje. Chwytasz $h za jeden z ko�c�w, poprawiasz chwyt i bierzesz pot�ny zamach. Twoja bro� opada z przera�aj�cym �wistem na ods�oni�ty �eb $Z. W momencie zderzenia or�a z celem do twych uszu dobiega og�uszaj�cy �oskot, a przez twe ramiona przebiega dreszcz niezno�nego b�lu i odr�twienia. Bro� odbija si� od twardej g�owy nie czyni�c przeciwnikowi �adnej szkody. Odskakujesz na bezpieczna odleg�o�� rozlu�niaj�c ramiona.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n p�dzi szale�czo na przeciwnika �ciskaj�c $h w r�kach. Odleg�o�� mi�dzy nimi zmniejsza si� z sekundy na sekund�. $n dopadaj�c $Z bierze pot�ny, szeroki zamach i t�ucze z ca�ych si� w ods�oni�ty �eb wroga. Kiedy bro� styka si� z czerepem do twych uszu dobiega og�uszaj�cy trzask i zduszony j�k b�lu, to $p odbija si� od twardego czo�a nie czyni�c $X najmniejszej krzywdy. $n wykrzywia twarz w cierpieniu i odskakuje par� metr�w rozlu�niaj�c obola�e ramiona.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Z rykiem w�ciek�o�ci rzucasz si� na przeciwnika. Unosisz wysoko w g�r� $h i bierzesz pot�ny zamach. Or� ze �wistem tnie powietrze i opada �ukowatym torem wprost ku celowi. Czerwone p�aty ob��du lataj�ce ci przed oczami zas�aniaj� nieco widoczno�� lecz teraz nie ma to ju� znaczenia. Bro� opada z potworn� si�� na kark $Z krusz�c go w dono�nym huku. Kaskady iskier i od�amk�w strzelaj� w powietrze na wiele metr�w wok�. Mocarne uderzenie posy�a oderwan� g�ow� wysoko w powietrze. Czerep zatacza szeroki �uk lawiruj�c powoli i opada z �oskotem na pod�o�e. Bezg�owy korpus ko�ysze si� jeszcze chwil� po czym opada z potwornym hukiem na ziemi� i rozpada si�.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n z rykiem w�ciek�o�ci rzuca si� na $C unosz�c $h wysoko w powietrze. Bierze pot�ny zamach i wyprowadza potworne walni�cie w ods�oni�ty kark swej ofiary. Rozp�dzony or� opada na cel krusz�c go w jednej sekundzie w dono�nym huku p�kaj�cego cia�a. Kaskady ostrych od�amk�w strzelaj� w powietrze na wiele metr�w wok�. Oderwany czerep lawiruje chwil� zataczaj�c szeroki �uk i l�duje z �oskotem na ziemi. Bezg�owe cia�o chwieje si� kilka sekund, po czym dopada na pod�o�e rozpadaj�c si� w u�amku sekundy.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Wyczekujesz odpowiedniego momentu i wyprowadzasz silne pchni�cie $f w ods�oni�ty korpus $Z, or� z trzaskiem t�ucze z potworn� si�� w $S bok. Z g�uchym trzaskiem spory kawa� materia�u, z kt�rego zbudowany jest przeciwnik, odrywa si� od reszty cia�a i wali z �oskotem na ziemi�. W boku wroga zieje olbrzymia wyrwa. $N chwiej�c si� na nogach kontynuuje powolny, mechaniczny i nienaturalny poch�d w twoim kierunku.{x", ch, wield, victim, TO_CHAR );
						act( "{5Nagle $n wydaj�c zduszony okrzyk wyprowadza pot�ne pchni�cie w ods�oni�ty korpus $Z, $s $h t�ucze z zastraszaj�c� si�� w bok ofiary, kt�ry kruszy si� z trzaskiem, a jego fragment odrywa si� od reszty cia�a i l�duje z �oskotem na ziemi. W tu�owiu $Z widnieje teraz olbrzymia, postrz�piona wyrwa.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Przekr�casz cia�o raz w lewo raz w prawo i wyczekujesz odpowiedniego momentu na zadanie ciosu. W pewnej chwili odchylaj�c do ty�u sylwetk� zauwa�asz, �e $N ods�ania si� na u�amek sekundy. Wykonujesz szeroki zamach $j i prostuj�c swe cia�o t�uczesz z ca�ych si� w kolana wroga. Ten chwiej�c si� chwil� ledwo odzyskuje r�wnowag� i kulej�c mocno zbli�a si� w twym kierunku wymachuj�c w�ciekle ramionami.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n wygina sw�j korpus w �uk i wykorzystuj�c luk� w obronie $Z bierze szeroki zamach i t�ucze $j w $S ods�oni�te kolana. Ten chwieje si� chwil�, ale odzyskuje r�wnowag�, i mocno kulej�c rusza w kierunku $z wymachuj�c w�ciekle ramionami.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if ( dam <= 0 )
				{
					act( "{5Biegniesz ze prosto na $Z szale�czo wymachuj�c $j, niestety bro� wypada ci z r�k i ledwo zd��asz j� podnie��.{x", ch, wield, victim, TO_CHAR );
					act( "{5$n biegnie prosto na ciebie szale�czo wymachuj�c $j, na szcz�cie bro� wypada mu z r�k i ledwo zd��a j� podnie��.{x", ch, wield, victim, TO_VICT );
					act( "{5$n biegnie ze sw� broni� prosto na $Z, jednak $p wypada z $s r�k i ledwo zd��a j� podnie��.{x", ch, wield, victim, TO_NOTVICT );
					return TRUE;
				}
				else if ( victim->hit - dam < -11 )
				{
					int types = 1;

					if ( ch->class == CLASS_BARBARIAN && is_affected(ch, gsn_berserk ) )
						types = 2;

					switch ( number_range( 0, types ) )
					{
						case 0:
							if ( IS_SET( victim->form, FORM_WARM ) || IS_SET( victim->form, FORM_COLD_BLOOD ) )
							{
								act( "{5Spogl�dasz na $C i ju� wiesz, �e to $S koniec. Z furi� rzucasz si� na wroga, wal�c go swym $j prosto w skro�. $S czaszka rozpada si� na kawa�ki, a twoj� twarz zalewa fala krwi i wyp�ywaj�cy m�zg!{x", ch, wield, victim, TO_CHAR );
								act( "{5$n widz�c, �e nie czujesz si� najlepiej, z furi� rzuca si� na ciebie, wal�c z ogromn� si�� $j prosto w tw� skro�. Przez chwilk� czujesz potworny b�l, a potem nie czujesz ju� nic.{x", ch, wield, victim, TO_VICT );
								act( "{5Dostrzegasz z�owieszczy b�ysk w oku $z. Widzisz jak z furi� rzuca si� $e na $C, wal�c swym $j prosto w $S skro�. Czaszka $Z rozpada si� na kawa�ki, a twarz zab�jcy zalewa fala krwi i wyp�ywaj�cy m�zg!{x", ch, wield, victim, TO_NOTVICT );
								make_brains( victim );
								break;
							}

						case 1:
							act( "{5Wrzeszcz�c g�o�no rzucasz si� na $C celuj�c pod brod�. Twoje pot�ne uderzenie urywa $S g�ow�, kt�ra po kilku sekundach l�duje na ziemi!{x", ch, wield, victim, TO_CHAR );
							act( "{5$n wrzeszczy g�o�no i biegnie prosto na ciebie, wal�c ci� pod brod�. B�l jest niewyobra�alny, ale po chwili na szcz�cie nie czujesz ju� nic.{x", ch, wield, victim, TO_VICT );
							act( "{5$n wrzeszcz�c g�o�no rzuca si� na $C, i z olbrzymim impetem trafia pod $S brod�. G�owa $Z wylatuje wysoko w powietrze i upada na ziemi� kilka metr�w obok!{x", ch, wield, victim, TO_NOTVICT );
							if ( IS_NPC( victim ) && victim->pIndexData->corpse_vnum < 0 )
								act( "{5Odr�bana g�owa $Z po chwili znika.{x", ch, wield, victim, TO_ROOM );
							else
								make_head( victim );
							break;

						case 2:
							act( "{5Ogarnia ci� sza� i w zaskakuj�cym tempie zbli�asz si� do $Z. W ostatniej chwili bierzesz pot�ny zamach i zadajesz $j pot�ny cios prosto w $S pier�. $N p�przytomny stoi przez chwil�, po czym osuwa si� na ziemi�.{x", ch, wield, victim, TO_CHAR );
							act( "{5$c ogarnia sza� i w zaskakuj�cym tempie biegnie w twoim kierunku. W ostatniej chwili bierze pot�ny zamach i wali ci� $j w klatk� piersiow�. Osuwasz si� p�przytomny z b�lu na ziemi� po czym tracisz �wiadomo��...{x", ch, wield, victim, TO_VICT );
							act( "{5$c ogarnia sza� i w zaskakuj�cym tempie zbli�a si� do $Z. W ostatniej chwili bierze pot�ny zamach i wali $j w $S pier�. Przez chwile panuje cisza, po czym $N pada na ziemi� i umiera...{x", ch, wield, victim, TO_NOTVICT );
							break;
					}
					return TRUE;
				}
				else
				{
					int types = 2;

					switch ( number_range( 0, types ) )
					{
						case 0:
							act( "{5Po wielkim rozp�dzie rzucasz si� na $C trzymaj�c wysoko nad g�ow� $h. Twoja bro� z wielk� si�� spada na $S �eb. $N na chwil� traci �wiadomo�� i wykonuje jakie� dziwne, nieskoordynowane ruchy.{x", ch, wield, victim, TO_CHAR );
							act( "{5$n po wielkim rozp�dzie rzuca si� na ciebie, trzymaj�c $h wysoko nad g�ow�. Wali ci� prosto w �eb, przez chwil� nie wiesz gdzie jeste� i co si� z tob� dzieje, a b�l wype�nia ca�e twe cia�o!{x", ch, wield, victim, TO_VICT );
							act( "{5Widzisz jak $n po wielkim rozp�dzie rzuca si� na $C, trzymaj�c $h wysoko nad g�ow�. Pot�ne hukni�cie $z pozbawia $C �wiadomo�ci, chwieje si� $E przez chwil� i wykonuje dziwne, nieskoordynowane ruchy.{x", ch, wield, victim, TO_NOTVICT );
							break;

						case 1:
							if ( IS_SET( victim->form, FORM_WARM ) || IS_SET( victim->form, FORM_COLD_BLOOD ) )
							{
								act( "{5Rozp�dzasz si�, trzymaj�c $h mocno w r�kach. Gdy jeste� ju� wystarczaj�co blisko $Z bierzesz szeroki zamach i grzmocisz bezlito�nie jego szcz�k�. Po chwili z zadowoleniem spogl�dasz jak $N spluwa krwi�.{x", ch, wield, victim, TO_CHAR );
								act( "{5Widzisz jak $n rozp�dza si� szale�czo, trzymaj�c $h mocno w r�kach. Gdy jest juz blisko ciebie bierze szeroki zamach i wali ci� prosto w szcz�k�!{x", ch, wield, victim, TO_VICT );
								act( "{5$n rozp�dza si� szale�czo, trzymaj�c obur�cz $h. B�d�c ju� blisko swojego celu bierze szeroki zamach i bezlito�nie grzmoci szcz�k� $Z, przez co $e soczy�cie spluwa krwi�.{x", ch, wield, victim, TO_NOTVICT );
								make_blood( victim );
								break;
							}

						case 2:
							switch (victim->sex)
							{
								case 0:
									act( "{5Cofasz si� kilka metr�w do ty�u, po czym biegniesz z $j w kierunku $Z. Nie wie $E co robi�, a ty wykorzystujesz to i walisz $j pod $S �ebra. $N jest zamroczony, a z $S g�by wyp�ywa bia�a piana.{x", ch, wield, victim, TO_CHAR );
									break;
								case 1:
									act( "{5Cofasz si� kilka metr�w do ty�u, po czym biegniesz z $j w kierunku $Z. Nie wie $E co robi�, a ty wykorzystujesz to i walisz $j pod $S �ebra. $N jest zamroczona, a z $S g�by wyp�ywa bia�a piana.{x", ch, wield, victim, TO_CHAR );
									break;
								default:
									act( "{5Cofasz si� kilka metr�w do ty�u, po czym biegniesz z $j w kierunku $Z. Nie wie $E co robi�, a ty wykorzystujesz to i walisz $j pod $S �ebra. $N jest zamroczone, a z $S g�by wyp�ywa bia�a piana.{x", ch, wield, victim, TO_CHAR );
									break;
							}
							act( "{5Obserwujesz dziwne zachowanie $z, a $e nagle cofa si� kilka metr�w, by po chwili ruszy� w twoim kierunku. Nie wiesz co robi�, a $n wykorzystuje to i wali ci� pod �ebra. Przez chwil� czujesz si� zamroczony, a z twoich ust wydostaje si� bia�a piana.{x", ch, wield, victim, TO_VICT );
							act( "{5$n cofa si� o kilka metr�w, po czym biegnie wymachuj�c $j w kierunku $Z. $N jest zak�opotany, a $n wykorzystuje to i wali pod $S �ebra. Poszkodowany chwieje si� zamroczony, a z $S ust wydostaje si� bia�a piana.{x", ch, wield, victim, TO_NOTVICT );
							break;
					}
					return TRUE;
				}
				break;
			case WEAPON_FLAIL:
				if (!str_cmp( race_table[ GET_RACE(victim) ].name, "zombi" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "kryptowiec" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "upiorny rycerz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "ghul" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "ghast" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "mumia" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "mumia wi�ksza" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "wampir" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Zbli�aj�c si� do przeciwnika oceniasz zimno sytuacj�. Mierzysz minimalny dystans potrzebny do zadania ostatecznego ciosu. $N ostro�nie sunie w twoj� stron� pow��cz�c nogami. Nagle stajesz w lekkim wykroku i zapieraj�c si� silnie nogami o pod�o�e unosisz w g�r� $h i opuszczasz z zastraszaj�c� si�� i pr�dko�ci�. O�ywieniec unosi w g�r� rami� w obronnym ge�cie. Bijak ze �wistem mija uniesion� d�o�, a brz�cz�ce cepigi owijaj� si� wok� g�rnej ko�czyny nie czyni�c krzywdy przeciwnikowi. W ostatniej chwili odchylasz g�ow� unikaj�c powracaj�cego w kierunku twojej twarzy bijaka. Szarpi�c za dzier�ak uwalniasz $h jednym ruchem.{x", ch, wield, victim, TO_CHAR );
						act( "{5Obserwujesz jak $n zimno kalkuluje szans� zadania ostatecznego ciosu. W pewnym momencie zapiera si� silnie nogami o pod�o�e, unosi w g�r� $h i opuszcza z zastraszaj�c� si��. Rozp�dzony bijak tn�c powietrze w�druje z pr�dko�ci� wichru w kierunku $Z. O�ywieniec unosi jednak przegni�e rami� w obronnym ge�cie. Brz�cz�ce cepigi owijaj� si� niegro�nie o rozk�adaj�c� si� ko�czyn� nie czyni�c nieumar�emu �adnej krzywdy. $n uchyla si� b�yskawicznie przed rozp�dzonym bijakiem wracaj�cym w kierunku $s twarzy, oraz jednym szarpni�ciem uwalnia sw� bro�.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Zaciskaj�c z�by z w�ciek�o�ci, warcz�c dziko z ob��ka�czym b�yskiem w zw�onych oczach biegniesz w d�ugich susach w kierunku przeciwnika z unosz�c wysoko $h. W kilka chwil dopadasz $Z i zadajesz morderczy cios. Bijak opada z zastraszaj�c� si�a i pr�dko�ci� trafiaj�c w skro� o�ywie�ca. Mia�d�ona, po��k�a ko�� zapada si� z dono�nym chrz�stem, uwalniaj�c ga�k� oczn� i rozbryzguj�cy si� wsz�dzie m�zg. Bijak sunie dalej mia�d��c przegni�y policzek, nos i szcz�k� nieumar�ego. Potworny impet walni�cia odrzuca $Z kilka metr�w w ty�. Przeciwnik chwieje si� zamroczony przez chwil�. Nie trac�c ani chwili poprawiaj�c chwyt na dzier�aku t�uczesz w klatk� piersiow� o�ywie�ca mia�d��c j� w huku p�kaj�cych ko�ci. Z przed�miertnym skrzekiem na ustach nieumar�y wali si� z �oskotem na pod�o�e i nieruchomieje ca�kowicie.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak $n warcz�c w�ciekle z b�yskiem ob��kania w zw�onych oczach naciera na przeciwnika, $s uniesiony or� ta�czy w�ciekle w powietrzu. $n w kilku d�ugich skokach dopada $Z i z dzik� si�� swych pot�nych ramion t�ucze bijakiem w g�ow� przeciwnika. Bro� z paskudnym chrz�stem opada na skro� uwalniaj�c ga�k� oczn� i bryzgaj�cy na wszystko m�zg nieumar�ego. Mia�d�y przy tym lew� po�ow� gnij�cej twarzy o�ywie�ca. Impet pot�nego uderzenia odrzuca przeciwnika na kilka metr�w. Poprawiaj�c chwyt na dzier�aku $n t�ucze z ca�ych si� w klatk� piersiow� wroga mia�d��c j� z dono�nym chrupotem. $N z przed�miertnym skrzekiem wali si� na pod�o�e i nieruchomieje.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5$N kieruje si� w twoj� stron� niezdarnie gubi�c przy tym cuchn�ce zgnilizn� p�aty mi�sa. �ciskaj�c mocno dier�ak $f bierzesz szeroki zamach i wyprowadzasz pot�ne walni�cie na korpus przeciwnika. Rozp�dzony bijak z chrz�stem mia�d�onych ko�ci oraz mlaskiem odrywanego cia�a opada na klatk� piersiow� przeciwnika. Z rozwartych, zniekszta�conych ust o�ywie�ca wydobywa si� pe�en b�lu skrzek i wylewa si� rzeka g�stej, cuchn�cej ��tawej juchy.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak $N niezdarnie sunie pow��cz�c zgni�ymi nogami w kierunku $z, $e zaciska mocno d�onie na dzier�aku $f, bierze szeroki zamach i wyprowadza pot�ne walni�cie na korpus przeciwnika. Rozp�dzony bijak w mgnieniu oka z paskudnym odg�osem mia�d�onych ko�ci i dono�nym mla�ni�ciem rozrywanego mi�sa oraz rwanych �y� dosi�ga klatki piersiowej. Z wykrzywionych, zniekszta�conych ust o�ywie�ca wydobywa si� pe�en b�lu skrzek oraz bucha fontanna g�stej, cuchn�cej ��tawej juchy.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Nacierasz zdecydowanie na przeciwnika. Unosisz $h w szerokim zamachu i opuszczasz z zastraszaj�c� si��, kieruj�c bijak na g�ow� $Z. O�ywieniec niezdarnie uchyla g�ow� unikaj�c pot�nego walni�cia, bijak jednak opada z chrz�stem na $S bark. Do twych uszu dociera st�umiony przez gnij�ce mi�so, suchy trzask p�kaj�cych ko�ci. Rami� przeciwnika osuwa si� nienaturalnie kilkana�cie centymetr�w w d� zwisaj�c bezw�adnie. Ze skrzekiem b�lu i w�ciek�o�ci $N rzuca si� na ciebie atakuj�c ze zdwojon� si�a.{x", ch, wield, victim, TO_CHAR );
						act( "{5Podziwiasz z zapartym tchem pot�ne natarcie $z. Unosi $e sw�j or� wysoko w g�r� i opuszcza go ze �wistem kieruj�c bijak na czaszk� $Z. O�ywieniec jednak przekrzywia g�ow� unikaj�c morderczego walni�cia, lecz $p opada z suchym trzaskiem mia�d�onych ko�ci st�umionym nieco przez gnij�ce mi�so na $S bark. Rami� przeciwnika osuwa si� kilkana�cie centymetr�w w d� i zwisa bezw�adnie wzd�u� rozk�adaj�cego si� korpusu. Z gard�a $Z wydobywa si� dono�ny skrzek b�lu i w�ciek�o�ci. Rzuca si� w furii na $c i atakuje ze zdwojon� si�� i precyzj�.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (!str_cmp( race_table[ GET_RACE(victim) ].name, "duch" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "banshee" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "cie�" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "widmo" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "zmora" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "upi�r" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Dostrzegaj�c przed sob� ledwo widoczn� sylwetk� niematerialnego cia�a $Z, zaczynasz wymachiwa� szale�czo swym pot�nym $j. Ju� prawie dostrzegasz jak wielka kula przebija niematerialne cia�o, jak ledwo widoczna dusza rozrywana jest na strz�py w pot�nym blasku czerwonej po�wiaty... Nagle jednak staje si� co� nieoczekiwanego! Tw�j cel momentalnie znika, a za plecami czujesz podmuch mro�nego wiatru. Trac�c r�wnowag� walisz si� na ziemi�, lecz przera�ony mo�liwymi skutkami tego b��du b�yskawicznie d�wigasz si� na nogi i nieco ju� roztropniej kontynuujesz walk�.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz, jak $n dostrzegaj�c przed sob� ledwo widoczn� sylwetk� $Z, zaczyna wymachiwa� szale�czo $f. Po chwili, b�d�c u celu, stara si� wykona� pot�ny atak, lecz w jednej chwili staje si� co� wprost niesamowitego! $N momentalnie znika i pojawia si� tu� za plecami $z, a $e z kolei trac�c r�wnowag� wali si� na ziemie jak bela, jednak�e szybko d�wiga si� na nogi i kontynuuje zaci�t� walk�.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Wymachuj�c $j bacznym wzrokiem �ledzisz tor ruchu pot�nej, nabijanej kolcami kuli, lawiruj�cej wok� twej g�owy. Ostatecznie, czuj�c przyp�yw niesamowitej energii bierzesz pot�ny zamach i wyprowadzasz silny, skierowany w stron� $Z atak. Kula zostawia w niematerialnym ciele wielk�, promieniuj�c� czerwonawym strumieniem pulsuj�cej negatywnej enrgii wyrw�, a w twej g�owie rozbrzmiewa pot�pie�czy wrzask b�lu. Otw�r w ciele zaczyna si� stopniowo powi�ksza�, wydaj�c si� zjada� $C od �rodka. W ko�cu nie zostaje ani �ladu, pr�cz odbijaj�cego si� jeszcze echem w twej g�owie, pe�nego b�lu krzyku.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz, jak wielka kula, lawiruj�ca wok� g�owy $z nabiera pr�dko�ci i finalnie spada na niematerialne cia�o $Z, pozostawiaj�c w nim wielk�, promieniuj�c� czerwonawym strumieniem pulsuj�cej negatywnej energii wyrw�, a twoje cia�o przeszywa dziwne uczucie b�lu. Otw�r z kolei zaczyna si� stopniowo powi�ksza�, wydaj�c si� zjada� $C od �rodka, a� ostatecznie nie zostaje ani �ladu.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Z dono�nym krzykiem na ustach dobiegasz do $Z i pot�nie wymachuj�c $j dokonujesz pot�nego ciosu! Ci�ka, �elazna kula opadaj�c z wielk� pr�dko�ci� przebija si� przez niematerialne cia�o niczym przez mas�o. Nagle odczuwasz, jakby wewn�trz twej g�owy co� pot�nie eksplodowa�o, jakby zagra�y w niej naraz tysi�ce dzwon�w. Przera�ony odbiegasz jak najdalej od krwawi�cego czerwonym blaskiem celu i nacierasz ponownie.{x", ch, wield, victim, TO_CHAR );
						act( "{5Z dono�nym okrzykiem na ustach $n dobiega do $Z i pot�nie wymachuj�c $j dokonuje pot�nego ciosu! Ci�ka, �elazna kula opadaj�c z wielk� pr�dko�ci� przebija si� przez niematerialne cia�o niczym przez mas�o. Okoliczne tereny rozdziera nie tyle s�yszalny, co odczuwalny krzyk b�lu, a zaniepokojony tym $n odbiega na bezpieczn� odleg�o�� i naciera ponownie.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5�ciskasz obur�cz jeszcze mocniej $h i rozpoczynasz szale�czy bieg. Niestety, w pewnym momencie okazuje si�, i� �le oceni�e� odleg�o�� i najprawdopodobniej zderzysz si� ze swym celem. Mimo to, wykonujesz pot�ny zamach, a $h spada na wroga tworz�c wyrw� w $S ciele, z kt�rej z sykiem wylatuje strumie� czerwonej, negatywnej energii.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n znajduje dogodny moment do wyprowadzenia pot�nej szar�y i rozpoczyna szale�czy bieg w stron� ods�oni�tego przeciwnika. Niestety �le ocenia odleg�o�� i najprawdopodobniej lada moment, nie mog�� wyhamowa�, zderzy si� z $V. Mimo to wykonuje z zapa�em pot�ny zamach, a jego $p spada na wroga tworz�c wyrw� w jego ciele, z kt�rej z sykiem wylatuje strumie� czerwonej, negatywnej enrgii.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (!str_cmp( race_table[ GET_RACE(victim) ].name, "drakolicz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "licz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "szkielet wojownik" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "szkielet" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Wci�gasz powietrze po czym rozpoczynasz szale�czy bieg w kierunku $Z. Bijak $f wiruje z�owieszczo, a ty wyprowadzasz pot�ne uderzenie w kierunku nieumar�ego, jednak jakims cudem ten dojrza� ci� i uchyli� si� przed ciosem, a ty si�� rozp�du zatrzyma�<&e�/a�/e�> si� dopiero po chwili.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n wci�ga powietrze po czym rozpoczyna szale�czy bieg w kierunku $Z. Bijak $s $f wiruje w powietrzu z�owieszczo gdy wyprowadza pot�ny cios w kierunku o�ywienca, jednak $N jakim� cudem zauwa�y� go i sprytnie si� przed nim uchyli�. {x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Ze w�ciek�ym rykiem wyprowadzasz morderczy cios skierowany w po�yskuj�c� czaszk� przeciwnika. Ten jednak nag�ym zrywem odchyla si� lekko do ty�u. Rozp�dzony bijak $f ze �wistem opada na miednic�. Si�a ciosu jest tak wielka, �e rozpada si� ona w mgnieniu oka z g�uchym odg�osem na tysi�ce drobnych kawa�k�w oddzielaj�c zmursza�e nogi $Z od korpusu. Chmura wiekowego, st�ch�ego py�y zas�ania ci widoczno��. Jak przez mg�� spogl�dasz na opadaj�cy, wymachuj�cy w powietrzu r�koma korpus przeciwnika. W jednej sekundzie unosisz bro� z morderczym b�yskiem w oku i opuszczasz j� raz za razem wgniataj�c o�ywie�ca w pod�o�e. Ze $Z nie pozostaje nic opr�cz porozrzucanych wsz�dzie kostnych drobin i ta�cz�cej w powietrzu chmury szarawego py�u. Ocieraj�c pot z czo�a warczysz w�ciekle rozgl�daj�c si� po okolicy.{x", ch, wield, victim, TO_CHAR );
						act( "{5Do twych uszu dociera potworny ryk $Z, szar�uje $e w�ciekle na przeciwnika. Pot�ne walni�cie z zastraszaj�c� pr�dko�ci� opada na ods�oni�t�, po�yskuj�c� czaszk� $Z, jednak o�ywieniec jest zbyt czujny i odchyla si� b�yskawicznie do ty�u, lecz rozp�dzony bijak opada na zmursza�� miednic� nieumar�ego, kt�ra z g�uchym, pustym �oskotem rozpada si� na tysi�ce drobin kostnych oddzielaj�c dolne ko�czyny od korpusu szkieletu. Wzbita przy tym chmura wiekowego py�u zas�ania widoczno��. Kiedy opada widzisz sylwetk� $z t�uk�c� bez opami�tania w le��cy korpus przeciwnika. Kiedy milknie odg�os ostatniego uderzenia po $X zostaje jedynie walaj�ca si� wsz�dzie kupka od�amk�w kostnych.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Chwytasz mocniej $h i z bojowym rykiem rzucasz si� na $C. W dw�ch krokach ju� stoisz u celu i twoja niszczycielska bro� leci w kierunku przeciwnika, kt�ry chyba nie wie co go czeka. Pot�ne uderzenie odrzuca $C w ty�, krusz�c �ebra i miednice o�ywienca. Szybko odskakujesz na bezpieczn� odleg�o��.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak $n mocniej chwyta $h, i z bojowym rykiem rzuca si� g��biej w wir walki. Pot�ny bijak $s broni zbliza si� z olbrzymi� pr�dkosci� do koscianego korpusu i uderza go z niszczycielsk� si��. $N odlatuje w ty�, a drobne kawa�ki od�amanych kosci i py�u unosz� si� jeszcze przez chwil�. $n odskakuje na bezpieczn� odleg�o��.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Z g�o�nym okrzykiem rzucasz si� na $C, wznosz�c $h wysoko i bior�c zamach. Pot�ne narz�dzie zniszczenia okreca si� kilka razy woko� twojej g�owy po czym uderza w cel. S�ycha� trzask p�kanych ko�ci, zgrzyt uderzenia o tward� powieszchnie po czym zatrzymujesz si� kilka metr�w za szkieletem.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n z g�o�nym okrzykiem rzuca si� na $C, wznos�c wysoko $h i bior�c zamach. Kula na �a�cuchu wiruje przez chwile w powietrzu po czym prowadzona pewn� r�k� trafia cel prosto w korpus. S�ychac trzask p�kanych kosci lecz $n zatrzymuje si� dopiero kilka krok�w za $V.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (IS_SET(victim->form, FORM_CONSTRUCT))
				{
					if ( dam <= 0 )
					{
						act( "{5�ciskaj�c obur�cz masywny or�, rozpoczynasz szale�czy bieg. Z bojowym okrzykiem, rzucasz si� zapalczywie na $C, $E jednak, przejrzawszy na wskro� twe niecne zamiary, robi zwinny unik, wysy�aj�c ci� na ziemi�. Odruchowo, podnosisz si� b�yskawicznie i kontynuujesz nieco rozs�dniej walk�.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n, �ciskaj�c obur�cz $h, rozpoczyna szale�czy bieg. Z bojowym okrzykiem, rzuca si� zapalczywie na $C, $E jednak robi zwinny unik, wysy�aj�c niedosz�ego zwyci�zc� na ziemi�. $n nie trac�c na czasie podnosi si� i kontynuuje ju� nieco rozs�dniej walk�.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Spogl�daj�c nienawistnie na rysuj�c� si� nieopodal masywn� sylwetk� $Z, zaciskasz mocniej d�onie na trzonku $f i wyprowadzasz pot�ny atak. Rozp�dzona do granic mo�liwo�ci ogromna kula spada z hukiem na korpus przeciwnika, przebijaj�c si� przez twardy, skorupiasty korpus. Wok� rozbrzmiewa dono�ne echo wal�cej si� na ziemi�, potrzaskanej sylwetki $Z.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n spogl�daj�c nienawistnie na rysuj�c� si� nieopodal masywn� sylwetk� $Z, zaciska mocniej d�onie na trzonku $f i wyprowadza pot�ny atak. Rozp�dzona do granic mo�liwo�ci ogromna kula spada z hukiem na korpus przeciwnika przebijaj�c si� przez twardy, skorupiasty korpus. Wsz�dzie rozbrzmiewa g�uchy odg�os wal�cej si� na ziemi� istoty.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Koncentrujesz si� najbardziej jak tylko mo�esz na $B i bez najmniejszego zastanowienia wyprowadzasz w jego stron� pot�ny atak wymachuj�c szale�czo $j. W ko�cu bierzesz zamach, a ogromna, stalowa kula spada z impetem na korpus $Z, od�upuj�c przy okazji wielkie kawa�y budulca.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n bez najmniejszego zastanowienia wyprowadza w stron� $Z pot�ny atak. Z bojowym zapa�em naciera na przeciwnika, wymachuj�c szale�czo $j. W ko�cu bierze zamach, a ogromna, stalowa kula spada z impetem na korpus $Z, odtrzaskuj�c par� poka�nej wielko�ci bry�, kt�re spadaj�c na ziemi�, d�wi�cznie postukuj�.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Dzier��c obur�cz $p, rozpoczynasz szale�czy bieg w stron� $Z i wymachujesz zapalczywie swym or�em, jakby� w ten spos�b chcia� odgoni� jakie� natr�tne owady. Gdy dobiegasz do niczego nie spodziewaj�cego si� przeciwnika bierzesz pot�ny zamach i zapalczywie �upiesz w jego masywny bark. Kula spada na� z wielkim hukiem, odrywaj�c przy okazji par� warstw dziwnego materia�u.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n dzier��c obur�cz $h, rozpoczyna szale�czy bieg w stron� $Z. Nabieraj�c na pr�dko�ci, wymachuje zapalczywie swym or�em, a gdy dobiega do $Z bierze pot�ny zamach i zapalczywie �upie w $S masywny bark. Kula spada na� z wielkim hukiem, uszkadzaj�c powa�nie cz�� ramienia, a unieruchomiona r�ka zwisa pod bardzo dziwnym k�tem.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if ( dam <= 0 )
				{
					act( "{5Biegniesz ze prosto na $Z szale�czo kr�c�c w powietrzu $j, niestety potykasz si� i kolczasta kula na �a�cuchu opl�tuje si� wok� ciebie harataj�c twe cia�o.{x", ch, wield, victim, TO_CHAR );
					act( "{5$n biegnie prosto na ciebie szale�czo kr�c�c w powietrzu $j, na szcz�cie �amaga potyka si� i kolczasta kula na �a�cuchu opl�tuje si� wok� $s cia�a harataj�c je.{x", ch, wield, victim, TO_VICT );
					act( "{5$n biegnie ze prosto na $Z szale�czo kr�c�c w powietrzu $j, jednak �amaga potyka si� i kolczasta kula na �a�cuchu opl�tuje si� wok� $s cia�a harataj�c je.{x", ch, wield, victim, TO_NOTVICT );
					ch->hit = ch->hit - ( dice( wield->value[ 1 ], wield->value[ 2 ]) + wield->value[ 6 ] );
					update_pos( ch );
					return TRUE;
				}
				else if ( victim->hit - dam < -11 )
				{
					int types = 1;

					if ( ch->class == CLASS_BARBARIAN && is_affected(ch, gsn_berserk ) )
						types = 2;

					switch ( number_range( 0, types ) )
					{
						case 0:
							if ( IS_SET( victim->form, FORM_WARM ) || IS_SET( victim->form, FORM_COLD_BLOOD ) )
							{
								act( "{5Spogl�dasz na $C i ju� wiesz, �e to $S koniec. Z furi� rzucasz si� na wroga, wal�c go kolczast� kul� swego $f prosto w skro�. $S czaszka rozpada si� na kawa�ki, a twoj� twarz zalewa fala krwi!{x", ch, wield, victim, TO_CHAR );
								act( "{5$n widz�c, �e nie czujesz si� najlepiej, z furi� rzuca si� na ciebie, wal�c z ogromn� si�� kolczast� kul� swego $f prosto w tw� skro�. Przez chwilk� czujesz potworny b�l, a potem nie czujesz ju� nic.{x", ch, wield, victim, TO_VICT );
								act( "{5Dostrzegasz z�owieszczy b�ysk w oku $z. Widzisz jak z furi� rzuca si� $e na $C, wal�c kolczast� kul� swego $f prosto w $S skro�. Czaszka $Z rozpada si� na kawa�ki, a twarz zab�jcy zalewa fala krwi!{x", ch, wield, victim, TO_NOTVICT );
								make_blood( victim );
								break;
							}
						case 1:
							act( "{5Wrzeszcz�c g�o�no rzucasz si� na $C kr�c�c w�ciekle kolczast� kul� swego $f celuj�c w $S bok. Twoje pot�ne uderzenie ca�kowicie mia�d�y miednic� $Z, a temu zdarzeniu towarzyszy g�o�ny pisk ofiary i dono�ny gruchot p�kaj�cych ko�ci. $N wykrzywia si� nienaturalnie i upada na ziemi�.{x", ch, wield, victim, TO_CHAR );
							act( "{5$n wrzeszczy g�o�no rzuca si� na ciebie kr�c�c w�ciekle kolczast� kul� swego $f. Cios by� tak szybki, �e nawet go nie zauwa�y�<&e�/a�/o�>, przez chwilk� czujesz tylko dziwny strzyk w boku. Nagle przestajesz czu� nogi. Ostatnimi si�y zauwa�asz jak �wiat kr�ci si� dooko�a, pod�oga pojawia si� tu� nad tob�, a ty na ni� spadasz...{x", ch, wield, victim, TO_VICT );
							act( "{5$n wrzeszcz�c g�o�no rzuca si� na si� na $C kr�c�c w�ciekle kolczast� kul� swego $f celuj�c w $S bok. $s pot�ne uderzenie ca�kowicie mia�d�y miednic� $Z, a temu zdarzeniu towarzyszy g�o�ny pisk ofiary i dono�ny gruchot p�kaj�cych ko�ci. $N wykrzywia si� nienaturalnie i upada na ziemi�.{x", ch, wield, victim, TO_NOTVICT );
							break;
						case 2:
							act( "{5Ogarnia ci� sza� i w zaskakuj�cym tempie zbli�asz si� do $Z. W ostatniej chwili bierzesz pot�ny zamach i zadajesz $j pot�ny cios prosto w $S pier�. $N p�przytomny stoi przez chwil�, po czym osuwa si� na ziemi�.{x", ch, wield, victim, TO_CHAR );
							act( "{5$c ogarnia sza� i w zaskakuj�cym tempie biegnie w twoim kierunku. W ostatniej chwili bierze pot�ny zamach i wali ci� $j w klatk� piersiow�. Osuwasz si� p�przytomny z b�lu na ziemi� po czym tracisz �wiadomo��...{x", ch, wield, victim, TO_VICT );
							act( "{5$c ogarnia sza� i w zaskakuj�cym tempie zbli�a si� do $Z. W ostatniej chwili bierze pot�ny zamach i wali $j w $S pier�. Przez chwile panuje cisza, po czym $N pada na ziemi� i umiera.{x", ch, wield, victim, TO_NOTVICT );
							break;
					}
					return TRUE;
				}
				else
				{
					int types = 0;
					int types2 = 1;
					if ( ch->class == CLASS_BARBARIAN && is_affected(ch, gsn_berserk ) )
					{
						types = 2;
						types2 = 2;
					}
					switch ( number_range( types, types2 ) )
					{
						case 0:
							act( "{5Po wielkim rozp�dzie rzucasz si� na $C trzymaj�c wysoko nad g�ow� $h. Kolczasta kula twej broni z wielk� si�� spada na $S �eb. $N na chwil� traci �wiadomo�� i wykonuje jakie� dziwne, nieskoordynowane ruchy.{x", ch, wield, victim, TO_CHAR );
							act( "{5$n po wielkim rozp�dzie rzuca si� na ciebie, trzymaj�c $h wysoko nad g�ow�. Wielka kolczasta kula spada z ogromn� si�� na tw� g�ow�, przez chwil� nie wiesz gdzie jeste� i co si� z tob� dzieje, a b�l wype�nia ca�e twe cia�o!{x", ch, wield, victim, TO_VICT );
							act( "{5Widzisz jak $n po wielkim rozp�dzie rzuca si� na $C, trzymaj�c $h wysoko nad g�ow�. Pot�ne hukni�cie $z pozbawia $C �wiadomo�ci, chwieje si� $E przez chwil� i wykonuje dziwne, nieskoordynowane ruchy.{x", ch, wield, victim, TO_NOTVICT );
							break;

						case 1:
							act( "{5Rozp�dzasz si�, trzymaj�c $h mocno w r�kach. Gdy jeste� ju� wystarczaj�co blisko $Z bierzesz szeroki zamach i grzmocisz bezlito�nie jego szcz�k�. Po chwili z zadowoleniem spogl�dasz jak $N spluwa krwi�.{x", ch, wield, victim, TO_CHAR );
							act( "{5Widzisz jak $n rozp�dza si� szale�czo, trzymaj�c $h mocno w r�kach. Gdy jest juz blisko ciebie bierze szeroki zamach i wali ci� prosto w szcz�k�!{x", ch, wield, victim, TO_VICT );
							act( "{5$n rozp�dza si� szale�czo, trzymaj�c obur�cz $h. B�d�c ju� blisko swojego celu bierze szeroki zamach i bezlito�nie grzmoci szcz�k� $Z, przez co $e soczy�cie spluwa krwi�.{x", ch, wield, victim, TO_NOTVICT );
							break;

						case 2:
							act( "{5Wype�niaj�ca ci� furia eksploduje i lekko cofaj�c ci� bierzesz naprawd� olbrzymi zamach i z ca�ej si�y, z lekkiego doskoku, walisz kolczast� kul� swego $f prosto w pier� $Z gruchocz�c $S �ebra! $N g�o�no wzdycha i j�cz�c wypuszcza powietrze z p�uc.{x", ch, wield, victim, TO_CHAR );
							act( "{5Nagle dostrzegasz b�ysk furii w oczach $z. Odsuwa si� $e lekko od ciebie i z lekkim doskokiem oraz rykiem na ustach bierze naprawd� olbrzymi zamach i wali ci� kolczast� kul� swego $f prosto w pier� gruchocz�c twe �ebra i przyprawiaj�c straszny b�l!{x", ch, wield, victim, TO_VICT );
							act( "{5Nagle dostrzegasz b�ysk furii w oczach $z. Odsuwa si� $e lekko od $Z i bior�c naprawd� olbrzymi zamach doskakuje do swej ofiary i z rykiem na ustach mia�dzy jej �ebra kolczast� kul� swego $f. $N g�o�no wzdycha i j�cz�c wypuszcza powietrze z p�uc.{x", ch, wield, victim, TO_NOTVICT );
							break;
					}
					return TRUE;
				}
				break;
		}
	}
	else if ( dt == gsn_stun )
	{
		if ( immune )
		{
			act( "{5Tw�j cios l�duje na czole $Z, ale $E nawet nie zwraca na to uwagi.{x", ch, wield, victim, TO_CHAR );
			act( "{5Cios $z l�duje ci na czole, jednak nawet nie zwracasz na to uwagi.{x", ch, wield, victim, TO_VICT );
			act( "{5Cios $z l�duje na czole $Z, ale $E nawet nie zwraca na to uwagi.{x", ch, NULL, victim, TO_NOTVICT );
		}
		else if ( dam > 0 )
		{
			act( "{5Tw�j cios l�duje na czole $Z kt�r$R przez chwil� chwieje si� og�uszon$t.{x", ch, victim->sex == 2 ? "a" : victim->sex == 0 ? "e" : "y", victim, TO_CHAR );
			act( "{5Cios $z l�duje ci na czole. Przez chwil� nie widzisz niczego opr�cz gwiazd.{x", ch, wield, victim, TO_VICT );
			act( "{5Cios $z l�duje na czole $Z, kt�r$R przez chwil� chwieje si� og�uszon$t.{x", ch, victim->sex == 2 ? "a" : victim->sex == 0 ? "e" : "y", victim, TO_NOTVICT );
		}
		else if ( dam <= 0 )
		{
			act( "{5Tw�j cios o w�os mija g�ow� $Z.{x", ch, wield, victim, TO_CHAR );
			act( "{5Cios $z ze �wistem przelatuje ko�o twojej g�owy.{x", ch, wield, victim, TO_VICT );
			act( "{5Cios $z o w�os mija g�ow� $Z.{x", ch, NULL, victim, TO_NOTVICT );
		}
		return TRUE;
	}
	else if ( dt == gsn_crush )
	{
		if ( immune )
		{
			act( "{5Tw�j cios jest nieskuteczny, $X nic si� nie sta�o.{x", ch, wield, victim, TO_CHAR );
			act( "{5Cios $z jest nieskuteczny, nic ci si� nie sta�o{x", ch, wield, victim, TO_VICT );
			act( "{5Pot�ny cios $z jest nieskuteczny, $X nic si� nie sta�o.{x", ch, NULL, victim, TO_NOTVICT );
		}
		return TRUE;
	}
	if ( dt == gsn_tail )
	{
		if ( immune )
		{
			act( "{5Robisz szybki zamach ogonem w kierunku $Z i uderzasz $M najmocniej jak potrafisz, jednak $E nawet nie drgnie.{x", ch, NULL, victim, TO_CHAR );
			act( "{5$n robi szybki zamach swoim ogonem i uderza ci� nim najmocniej jak mo�e, jednak nawet tego nie zauwa�asz.{x", ch, NULL, victim, TO_VICT );
			act( "{5$n robi szybki zamach ogonem w kierunku $Z i uderza go najmocniej jak potrafi, jednak $E nawet tego nie zauwa�a.{x", ch, NULL, victim, TO_NOTVICT );
			return TRUE;
		}
		else if ( dam <= 0 )
		{
			switch ( number_range( 0, 3 ) )
			{
				case 0:
					act( "{5Bierzesz zamach ogonem i pr�bujesz nim trafi� $C, ale chybiasz $M z kretesem.{x", ch, NULL, victim, TO_CHAR );
					act( "{5$n wykonuje jakie� dziwne ruchy, jakby pr�bowa� uderzy� ci� ogonem, jednak chybia z kretesem.{x", ch, NULL, victim, TO_VICT );
					act( "{5$n robi szybki zamach swoim ogonem, jednak chybia $C z kretesem.{x", ch, NULL, victim, TO_NOTVICT );
					break;

				case 1:
					act( "{5Robisz zamach swoim ogonem, jednak $N wykonuje nag�y unik i tw�j cios chybia.{x", ch, NULL, victim, TO_CHAR );
					act( "{5$n robi zamach swoim ogonem, jednak szybki unik wystarcza, aby silny, acz niedok�adny cios omin�� ci� o par� centymetr�w.{x", ch, NULL, victim, TO_VICT );
					act( "{5$n wykonuje zwinny zamach swoim ogonem, szybki unik jednak wystarcza, aby silny, acz niedok�adny cios chybi� $Z.{x", ch, NULL, victim, TO_NOTVICT );
					break;

				case 2:
					act( "{5Pr�bujesz uderzy� ogonem $C, ale tracisz na sekund� r�wnowag� i chybiasz haniebnie.{x", ch, NULL, victim, TO_CHAR );
					act( "{5$n pr�buje ci� uderzy� swoim umi�nionym ogonem, ale podczas zamachu traci na sekund� r�wnowag� i chybia haniebnie.{x", ch, NULL, victim, TO_VICT );
					act( "{5$n pr�buje uderzy� swoim ogromnym ogonem $C, ale podczas zamachu traci na sekund� r�wnowag� i chybia haniebnie.{x", ch, NULL, victim, TO_NOTVICT );
					break;

				case 3:
					act( "{5Pr�buj�c uderzy� swoim ogonem $C prawie $t obr�t o p� osi.{x", ch, ch->sex == 2 ? "zrobi�a�" : "zrobi�e�", victim, TO_CHAR );
					act( "{5$n wykonuje jakie� podejrzane ruchy, bierze tak t�gi zamach swoim ogonem, �e kiedy nie trafia obraca si� do ciebie ty�em.{x", ch, NULL, victim, TO_VICT );
					act( "{5$n wykonuje jakie� podejrzane ruchy, bierze tak t�gi zamach swoim ogonem, �e kiedy nie trafia obraca si� do $Z ty�em.{x", ch, NULL, victim, TO_NOTVICT );
					break;
			}
			return TRUE;
		}
		else if ( victim->hit - dam < -11 )
		{
			switch ( number_range( 0, 2 ) )
			{
				case 0:
					act( "{5Przymierzasz si� do zamachu swoim ogonem, wyrzucasz go gwa�townie do przodu, lekko si� obracaj�c, po czym uderzasz $C. Niesamowicie pot�ny cios sprawia, �e $N pada na ziemi�, pluj�� krwi� rz�zi chwil� po czym umiera w niewypowiedzianym b�lu.{x", ch, NULL, victim, TO_CHAR );
					act( "{5$n przymierza si� przez chwil� do zamachu swoim ogonem, wyrzuca go gwa�townie do przodu i w powietrzu jakby $t zamiar, bowiem uderza ci� prosto w g�ow�. Widzisz jak oczy zachodz� ci czarno-czerwon�, krwaw� mg��. Powoli opadasz bez si� na kolana, nast�pnie na plecy. Ostatkiem si� pr�bujesz zaczerpn�c odrobin� powietrza, jednak cia�o ju� nie s�uga m�gu. Czujesz jak odp�ywasz... Umierasz..{x", ch, ch->sex == 2 ? "zmieni�a" : "zmieni�", victim, TO_VICT );
					act( "{5$n przymierza si� przez chwil� do zamachu swoim ogonem, wyrzuca go gwa�townie do przodu i w powietrzu jakby $t zamiar, bowiem uderza $C prosto mi�dzy oczy. Pot�ny cios sprawia, �e $N pada na kolana, a nast�pnie osuwa si� na ziemi�. Rz�zi jeszcze przez chwil� i umiera w niewypowiedzianym b�lu.{x", ch, ch->sex == 2 ? "zmieni�a" : "zmieni�", victim, TO_NOTVICT );
					break;

				case 1:
					act( "{5Udeszasz ogonem $C tak mocno jak tylko potrafisz. $N pada natychmiast na ziemi� ca�kowicie oszo�omion$R. Przez chwile wydaje ci si�, �e dostrzegasz gasn�ce w $Q �ycie.{x", ch, NULL, victim, TO_CHAR );
					act( "{5$n uderza ci� mocno swoim ogonem, sprawiaj�c, �e przewracasz si� na ziemi�. Uderzenie wypchn�o z twoich p�uc ca�e powietrze, a gdy pr�bujesz go troche zaczerpn�� z twojego gard�a wydobywa si� jedynie g�uche rz�enie, oczy zachodz� ci mg��, umierasz.{x", ch, NULL, victim, TO_VICT );
					act( "{5$n uderza $C mocno swoim ogonem wk�adaj�c w to chyba ca�� si�� i umiej�tno�ci. $N upada na ziemi� zupe�nie oszo�omion$R, dostrzegasz tylko przez moment gasn�ce w $Q �ycie a ju� po sekundzie jest po wszystkim.{x" , ch, NULL, victim, TO_NOTVICT );
					break;

				case 2:
					act( "{5$N stara si� w ostatniej chwili wymin�� tw�j cios, otrzymuje jednak pot�ne uderzenie twoim ogonem nie bed�c na to w og�le przygotowan$R. $N pada na ziemi�, wyje z b�lu i nieruchomieje.{x", ch, NULL, victim, TO_CHAR );
					act( "{5Otrzymujesz silne uderzenie ogonem od $z prosto w kark. Czujesz jak p�ka ci kr�gos�up, a w ca�ym ciele tracisz czucie... umierasz natychmiast.{x", ch, NULL, victim, TO_VICT );
					act( "{5$N stara si� w ostatniej chwili omin�� nadlatuj�cy cios, prawie mu si� udaje, ale po chwili jednak zostaje przez $c pot�nie uderzony ogonem w kark. S�ycha� tylko g�o�ny chrupot p�kaj�cego kr�gos�upa. $N wali si� bez czucia na ziemi� i umiera.{x", ch, NULL, victim, TO_NOTVICT );
					break;
			}
			return TRUE;
		}
		else
		{
			switch ( number_range( 0, 3 ) )
			{
				case 0:
					act( "{5Wyczekujesz cierpliwie na okazje po czym uderzasz $C z ca�ej si�y swoim ogonem.{x", ch, NULL, victim, TO_CHAR );
					act( "{5$n wyczekuje na odpowiedni� chwil�, kiedy jeste� $t i uderza ci� z ca�ej si�y swoim ogonem.{x", ch, victim->sex == 2 ? "ods�oni�ta" : "ods�oni�ty", victim, TO_VICT );
					act( "{5$n wyczekuje na odpowiedni moment, kiedy $N jest $t i uderza $M z ca�ej si�y swoim ogonem.{x", ch, victim->sex == 2 ? "ods�oni�ta" : "ods�oni�ty", victim, TO_NOTVICT );
					break;

				case 1:
					act( "{5Odchodzisz lekko od $Z i uderzasz $M swoim ogonem tak mocno jak tylko mo�esz.{x", ch, NULL, victim, TO_CHAR );
					act( "{5$n cofa si� przezornie od ciebie nabieraj�c dystansu, po czym uderza ci� swoim ogonem najmocniej jak potrafi.{x", ch, NULL, victim, TO_VICT );
					act( "{5$n cofa si� przezornie od $Z nabieraj�c dystansu, po czymi uderza $M swoim ogonem najmocniej jak potrafi.{x", ch, NULL, victim, TO_NOTVICT );
					break;

				case 2:
					act( "{5Odczekujesz chwil� a� $N si� troch� ods�oni i kiedy to si� staje uderzasz $M swoim ogonem.{x", ch, NULL, victim, TO_CHAR );
					act( "{5$n przez chwile dziwnie si� zachowuje... Zastanawiaj�c si� nad tym, na sekunde ods�aniasz si� i natychmiast odczuwasz na sobie skutek uderzenia ogona.{x", ch, victim->sex == 2 ? "pocz�stowana" : "pocz�stowany", victim, TO_VICT );
					act( "{5$n przez chwil� dziwnie si� zachowuje obserwuj�c $Z, po czym wykorzystuj�c dogodny moment i kiedy $t si� ods�ania, najmocniej jak potrafi uderza swoim ogonem.{x", ch, victim->sex == 2 ? "tamta" : "tamten", victim, TO_NOTVICT );
					break;

				case 3:
					act( "{5Ooo, udaje ci si� wyprowadzi� niez�y cios, uderzasz ogonem $C z p�obrotu.{x", ch, NULL, victim, TO_CHAR );
					act( "{5Niech to! $x udaje si� zrobi� g��boki zamach swoim ogonem, a nast�pnie uderza ci� nim. Niezbyt interesuje ci� jakim cudem to sie sta�o, wa�nym jest wszechogarniaj�cy twoje cia�o b�l spowodowany mocarnym uderzeniem ogona!{x", ch, NULL, victim, TO_VICT );
					act( "{5$x udaje si� zrobi� gwa�towny, mocny i precyzyjny zamach swoim ogonem. Trafia $C prosto w czu�e miejsce!{x", ch, NULL, victim, TO_NOTVICT );
					break;
			}
			return TRUE;
		}
	}
	else if ( dt == gsn_trip )
	{
		if ( dam > 0 )
		{
			act( "{5Przewracasz $C, $N z g�o�nym st�kni�ciem l�duje na ziemi�!{x", ch, NULL, victim, TO_CHAR );
			act( "{5$n przewraca ci�, walisz si� na ziemi�!{x", ch, NULL, victim, TO_VICT );
			act( "{5$n przewraca $C, posy�aj�c $M na ziemi�.{x", ch, NULL, victim, TO_NOTVICT );
			victim->position = POS_SITTING;
		}
		else
		{
			act( "{5Probujesz przewr�ci� $C, bezskutecznie!{x", ch, NULL, victim, TO_CHAR );
			act( "{5$n pr�buje ci� przewr�ci�, bezskutecznie!{x", ch, NULL, victim, TO_VICT );
			act( "{5$n pr�buje przewr�ci� $C, bezskutecznie.{x", ch, NULL, victim, TO_NOTVICT );
		}
		return TRUE;
	}
	else if ( dt == gsn_smite )
	{
		if ( victim->hit - dam < -11 )
		{
			act( "{5Zbierasz w sobie moce dobra i czuj�c jak rozpiera ci� niesamowita si�a zadajesz $X �miertelny cios.{x", ch, NULL, victim, TO_CHAR );
			act( "{5$n zbiera w sobie moce dobra i zadaje ci �miertelny cios!{x", ch, NULL, victim, TO_VICT );
			act( "{5$n zbiera w sobie moce dobra i zadaje $X �miertelny cios.{x", ch, NULL, victim, TO_NOTVICT );
		}
		else
		{
			if ( dam >= 75 )
			{
				act( "{5Szepcz�c s�owa kr�tkiej modlitwy prosisz bog�w o wsparcie i natychmiast wyprowadzasz morderczy cios, kt�ry rozpruwa $C!{x", ch, NULL, victim, TO_CHAR );
				act( "{5$n szepcze s�owa kr�tkiej modlitwy prosi bog�w o wsparcie i natychmiast wyprowadza morderczy cios, kt�ry ci� rozpruwa!{x", ch, NULL, victim, TO_VICT );
				act( "{5$n szepcze s�owa kr�tkiej modlitwy prosi bog�w o wsparcie i natychmiast wyprowadza morderczy cios, kt�ry rozpruwa $C!{x", ch, NULL, victim, TO_NOTVICT );
			}
			else if ( dam >= 50 && dam < 75 )
			{
				act( "{5Skupiasz w sobie energi� �wiat�a po czym wyprowadzasz silny cios rani�c mocno $C.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$n skupia w sobie energi� �wiat�a po czym wyprowadza silny cios mocno ci� rani�c.{x", ch, NULL, victim, TO_VICT );
				act( "{5$n skupia w sobie energi� �wiat�a po czym wyprowadza silny cios rani�c mocno $C.{x", ch, NULL, victim, TO_NOTVICT );
			}
			else if ( dam >= 35 && dam < 50 )
			{
				act( "{5Zbierasz w sobie moce dobra i wyprowadzasz pot�ny cios dotkliwie rani�c $C.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$n zbiera w sobie moce dobra i wyprowadza pot�ny cios dotkliwie ci� rani�c.{x", ch, NULL, victim, TO_VICT );
				act( "{5$n zbiera w sobie moce dobra i wyprowadza pot�ny cios dotkliwie rani�c $C.{x", ch, NULL, victim, TO_NOTVICT );
			}
			else if ( dam >= 18 && dam < 35 )
			{
				act( "{5Zbierasz w sobie moce dobra i wyprowadzasz pot�ny cios rani�c $C.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$n zbiera w sobie moce dobra i wyprowadza pot�ny cios rani�c ci�.{x", ch, NULL, victim, TO_VICT );
				act( "{5$n zbiera w sobie moce dobra i wyprowadza pot�ny cios rani�c $C.{x", ch, NULL, victim, TO_NOTVICT );
			}
			else if ( dam > 0 )
			{
				act( "{5Zbierasz w sobie moce dobra i wyprowadzasz cios rani�c lekko $C.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$n zbiera w sobie moce dobra i wyprowadza cios rani�c ci� lekko.{x", ch, NULL, victim, TO_VICT );
				act( "{5$n zbiera w sobie moce dobra i wyprowadza cios rani�� lekko $C.{x", ch, NULL, victim, TO_NOTVICT );
			}
			else
			{
				act( "{5Zbierasz w sobie moce dobra i pr�bujesz wyprowadzi� cios w kierunku $Z, jednak potykasz si� i ledwo utrzymujesz r�wnowag�.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$n zbiera w sobie moce dobra i pr�buje wyprowadzi� cios w twoim kierunku, jednak potyka si� i ledwo utrzymuje r�wnowag�.{x", ch, NULL, victim, TO_VICT );
				act( "{5$n zbiera w sobie moce dobra i pr�buje wyprowadzi� cios w kierunku $Z, jednak potyka si� i ledwo utrzymuje r�wnowag�.{x", ch, NULL, victim, TO_NOTVICT );
			}
		}
		return TRUE;
	}
	else if ( dt == gsn_smite_good )
	{
		if ( victim->hit - dam < -11 )
		{
			act( "{5Zbierasz w sobie moce z�a i czuj�c jak rozpiera ci� niesamowita si�a zadajesz $X �miertelny cios.{x", ch, NULL, victim, TO_CHAR );
			act( "{5$n zbiera w sobie moce z�a i zadaje ci �miertelny cios!{x", ch, NULL, victim, TO_VICT );
			act( "{5$n zbiera w sobie moce z�a i zadaje $X �miertelny cios.{x", ch, NULL, victim, TO_NOTVICT );
		}
		else
		{
			if ( dam >= 75 )
			{
				act( "{5Szepcz�c s�owa kr�tkiej modlitwy prosisz mrocznych bog�w o wsparcie i natychmiast wyprowadzasz morderczy cios, kt�ry rozpruwa $C!{x", ch, NULL, victim, TO_CHAR );
				act( "{5$n szepcze s�owa kr�tkiej modlitwy prosi mrocznych bog�w o wsparcie i natychmiast wyprowadza morderczy cios, kt�ry ci� rozpruwa!{x", ch, NULL, victim, TO_VICT );
				act( "{5$n szepcze s�owa kr�tkiej modlitwy prosi mrocznych bog�w o wsparcie i natychmiast wyprowadza morderczy cios, kt�ry rozpruwa $C!{x", ch, NULL, victim, TO_NOTVICT );
			}
			else if ( dam >= 50 && dam < 75 )
			{
				act( "{5Skupiasz w sobie energi� ciemno�ci po czym wyprowadzasz silny cios powa�nie rani�c $C.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$n skupia w sobie energi� ciemno�ci po czym wyprowadza silny cios powa�nie ci� rani�c.{x", ch, NULL, victim, TO_VICT );
				act( "{5$n skupia w sobie energi� ciemno�ci po czym wyprowadza silny cios powa�nie rani�c $C.{x", ch, NULL, victim, TO_NOTVICT );
			}
			else if ( dam >= 35 && dam < 50 )
			{
				act( "{5Zbierasz w sobie mroczne moce i wyprowadzasz pot�ny cios dotkliwie rani�c $C.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$n zbiera w sobie mroczne moce i wyprowadza pot�ny cios dotkliwie ci� rani�c.{x", ch, NULL, victim, TO_VICT );
				act( "{5$n zbiera w sobie mroczne moce i wyprowadza pot�ny cios dotkliwie rani�c $C.{x", ch, NULL, victim, TO_NOTVICT );
			}
			else if ( dam >= 18 && dam < 35 )
			{
				act( "{5Zbierasz w sobie moce z�a i wyprowadzasz pot�ny cios rani�c $C.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$n zbiera w sobie moce z�a i wyprowadza pot�ny cios rani�c ci�.{x", ch, NULL, victim, TO_VICT );
				act( "{5$n zbiera w sobie moce z�a i wyprowadza pot�ny cios rani�c $C.{x", ch, NULL, victim, TO_NOTVICT );
			}
			else if ( dam > 0 )
			{
				act( "{5Zbierasz w sobie mroczne moce i wyprowadzasz cios rani�c lekko $C.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$n zbiera w sobie mroczne moce i wyprowadza cios rani�c ci� lekko.{x", ch, NULL, victim, TO_VICT );
				act( "{5$n zbiera w sobie mroczne moce i wyprowadza cios rani�� lekko $C.{x", ch, NULL, victim, TO_NOTVICT );
			}
			else
			{
				act( "{5Zbierasz w sobie mroczne moce i pr�bujesz wyprowadzi� cios w kierunku $Z, jednak potykasz si� i ledwo utrzymujesz r�wnowag�.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$n zbiera w sobie mroczne moce i pr�buje wyprowadzi� cios w twoim kierunku, jednak potyka si� i ledwo utrzymuje r�wnowag�.{x", ch, NULL, victim, TO_VICT );
				act( "{5$n zbiera w sobie mroczne moce i pr�buje wyprowadzi� cios w kierunku $Z, jednak potyka si� i ledwo utrzymuje r�wnowag�.{x", ch, NULL, victim, TO_NOTVICT );
			}
		}
		return TRUE;
	}
	return FALSE;
}

/* autoresc dla berserkerow */
void check_berserk_rescue( CHAR_DATA * ch )
{
    CHAR_DATA * tank = NULL, *tch, *victim;
    int skill;

    if ( !ch ||
            !is_affected( ch, gsn_berserk ) ||
            !ch->fighting ||
            !ch->fighting->fighting ||
            ch->fighting->fighting == ch )
    {
        return;
    }

    victim = ch->fighting;

    for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
    {
        if ( tch != ch && victim->fighting == tch )
        {
            tank = tch;
            break;
        }
    }
    if ( !tank )
    {
        return;
    }
    /**
     * dla PC minimum 15 - nawet, je�eli nie zna skilla
     * dla NPC maximum 15
     */
    skill = IS_NPC( ch ) ? UMIN( 15, get_skill( ch, gsn_rescue ) ) : UMAX( 15, get_skill( ch, gsn_rescue ) );

    if ( number_percent( ) < skill )
    {
        act( "{5Odpychasz $C na bok, wy��czaj�c $M z walki!{x", ch, NULL, tank, TO_CHAR );
        act( "{5$n odpycha ci� na bok, wy��czaj�c ci� z walki!{x", ch, NULL, tank, TO_VICT );
        act( "{5$n odpycha $C na bok, wy��czaj�c $M z walki!{x", ch, NULL, tank, TO_NOTVICT );
        check_improve( ch, NULL, gsn_rescue, TRUE, 18 );

        stop_fighting( tank, FALSE );
        stop_fighting( victim, FALSE );
        check_killer( ch, victim );
        set_fighting( ch, victim );
        set_fighting( victim, ch );
    }
    else
    {
        act( "{5Probujesz wepcha� si� przed $C, ale ci si� nie udaje!{x", ch, NULL, tank, TO_CHAR );
        switch ( ch->sex )
        {
            case SEX_NEUTRAL:
            case SEX_MALE:
            default:
                act( "{5$n pr�buje wepcha� si� przed ciebie, ale mu si� to nie udaje!{x", ch, NULL, tank, TO_VICT );
                act( "{5$n pr�buje wepcha� si� przed $C, ale mu si� to nie udaje!{x", ch, NULL, tank, TO_NOTVICT );
                break;
            case SEX_FEMALE:
                act( "{5$n pr�buje wepcha� si� przed ciebie, ale jej si� to nie udaje!{x", ch, NULL, tank, TO_VICT );
                act( "{5$n pr�buje wepchac si� przed $C, ale jej si� to nie udaje!{x", ch, NULL, tank, TO_NOTVICT );
                break;
        }
        check_improve( ch, NULL, gsn_rescue, FALSE, 18 );
    }
    return;
}

bool check_dodge( CHAR_DATA *ch, CHAR_DATA *victim, int ac, int th_roll, int thac0, bool critical )
{
	int skill, dex_mod = 0, size_mod;

	//nie rusza sie, nie widzi albo krytyk - nie ma dodgowania
	if ( !IS_AWAKE( victim ) ||
			!can_move( victim ) ||
			!can_see( victim, ch ) ||
			critical )
		return FALSE;

	if ( ( IS_NPC( victim ) && EXT_IS_SET( victim->act, ACT_MONK ) ) ||
			( !IS_NPC( victim ) && victim->class == CLASS_MONK ) )
	{
		int bonus_ac = 0;

		if ( ( skill = get_skill( victim, gsn_monk_dodge ) ) <= 0 )
			return FALSE;

		if ( number_percent() > URANGE( 10, skill, 95 ) )
		{
			check_improve( victim, ch, gsn_monk_dodge, FALSE, 60 );
			return FALSE;
		}

		bonus_ac = URANGE( 1, 1 + ch->level / 5, 6 );

		if ( th_roll < ( thac0 - ( ac - bonus_ac ) ) )
		{
			act( "{5Robisz b�yskawiczny unik i cios ci� nie trafia.{x", ch, NULL, victim, TO_VICT );
			act( "{5$N robi b�yskawiczny unik i tw�j atak nie trafia.{x", ch, NULL, victim, TO_CHAR );
			act( "{5$N robi b�yskawiczny unik i cios $z nie trafia.{x", ch, NULL, victim, TO_NOTVICT );
			check_improve( victim, ch, gsn_monk_dodge, TRUE, 60 );
			return TRUE;
		}

		check_improve( victim, ch, gsn_monk_dodge, FALSE, 70 );
		return FALSE;
	}


	if ( !IS_SET( victim->fight_data, FIGHT_DODGE ) )
		return FALSE;

	skill = get_skill( victim, gsn_dodge );

	switch ( get_curr_stat_deprecated( victim, STAT_DEX ) )
	{
		case 19: dex_mod = 2;	break;
		case 20: dex_mod = 3;	break;
		case 21: dex_mod = 4;	break;
		case 22: dex_mod = 5;	break;
		case 23: dex_mod = 6;	break;
		case 24: dex_mod = 7;	break;
		case 25: dex_mod = 7;	break;
		case 26: dex_mod = 8;	break;
		case 27: dex_mod = 8;	break;
		case 28: dex_mod = 9;	break;
		default: break;
	}

	switch ( victim->size )
	{
		case SIZE_TINY: size_mod = 130;	break;
		case SIZE_SMALL: size_mod = 100;	break;
		case SIZE_MEDIUM: size_mod = 85;	break;
		case SIZE_LARGE: size_mod = 65;	break;
		case SIZE_HUGE: size_mod = 60;	break;
		case SIZE_GIANT: size_mod = 50;	break;
		default: size_mod = 100;	break;
	}

	dex_mod = UMAX( 1, size_mod * dex_mod / 100 );

	dex_mod += skill / 16;

#ifdef INFO
	print_char( victim, "Dex_mod = %d\n\r", dex_mod );
	print_char( victim, "THAC0: %d\n\r", thac0 );
	print_char( victim, "AC: %d\n\r", ac );
	print_char( victim, "DICEROLL: %d\n\r", th_roll );
	print_char( victim, "THAC0-ac-dex_mod: %d\n\r", thac0 - ( ac - dex_mod ) );
#endif


	if ( th_roll < ( thac0 - ( ac - dex_mod ) ) )
	{
		act( "{5Robisz b�yskawiczny unik i cios $z ci� nie trafia.{x", ch, NULL, victim, TO_VICT );
		act( "{5$N robi b�yskawiczny unik i tw�j atak nie trafia.{x", ch, NULL, victim, TO_CHAR );
		act( "{5$N robi b�yskawiczny unik i cios $z nie trafia.{x", ch, NULL, victim, TO_NOTVICT );

		if( ch->size <= SIZE_LARGE && ch->size >= SIZE_SMALL && ch->position > POS_SITTING )
        {
                check_trick( victim, ch, SN_TRICK_SHOVE );
        }

		return TRUE;
	}

	return FALSE;
}

//naliczanie zgonow/zabojstw dla postaci i klanu
void update_death_statistic( CHAR_DATA *victim, CHAR_DATA *ch )
{
	if ( IS_NPC( victim ) && IS_NPC( ch ) )
		return;

	if ( IS_NPC( victim ) && !IS_NPC( ch ) )
	{
		ch->pcdata->death_statistics.mob_kills++;

		if ( ch->pcdata->clan )
			ch->pcdata->clan->mkills++;

		return;
	}
	else if ( !IS_NPC( victim ) )
	{
		if ( victim == ch )
		{
			victim->pcdata->death_statistics.suicides++;
			victim->pcdata->death_statistics.deaths++;
			return;
		}
		else if ( !IS_NPC( ch ) || ( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) && ch->master && !IS_NPC( ch->master ) ) )
		{
			MURDER_LIST * tmp_death, *tmp_kill;
			char buf[ MAX_STRING_LENGTH ];
			bool charm = FALSE;
			CHAR_DATA * killer = ch;

			tmp_death = malloc( sizeof( *tmp_death ) );

			if ( IS_NPC( ch ) )
			{
				charm = TRUE;
				killer = ch->master;
			}

			if ( charm )
			{
				sprintf( buf, "%s [%5d] (%d) - charm gracza %s", ch->short_descr, ch->pIndexData->vnum, ch->level, killer->name );
				tmp_death->name = str_dup( buf );
				tmp_death->char_level = killer->level;
				tmp_death->victim_level = victim->level;
			}
			else
			{
				tmp_death->name = str_dup( killer->name4 );
				tmp_death->char_level = killer->level;
				tmp_death->victim_level = victim->level;
			}

			tmp_death->room = victim->in_room->vnum;
			tmp_death->time = current_time;
			tmp_death->next = victim->pcdata->death_statistics.pkdeath_list;
			victim->pcdata->death_statistics.pkdeath_list = tmp_death;

			tmp_kill = malloc( sizeof( *tmp_kill ) );
			if ( charm )
			{
				sprintf( buf, "gracza %s (%d) z wykorzystaniem charma %s [%5d]", victim->name, victim->level, ch->short_descr, ch->pIndexData->vnum );
				tmp_kill->name = str_dup( buf );
				tmp_kill->char_level = killer->level;
				tmp_kill->victim_level = ch->level;
			}
			else
			{
				tmp_kill->name = str_dup( victim->name4 );
				tmp_kill->char_level = killer->level;
				tmp_kill->victim_level = victim->level;
			}

			tmp_kill->time = tmp_death->time;
			tmp_kill->room = tmp_death->room;
			tmp_kill->next = killer->pcdata->death_statistics.pkills_list;
			killer->pcdata->death_statistics.pkills_list = tmp_kill;


			victim->pcdata->death_statistics.pkdeaths++;
			victim->pcdata->death_statistics.deaths++;
			killer->pcdata->death_statistics.player_kills++;

			if ( victim->level > 5 && killer->pcdata->clan )
				killer->pcdata->clan->pkills++;

			if ( victim->pcdata->clan )
				victim->pcdata->clan->pdeaths++;
			return;
		}
		else if ( IS_NPC( ch ) )
		{
			MURDER_LIST * tmp_death;
			char buf[ MAX_STRING_LENGTH ];
			tmp_death = malloc( sizeof( *tmp_death ) );
			sprintf( buf, "%s [%5d]", ch->short_descr, ch->pIndexData->vnum );
			tmp_death->name = str_dup( buf );
			tmp_death->char_level = ch->level;
			tmp_death->victim_level = victim->level;
			tmp_death->room = victim->in_room->vnum;
			tmp_death->time = current_time;
			tmp_death->next = victim->pcdata->death_statistics.pkdeath_list;
			victim->pcdata->death_statistics.pkdeath_list = tmp_death;

			victim->pcdata->death_statistics.deaths++;

			if ( victim->pcdata->clan )
				victim->pcdata->clan->mdeaths++;

			return;
		}

	}
}

void check_special_unarmed_hit( CHAR_DATA *ch, CHAR_DATA *victim, int base_dam )
{
	int dam;
	AFFECT_DATA bleeding_wound;

	if ( !IS_NPC( ch ) || ch->fighting != victim || base_dam <= 0 )
		return;

	if ( IS_SET( ch->attack_flags, WEAPON_POISON ) )
	{
		poison_to_char( victim, 1 );
	}

	/* wysysanie, nie zadaje obrazen bo juz zadalo wczesniej */
	if ( IS_SET( ch->attack_flags, WEAPON_VAMPIRIC ) && !is_undead( victim ) && !IS_SET( victim->form, FORM_CONSTRUCT ) )
	{
		dam = number_range( base_dam / 4, base_dam / 2 );
		if (dam>0)
		{
			act( "Dotkni�cie $Z wysysa �ycie z $z.", victim, NULL, ch, TO_ROOM );
			act( "Czujesz jak dotkni�cie $Z wysysa z ciebie energi�.", victim, NULL, ch, TO_CHAR );
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

	if ( IS_SET( ch->attack_flags, WEAPON_FLAMING ))
	{
		dam = number_range( base_dam / 4, base_dam / 2 );
		dam = UMAX( dam, 1 );
		act( "$n parzy si� przy zetkni�ciu z $V.", victim, NULL, ch, TO_ROOM );
		act( "Parzysz si� przy zetkni�ciu z $V.", victim, NULL, ch, TO_CHAR );
		fire_effect( victim, 1, dam, TARGET_CHAR );
		damage( ch, victim, dam, 0, DAM_FIRE, FALSE );
	}

	if ( IS_SET( ch->attack_flags, WEAPON_TOXIC ) )
	{
		dam = number_range( base_dam / 4, base_dam / 2 );
		dam = UMAX( dam, 1 );
		act( "Pokrywaj�cy r�ce $Z kwas wy�era sk�r� $z.", victim, NULL, ch, TO_ROOM );
		act( "Pokrywaj�cy r�ce $Z kwas wy�era tw� sk�r�!", victim, NULL, ch, TO_CHAR );
		acid_effect( victim, 1, dam, TARGET_CHAR );
		damage( ch, victim, dam, 0, DAM_ACID, FALSE );
	}

	if ( IS_SET( ch->attack_flags, WEAPON_FROST ) )
	{
		dam = number_range( base_dam / 4, base_dam / 2 );
		dam = UMAX( dam, 1 );
		act( "Dotyk $Z zamra�a $c.", victim, NULL, ch, TO_ROOM );
		act( "Zimny dotyk $Z zamienia ci� w bry�� lodu.", victim, NULL, ch, TO_CHAR );
		cold_effect( victim, 1, dam, TARGET_CHAR );
		damage( ch, victim, dam, 0, DAM_COLD, FALSE );
	}

	if ( IS_SET( ch->attack_flags, WEAPON_SHOCKING ) )
	{
		dam = number_range( base_dam / 4, base_dam / 2 );
		dam = UMAX( dam, 1 );

		if ( victim->sex == 2 )
			act( "B�yskawica z $Z trafia $c wywo�uj�c u niej niekontrolowane drgawki.", victim, NULL, ch, TO_ROOM );
		else
			act( "B�yskawica z $Z trafia $c wywo�uj�c u niego niekontrolowane drgawki.", victim, NULL, ch, TO_ROOM );

		act( "Dotkni�cie $Z pora�a ci� seri� elektrowstrz�s�w.", victim, NULL, ch, TO_CHAR );
		shock_effect( victim, 1, dam, TARGET_CHAR );
		damage( ch, victim, dam, 0, DAM_LIGHTNING, FALSE );
	}

	if ( IS_SET( ch->attack_flags, WEAPON_RESONANT ) )
    {
        dam = number_range( base_dam / 4, base_dam / 2 );
        dam = UMAX( dam, 1 );
        if (IS_SET(victim->parts, PART_HEAD ) )
        {
            act( "Fala ultrad�wi�k�w z d�oni $Z wdziera si� do g�owy $z.", victim, NULL, NULL, TO_ROOM );
            act( "Fala ultrad�wi�k�w z d�oni $Z wdziera si� do twej g�owy wraz z pulsuj�cym b�lem.", victim, NULL, NULL, TO_CHAR );
        }
        else
        {
            act( "Fala ultrad�wi�k�w z d�oni $Z wdziera si� do cia�a $z.", victim, NULL, NULL, TO_ROOM );
            act( "Fala ultrad�wi�k�w z d�oni $Z wdziera si� do twojego cia�a wraz z pulsuj�cym b�lem.", victim, NULL, NULL, TO_CHAR );
        }
		sound_effect( victim, 1, dam, TARGET_CHAR );
		damage( ch, victim, dam, 0, DAM_SOUND, FALSE );
	}

	//nie-undeadom zada polowe tego
	if ( IS_SET( ch->attack_flags, WEAPON_SACRED ) )
	{
		dam = number_range( base_dam / 4, base_dam / 2 );
		dam = UMAX( dam, 1 );
		act( "Niewielki strumie� �wi�tej mocy z d�oni $Z trafia $c.", victim, NULL, NULL, TO_ROOM );
		act( "Niewielki strumie� �wi�tej mocy z d�oni $Z trafia ci� prosto w pier�.", victim, NULL, NULL, TO_CHAR );
		damage( ch, victim, dam, 0, DAM_HOLY, FALSE );
		if( IS_NPC(victim) && victim->pIndexData->vnum == MOB_VNUM_AVATAR )
		{
			act( "{5W kr�tkim rozb�ysku �wiat�a $n zamienia si� w kupk� dymi�cego popio�u.{x", victim, NULL, NULL, TO_ROOM );
			raw_damage( ch, victim, 20 + get_max_hp(victim) );
		}
	}
	
			if ( IS_SET( ch->attack_flags, WEAPON_SACRED ) )
			{
				dam = number_range( base_dam / 4, base_dam / 2 );
				dam = UMAX( dam, 1 );
				act( "Dotyk $Z pora�a $c mroczn�, przekl�t� energi�.", victim, NULL, NULL, TO_ROOM );
				act( "Dotyk $Z pora�a ci� mroczn�, przekl�t� energi�", victim, NULL, NULL, TO_CHAR );
				damage( ch, victim, dam, 0, DAM_NEGATIVE, FALSE );
			}

//	void spell_great_dispel( int sn, int level, CHAR_DATA *ch, void *vo,int target )
//  void spell_dispel_magic( int sn, int level, CHAR_DATA *ch, void *vo, int target )
	if ( IS_SET( ch->attack_flags, WEAPON_DISPEL ) )
	{
		weapon_dispeller(ch, victim, base_dam);
	}

	if ( !is_affected(victim,gsn_bleeding_wound) && !is_undead(victim) && ( ( IS_SET( ch->attack_flags, WEAPON_SHARP ) ) || ( IS_SET( ch->attack_flags, WEAPON_VORPAL ) ) || ( IS_SET( ch->attack_flags, WEAPON_INJURIOUS ) ) ) && ( IS_SET( victim->form, FORM_WARM ) || IS_SET( victim->form, FORM_COLD_BLOOD ) ) )
	{
		//chance na zranienie
		if( IS_SET( ch->attack_flags, WEAPON_SHARP ) && !stat_throw( victim, STAT_CON ) && number_percent() <= 15 )
		{
			act( "Cios zadany przez $C pozostawia na ciele $z krwawi�c� ran�!", victim, NULL, ch, TO_ROOM );
			act( "Cios zadany przez $C pozostawia na twoim ciele krwawi�c� ran�!", victim, NULL, ch, TO_CHAR );
			bleeding_wound.where = TO_AFFECTS;
			bleeding_wound.type = gsn_bleeding_wound;
			bleeding_wound.level = 50;
			bleeding_wound.duration = number_range(1,3);
			bleeding_wound.rt_duration = 0;
			bleeding_wound.location = APPLY_NONE;
			bleeding_wound.modifier = 0;
			bleeding_wound.bitvector = &AFF_NONE;
			affect_to_char(	victim,	&bleeding_wound, NULL, TRUE );
		}
		else if( IS_SET( ch->attack_flags, WEAPON_VORPAL ) && !stat_throw( victim, STAT_CON ) && number_percent() <= 35 )
		{
			act( "Cios zadany przez $C pozostawia na ciele $z krwawi�c� ran�!", victim, NULL, ch, TO_ROOM );
			act( "Cios zadany przez $C pozostawia na twoim ciele krwawi�c� ran�!", victim, NULL, ch, TO_CHAR );
			bleeding_wound.where = TO_AFFECTS;
			bleeding_wound.type = gsn_bleeding_wound;
			bleeding_wound.level = 50;
			bleeding_wound.duration = number_range(1,5);
         bleeding_wound.rt_duration = 0;
			bleeding_wound.location = APPLY_NONE;
			bleeding_wound.modifier = 0;
			bleeding_wound.bitvector = &AFF_NONE;
			affect_to_char(	victim,	&bleeding_wound, NULL, TRUE );
		}
		else if( IS_SET( ch->attack_flags, WEAPON_INJURIOUS ) && number_percent() <= 25 )
		{
			act( "Cios zadany przez $C pozostawia na ciele $z krwawi�c� ran�!", victim, NULL, ch, TO_ROOM );
			act( "Cios zadany przez $C pozostawia na twoim ciele krwawi�c� ran�!", victim, NULL, ch, TO_CHAR );
			bleeding_wound.where = TO_AFFECTS;
			bleeding_wound.type = gsn_bleeding_wound;
			bleeding_wound.level = 50;
			bleeding_wound.duration = number_range(1,10);
			bleeding_wound.rt_duration = 0;
			bleeding_wound.location = APPLY_NONE;
			bleeding_wound.modifier = 0;
			bleeding_wound.bitvector = &AFF_NONE;
			affect_to_char(	victim,	&bleeding_wound, NULL, TRUE );
		}
	}

	return;
}

/* do_one_hit_monk*/
void one_hit_monk( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
	int victim_ac;
	int thac0;
	int dam, base_dam = 0;
	int diceroll;
	int dam_type = DAM_BASH;
	bool result = TRUE, immune = FALSE, critical = FALSE, prog_mod = FALSE;
	MONK_HITS * monk_hit = generate_hit( ch, victim );

	DEBUG_INFO( "one_hit:checks" );
	if ( victim == ch || ch == NULL || victim == NULL )
		return;

	if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
		return;

	DEBUG_INFO( "onehit:get_AC" );
	victim_ac = GET_AC( victim, AC_BASH ) / 10;

	DEBUG_INFO( "onehit:ac_modifiers" );
	if ( !can_see( ch, victim ) )
		victim_ac -= 4;

	if ( ch->alignment < -750 && IS_AFFECTED( victim, AFF_PROTECT_EVIL ) )
		victim_ac -= 2;

	if ( ch->alignment > 750 && IS_AFFECTED( victim, AFF_PROTECT_GOOD ) )
		victim_ac -= 2;

	if ( victim->position < POS_FIGHTING )
		victim_ac += 3;

	if ( victim->position < POS_RESTING )
		victim_ac += 6;

	if ( IS_SET( victim->fight_data, FIGHT_CASTING ) )
		victim_ac += 3;

	if ( victim->daze > 0 )
		victim_ac += 4;

	DEBUG_INFO( "onehit:compute_thac0" );
	thac0 = compute_tohit( ch, FALSE );
	thac0 -= monk_hit->hit_bonus;

	DEBUG_INFO( "onehit:trig_onehit" );
	to_hit_bonus = 0;
	to_dam_bonus = 0;
	fight_text_ch[ 0 ] = '\0';
	fight_text_victim[ 0 ] = '\0';
	fight_text_room[ 0 ] = '\0';

	/*onehit trigger for mobiles*/
	if ( IS_NPC( ch ) && HAS_TRIGGER( ch, TRIG_ONEHIT ) )
		prog_mod = mp_onehit_trigger( ch, victim );

	if ( !victim->in_room )
		return;

	if ( prog_mod && to_hit_bonus != 0 )
		thac0 -= to_hit_bonus;


	check_defensive_spells( ch, victim );

	if ( ch->position < POS_RESTING || !ch->in_room )
		return;

	while ( ( diceroll = number_bits( 5 ) ) >= 20 );

	/* standardowe rzeczy przeklejone z damage(...)*/

	DEBUG_INFO( "onehit:standard_checks" );
	if ( victim->position == POS_DEAD )
		return;

	if ( victim != ch )
	{
		DEBUG_INFO( "onehit:is_safe" );
		if ( is_safe( ch, victim ) )
			result = FALSE;

		DEBUG_INFO( "onehit:trig_kill" );
		if ( victim->position > POS_STUNNED )
		{
			if ( victim->fighting == NULL )
			{
				set_fighting( victim, ch );

				if ( IS_NPC( victim ) && HAS_TRIGGER( victim, TRIG_KILL ) )
					mp_percent_trigger( victim, ch, NULL, NULL, &TRIG_KILL );
			}
		}

		DEBUG_INFO( "onehit:set_fighting" );
		if ( ch->position > POS_STUNNED )
		{
			if ( ch->fighting == NULL )
				set_fighting( ch, victim );
		}

		DEBUG_INFO( "onehit:charm_stop_follower" );
		if ( victim->master == ch && !accident )
		{
			if( IS_NPC( victim ) && IS_AFFECTED(victim,AFF_CHARM) )
			{
				act( "Wi� mi�dzy tob� a $V zostaje brutalnie przerwana!", ch, NULL, victim, TO_CHAR );
				raw_damage( ch, ch, number_range( ch->level/2, ch->level+ch->level/2 ) );
				if ( ch->position <= POS_STUNNED )
					return;
			}
			stop_follower( victim );
		}

		DEBUG_INFO( "onehit:strip_hide/invis" );
		if ( IS_AFFECTED( ch, AFF_HIDE ) )
			affect_strip( ch, gsn_hide );

		strip_invis( ch, TRUE, TRUE );

		DEBUG_INFO( "onehit:start_hating" );
		if ( IS_NPC( victim ) && !is_fearing( victim, ch ) && !accident && !is_hating( victim, ch ) )
		{
		DEBUG_INFO( "onehit:start_hating:start_hunting" );
			if ( !EXT_IS_SET( victim->act, ACT_SENTINEL ) )
            {
				start_hunting( victim, ch );
            }
		DEBUG_INFO( "onehit:start_hating" );
			start_hating( victim, ch );
		}
	}
	/* koniec standardowych rzeczy*/


	DEBUG_INFO( "onehit:miss" );
	/*czesc kodu przy nietrafieniu kogostam*/
	if ( ( diceroll == 0 || ( can_move( victim ) && diceroll != 19 && diceroll < thac0 - victim_ac ) ) )
	{
		generate_monk_miss( ch, victim );
		return;
	}
	/* koniec czesci kodu przy nie-trafieniu kogostam */

	DEBUG_INFO( "onehit:mob_hitting_without_weapon" );
	/* mobile wali z reki */
	dam = dice( monk_hit->damage[ 0 ], monk_hit->damage[ 1 ] ) + monk_hit->dam_bonus;
	base_dam = dam;

	/* critical mobilesa*/
	if ( diceroll == 19 )
	{
		dam += dice( monk_hit->damage[ 0 ], monk_hit->damage[ 1 ] ) + monk_hit->dam_bonus;
		critical = TRUE;
	}

	if ( prog_mod && to_dam_bonus != 0 )
		dam += to_dam_bonus;

	/* dodajemy reszte dodatkowych obrazen*/
	dam += GET_DAMROLL( ch, NULL );
	dam += weapon_skill_mod( ch, TRUE, FALSE );


#ifdef INFO
	print_char( ch, "DAM +DAMROLL: %d\n\r", dam );
#endif


	DEBUG_INFO( "onehit:bashed_mod" );
	/* jesli koles nie stoi */
	if ( victim->position < POS_FIGHTING )
	{
		/* nieprzytomy/spiacy obrazenia razy 2, reszta +33%*/
		if ( !IS_AWAKE( victim ) )
			dam *= 2;
		else
			dam += dam / 4;
#ifdef INFO
		print_char( ch, "DAM + POSITION_BONUS: %d\n\r", dam );
#endif

	}

	/* sparowany atak??*/
	if ( dt != gsn_backstab )
	{
		DEBUG_INFO( "onehit:parry" );
		if ( check_parry( ch, victim, dam, NULL ) )
			return;

		DEBUG_INFO( "onehit:dodge" );
		/* po dodge, jest chance ze cios pojdzie w kogos obok */
		if ( check_dodge( ch, victim, victim_ac , diceroll, thac0, critical ) )
		{
			CHAR_DATA * vch = NULL;

			if ( number_percent() < 15 )
			{
				for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
					if ( vch != ch && vch->fighting == victim )
						break;

				if ( vch )
				{
					accident = TRUE;
					one_hit_monk( ch, vch, dt );
					accident = FALSE;
					return;
				}
			}

			return;
		}
	}
	/* jesli trafilismy to obrazenia minimum 1*/
	if ( dam <= 0 )
		dam = 1;

	if ( dam > 50 && !IS_NPC( ch ) && !IS_IMMORTAL( ch ) && dt == TYPE_UNDEFINED )
	{
		bugf( "Koles %s cos podejrzanie mocno bije z reki!", ch->name );
		dam = 50;
	}

	DEBUG_INFO( "onehit:mirror_image" );
	if ( check_mirror_image( victim, ch ) )
	{
		print_char( ch, "Tw�j cios trafia, %s znika.\n\r", PERS( victim, ch ) );
		print_char( victim, "%s trafia jedno z twoich lustrzanych odbi�.\n\r", PERS2( ch, victim ) );
		act( "$n trafia jedno z lustrzanych odbi� $Z.", ch, NULL, victim, TO_NOTVICT );
		return;
	}

	DEBUG_INFO( "onehit:blink" );
	if ( check_blink( victim, ch, TRUE ) )
	{
		act( "Kiedy tw�j cios ju� ma trafi� w $C, $S sylwetka chwilowo znika.", ch, NULL, victim, TO_CHAR );
		act( "Kiedy cios $z ma ju� w ciebie trafi� znikasz na chwilk� z tego planu egzystencji.\n\r", ch, NULL, victim, TO_VICT );
		act( "Kiedy cios $z ma ju� trafi� w $C, $S sylwetka chwilowo znika.", ch, NULL, victim, TO_NOTVICT );
		return;
	}

	DEBUG_INFO( "onehit:stone_skin" );
	if ( check_stone_skin( ch, victim, dam_type, &dam, NULL ) )
	{
		if ( dam == 0 )
		{
			print_char( ch, "Tw�j cios jest nieskuteczny, jakby� uderzy�%s w kamie�.\n\r", ch->sex == 2 ? "a" : ch->sex == 0 ? "o" : "" );
			print_char( victim, "%s trafia ci�, jednak %s cios jest nieskuteczny.\n\r", PERS( ch, victim ), ch->sex == 2 ? "jej" : "jego", ch->sex == 2 ? "uderzy�a" : "uderzy�" );
			act( "$n trafia $C, jednak $s cios jest nieskuteczny.", ch, NULL, victim, TO_NOTVICT );
			return;
		}
		else
		{
			print_char( ch, "Tw�j cios przebija si� przez kamienn� sk�r� pokrywajac� %s.\n\r", PERS4( victim, ch ) );
			print_char( victim, "Cios %s przebija si� przez twoja kamienn� sk�r�.\n\r", PERS2( ch, victim ) );
			act( "Cios $z trafia $C przebijajac si� przez kammiena sk�r�.", ch, NULL, victim, TO_NOTVICT );
		}
	}

	DEBUG_INFO( "onehit:magic_weapon" );
	if ( victim->resists[ RESIST_MAGIC_WEAPON ] > 0 )
	{
		//do uzupelnienia
	}

	if ( immune || dam == 0 )
	{
		print_char( ch, "Tw�j atak jest nieskuteczny, cios nic %s nie robi.\n\r", PERS3( victim, ch ) );
		print_char( victim, "Atak %s jest nieskuteczny, %s cios niczego ci nie robi.\n\r", PERS2( ch, victim ), ch->sex == 2 ? "jej" : "jego" );
		act( "$n trafia, jednak $s cios jest nieskuteczny.", ch, NULL, victim, TO_NOTVICT );
		return;
	}

	DEBUG_INFO( "onehit:check_resist" );
	dam = check_resist( victim, dam_type, dam );

#ifdef INFO
	print_char( ch, "DAM -RESISTS: %d\n\r", dam );
#endif

	if ( immune || dam == 0 )
	{
		print_char( ch, "Tw�j atak jest nieskuteczny, cios nic %s nie robi.\n\r", PERS3( victim, ch ) );
		print_char( victim, "Atak %s jest nieskuteczny, %s cios niczego ci nie robi.\n\r", PERS2( ch, victim ), ch->sex == 2 ? "jej" : "jego" );
		act( "Atak $z nie robi wrazenia na $B, $s cios jest nieskuteczny.", ch, NULL, victim, TO_NOTVICT );

		return;
	}

	DEBUG_INFO( "onehit:increase_wounds" );
	/* spell zwiekszajacy obrazenia slash/pierce/bash */
	check_increase_wounds( victim, dam_type, &dam );

#ifdef INFO
	print_char( ch, "DAM +INCREASE_WOUNDS: %d\n\r", dam );
#endif

	DEBUG_INFO( "onehit:troll_power" );
	if ( IS_AFFECTED( victim, AFF_TROLL ) && victim->hit > 0 && victim->hit - dam < 0 )
	{
		dam = victim->hit + 1;
		create_event( EVENT_TROLL_POWER_REGEN, number_range( 3, 5 ) * PULSE_VIOLENCE, victim, NULL, 0 );
	}

	if ( is_affected(victim,gsn_demon_aura))
	{
        int dam_mod = 14;
		if( IS_GOOD(ch))
        {
			dam -= get_skill(victim,gsn_demon_aura)/dam_mod;
        }
		else if ( !IS_EVIL(ch))
        {
			dam -= get_skill(victim,gsn_demon_aura)/22;
        }
	}

	DEBUG_INFO( "onehit:damage_reduction" );
	if ( is_affected( victim, gsn_damage_reduction ) && dam > 0 )
	{
		AFFECT_DATA * pAff;

		if ( ( pAff = affect_find( victim->affected, gsn_damage_reduction ) ) != NULL )
		{
			if ( pAff->level <= 16 )
				dam -= 1;
			else if ( pAff->level <= 20 )
				dam -= 2;
			else if ( pAff->level <= 25 )
				dam -= 3;
			else if ( pAff->level <= 30 )
				dam -= 4;
			else
				dam -= 5;

			dam = UMAX( dam, 1 );
		}
#ifdef INFO
		print_char( ch, "DAM - DAMAGE_REDUCTION: %d\n\r", dam );
#endif

	}

#ifdef INFO
	print_char( ch, "FINAL DAMAGE: %d\n\r", dam );
#endif

	DEBUG_INFO( "onehit:dam_mesg" );
	/* walimy opis ciosu*/
	generate_monk_dam_message( ch, victim, monk_hit );

	DEBUG_INFO( "onehit:raw_damage" );
	raw_damage( ch, victim, dam );

	/* koniec ciekawostek dla umierajacych*/
	if ( ch->fighting != victim || !victim->in_room )
		return;

	DEBUG_INFO( "onehit:check_armor" );
	/* dla tych co przezyli, sprawdzamy armora */
	check_armor_spell( victim, dam );

	if ( result && IS_NPC( ch ) && ch->attack_flags != 0 )
		check_special_unarmed_hit( ch, victim, base_dam );

	if ( IS_NPC( ch ) && HAS_TRIGGER( ch, TRIG_HIT ) )
		mp_hit_trigger( ch, victim );

	tail_chain( );
	return;
}

void strip_fight_affects( CHAR_DATA * ch )
{
	DEBUG_INFO( "strip_fight_affects(1)" );
	if ( is_affected( ch, gsn_berserk ) )
	{
        bool check = FALSE;
        //Rysand: Analiza kodu: z featem berserk nie schodzi?!
		if ( ch->hit > -1 && !check )
        {
			affect_strip( ch, gsn_berserk );
        }
	}

	DEBUG_INFO( "strip_fight_affects(2)" );
	if ( is_affected( ch, gsn_wardance ) )
		affect_strip( ch, gsn_wardance );

	DEBUG_INFO( "strip_fight_affects(3)" );
	if ( is_affected( ch, gsn_damage_reduction ) )
		affect_strip( ch, gsn_damage_reduction );

	DEBUG_INFO( "strip_fight_affects(4)" );
	if ( is_affected( ch, gsn_chant ) )
		affect_strip( ch, gsn_chant );

	DEBUG_INFO( "strip_fight_affects(5)" );
	if ( is_affected( ch, gsn_prayer ) )
		affect_strip( ch, gsn_prayer );

	DEBUG_INFO( "strip_fight_affects(6)" );
	if ( is_affected( ch, gsn_flurry_of_blows ) )
		affect_strip( ch, gsn_flurry_of_blows );

	if ( is_affected( ch, gsn_weapon_wrench ) )
		affect_strip( ch, gsn_weapon_wrench );

	if ( is_affected( ch, gsn_glorious_impale ) )
		affect_strip( ch, gsn_glorious_impale );

	REMOVE_BIT( ch->fight_data, FIGHT_THOUSANDSLAYER );

	ch->fight_data = 0;
	ch->num_attacks = 0;
}

//fireshield, iceshield, blade barrier, confusion shell
void check_defensive_spells( CHAR_DATA * ch, CHAR_DATA * victim )
{
//Affecty do shield of nature
AFFECT_DATA af;
AFFECT_DATA bleeding_wound;

	if ( !ch || !victim || !ch->in_room || !victim->in_room )
		return;

	DEBUG_INFO( "check_def_spells:fireshield" );
	/* ognista tarcza: 2d6 obrazen ogniowych */
	if ( IS_AFFECTED( victim, AFF_FIRESHIELD ) )
	{
		if ( IS_NPC( ch ) && ( ch->pIndexData->vnum == 3 || IS_AFFECTED(ch, AFF_ILLUSION ) ) )
			;
		else
		{
			print_char( ch, "{4J�zor p�omienia z ognistej tarczy otaczaj�cej %s {4pali ci�!\n\r{x", PERS4( victim, ch ) );
			print_char( victim, "{2J�zor p�omienia z twojej ognistej tarczy pali %s{2!\n\r{x", PERS4( ch, victim ) );
			act( "{3J�zor p�omienia z ognistej tarczy $z {3pali $C{3!{x", victim, NULL, ch, TO_NOTVICT );
			damage( victim, ch, dice( 2, 6 ), gsn_fireshield, DAM_FIRE, FALSE );
		}

		if ( ch->position < POS_RESTING || !ch->in_room )
			return;
	}

	DEBUG_INFO( "check_def_spells:iceshield" );
	/* lodowa tarcza: 2d6 obrazen od zimna */
	if ( IS_AFFECTED( victim, AFF_ICESHIELD ) && !IS_AFFECTED( victim, AFF_FIRESHIELD ) )
	{
		if ( IS_NPC( ch ) && ( ch->pIndexData->vnum == 3 || IS_AFFECTED(ch, AFF_ILLUSION ) ) )
			;
		else
		{
			print_char( ch, "{4Lodowa tarcza otaczaj�ca %s {4mrozi ci�!{x\n\r", PERS4( victim, ch ) );
			print_char( victim, "{2Twoja lodowa tarcza mrozi %s{2!{x\n\r", PERS4( ch, victim ) );
			act( "{3Lodowa tarcza $z {3mrozi $C{3!{x", victim, NULL, ch, TO_NOTVICT );

			damage( victim, ch, dice( 2, 6 ), gsn_iceshield, DAM_COLD, FALSE );
		}

		if ( ch->position < POS_RESTING || !ch->in_room )
			return;
	}

	/* bariera ostrzy: 2d8 obrazen od 'slash/pierce' + (level-10)/5 */
	if ( IS_AFFECTED( victim, AFF_BLADE_BARRIER ) )
	{
		AFFECT_DATA * pAff;
		int level = victim->level;

		if ( ( pAff = affect_find( victim->affected, gsn_blade_barrier ) ) != NULL )
			level = pAff->level;

		if ( IS_NPC( ch ) && ch->pIndexData->vnum == 3 )
			;
		else
		{
			print_char( ch, "{4Bariera wiruj�cych ostrzy %s {4dotkliwie ci� rani!{x\n\r", PERS4( victim, ch ) );
			print_char( victim, "{2Twoja bariera wiruj�cych ostrzy dotkliwie rani %s{2!{x\n\r", PERS4( ch, victim ) );
			act( "{3Bariera wiruj�cych ostrzy $z {3dotkliwie rani $C{3!{x", victim, NULL, ch, TO_NOTVICT );

			damage( victim, ch, dice( 2, 8 ) + (level-10) / 5, gsn_blade_barrier, number_percent() < 50 ? DAM_PIERCE : DAM_SLASH, FALSE );
		}

		if ( ch->position < POS_RESTING || !ch->in_room )
			return;
	}


	/* bariera element�w: 2d8 obrazen od 'magic' + (level-10)/5 */
	DEBUG_INFO( "check_def_spells:shield_of_nature" );
	if ( IS_AFFECTED( victim, AFF_SHIELD_OF_NATURE ) )
	{
		AFFECT_DATA * pAff;
		int level = victim->level;

		if ( ( pAff = affect_find( victim->affected, gsn_shield_of_nature ) ) != NULL )
		{
			level = pAff->level;
		}

		if (!( IS_NPC( ch ) && ch->pIndexData->vnum == 3 ))
		{
			int chance;
			switch( number_range( 1, 4 ) )
			{
				case 1:
					
					print_char( ch, "{4Od�amki skalne z tarczy �ywio��w %s {4dotkliwie ci� rani�!{x\n\r", PERS4( victim, ch ) );
					print_char( victim, "{2Od�amki skalne z otaczaj�cej ci� tarczy �ywio��w dotkliwie rani� %s{2!{x\n\r", PERS4( ch, victim ) );
					act( "{3Od�amki skalne z tarczy �ywio��w $z {3dotkliwie rani� $C{3!{x", victim, NULL, ch, TO_NOTVICT );
					damage( victim, ch, dice( 2, 8 ) + (level-10) / 5, gsn_shield_of_nature, number_percent() < 50 ? DAM_PIERCE : DAM_SLASH, FALSE );
					chance = number_range( 1, 100 );
					if ( chance == 1 || chance == 2 || chance == 3 || chance == 4 || chance == 5 ) //czyli na 5% szansy
						{
							bleeding_wound.where = TO_AFFECTS;
							bleeding_wound.type = gsn_bleeding_wound;
							bleeding_wound.level = 50;
							bleeding_wound.duration = number_range(2,4);
							bleeding_wound.rt_duration = 0;
							bleeding_wound.location = APPLY_NONE;
							bleeding_wound.modifier = 0;
							bleeding_wound.bitvector = &AFF_NONE;
							affect_to_char(	ch,	&bleeding_wound, NULL, TRUE ); 
						}
					break;
				case 2:
					print_char( ch, "{4P�omienie z tarczy �ywio��w %s {4pal� ci�!{x\n\r", PERS4( victim, ch ) );
					print_char( victim, "{2P�omienie z otaczaj�cej ci� tarczy �ywio��w pal� %s{x\n\r", PERS4( ch, victim ) );
					act( "{3P�omienie z tarczy �ywio��w $z {3pal� $C{3!{x", victim, NULL, ch, TO_NOTVICT );
					damage( victim, ch, dice( 3, 8 ) + (level-10) / 5, gsn_shield_of_nature, DAM_FIRE, FALSE );
					break;
				case 3:
					
					print_char( ch, "{4Ch��d z tarczy �ywio��w %s {4mrozi ci�!{x\n\r", PERS4( victim, ch ) );
					print_char( victim, "{2Ch��d z otaczaj�cej ci� tarczy �ywio��w mrozi %s{2!{x\n\r", PERS4( ch, victim ) );
					act( "{3Ch��d z tarczy �ywio��w $z {3mrozi $C{3!{x", victim, NULL, ch, TO_NOTVICT );
					damage( victim, ch, dice( 2, 8 ) + (level-10) / 5, gsn_shield_of_nature, DAM_COLD, FALSE );
					chance = number_range( 1, 100 );
					if ( chance == 1 || chance == 2 ) //czyli na 2% szansy
					{
						af.where = TO_AFFECTS;
						af.type = 90;
						af.level = level;
						af.duration = duration_modifier_by_spell_type( dice( 2, 2 ), SECT_COLD, ch );
						af.rt_duration = 0;
						af.location = APPLY_NONE;
						af.modifier = 0;
						af.bitvector = &AFF_SLOW;

						affect_to_char( ch, &af, NULL, TRUE );

						send_to_char( "�wiat wok� ciebie znacznie przy�piesza...\n\r", ch );
						act( "$n zaczyna porusza� si� znacznie wolniej.", ch, NULL, NULL, TO_ROOM );
					}
					break;
				case 4:
					print_char( ch, "{4Wy�adowania z tarczy �ywio��w %s {4pora�aj� twe mi�nie!{x\n\r", PERS4( victim, ch ) );
					print_char( victim, "{2Wy�adowania z otaczaj�cej ci� tarczy �ywio��w pora�aj� %s{2!{x\n\r", PERS4( ch, victim ) );
					act( "{3Wy�adowania z tarczy �ywio��w $z {3pora�aj� $C{3!{x", victim, NULL, ch, TO_NOTVICT );
					damage( victim, ch, dice( 2, 8 ) + (level-10) / 5, gsn_shield_of_nature, DAM_LIGHTNING, FALSE );
					chance = number_range( 1, 100 );
					if ( chance == 1 || chance == 2 ) //czyli na 2% szansy
					{
						act( "{5Pod wp�ywem wy�adowa� elektrycznych $z opada na ziemi�.{x", ch, NULL, NULL, TO_ROOM );
						send_to_char( "{5Pod wp�ywem wy�adowa� elektrycznych opadasz na ziemi�.{x\n\r", ch );
						ch->position = POS_SITTING;
						WAIT_STATE( ch, 3 * PULSE_VIOLENCE );
					}
					break;
				default:
					print_char( ch, "{4Wy�adowania z tarczy �ywio��w %s {4pora�aj� twe mi�nie!{x\n\r", PERS4( victim, ch ) );
					print_char( victim, "{2Wy�adowania z otaczaj�cej ci� tarczy �ywio��w pora�aj� %s{2!{x\n\r", PERS4( ch, victim ) );
					act( "{3Wy�adowania z tarczy �ywio��w $z {3pora�aj� $C{3!{x", victim, NULL, ch, TO_NOTVICT );
					damage( victim, ch, dice( 2, 8 ) + (level-10) / 5, gsn_shield_of_nature, DAM_LIGHTNING, FALSE );
					chance = number_range( 1, 100 );
					if ( chance == 1 || chance == 2 ) //czyli na 2% szansy
					{
						act( "{5Pod wp�ywem wy�adowa� elektrycznych $z opada na ziemi�.{x", ch, NULL, NULL, TO_ROOM );
						send_to_char( "{5Pod wp�ywem wy�adowa� elektrycznych opadasz na ziemi�.{x\n\r", ch );
						ch->position = POS_SITTING;
						WAIT_STATE( ch, 3 * PULSE_VIOLENCE );
					}
					break;
			}
		}

		if ( ch->position < POS_RESTING || !ch->in_room )
		{
			return;
		}
	}

	DEBUG_INFO( "check_def_spells:demon_aura" );
	if ( is_affected( victim, gsn_demon_aura ) )
	{
		AFFECT_DATA * aura = affect_find( victim->affected, gsn_demon_aura );

		if ( ( IS_NPC( ch ) && ch->pIndexData->vnum == 3 ) || !IS_GOOD(ch) || aura->level < 50 || IS_AFFECTED(ch, AFF_ILLUSION ) )
			;
		else
		{
			int dam;
			if ( IS_AFFECTED(ch, AFF_PROTECT_EVIL ) )
			{
				dam = dice( 1, aura->level/20 );
				if (dam>0)
				{
					act( "Aura mroku otaczaj�ca $C sprawia, �e zaczynasz czu� si� �le, ale dzi�ki ochronnemu zakl�ciu jej wp�yw zostaje lekko zniwelowany!", ch, NULL, victim, TO_CHAR );
					act( "Otaczaj�ca ci� aura mroku wysysa niewiele energii z $z, bowiem jej wp�yw zosta� zniwelowany przez jakie� zakl�cie!", ch, NULL, victim, TO_VICT );
					act( "Aura mroku otaczaj�ca $C pr�buje wysysa� �ycie z $z, jednak co� sprawia, �e jej wp�yw zostaje zminimalizowany.", ch, NULL, victim, TO_NOTVICT );
					damage( victim, ch, dam, gsn_demon_aura, DAM_NEGATIVE, FALSE );
				}
			}
			else
			{
				dam = dice( 1, aura->level/10 );
				if (dam>0)
				{
					act( "Aura negatywnej energii otaczaj�ca $C wysysa z ciebie �ycie!", ch, NULL, victim, TO_CHAR );
					act( "Otaczaj�ca ci� aura negatywnej energii wysysa �ycie z $z.", ch, NULL, victim, TO_VICT );
					act( "Aura negatywnej energii otaczaj�ca $C wysysa �ycie z $z.", ch, NULL, victim, TO_NOTVICT );
					damage( victim, ch, dam, gsn_demon_aura, DAM_NEGATIVE, FALSE );
				}
			}
		}
		if ( ch->position < POS_RESTING || !ch->in_room )
		{
			return;
		}
	}

	AFFECT_DATA *damned, hold;
	OBJ_DATA *damned_armor, *weapon2;
	DEBUG_INFO( "check_def_spells:damned_armor" );
	if ( IS_GOOD(ch) && victim->class == CLASS_BLACK_KNIGHT && ( damned_armor = get_eq_char(victim,WEAR_BODY)) != NULL )
	{
		if ( ( IS_NPC( ch ) && ch->pIndexData->vnum == 3 ) )
			;
		else if ( get_skill(victim,gsn_damn_armor) > number_range(35,70 ) )
		{
			for ( damned = damned_armor->affected; damned != NULL; damned = damned->next )
				if ( damned->type == gsn_damn_armor )
				{
                    int mod = 0;
					switch ( number_range( 1, 25 + mod ) )
					{
						case 1:
							act( "Z twojej zbroi ulatuje przekl�ta dusza i zawodz�c okr�ca si� wok� $Z.", victim, NULL, ch, TO_CHAR );
							act( "Ze zbroi $z ulatuje przekl�ta dusza i zawodz�c okr�ca si� wok� ciebie!", victim, NULL, ch, TO_VICT );
							act( "Ze zbroi $z ulatuje przekl�ta dusza i zawodz�c okr�ca si� wok� $Z.", victim, NULL, ch, TO_NOTVICT );

							if ( IS_AFFECTED(victim, AFF_PROTECT_EVIL ) && number_percent() > 50 )
							{
								act( "Chwile p�niej dusza ze straszliwym j�kiem znika w rozb�ysku jasnego �wiat�a.", NULL, NULL, NULL, TO_ALL );
								break;
							}

							if ( !IS_AFFECTED( ch, AFF_PARALYZE ) && !IS_AFFECTED( ch, AFF_DAZE ) && !IS_AFFECTED( ch, AFF_FREE_ACTION ) && 2*get_curr_stat_deprecated( ch, STAT_WIS ) < number_range( 1, get_skill(victim,gsn_damn_armor )))
							{
								hold.where     = TO_AFFECTS;
								hold.level     = victim->level;
								hold.location  = APPLY_NONE;
								hold.duration  = damned->level/6 + get_skill(ch,gsn_damn_armor)/24;
								hold.rt_duration = 0;
								hold.modifier	 = 0;

								if( damned->level > 15 )
								{
									send_to_char( "Zosta�<&e�/a�/o�> sparali�owan<&y/a/e>!\n\r", ch );
									act("$n zatrzymuje si� w miejscu.",ch,NULL,NULL,TO_ROOM);
									hold.type      = gsn_holdmonster;
									hold.bitvector = &AFF_PARALYZE;
								}
								else
								{
									switch ( ch->sex )
									{
										case 0:
											act( "$n rozgl�da si� dooko�a, lekko oszo�omione.", ch, NULL, NULL, TO_ROOM );
											send_to_char( "Czujesz si� lekko oszo�omione!\n\r", ch );
											break;
										case 2:
											act( "$n rozgl�da si� dooko�a, lekko oszo�omiona.", ch, NULL, NULL, TO_ROOM );
											send_to_char( "Czujesz si� lekko oszo�omiona!\n\r", ch );
											break;
										default:
											act( "$n rozgl�da si� dooko�a, lekko oszo�omiony.", ch, NULL, NULL, TO_ROOM );
											send_to_char( "Czujesz si� lekko oszo�omiony!\n\r", ch );
											break;
									}
									hold.type      = 304;
									hold.bitvector = &AFF_DAZE;
								}

								affect_to_char( ch, &hold, NULL, TRUE  );
							}
							break;
						case 2:
							act( "Z twojej zbroi ulatuje przekl�ta dusza i zawodz�c okr�ca si� wok� $Z.", victim, NULL, ch, TO_CHAR );
							act( "Ze zbroi $z ulatuje przekl�ta dusza i zawodz�c okr�ca si� wok� ciebie!", victim, NULL, ch, TO_VICT );
							act( "Ze zbroi $z ulatuje przekl�ta dusza i zawodz�c okr�ca si� wok� $Z.", victim, NULL, ch, TO_NOTVICT );

							if ( IS_AFFECTED(victim, AFF_PROTECT_EVIL ) && number_percent() > 50 )
							{
								act( "Chwile p�niej dusza ze straszliwym j�kiem znika w rozb�ysku jasnego �wiat�a.", NULL, NULL, NULL, TO_ALL );
								break;
							}

							if ( ( weapon2 = get_eq_char( ch, WEAR_SECOND ) ) == NULL )
								weapon2 = get_eq_char( ch, WEAR_WIELD );
							if( 2*get_curr_stat_deprecated( ch, STAT_WIS ) < number_range( 1, get_skill(victim,gsn_damn_armor )))
							{
								if ( weapon2 && !IS_OBJ_STAT( weapon2, ITEM_NOREMOVE ))
								{
									act( "Z twoich trz�s�cych si� ze strachu d�oni wypada $h.",ch, weapon2, victim, TO_CHAR);
									act( "Z trz�s�cych si� ze strachu d�oni $z wypada $h.",ch, weapon2, victim, TO_ROOM);
									if ( is_artefact( weapon2 ) && !IS_NPC( ch ) && !IS_IMMORTAL( ch ) )
										artefact_from_char( weapon2, ch );
									obj_from_char( weapon2 );
									if ( IS_OBJ_STAT( weapon2, ITEM_NODROP ) || IS_OBJ_STAT( weapon2, ITEM_INVENTORY ) )
									{
										if ( is_artefact( weapon2 ) && !IS_NPC( ch ) && !IS_IMMORTAL( ch ) )
											artefact_to_char( weapon2, ch );
										obj_to_char( weapon2, ch );
									}
									else
									{
										obj_to_room( weapon2, ch->in_room );
										if ( IS_NPC( ch ) && ch->wait == 0 && can_see_obj( ch, weapon2 ) )
											get_obj( ch, weapon2, NULL );
									}
									break;
								}
							}
							break;
						case 3:
							act( "Z twojej zbroi ulatuje przekl�ta dusza i zawodz�c okr�ca si� wok� $Z.", victim, NULL, ch, TO_CHAR );
							act( "Ze zbroi $z ulatuje przekl�ta dusza i zawodz�c okr�ca si� wok� ciebie!", victim, NULL, ch, TO_VICT );
							act( "Ze zbroi $z ulatuje przekl�ta dusza i zawodz�c okr�ca si� wok� $Z.", victim, NULL, ch, TO_NOTVICT );

							if ( IS_AFFECTED(victim, AFF_PROTECT_EVIL ) && number_percent() > 50 )
							{
								act( "Chwile p�niej dusza ze straszliwym j�kiem znika w rozb�ysku jasnego �wiat�a.", NULL, NULL, NULL, TO_ALL );
								break;
							}

							if (ch->position > POS_SITTING)
							{
								if( 2*get_curr_stat_deprecated( ch, STAT_WIS ) < number_range( 1, get_skill(victim,gsn_damn_armor )))
								{
									act( "Potykasz si� i padasz na ziemi�!",ch, NULL, victim, TO_CHAR);
									act( "$n potyka si� i upada na ziemi�.",ch, NULL, victim, TO_ROOM);
									if ( IS_AFFECTED( ch, AFF_FLYING ) || IS_AFFECTED(ch, AFF_FLOAT))
									{
										affect_strip( ch, gsn_fly );
										affect_strip( ch, gsn_float );
									}
									ch->position = POS_SITTING;
									WAIT_STATE( ch, 2*PULSE_VIOLENCE );
								}
							}
							break;
						default:
							break;
					}
					break;
				}
		}
	}

	DEBUG_INFO( "check_def_spells:confusion_shell" );
	/* iluzje zlych stworkow - victim dostaje jakiestam feary i chance ze spieprzy */
	if ( IS_AFFECTED( victim, AFF_CONFUSION_SHELL ) && !IS_AFFECTED( ch, AFF_BRAVE_CLOAK ) )
	{
		AFFECT_DATA fear;
		OBJ_DATA * weapon;
		if ( IS_NPC( ch ) && ch->pIndexData->vnum == 3 )
			;
		else if (ch->resists[RESIST_FEAR] > 0 && number_percent() < ch->resists[RESIST_FEAR])
			;
		else if ( ch->level - number_range(-1,5) > victim->level )
			;
		else if	( IS_AFFECTED( ch, AFF_PARALYZE ) )
			;
		else if ( !saves_spell_new(ch, skill_table[gsn_confusion_shell].save_type, skill_table[gsn_confusion_shell].save_mod, victim, gsn_confusion_shell ) && get_curr_stat_deprecated(ch, STAT_WIS) < number_range(1,30))
		{
			if ( IS_AFFECTED( ch, AFF_BERSERK ) )
			{
				act( "Sza� bitewny za bardzo opanowa� $z, nie zwraca $e uwagi na otaczaj�ce ci� iluzje.\n\r", ch, NULL, victim, TO_VICT );
				return;
			}
			if( !IS_AFFECTED(ch, AFF_FEAR))
			{
				fear.where = TO_AFFECTS;
				fear.type = 172;//fear
				fear.level = victim->level;
				fear.duration = URANGE(1, victim->level/8, 3);
				fear.rt_duration = 0;
				fear.location = APPLY_NONE;
				fear.modifier = 0;
				fear.bitvector = &AFF_FEAR;
				affect_to_char( ch, &fear, NULL, TRUE  );
			}

			act( "Otaczaj�ce $C monstra rzucaj� si� na ciebie! Aaaaa! Biegasz w k�ko w panice.",ch, NULL, victim, TO_CHAR);
			act( "$n panikuje widz�c otaczaj�ce ci� iluzje potwor�w.",ch, NULL, victim, TO_VICT);
			act( "Otaczaj�ce $C monstra rzucaj� si� na $c, $e krzyczy i biega w k�ko w panice.",ch, NULL, victim, TO_NOTVICT);
			if( ch->fighting && number_range(1,ch->level) < victim->level/3 )
			{
				if(IS_NPC(ch)) start_fearing(ch,victim);
				switch(number_range(1,3))
				{
					case 1:
						interpret(ch,"flee");
						break;
					case 2:
						if ( ( weapon = get_eq_char( ch, WEAR_SECOND ) ) == NULL )
							weapon = get_eq_char( ch, WEAR_WIELD );
						if ( weapon && !IS_OBJ_STAT( weapon, ITEM_NOREMOVE ))
						{
							act( "Z twoich trz�s�cych si� ze strachu d�oni wypada $h.",ch, weapon, victim, TO_CHAR);
							act( "Z trz�s�cych si� ze strachu d�oni $z wypada $h.",ch, weapon, victim, TO_ROOM);
							if ( is_artefact( weapon ) && !IS_NPC( ch ) && !IS_IMMORTAL( ch ) )
								artefact_from_char( weapon, ch );
							obj_from_char( weapon );
							if ( IS_OBJ_STAT( weapon, ITEM_NODROP ) || IS_OBJ_STAT( weapon, ITEM_INVENTORY ) )
							{
								if ( is_artefact( weapon ) && !IS_NPC( ch ) && !IS_IMMORTAL( ch ) )
									artefact_to_char( weapon, ch );
								obj_to_char( weapon, ch );
							}
							else
							{
								obj_to_room( weapon, ch->in_room );
								if ( IS_NPC( ch ) && ch->wait == 0 && can_see_obj( ch, weapon ) )
									get_obj( ch, weapon, NULL );
							}
							break;
						}
					case 3:
						if (ch->position > POS_SITTING)
						{
							act( "Potykasz si� i padasz na ziemi�!",ch, NULL, victim, TO_CHAR);
							act( "$n potyka si� i upada na ziemi�.",ch, NULL, victim, TO_ROOM);
							if ( IS_AFFECTED( ch, AFF_FLYING ) || IS_AFFECTED(ch, AFF_FLOAT))
							{
								affect_strip( ch, gsn_fly );
								affect_strip( ch, gsn_float );
							}
							ch->position = POS_SITTING;
							WAIT_STATE( ch, 2*PULSE_VIOLENCE );
						}
						break;
				}
			}
			DAZE_STATE( ch, number_range(1,3)*PULSE_VIOLENCE );
		}

		if ( ch->position < POS_RESTING || !ch->in_room )
			return;
	}
	return;
}

bool check_shield_block( CHAR_DATA *ch, CHAR_DATA *victim, int ac, int th_roll, int thac0, bool critic, int * dam )
{
	int ac_bonus = 0;
	int dex_skill_mod = 0;		//bonus do skilla
	int dex_block_bonus = 0;	//bonus do blokowania wiekszej ilosci atakow
	int shield_type = -1;
	int max_dam = 0;
	int alternatywny_blok = 0;	//jezeli mimo shield blocka atakujacy zda test na thac0, ale cios bedzie bardzo slaby damowo, to i tak bedzie chance na zablokowanie
	OBJ_DATA *shield = NULL;
	int shield_skill = get_skill( victim, gsn_shield_block );
    /**
     * jak nie umie, to spadamy
     */
    if ( shield_skill < 1 )
    {
        return FALSE;
    }
    /**
     * jak nie ma tarczy to tez sio
     */
    if( ( shield = get_eq_char( victim, WEAR_SHIELD ) ) == NULL )
    {
        return FALSE;
    }
    /**
     * nie blokujemy gdy nie mozemy sie ruszac etc
     */
    if ( !IS_AWAKE( victim ) || !can_move( victim ) || !can_see( victim, ch ) || critic )
    {
        return FALSE;
    }
    if ( get_curr_stat_deprecated( ch, STAT_INT ) < 16 )
    {
        shield_skill /= 2;
    }

    /**
     * Drake: Dodanie shield blocka dla paladyn�w(80%), kleryk�w i druid�w(50%).
     */
    switch ( victim->class )
    {
        case CLASS_PALADIN:
            shield_skill -= get_skill( victim, gsn_shield_block )/5;
            break;
        case CLASS_CLERIC:
        case CLASS_DRUID:

            shield_skill -= get_skill( victim, gsn_shield_block )/2;
            break;
    }
    shield_skill = UMAX(1, shield_skill);

    //jesli to nowy typ obiektu pobieramy typ tarczy z v[0], jesli nie to typ = -1
    if( shield->item_type == ITEM_SHIELD )
    {
        shield_type = shield->value[0];
    }

	switch( get_curr_stat_deprecated( victim, STAT_DEX ) )
	{
		case 16:
		case 17:
			dex_skill_mod = 5;
			dex_block_bonus = 2;
			break;
		case 18:
		case 19:
			dex_skill_mod = 8;
			dex_block_bonus = 3;
			break;
		case 20:
		case 21:
		case 22:
			dex_skill_mod = 10;
			dex_block_bonus = 5;
			break;
		case 23:
		case 24:
			dex_skill_mod = 15;
			dex_block_bonus = 7;
			break;
		case 25:
		case 26:
			dex_skill_mod = 20;
			dex_block_bonus = 10;
			break;
		case 27:
		case 28:
			dex_skill_mod = 25;
			dex_block_bonus = 15;
			break;
		default:break;
	}

	//za rozmiary - bardzo duzy nie zablokuje bardzo malego, maly nie zablokuje malego,
	//najlepiej jak blokuje sie kogos o 1 mniejszego, najgorzej jak kogos duzo wiekszego:p
	//ch to bijacy, victim to blokujacy sie
	if ( ch->size - victim->size == 1 )
	{
		dex_skill_mod -= 4;
		dex_block_bonus -= 3;
	}
	else if ( ch->size - victim->size == 2 )
	{
		dex_skill_mod -= 8;
		dex_block_bonus -= 8;
	}
	else if ( ch->size - victim->size == 3 )
	{
		dex_skill_mod -= 15;
		dex_block_bonus -= 15;
	}
	else if ( ch->size - victim->size >= 4 )
	{
		dex_skill_mod -= 30;
		dex_block_bonus -= 30;
	}

	if ( ch->size - victim->size == -1 )
	{
		dex_skill_mod += 10;
		dex_block_bonus += 3;
	}
	else if ( ch->size - victim->size == -2 )
	{
		dex_skill_mod += 5;
		dex_block_bonus += 6;
	}
	else if ( ch->size - victim->size == -3 )
	{
		dex_skill_mod -= 10;
		dex_block_bonus += 3;
	}
	else if ( ch->size - victim->size <= -4 )
	{
		dex_skill_mod -= 25;
		dex_block_bonus -= 3;
	}


	//nie wyszlo
	if( number_percent() > ( shield_skill + dex_skill_mod ) )
	{
		check_improve( victim, ch, gsn_shield_block, FALSE, 250 );
		return FALSE;
	}

	//test na dodatkowe ataki
	//bonus od tarczy zasadniczo odnosi sie tylko do jednego atakujacego, tego z ktorym walczymy
	//ataki od inny atakujacych maja szanse trafic w tarcze, ale juz nie tak czesto
	if( victim->fighting != ch	&&
			( shield_type >= 0		||
			  number_percent() > shield->value[3] + dex_block_bonus ) )
	{
		check_improve( victim, ch, gsn_shield_block, FALSE, 270 );
		return FALSE;
	}

	//nowy typ tarczy
	if( shield_type >= 0 )
		ac_bonus = URANGE( 1, ( shield->value[1] * ( 50 + shield_skill ))/100, 8 );
	else
		ac_bonus = 1 + shield_skill/30;

	if ( th_roll < ( thac0 - ( ac - ac_bonus ) ) )
	{
		if( shield_type >= 0 )
			max_dam = shield->value[2];
		else
			max_dam = 25;

		check_improve( victim, ch, gsn_shield_block, TRUE, 280);

		max_dam += str_app[get_curr_stat(victim,STAT_STR)].todam;


		if( max_dam < *dam )
		{
			if ( max_dam <= ( *dam * 2 ) && !IS_OBJ_STAT( shield, ITEM_UNDESTRUCTABLE )
					&& number_percent() > material_table[ shield->material ].hardness )
			{
				shield->condition -= UMIN( 5, *dam / UMAX( 1, max_dam ) );
			}

			*dam -= max_dam;

			if ( shield->condition <= 0 )
			{
				act( "{5Pr�bujesz zablokowa� mia�d��cy cios $z, ale $p rozpada si� na dwie cz�ci. Czujesz dotkliwy b�l w przedramieniu.{x", ch, shield, victim, TO_VICT );
				act( "{5$N pr�buje zablokowa� twoj mia�d��cy cios, ale $S $p rozpada si� na dwie cz�ci. Przez twarz $Z przebiega grymas b�lu.{x", ch, shield, victim, TO_CHAR );
				act( "{5$N pr�buje zablokowa� mia�d��cy cios $z, ale $S $p rozpada si� na dwie cz�ci. Przez twarz $Z przebiega grymas b�lu.{x", ch, shield, victim, TO_NOTVICT );

				/*artefact*/
				if ( is_artefact( shield ) ) extract_artefact( shield );

				extract_obj( shield );
				return FALSE;
			}
			else
			{
				switch ( number_range( 1, 3 ) )
				{
					case 1:
						act( "{5Szybkim ruchem unosisz tarcz�, cios $z by� jednak zbyt pot�ny.{x", ch, shield, victim, TO_VICT );
						act( "{5$N szybkim ruchem unosi tarcz�, jednak tw�j cios by� zbyt pot�ny.{x", ch, shield, victim, TO_CHAR );
						act( "{5$N szybkim ruchem unosi tarcz�, cios $z by� jednak zbyt pot�ny.{x", ch, shield, victim, TO_NOTVICT );
						break;
					case 2:
						act( "{5Os�aniasz si� $j, jednak pot�ny cios $z odtr�ca tarcz� na bok i trafia ci�!{x", ch, shield, victim, TO_VICT );
						act( "{5$N os�ania si� $j, jednak tw�j pot�ny cios odtr�ca tarcz� na bok i trafia $C!{x", ch, shield, victim, TO_CHAR );
						act( "{5$N os�ania si� $j, jednak pot�ny cios $z odtr�ca tarcz� na bok i trafia $C!{x", ch, shield, victim, TO_NOTVICT );
						break;
					case 3:
						act( "{5P�ynnym ruchem zas�aniasz si� $j, jednak uderzenie $z odtr�ca twoj� tarcze na bok i trafia ci�!{x", ch, shield, victim, TO_VICT );
						act( "{5P�ynnym ruchem $N zas�ania si� $j, jednak tw�j silny cios uderza w tarcz�, odrzucaj�c j� na bok i trafia w $Z!{x", ch, shield, victim, TO_CHAR );
						act( "{5P�ynnym ruchem $N zas�ania si� $j, jednak silne uderzenie $z odtr�ca $s tarcz� na bok i trafia $m!{x", ch, shield, victim, TO_NOTVICT );
						break;
				}
				return FALSE;
			}
		}
		else
		{
			switch ( number_range( 1, 4 ) )
			{
				case 1:
					act( "{5Szybkim ruchem unosisz tarcz� w ostatniej chwili zbijaj�c cios $z.{x", ch, shield, victim, TO_VICT );
					act( "{5$N szybkim ruchem unosi tarcz� w ostatniej chwili zbijaj�c twoj cios.{x", ch, shield, victim, TO_CHAR );
					act( "{5$N szybkim ruchem unosi tarcz� w ostatniej chwili zbijaj�c cios $z.{x", ch, shield, victim, TO_NOTVICT );
					break;
				case 2:
					act( "{5Szybkim ruchem wystawiasz $h i parujesz uderzenie $z.{x", ch, shield, victim, TO_VICT );
					act( "{5$N szybkim, wy�wiczonym ruchem wystawia przed siebie $h i paruje twoje uderzenie.{x", ch, shield, victim, TO_CHAR );
					act( "{5$N szybkim, wy�wiczonym ruchem wystawia $h i paruje uderzenie $z.{x", ch, shield, victim, TO_NOTVICT );
					break;
				case 3:
					act( "{5Dok�adnym, wy�wiczonym ruchem wystawiasz $h przed siebie i cios $z ze�lizguje si� nie czyni�c ci �adnej szkody.{x", ch, shield, victim, TO_VICT );
					act( "{5Dok�adnym, wy�wiczonym ruchem $N wystawia $h przed siebie i tw�j atak ze�lizguje si� nie czyni�c $O �adnej szkody.{x", ch, shield, victim, TO_CHAR );
					act( "{5Dok�adnym, wy�wiczonym ruchem $N wystawia $h przed siebie i cios $z ze�lizguje si� nie czyni�c $O �adnej szkody.{x", ch, shield, victim, TO_NOTVICT );
					break;
				case 4:
					act( "{5Zas�aniasz si� $j, tak �e cios $z nie czyni ci �adnej szkody.{x", ch, shield, victim, TO_VICT );
					act( "{5$N zas�ania si� $j, tak �e tw�j cios nie czyni $O �adnej szkody.{x", ch, shield, victim, TO_CHAR );
					act( "{5$N zas�ania si� $j, tak �e cios $z nie czyni $O �adnej szkody.{x", ch, shield, victim, TO_NOTVICT );
					break;
			}
			return TRUE;
		}
	}

	alternatywny_blok = number_range( UMIN( get_skill( victim, gsn_shield_block )/10, 10 ), UMAX( 11, (UMAX( get_curr_stat_deprecated( ch,STAT_STR ), get_curr_stat_deprecated( ch,STAT_DEX ) )-13)*2 ) );


	if ( alternatywny_blok > *dam )
	{
		if( shield_type >= 0 )
			max_dam = shield->value[2];
		else
			max_dam = 25;

		check_improve( victim, ch, gsn_shield_block, TRUE, 80);

		max_dam += str_app[get_curr_stat(victim,STAT_STR)].todam;


		if( max_dam < *dam )
		{
			if ( max_dam <= ( *dam * 2 ) && !IS_OBJ_STAT( shield, ITEM_UNDESTRUCTABLE )
					&& number_percent() > material_table[ shield->material ].hardness )
			{
				shield->condition -= UMIN( 5, *dam / UMAX( 1, max_dam ) );
			}

			*dam -= max_dam;

			if ( shield->condition <= 0 )
			{
				act( "{5Pr�bujesz zablokowa� mia�d��cy cios $z, ale $p rozpada si� na dwie cz�ci. Czujesz dotkliwy b�l w przedramieniu.{x", ch, shield, victim, TO_VICT );
				act( "{5$N pr�buje zablokowa� twoj mia�d��cy cios, ale $S $p rozpada si� na dwie cz�ci. Przez twarz $Z przebiega grymas b�lu.{x", ch, shield, victim, TO_CHAR );
				act( "{5$N pr�buje zablokowa� mia�d��cy cios $z, ale $S $p rozpada si� na dwie cz�ci. Przez twarz $Z przebiega grymas b�lu.{x", ch, shield, victim, TO_NOTVICT );

				/*artefact*/
				if ( is_artefact( shield ) ) extract_artefact( shield );

				extract_obj( shield );
				return FALSE;
			}
			else
			{
				switch ( number_range( 1, 3 ) )
				{
					case 1:
						act( "{5Szybkim ruchem unosisz tarcz�, cios $z by� jednak zbyt pot�ny.{x", ch, shield, victim, TO_VICT );
						act( "{5$N szybkim ruchem unosi tarcz�, cios $z by� jednak zbyt pot�ny.{x", ch, shield, victim, TO_CHAR );
						act( "{5$N szybkim ruchem unosi tarcz�, cios $z by� jednak zbyt pot�ny.{x", ch, shield, victim, TO_NOTVICT );
						break;
					case 2:
						act( "{5Os�aniasz si� $j, jednak pot�ny cios $z odtr�ca tarcz� na bok i trafia ci�!{x", ch, shield, victim, TO_VICT );
						act( "{5$N os�ania si� $j, jednak tw�j pot�ny cios odtr�ca tarcz� na bok i trafia $C!{x", ch, shield, victim, TO_CHAR );
						act( "{5$N os�ania si� $j, jednak pot�ny cios $z odtr�ca tarcz� na bok i trafia $C!{x", ch, shield, victim, TO_NOTVICT );
						break;
					case 3:
						act( "{5P�ynnym ruchem zas�aniasz si� $j, jednak uderzenie $z odtr�ca twoj� tarcze na bok i trafia ci�!{x", ch, shield, victim, TO_VICT );
						act( "{5P�ynnym ruchem $N zas�ania si� $j, jednak tw�j silny cios uderza w tarcz�, odrzucaj�c j� na bok i trafia w $Z!{x", ch, shield, victim, TO_CHAR );
						act( "{5P�ynnym ruchem $N zas�ania si� $j, jednak silne uderzenie $z odtr�ca $s tarcz� na bok i trafia $m!{x", ch, shield, victim, TO_NOTVICT );
						break;
				}
				return FALSE;
			}
		}
		else
		{
			switch ( number_range( 1, 4 ) )
			{
				case 1:
					act( "{5Szybkim ruchem unosisz tarcz� w ostatniej chwili zbijaj�c cios $z.{x", ch, shield, victim, TO_VICT );
					act( "{5$N szybkim ruchem unosi tarcz� w ostatniej chwili zbijaj�c twoj cios.{x", ch, shield, victim, TO_CHAR );
					act( "{5$N szybkim ruchem unosi tarcz� w ostatniej chwili zbijaj�c cios $z.{x", ch, shield, victim, TO_NOTVICT );
					break;
				case 2:
					act( "{5Szybkim ruchem wystawiasz $h i parujesz uderzenie $z.{x", ch, shield, victim, TO_VICT );
					act( "{5$N szybkim, wy�wiczonym ruchem wystawia przed siebie $h i paruje twoje uderzenie.{x", ch, shield, victim, TO_CHAR );
					act( "{5$N szybkim, wy�wiczonym ruchem wystawia $h i paruje uderzenie $z.{x", ch, shield, victim, TO_NOTVICT );
					break;
				case 3:
					act( "{5Dok�adnym, wy�wiczonym ruchem wystawiasz $h przed siebie i cios $z ze�lizguje si� nie czyni�c ci �adnej szkody.{x", ch, shield, victim, TO_VICT );
					act( "{5Dok�adnym, wy�wiczonym ruchem $N wystawia $h przed siebie i tw�j atak ze�lizguje si� nie czyni�c $O �adnej szkody.{x", ch, shield, victim, TO_CHAR );
					act( "{5Dok�adnym, wy�wiczonym ruchem $N wystawia $h przed siebie i cios $z ze�lizguje si� nie czyni�c $O �adnej szkody.{x", ch, shield, victim, TO_NOTVICT );
					break;
				case 4:
					act( "{5Zas�aniasz si� $j, tak �e cios $z nie czyni ci �adnej szkody.{x", ch, shield, victim, TO_VICT );
					act( "{5$N zas�ania si� $j, tak �e tw�j cios nie czyni $O �adnej szkody.{x", ch, shield, victim, TO_CHAR );
					act( "{5$N zas�ania si� $j, tak �e cios $z nie czyni $O �adnej szkody.{x", ch, shield, victim, TO_NOTVICT );
					break;
			}
			return TRUE;
		}
	}

	check_improve( victim, ch, gsn_shield_block, FALSE, 280);

	return FALSE;
}


void one_hit_shield( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
	OBJ_DATA * shield;
	OBJ_DATA * obj, *obj_next, *damned_armor;
	CHAR_DATA *tch;
	int victim_ac;
	int thac0_00, thac0_32, thac0;
	int dam;
#ifdef INFO
    int base_dam = 0;
#endif
	int dice_1, dice_2;
	int diceroll, critical_chance;
	int sn = -1;
	int dam_type;
	bool immune = FALSE, critical = FALSE, prog_mod = FALSE, check;
	AFFECT_DATA *scarfskin, *damned, *defense_curl;

	DEBUG_INFO( "one_hit_shield:checks" );
	if ( victim == ch || ch == NULL || victim == NULL )
		return;

	if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
		return;

	if ( check_blink( ch, victim, FALSE ) )
	{
		send_to_char( "Pr�bujesz wyprowadzi� cios, jednak przeskakujesz na chwilk� do innego planu egzystencji i przeciwnik znika ci z oczu.\n\r", ch );
		return;
	}

	shield = get_eq_char( ch, WEAR_SHIELD );

	if ( shield == NULL )
		return;

	if ( dt == TYPE_UNDEFINED )
	{
		dt = TYPE_HIT + 43;
	}

	//zawsze uderzenie
	dam_type = DAM_BASH;

	DEBUG_INFO( "onehit_shield:get_weapon_skill" );
	sn = gsn_shield_block;

	DEBUG_INFO( "onehit_shield:get_AC" );
	victim_ac = GET_AC( victim, AC_BASH ) / 10;

	DEBUG_INFO( "onehit_shield:ac_modifiers" );
	if ( !can_see( ch, victim ) )
		victim_ac -= 4;

    check = FALSE;
	if ( ch->alignment < -750 && IS_AFFECTED( victim, AFF_PROTECT_EVIL ) && !check )
    {
		victim_ac -= 2;
    }

	if ( !IS_EVIL(ch) && victim->class == CLASS_BLACK_KNIGHT && ( damned_armor = get_eq_char(victim,WEAR_BODY)) != NULL )
	{
		for ( damned = damned_armor->affected; damned != NULL; damned = damned->next )
            if ( damned->type == gsn_damn_armor )
            {
                check = FALSE;
                if( IS_GOOD(ch) && !check )
                {
                    victim_ac -= URANGE( 1, damned->level/10 +  get_skill(victim,gsn_damn_armor)/24, 6 );
                }
				else
                {
					victim_ac -= URANGE( 1, damned->level/12 +  get_skill(victim,gsn_damn_armor)/33, 4 );
                }
			}
	}

	if ( victim->position < POS_FIGHTING )
		victim_ac += 3;

	if ( victim->position < POS_RESTING )
		victim_ac += 6;

	if ( IS_SET( victim->fight_data, FIGHT_CASTING ) )
		victim_ac += 3;

	if ( IS_SET( victim->fight_data, FIGHT_VERTICAL_SLASH ) )
		victim_ac += 6;

	if ( victim->daze > 0 )
		victim_ac += 4;

	if ( IS_NPC( ch ) )
	{
		thac0_00 = 20;
		if ( EXT_IS_SET( ch->act, ACT_WARRIOR ) )
			thac0_32 = -2;
		if ( EXT_IS_SET( ch->act, ACT_MONK ) )
			thac0_32 = -2;
		else if ( EXT_IS_SET( ch->act, ACT_PALADIN ) )
			thac0_32 = 0;
		else if ( EXT_IS_SET( ch->act, ACT_BLACK_KNIGHT ) )
			thac0_32 = 0;
		else if ( EXT_IS_SET( ch->act, ACT_BARBARIAN ) )
			thac0_32 = 0;
		else if ( EXT_IS_SET( ch->act, ACT_THIEF ) )
			thac0_32 = 4;
		else if ( EXT_IS_SET( ch->act, ACT_BARD ) )
			thac0_32 = 4;
		else if ( EXT_IS_SET( ch->act, ACT_CLERIC ) )
			thac0_32 = 4;
		else if ( EXT_IS_SET( ch->act, ACT_MAGE ) )
			thac0_32 = 8;
		else if ( EXT_IS_SET( ch->act, ACT_SHAMAN ) )
			thac0_32 = 4;
		else
			thac0_32 = 2;
	}
	else
	{
		thac0_00 = class_table[ ch->class ].thac0_00;
		thac0_32 = class_table[ ch->class ].thac0_32;
	}

	thac0 = interpolate( ch->level, thac0_00, thac0_32 );

	thac0 -= GET_HITROLL( ch );

	thac0 -= UMIN( shield->value[ 4 ], 6 );

	if ( get_eq_char(ch,WEAR_WIELD))
    {
        thac0 += 2 + 6 * ( 100 - get_skill(ch,gsn_two_weapon_fighting) ) / 100;
    }

	if ( get_eq_char(ch,WEAR_WIELD) )
		check_improve( ch, victim, gsn_two_weapon_fighting, TRUE, 220 );

	if ( !IS_NPC( ch ) && ch->condition[ COND_SLEEPY ] == 0 )
		thac0 += 4;

	if ( IS_SET( ch->fight_data, FIGHT_DISARM ) )
		thac0 += 4;

	if ( IS_SET( ch->fight_data, FIGHT_DODGE ) )
		thac0 += 2;

	if ( IS_SET( ch->fight_data, FIGHT_PARRY ) )
		thac0 += 6;

	if ( ch->daze > 0 )
		thac0 += 6;

	//premie i kary za rozmiary

	//tu jak jest wiekszy - kara za bicie w malego
	if ( ch->size - victim->size == 1 )
		thac0 += 2;
	else if ( ch->size - victim->size == 2 )
		thac0 += 3;
	else if ( ch->size - victim->size == 3 )
		thac0 += 4;
	else if ( ch->size - victim->size >= 4 )
		thac0 += 5;

	//jak jest mniejszy - poczatkowo premia, ale jak jest za duzy to tez kara
	if ( ch->size - victim->size == -1 )
		thac0 -= 2;
	else if ( ch->size - victim->size == -2 )
		;
	else if ( ch->size - victim->size == -3 )
		thac0 += 2;
	else if ( ch->size - victim->size <= -4 )
		thac0 += 4;

	//goody w grupce z death knightem z demon aura maja minusy do trafien
	for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
	{
		if( IS_GOOD(ch) && is_affected(tch,gsn_demon_aura) && is_same_group(ch,tch))
			thac0 += URANGE( 1, get_skill(tch,gsn_demon_aura)/16, 6);
	}

	tch = NULL;

	DEBUG_INFO( "onehit:trig_onehit" );
	to_hit_bonus = 0;
	to_dam_bonus = 0;
	fight_text_ch[ 0 ] = '\0';
	fight_text_victim[ 0 ] = '\0';
	fight_text_room[ 0 ] = '\0';

	/*onehit trigger for mobiles*/
	if ( shield && HAS_OTRIGGER( shield, TRIG_ONEHIT ) )
		prog_mod = op_onehit_trigger( shield, ch );

	if ( !victim->in_room )
		return;

	if ( prog_mod && to_hit_bonus != 0 )
		thac0 -= to_hit_bonus;

	check_defensive_spells( ch, victim );

	if ( ch->position < POS_RESTING || !ch->in_room )
		return;

	while ( ( diceroll = number_bits( 5 ) ) >= 20 );

	/* standardowe rzeczy przeklejone z damage(...)*/

	DEBUG_INFO( "onehit:standard_checks" );
	if ( victim->position == POS_DEAD )
		return;

	if ( victim != ch )
	{
		DEBUG_INFO( "onehit:trig_kill" );
		if ( victim->position > POS_STUNNED )
		{
			if ( victim->fighting == NULL )
			{
				set_fighting( victim, ch );

				if ( IS_NPC( victim ) && HAS_TRIGGER( victim, TRIG_KILL ) )
					mp_percent_trigger( victim, ch, NULL, NULL, &TRIG_KILL );
			}
		}

		DEBUG_INFO( "onehit:set_fighting" );
		if ( ch->position > POS_STUNNED )
		{
			if ( ch->fighting == NULL )
				set_fighting( ch, victim );
		}

		DEBUG_INFO( "onehit:charm_stop_follower" );
		if ( victim->master == ch && !accident )
		{
			if( IS_NPC( victim ) && IS_AFFECTED(victim,AFF_CHARM) )
			{
				act( "Wi� mi�dzy tob� a $V zostaje brutalnie przerwana!", ch, NULL, victim, TO_CHAR );
				raw_damage( ch, ch, number_range( ch->level/2, ch->level+ch->level/2 ) );
				if ( ch->position <= POS_STUNNED )
					return;
			}
			stop_follower( victim );
		}

		DEBUG_INFO( "onehit:strip_hide/invis" );
		if ( IS_AFFECTED( ch, AFF_HIDE ) )
			affect_strip( ch, gsn_hide );

		strip_invis( ch, TRUE, TRUE );

		DEBUG_INFO( "onehit:start_hating" );
		if ( IS_NPC( victim ) && !is_fearing( victim, ch ) && !accident &&
				!is_hating( victim, ch ) )
		{
			if ( !EXT_IS_SET( victim->act, ACT_SENTINEL ) )
				start_hunting( victim, ch );

			start_hating( victim, ch );
		}
	}
	/* koniec standardowych rzeczy*/


	DEBUG_INFO( "onehit:miss" );
	/*czesc kodu przy nietrafieniu kogostam*/
	if ( ( diceroll == 0 || ( can_move( victim ) && diceroll != 19 && diceroll < thac0 - victim_ac ) ) )
	{
		switch( number_range(1, 2 + ( get_eq_char( victim,WEAR_BODY ) ? 1 : 0 ) ) )
		{
			case 1:
				act( "Pr�bujesz uderzy� $C $j, jednak chybiasz.", ch, shield, victim, TO_CHAR );
				act( "$n pr�buje uderzy� ci� $j, jednak chybia.", ch, shield, victim, TO_VICT );
				act( "$n pr�buje uderzy� $C $j, jednak chybia.", ch, shield, victim, TO_NOTVICT );
				break;
			case 2:
				act( "Bior�c szeroki zamach pr�bujesz trafi� $C kantem $f, jednak $E uchyla si�.", ch, shield, victim, TO_CHAR );
				act( "Bior�c szeroki zamach $n pr�buje trafi� ci� kantem $f, jednak szybko uchylasz si�.", ch, shield, victim, TO_VICT );
				act( "Bior�c szeroki zamach $n pr�buje trafi� $C kantem $f, jednak $E uchyla si�.", ch, shield, victim, TO_NOTVICT );
				break;
			case 3:
				act( "P�ynnym ruchem r�ki uderzasz tarcz� w pier� $Z, jednak przez $f cios nie odni�s� skutku.", ch, get_eq_char(victim,WEAR_BODY), victim, TO_CHAR );
				act( "P�ynnym ruchem r�ki $n uderza tarcz� w tw� pier�, jednak dzi�ki $f nic ci si� nie sta�o.", ch, get_eq_char(victim,WEAR_BODY), victim, TO_VICT );
				act( "P�ynnym ruchem r�ki $n uderza tarcz� w pier� $Z, jednak dzi�ki $f cios nie odni�s� skutku.", ch, get_eq_char(victim,WEAR_BODY), victim, TO_NOTVICT );
				break;
		}
		if ( sn != -1 )
        {
			check_improve( ch, victim, sn, FALSE, 280 );
        }
		return;
	}
	/* koniec czesci kodu przy nie-trafieniu kogostam */

	DEBUG_INFO( "onehit_shield:hitting" );
	/* jesli konkretna umiejetnosc broni to sprawdzamy czy si� nauczyl czegos*/
	if ( sn != -1 )
    {
		check_improve( ch, victim, sn, TRUE, 250 );
    }

	switch( shield->value[0] )
	{
        default:
		case 0://puklerz
			dice_1 = 1;
			dice_2 = 5;
			break;
		case 1://lekka
			dice_1 = 2;
			dice_2 = 3;
			break;
		case 2://srednia
			dice_1 = 2;
			dice_2 = 4;
			break;
		case 3://ciezka
			dice_1 = 2;
			dice_2 = 5;
			break;
		case 4://pawez
			dice_1 = 2;
			dice_2 = 6;
			break;
	}

	/* jesli uzywa jakiegos narzedzia*/
	dam = dice( UMIN( dice_1, 8 ), UMIN( dice_2, 20 ) ) + UMIN( shield->value[ 5 ], 6 );

#ifdef INFO
	base_dam = dam;
	print_char( ch, "BASE_DAM:%d\n\r", base_dam );
#endif

	/* critical */

	critical_chance = 19;
	if ( is_affected(ch,gsn_wardance) && number_percent() < get_skill(ch,gsn_wardance) ) --critical_chance;
	if ( is_affected(ch,gsn_berserk) && number_percent() < get_skill(ch,gsn_berserk) ) --critical_chance;
	if ( IS_AFFECTED(ch,AFF_HALLUCINATIONS_POSITIVE) ) --critical_chance;
	if ( IS_AFFECTED(ch,AFF_EYES_OF_THE_TORTURER) ) critical_chance -= 3;

	if ( diceroll >= critical_chance )
	{
		dam += dice( UMIN( dice_1, 8 ), UMIN( dice_2, 20 ) ) + UMIN( shield->value[ 5 ], 6 );
		critical = TRUE;

#ifdef INFO
		print_char( ch, "CRITICAL: %d\n\r", dam );
#endif

	}

	/* modyfikator obrazen ze wzgledu na uszkodzenie broni */
	//niezbyt pikne
	if ( shield->condition >= 90 )
		;
	else if ( shield->condition >= 85 )
		dam = UMAX( ( ( 95 * dam ) / 100 ), 1 );
	else if ( shield->condition >= 75 )
		dam = UMAX( ( ( 90 * dam ) / 100 ), 1 );
	else if ( shield->condition >= 65 )
		dam = UMAX( ( ( 70 * dam ) / 100 ), 1 );
	else if ( shield->condition >= 45 )
		dam = UMAX( ( ( 65 * dam ) / 100 ), 1 );
	else
		dam = UMAX( dam / 2, 1 );

#ifdef INFO
	print_char( ch, "DAM -WEAPON_CONDITION: %d\n\r", dam );
#endif

	if ( prog_mod && to_dam_bonus != 0 )
		dam += to_dam_bonus;

	//bonus do walki bronia
	if ( IS_NPC( ch ) )
		dam += ch->weapon_damage_bonus;

	//dobzi slabiej bija jak sa w grupce z death knightem z demoniczna aura
	for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
	{
		if( IS_GOOD(ch) && is_affected(tch,gsn_demon_aura) && is_same_group(ch,tch))
			dam -= URANGE( 1, get_skill(tch,gsn_demon_aura)/24, 4);
	}

	/* dodajemy reszte dodatkowych obrazen */
	dam += GET_DAMROLL( ch, shield );

#ifdef INFO
	print_char( ch, "DAM +DAMROLL: %d\n\r", dam );
#endif

	DEBUG_INFO( "onehit_shield:bashed_mod" );
	/* jesli koles nie stoi */
	if ( victim->position < POS_FIGHTING )
	{
		/* nieprzytomy/spiacy obrazenia razy 2, reszta +25%*/
		if ( !IS_AWAKE( victim ) )
			dam *= 2;
		else
			dam += dam / 4;
#ifdef INFO
		print_char( ch, "DAM + POSITION_BONUS: %d\n\r", dam );
#endif

	}

	/* sparowany atak??*/
	if ( dt != gsn_backstab /*dt != gsn_backstab && dt != gsn_circle && dt != gsn_charge*/ )
	{
		DEBUG_INFO( "onehit_shield:parry" );
		if ( check_parry( ch, victim, dam, shield ) )
			return;

		DEBUG_INFO( "onehit_shield:dodge" );
		/* po dodge, jest szansa ze cios pojdzie w kogos obok */
		if ( check_dodge( ch, victim, victim_ac , diceroll, thac0, critical ) )
		{
			CHAR_DATA * vch = NULL;

			if ( number_percent() < 15 )
			{
				for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
					if ( vch != ch && vch->fighting == victim )
						break;

				if ( vch )
				{
					accident = TRUE;
					one_hit_shield( ch, vch, dt );
					accident = FALSE;
					return;
				}
			}

			return;
		}

		if ( check_shield_block( ch, victim, victim_ac , diceroll, thac0, critical, &dam  ) )
			return;
	}

	/* jesli trafilismy to obrazenia minimum 1*/
	if ( dam <= 0 )
		dam = 1;

	if ( dam > 70 && !IS_NPC( ch ) && !IS_IMMORTAL( ch ) && dt == TYPE_UNDEFINED )
	{
		bugf( "Gracz: %s co� podejrzanie mocno bije z %s [%d]!", ch->name, shield->short_descr, shield->pIndexData->vnum );
	}

	DEBUG_INFO( "onehit_shield:blink" );
	if ( check_blink( victim, ch, TRUE ) )
	{
		act( "Kiedy tw�j cios ju� ma trafi� w $C, $S sylwetka chwilowo znika.", ch, NULL, victim, TO_CHAR );
		act( "Kiedy cios $z ma ju� w ciebie trafi� znikasz na chwilk� z tego planu egzystencji.", ch, NULL, victim, TO_VICT );
		act( "Kiedy cios $z ma ju� trafi� w $C, $S sylwetka chwilowo znika.", ch, NULL, victim, TO_NOTVICT );
		return;
	}

	DEBUG_INFO( "onehit_shield:mirror_image" );
	if ( check_mirror_image( victim, ch ) )
	{
		print_char( ch, "Tw�j cios trafia, %s znika.\n\r", PERS( victim, ch ) );
		print_char( victim, "%s trafia jedno z twoich lustrzanych odbi�.\n\r", PERS( ch, victim ) );
		act( "$n trafia jedno z lustrzanych odbi� $Z.", ch, NULL, victim, TO_NOTVICT );
		return;
	}

	DEBUG_INFO( "onehit_shield:illusion" );
	if ( IS_AFFECTED( victim, AFF_ILLUSION ) )
	{
		print_char( ch, "Tw�j cios trafia, %s rozp�ywa si� w powietrzu.\n\r", PERS( victim, ch ) );
		print_char( victim, "%s trafia ci�, rozp�ywasz si� w powietrzu.\n\r", PERS( ch, victim ) );
		act( "$n trafia, $N rozp�ywa si� w powietrzu.", ch, NULL, victim, TO_NOTVICT );

		/* itemy co je trzymala iluzja opadaja na ziemie*/
		for ( obj = victim->carrying; obj != NULL; obj = obj_next )
		{
			OBJ_NEXT_CONTENT( obj, obj_next );

			if ( IS_OBJ_STAT( obj, ITEM_NODROP ) )
				continue;
			if ( obj->wear_loc != WEAR_NONE )
				unequip_char( victim, obj );

			obj_from_char( obj );
			act( "$p upada na ziemi�.", victim, obj, NULL, TO_ROOM );
			obj_to_room( obj, victim->in_room );

			if ( IS_OBJ_STAT( obj, ITEM_MELT_DROP ) )
			{
				switch ( number_range( 1, 3 ) )
				{
					case 1:
						act( "$p rozp�ywa si� w powietrzu.", ch, obj, NULL, TO_ROOM );
						break;
					case 2:
						act( "$p znika bez �ladu.", ch, obj, NULL, TO_ROOM );
						break;
					default:
						act( "$p rozp�ywa si� pozostawiaj�c tylko chmur� dymu.", ch, obj, NULL, TO_ROOM );
						break;
				}
			}
			else if ( obj->item_type == ITEM_POTION && ( obj->material == 11 || obj->material == 15 ) )
			{
				switch ( number_range( 1, 3 ) )
				{
					case 1:
						act( "$p toczy si� przez chwil�, po czym rozpryskuj� si� na kwa�eczki.", ch, obj, NULL, TO_ROOM );
						break;
					case 2:
						act( "$p p�ka z hukiem i rozpryskuje si� dooko�a.", ch, obj, NULL, TO_ROOM );
						break;
					default:
						act( "$p rozpryskuje si� na kawa�eczki.", ch, obj, NULL, TO_ROOM );
						break;
				}
			}
		}

		extract_char( victim, TRUE );

		return;
	}

	DEBUG_INFO( "onehit_shield:soul" );
	if ( IS_AFFECTED( victim, AFF_SOUL ) )
	{
		print_char( ch, "Tw�j cios trafia, uwolniona dusza ulatuje do swojego wymiaru.\n\r", PERS( victim, ch ) );
		print_char( victim, "%s trafia ci�, ulatujesz do swojego wymiaru.\n\r", PERS( ch, victim ) );
		act( "$n trafia, dusza $Z ulatuje do swojego wymiaru.", ch, NULL, victim, TO_NOTVICT );

		/* itemy co je trzymala dusza opadaja na ziemie*/
		for ( obj = victim->carrying; obj != NULL; obj = obj_next )
		{
			OBJ_NEXT_CONTENT( obj, obj_next );

			if ( IS_OBJ_STAT( obj, ITEM_NODROP ) )
				continue;
			if ( obj->wear_loc != WEAR_NONE )
				unequip_char( victim, obj );

			obj_from_char( obj );
			act( "$p upada na ziemi�.", victim, obj, NULL, TO_ROOM );
			obj_to_room( obj, victim->in_room );

			if ( IS_OBJ_STAT( obj, ITEM_MELT_DROP ) )
			{
				switch ( number_range( 1, 3 ) )
				{
					case 1:
						act( "$p rozp�ywa si� w powietrzu.", ch, obj, NULL, TO_ROOM );
						break;
					case 2:
						act( "$p znika bez �ladu.", ch, obj, NULL, TO_ROOM );
						break;
					default:
						act( "$p rozp�ywa si� pozostawiaj�c tylko chmur� dymu.", ch, obj, NULL, TO_ROOM );
						break;
				}
			}
			else if ( obj->item_type == ITEM_POTION && ( obj->material == 11 || obj->material == 15 ) )
			{
				switch ( number_range( 1, 3 ) )
				{
					case 1:
						act( "$p toczy si� przez chwil�, po czym rozpryskuj� si� na kwa�eczki.", ch, obj, NULL, TO_ROOM );
						break;
					case 2:
						act( "$p p�ka z hukiem i rozpryskuje si� dooko�a.", ch, obj, NULL, TO_ROOM );
						break;
					default:
						act( "$p rozpryskuje si� na kawa�eczki.", ch, obj, NULL, TO_ROOM );
						break;
				}
			}
		}
		extract_char( victim, TRUE );
		return;
	}

	DEBUG_INFO( "onehit_shield:steel_scarfskin" );
		if( is_affected( victim, gsn_steel_scarfskin ) &&
				dam <= number_range( 6, 14 ) )
		{
			scarfskin = affect_find( victim->affected, gsn_steel_scarfskin );

			if ( scarfskin->modifier > 0 )
			{
				scarfskin->modifier -= dam;
				dam = 0;

				print_char( ch, "%s odbija si� jakby� uderzy�%s w stal.\n\r", capitalize( shield->short_descr), ch->sex == 2 ? "a" : ch->sex == 0 ? "o" : "" );
				print_char( victim, "%s trafia cie, jednak %s %s odskakuje jakby uderzy�%s w stal.\n\r", PERS( ch, victim ), ch->sex == 2 ? "jej" : ch->sex == 0 ? "tego" : "jego", shield->short_descr, ch->sex == 2 ? "a" : ch->sex == 0 ? "o" : "" );
				switch ( victim->sex )
				{
					case 0:
						act( "$n trafia $C, jednak $s $p odskakuje jakby $N by�o ze stali.", ch, shield, victim, TO_NOTVICT );
						break;
					case 1:
						act( "$n trafia $C, jednak $s $p odskakuje jakby $N by� ze stali.", ch, shield, victim, TO_NOTVICT );
						break;
					default :
						act( "$n trafia $C, jednak $s $p odskakuje jakby $N by�a ze stali.", ch, shield, victim, TO_NOTVICT );
						break;
				}
			}
		}

	DEBUG_INFO( "onehit_shield:stone_skin" );
	if ( check_stone_skin( ch, victim, dam_type, &dam, NULL ) )
	{
		if ( dam == 0 )
		{
			print_char( ch, "%s odbija si� jakby� uderzy�%s w kamie�.\n\r", capitalize( shield->short_descr), ch->sex == 2 ? "a" : ch->sex == 0 ? "o" : "" );
			print_char( victim, "%s trafia cie, jednak %s %s odskakuje jakby uderzy�%s w kamie�.\n\r", PERS( ch, victim ), ch->sex == 2 ? "jej" : ch->sex == 0 ? "tego" : "jego", shield->short_descr, ch->sex == 2 ? "a" : ch->sex == 0 ? "o" : "" );
			switch ( victim->sex )
			{
				case 0:
					act( "$n trafia $C, jednak $s $p odskakuje jakby $N by�o z kamienia.", ch, shield, victim, TO_NOTVICT );
					break;
				case 1:
					act( "$n trafia $C, jednak $s $p odskakuje jakby $N by� z kamienia.", ch, shield, victim, TO_NOTVICT );
					break;
				default :
					act( "$n trafia $C, jednak $s $p odskakuje jakby $N by�a z kamienia.", ch, shield, victim, TO_NOTVICT );
					break;
			}
			return;
		}
		else
		{
			print_char( ch, "%s przebija si� przez kamienn� skor� pokrywaj�c� %s.\n\r", capitalize( shield->short_descr), PERS2( victim, ch ) );
			print_char( victim, "%s %s przebija si� przez twoj� kamienn� sk�r�.\n\r", capitalize( shield->short_descr), PERS2( ch, victim ) );
			act( "Tarcza $z trafia $C przebijaj�c si� przez kammien� sk�r�.", ch, NULL, victim, TO_NOTVICT );
		}
	}

	DEBUG_INFO( "onehit_shield:magic_weapon" );
	if ( victim->resists[ RESIST_MAGIC_WEAPON ] > 0 && check_magic_attack( ch, victim, shield ) < victim->resists[ RESIST_MAGIC_WEAPON ] )
	{
		if( check_magic_attack( ch, victim, shield ) == 0 && IS_AFFECTED(victim, AFF_RESIST_MAGIC_WEAPON))
		{
			;
		}
		else
		{
			immune = TRUE;
			dam = 0;
		}
	}

	if ( immune || dam == 0 )
	{
		print_char( ch, "Tw�j atak jest nieskuteczny, cios nic %s nie robi.\n\r", PERS3( victim, ch ) );
		print_char( victim, "Atak %s jest nieskuteczny, %s cios niczego ci nie robi.\n\r", PERS2( ch, victim ), ( ch->sex == 2 ) ? "jej" : "jego" );
		act( "$n trafia, jednak $s cios jest nieskuteczny.", ch, NULL, victim, TO_NOTVICT );
		return;
	}

	DEBUG_INFO( "onehit_shield:check_resist" );
	dam = check_resist( victim, dam_type, dam );

#ifdef INFO
	print_char( ch, "DAM -RESISTS: %d\n\r", dam );
#endif

	if ( immune || dam == 0 )
	{
		print_char( ch, "Tw�j atak jest nieskuteczny, cios nic %s nie robi.\n\r", PERS3( victim, ch ) );
		print_char( victim, "Atak %s jest nieskuteczny, %s cios niczego ci nie robi.\n\r", PERS2( ch, victim ), ch->sex == 2 ? "jej" : "jego" );
		act( "Atak $z nie robi wrazenia na $B, $s cios jest nieskuteczny.", ch, NULL, victim, TO_NOTVICT );
		return;
	}

	DEBUG_INFO( "onehit_shield:increase_wounds" );
	/* spell zwiekszajacy obrazenia slash/pierce/bash */
	check_increase_wounds( victim, dam_type, &dam );

#ifdef INFO
	print_char( ch, "DAM +INCREASE_WOUNDS: %d\n\r", dam );
#endif

	DEBUG_INFO( "onehit_shield:troll_power" );
	if ( IS_AFFECTED( victim, AFF_TROLL ) && victim->hit > 0 && victim->hit - dam < 0 )
	{
		dam = victim->hit + 1;
		create_event( EVENT_TROLL_POWER_REGEN, number_range( 3, 5 ) * PULSE_VIOLENCE, victim, NULL, 0 );
	}

	DEBUG_INFO( "onehit_shield:damage_reduction" );


	if ( is_affected(victim,gsn_demon_aura))
	{
        int mod = 14;
		if( IS_GOOD(ch))
        {
			dam -= get_skill(victim,gsn_demon_aura) / mod;
        }
		else if ( !IS_EVIL(ch))
        {
			dam -= get_skill(victim,gsn_demon_aura)/22;
        }
	}

	if ( is_affected(victim,gsn_defense_curl))
	{
		defense_curl = affect_find(victim->affected,gsn_defense_curl );
		defense_curl->modifier -= UMIN( defense_curl->level, dam );
		dam -= UMIN( defense_curl->level, dam );
		if( defense_curl->modifier <= 0 )
		{
			affect_remove( victim, defense_curl );
			act( "Otaczaj�ca ci� ochronna spirala zwija si�, a po chwili zanika.", victim,NULL,NULL,TO_CHAR);
		}
	}

	if ( is_affected( victim, gsn_damage_reduction ) && dam > 0 )
	{
		AFFECT_DATA * pAff;

		if ( ( pAff = affect_find( victim->affected, gsn_damage_reduction ) ) != NULL )
		{
			if ( pAff->level <= 16 )
				dam -= 1;
			else if ( pAff->level <= 20 )
				dam -= 2;
			else if ( pAff->level <= 25 )
				dam -= 3;
			else if ( pAff->level <= 30 )
				dam -= 4;
			else
				dam -= 5;

			dam = UMAX( dam, 1 );
		}
#ifdef INFO
		print_char( ch, "DAM - DAMAGE_REDUCTION: %d\n\r", dam );
#endif

	}

#ifdef INFO
	print_char( ch, "FINAL DAMAGE: %d\n\r", dam );
#endif

	DEBUG_INFO( "onehit_shield:dam_mesg" );
	/* walimy opis ciosu*/
	dam_message( ch, victim, dam, dt, immune );

	DEBUG_INFO( "onehit_shield:raw_damage" );
	raw_damage( ch, victim, dam );

	/* koniec ciekawostek dla umierajacych*/
	if ( ch->fighting != victim || !victim->in_room )
		return;

	DEBUG_INFO( "onehit_shield:check_armor" );
	/* dla tych co przezyli, sprawdzamy armora */
	check_armor_spell( victim, dam );

	if ( shield && HAS_OTRIGGER( shield, TRIG_HIT ) )
		op_hit_trigger( shield, ch );

	tail_chain( );
	return;
}


int get_backstabbed_dam( CHAR_DATA * ch, CHAR_DATA * victim, int old_dam )
{
    int dam = 0, multiplier, skill = get_skill( ch, gsn_backstab ), scinacz = 0, scinacz_mod = 0;
    OBJ_DATA * dagger = get_eq_char(ch, WEAR_WIELD );
    bool check;

    if( !dagger || dagger->item_type != ITEM_WEAPON || dagger->value[0] != WEAPON_DAGGER )
    {
        return old_dam;
    }

	multiplier = (30+skill)/6.5;
	if ( get_curr_stat_deprecated( ch, STAT_STR ) > 27 && stat_throw(ch,STAT_DEX) && number_range( 0, get_curr_stat_deprecated(ch,STAT_LUC) > 3 )) ++multiplier;
	if ( get_curr_stat_deprecated( ch, STAT_STR ) > 25 && stat_throw(ch,STAT_DEX) && number_range( 0, get_curr_stat_deprecated(ch,STAT_LUC) > 3 )) ++multiplier;
	if ( get_curr_stat_deprecated( ch, STAT_STR ) > 23 && stat_throw(ch,STAT_DEX) && number_range( 0, get_curr_stat_deprecated(ch,STAT_LUC) > 3 )) ++multiplier;
	if ( get_curr_stat_deprecated( ch, STAT_STR ) > 20 && stat_throw(ch,STAT_DEX) && number_range( 0, get_curr_stat_deprecated(ch,STAT_LUC) > 3 )) ++multiplier;
	if ( get_curr_stat_deprecated( ch, STAT_STR ) > 17 && stat_throw(ch,STAT_DEX) && number_range( 0, get_curr_stat_deprecated(ch,STAT_LUC) > 3 )) ++multiplier;

	//duze w przyblizeniu czlekoksztalne monstery
	if( !str_cmp( race_table[ GET_RACE(victim) ].name, "ogr" ) ||
			!str_cmp( race_table[ GET_RACE(victim) ].name, "troll" ) ||
			!str_cmp( race_table[ GET_RACE(victim) ].name, "umberkolos" ) ||
			!str_cmp( race_table[ GET_RACE(victim) ].name, "yeti" ))
	{
		if ( ch->size == SIZE_SMALL || ch->size == SIZE_TINY )//tacy co nie siegna ogrowi do plecow
			multiplier /= 2;
	}//monstery o wielkim cielsku :P
	else if (!str_cmp( race_table[ GET_RACE(victim) ].name, "wywerna" ) ||
			!str_cmp( race_table[ GET_RACE(victim) ].name, "hydra" ) ||
			!str_cmp( race_table[ GET_RACE(victim) ].name, "mantykora" ) ||
			!str_cmp( race_table[ GET_RACE(victim) ].name, "smok" ) ||
			!str_cmp( race_table[ GET_RACE(victim) ].name, "bazyliszek" ))
	{//za bardzo nie rozni sie bees w takie wielkie cus od zwyklego ciosu - 1/4 dama zwyklego beesa
		multiplier /= 4;
	}

	dam = old_dam * multiplier;

	if( dam > UMAX( 40, ( ch->level - 3 ) * 10 ) )
	{
		scinacz = number_range( (75*UMIN( (2*dam)/5, dam - UMAX( 40, (ch->level-3)*10 )))/100,UMIN( (2*dam)/5, dam - UMAX( 40, (ch->level-3)*10 )) ) ;

		switch( (number_range( 20, get_curr_stat_deprecated(ch,STAT_DEX) + get_curr_stat_deprecated(ch,STAT_INT) + get_curr_stat_deprecated(ch,STAT_LUC) )/4 ) )
		{
			default:
				break;
			case 7:
				scinacz = (scinacz*95)/100;
				break;
			case 8:
				scinacz = (scinacz*9)/10;
				break;
			case 9:
				scinacz = (scinacz*85)/100;
				break;
			case 10:
				scinacz = (scinacz*8)/10;
				break;
			case 11:
				scinacz = (scinacz*75)/100;
				break;
			case 12:
				scinacz = (scinacz*7)/10;
				break;
			case 13:
				scinacz = (scinacz*65)/100;
				break;
			case 14:
				scinacz = (scinacz*6)/10;
				break;
			case 15:
				scinacz = (scinacz*55)/100;
				break;
			case 16:
				scinacz = (scinacz*5)/10;
				break;
			case 17:
				scinacz = (scinacz*45)/100;
				break;
			case 18:
				scinacz = (scinacz*4)/10;
				break;
			case 19:
				scinacz = (scinacz*35)/100;
				break;
			case 20:
				scinacz = (scinacz*3)/10;
				break;
			case 21:
				scinacz = (scinacz*25)/100;
				break;
			case 22:
				scinacz = (scinacz*2)/10;
				break;
		}

        check = FALSE;
        if( (dam - scinacz > 300 && !check ) || dam - scinacz > 350 )
		{
			switch( (dam - scinacz - 300)/10 )
			{
				case 0:
				case 1:
					break;
				case 2:
					scinacz_mod = (scinacz*105)/100;
				case 3:
					scinacz_mod = (scinacz*110)/100;
				case 4:
					scinacz_mod = (scinacz*115)/100;
				case 5:
					scinacz_mod = (scinacz*120)/100;
				case 6:
					scinacz_mod = (scinacz*125)/100;
				case 7:
					scinacz_mod = (scinacz*130)/100;
				case 8:
					scinacz_mod = (scinacz*135)/100;
				case 9:
					scinacz_mod = (scinacz*140)/100;
				case 10:
					scinacz_mod = (scinacz*145)/100;
				case 11:
					scinacz_mod = (scinacz*150)/100;
				case 12:
					scinacz_mod = (scinacz*155)/100;
				case 13:
					scinacz_mod = (scinacz*160)/100;
				case 14:
					scinacz_mod = (scinacz*165)/100;
				case 15:
					scinacz_mod = (scinacz*170)/100;
				case 16:
					scinacz_mod = (scinacz*175)/100;
				case 17:
					scinacz_mod = (scinacz*180)/100;
				case 18:
					scinacz_mod = (scinacz*185)/100;
				case 19:
					scinacz_mod = (scinacz*190)/100;
				case 20:
					scinacz_mod = (scinacz*195)/100;
				default:
					scinacz_mod = scinacz * 2;
			}
			scinacz_mod -= scinacz;

			if( dice(2,get_curr_stat_deprecated(ch,STAT_STR)) >= dice(2,get_curr_stat_deprecated(victim,STAT_CON)))
			{
				switch( (scinacz_mod*100)/dam )
				{
					case 0:
					case 1:
						break;
					case 2:
						--scinacz_mod;
						break;
					case 3:
						scinacz_mod -= scinacz_mod/10;
						break;
					case 4:
						scinacz_mod -= scinacz_mod/9;
						break;
					case 5:
						scinacz_mod -= scinacz_mod/8;
						break;
					case 6:
						scinacz_mod -= scinacz_mod/7;
						break;
					case 7:
						scinacz_mod -= scinacz_mod/6;
						break;
					case 8:
						scinacz_mod -= scinacz_mod/5;
						break;
					case 9:
						scinacz_mod -= scinacz_mod/4;
						break;
					case 10:
						scinacz_mod -= scinacz_mod/3;
						break;
					default:
						scinacz_mod -= scinacz_mod/2;
						break;
				}
			}
			scinacz += scinacz_mod;
		}
		scinacz = UMIN( scinacz, number_range( (40*dam)/10, (6*dam)/10) );
	}

	dam -= scinacz;


    if ( IS_SET( dagger->value[ 4 ], WEAPON_HEARTSEEKER ) )
    {
        dam *= ( 100 + number_range( 1, 5 ) );
        dam /= 100;
    }

	return UMAX( old_dam*3, dam);
}

void remove_mirror( CHAR_DATA *ch, CHAR_DATA *victim, int how_many )
{
	AFFECT_DATA *mirror = NULL;

	if ( !IS_AFFECTED( victim, AFF_MIRROR_IMAGE ) )
    {
		return;
    }
	if ( ( mirror = affect_find( victim->affected, gsn_mirror_image ) ) == NULL )
    {
		return;
    }
	if ( mirror->modifier > 0 )
	{
		print_char( ch, "Jedno z lustrzanych odbi� %s znikn�o.\n\r", PERS2( victim, ch ) );
		send_to_char( "Jedno z twoich odbi� znikn�o.\n\r", victim );

		if ( is_affected(victim, gsn_mirrorfall) )
        {
			affect_strip(victim, gsn_mirrorfall);
        }
	}
	mirror->modifier -= UMAX( 1, how_many );
	if ( mirror->modifier <= 0 )
	{
		if ( mirror->type > 0 && skill_table[ mirror->type ].msg_off )
		{
			send_to_char( skill_table[ mirror->type ].msg_off, victim );
			send_to_char( "\n\r", victim );
		}
		affect_remove( victim, mirror );
	}
	return;
}

int	check_critical_hit( CHAR_DATA *ch, CHAR_DATA *victim, int *diceroll, int *dt, OBJ_DATA *wield, int *base_dam, int old_dam, int *twohander_fight_style, int *weapon_mastery, bool *critical )
{
    int chance = 0, multiplier = 1, new_dam;

    if ( *dt == gsn_charge )
    {
        new_dam = (*base_dam + old_dam)/2;//old dam tym sie tylko rozni od base, ze sa wliczone premie za sharp/vorpal
        new_dam *= 2;
        return new_dam;
    }

    //DRAKE: NIE ZMIENIAC DO KRWY NEDZY BEZ UZGODNIENIA, BO NOGI Z DUPY POWYRYWAM. ZE KURWA SWORD Z WIEKSZA SZANSA KRYTYKA MA SZANSE JAK AXE? KTO OCIPIAL?!
    if( wield->item_type == ITEM_WEAPON )
    {
        switch( wield->value[0] )
        {
            case WEAPON_EXOTIC:
                chance = 1;
                multiplier = 2;
                break;

            case WEAPON_FLAIL:
            case WEAPON_MACE: //Dwureczna bron tego typu ma szanse na mocniejsze krytyki
                chance = 1;
                if( IS_WEAPON_STAT( wield, WEAPON_TWO_HANDS ) )
                {
                    multiplier = number_range(2,3);
                }
                else
                {
                    multiplier = 2;
                }
                break;
            case WEAPON_POLEARM:
            case WEAPON_SPEAR:
            case WEAPON_AXE:
            case WEAPON_CLAWS:
                chance = 1;
                multiplier = 3;
                break;

            case WEAPON_STAFF:
            case WEAPON_SWORD:
            case WEAPON_DAGGER:
            case WEAPON_WHIP:
            case WEAPON_SHORTSWORD:
                chance = 2;
                multiplier = 2;
                break;
        }
    }

    if ( IS_WEAPON_STAT( wield, WEAPON_TWO_HANDS ) )
    {
        if ( number_percent() < *twohander_fight_style ) ++chance;
    }
    else
    {
        if ( number_percent() < *weapon_mastery ) ++chance;
    }
	
    if ( IS_AFFECTED(ch, AFF_EYES_OF_THE_TORTURER) )
    {
        chance *= 2;
    }


    if ( is_affected(ch,gsn_wardance) && ( number_percent() < get_skill(ch,gsn_wardance)) )
    {
        ++chance;
    }

    if ( is_affected(ch,gsn_berserk) && ( number_percent() < get_skill(ch,gsn_berserk)) )
    {
        ++chance;
    }

    if ( IS_AFFECTED(ch,AFF_HALLUCINATIONS_POSITIVE) )
    {
        ++chance;
    }

    if ( IS_WEAPON_STAT( wield, WEAPON_VORPAL )  )
    {
        chance += number_range(0,1);
    }

    if ( IS_WEAPON_STAT( wield, WEAPON_KEEN ) )
    {
        ++chance;
    }

    if ( IS_SET( wield->value[ 4 ], WEAPON_HEARTSEEKER ) )
    {
        ++chance;
    }

    if ( IS_WEAPON_STAT( wield, WEAPON_THUNDERING ) )
    {
        ++multiplier;
    }

    if ( IS_AFFECTED(ch, AFF_SPIRITUAL_GUIDANCE) )
    {
        multiplier += number_range(0,1);
    }

    if( *diceroll >= (20 - chance) || IS_SET( ch->fight_data, FIGHT_THOUSANDSLAYER ) || IS_SET( ch->fight_data, FIGHT_CRITICAL_STRIKE ))
    {
        new_dam = *base_dam;
        new_dam *= multiplier;
        *critical = TRUE;
        if ( IS_SET( ch->fight_data, FIGHT_CRITICAL_STRIKE ) )
        {
            REMOVE_BIT( ch->fight_data, FIGHT_CRITICAL_STRIKE );
        }
    }
    else
    {
        new_dam = old_dam;
        *critical = FALSE;
    }
    return new_dam;
}

int one_hit_check_mastery( CHAR_DATA *ch, CHAR_DATA *victim, int gsn, int check_improve_level )
{
    if ( get_skill( ch, gsn ) )
    {
        check_improve( ch, victim, gsn, TRUE, check_improve_level );
        return get_skill( ch, gsn );
    }
    return 0;
}

