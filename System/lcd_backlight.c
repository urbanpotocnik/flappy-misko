/*
 * lcd_backlight.c
 *
 *  Created on: 
 *      Author: 
 */





/* **************** MODULE DESCRIPTION *************************

Ta modul implementira sistemske funkcije za upravljanje svetilnosti
osvetlitve zaslona.

Spreminjanje svetilnosti osvetlitve se doseže s pomočjo spreminjanja
širine pravokotnega pulza, ki krmili vklop in in izklo osvetlitve zaslona
(tj. pulzno-širinske modulacije krmilnega signala, angl. pulse width
modulation, PWM).

Modul omogoča:
	-	nastavitev svetilnosti v smislu nastavitve relativne jakosti
		osvetlitve podane v procentih (tj. od 0% do 100%),
	-	ugašanje osvetlitve
	-	vklop osvetlitve na pred-definirano privzeto vrednost
	- 	branje trenutno nastavljene jakosti osvetlitve


POZOR: Za uspešno uporabo modula je potrebno predhodno poskrbeti
za ustrezno nizko-nivojsko inicializacijo pripadajočega časovnika,
ki je uporabljen za generiranje PWM signala.

************************************************************* */





// ----------- Include other modules (for private) -------------

#include "lcd_backlight.h"		// lastne definicije modula

#include "SCI.h"				// za demonstracijo





// ---------------------- Private definitions ------------------



// Definiramo tip "handle" strukture "LCD_BKLT_handle_t", ki jo bomo uporabili za
// upravljanje osvetlitve LCD zaslona.
// V strukturi je potrebno pomniti:
//		-	kateri časovnik ("timer") bo uporabljen za implementacijo PWM signala,
//		- 	kateri kanal časovnika ("timer channel") bo uporabljen za implementacijo PWM signala
//		- 	katera LL funkcija bo uporabljana za nastavitev širine PWM pulza ("compare value")
//		- 	katera LL funkcija bo uporabljana za branje nastavitve širine PWM pulza ("compare value")
//		-	kolikšna bo privzeta relativna jakost osvetlitve (tj. "default brightness value in %")
//
// Vidite, da bo tokrat med parametri potrebno hraniti tudi informacijo o tem, katero LL funkcijo
// je potrebno uporabiti za delo s kanalom časovnika, ki implementira PWM signal.
// To informacijo bomo hranili s pomočjo kazalca na funkcijo. Kazalec na funkcijo definiramo
// po sledeči predlogi:
//
//		function_return_type (*Function_pointer_name) (function_argument_list)
//
// Primer uporabe kazalcev na funkcije si lahko pogledate na:
//		https://www.tutorialspoint.com/function-pointer-in-c
//


// Definirajmo torej podatkovni tip "handle" strukture.
typedef struct
{
	// Informacija o uporabljenem časovniku in kanalu časovnika.
	TIM_TypeDef *	timer;
	uint32_t		timer_channel;

	// Informacija o tem, katere LL funkcije bomo uporabljali za upravljanje
	// kanala časovnika. Kazalec na funkcijo definiramo po sledeči predlogi:
	//
	// 		function_return_type (*Function_pointer_name) (function_argument_list)
	//
	void	(* SetCompare) (TIM_TypeDef * TIMx, uint32_t CompareValue);		// kazalec na funkcijo za določanje vrednosti za primerjavo
	uint32_t (* GetCompare) (TIM_TypeDef * TIMx);				// kazalec na funkcijo za branje vrednosti za primerjavo

	// informacija o privzeti jakosti osvetlitve
	// (ker je jakost osvetlitve na intervalu od 0 do 100, je dovolj 8-bitna spremenljivka)
	uint8_t		default_brightness;


} LCD_BKLT_handle_t;




// In sedaj na podlagi zgoraj definiranega tipa definiramo še globalno strukturno spremenljivko "LCD_backlight".

LCD_BKLT_handle_t  LCD_backlight;








// -------------- Public function implementations --------------



// Funkcija LCD_BKLT_init() poskrbi za inicializacijo "handle" strukture za upravljanje LCD osvetlitve,
// za omogočitev delovanja časovnika, omogočitev delovanja kanala časovnika ter za določitev začetne
// vrednosti jakosti osvetlitve LCD zaslona ob inicializaciji sistema.
void LCD_BKLT_init(void)
{

	// 0. Poskrbite, da je ustrezna nizko-nivojska strojna oprema že inicializirana

		// Za nizko-nivojsko inicializacijo časovnika, s katerim implementiramo PWM signal za
		// krmiljenje LCD osvetlitve, je poskrbljeno s pomočjo avtomatsko generirane CubeMX
		// programske kode znotraj funkcije MX_TIM4_Init() v "main.c"!



	// 1. Inicializacija "handle" strukture za za upravljanje LCD osvetlitve


		// --- Specifikacija časovnika ter kanala časovnika ---
		//
		// Ti dva parametra strukture določimo s pred-definiranima makrojema iz LL knjižnice,
		// podobno kot smo to storili v prejšnjih vajah.
		// Pozor: kanala ne podamo s številko, temveč z masko.
		LCD_backlight.timer = TIM4 ;
		LCD_backlight.timer_channel = LL_TIM_CHANNEL_CH1 ;		// namig: poglejte komentar pri implementaciji funkcije LL_TIM_CC_EnableChannel()



		// --- Specifikacija LL funkcij za delo s kanalom časovnika ---
		//
		// Kazalca na funkciji inicializiramo s pomočjo imen LL funkcij za nastavitev in
		// branje vrednosti za primerjavo (angl. compare value).
		LCD_backlight.SetCompare = LL_TIM_OC_SetCompareCH1;
		LCD_backlight.GetCompare = LL_TIM_OC_GetCompareCH1;


		// --- Specifikacija privzete vrednosti za osvetlitev zaslona ---
		//
		LCD_backlight.default_brightness = 50;		// podano v procentih




	// 2. Omogočitev delovanja časovnika, da prične s štetjem.

		// Uporabimo ustrezno LL funkcijo.

		LL_TIM_EnableCounter (TIM4);



	// 3. Vklop osvetlitve LCD zaslona s privzeto jakostjo osvetlitve.

		// Tu si lahko pomagamo kar z ustrezno LCD_BKLT_ za vklop osvetlitve.

		 LCD_BKLT_set_brightness( LCD_backlight.default_brightness);



	// 4. Omogočitev delovanja ustreznega kanala časovnika

		// Ta kanal časovnika bo poskrbel za generacijo PWM signala na pinu mikrokrmilnika
		// po principu primerjanja vrednosti (angl. output compare functionality).
		// Uporabite ustrezno LL funkcijo.

		 LL_TIM_CC_EnableChannel ( TIM4, LL_TIM_CHANNEL_CH1);

}




