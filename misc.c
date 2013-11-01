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
 * misc.c and associated patches copyright 2011 by Gurthg              *
 *                                                                     *
 * In order to use any part of this ROM Merc Diku code you must comply *
 * the original Diku license in 'license.doc' as well the Merc license *
 * in 'license.txt' and also the ROM license in 'rom.license', each to *
 * be found in doc/. Using the reward.c code without conforming to the *
 * requirements of each of these documents is violation of any and all *
 * applicable copyright laws. In particular, you may not remove any of *
 * these copyright notices or claim other's work as your own.          *
 *                                                                     *
 * Much time and thought has gone into this software you are using.    *
 * We hope that you share your improvements, too.                      *
 * "What goes around, comes around."                                   *
 *                                                                     *
 * KILLER MUD is copyright 2011-2011 Killer MUD Staff (alphabetical)   *
 *                                                                     *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: misc.c 110 2013-07-18 08:34:31Z gurthg $
 * $HeadURL: svn+ssh://svn@killer.mud.pl/killer/branches/current/src/misc.c $
 *
 */
#if defined(macintosh)
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "money.h"

/**
 * tworzy pi�ro dla rasy ktora posiada pi�ra
 */
void drop_feather( CHAR_DATA *ch )
{
    static OBJ_DATA *obj;
    char text[ MAX_INPUT_LENGTH ];

    if ( !IS_NPC( ch ) || !IS_SET( ch->parts, PART_FEATHERS ) )
    {
        return;
    }

    obj = create_object( get_obj_index( OBJ_VNUM_RANDOM_ITEM ), FALSE );

    obj->condition    = number_percent();
    obj->cost         = number_range( ch->level/2, ch->level);
    obj->gender       = GENDER_NIJAKI;
    obj->item_type    = ITEM_TRASH;
    obj->liczba_mnoga = 0;  // pojedyncza
    obj->material     = 51; // pierze
    obj->timer        = 0;
    obj->value[ 1 ]   = obj->value[ 2 ] = obj->value[ 3 ] = obj->value[ 4 ] = obj->value[ 5 ] = obj->value[ 6 ] = 0;
    obj->wear_flags   = ITEM_TAKE | ITEM_HOLD;
    obj->weight       = 1;

    free_string( obj->name );
    free_string( obj->name2 );
    free_string( obj->name3 );
    free_string( obj->name4 );
    free_string( obj->name5 );
    free_string( obj->name6 );
    free_string( obj->short_descr );
    free_string( obj->description );

    sprintf ( text, "pi�ro %s pioronazwa%s piororasa%s", ch->name2, strip_spaces( strip_colour_codes( ch->name ) ), race_table[ GET_RACE( ch ) ].name );
    obj->name = str_dup( text );

    sprintf ( text, "pi�ro %s", ch->name2 );
    obj->short_descr = str_dup( text );

    sprintf( text, "%s le�y tutaj.", capitalize( text ) );
    obj->description = str_dup( text );

    sprintf ( text, "pi�ra %s", ch->name2 );
    obj->name2 = str_dup( text);

    sprintf ( text, "pi�rze %s", ch->name2 );
    obj->name3 = str_dup( text);

    sprintf ( text, "pi�ro %s", ch->name2 );
    obj->name4 = str_dup( text);

    sprintf ( text, "pi�rem %s", ch->name2 );
    obj->name5 = str_dup( text);

    sprintf ( text, "pi�rze %s", ch->name2 );
    obj->name6 = str_dup( text);

    /**
     * pioro sepa jest komponentem
     */
    if ( !str_cmp( race_table[ GET_RACE( ch ) ].name, "sep" ) )
    {
        obj->is_spell_item      = TRUE;
        obj->spell_item_counter = 1;
        obj->spell_item_timer   = 525600;
    }

    obj_to_room( obj, ch->in_room );

    act( "$p odpada od $z.", ch, obj, NULL, TO_ROOM );

    obj = NULL;
}


/**
 * tworzy �usk� dla rasy ktora posiada �uski
 */
void drop_scale( CHAR_DATA *ch )
{
    static OBJ_DATA *obj;
    char text[ MAX_INPUT_LENGTH ];

    if ( !IS_NPC( ch ) || !IS_SET( ch->parts, PART_SCALES ) )
    {
        return;
    }

    obj = create_object( get_obj_index( OBJ_VNUM_RANDOM_ITEM ), FALSE );

    obj->condition    = number_percent();
    obj->cost         = number_range( ch->level, 2*ch->level);
    obj->gender       = GENDER_ZENSKI;
    obj->item_type    = ITEM_TRASH;
    obj->liczba_mnoga = 0; // pojedyncza
    obj->material     = str_cmp( race_table[ GET_RACE( ch ) ].name, "smok" )? 52:41; // �uska
    obj->timer        = 0;
    obj->value[ 1 ]   = obj->value[ 2 ] = obj->value[ 3 ] = obj->value[ 4 ] = obj->value[ 5 ] = obj->value[ 6 ] = 0;
    obj->wear_flags   = ITEM_TAKE | ITEM_HOLD;
    obj->weight       = 1;

    free_string( obj->name );
    free_string( obj->name2 );
    free_string( obj->name3 );
    free_string( obj->name4 );
    free_string( obj->name5 );
    free_string( obj->name6 );
    free_string( obj->short_descr );
    free_string( obj->description );

    sprintf ( text, "�uska %s luskanazwa%s luskarasa%s", ch->name2, strip_spaces( strip_colour_codes( ch->name ) ), race_table[ GET_RACE( ch ) ].name );
    obj->name = str_dup( text );

    sprintf ( text, "�uska %s", ch->name2 );
    obj->short_descr = str_dup( text );

    sprintf( text, "%s le�y tutaj.", capitalize( text ) );
    obj->description = str_dup( text );

    sprintf ( text, "�uski %s", ch->name2 );
    obj->name2 = str_dup( text);

    sprintf ( text, "�usce %s", ch->name2 );
    obj->name3 = str_dup( text);

    sprintf ( text, "�usk� %s", ch->name2 );
    obj->name4 = str_dup( text);

    sprintf ( text, "�usk� %s", ch->name2 );
    obj->name5 = str_dup( text);

    sprintf ( text, "�usce %s", ch->name2 );
    obj->name6 = str_dup( text);

    /**
     * loska smoka jest komponentem
     */
    if ( !str_cmp( race_table[ GET_RACE( ch ) ].name, "smok" ) )
    {
        obj->is_spell_item      = TRUE;
        obj->spell_item_counter = 1;
        obj->spell_item_timer   = 525600;
    }

    obj_to_room( obj, ch->in_room );

    act( "$p odpada od $z.", ch, obj, NULL, TO_ROOM );

    obj = NULL;
}

void part_dropper_wraper ( CHAR_DATA *ch )
{
    if ( IS_NPC( ch ) && IS_SET( ch->parts, PART_FEATHERS ) )
    {
        if ( number_percent() < ( ( 100 - ch->level ) / 10 ) )
        {
            drop_feather( ch );
        }
    }
    if ( IS_NPC( ch ) && IS_SET( ch->parts, PART_SCALES ) )
    {
        if ( number_percent() < ( ( 100 - ch->level ) / 20 ) )
        {
            drop_scale( ch );
        }
    }
}
