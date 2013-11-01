/************************************************************************
 *                                                                      *
 * KILLER MUD is copyright 1999-2011 Killer MUD Staff (alphabetical)    *
 *                                                                      *
 *    Andrzejczak Dominik   (kainti@go2.pl                 ) [Kainti  ] *
 *    Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor   ] *
 *    Pietrzak Marcin       (marcin.pietrzak@mud.pl        ) [Gurthg  ] *
 *    Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas  ] *
 *    Skrzetnicki Krzysztof (tener@tenet.pl                ) [Tener   ] *
 *    Trebicki Marek        (maro@killer.radom.net         ) [Maro    ] *
 *    Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron   ] *
 *                                                                      *
 ************************************************************************/
/*

 UNSAFE:

 sprawdzi� czy wszystkie zewn�trzne zale�no�ci do danych zwi�zanych
 z shapeshiftingiem si� zgadzaj� z now� implementacj�, doda� warunki
 i sprawdzenia bezpiecze�stwa, �eby si� co� przypadkiem nie wysypa�o

 TODO:

 act_wiz.c:5860:
      * przywracanie z przemiany w przypadku copyover
      * zapisywanie do pliku faktu, �e kto� w trakcie copy by� przemieniony

 doda� obs�ug� samego procesu przemiany w nast�puj�cy spos�b:
      * w momencie przemiany odpalana jest funkcja shapeshift_one_step();
      * funkcja ta sprawdza, w kt�rym momencie przemiany jest aktualnie gracz
      * je�eli gracz pomi�dzy wywo�aniami zgin��, czy�ci dane przemiany
      * je�eli gracz zosta� zaatakowany, czyta odpowiedni� flag�, czy�ci dane i ko�czy
      * je�eli trzeba wy�wietli� komunikat
      * je�eli to nie jest koniec przemiany, zaklepuje swoje wywo�anie w przysz�o�ci
      * je�eli to koniec, wy�wietla komunikat ko�cowy, wywo�uje do_shapeshift_silent i ko�czy

 doda� obs�ug� przerwania przemiany w momencie ataku

 wprowadzi� opcjonalne ograniczenie czasowe na maksymalny okres trwania przemiany

 */

/* Q: "Jak dodawac nowe przemiany?"
 * A:
 *    1. Nalezy dodac odpowiedni wpis w tabeli shapeshift_table, najlepiej na samym jej koncu.
 *    2. Jezeli grupa jest inna niz "basic druid shapeshift", tworzymy nowy wpis do const.c, do skill_table
 *    3. Tworzymy przedmiot o vnumie zgodnym z tym, jaki podalismy w punkcie 1.
 *    4. Dajemy do apply przedmiotu wszystkie profity/minusy jakie ma otrzymywac gracz
 */

#define __SHAPESHIFTING_C__

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "merc.h"
#include "shapeshifting.h"
#include "recycle.h"

/* Stuff written by Tener */

// ob�uga komendy "reform" dla graczy
// sprawdza czy posta� spe�nia warunki i
// przemienia z powrotem do postaci gracza
//
//
// wymagana jest:
//  znajomo�� przemian
//  bycie przemienionym
//  odpowiednia pozycja
//
// przemiana powrotna zajmuje pewien czas,
// wy�wietlane s� wtedy odpowiednie komunikaty
//
// argumenty funkcji:
// ch : posta� do przemiany
// argument : tekst kt�ry wpisa�a ona przy reform, ignorowany

/* tabela przemian */