// Funkcija LCD_BKLT_get_brightness() vrne trenutno nastavitev relativne
// jakosti osvetlitve LCD zaslona.
//
// PREMISLITE: ker smo premišljeno definirali periodo časovnika, s katerim
// generiramo PWM signal, se v vrednosti za primerjanje ("compare value")
// že kar skriva informacija o relativni jakosti osvetlitve.
uint8_t LCD_BKLT_get_brightness(void)
{
	// Vrednost za primerjavo ("compare value") preberemo s pomočjo
	// LL funkcije, ki smo jo specificali ob inicializaciji "handle" strukture.
	// Ustrezno LL funkcijo torej kličemo s pomočjo kazalca na funkcijo,
	// ki je shranjen v "handle" strukturi po sledeči predlogi:
	//
	// 		Function_pointer (function_argument_list)
	//
	// Poglejte primer spodaj.
	return LCD_backlight.GetCompare( LCD_backlight.timer );
}






// Funkcija LCD_BKLT_set_brightness() poskrbi za nastavitev relativne
// jakosti osvetlitve LCD zaslona. Vhodni argument funkcije je relativna
// jakost osvetlitve, podana v procentih (od 0 do 100).
//
// PREMISLITE: ker smo premišljeno definirali periodo časovnika, s katerim
// generiramo PWM signal, vhodni argument funkcije kar neposredno lahko
// uporabimo za nastavitev vrednosti za primerjanje ("compare value")
void LCD_BKLT_set_brightness(uint8_t brightness)
{
	// Vrednost za primerjavo ("compare value") nastavimo s pomočjo
	// LL funkcije, ki smo jo specificali ob inicializaciji "handle" strukture.
	// Ustrezno LL funkcijo torej kličemo s pomočjo kazalca na funkcijo,
	// ki je shranjen v "handle" strukturi po sledeči predlogi:
	//
	// 		Function_pointer (function_argument_list)
	//
	return LCD_backlight.SetCompare( LCD_backlight.timer, brightness );
	// NAMIG: poglejte, kako smo implementirali klic pri funkciji
	// LCD_BKLT_get_brightness() zgoraj.
}












// Funkcija LCD_BKLT_on() prižge osvetlitev LED zaslona in nastavi jakost
// osvetlitve na specificirano privzeto vrednost ("default value").
void LCD_BKLT_on(void)
{

	LCD_BKLT_set_brightness(LCD_backlight.default_brightness );
}


// Funkcija LCD_BKLT_off() ugasne osvetlitev LED zaslona.
void LCD_BKLT_off(void)
{
	LCD_BKLT_set_brightness(0);
}






// -------------- Test functions --------------


// Z demo funkcijo stestiramo in prikažemo delovanje nastavljanja jakosti
// osvetlitve LCD zaslona.
// PREDLOG: V prvi zanki počasi povečujte svetilnost, v drugi jo počasi zmanjšujte.
// (Namig: "počasi" pomeni, da je potrebno uporabiti zakasnitve).
// Poskusite stestirati in demonstrirati vse LCD_BKLT_ funkcije.
void LCD_BKLT_demo(void)
{
	uint8_t	compare_value;


	// "ON - OFF" demo.
	LCD_BKLT_on();
	HAL_Delay(2000);

	LCD_BKLT_off();
	HAL_Delay(2000);


	// Postopno spreminjanje osvetlitve.
	while(1)
	{

		// Naraščanje jakosti osvetlitve.
		for(int val=0; val <= 100; val++)
		{
			// Nastavimo novo vrednost jakosti osvetlitve.
			LCD_BKLT_set_brightness(val);

			// Preberemo dejansko nastavljeno vrednost osvetlitve
			// in jo pošljemo s pomočjo printf() preko SCI vmesnika.
			compare_value = LCD_BKLT_get_brightness();
			printf("compare value = %d\n", compare_value);

			// Kratka zakasnitev.
			HAL_Delay(10);
		}


		// Daljša zakasnitev.
		HAL_Delay(1000);


		// Upadanje jakosti osvetlitve.
		for(int val=100; val >= 0; val--)
		{
			// Nastavimo novo vrednost jakosti osvetlitve.
			LCD_BKLT_set_brightness(val);

			// Preberemo dejansko nastavljeno vrednost osvetlitve
			// in jo pošljemo s pomočjo printf() preko SCI vmesnika.
			compare_value = LCD_BKLT_get_brightness();
			printf("compare value = %d\n", compare_value);

			// Kratka zakasnitev.
			HAL_Delay(10);
		}

		// Daljša zakasnitev.
		HAL_Delay(1000);

	}
}





