/*
 * kbd.c
 *
 *  Created on:
 *      Author:
 */




/* **************** MODULE DESCRIPTION *************************

Ta modul implementira tipkovnico sistema na podlagi tipk, ki so
v sistemu razpoložljive. Modul implementira branje tipkovnice
("skeniranje"), detekcijo pritiska tipk in shranjevanje informacije
o pritisnjenih tipkah v medpomnilnik (angl. buffer).

POZOR: za uspešno uporabo modula je potrebno predhodno poskrbeti
za ustrezno nizko-nivojsko inicializacijo digitalnih GPIO vhodov.


************************************************************* */




// ----------- Include other modules (for private) -------------


#include "LED.h"		// vključimo LED modul za potrebe "keyboard demo" funkcije





// ---------------------- Private definitions ------------------




// -------- Keyboard definitions ----------


#include "kbd.h"


// Najprej pripravimo "handle" strukturo, v kateri bomo hranili vse potrebne parametre za
// delo s tipkovnico.

// "Handle" strukturo bomo definirali po principu "od spodaj navzgor" (angl. "bottom-up approach").


// Definirajmo najprej tip "button_handle_t" strukture, ki hrani vse potrebne parametre
// za delo z eno samo tipko.
typedef struct
{
	GPIO_TypeDef *		port;
	uint32_t 			pin;
	button_sig_value_t	state_old;
	button_sig_value_t	state_new;

} button_handle_t;





// Sedaj pa s pomočjo zgoraj definirane strukture button_handle_t za delo z eno samo tipko
// definirajmo tip "handle" strukture keyboard_handle_t, ki jo bomo uporabljali za delo s
// celotno tipkovnico.
typedef struct
{
	// tabela "handle" struktur za vse tipke v sistemu
		// DOPOLNI

	button_handle_t buttons[BTN_NUM];


} keyboard_handle_t;




// Vse to zgoraj so bile le DEFINICIJE ZGRADBE podatkovne strukture. Sedaj pa lahko končno ustvarimo
// še strukturno spremenljivko "keyboard", ki bo imela tako zgradbo in bo za nas hranila vse potrebne
// parametre za delo s tipkovnico.

keyboard_handle_t 		keyboard;		// spremenljivka "handle" strukture za tipkovnico




// Vidite, da smo "handle strukturo" shranili v GLOBALNO spremenljivko znotraj "kbd.c" modula.
// To je ugodno, saj bodo vse sistemske funkcije za delo s tipkovnico lahko enostavno
// dostopale do potrebnih parametrov znotraj "handle" strukture.


// Zgoraj smo si pripravili "handle" strukturo za delo s tipkovnico, vendar pa je ta struktura
// še neuporabna, saj ni inicializirana! Inicializacijo "handle" strukture bomo izvedli v sklopu
// _init() funkcije spodaj.







// -------- Keyboard buffer definitions ----------

// Pripraviti moramo tudi podatkovne strukture, ki bodo implementirale
// ciklični medpomnilnik (angl. cyclic buffer), kamor bomo shranjevali
// informacijo o pritisnjenih tipkah tipkovnice, ki jih še nismo "sprocesirali".


// Najprej je potrebno vključiti knjižnico za delo s cikličnim medpomnilnikom.
#include "buf.h"

// Definirajmo dolžino cikličnega medpomnilnika za tipkovnico. Definirajmo
// jo kot makro parameter.
#define KBD_BUF_LEN 	32


// In sedaj še definirajmo podatkovne strukture, s katerimi bomo implementirali
// ciklični medpomnilnik. Potrebujemo tabelo, kamor bomo shranjevali informacijo
// o pritisnjenih tipkah. In potrebujemo "handle" strukturo za upravljanje
// cikličnega medpomnilnika.
uint8_t 		kbd_buffer[KBD_BUF_LEN];	// the buffer data array
buf_handle_t 	kbd_buf_handle;				// the buffer handle structure


// To je vse v smislu podatkovnih struktur, ki so potrebne za implementacijo
// cikličnega medpomnilnika. Ta medpomnilnik pa  bomo v nadaljevanju uporabljali
// s pomočjo funkcij za delo z medpomnilnikom, ki pa se nahajajo v buf.c modulu.






