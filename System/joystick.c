/*
 * joystick.c
 *
 *  Created on:
 *      Author:
 */



/* **************** MODULE DESCRIPTION *************************

Ta modul implementira funkcionalnost "joysticka" na sistemskem
nivoju. Modul je implementiran na podlagi pristopa s pomoÄjo DMA enote
(angl. DMA mode), kjer DMA enota poskrbi za prenos rezultatov
AD pretvornika v sistemske spremenljivke.

Modul omogoÄa:

	- kalibracijo osi "joysticka",

	- branje trenutne relativne pozicije "joystika" (izraĹžene v
	 	 procentualnem deleĹžu polnega odklona osi),

	- detekcijo pritiska tipke,

	- branje trenutnega stanja tipke,

	- demonstracijo delovanja "joysticka" (s pomoÄjo SCI in LED
		modulov)



POZOR: za uspeĹĄno uporabo modula je potrebno predhodno poskrbeti
za ustrezno inicializacijo perifernih enot, ki zagotovijo delovanje
AD pretvornika v navezi z DMA enoto.


************************************************************* */





// ----------- Include other modules (for private) -------------


#include "joystick.h"

// VkljuÄimo ĹĄe LED in SCI sistemska modula, ki bosta pomagala
// pri demonstraciji delovanja "joysticka".
#include "LED.h"
#include "SCI.h"

#include <stdio.h>




// ---------------------- Private definitions ------------------


// Na enak naÄin kot smo to storili pri tipkovnici, definirajmo najprej tip
// "joystick_button_handle_t" strukture, ki hrani vse potrebne parametre
// za delo s tipko "joysticka".

typedef struct
{
	GPIO_TypeDef 						*port;
	uint32_t 							pin;
	joystick_button_sig_value_t			state_new;
	joystick_button_sig_value_t			state_old;

} joystick_button_handle_t;





// Sedaj pa definirajmo tip "handle" strukture joystick_handle_t, ki jo bomo uporabljali
// za delo z "joystickom".
typedef struct
{
	// Dodamo ji "handle" strukturo za delo s tipko "joysticka".
	joystick_button_handle_t	button;


	// Dodamo sistemske spremenljivke za delo z osmi "joysticka".
	uint16_t 	position_raw[NUM_OF_AXES];			// meritev trenutne pozicije osi "joysticka"

	uint16_t 	position_raw_min[NUM_OF_AXES];			// informacija o najmanjĹĄem odklonu osi "joysticka"
	uint16_t 	position_raw_max[NUM_OF_AXES];			// informacija o najveÄjem odklonu osi "joysticka"
	uint16_t 	position_raw_range[NUM_OF_AXES];			// informacija o razponu odklona osi "joysticka" (angl. axis range)


	// Dodamo ji kazalca na "handle" strukturi za Äasovnik in AD pretvornik,
	// da lahko po potrebi s pomoÄjo HAL funkcij upravljamo periferne enote,
	// ki so kljuÄne za delovanje "joystick" modula.
	TIM_HandleTypeDef 	*timer;
	ADC_HandleTypeDef 	*ADC;


} joystick_handle_t;





// Vse to zgoraj so bile le DEFINICIJE ZGRADBE podatkovne strukture. Sedaj pa lahko konÄno ustvarimo
// ĹĄe strukturno spremenljivko "joystick", ki bo imela tako zgradbo in bo za nas hranila vse potrebne
// parametre za delo z "joystickom".

	joystick_handle_t joystick;






// -------- Joystick button buffer ----------

// "Joystick" ima tudi tipke. In pri delu s tipkami je smiselno uporabiti
// medpomnilnik, ki hrani informacijo o tem, katere tipke so bile pritisnjene,
// a se na njihov pritisk ĹĄe nismo odzvali. Äeprav ima naĹĄ "joystick" le eno
// tipko, je uporaba medpomnilnika smiselna. Zakaj? Ker lahko to tipko pritisnemo
// veÄkrat zapored in se je potrebno na vsak pritisk odzvati posebej
// (primer: s tipko "joysticka" streljate in vsak pritisk pomeni en strel).

// Medpomnilnik uporabimo na popolnoma enak naÄin, kot smo to storili v primeru
// tipkovnice.


// Najprej je potrebno vkljuÄiti knjiĹžnico za delo s cikliÄnim medpomnilnikom.
#include "buf.h"

// Definirajmo dolĹžino cikliÄnega medpomnilnika za tipke "joysticka". Definirajmo
// jo kot makro parameter.
#define JOY_BTN_BUF_LEN 	16