const SS_FORM shapeshift_table[ MAX_SHAPE ] =
        {
                {
                        "null shape", // nazwa
                        CLASS_DRUID, // klasa
                        { 4000, 7805, 7805, 7805, 7805, }, // vnumy na przemiany
                        1, // liczba tych vnum�w powyzej
                        0, // minimalny poziom
                        0, // vnum itemu
                        0, // modyfikator trudnosci
                        &gsn_none, // skill name
			1,
                        {{ "text 1" }, // teksty
			 { "text 2" }},
			1,
                        {{ "text 3" },
			 { "text 4" }}
                },
                {
                        "Nied�wied�",
                        CLASS_DRUID,
                        { 32807, 7805, 39973, 43067, 42654 },
                        5,
                        DRUID_LOW_LEV,
                        SS_BEAR_ITEM,
                        DRUID_LOW_MOD,
                        &gsn_basic_shapeshift,
			1,
                        {{ "W b�ysku swiat�a $N przemienia si� w $c!\n" },
			 { "Przyzywasz moce natury i stajesz si� $v...\n" }},
			1,
                        {{ "W miejscu $Z pojawia si� $n!\n" },
			 { "Przyjmujesz swe zwyk�e cia�o.\n" }}
                },
                {
                        "Ptak",
                        CLASS_DRUID,
                        { 9327, 1704, 1707, 3044, 9327 },
                        5,
                        DRUID_LOW_LEV,
                        SS_EAGLE_ITEM,
                        DRUID_LOW_MOD,
                        &gsn_basic_shapeshift,
			1,
                        {{ "Wiatr ogarniaj�cy $N przemienia go w $c!\n" },
			 { "W klatce piersiowej czujesz dziwny rodzaj b�lu... Niebo zaczyna by� ci ziemi�, przemieni�e� si� w $c.\n" }},
			1,
			{{ "Wiatr wzm�g� si� nagle, $n pojawia si� przed tob�.\n" },
			 { "Powr�ci�e� do dawnego cia�a.\n" }}
                },
                {
                        "Kot",
                        CLASS_DRUID,
                        { 524, 4350, 27895, 2667, 2703 },
                        5,
                        DRUID_MED_LEV,
                        SS_TIGER_ITEM,
                        DRUID_MED_MOD,
                        &gsn_basic_shapeshift,
			2,
                        {{ "$N upada na ziemi� i jego posta� zaczyna si� rozp�ywa�.",
                          "Powoli zaczyna przyjmowa� kocie kszta�ty. Po kr�tkiej chwili staje przed tob� $n." },
                        { "Przyzywaj�c w milczeniu si�y natury padasz na ziemi� i zaczynasz przechodzi� mutacj�.",
                          "Czujesz, jak twoje cia�o zaczyna przyjmowa� kocie kszta�ty. Po kr�tkiej chwili podnosisz si�. Ju� jako $n." }},
			2,
                        {{ "Nagle $N staje na dw�ch �apach i zaczyna przyjmowa� ludzkie kszta�ty." ,
			   "Po chwili stoi przed tob� $n"},
                         { "Stajesz na dw�ch �apach i zaczynasz powraca� do ludzkiej postaci." ,
			   "Po chwili zn�w jeste� sob�." }}
                },
                {
                        "Jele�",
                        CLASS_DRUID,
                        { 2663, 3651, 5219, 5303, 32808 },
                        5,
                        DRUID_LOW_LEV,
                        SS_DEER_ITEM,
                        DRUID_LOW_MOD,
                        &gsn_basic_shapeshift,
			3,
                        {{ "$N staje nieruchomo w miejscu... po chwili zaczyna przechodzi� dziwn� przemian�.",
                          "W miejsce ubrania i sk�ry pojawia si� g�sta, br�zowa sier�� zwierz�cia.",
			   "Po chwili $N opada na r�ce, kt�re zamieniaj� si� w kopyta... staje przed tob� $n!"},
                        { "W skupieniu przyzywasz pierwotne moce natury, staraj�c si� przybra� now� form�.",
                          "Czujesz jak magia przemienia twoje cia�o, nadaj�c mu inny kszta�t i wch�aniaj�c poprzedni...",
                          "Opadasz na przednie odn�a, przemienione w nogi $z!"}},
			2,
                        {{ "$N wydaje z siebie dono�ne r�enie... po czym w b�yskawicznym tempie zaczyna si� przemienia�!",
			   "Po chwili stoi przed tob� nie $N, lecz $n w swej zwyk�ej postaci."},
			 { "Nag�ym aktem woli starasz si� odrzuci� t� form�...",
			   "Po chwili z zadowoleniem spostrzegasz, �e ju� nie jeste� zwierz�ciem..."}}
                }
        };




inline int is_animal_or_plant( CHAR_DATA* ch )
{
  return (( IS_SET(race_table[GET_RACE(ch)].type , ANIMAL )) ||
         (  IS_SET(race_table[GET_RACE(ch)].type , INSECT )) ||
         (  IS_SET(race_table[GET_RACE(ch)].type , PLANT  ))   ) ;
}

void finalize_reform( CHAR_DATA* ch, char* arg )
{

  SHAPESHIFT_DATA* ss;
  ch->hit = get_max_hp(ch);
  ss = do_reform_silent( ch );
  free_shape_data( ss );
  update_pos(ch);

}