// -------------- Public function implementations --------------



// Funkcija KBD_init() poskrbi, da se pravilno inicializira keyboard_handle_t strukturna spremenljivka.
// Poskrbi tudi za inicializacijo medpomnilnika tipkovnice.
//
void KBD_init(void)
{

	// 0. Poskrbite, da je ustrezna nizko-nivojska strojna oprema že inicializirana

		// Za inicializacijo GPIO digitalnih vhodov, s katerimi beremo stanja tipk,
		// je poskrbljeno s pomočjo avtomatsko generirane CubeMX programske kode
		// znotraj funkcije MX_GPIO_Init() v "main.c"!



	// 1. Inicializirajte "handle" strukture za posamezne tipke v tipkovnici na strojnem nivoju

		// Definirajte, kako so posamezne tipke povezane na GPIO pine in porte.
		// To storite tako, da pravilno inicializirate "handle" strukture posameznih tipk.
		// Uporabite makroje "GPIOn" in "LL_GPIO_PIN_n" iz nizko-nivojske LL knjižnice.


		// buttons on port C
		keyboard.buttons[ BTN_ESC ].pin = LL_GPIO_PIN_14 ;
		keyboard.buttons[ BTN_ESC ].port = GPIOC;

		keyboard.buttons[ BTN_OK ].pin = LL_GPIO_PIN_15 ;
		keyboard.buttons[ BTN_OK ].port = GPIOC;

			// DOPOLNI


		// buttons on port G

		keyboard.buttons[ BTN_RIGHT ].pin = LL_GPIO_PIN_8 ;
		keyboard.buttons[ BTN_RIGHT ].port = GPIOG;

		keyboard.buttons[ BTN_LEFT ].pin = LL_GPIO_PIN_6 ;
		keyboard.buttons[ BTN_LEFT ].port = GPIOG;

		keyboard.buttons[ BTN_DOWN ].pin = LL_GPIO_PIN_1 ;
		keyboard.buttons[ BTN_DOWN ].port = GPIOG;

		keyboard.buttons[ BTN_UP ].pin = LL_GPIO_PIN_0 ;
		keyboard.buttons[ BTN_UP ].port = GPIOG;

			// DOPOLNI




	// 2. Inicializirajte začetne vrednosti "handle" strukture za posamezne tipke v tipkovnici na sistemskem nivoju


		// Sprehodimo se preko vseh "handle" struktur za delo s posameznimi tipkami.
		for(int i=0; i < BTN_NUM; i++)
		{
			// Ne pozabite, da v "handle" strukturah za posamezne tipke hranimo tudi vrednost prejšnjega
			// in trenutnega stanja tipk! In te vrednosti je potrebno na začetku nastaviti.
			// Smiselno jih je nastaviti na tako vrednost, kot da so tipke na začetku v sproščenem,
			// nestisnjenem stanju. In tu si lahko pomagate s pomožnim naštevnim tipom button_sig_value_t.

			keyboard.buttons[i].state_new = BTN_SIG_VALUE_RELEASED;
			keyboard.buttons[i].state_old = BTN_SIG_VALUE_RELEASED;
		}



	// 3. Inicializiramo še medpomnilnik tipkovnice.

		// Uporabimo funkcijo BUF_init(), ki določi, katera tabela se bo uporabljala kot
		// ciklični medpomnilnik ter kako dolg bo ta medpomnilnik.
		BUF_init( &kbd_buf_handle, kbd_buffer, KBD_BUF_LEN);

}