// In sedaj ĹĄe definirajmo podatkovne strukture, s katerimi bomo implementirali
// cikliÄni medpomnilnik. Potrebujemo tabelo, kamor bomo shranjevali informacijo
// o pritisnjenih tipkah "joysticka". In potrebujemo "handle" strukturo za upravljanje
// cikliÄnega medpomnilnika.
uint8_t 		joy_btn_buffer[JOY_BTN_BUF_LEN];			// the buffer data array
buf_handle_t 	joy_btn_buf_handle;					// the buffer handle structure









// ------------- Public function implementations --------------




// Funkcija JOY_init() poskrbi za inicializacijo "joystick" modula na sistemskem
// nivoju: inicializira parametre "joystick" handle strukture, inicializira
// medpomnilnik "joysticka" ter inicializira in zaĹžene periferne enote za
// merjenje signalov "joysticka".
// Mimogrede: inicializacijska funkcija se razlikuje od prejĹĄnjih inicializacijskih funkcij
// v tem, da za vhodna argumenta funkcije prejme kazalce na "handle" strukture, ki so
// potrebne za delo s HAL funkcijami.
void JOY_init(ADC_HandleTypeDef *ADC_handle, TIM_HandleTypeDef *timer_handle)
{

	// 0. Poskrbite, da je ustrezna nizko-nivojska strojna oprema Ĺže inicializirana

		// Za merjenje pozicije "joysticka" uporabljamo pristop z DMA enoto in proĹženje
		// AD pretvornika s pomoÄjo Äasovnika. Da bo "joystick" modul pravilno deloval,
		// je potrebno torej poskrbeti za inicializacijo GPIO digitalnega vhoda,
		// Äasovnika, AD pretvornika ter DMA enote, za kar pa je poskrbljeno s pomoÄjo
		// avtomatsko generirane CubeMX programske kode v "main.c".



	 // 1. Inicializacija "handle" strukture za tipke "joysticka" na strojnem nivoju

		// Definirajte, kako so posamezne tipke povezane na GPIO pine in porte.
		// To storite tako, da pravilno inicializirate "handle" strukturo tipke "joysticka".
		// Uporabite makroje "GPIOn" in "LL_GPIO_PIN_n" iz nizko-nivojske LL knjiĹžnice.
		// Postopate popolnoma enako, kot smo to storili v primeru tipkovnice.

			joystick.button.pin = LL_GPIO_PIN_13;
			joystick.button.port = GPIOC;



	// 2. Nastavitev zaÄetnih vrednosti "joystick" sistemskih spremenljivk

		// ZaÄetne vrednosti tipke "joysticka"
		joystick.button.state_old = JOY_BTN_SIG_VALUE_RELEASED;
		joystick.button.state_new = JOY_BTN_SIG_VALUE_RELEASED;


		// Smiselno nastavimo zaÄetne ekstremne vrednosti pozicije "joysticka".
		joystick.position_raw_min[X] = 820;

		joystick.position_raw_min[Y] = 739;


		joystick.position_raw_max[X] = 3101;

		joystick.position_raw_max[Y] = 3100;


		// Shranimo si kazalce na "handle" strukturi za AD pretvornik in Äasovnik,
		// ki ju uporabljamo pri implementaciji "joystick" modula.
		// Ti dve infromaciji seveda shranimo v spremenljivki "joystick handle" strukture.
		joystick.ADC = ADC_handle;
		joystick.timer = timer_handle;



	// 3. Inicializiramo medpomnilnik za tipke "joysticka"

		// Uporabimo funkcijo BUF_init(), ki doloÄi, katera tabela se bo uporabljala kot
		// cikliÄni medpomnilnik ter kako dolg bo ta medpomnilnik.
		BUF_init( &joy_btn_buf_handle, joy_btn_buffer, JOY_BTN_BUF_LEN);




	// 4. Poskrbimo za zagon strojne opreme za merjenje signalov "joysticka"

		// Pri tem uporabljamo funkcije HAL knjiĹžnice.


		// Najprej kalibriramo AD pretvornik, da bodo meritve pozicije "joysticka" bolj toÄne.
		// Kalibracijo izvedemo za "single-ended" vhodne signale.
		HAL_ADCEx_Calibration_Start( joystick.ADC, ADC_SINGLE_ENDED );


		// Nato omogoÄimo DMA enoto v navezi z AD pretvornikom (reÄemo tudi, da zaĹženemo
		// AD pretovrnik v DMA naÄinu).
		// To storimo s klicem spodnje HAL funkcije, ki DMA enoti
		// "sporoÄi", s katerim AD pretvornikom bo delala, na katero mesto v pomnilniku
		// naj shranjuje rezultate AD pretvorbe ter koliko teh rezultatov bo morala prenesti
		// ob vsaki konÄani AD pretvorbi.
		HAL_ADC_Start_DMA( joystick.ADC, (uint32_t *) joystick.position_raw , 2 );


		// Na koncu pa ĹĄe zaĹženemo Äasovnik, ki bo proĹžil AD pretvorbe.
		// S tem smo pravzaprav sproĹžili "avtomatsko" merjenje pozicije "joysticka".
		HAL_TIM_Base_Start( joystick.timer );


	// 5. PoÄakamo na prvo meritev pozicije "joysticka"

		// Äasovnik proĹži AD pretvorbe vsakih 10 milisekund.
		// Torej je smiselno poÄakati milisekundo dlje.

		HAL_Delay(11);


}








