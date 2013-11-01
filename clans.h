#define CLAN_FILE       "../system/clans.txt"

#define CLAN_MAX_RANKS		10
#define CLAN_MAX_MEMBERS    30
#define CLAN_RANK_MEMBER	0
#define CLAN_RANK_OVERLORD	1
#define CLAN_RANK_CHIEFTAIN	2
#define CLAN_RANK_CUSTOM	3

#define CLANSTATE_WAR			1
#define CLANSTATE_NEUTRAL		2
#define CLANSTATE_PEACE			3
#define CLANSTATE_ALIANCE		4

#define CLAN_INACTIVE_PERIOD	2592000	//(30 dni - 30 * 24 * 60 * 60 )

typedef struct clan_member				CLAN_MEMBER;

extern      CLAN_DATA *           clan_list;

struct clan_member
{
	CLAN_MEMBER *	next;
	char *			name;
	int				rank;
	time_t			joined;
};

struct  clan_data
{
	CLAN_DATA *		next;							//wska�nik na nastepny klan
	long			id;								//id klanu (data stworzenia)
	char *			name;							//nazwa
	char *			motto;							//jakis slogan
	char *			description;					//opis klanu
	char *			who_name;						//nazwa na who
	CLAN_MEMBER *	members;						//lista cz�onk�w
	char *			rank_names[CLAN_MAX_RANKS];		//tablica nazw rang, pod 0 zawsze szef, pod 1 zawsze zast�pca, pod 2 zawsze zwyk�y cz�onek
	int				pkills;							//liczba graczy zabitych przez cz�onk�w
	int				pdeaths;						//liczba zgon�w cz�onk�w z r�k innych graczy
	int				mkills;							//liczba mob�w zabitych przez cz�onk�w
	int				mdeaths;						//liczba zgon�w cz�onk�w na mobach
	bool			active;							//czy klan jest aktywny
	int				ranks;							//ile rang jest w klanie (wliczajac obowiazkowe 3)
	time_t			last_entered;					//kiedy ostatnio wszed� do gry cz�onek klanu
	CLAN_STANCE*    stances;						//lista klan�w, z kt�rymi mamy jakie� sprawy (wojna, neutralno��, pok�j, sojusz);
};

struct clan_stance //CLAN_STANCE
{
	CLAN_STANCE* next;
	int clan_id;
	int clan_state;
};

/* clans.c */
long
get_clan_id				( void );
CLAN_MEMBER *
new_clan_member			( void );
void
free_clan_member		( CLAN_MEMBER * member );
void
free_members_list		( CLAN_MEMBER ** list );
CLAN_MEMBER *
get_member				( CLAN_DATA * clan, char * name );
CLAN_MEMBER *
add_to_clan				( CLAN_DATA * clan, char * name, int rank );
void
remove_from_clan		( CLAN_DATA * clan, CLAN_MEMBER * member );
void
set_clan_rank			( CLAN_DATA * clan, char * name, int rank );
void
change_member_name		( CLAN_DATA * clan, char * old_name, char * new_name );
char *
get_overlord			( CLAN_DATA * clan );
char *
get_chieftain			( CLAN_DATA * clan );
char *
get_list_by_rank		( CLAN_DATA * clan, int rank, char * colour );
int
get_count_by_rank		( CLAN_DATA * clan, int rank );
CLAN_DATA *
new_clan				( void );
void
free_clan				( CLAN_DATA *clan );
void
clan_from_list			( CLAN_DATA *clan );
CLAN_DATA *
get_clan_by_member		( char * name );
CLAN_DATA *
get_clan_by_name		( char *clan_name );
CLAN_DATA *
get_clan_by_id			( long id );
void
save_clans				( void );
void
load_clans				( void );
bool
is_clan					( CHAR_DATA *ch );
bool
is_same_clan			( CHAR_DATA *ch, CHAR_DATA *victim );
int
count_members			( CLAN_DATA * clan );
bool
clans_in_aliance(CLAN_DATA* clan, CLAN_DATA* clan_ch);
