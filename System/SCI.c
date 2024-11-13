/*
 * SCI.c
 *
 *  Created on:
 *      Author:
 */


/* **************** MODULE DESCRIPTION *************************

Ta modul implementira serijski komunikacijski vmesnik (SCI) na sistemskem nivoju.
SCI vmesnik nudi serijsko povezavo z osebnim računalnikom preko periferne USART enote.
Z drugumi besedami: SCI vmesnik je implementiran z USART vmesnikom (lahko bi namreč
uporabili kak drug serijski vmesnik za implementacijo SCI vmesnika).

Podprta sta dva tipa prenosa podatkov preko SCI:

	1. prenos znakov (uporablja se za prenos besedilnih sporočil),

	2. prenos binarnih podatkov (uporablja se za prenos binarnih podatkov).


Poleg tega sta na voljo dve vrsti funkcij za pošiljanje "send":

	1. za pošiljanje enega samega znaka ali enega bajta podatkov

	2. za pošiljanje niza znakov ali zaporedja bajtov


POZOR: za uspešno uporabo modula je potrebno predhodno poskrbeti
za ustrezno nizko-nivojsko inicializacijo USART vmesnika.

************************************************************* */




// ----------- Include other modules (for private) -------------

#include "SCI.h"

#include <stdio.h>			// dodamo printf() funkcionalnost



// --------- Dodati je potrebno knjižnico za delo z bufferji:

// S pomočjo modula "buf.c" bomo lahko implementirali SCI sprejemni (RX) ciklični medpomnilnik,
// ki bo hranil sprejete, a še neobdelane podatke, ter SCI oddajni (TX) medpomnilnik, ki pa bo
// hranil podatke, ki jih je še potrebno poslati.

	#include "buf.h"










// ---------------------- Private definitions ------------------


// ------ Definicje v zvezi z SCI -------


// Definiramo tip "handle" strukture "SCI_handle_t", ki jo bomo uporabili za upravljanje SCI.
// V strukturi je pravzaprav potrebno pomniti le, katera USART periferna enota bo uporabljena.

typedef struct
{
	USART_TypeDef *		USART;

} SCI_handle_t;



// In sedaj na podlagi zgoraj definiranega tipa definiramo še globalno strukturno spremenljivko "SCI".



SCI_handle_t SCI;



// ------ SCI receive (RX) buffer definitions -------

// Pripraviti je potrebno podatkovne strukture, s katerimi bomo implementirali
// sprejemni medpomnilnik.

// Definirajmo makro parameter, ki bo specificiral dolžino medpomnilnika.
#define SCI_RX_BUF_LEN 	512

// In definirajmo še dve podatkovni strukturi, ki sta potrebni za implementacijo
// medpomnilnika.
uint8_t SCI_RX_buffer[SCI_RX_BUF_LEN];			// tabela bajtov, ki bo hranila podatke
buf_handle_t SCI_RX_buf_handle;			// "handle" struktura za RX medpomnilnik




// ------ SCI transmit (TX) buffer definitions -------

// Pripraviti je potrebno podatkovne strukture, s katerimi bomo implementirali
// sprejemni medpomnilnik.

// Definirajmo makro parameter, ki bo specificiral dolžino medpomnilnika.
#define SCI_TX_BUF_LEN 	512

// In definirajmo še dve podatkovni strukturi, ki sta potrebni za implementacijo
// medpomnilnika.
uint8_t SCI_TX_buffer[SCI_TX_BUF_LEN];			// tabela bajtov, ki bo hranila podatke
buf_handle_t SCI_TX_buf_handle;			// "handle" struktura za TX medpomnilnik




// ------ printf() customization definitions -----


// V zgornjem delu SCI.c datoteke definiramo prototip funkcije
//
//		int _write(int file, char *ptr, int len)
//
// s pomočjo katere bomo lahko priredili uporabo printf() funkcije tako, da bo
// sporočila pošiljala preko SCI vmesnika (tj. redefinirali bomo pošiljanje sporočil
// preko standardnega izhoda "stdout").
int _write(int file, char *ptr, int len);







// -------------- Public function implementations --------------