// ------- Funkcionalnost povezana s tipkami "joysticka" ----------


// Funkcionalnost "joystickovih" tipk implementiramo na popolnoma enak naÄin,
// kot smo to storili v primeru tipkovnice.


// Funkcija JOY_scan_button() prebere trenutno stanje tipke "joysticka"; ugotovi,
// ali je bila tipka pritisnjena ter shrani to informacijo v medpomnilnik
// "joysticka", da se bo kasneje lahko sistem odzval na pritisk te tipke.
void JOY_scan_button(void)
{
	// Ker ima naĹĄ "joystick" le eno samo tipko, se funkcija poenostavi.

	// Trenutno, novo stanje tipke postane sedaj staro stanje tipke
	joystick.button.state_old = joystick.button.state_new ;

	// in novo, trenutno stanje tipke se prebere iz ustreznega digitalnega GPIO vhoda.
	joystick.button.state_new = LL_GPIO_IsInputPinSet( joystick.button.port, joystick.button.pin );



	// Sedaj je potrebno preveriti, ali se je za to tipko zgodil pritisk. Pri tem si pomagajte s starim in
	// trenutnim stanjem tipke. Pomaga vam lahko tudi pomoĹžni naĹĄtevni tip joystick_button_sig_value_t.
	if ( ( joystick.button.state_old == JOY_BTN_SIG_VALUE_RELEASED) && ( joystick.button.state_new == JOY_BTN_SIG_VALUE_PRESSED ) )
	{

		// In Äe zaznamo pritisk tipke, shranimo to informacijo v medpomnilnik "joystick" tipkovnice.
		// Ker ima "joystick" le eno samo tipko, shranimo vedno isto informacijo: JOY_BTN_FIRE.
		//Uporabimo funkcijo BUF_store_byte().
		BUF_store_byte( &joy_btn_buf_handle, JOY_BTN_FIRE);

	}

}



// Funkcija JOY_get_pressed_key() iz medpomnilnika "joysticka" vrne
// informacijo o tem, katera je naslednja pritisnjena tipka, na katero
// se ĹĄe nismo odzvali (tj. je nismo obdelali, "sprocesirali").
joystick_buttons_enum_t JOY_get_pressed_key(void)
{
	// Ustvarimo pomoĹžno spremenljivko, v kateri bomo pripravili
	// vrednost, ki jo bo funkcija JOY_get_pressed_key() vrnila.
	// Ustvarimo tudi pomoĹžno spremenljivko, ki bo hranila informacijo
	// o uspeĹĄnosti branja iz medpomnilnika.
	joystick_buttons_enum_t 		pressed_button;
	buf_rtrn_codes_t				return_code;


	// Sedaj poskusimo prebrati nov element iz medpomnilnika in ga shraniti v spremenljivko "pressed_button"
	// Hkrati si v spremenljivko "return_code" zabeleĹžimo vrnjeno kodo "buffer" funkcije.
	return_code = BUF_get_byte(&joy_btn_buf_handle, &pressed_button);


	// Äe je bilo branje elementa iz medpomnilnika v spremenljivko "pressed_button"
	// uspeĹĄno (tj. vrnjen BUFFER_OK),
	if ( return_code == BUFFER_OK )
	{
		//potem je vrednost, ki jo mora funkcija JOY_get_pressed_key() vrniti
		// kar enaka vrednosti pomoĹžne spremenljivke "pressed_button".
		return pressed_button;
	}
	else
	{
		// Äe pa je bilo branje elementa iz medpomnilnika v spremenljivko "pressed_button"
		// neuspeĹĄno (tj. ni bil vrnjen BUFFER_OK), potem pa sklepamo, da je medpomnilnik
		// prazen in da pravzaprav ni bila pritisnjena nobena nova tipka.
		// Funkcija JOY_get_pressed_key() mora vrniti kodo "JOY_BTN_NONE".
		return JOY_BTN_NONE;
	}

}



