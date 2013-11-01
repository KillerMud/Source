/*
 * track.h
 *
 *  Created on: 2010-05-04
 *      Author: Rafal
 */

#ifndef TRACK_H_
#define TRACK_H_

//Rysand - maksymalna d�ugo�� tracka, ustawienie na 0 powoduje powr�t do starego dzia�ania
#define MAX_TRACK_LEN 20

/**
 * Aktualizuje dane dla tracka w podanej lokacji.
 * ch - poruszaj�cy si� mob
 * door - kierunek poruszania si�
 * in_room - bie��cy pok�j
 * @see act_move.c#move_char
 */
void save_track_data(CHAR_DATA *ch, ROOM_INDEX_DATA *from_room, ROOM_INDEX_DATA *to_room, sh_int inout);
void track_new(CHAR_DATA *ch, int skill, char *argument);
void cleanup_track_data(ROOM_INDEX_DATA *room);
#endif /* TRACK_H_ */