// Funkcija SCI_init() poskrbi za inicializacijo SCI "handle" strukture SCI
// in modificira dolžino medpomnilnika printf() funkcije.
void SCI_init(void)
{


	// 0. Poskrbite, da je ustrezna nizko-nivojska strojna oprema že inicializirana

			// Za inicializacijo USART vmesnika, ki ga nameravamo uporabiti za implementacijo
			// SCI komunikacije, je poskrbljeno s pomočjo avtomatsko generirane CubeMX programske kode
			// znotraj funkcije MX_USARTx_UART_Init() v "main.c"!



	// 1. Določite, katera USART periferna enota bo uporabljena za implementacijo SCI vmesnika.

		// USART vmesnik specificiramo s pred-definiranim makrojem iz LL knjižnice, podobno kot
		// smo to storili za specifikacijo GPIO porta v prejšnjih vajah.

			// DOPOLNI
			SCI.USART = USART3;


	// 2. modifikacija printf() medpomnilnika

		// Dolžino medpomnilnika, ki se uporablja za pošiljanje sporočil na
		// standardni izhod "stdout" s pomočjo printf() funkcije,
		// bomo nastavili na 0. Na ta način dosežemo najboljšo odzivnost
		// printf() funkcije, saj se tako ne čaka, da se medpomnilnik napolne,
		// preden se pošlje sporočilo. Sporočila bodo tako poslana takoj, ko se
		// prejme vsaj en znak.
		setvbuf(stdout, NULL, _IONBF, 0);

		// Opomba: uporabimo funkcijo setvbuf() iz modula "stdio.h" iz t.i. "newlib" knjižnice,
		//
		//		int setvbuf(FILE *fp, char *buf, int mode, size_t size);
		//
		// s katero specificiramo dolžino medpomnilnika za standardni izhod "stdout" (prvi argument funkcije).
		// Za 3. vhodni parameter uporabimo že pred-definirani makro _IONBF (I/O No Buffer), ki bo povzročil,
		// da se za standardni izhod "stdout" ne uporablja medpomnilnika, temveč
		// se znake takoj pošlje na standardni izhod.


		// ----- Dodati je potrebno še dva koraka v inizializaciji:



		// --------- Dodati je potrebno definicije v zvezi s sistemskimi SCI medpomnilniki:








			// 3. Inicializacija SCI medpomnilnikov  (RX and TX)
				BUF_init( &SCI_RX_buf_handle, SCI_RX_buffer, SCI_RX_BUF_LEN);	// RX SCI medpomnilnik
				BUF_init( &SCI_TX_buf_handle, SCI_TX_buffer, SCI_TX_BUF_LEN);	// TX SCI medpomnilnik


		    // 4. Omogočitev prekinitev ob prejemu novega podatka

				// Kot zadnji korak inicializacije SCI vmesnika poskrbimo, da se vklopijo
				// prekinitve ob sprejemu novega podatka presko USART enote, s katero
				// implementiramo SCI vmesnik. Uporabimo seveda primerno LL funkcijo,
				// ki omogoči prekinitve ob postavitvi RXNE zastavice.



				LL_USART_EnableIT_RXNE_RXFNE (SCI.USART);


				// V vednost: prekinitve ob sprejemu novega podatka lahko vklopimo šele,
				// ko že imamo pripravljen sprejemni RX medpomnilnik, kamor se bodo ob
				// prekinitvah shranjevali novoprejeti podatki.






}







// ------ Funkcije za pošiljanje enega samega bajta podatkov -------



// Funkcija SCI_send_char() pošlje en sam ZNAK preko SCI vmesnika (uporabno, kadar delamo z besedilnimi sporočili).
// Ker SCI vmesnik implementiramo z USART vmesnikom, je potrebno pravzaprav znak poslati z oddajnikom USART enote.
// Pri tem si pomagamo z LL funkcijo.
void SCI_send_char(char c)
{
	// S čakanjem v zanki zagotovimo, da lahko USART oddajnik sprejme nov znak v oddajni register.
	// Za ugotavljanje, ali je že mogoče pisati v oddajni register USART vmesnika, uporabimo ustrezno
	// nizko-nivojsko LL funkcijo za delo z USART-om.
	while( ! LL_USART_IsActiveFlag_TXE_TXFNF (SCI.USART) )
	{
	}

	// In nato z uporabo USART vmesnika pošljemo en znak "c". Za pošiljane podatka preko USART vmesnika
	// prav tako uporabimo ustrezno nizko-nivojsko LL funkcijo.

	LL_USART_TransmitData8 (SCI.USART, c);

}