void show_communicates( CHAR_DATA* ch, char* arg )
{
  int shape, num;

  if ( ! ch->ss_data )
    {
      return;
    }

  shape = ch->ss_data->shape;
  num = ch->ss_data->communicate_number;
  ch->ss_backup->in_room = ch->in_room; // hack

  if (shapeshift_table[ shape ].comm_ref_num != num)
    {
      act( shapeshift_table[ shape ].comm_ref[COMM_ROOM][num], ch, NULL, ch->ss_backup, TO_ROOM  );
      act( shapeshift_table[ shape ].comm_ref[COMM_CHAR][num], ch, NULL, ch->ss_backup, TO_CHAR  );

      ch->ss_data->communicate_number += 1;
      wait_function( ch, WAIT_SHAPESHIFT, &show_communicates, "" );
    }

}


/* UNSAFE */

void do_reform( CHAR_DATA* ch, char* argument )
{
  //        SHAPESHIFT_DATA* ss = NULL;
//      CHAR_DATA * animal = NULL;
//        int k,main_skill,wait = 0;

        int main_skill = 0;

        if ( IS_NPC( ch ) )
        {
                return;
        }

        // jak kto� nie zna, to nie zna
        if ( ( main_skill = get_skill( ch, gsn_shapeshift ) ) < 1 )
        {
                send_to_char( "Huh?\n\r", ch );
                return;
        }

        if ( !ch->ss_data )
        {
                send_to_char( "Nie jeste� pod wp�ywem �adnej przemiany.\n\r", ch );
                return;
        }

        if ( ch->position != POS_RESTING )
        {
                send_to_char( "Aby si� przemieni� usi�d� wygodnie i odpr� si�.\n\r", ch );
                return;
        }

        // TODO: byc moze ustawianie hp'k�w trzeba zrobic w reform_silent?

	ch->ss_data->communicate_number = 0;
	show_communicates( ch, "" );

	wait_function( ch, shapeshift_table[ ch->ss_data->shape ].comm_ref_num * WAIT_SHAPESHIFT, &finalize_reform, "" );
	WAIT_STATE( ch, shapeshift_table[ ch->ss_data->shape ].comm_ref_num * WAIT_SHAPESHIFT * PULSE_PER_SECOND );

}



// obs�uga komendy "shapeshift" dla graczy
// sprawdza czy posta� spe�nia warunki i
// i przemiania posta� na zwierzaka
//
// wywo�ana bez argument�w funkcja wypisuje dost�pne przemiany
// (o ile posta� ma tak� mo�liwo��) i ko�czy swe dzia�anie
//
// wywo�ana z argumentami pr�buje przemieni� posta�
//
// wymagana jest:
//  znajomo�� przemian
//  odpowiednia pozycja
//  znajomo�� konkretnej przemiany
//
// najpierw alokowane s� dane na przemiane,
// nast�pnie wypisywana jest seria komunikat�w,
// na koniec wywo�ywana jest funkcja do_shapeshift_silent

/* UNSAFE */


