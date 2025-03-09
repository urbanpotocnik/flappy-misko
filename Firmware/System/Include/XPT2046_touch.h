/*
 * XPT2046_touch.h
 *
 */

#ifndef XPT2046_TOUCH_H_
#define XPT2046_TOUCH_H_


// Vključi se podpora za delo z logičnimi ("boolean") vrednostmi.
#include <stdbool.h>





// -------------------- User-defined parameters ---------------------
//
// POZOR: za uspešno uporabo modula je potrebno predhodno poskrbeti
// za ustrezno nizko-nivojsko inicializacijo digitalnih GPIO vhodov in
// izhodov ter SPI vmesnika za komunikacijo s čipom XPT2046.
// Za to v našem primeru poskrbi CubeMX z avtomatsko generirano kodo.


// Potrebno je vključiti podporo za delo s HAL knjižnico. Osnovna knjižnjica
// je bila namreč predelana tako, da predvideva implementacijo s pomočjo
// HAL knjižnice za komunikacijo preko serijskega vmesnika SPI.
#include "stm32g4xx_hal.h"


// Warning! Use SPI bus with < 2.5 Mbit speed, better ~650 Kbit to be save.
#define XPT2046_SPI_PORT 		hspi1


// Specificirati je potrebno pina za proženje prekinitev (IRQ pin)
// ter "chip select".
#define XPT2046_IRQ_GPIO_Port 	GPIOD
#define XPT2046_IRQ_Pin       	GPIO_PIN_6

#define XPT2046_CS_GPIO_Port  	GPIOE
#define XPT2046_CS_Pin        	GPIO_PIN_1


// Specificirati je potrebno število meritev za izračun
// povprečne vrednosti meritve lokacije.Tako se
// bo iz meritev lokacije pritiska na zaslon odstranil šum.
#define XPT2046_AVERAGING_LENGTH			255



// DOPOLNI: Tu je potrebno odkomentirati ustrezen makro, s katerim povemo,
// kako je orientirana LCD in posledično folija za detekcijo pritiska.
//#define XPT2046_ORIENTATION_PORTRAIT
//#define XPT2046_ORIENTATION_LANDSCAPE
//#define XPT2046_ORIENTATION_PORTRAIT_ROT180
#define XPT2046_ORIENTATION_LANDSCAPE_ROT180


// Tu se specificira velikost LCD zaslona v pikslih.
#define XPT2046_SCALE_X 	320
#define XPT2046_SCALE_Y 	240



// Tu se specificira mejne vrednosti koordinat na X in Y
// osi detektorja za zaznavanje pritiska. Te vrednosti
// so rezultat kalibracije.
#define XPT2046_MIN_RAW_X 1756
#define XPT2046_MAX_RAW_X 28899
#define XPT2046_MIN_RAW_Y 3327
#define XPT2046_MAX_RAW_Y 30198




// --------------------------------------------------------------------





// Prototipi funkcij
bool XPT2046_touch_pressed();
void XPT2046_touch_get_coordinates(uint16_t* x, uint16_t* y);

bool XPT2046_touch_get_coordinates_if_pressed(uint16_t* x, uint16_t* y);





#endif /* XPT2046_TOUCH_H_ */



