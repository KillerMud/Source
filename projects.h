#pragma once


//poziomy debugowania (czyli ilosci informacji vs spam do loga)
#define DEBUG_LEVEL_ERROR				0
#define DEBUG_LEVEL_SUSPECTED		1
#define DEBUG_LEVEL_INFO				2
#define DEBUG_LEVEL_ALL					3

//flagi projektow
#define DEBUG_PROJECT_UNKNOWN		0
#define DEBUG_PROJECT_BIGFLAG		1
#define DEBUG_PROJECT_OLCLAW		2
#define DEBUG_PROJECT_LAW				3
#define DEBUG_PROJECT_SPELL_ITEMS 4
#define DEBUG_PROJECT_DOOM			5
#define DEBUG_PROJECT_STEEL_SKIN	6

//Nil: more verbosive true and false for checking whetcher particular features are on or off
#define DISPLAY_ON TRUE
#define DISPLAY_OFF FALSE

//rellik: do debugowania, zmienna okre�laj�ca czy aktywny tryb debugowania
int debugged_project;
bool debuguj;

//rellik: do debugowania, poziomy debugowania 0 tylko b��dy (zawsze w��czone) kolejne zapisuj� coraz wi�cej informacji
//rellik: zmiana z poziomu debugowania na tabel� projekt�w [20080516]
struct projects
{
	unsigned int number; //number to jest tez numer bitu w BIGFLAG, w ten spos�b mo�na p�niej sprawdzi� czy s� aktywne 2 debugi projektow
	char *name; //name of project
	bool display_is_on;
	const char *display_to; //player name to display in-game debug info
};

typedef struct projects PROJECTS;

extern const PROJECTS projects_table[];

char *nazwa_projektu( int nr );