// Funkcija SCI_send_byte() pošlje en sam BAJT preko SCI vmesnika (uporabno, kadar pošiljamo binarne podatke).
// Ker SCI vmesnik implementiramo z USART vmesnikom, je potrebno pravzaprav bajt podatkov poslati z oddajnikom USART enote.
// Pri tem si pomagamo z LL funkcijo.
void SCI_send_byte(uint8_t data)
{
	// S čakanjem v zanki zagotovimo, da lahko USART oddajnik sprejme nov bajt v oddajni register.
	// Za ugotavljanje, ali je že mogoče pisati v oddajni register USART vmesnika, uporabimo ustrezno
	// nizko-nivojsko LL funkcijo za delo z USART-om.

	while( ! (LL_USART_IsActiveFlag_TXE_TXFNF (SCI.USART)))
	{
	}


	// In nato z uporabo USART vmesnika pošljemo en bajt podatkov "data". Za pošiljane podatka preko USART vmesnika
	// prav tako uporabimo ustrezno nizko-nivojsko LL funkcijo.

		LL_USART_TransmitData8 (SCI.USART,data);
}







// ------ Funkcije za pošiljanje niza znakov ali zaporedja bajtov -------

// Te funkcije lahko enostavno implementiramo s pomočjo funkcij za pošiljanje
// enega samega znaka oziroma bajta podaktkov, ki smo jih definirali zgoraj.


// Funkcija SCI_send_string() pošlje znakovni niz (tj. string) preko SCI vmesnika.
void SCI_send_string(char *str)
{
	// Pomožna spremenljivka - indeks znaka, ki ga pošiljamo.
	uint32_t i = 0;

	// Pošlji celoten niz znakov tako, da pošiljaš znak za znakom
	// vse dokler ne naletiš na vrednost 0 (null terminated string).
	// Premislite: "null terminating" znaka ni potrebno pošiljati preko SCI vmesnika.
	while( str[i] != '\0' )
	{
		SCI_send_char(str[i]);
		i++;
	}


}




// Funkcija SCI_send_bytes() pošlje zaporedje bajtov preko SCI vmesnika.
// Lokacija podatkov zapošiljanje je podana z vhodnim argumentom "*data",
// dolžina podatkov za pošiljanje pa je podano z vhodnim argumentom "size".
void SCI_send_bytes(uint8_t *data, uint32_t size)
{
	for( int i = 0 ; i < size ; i++ )
	{

		SCI_send_byte(data[i]);

	}
}










// ------ Funkcije za sprejemanje enega samega bajta podatkov -------


// Najprej si pripravimo pomožno funkcijo, s katero bomo lahko preverjali, ali je v SCI vmesniku
// na voljo nov prejeti podatek.


// Funkcija SCI_is_data_waiting() preveri, če je v SCI vmesniku na voljo nov prejeti podatek.
// Ker SCI vmesnik implementiramo z USART vmesnikom, je potrebno pravzaprav preveriti, ali je v sprejemniku USART-a
// na voljo nov podatek.
uint8_t SCI_is_data_waiting(void)
{
	// V premislek: LL funkcija, ki jo boste uporabili za preverjanje prejetega podatka vrača vrednost
	// tipa "uint32_t", sistemska funkcija SCI_is_data_waiting() pa vrača vrednost tipa uint8_t.
	// Torej boste imeli opravka s pretvorbo tipov.

	return (uint8_t) LL_USART_IsActiveFlag_RXNE_RXFNE (SCI.USART);

}




