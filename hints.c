/*********************************************************************
 *                                                                   *
 * KILLER MUD is copyright 1999-2008 Killer MUD Staff (alphabetical) *
 *                                                                   *
 *   ZMIENIA�E� CO�? DOPISZ SI�!                                     *
 *                                                                   *
 *   Pietrzak Marcin     (marcin.pietrzak@mud.pl        ) [Gurthg  ] *
 *   Sawicki Tomasz      (furgas@killer-mud.net         ) [Furgas  ] *
 *   Trebicki Marek      (maro@killer.radom.net         ) [Maro    ] *
 *                                                                   *
 *********************************************************************/
/* $Id: hints.c 110 2013-07-18 08:34:31Z gurthg $ */
#if defined (macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"

void hint_update( void )
{
    CHAR_DATA * ch;
    CHAR_DATA *ch_next;

    for ( ch = char_list;ch != NULL;ch = ch_next )
    {
        ch_next = ch->next;

        if ( !IS_NPC( ch ) && IS_SET( ch->comm, COMM_HINT ) )
        {
            send_to_char( "{R[PORADA]: ", ch );
            switch ( number_range( 0, 52 ) )
            {
                default: send_to_char( "{YJe�li czego� nie wiesz, skorzystaj na pocz�tku z pomocy, komenda {Ghelp{Y (polskie: {Gpomoc{Y). Je�li nie znajdziesz tam odpowiedzi na swoje pytanie, mo�esz zada� je innym graczom - wpisz znak zapytania i tre�� pytania (np. ?jak napi� si� wody?).{x", ch ); break;
                case 1: send_to_char( "{YJe�li nie chcesz traci� ekwipunku, kiedy wychodzisz z gry musisz si� przej�� do jednej z gospod i wynaj�� pok�j. Wpisz {Grent{Y (polskie: {Gwynajmij{Y), kiedy ju� jeste� w gospodzie.{x", ch ); break;
                case 2: send_to_char( "{YKomenda {Goffer{Y (polskie: {Goferta{Y) poka�e ci ile b�dziesz p�aci<&�/�a/�o> za przechowywanie swojego ekwipunku w gospodzie. Mo�e okaza� si�, ze szkoda pieni�dzy na niekt�re jego cz�ci. Sprzedaj je wtedy lub te� po prostu wyrzu�.{x", ch ); break;
                case 3: send_to_char( "{YPami�taj, �e czasami, kiedy chodzisz w grupie z innymi graczami �atwiej jest sobie poradzi� z niekt�rymi potworami. Praca zespo�owa czyni cuda.{x", ch ); break;
                case 4: send_to_char( "{YJe�li nie masz wystarczaj�co du�o pieni�dzy na przechowywanie ekwipunku w gospodzie, pomy�l nad sprzedaniem go czy tez innymi podobnymi rozwi�zaniami. Mo�esz przechowywa� tylko tyle dni na ile ci� sta�, potem ca�y ekwipunek przepada.{x", ch ); break;
                case 5: send_to_char( "{YZawsze, kiedy chcesz si� zmierzy� z r�nymi potworami mo�esz wpisa� {Gconsider imi� potwora{Y (polskie: {Goce�{Y), (na przyk�ad {Gconsider troll{Y). Pozwoli ci to w przybli�eniu oceni� czy poradzisz sobie z jak�� besti� czy nie. Czasami po prostu nie warto ryzykowa� walki praktycznie z g�ry skazanej na pora�k�.{x", ch ); break;
                case 6: send_to_char( "{YJe�li nie wiesz jak u�y� jakiej� komendy spr�buj wpisa� {Ghelp komenda{Y (polskie: {Gpomoc{Y). Pomoc powinna mniej wi�cej podpowiedzie� ci do czego dana komenda s�u�y.{x", ch ); break;
                case 7: send_to_char( "{YSzybciej odnawiaj� si� twoje si�y �yciowe, kiedy jeste� najedzony i napity. Warto wiec zawsze mie� ze sob� jaki� pojemnik z wod� i prowiant.{x", ch ); break;
                case 8: send_to_char( "{YJe�li jeste� wypocz�ty lepiej walczysz, niewyspany stajesz si� nieprecyzyjny. Pami�taj o tym. Kiedy d�uga przygoda da ci czasami szans� odpoczynku, korzystaj z niego.{x", ch ); break;
                case 9: send_to_char( "{YJe�li widzisz, �e walka jest praktycznie dla ciebie przegrana spr�buj uciec walcz�cemu z tob� przeciwnikowi. Pomocna b�dzie komenda {Gflee{Y (polskie: {Guciekaj{Y).{x", ch ); break;
                case 10: send_to_char( "{YCzasami zwyczajnie nie warto ryzykowa� straty du�ej partii punkt�w do�wiadczenia, kiedy do uko�czenia kolejnego etapu przygody (poziomu do�wiadczenia) pozosta�o ci ju� naprawd� niewiele, ale... Wszystko zale�y od ciebie.{x", ch ); break;
                case 11: send_to_char( "{YJedna osoba nie mo�e gra� w jednym momencie dwiema postaciami. Je�li taki proceder zostanie wykryty spodziewaj si� kary... ��cznie z tymi najsurowszymi.{x", ch ); break;
                case 12: send_to_char( "{YPami�taj, �e imi� twojej postaci musi spe�nia� postawione przez tw�rc�w tego muda i umieszczone na etapie tworzenia postaci kryteria. Je�li ich nie przeczyta�e� - zr�b to.{x", ch ); break;
                case 13: print_char( ch, "{YTroche informacji o naszym mudzie, mudach w og�le i innych rzeczach, jakie mog� ci� zainteresowa� mo�esz znale�� na stronie naszego muda pod adresem: %s{x", WWW_ADDRESS ); break;
                case 14: send_to_char( "{YPami�taj, ze kiedy rozmawiasz z jednym z w�adc�w (Nie�miertelnych) tego muda mo�esz zachowywa� si� tak jak to uwa�asz za stosowne, ale... Pami�taj tez, �e i �w w�adca zrobi z tob� to, co uwa�a za stosowne i we�mie pod uwag� to jak si� zachowywa�e�. Jak wsz�dzie � chamstwo, wrzaskliwo�� i temu podobne niechlubne zachowania na pewno nie zostan� docenione.{x", ch ); break;
                case 15: send_to_char( "{YKiedy wpiszesz komend� {Gsave{Y (polskie: {Gzapisz{Y) dane twojej postaci zostan� zachowane na serwerze muda. Co jaki� czas jednak mud zachowuje te dane automatycznie.{x", ch ); break;
                case 16: send_to_char( "{YZapoznaj si� dok�adnie z komend� {Gconfig{Y (polskie: {Gopcje{Y). Pozwala ona wy�wietli� i ustawi� parametry gry oraz automatyczne zachowania Twojej postaci. Dok�adniejsze informacje znajduj� si� w pomocy do tej komendy ({Ghelp config{Y lub {Gpomoc opcje{Y).{x", ch ); break;
                case 17: send_to_char( "{YKiedy chcesz zaatakowa� jakiego� potwora wpisz {Gkill potw�r{Y (polskie: {Gzabij{Y). Nie zawsze b�dzie dane ci wygra�. Mierz si�y na zamiary. Ale... Jak zwykle, wszystko zale�y od ciebie.{x", ch ); break;
                case 18: send_to_char( "{YMo�esz zg�osi� liter�wk� za pomoc� komendy {Gtypo{Y (polskie: {Gliter�wka{Y).{x", ch ); break;
                case 19: send_to_char( "{YJe�li chcesz zg�osi� b��d wpisz {Gbug{Y (polskie: {Gb��d{Y) i kr�tki jego opis. {x", ch ); break;
                case 20: send_to_char( "{YJe�li masz jaki� pomys� wpisz {Gidea{Y i kr�tko wyja�nij na czym ten pomys� polega.{x", ch ); break;
                case 21: send_to_char( "{YKomenda {Geat{Y (polskie: {Gzjedz{Y) pozwoli ci spo�y� co� co znajduje si� w twoim inwentarzu. Niekt�re rzeczy mog� by� truj�ce, a trucizny z kolei �miertelne. Na og� jednak sporo rzeczy sprzedawanych w sklepach i na straganach zgodnie ze swoim przeznaczeniem do zjedzenia si� nadaje� Do czasu jednak, po pewnym czasie wi�kszo�� si� psuje.{x", ch ); break;
                case 22: send_to_char( "{YKiedy widzisz opis jakiego� kierunku w nawiasie zwyk�ym, na przyk�ad (E) oznacza to, ze w tym kierunku s� drzwi. Mo�esz spr�bowa� je otworzy� komend� {Gopen kierunek{Y (polskie: {Gotw�rz{Y). Do otwarcia niekt�rych drzwi jednak potrzebny b�dzie klucz.{x", ch ); break;
                case 23: send_to_char( "{YOtwarcie drzwi zamkni�tych na klucz nie jest rzecz� skomplikowan�. Wystarczy wpisa� {Gunlock kierunek{Y, b�d� te� {Gunlock nazwa drzwi{Y (polskie: {Godklucz{Y). Przedtem musisz jednak zdoby� odpowiedni klucz i trzyma� go w swoim inwentarzu.{x", ch ); break;
                case 24: send_to_char( "{YKiedy �pisz twoje si�y �yciowe szybciej do ciebie wracaj�. Korzystaj z tego.{x", ch ); break;
                case 25: send_to_char( "{YPami�taj, ze mo�esz by� zaatakowany, kiedy �pisz lub te� odpoczywasz w niebezpiecznych miejscach. O ile to w og�le mo�liwe pr�buj czasami spa� w miejscach o mniejszym stopniu ryzyka.{x", ch ); break;
                case 26: send_to_char( "{YKiedy odpoczywasz szybciej wracaj� ci si�y �yciowe. Nie tak szybko wprawdzie jak podczas snu, ale zawsze. Dodatkowo podczas odpoczynku profesje czaruj�ce mog� zapami�tywa� zakl�cia.{x", ch ); break;
                case 27: send_to_char( "{YNie ka�dy typ terenu jest dla ciebie r�wnie �atwy do pokonania. Wi�cej energii b�dzie trzeba w�o�y� na terenach g�rzystych ni� na prostej drodze. Jednak czasami mo�na dosi��� jakiego� wierzchowca, a czasami dzi�ki uprzejmo�ci jakiego� maga nawet polewitowa� w powietrzu.{x", ch ); break;
                case 28: send_to_char( "{YTwoja posta� b�dzie posiada�a wiele umiej�tno�ci. Na pocz�tku niekt�re ze wzgl�du na mizerny poziom wytrenowania nie na wiele si� zdadz�. Rozwijaj je, by� mo�e przydadz� si� w przysz�o�ci.{x", ch ); break;
                case 29: send_to_char( "{YKiedy chodzisz na bosaka twoje stopy s� podatniejsze na zranienie. Zawsze nawet najgorsze buciory mog� si� na co� przyda�.{x", ch ); break;
                case 30: send_to_char( "{YCz�� pieni�dzy mo�esz przechowywa� w banku. Zwyczaj taki mo�e ci� uratowa� przed �mierci� g�odow�, kiedy kto� ci� napadnie i ograbi na rozstajach dr�g.{x", ch ); break;
                case 31: send_to_char( "{YZawsze mo�esz skasowa� posta�, kt�ra ci� znudzi�a, b�d� tez sta�a si� dla ciebie bezu�yteczna (opcja kasowania postaci w menu przy wchodzeniu do gry).{x", ch ); break;
                case 32: send_to_char( "{YMniej si� m�czysz przemierzaj�c �wiat na grzbiecie wierzchowca. Niekt�re kosztuj�, ale... s� po�yteczne.{x", ch ); break;
                case 33: send_to_char( "{YKiedy u�ywasz r�nych swoich umiej�tno�ci zdobywasz w nich punkty wytrenowania. Je�li ich troch� uzbierasz mo�esz si� przej�� do odpowiedniego nauczyciela a on nauczy ci� o danej umiej�tno�ci czego� wi�cej.{x", ch ); break;
                case 34: send_to_char( "{YKiedy wychodzisz prawid�owo z muda i zachowujesz ekwipunek komend� {Grent{Y zapoznaj si� z informacj� na ile dni wystarczy ci pieni�dzy na przechowywanie i wr�� przed up�ywem tego terminu, aby ekwipunek ci nie przepad�.{x", ch ); break;
                case 35: send_to_char( "{YNiekt�re potwory s� zbyt silne aby walczy� z nimi samotnie. Zapoznaj si� z komend� {Ggroup{Y (help group) (polskie: {Ggrupa{Y) i spr�buj wybra� si� na takiego potwora w towarzystwie dru�yny.{x", ch ); break;
                case 36: send_to_char( "{YR�ne postaci posiadaj� odmienne umiej�tno�ci. Mo�e si� okaza�, �e po��czone umiej�tno�ci w dru�ynie s� w stanie zdzia�a� cuda.{x", ch ); break;
                case 37: send_to_char( "{YKiedy zginiesz twoje cia�o b�dzie le�a�o tam, gdzie kto� lub co� ci� zabi�o. Musisz wr�ci� po cia�o, aby odzyska� rzeczy jakie nosi�<&e�/a�/e�> przed �mierci�.{x", ch ); break;
                case 38: send_to_char( "{YCzasami mo�e si� zdarzy�, �e zginiesz z miejscu, z kt�rego nie ma wyj�cia (np. przepa�cie). Lepiej nie wracaj tam po sw�j ekwipunek chyba, �e chcesz zgin�� kolejny raz. Czasem trzeba pogodzi� si� ze strat� ca�ego dobytku.{x", ch ); break;
                case 39: send_to_char( "{YKiedy zginiesz i masz niewiele punkt�w do�wiadczenia mo�esz straci� poziom do�wiadczenia, punkty �yciowe i kilka innych rzeczy. Zachowuj ostro�no��.{x", ch ); break;
                case 40: send_to_char( "{YNie wykorzystuj b��d�w w grze, mo�esz za to ponie�� kar�. B��dy nale�y zg�asza� komenda {Gbug{Y (polskie: {Gb��d{Y).{x", ch ); break;
                case 41: send_to_char( "{YPami�taj, �e niekt�re produkty psuj� si�, kiedy d�ugo je nosisz ze sob� i nie zjadasz. Uwa�aj na to, gdy� mo�esz si� zatru�.{x", ch ); break;
                case 42: send_to_char( "{YKiedy u�ywasz jakiej� umiej�tno�ci rozwijasz j�, aby jednak zamieni� wszystkie twoje og�lne spostrze�enia i przyzwyczajenia na rzeczywist� wiedz� musisz przej�� si� do okre�lonego nauczyciela danej umiej�tno�ci i pobra� u niego nauki. S�u�y do tego komenda {Glearn{Y (polskie: {Gucz{Y).{x", ch ); break;
                case 43: send_to_char( "{YJe�li twoja profesja jest tak zwan� profesj� czaruj�c� mo�esz uczy� si� ni� r�nych zakl��. Niekt�rych z nich mo�esz nauczy� si� u nauczycieli porozrzucanych po ca�ym �wiecie. Niekt�rych trzeba naprawd� szuka�, szuka�, szuka�. Magia w ko�cu nie jest czym� pospolitym.{x", ch ); break;
                case 44: send_to_char( "{YUczenia si� danego czaru u danego nauczyciela mo�esz spr�bowa� tylko raz na poziom do�wiadczenia, je�li ci si� nie uda, nast�pna pr�ba odbywa si� dopiero na nast�pnym levelu. Mo�esz te� poszuka� innego nauczciela tego czaru.{x", ch ); break;
                case 45: send_to_char( "{YNie wszystkich czar�w mo�na si� nauczy� u nauczycieli, niekt�re znajdziesz w ksi�gach i zwojach, kt�re cz�sto b�dzie trudno znale�� i zdoby�. Do nauki czar�w z ksi�g s�u�y komenda {Gstudy{Y (polskie: {Gstudiuj{Y){x", ch ); break;
                case 46: send_to_char( "{YMagowie nie najlepiej nadaj� si� na �ywe tarcze, dlatego cz�sto b�d� potrzebowali do dru�yny wojownika czy innego wytrzyma�ego specjalisty od nadstawiania twarzy.{x", ch ); break;
                case 47: send_to_char( "{YDobry mag powinien troszczy� si� o osobnika, kt�ry broni go przed wszelkimi formami bezpo�redniego starcia. Czary ochronne powinny by� tu bardzo przydatne.{x", ch ); break;
                case 48: send_to_char( "{YGrupie wyprawiaj�cej si� w zapomniane przez �wiat ost�py, zamieszka�e przez tajemnicze i bardzo gro�ne bestie przyda si� kleryk, b�dzie leczy� rany w krytycznych chwilach i zajmowa� si� rannymi w czasie odpoczynku.{x", ch ); break;
                case 49: send_to_char( "{YBarbarzy�cy nie mog� co prawda nosi� wszystkich rodzaj�w zbroi i nie s� tak opancerzeni jak paladyni czy wojownicy, ale ich dziki hart ducha i wrodzona wytrzyma�o�� rekompensuj� te braki.{x", ch ); break;
                case 50: send_to_char( "{YDobra grupa jest jak palce jednej r�ki, warto czasami przemy�le� i skoordynowa� przysz�e dzia�ania, aby z sukcesem dokona� rzeczy niebezpiecznych.{x", ch ); break;
                case 51: send_to_char( "{YWarto pami�ta�, �e do�wiadczenie mo�na zdoby� nie tylko walcz�c i zabijaj�c. Rozwi�zywanie zada� mo�e si� nierzadko okaza� nie tylko op�acalne, ale i bardzo interesuj�ce.{x", ch ); break;
                case 52: send_to_char( "{YJe�li potrzebujesz pomocy mo�esz zada� pytanie bardziej do�wiadczonym graczom. Aby to zrobi� wpisz znak zapytania a za nim tre�� pytania (np. ?jak napi� si� wody?). Mo�e si� zdarzy�, �e w grze nie b�dzie nikogo kto zna�by odpowied� lub chcia�by odpowiedzie�. U�ywaj tej funkcji {Rz umiarem{Y.{x", ch ); break;
            }
            send_to_char( " {yAby wy��czy� porady wpisz: {Gconfig hints{Y (polskie: {Gopcje{Y).{x\n\r", ch );
        }
    }
    return ;
}