// Funkcija JOY_get_button_state() vrne TRENUTNO stanje tipke "joysticka".
// Ker ima "joystick" eno samo tipko, je funkcija poenostavljena.
joystick_button_sig_value_t JOY_get_button_state(void)
{
	return joystick.button.state_new;
}








// ------- Funkcionalnost povezana z osmi "joysticka" ----------




// Funkcija JOY_calibrate() kalibrira "joystick". To pomeni, da
// funkcija spremlja meritve lege osi "joysticka" in si beleĹži,
// kolikĹĄna je bila najmanjĹĄa oziroma najveÄja vrednost meritve.
// Kalibrirane ekstremne vrednosti bomo nato uporabili za izraÄun
// relativne pozicije osi "joysticka".
// Kalibracijo zakljuÄimo s pritiskom na tipko "joysticka".
void JOY_calibrate(void)
{

	// PomoĹžna spremenljivka, kjer hranimo informacijo o pritisnjeni tipki "joysticka".
	// Sprva privzamemo, da tipka "joysticka" ĹĄe ni bila pritisnjena.
	joystick_buttons_enum_t pressed_key = JOY_BTN_NONE;


	// Dokler ni pritisnjena tipka "joystikca", izvajmo kalibracijo.
	while (pressed_key == JOY_BTN_NONE)
	{

		// BeleĹžimo si minimalne vrednosti (najmanĹĄi odklon osi).


			// Za X os "joysticka".
			// Äe je trenutna meritev odklona manjĹĄa od zabeleĹžene minimalne vrednosti,
			if ( joystick.position_raw[X]<joystick.position_raw_min[X] )
			{
				// to vrednost zabeleĹžimo kot novo minimalno vrednost.

				joystick.position_raw_min[X] =  joystick.position_raw[X];
			}

			// Za Y os "joysticka".
			// Äe je trenutna meritve odklona manjĹĄa od zabeleĹžene minimalne vrednosti,
			if (joystick.position_raw[Y]<joystick.position_raw_min[Y] )
			{
				// to vrednost zabeleĹžimo kot novo minimalno vrednost.
				joystick.position_raw_min[Y] = joystick.position_raw[Y];

			}


		// BeleĹžimo si maksimalne vrednosti (najveÄji odklon osi).

			// Za X os "joysticka".
			// Äe je trenutna meritev odklona veÄja od zabeleĹžene maksimalne vrednosti,
			if ( joystick.position_raw[X]>joystick.position_raw_max[X] )
			{
				// to vrednost zabeleĹžimo kot novo maksimalno vrednost.
				joystick.position_raw_max[X] = joystick.position_raw[X];

			}

			// Za Y os "joysticka".
			// Äe je trenutna meritev odklona veÄja od zabeleĹžene maksimalne vrednosti,
			if ( joystick.position_raw[Y]>joystick.position_raw_max[Y] )
			{
				// to vrednost zabeleĹžimo kot novo maksimalno vrednost.
				joystick.position_raw_max[Y] = joystick.position_raw[Y];

			}



		// Iz ekstremnim vrednosti lahko poraÄunamo razpon odklona (angl. axis range).
		joystick.position_raw_range[X] = joystick.position_raw_max[X]-joystick.position_raw_min[X];

		joystick.position_raw_range[Y] = joystick.position_raw_max[Y]-joystick.position_raw_min[Y];


		// Preverimo, Äe ni morda medtem bila pritisnjena tipka "joysticka".
		JOY_scan_button(); // DOPOLNI
		pressed_key = JOY_get_pressed_key();



		// IzpiĹĄemo status "joysticka" preko SCI vmesnika, kar nam pomaga
		// pri "debuggiranju" kalibracije "joysticka".
		printf("******** CALIBRATION IN PROGRESS ********\n");
		JOY_SCI_send_status();


		// Dodamo Äasovni zamik, preden se ponovno lotimo beleĹženja ekstremnih vrednosti.
		HAL_Delay(100);

	}

}