// Funkcija SCI_read_char() POSKUSI prebrati en ZNAK iz SCI vmesnika in
// ga shraniti na lokacijo, ki jo določa vhodni argument funkcije *c.
// Ker SCI vmesnik implementiramo z USART vmesnikom, je potrebno pravzaprav podatek prebrati iz
// sprejemnika USART vmesnika. Poskus branja podatka iz SCI vmesnika je
// lahko uspešen (return code = SCI_NO_ERROR) ali pa neuspešen (return code = SCI_ERROR).
SCI_rtrn_codes_t SCI_read_char(char *c)
{
	// Najprej preverimo, če sploh je v SCI vmesniku na voljo nov prejeti podatek.
	// Uporabimo zgoraj definirano SCI funkcijo.
	if ( SCI_is_data_waiting() )
	{

		// In če je na voljo nov prejeti podatek, ta podatek preberemo
		// in ga shranimo na specificirano mesto.
		// Preberemo ga seveda iz USART vmesnika s pomočjo LL funkcije.

		*c = LL_USART_ReceiveData8 (SCI.USART);


		// In ker je bilo branje novega prejetega podatka uspešno, vrnemo
		// kodo SCI_NO_ERROR.
		return SCI_NO_ERROR;

	}
	else
	{
		// Če pa ni na voljo novega prejetega podatka, pa vrnemo kodo SCI_ERROR
		// in ne spreminjamo vrednosti spremenljivke *c.
		return SCI_ERROR;
	}

}





// Funkcija SCI_read_byte() POSKUSI prebrati en BAJT iz SCI vmesnika in
// ga shraniti na lokacijo, ki jo določa vhodni argument funkcije *data.
// Ker SCI vmesnik implementiramo z USART vmesnikom, je potrebno pravzaprav podatek prebrati iz
// sprejemnika USART vmesnika. Poskus branja podatka iz SCI vmesnika je
// lahko uspešen (return code = SCI_NO_ERROR) ali pa neuspešen (return code = SCI_ERROR).
SCI_rtrn_codes_t SCI_read_byte(uint8_t *data)
{
	// Najprej preverimo, če sploh je v SCI vmesniku na voljo nov prejeti podatek.
	// Uporabimo zgoraj definirano SCI funkcijo.
	if ( SCI_is_data_waiting() )
	{

		// In če je na voljo nov prejeti podatek, ta podatek preberemo
		// in ga shranimo na specificirano mesto.
		// Preberemo ga seveda iz USART vmesnika s pomočjo LL funkcije.

		*data = LL_USART_ReceiveData8 (SCI.USART);


		// In ker je bilo branje novega prejetega podatka uspešno, vrnemo
		// kodo SCI_NO_ERROR.
		return SCI_NO_ERROR;

	}
	else
	{
		// Če pa ni na voljo novega prejetega podatka, pa vrnemo kodo SCI_ERROR
		// in ne spreminjamo vrednosti spremenljivke *data.
		return SCI_ERROR;
	}
}








// -------------- Private function implementations -------------



// ----------- printf() customization ----------


// Na tem mestu pa REDEFINIRAMO "newlib" funkcijo _write(). Funkcija _write()
// se namreč uporablja za pošiljanje sporočil s printf() funkcijo. S tem ko
// redefiniramo _write() funkcijo pravzaprav priredimo uporabo printf() funkcije
// tako, da pošilja sporočila na točno tak način, kakor to sami želimo.
// In mi želimo uporabljati funkcijo printf() tako, da bomo s pomočjo nje
// pošiljali formatirana sporočila preko SCI vmesnika.
int _write(int file, char *ptr, int len)
{
	// Vhodna argumenta _write funkcije "*ptr" in "len" specificirata,
	// kje se nahajajo podatki, ki jih printf() funkcija želi poslati
	// ter kako dolgi so ti podatki (v smislu števila bajtov).

	// Sedaj pa uporabimo našo SCI sistemsko funkcijo SCI_send_bytes()
	// da z njo pošljemo te podatke, na katere kaže "ptr".
	// Ker naša funkcija SCI_send_bytes() uporablja drugačen tip
	// vhodnih argumentov, je potrebno poskrbeti za eksplicitno
	// pretvorbo med tipi (angl. type-casting).
	SCI_send_bytes( (uint8_t*) ptr, (uint32_t) len );



	// Funkcija _write() mora vrniti število uspešno poslanih
	// znakov. Tu poenostavimo in privzamemo, da je bilo uspešno
	// poslanih vseh "len" znakov.
	return len;
}


// --------------------------------------------------------------











// ------- Testne funkcije ---------


// Funkcija SCI_demo_Hello_world() naj preko SCI vmesnika pošlje
// klasično sporočilo "Hello world!".
void SCI_demo_Hello_world(void)
{
	//SCI_send_char('a');

	SCI_send_string("Hello world!");
}