void do_shapeshift_list( CHAR_DATA *ch, char* argument )
{
        char buffer[ MSL*10 ]; // grep "MSL" merc.h
        SHAPESHIFT_DATA *ssdata = NULL;
        int shape = 0;
        int main_skill = 0;
        int n = 0;
        int available = 0;
        int skill =0;
	//        int k = 0;
        //int wait = 0;

	int i;

        if ( IS_NPC( ch ) )
        {
                return;
        }
        //nie znamy skilla?
        if ( ( main_skill = get_skill( ch, gsn_shapeshift ) ) < 1 )
        {
                send_to_char( "Wyobra�asz sobie, �e stajesz si� ogromnym smokiem, ale nadal jeste� sob�...\n\r", ch );
                return;
        }
        // Dodajemy gosciowi podstawowe przemiany, gdy tylko osiagnal level DRUID_LOW_LEV
        if ( ( ch->class == CLASS_DRUID ) && ( ch->level >= DRUID_LOW_LEV ) )
        {
                ch->pcdata->learned[ gsn_basic_shapeshift ] = 100;
        }

        buffer[ 0 ] = '\0';

        if ( !str_cmp( argument, "list" ) )
        {
                strcat( buffer, "\n\r" );
                strcat( buffer, "{b==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>=={x\n\r" );
                strcat( buffer, "{g- {WZnane tobie przemiany: {g-\n\r" );
                strcat( buffer, "{b==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>=={x\n\r" );

                for ( n = 0; n < MAX_SHAPE; n++ )
                {
                        if ( ( shapeshift_table[ n ].class == ch->class ) &&
                             ( shapeshift_table[ n ].level <= ch->level ) &&
                               get_skill( ch, *shapeshift_table[ n ].sn ) > 0 )
                        {
                                available++;
                                sprintf( buffer+strlen(buffer), "%i(%i). %s\n\r", available,  n, shapeshift_table[ n ].name );
				// act( buffer, ch, NULL, NULL, TO_CHAR );
                        }
                }

                if ( available == 0 )
                {
                        send_to_char( "{RNie posiadasz jeszcze mo�liwo�ci przemiany.{x\n\r", ch );
			return;
                }

                strcat( buffer, "{G-------------------------------------{x\n\r" );

		send_to_char( buffer, ch );
                return;
        }

        //juz przemieniony?
        if ( ch->ss_data )
        {
                send_to_char( "Twoje cia�o ju� zosta�o przemienione... co� przespa<&�e�/�a�/�>?\n\r", ch );
                return;
        }

        //na stojaco nie da rady
        if ( ch->position != POS_RESTING )
        {
                send_to_char( "Aby si� przemieni� usi�d� wygodnie i odpr� si�.\n\r", ch );
                return;
        }

        //rozbiera� si�, ale ju�!
        if ( ! all_unequiped( ch->on ) )
        {
                send_to_char( "W trakcie przemiany nie mo�esz mie� nic na sobie.\n\r", ch );
                return;
        }

        // TODO:
        // tutaj mo�na da� obs�ug� przemiany w zwierz�ta z muda
        if ( (shape = atoi(argument) ) == 0 )
        {
                send_to_char( "W kogo chcesz si� przemieni�?\n\r", ch );
                return;
        }
        else
        {
                //przemiana z listy
                if ( ( shapeshift_table[ shape ].class != ch->class ) )
                {
                        send_to_char( "Nie wiesz niczego na temat takiego kszta�tu.\n\r", ch );
                        return;
                }
                if ( shapeshift_table[ shape ].level > ch->level )
                {
                        send_to_char( "Nie potrafisz przemieni� si� w to zwierz�.", ch );
                        return;
                }

                if ( ( skill = get_skill( ch, *shapeshift_table[ shape ].sn ) ) <= 0 )
                {
                        send_to_char( "Nie potrafisz przemieni� si� w to zwierz�.", ch );
                        return;
                }

                if ( number_percent() * 3 > ( shapeshift_table[ shape ].skill_mod + 2 * main_skill + skill ) )
                {
                        send_to_char( "Starasz si� skupi� na przemienieniu swej postaci.\n\rNiestety rozpraszasz si� i nie udaje ci si� to.\n\r", ch );
                        check_improve( ch, NULL, gsn_shapeshift, FALSE, 1 );
                        check_improve( ch, NULL, *shapeshift_table[ shape ].sn, FALSE, 2 );
                        return;
                }

                ssdata = make_shape_data( ch, shape, NULL );
        }

        if ( ssdata == NULL )
        {
                bug("do_shapeshift_list: null ssdata.", 1 );
                return;
        }

	check_improve( ch, NULL, gsn_shapeshift, TRUE, 1 );
	check_improve( ch, NULL, *shapeshift_table[ shape ].sn, TRUE, 2 );

        // komunikaty stuff

	int total_wait = 0;
	int total_comms = 0;

        // waity

        // let's rock
        do_shapeshift_silent( ch, ssdata );
        ch->hit = get_max_hp(ch);
        update_pos(ch);

	ch->ss_backup->in_room = ch->in_room; // hack

	for(i=0; i < MAX_SS_FORM_COMM; i++)
	  {
	    if ( shapeshift_table[ shape ].comm_shift_num == i )
	      break;

	    wait_act( total_wait, shapeshift_table[ shape ].comm_shift[COMM_ROOM][i], ch, NULL, ch->ss_backup, TO_ROOM  );
	    wait_act( total_wait, shapeshift_table[ shape ].comm_shift[COMM_CHAR][i], ch, NULL, ch->ss_backup, TO_CHAR  );

	    total_comms += 1;
	    total_wait += WAIT_SHAPESHIFT;

	  }

	WAIT_STATE( ch, total_wait*PULSE_PER_SECOND );

}


inline bool all_unequiped( OBJ_DATA* obj )
{
/*
 *         while( obj )
        {
                if ( obj->wear_loc != WEAR_NONE )
                        return FALSE;
                obj = obj->next;
        }
        */
        return ( obj ? FALSE : TRUE );
}