// Funkcija KBD_scan() prebere trenutno stanje tipk v tipkovnici; ugotovi,
// katere tipke so bile pritisnjene ter shrani to informacijo v medpomnilnik
// tipkovnice, da se bo kasneje lahko sistem odzval na pritisk teh tipk.
//
void KBD_scan(void)
{
	// Za vsako tipko v tipkovnici, preberi njeno trenutno stanje
	// in ugotovi, ali je bila pritisnjena. Če je bila pritisnjena,
	// shrani to informacijo v medpomnilnik tipkovnice, da se bo
	// kasneje lahko sistem odzval na pritisk te tipke.

	// Sprehodimo se preko vseh "handle" struktur za delo s posameznimi tipkami.
	for(int i=0; i < BTN_NUM; i++)
	{
		// Znotraj zanke delamo trenutno z i-to tipko. Spremenljivka "i" je pomožni števec zanke.

		// Trenutno, novo stanje tipke postane sedaj staro stanje tipke
		keyboard.buttons[i].state_old = keyboard.buttons[i].state_new ;

		// in novo, trenutno stanje tipke se prebere iz ustreznega digitalne GPIO vhoda.
		//DOPOLNI

		keyboard.buttons[i].state_new = LL_GPIO_IsInputPinSet( keyboard.buttons[i].port, keyboard.buttons[i].pin);



		// Sedaj je potrebno preveriti, ali se je za i-to tipko zgodil pritisk. Pri tem si pomagajte s starim in
		// trenutnim stanjem tipke. Pomaga vam lahko tudi pomožni naštevni tip button_sig_value_t.
		if ((keyboard.buttons[i].state_new != keyboard.buttons[i].state_old) && (keyboard.buttons[i].state_new==BTN_SIG_VALUE_PRESSED ))
		{

			// In če zaznamo pritisk i-te tipke, shranimo to informacijo v medpomnilnik tipkovnice.
			// Shranimo seveda kar vrednost elementa naštevnega tipa, ki pripada obravnavani i-ti tipki.
			// In to je seveda kar vrednost pomožne spremenljivke "i". Uporabimo funkcijo BUF_store_byte().
			BUF_store_byte( &kbd_buf_handle, i);

		}

	}

}







// Funkcija KBD_get_pressed_key() iz medpomnilnika tipkovnice vrne
// informacijo o tem, katera je naslednja pritisnjena tipka, na katero
// se še nismo odzvali (tj. je nismo obdelali, "sprocesirali").
//
buttons_enum_t KBD_get_pressed_key(void)
{

	// Pripravimo si pomožno spremenljivko, v kateri bomo pripravili
	// vrednost, ki jo bo funkcija KBD_get_pressed_key() vrnila.
	buttons_enum_t pressed_button;

	// Vidite, da vračamo vrednosti iz seznama naštevnega tipa "buttons_enum_t".



	// Sedaj pa moramo iz medpomnilnika tipkovnice prebrati naslednji element,
	// tj. informacijo o naslednji pritisnjeni tipki, ki je še nismo obdelali.
	//
	// To storimo z uporabo "buffer" funkcije
	//
	//		BUF_get_byte(buf_handle_t *buf_handle, uint8_t *data)
	//
	// Funkciji je potrebno posredovati naslov "handle" strukture za medpomnilnik
	// (v našem primeru je to "&kbd_buf_handle") ter naslov spremenljivke,
	// kamor naj se shrani vrednost prebranega elementa iz medpomnilnika (v našem primeru
	// je to naslov pomožne spremenljivke "&pressed_button").
	//
	// Če funkcija BUF_get_byte() uspešno prebere element in medpomnilnika,
	// bo vrnila vrednost BUFFER_OK.
	// V nasprotnem primeru sklepamo, da je bilo branje elementa iz
	// medpomnilnika neuspešno, ker je medpomnilnik prazen.


	// Torej poskusimo prebrati nov element iz medpomnilnika in ga shraniti v spremenljivko "pressed_button".
	if ( BUF_get_byte( &kbd_buf_handle, &pressed_button ) == BUFFER_OK )
	{
		// Če je bilo branje elementa iz medpomnilnika v spremenljivko "pressed_button"
		// uspešno (tj. vrnjen BUFFER_OK), potem je vrednost, ki jo mora funkcija
		// KBD_get_pressed_key() vrniti kar enaka vrednosti pomožne spremenljivke "pressed_button".

		return pressed_button;
	}
	else
	{

		// Če pa je bilo branje elementa iz medpomnilnika v spremenljivko "pressed_button"
		// neuspešno (tj. ni bil vrnjen BUFFER_OK), potem pa sklepamo, da je medpomnilnik
		// prazen in da pravzaprav ni bila pritisnjena nobena nova tipka.
		// Funkcija KBD_get_pressed_key() mora vrniti kodo "BTN_NONE".

		return BTN_NONE;
	}

}