// Funkcija JOY_get_axis_position() vrne RELATIVNO pozicijo osi "joysticka".
// Relativno pozicijo "joystcika" izraÄunamo na podlagi trenutne "surove" meritve
// pozicije osi ter ekstremnih vrednosti osi, ki jih hranimo v "handle" strukturi
// "joysticka". Os "joysticka" specificiramo kot vhodni argument funkcije.
// Funkcija mora poskrbeti tudi, da so vrednosti, ki jih vraÄamo, vedno smiselno
// omejene na interval [0..100].
uint8_t JOY_get_axis_position(joystick_axes_enum_t axis)
{

	// PomoĹžna spremenljivka, kjer bomo hranili izraÄunano relativno pozicijo osi.
	// Pozor: uporabimo predznaÄeni celoĹĄtevilski tip, ki nam bo pomagal zaznati
	// primer, da je trenutna meritev pozicije manjĹĄa od kalibrirane minimalne vrednosti.
	int16_t	relative_position;


	// IzraÄun relativne pozicije osi na procent natanÄno.
	// Pazite: uporablja se celoĹĄtevilska aritmetika! Operacija deljenja vrne celoĹĄtevilski koliÄnik brez deleĹža zaradi ostanka!
	relative_position = ((joystick.position_raw[axis]-joystick.position_raw_min[axis])*100)/(joystick.position_raw_range[axis]);


	// Poskrbimo ĹĄe, da se vrnjena vrednost nahaja na smiselnem intervalu [0..100].

		// Obravnavamo primer, ko je trenutna meritev pozicije manjĹĄa od kalibrirane minimalne vrednosti.
		if (relative_position > 100  )
		{
			return 100 ;
		}

		// Obravnavamo primer, ko je trenutna meritev pozicije veÄja od kalibrirane maksimalne vrednosti.
		else if ( relative_position < 0 )
		{
			return 0 ;
		}

		// Ostane le ĹĄe primer, ko se izraÄunana relativna pozicija nahaja na intervalu [0..100].
		else
		{
			return relative_position;
		}

}









// ------- Test functions ---------


// Funkcija JOY_SCI_send_status() je pomoĹžna "debug" funkcija, ki nam pomaga izpisovati
// stanje "joysticka" preko serijskega vmesnika SCI. Tako lahko v terminalu opazujete
// meritve osi "joysticka" v realnem Äasu.
void JOY_SCI_send_status(void)
{
	printf("--- Joystick status ---\n");

	// Sistemske spremenljivke "joystick" modula.
	printf("X_RAW = %d\n", joystick.position_raw[X] );
	printf("Y_RAW = %d\n", joystick.position_raw[Y] );

	printf("X_RAW_MIN = %d\n", joystick.position_raw_min[X] );
	printf("X_RAW_MAX = %d\n", joystick.position_raw_max[X] );

	printf("Y_RAW_MIN = %d\n", joystick.position_raw_min[Y] );
	printf("Y_RAW_MAX = %d\n", joystick.position_raw_max[Y] );

	printf("X_RAW_RANGE = %d\n", joystick.position_raw_range[X] );
	printf("Y_RAW_RANGE = %d\n", joystick.position_raw_range[Y] );


	// Trenutna relativna lega osi "joysticka".
	printf("X = %d\n",  JOY_get_axis_position(X) );
	printf("Y = %d\n", JOY_get_axis_position(Y)  );

	printf("\n");
}




// Funkcija JOY_button_demo() demonstrira delovanje tipke "joysticka".
// Demonstracija je izvedena na popolnoma enak naÄin, kot smo to storili
// v primeru tipkovnice.
void JOY_button_demo(void)
{

	joystick_buttons_enum_t pressed_key;


	// scan joystick buttons
	JOY_scan_button();

	// get next pressed button
	pressed_key = JOY_get_pressed_key();


	// read all pressed keys from the joystick buffer
	while (pressed_key != JOY_BTN_NONE)
	{
		// toggle LEDs according to pressed keys
		switch (pressed_key)
		{
		  case JOY_BTN_FIRE:

			  LED_toggle(LED7);

			  break;

		  default:
			  break;
		}

		// get next pressed button
		pressed_key = JOY_get_pressed_key();

	}

	// test get button state
	LED_set(LED7, JOY_get_button_state() );


	// button scan period
	HAL_Delay(10);
}





// Funkcija JOY_LED_demo() demonstrira funkcionalnost "joysticka" s pomoÄjo
// priĹžiganja LEDice glede na trenutno relativno lego osi joysticka: Äe je os
// "joysticka" porinjena v skrajno pozicijo, potem sveti "krajna" LEDica.
void JOY_LED_demo(void)
{
	// DODATNA NEOBVEZNA NALOGA

	// DOPOLNI
}