/*
 * Przenies przedmioty z from->carrying do to->carrying,
 * sprawdzaj�c wcze�niej czy all_unequiped( ch->carrying ).
 *       (dla bezpiecze�stwa podw�jne sprawdzenie)
 *
 * transfer_inventory( a, b, f ); transfer_inventory( b, a, !f ) } <=> nic si� nie powinno zmieni�
 *
 * zwraca false je�eli co� si� spieprzy�o przy sprawdzaniu warunk�w
 */

bool transfer_inventory( CHAR_DATA* from, CHAR_DATA* to, bool flag_to_set )
{
        OBJ_DATA * obj = NULL;
        // sprawdzenie
        if ( ! all_unequiped( from->on ) )
        {
                bug("transfer_inventory: not all equipment were removed.", 1 );
                return FALSE;
        }

        // czy aby na pewno nie nadpisujemy jakichs rzeczy?
        if ( to->carrying )
        {
                bug("cannot tranfser anything because to->carrying is non-NULL", 1);
                return FALSE;
        }

        // jednemu dajemy, drugiemu zabieramy, i nak�adamy flag�

        while( from->carrying )
        {
                obj = from->carrying;
                obj->shapeshift = flag_to_set;
                obj_from_char( obj );
                obj_to_char( obj, to );
        }

        return TRUE;
}



// make_char_copy kopiuje niekt�re struktury postaci ch na postac tch
// uzywane do robienia backupa statow postaci

// zlepek makr do kopiowania wartosci pomiedzy postaciami

#define COPY_VAL( x )     \
      tch->x = ch->x;

/* UNSAFE */
#define COPY_STR( x )               \
      tch->x = str_dup( ch->x ) ;
//      free_string( tch->x );

#define COPY_ARR( x, y, var )            \
      for( (var)=0; (var)<(y); (var)++ ) \
      {                                  \
              COPY_VAL( x[(var)] );    \
      }

/* UNSAFE */
bool make_char_copy ( CHAR_DATA *ch, CHAR_DATA *tch )
{
//      int i;
        if ( !ch )
        {
                bug("make_char_copy: ch == NULL", 1);
                return FALSE;
        }

        if ( !tch )
        {
                bug("make_char_copy: tch == NULL", 1);
                return FALSE;
        }
/*
        memcpy( tch, ch, sizeof( CHAR_DATA ) );
        return TRUE;

        name..name6;
        num_attacks;
        short_descr;
        long_descr;
        description;
        sex;
        weight;
        height;
        race;
        // real_race
        hit;
        max_hit;
        // move
        // max_move
        // hitroll
        // damroll
        form;
        parts;
        size;
        material;
        dam_type;
        speaking;
*/



//      COPY_VAL ( valid );

        COPY_VAL ( weight );
        COPY_VAL ( height );
        COPY_VAL ( race );
        COPY_VAL ( hit );
        COPY_VAL ( max_hit );
        COPY_VAL ( move );
        COPY_VAL ( max_move );
        COPY_VAL ( hitroll );
        COPY_VAL ( damroll );
        COPY_VAL ( size );
        COPY_VAL ( dam_type );
        COPY_VAL ( carrying );

        COPY_STR ( name );
        COPY_STR ( name2 );
        COPY_STR ( name3 );
        COPY_STR ( name4 );
        COPY_STR ( name5 );
        COPY_STR ( name6 );
        COPY_STR ( short_descr );
        COPY_STR ( long_descr );
        COPY_STR ( description );
/*
        COPY_ARR ( armor,4, i );
        COPY_ARR ( resists,MAX_RESIST, i );
        COPY_ARR ( affected_by,MAX_VECT_BANK, i );
*/
        COPY_VAL ( perm_stat[STAT_STR] );
        COPY_VAL ( perm_stat[STAT_DEX] );
        COPY_VAL ( perm_stat[STAT_CON] );
        COPY_VAL ( perm_stat[STAT_CHA] );

        COPY_VAL ( new_mod_stat[STAT_STR] );
        COPY_VAL ( new_mod_stat[STAT_DEX] );
        COPY_VAL ( new_mod_stat[STAT_CON] );
        COPY_VAL ( new_mod_stat[STAT_CHA] );

        return TRUE;
}

#undef COPY_VAL
#undef COPY_STR
#undef COPY_ARR