// Funkcija SCI_demo_Hello_world() naj preko SCI vmesnika pošlje
// sporočilo "Hello world!" s pomočjo prirejene funkcije printf().
// Sporočila naj bodo izpisana vsaka v svojo vrstico in naj bodo
// sledeče oblike:
//
//		1 : Hello printf() world!
//		2 : Hello printf() world!
//		3 : Hello printf() world!
//
// in tako dalje izpišite 10 vrstic.
// Sporočila naj se pošiljajo na vsako sekundo.
void SCI_demo_Hello_world_printf(void)
{
	for(int i=0; i<10 ; i++)
	{
		printf("%d : Hello printf() world!\n",i+1);

		HAL_Delay(1000);
	}
}




// Funkcija SCI_demo_echo_with_polling() naj demonstrira t.i. "echo funkcionalnost"
// s pristopom poizvedovanja (angl. polling).
//
// To pomeni da funkcija vsake toliko časa, ko ima na vojo "procesorski čas",
// preveri, če je bil preko SCI vmesnika sprejet nov znak in če je bil,
// naj ta znak takoj pošlje preko SCI vmesnika nazaj. Tako ste pravzaprav
// naredili nekakšen "odmev znakov", ki prihajajo preko SCI vmesnika.
// Demonstracija bo ključnega pomena, saj bomo z njo lahko pokazali
// pomembno težavo, ki jo prinese pristop poizvedovanja (angl. polling).
void SCI_demo_echo_with_polling(void)
{

	// pomožna spremenljivka, kamor bomo shranili prejeti znak
	char c;


	// spremenljivke, s pomočjo katerih bomo simulirali "zaposlenost procesorja" ("CPU busy")
	float a_0 = 0.5;
	float a_n;
	float sum = 0;

	#define N_MAX		50


	// V neskončni zanki
	while(1)
	{

		// s tehniko poizvedovanja ("polling") prevrejamo, ali je preko SCI vmesnika prispel
		// nov znak
		if ( SCI_read_char(&c)  == SCI_NO_ERROR )
		{
			// in če je prispel, ga takoj pošljemo preko SCI vmesnika nazaj (tj. "echo").

			SCI_send_string(&c);
		}



		// Potem pa simuliramo "zaposlenost procesorja" ("CPU busy") z drugimi opravili.
		// V našem primeru bomo to simulirali tako, da bomo računali privh N_MAX členov
		// geometrijske vsote.
		//
		// Na začetku, ko preverjate, kako dobro deluje "echo funkcionalnost",
		// pustite N_MAX nastavljen na 50. Kasneje pa lahko preverite, kaj se
		// dogaja, ko vrednost N_MAX večate oziroma manjšate. Lahko tudi poskusite zakomentirati
		// spodnjo kodo.

		a_n = a_0;
		for(uint32_t n=0; n<N_MAX; n++)
		{
			 sum = sum + a_n;
			 a_n = a_n * a_0;
		}


	}

}


// ---------- Dopolniti je potrebno SCI_init():


// --- Spremeniti je potrebno komentar za opis vsebine SCI_init() funkcije:


// Funkcija SCI_init() poskrbi za inicializacijo "handle" strukture SCI,
// nastavi dolžino medpomnilnika printf() funkcije, poskrbi za inicializacijo
// sprejemnega RX in oddajnega TX sistemskega medpomnilnika za SCI vmesnik
// in omogoči prekinitve v zvezi s sprejemom podatkov.









// ---------- Dodati je potrebno funkciji za pošiljanje več podatkov s pomočjo prekinitev:


// ------ Funkcije za pošiljanje niza znakov ali zaporedja bajtov s pomočjo prekinitev -------