/* Projekty:

Pomocne przy wyszukiwaniu zmian nale��cych do danego projektu.

* rellik: friend_who
Lista znajomych pokazywanych na who.

* rellik: do_bash
Przerobiona w do�� znacznym stopniu

* rellik: wtykanie pochodni
Wtykanie, zawieszanie itp. lamp, pochodni...
Niestety niekomentowane zmiany.

* rellik: do debugowania
Funkcje i zmiany maj�ce na celu logowanie wi�kszej ilo�ci informacji do analizy czemu mud si� wysypa� np.
Dodana komenda debug x, gdzie x to poziom dok�adno�ci zapisu zdarze� do loga.
Dany jest przyk�ad u�ycia tego z funkcjami w kt�rych mo�ena doda� argument caller i takimi w kt�rych nie mo�na (u�ywane globalne gcaller).

* rellik: carve
Dodanie carvingu (nie wszystkie zmiany skomentowane)

* rellik: kary za zgon
Zmiany w karach za zgon.

* rellik: prawo w miastach
Wprowadzenie stref w kt�rych obowi�zuje prawo.

Cz�� OLC:

law
	U�ycie: law
		Wypisuje zajete ID stref.
	U�ycie: law {set|clear} <vnum_min> <vnum_max> <id_law>.\n\r", ch );
		Ustawia lub czysci przynaleznosc zakresu <vnum_min> do <vnum_max> do strefy <id_law>
	U�ycie: law id <id_law>
		Wyswietla dane strefy o id <id_law>
	U�ycie: law name <nazwa_strefy>
		Wyswietla dane strefy o nazwie <name>

LZEdit
	Sk�adnia: lzedit create [vnum] lub lzedit [vnum] lub lzedit save\n\r", ch );
	lzedit create [vnum] - tworzy now� stref� i ustawia edytor w tryb edycji strefy
	lzedit [vnum] - ustawia edytor w tryb edycji strefy
	lzedit save - zapisuje list� stref (u�ywa� po zako�czeniu modyfikacji)

W trybie edycji roomu:
	setlaw - ustawianie przynale�nosci rooma do strefy obj�tej prawem
	clearlaw - resetowanie przynale�nosci rooma do strefy obj�tej prawem

W trybie edycji stref (lzedit):

	show						//pokazuje ustawienia strefy
	remove					//usuwa strefe
	newid						//ustawia nowe id dla aktualnie edytowanej strefy
	copy						//kopiuje dane ze strefy o id podanym jako argument
	frace						//ustawianie flagi ras
	fprof						//ustawianie flagi profesji
	falign					//ustawianie flagi align
	level						//ustawianie levela do ktorego chroni prawo
	guard						//ustawianie vnum stra�nik�w ( 1, 2, 3)
	nguard					//ustawianie ilu straznik�w ( 1, 2, 3)
	time						//ustawianie czasu pami�tania zbrodni
	penalty					//ustawianie rodzaju przestepstw z odpowiadaj�cymi karami
	hunters					//ustawianie czy dozwoleni �owcy g��w
	prison					//ustawianie vnum lokacji kt�ra ma by� miejscem ka�ni

* Raszer: config showposition

- plik act_info, funkcje show_config i do_config, szukac po showpos
- plik comm.c, funckja bust_a_prompt, szukac po showpos, tutaj jest funkcja wyswietlajaca prompt podczas walki i grupowy
- plik bit.c, linia 210, flagi PLR_SHOWPOS i PLR_SHOWPOSS, umowzliwiajace wlaczenie/wylaczenie showposition i wybor rodzaju (pelny lub skrocony)
- plik merc.h linia 1571 dodanie extern, deklaracji flagi?
- tables.c, szukac po showpos

* Raszer: zmiana mv dla nieumarlych

- plik act_info, szukac po mv_undead
- plik act_move, szukac po mv_undead, edytowanie dosyc rozlegle, powtykane miedzy linijki 660 do 860
- plik comm.c, kilka miejsc, szukac tak jak wyzej RELLIK: SZUKA� TAK JAK WY�EJ? MV_UNDEAD NIE ZNALEZIONO W TYM PLIKU...
- update.c, najwa�niejsza zmiana w 4785 lini, szukac po mv_undead RELLIK: mv_undead nie znaleziono w tym pliku

* Raszer: zmiana efektu po spadnieciu energize dla nieumarlych
- zamiast zmeczenia i sennosci - weaken i slow
- plik handler.c - szukac po ener_undead RELLIK: ener_undead nie znaleziono w tym pliku

* Drake: dodanie czar�w nature ally I do IV
- podzielenie summon animal druida na cztery czary
- sprawdzenie istniej�cych mob�w
- dodanie pi�ciu nowych mob�w: monstrualny paj�k (nature ally III), pomniejszy element powietrza/wody/ziemi/ognia (nature ally IV)
- spells_dru, const, merc, magic.h

* Drake: Dodanie czar�w cure/reinvigore animal/plant
- Zmiana zasady dzia�ania tych czar�w (wcze�niej bra�y pod uwage max_hp moba), teraz s� jak zwyk�e cure'y
- przeniesienie heal plant/animal na 6 kr�g
- cure/reinvigore s�abszymi wersjami heal'a.
- spells_dru, const, magic.h

* Drake: dodanie czar�w bark guardian i wildthorn (sumonuj� planty dla druida) - lekkie podci�gni�cie liveoaka
- �miesznie niski poziom (od 2 do 20 losowo - jak na czar 5 kr�gu!), od kt�rego zale�a�y statystyki, kt�re
	mia� do�� dobrze zbalansowane. Podci�gno�em do 12-18, bark i wildthorn podobni, tyle tylko, �e inne przedzia�y
	poziomowe. (Bark na III kr�g, wildthorn na VII)
- spells_dru, const, magic.h, merc.

* Drake: Zmiana w niekt�rych czarach mag�w, druid�w i kleryk�w.
- Spells_cle: dispel evil i dispel good juz nie zabijaja przy duzej roznicy poziomow. Teraz przy roznicy 8 i wiecej
	poziomow - za kazdy poziom powyzej 8 kleryk dostaje dodatkowo +1d10 do obrazen czarku.
- Spells_dru: poprawki we wczesniej wprowadzonych czarach.
- Spells_mag: czary create_lesser_ilusion i create_greater_ilusion - iluzje maja ustawiany typ ataku 'iluzoryczne
uderzenie', o rodzaju obrazen mental. Teraz nie powinny nic robic rzeczom bezmyslnym, jak nieumarli i golemy.
- Const i merc - wprowadzenie nowego typu obrazen, 'iluzoryczne uderzenie' i zwiekszenie ilosci attack_type
 (nadaremno, iluzoryczne uderzenie jest 44, na 46 max).

* Tener: FS#4379, czyli shapeshifting
- Zmieniona struktura gracza w merc.h, co za tym idzie zmieniona procedura inicjalizacji tej struktury w save.c
- W save.c dodana obs�uga zapisu gracza przemienionego: jest on na u�amek sekundy przywracany do swojej oryginalnej postaci,
  zapisywany, i przemieniany ponownie.
- W trakcie copyover zapisywany jest numer przemiany gracza, po copy jest on przywr�cony do tej w�a�nie przemiany
- Immo widz� przemienionego gracza, je�eli stoj� tu� obok.
- Ponadto w wielu miejscach w kodzie posta� pod wp�ywem shapeshift'a udaje moba
- Dla pe�niejszego wykazu: grep ss_data *.[hc]

* rellik: bigflagi, system obslugi flag [20080517]
Struktura i funkcje do obslugi flag na bitach. Du�o prostsze od istniej�cego mechanizmu EXT_, nie operuje tak na pami�ci.
Dzia�a na systemach 32 i 64 bitowych, na tych drugich ma 2 razy wi�ksz� pojemno�� ale "o tym wie".
Flagi teraz zapisujemy jako numer bitu a nie jego warto�� wi�c #define MOJA_FLAGA 3 odnosi si� do 3 bitu, #define INNA_FLAGA 51 do 51 bitu.
Struktura w merc.h, funkcje w bit.c.

* rellik: FS#4288 , dodanie do affect�w pola kt�re odmierza czas trwania affectu nawet gdy posta� nie jest w grze
Je�li b�dzie potrzebne pozosaje zakodowanie rt_duration w olc. Na razie przewiduj� jedynie ustawianie z kodu.

* rellik: kana� newbie [20080711]
Komenda newbie kt�ra dzia�a jak immtalk czyli info do wszystkich, po��czone z istnieniem kana�u NEWBIE
oraz blocknewbie dla immo �eby komu� blokowac ten kana�. Kana� otwarty tylko dla <=LEVEL_NEWBIE
Dodanie w config komendy okre�laj�cej czy chcemy widzie� ten kana�.
Za to do ludzi o tym levelu mo�na m�wi� na tell.

* rellik: komponenty [20080820]
Komponenty niezb�dne do rzucania czar�w.
Aby obiekt by� komponentem musi mie� zdefiniowane w definicji lub instancji pole is_spell_item na TRUE.
W definicji i/lub instancji jest r�wnie� okre�lona ilo�� �adunk�w oraz czas �ycia komponentu.
W tabeli spell_items_table s� okre�lone dodatkowe parametry komponentu oraz jego powi�zanie z czarem.
Komponent mo�na ustawia� standardowymi funkcjami OLC, dodatkowo SITABLE pozwala ogl�da� tabel� powi�za� komponent�w z czarami
a SILIST pokazuje obiekty kt�re s� komponentami.
W parametrach postaci gracza zosta�a dodana bigflaga kt�ra definiuje znajomo�� poszczeg�lnych komponent�w.
Wa�ne miejsca:
spell_items_table - ustawianie powi�zania komponent�w z czarami, aby obiekt by� komponentem powi�zanym z czarem pole spell_item_name musi by� w obj->name
Dodane komendy
set SIKnow - ustawianie znajomo�ci komponent�w przez posta�
set object - ustawianie parametr�w komponentowych obiektu
SITable - pokazywanie tabeli powi�za� komponent�w z czarami
SIList - pokazywanie listy obiekt�w ustawionych jako komponenty
spell <nazwa_czaru> - pokazuje komponenty jakie znamy do czaru <nazwa_czaru>
Dzia�anie:
Aby u�y� komponentu musimy mie� o nim wiedz� z ksi�gi. Najpierw musimy nauczy� si� czaru a p�niej uczymy si� komponent�w dla tego czaru.
Nast�pnie do rzucania niekt�rych czar�w s� u�ywane komponenty, maj� r�ne dzia�anie, mo�e by� te� zdefiniowana grupa komponent�w kt�ra dzia�a
jedynie gdy s� obecne wszystkie sk�adniki grupy. Je�li mamy wi�cej ni� jeden przydatny komponent to wybierany jest losowy.
Komponenty maj� okre�lon� ilo�� �adunk�w (lub niesko�czon�) i okre�lony czas �ycia. Po odpaleniu ostatniego �adunku komponent mo�e znikn��.

Proces dodawania komponentu:

1. Okre�lamy sn czaru do jakiego chcemy doda� komponent
2a. Je�li wycinany to dodajemy w const.c do tabeli 'body_parts_table' i w skills.c w 'carve_do_spec_fun' nadajemy cz�ci w�a�ciwo�ci komponentowe
2b. Je�li jest na mudzie to w oedit nadajemy komend� komponent w�a�ciwo�ci komponentowe
3. W const.c w tabeli 'spell_items_table' dodajemy komponent
4. W definicji danego czaru wywo�ujemy funkcj� 'spell_item_check' i po zwr�ceniu TRUE (ma niezb�dny komponent) lub FALSE (nie ma) okre�lamy stosown� akcj�.

* rellik: mining [20080914]
Normalny skill, potrzebny kilof (r�ne klasy) i wydobywa si� kruszec.

v doda� narz�dzia, v0 - jako�� narz�dzia 1-9, v1 - poziom skilla od kt�rego mo�na u�ywa�, v2 - typ narz�dzia ( z tool_table )
v doda� sam skill
v doda� tabel� materia��w i makro
v doda� w roomach tablel� prawdopodobie�stwa wypadni�cia grudki danego materia�u, doda� w olc ustawianie tego i zapisywanie w plikach
v Mo�na wprowadzi� "pojemno��" rooma, czyli ile bry�ek/warto�ci mo�na wyku� zanim �y�a si� wyczerpie (do resetu pewnie)
v zr�nicowa� komunikaty podczas wydobywania
v doda� uczenie si� skilla
v doda� pole capcity_now do room i przy resecie przepisywa� z capcity bo cholera pami�ta aktualn� warto��
- zrobi� zamiast wait_print to wait_function i tam wypisywac i kontynuowa� (jako parametr ile jeszcze wywo�a�) je�li warunki spe�nione (np. posta� nie walczy) i waita nak�ada� tylko do nast�pnego sprawdzenia
- helpy dorobi�
v doda� m�czenie si� przy wydobywaniu (spadek mv)
v jakby narz�dzia sie tak psu�y jak bronie
v w try trzeba by te� dorobi� narz�dzia i sprawdza� poziom skilla, informuje je�li za niski i m�wi od jakiego poziomu mo�na
v sprawdza� sector_type �eby w wodzie albo lesie nie kuli

Builderzy:
- doda� kilofy
- poustawia� lokacje
- doda� skill do nauczycieli
- poustawia� bry�ki i vnumy przepisa� do merc
- poustawia� �eby naprawiali te� narz�dzia

* Nil: steel_skin [20100601]
Szuka� w kodzie po: steel_skin
Wszystkie zmiany skomentowane.
W merc.h jest define STEEL_SKIN_ON, mo�na to zakomentowa� i wy��czy ten spell.
Przerobiony steel skin. Na razie dzia�a analogicznie jak stone skin. Poziom jest ustawiony na 25.
Obni�a AC o 30. Przy uderzeniu tarcz� jest b. du�a szansa na wywrotk� (bez waita).
Mo�na rzuca� na innych

*/

//TODO: zrobi� wy�wietlanie sitable jako� po ludzku
/*TODO: (rellik#) potestowa� komponenty ostatnie
 * czy �o��d� gdzie� si� �aduje
 */