// Funkcija KBD_get_button_state() vrne TRENUTNO stanje tipke, ki jo specificiramo
// z vhodnim argumentom funkcije "button". Vhodni argument je seveda element naštevnega
// tipa "buttons_enum_t".
//
button_sig_value_t KBD_get_button_state(buttons_enum_t button)
{
	// Namig: tu ni potrebno z LL funkcijo brati trenutnega stanja tipke. Trenutno stanje
	// tipke ugotavljate izključno s funkcijo KBD_scan() in tako zagotovite, da
	// je parameter "state_new" za posamezne tipke vedno pravilno določen in smiseln.

	// Torej trenutno stanje tipke vam je že na voljo in je shranjeno v "state_new"
	// parametru ustrezne "handle" strukture tipke.

	return keyboard.buttons[button].state_new;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////VAJA_09////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Funkcija KBD_demo_toggle_LEDs_if_buttons_pressed() je namenjena testiranju
// delovanja tipkovnice. Deluje tako, da ugotovi, katere tipke so bile pritisnjene
// in se na te pritiske odzove s spremembo stanja ustrezne LEDice ("toggle").
//
// PAZITE: demo funkcija ne izvaja skeniranja tipkovnice! To je potrebno
// izvesti zunaj te funkcije.
void KBD_demo_toggle_LEDs_if_buttons_pressed(void)
{

	// Pomožna spremenljivka, kjer bomo hranili informacijo o nazadnje
	// pritisnjeni tipki.
	buttons_enum_t pressed_key;


	// Preberemo, katera tipka je bila pritisnjena nazadnje
	// (uporabite ustrezno KBD_ funkcijo).
	pressed_key = KBD_get_pressed_key() ;


	// Če je v pomožni spremenljivki informacija o pritisku dejanske tipke,
	while (pressed_key != BTN_NONE)
	{
		// spremenimo stanje ustrezne LEDice glede na to, katera tipka
		// je bila pritisnjena.
		switch ( pressed_key )
		{
		  case BTN_OK:
			  LED_toggle(LED2);
			  break;

		  case BTN_ESC:
			  LED_toggle(LED3);
			  break;

		  case BTN_UP:
			  LED_toggle(LED4);
			  break;

		  case BTN_DOWN:
			  LED_toggle(LED5);
			  break;

		  case BTN_LEFT:
			  LED_toggle(LED6);
			  break;

		  case BTN_RIGHT:
			  LED_toggle(LED7);
			  break;

		  // DOPOLNITE še za ostale tipke


		  default:
			  break;
		}


		// Preberemo, če je bila morda pritisnjena še kaka tipka,
		// ki še ni bila obdelana.
		pressed_key = KBD_get_pressed_key() ;

	}

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////VAJA_09////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// ------- Test functions ---------




void KBD_demo(void)
{

	buttons_enum_t pressed_key;

		LED_init();
		KBD_init();

	// scan keyboard buttons
		// DOPOLNI

		KBD_scan();

		// get next pressed button
		// DOPOLNI

		pressed_key = KBD_get_pressed_key();


	// read all pressed keys from the keyboard buffer
	while (pressed_key != BTN_NONE)
	{
		// toggle LEDs according to pressed keys
		switch (pressed_key)
		{

		case BTN_UP:

			LEDs_on(60);

			break;

		case BTN_DOWN:

			LEDs_on(195);

			break;

		case BTN_LEFT:

			LED_toggle(LED4);
			LED_toggle(LED5);
			LED_toggle(LED6);
			LED_toggle(LED7);

			break;

		case BTN_RIGHT:

			LED_toggle(LED0);
			LED_toggle(LED1);
			LED_toggle(LED2);
			LED_toggle(LED3);

			break;

		case BTN_OK:

			LEDs_on(255);

			break;

		case BTN_ESC:

			LEDs_on(255);

			break;

		  default:
			  break;
		}

		// get next pressed button
		pressed_key = KBD_get_pressed_key();

	}

	// test KBD_get_button_state() function
		// DOPOLNI
	if (KBD_get_button_state(BTN_OK) == BTN_SIG_VALUE_PRESSED){

		LED_demo();
	}



	// scan keyboard period
	HAL_Delay(10);
}


void KBD_flush(void){   // vaja 13

	BUF_flush(&kbd_buf_handle);
}










// -------------- Private function implementations -------------