// alokuje w pamieci miejsce na nowy ksztalt
// inicjalizuje go danymi zaleznymi od ksztaltu (shape) lub
// zwierzaka (animal) (TODO)
//
// argumenty:
//
// ch          : wiadomo, posta� kt�ra jest przemieniana
// shape       : numer przemiany
// animal_vnum : na przyszlosc, do przemian w dowolnego
//               zwierzaka na mudzie
//
// TODO: sprawdzi� czy wszystkie elementy z CHAR_DATA s� kopiowane

/* UNSAFE */
SHAPESHIFT_DATA* make_shape_data( CHAR_DATA*ch, int shape, CHAR_DATA* animal )
{
        int animal_vnum = 0;
        int i;

        /*      TODO: obs�uga przemian w konkretne zwierze z muda wskazane przez gracza
                if ( animal != NULL )
                {
                        ...
                }
                else
        */
        int vnum_cnt = shapeshift_table[ shape ].mob_vnum_cnt;
        animal_vnum = shapeshift_table[ shape ].mob_vnum[ rand() % vnum_cnt ];

// alokujemy pami��

        SHAPESHIFT_DATA* data;
        CREATE( data, SHAPESHIFT_DATA, 1 );

// inicjalizujemy pola

        CREATE( data->animal, CHAR_DATA, 1 );

        MOB_INDEX_DATA* ix = get_mob_index( animal_vnum );
        if ( ix == NULL )
        {
                bug( "Make_shape_data: NULL ix (unknown mobile vnum).", 0 );

                // dealokujemy niewykorzystane pola

                DISPOSE( data->animal );
                DISPOSE( data );
                return NULL;
        }

// init_new_char_data:
// Nie alokuje ona nowej pamieci, tylko ustawia odpowiednie pola na warto�ci startowe

        init_new_char_data( data->animal ); // zerowanie p�l, jak w new_char() z recycle.h

// ustawianie danych moba, kopiujemy, cho� nie wszystkie, dane ustawione
// mu wed�ug OLC. generalnie jest to do�� podobny kod do tego z create_char,
// tyle �e nie chcemy tutaj o�ywia� nowego moba -> nie dodajemy go do spisu mob�w
//
// Minusem tego kodu jest zale�no�� od create_mobile: w razie aktualizowania
// tamtej funkcji trzeba b�dzie doda� odpowiedni kod tak�e tutaj.
//
// Plus jest taki �e raczej nie b�dzie du�ych zmian w create_mobile

        data->animal->pcdata = ch->pcdata;

        data->animal->name               = str_dup( ix->player_name );    /* OLC */
        data->animal->short_descr        = str_dup( ix->short_descr );    /* OLC */
        data->animal->long_descr  = str_dup( ix->long_descr );     /* OLC */
        data->animal->description        = str_dup( ix->description );    /* OLC */

        data->animal->id = ch->id;

        data->animal->prompt     = NULL;
        data->animal->prog_target = NULL;

        data->animal->name2=str_dup(ix->name2);
        data->animal->name3=str_dup(ix->name3);
        data->animal->name4=str_dup(ix->name4);
        data->animal->name5=str_dup(ix->name5);
        data->animal->name6=str_dup(ix->name6);

        data->animal->copper  = 0;
        data->animal->silver   = 0;
        data->animal->gold    = 0;
        data->animal->mithril = 0;

        data->animal->comm = ch->comm;

        /* tutaj trzeba by� ostro�nym.
         * TODO: sprawdzi� dok�adnie jak to dzia�a*/
        for (i=0;i<MAX_VECT_BANK;i++)
                data->animal->affected_by[i] = ch->affected_by[i];

        data->animal->alignment = ch->alignment;
        data->animal->level = ch->level;
        data->animal->hitroll = ix->hitroll;
        data->animal->damroll = 0;
        data->animal->max_hit = dice(ix->hit[DICE_NUMBER],
                                     ix->hit[DICE_TYPE])
                                + ix->hit[DICE_BONUS];

        data->animal->hit = data->animal->max_hit;
        data->animal->damage[DICE_NUMBER]= ix->damage[DICE_NUMBER];
        data->animal->damage[DICE_TYPE] = ix->damage[DICE_TYPE];
        data->animal->damage[DICE_BONUS] = ix->damage[DICE_BONUS];
        data->animal->dam_type  = ix->dam_type;

        if (data->animal->dam_type == 0)
        {
                bugf("make_shape_data: animal vnum %d from shape %d doesn't have any dam_type set. It should be considered a bug.",
                     animal_vnum, shape);

                switch (number_range(1,3))
                {
                case (1): data->animal->dam_type = 3;
                        break;  /* slash */
                case (2): data->animal->dam_type = 7;
                        break;  /* pound */
                case (3): data->animal->dam_type = 11;
                        break;  /* pierce */
                }
        }

        for (i = 0; i < 4; i++)
                data->animal->armor[i] = ix->ac[i];

        for (i = 0; i < MAX_RESIST; i++)
                data->animal->resists[i] = ix->resists[i];

        for (i = 0; i < MAX_RESIST; i++)
                data->animal->healing_from[i] = ix->healing_from[i];

// TODO: cz�� stats�w trzeba b�dzie kopiowa� od gracza

        for (i = 0; i < MAX_STATS; i ++)
        {
                data->animal->perm_stat[i] = ix->stats[i];
                data->animal->new_mod_stat[i]  = 0;
        }

// TODO: sprawdzic czy nie trzebaby odkomentowa� nast�pnej linijki
// ext_flags_copy( ix->off_flags, data->animal->off_flags );

        data->animal->start_pos  = ix->start_pos;
        data->animal->default_pos = ix->default_pos;
        data->animal->sex  = ix->sex;
        data->animal->speaking   = ix->speaking;

        if (data->animal->sex == 3) /* random sex */
                data->animal->sex = number_range(1,2);

        data->animal->race  = ix->race;
        data->animal->form  = ix->form;
        data->animal->parts  = ix->parts;
        data->animal->size  = ix->size;
        data->animal->material  = str_dup(ix->material);

        data->animal->precommand_pending    = FALSE;
        data->animal->precommand_fun        = NULL;
        data->animal->precommand_arg        = NULL;

        data->animal->hit = get_max_hp(data->animal);

        data->animal->position = ch->position;

        switch (data->animal->size)
        {
        case SIZE_TINY:
                data->animal->weight = number_range(1, 300 );
                data->animal->height = number_range(1, 40 );
                break;
        case SIZE_SMALL:
                data->animal->weight = number_range( 250, 450 );
                data->animal->height = number_range(40, 100 );
                break;
        default:
        case SIZE_MEDIUM:
                data->animal->weight = number_range( 650, 1200 );
                data->animal->height = number_range(110, 210 );
                break;
        case SIZE_LARGE:
                data->animal->weight = number_range( 850, 2200 );
                data->animal->height = number_range(210, 260 );
                break;
        case SIZE_HUGE:
                data->animal->weight = number_range( 2100, 3700 );
                data->animal->height = number_range( 260, 400 );
                break;
        case SIZE_GIANT:
                data->animal->weight = number_range( 3500, 6000 );
                data->animal->height = number_range(400, 700 );
                break;
        }


// TODO: kopiowanie *wybi�rcze* act'�w z ch->act
// trzeba sprawdzi� jakie w�a�ciwie acty ma ustawione przecietny gracz
        /* UNSAFE */
        for(i = 0; i < MAX_VECT_BANK; i++)
                data->animal->act[i] = ch->act[i];

        data->animal->pcdata = ch->pcdata;
////////////////////////////////////////////////////////////////////////
// inicjowanie danych data->animal mamy za sob�, teraz pozosta�e pola //
        data->backup = NULL;
        data->shape = shape;
        data->animal_vnum = animal_vnum;

        /* UNSAFE */
        // make_char_copy( ch, data->backup ); // kopiuje dane, ale wybi�rczo

        return data;
}