// Funkcija SCI_send_string_IT() pošlje znakovni niz (tj. string) preko SCI vmesnika s pomočjo prekinitev.
void SCI_send_string_IT(char *str)
{
	// pomožni spremenljivki
	uint32_t i = 0;		// indeks
	uint8_t c;			// hrani znak, ki ga bomo poskusili poslati takoj


	// Najprej je potrebno celoten znakovni niz "*str" shraniti v oddajni medpomnilnik.
	// Z "while" zanko se sprehodimo preko vseh znakov niza
	while( str[i] != '\0')
	{
		// in jih enega po enega shranjujemo v oddajni mepodmnilnik.
		BUF_store_byte( &SCI_TX_buf_handle, (uint8_t) str[i] );

		// Indeks se premakne na naslednji znak v nizu.

			i++;
	}


	// Če je potrebno poslati vsaj en znak,
	if ( i > 0 )
	{

		// najprej preverimo, če lahko prvi znak za pošiljanje vpišemo
		// v oddajni podatkovni register TDR že kar takoj. Uporabimo
		// ustrezno LL funkcijo, ki nam vrne to informacijo.
		if ( LL_USART_IsActiveFlag_TXE_TXFNF(SCI.USART) )		// TX empty?
		{
			// Če oddajnik lahko sprejme nov znak, potem ta prvi znak
			// najprej preberemo iz oddajnega medpomnilnika v pomožno
			// spremenljivko "c".
			BUF_get_byte( &SCI_TX_buf_handle, &c );

			// in nato ta znak s pomočjo LL funkcije takoj vpišemo
			// v oddajni podatkovni register.

			LL_USART_TransmitData8(SCI.USART, c);

			// V vednost: pisanje v oddajni podatkovni register bo avtomatsko zbrisalo
			// TXE zastavico.

		}

		// Z zgornjo kodo smo zagovotili, da se trenutno v oddajnem podatkovnem
		// registru zagovoto nahaja nov podatek, ki se bo kmalu poslal.

		// In sedaj ključni del: ko se bo nov podatek pričel pošiljati, bo
		// oddajni podatkovni register spet prost (TXE) in takrat lahko vanj
		// takoj vpišemo naslednji podatek, ki ga želimo poslati iz oddajnega medpomnilnika.
		// In ta vpis naslednjega podatka bomo opravili s pomočjo prekinitvene rutine.
		//
		// Da pa bo sploh prišlo do prekinitve, ko bo oddajni register spet prost,
		// je pa potrebno te prekinitve najprej omogočiti. To storimo z LL funkcijo
		// tu spodaj.

		LL_USART_EnableIT_TXE_TXFNF (SCI.USART);

	}

}




// Funkcija SCI_send_bytes_IT() pošlje zaporedje bajtov preko SCI vmesnika s pomočjo prekinitev.
// Lokacija podatkov zapošiljanje je podana z vhodnim argumentom "*data",
// dolžina podatkov za pošiljanje pa je podano z vhodnim argumentom "size".
void SCI_send_bytes_IT(uint8_t *data, uint32_t size)
{
	// pomožna spremenljivka za hranjenje enega bajta podakov
	uint8_t	d;


	// Najprej je potrebno celotno zaporedje bajtov z naslova "data" shraniti v oddajni medpomnilnik.
	// To storimo s "for" zanko.
	for(uint32_t i=0; i < size ; i++ )
	{
		BUF_store_byte( &SCI_RX_buf_handle, (uint8_t) data[i] );
	}


	// Če je potrebno poslati vsaj en znak,
	if ( size > 0 )
	{
		// najprej preverimo, če lahko prvi bajt za pošiljanje vpišemo
		// v oddajni podatkovni register TDR že kar takoj. Uporabimo
		// ustrezno LL funkcijo, ki nam vrne to informacijo.
		if ( LL_USART_IsActiveFlag_TXE_TXFNF(SCI.USART) )		// TX empty?
		{
			// Če oddajnik lahko sprejme nov bajt, potem ta prvi bajt
			// najprej preberemo iz oddajnega medpomnilnika v pomožno
			// spremenljivko "d".
			BUF_get_byte( &SCI_TX_buf_handle, &d );

			// in nato ta bajt s pomočjo LL funkcije takoj vpišemo
			// v oddajni podatkovni register.

			LL_USART_TransmitData8(SCI.USART, d);

			// V vednost: pisanje v oddajni podatkovni register bo avtomatsko zbrisalo
			// TXE zastavico.
		}


		// Z zgornjo kodo smo zagovotili, da se trenutno v oddajnem podatkovnem
		// registru zagovoto nahaja nov podatek, ki se bo kmalu poslal.

		// In sedaj ključni del: ko se bo nov podatek pričel pošiljati, bo
		// oddajni podatkovni register spet prost (TXE) in takrat lahko vanj
		// takoj vpišemo naslednji podatek, ki ga želimo poslati iz oddajnega medpomnilnika.
		// In ta vpis naslednjega podatka bomo opravili s pomočjo prekinitvene rutine.
		//
		// Da pa bo sploh prišlo do prekinitve, ko bo oddajni register spet prost,
		// je pa potrebno te prekinitve najprej omogočiti. To storimo z LL funkcijo
		// tu spodaj.

			LL_USART_EnableIT_TXE_TXFNF (SCI.USART);
	}

}