// uwalnia z pami�ci niepotrzebny ju� kszta�t
// TODO: sprawdzi� poprawno�� tej funkcji

/* UNSAFE */

void free_shape_data( SHAPESHIFT_DATA* ss_data )
{
        if ( ss_data == NULL )
        {
                bug("free_shape_data: null ss_data", 0 );
                return;
        }

        free_char( ss_data->backup );
        free_char( ss_data->animal );
        DISPOSE( ss_data );
}

// przemienia posta� ch z wykorzystaniem kszta�tu shape
// nie wyrzuca do muda �adnych komunikat�w, chyba �e do log�w
//
//      u�ywane przy przemianie postaci
// ORAZ przy zapisywaniu postaci do pliku
//
//             !!!!!!!         UWAGA         !!!!!!!
//
// Ta funkcja jest bardzo wa�na i je�eli zwr�ci�a FALSE, to znaczy
// �e co� jest bardzo, bardzo nie w porz�dku, poniewa� jest wywo�ywana
// mi�dzy innymi tu� przed ko�cem zapisywania postaci do pliku.
// Naprawd�, to nie powinno si� wydarzy�.
//

/* UNSAFE */

bool do_shapeshift_silent( CHAR_DATA* ch, SHAPESHIFT_DATA *shape )
{
        OBJ_INDEX_DATA *pObjIndex = NULL;
        OBJ_DATA *obj = NULL;

        if ( ! ch )
        {
                bug("CRITICAL: do_shapeshift_silent: ch == NULL", 1 );
                return FALSE;
        }
        if ( ! shape )
        {
                bug("CRITICAL: do_shapeshift_silent: shape == NULL", 1 );
                return FALSE;
        }

        if ( ch->ss_data )
        {
                bug("CRITICAL: do_shapeshift_silent: ch->ss_data != NULL", 1 );
                return FALSE;
        }

        if ( ch->ss_backup )
        {
                bug("CRITICAL: do_shapeshift_silent: ch->ss_backup != NULL", 1);
 //             return FALSE;
        }
        // sprawdzenie czy ekwipunek jest zdj�ty

        if ( ! all_unequiped( ch->on ) )
        {
                bug("CRITICAL: do_shapeshift_silent: all equipment should be removed prior to transformation.", 1 );
                return FALSE;
        }

        ch->ss_data = shape;

        if( ! ch->ss_backup )
                CREATE( ch->ss_backup, CHAR_DATA, 1 );

        transfer_inventory( ch, ch->ss_backup, TRUE );

        make_char_copy( ch, ch->ss_backup );
        make_char_copy( shape->animal, ch );

        // zak�adanie przedmiotu
        int vnum = shapeshift_table[ shape->shape ].item_vnum;
        if ( vnum != 0 )
        {
                if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
                {
                        bugf( "do_shapeshift_silent: weapon loading error [vnum:%d][animal vnum:%d]", vnum, shape->animal_vnum );
                        // akurat ten blad nie jest zbyt powazny
                }
                else
                {
                        obj = create_object( pObjIndex, FALSE );
                        obj_to_char( obj, ch );
                        equip_char( ch, obj, WEAR_WIELD, FALSE );
                }
        }

        return TRUE;
}

// przemienia posta� ch z powrotem do jej oryginalnej formy
// funkcja nie wypisuje �adnych komunikat�w do �wiata,
// chyba �e do log�w.
// U�ywana jest przy przemianie z powrotem do cz�owieka
// ORAZ przy zapisywaniu postaci przemienionej do pliku.
//
// Zwraca wska�nik do danych kszta�tu, je�eli nie jest
// on d�u�ej potrzebny nale�y go zwolni� przy u�yciu
// funkcji free_shape_data
//
//      !!!!!!!         UWAGA         !!!!!!!
//
// Ta funkcja jest bardzo wa�na i je�eli zwr�ci�a NULL, to znaczy
// �e co� jest bardzo, bardzo nie w porz�dku, poniewa� jest wywo�ywana
// mi�dzy innymi tu� przed ko�cem zapisywania postaci do pliku.
// Naprawd�, to nie powinno si� wydarzy�.
//
/* UNSAFE */

SHAPESHIFT_DATA* do_reform_silent( CHAR_DATA* ch )
{
        // zabieranie grata noszonego jako bro�
        OBJ_DATA * obj = get_eq_char( ch, WEAR_WIELD );
        if ( obj )
        {
                // obj_from_char( ch, obj ); // I tak extract_obj wywo�uje obj_from_char i takie tam
                /* na wszelki wypadek, gdyby si� komu� zachcia�o robi� artefakty jako wieldy, heh */
        	if ( is_artefact( obj ) ) extract_artefact( obj );
		if ( obj->contains ) extract_artefact_container( obj );
			extract_obj( obj );

                extract_obj( obj );
        }

        //
        // Spraw� rozwi�zujemy tak: je�eli jakim� trafem
        // druid dosta� co� do ekwipunku w trakcie przemiany,
        // zg�aszamy to jako buga ale zostawiamy mu to przy sobie
        //

        if ( ch->carrying )
        {
                bug("do_reform_silent: character has some equipment.",1 );
        }

        transfer_inventory( ch->ss_backup, ch, FALSE );
        make_char_copy( ch->ss_backup, ch );
        DISPOSE( ch->ss_backup );
//      free_char( ch->ss_backup );
        SHAPESHIFT_DATA * ss = ch->ss_data;
        ch->ss_data = NULL;
        return ss;
}


#undef __SHAPESHIFTING_C__