// ------------ Dodati je potrebno "callback" funkcije za pošiljanje in sprejemanje enega samega znaka:



// ------- "callback" funkcije, ki implementirajo prekinitvene rutine ---------

// Funkcija SCI_receive_char_Callback() je "callback" funkcija, ki jo bomo poklicali
// ob prekinitvah, ko bo sprejet nov znak preko USARTa, s katerim implementiramo sistemski SCI vmesnik.
void SCI_receive_char_Callback(void)
{
	// pomožna spremenljivka za novo-sprejeti podatek
	uint8_t received_data;

	// Ko preko USART vmesnika prejmemo nov podatek, ga shranimo v sprejemni RX medpomnilnik SCI vmesnika.


	// Najprej z uporabo LL funkcije preberemo novo-sprejeti podatek in ga shranimo v pomožno spremenljivko.
	received_data = LL_USART_ReceiveData8 (SCI.USART);

	// Nato pa ta podatek shranimo v sprejemni RX medpomnilnik SCI vmesnika za nadaljno obdelavo kasneje.
	BUF_store_byte( &SCI_RX_buf_handle, received_data );
}






// Funkcija SCI_transmit_char_Callback() je "callback" funkcija, ki jo bomo poklicali
// ob prekinitvah, ko bo oddajni register USART vmesnika prost (tj. bo lahko sprejel nov podatek).
void SCI_transmit_char_Callback(void)
{
	// pomožna spremenljivke
	uint8_t 			data_to_transmit;		// sem shranimo podatek, ki ga želimo poslati
	buf_rtrn_codes_t	return_code;			// sem shranimo rezultat poskusa branja iz medpomnilnika


	// Najprej poskusimo prebrati naslednji podatek, ki ga želimo poslati.
	// Zapomnimo si "vrnjeno kodo" (angl. return code), ki jo vrne BUF_ funkcija.
	return_code = BUF_get_byte( &SCI_TX_buf_handle, &data_to_transmit );

	// S pomočjo "vrnjene kode" ugotovimo, če sedaj imamo na voljo naslednji podatek za pošiljanje.
	if ( return_code == BUFFER_OK )
	{
		// In če je na voljo naslednji podatek, ga kar vpišemo v oddajni podatkovni register
		// USART vmesnika s pomočjo LL funkcije.

		LL_USART_TransmitData8 (SCI.USART, data_to_transmit);
	}


	// Po vsakem podatku, ki ga pošljemo, je potrebno preveriti, če smo morda
	// poslali zadnji podatek. To je pomembno, saj moramo v tem primeru ustaviti
	// "avtomatsko" pošiljanje podatkov s pomočjo prekinitev.
	if ( BUF_get_data_size( &SCI_TX_buf_handle ) == 0)
	{
		// In če smo res poslali zadnji podatek, potem moramo onemogočiti nadaljne
		// prekinitve ob sprostitvi oddajnega podatkovnega registra (TXE),
		// saj nimamo več kaj pošiljati. Tako onemogočimo avtomatsko
		// pošiljanje iz oddajnega TX medpomnilnika SCI vmesnika s pomočjo prekinitev.
		// Uporabimo ustrezno LL funkcijo za onemogočitev prekinitve.

		LL_USART_EnableIT_TXE_TXFNF (SCI.USART);
	}

}








// ---------- Dodati je potrebno nove testne funkcije za demonstracijo uporabe prekinitev:


// Funkcija SCI_demo_receive_with_interrupts() demonstrira sprejem podatkov s pomočjo prekinitev.
// To stori tako, da ves čas preverja sprejemni RX medpomnilnik SCI vmesnika, kjer se s pomočjo
// prekinitvene rutine shranjujejo sprejeti podatki. In če so v tem medpomnilnu na voljo novi
// podatki, jih pošlje takoj nazaj pošiljatelju (echo) bajt po bajt.
void SCI_demo_receive_with_interrupts(void)
{

	// pomožna spremenljivka
	uint8_t data;	// tu shranimo naslednji sprejeti podatek

	while(1)
	{
		// Preveri, če v sprejemnem RX medpomnilniku SCI vmesnika na voljo novi podatki.
		if ( BUF_get_data_size( &SCI_RX_buf_handle) )
		{
			// Če so, preberi naslednji sprejeti podatek in ga shrani v pomožno spremenljivko.
			BUF_get_byte( &SCI_RX_buf_handle, &data );

			// Nato pa ta podatek pošlji takoj nazaj s pomočjo SCI_send_byte() funkcije
			// (tj. brez uporabe prekinitev).

			SCI_send_byte(data);

		}
	}

}




// Funkcija SCI_demo_transmit_with_interrupts() demonstrira pošiljanje podatkov s pomočjo prekinitev.
// To stori tako, da za pošiljanje sporočila uporabi funkcijo SCI_send_string_IT().
void SCI_demo_transmit_with_interrupts(void)
{
	// Definiramo sporočilo, ki ga bomo poslali.
	char message[100] = "This was sent using USART interrupts!\n";

	// Nato pa pričnemo s pošiljanjem enega in istega sporočila.
	while(1)
	{
		// Sporočilo pošljemo z uporabo SCI funkcije za pošiljanja niza znakov s pomočjo prekinitev.

		SCI_send_string(message);

		// Nato pa počakamo eno sekundo (premor med sporočili).

		HAL_Delay(1000);

	}

}




// Funkcija SCI_demo_echo_with_interrupts() demonstrira "echo funkcionalnost" s pomočjo uporabe prekintev
// za sprejemanje in pošiljanje podatkov. Ideja testne funkcije je popolnoma enaka kot v primeru "polling demo"
// funkcije SCI_demo_echo_with_polling(), le da tokrat za sprejem podatkov poskrbijo prekinitve v navezi s sprejemnim
// RX medpomnilnikom SCI vmesnika. Pošiljanje podatkov nazaj pa je seveda tudi izvedeno s pomočjo prekinitev s
// funkcijo SCI_send_bytes_IT().
//tokrat izvedemo sprejem podatkov preko sprejemnega RX medpomnilnika
void SCI_demo_echo_with_interrupts(void)
{

	// spremenljivke, s pomočjo katerih bomo simulirali "zaposlenost procesorja" ("CPU busy")
	#define N_MAX_IT		500

	float a_0 = 0.5;
	float a_n;
	float sum = 0;


	// pomožna spremenljivke za tvorjenje sporočila, ki ga bomo poslali nazaj pošiljatelju
	uint8_t message[SCI_RX_BUF_LEN];
	uint32_t message_size;


	// V neskončni zanki
	while(1)
	{



		// preverjamo, koliko bajtov novo-sprejetih podatkov je na voljo v sprejemnem RX medpomnilniku SCI vmesnika.
		message_size = BUF_get_data_size( &SCI_RX_buf_handle);

		// In če je na voljo vsaj en bajt podatkov,
		if ( message_size > 0 )
		{

			// shranimo te novo-sprejete podatke v pomožno spremenljivko za sporočilo, ki ga bomo poslali nazaj.
			BUF_get_bytes( &SCI_RX_buf_handle, message, message_size );

			// In nato vsebino te pomožne spremenljivke pošljemo nazaj pošiljatelju z uporabo funkcije,
			// ki pošilja binarne podatke s pomočjo prekinitev.

			SCI_send_bytes(message, message_size);

		}




		// Potem pa simuliramo "zaposlenost procesorja" ("CPU busy") z drugimi opravili.
		// V našem primeru bomo to simulirali tako, da bomo računali privh N_MAX_IT členov
		// geometrijske vsote.
		//
		// Večajte število členov N_MAX_IT in opazujte, če povečana zaposlenost procesorja
		// še vedno vpliva na sprejem podatkov preko serijskega vmesnika. Napake v sprejemu
		// podatkov seveda opazujete v "echo" odzivu na terminalu.


		a_n = a_0;
		for(uint32_t n=0; n<N_MAX_IT; n++)
		{
			 sum = sum + a_n;
			 a_n = a_n * a_0;
		}


	}


}








