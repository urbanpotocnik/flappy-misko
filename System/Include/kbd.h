/*
 * kbd.h
 *
 *  Created on:
 *      Author:
 */

#ifndef INCLUDE_KBD_H_
#define INCLUDE_KBD_H_



// ----------- Include other modules (for public) -------------

// Vključimo nizko-nivojsko LL knjižnjico, da dobimo podporo za delo s tipkami na strojnem nivoju preko GPIO-jev.
#include "stm32g4xx_ll_gpio.h"		// support for GPIOs


// Pri implementaciji sistemskih funkcij za delo s tipkovnico bomo potrebovali sledeče nizko-nivojske funkcije:
//		- uint32_t LL_GPIO_IsInputPinSet (GPIO_TypeDef * GPIOx, uint32_t PinMask) ... DOPOLNI




// -------------------- Public definitions --------------------


// Tu definiramo naštevni tip, s katerim pravzaprav definiramo vse tipke v sistemu (tj. v tipkovnici).
// Naštevni tip ima dva dodatna elementa: NUM_OF_ element ter element za fiktivno tipko, s katero sistem
// lahko sporoči, da ni bila pritisnjena nobena nova tipka.

typedef enum { BTN_UP,BTN_DOWN,BTN_LEFT,BTN_RIGHT,BTN_OK,BTN_ESC,BTN_NUM,BTN_NONE } buttons_enum_t;


// Poglejte: elementom naštevnega tipa smo dodali predpono "BTN_", s katero jasno namigujemo,
// da so ti elementi definirani za delo s tipkami.





// Definirali bomo še en pomožni naštevni tip, ki bo nosil informacijo o tem, kakšna je logična vrednost
// signala na digitalnem vhodu, kamor je priključena tipka. Namreč, če pogledate električno shemo
// za priklop tipk na mikrokrmilnik, boste opazili, da se ob pritisku na tipko na digitalnem vhodu
// pojavi logična ničla - in ne enica. In to je pomemben podatek, kadar delamo s stanji tipk.

typedef enum {BTN_SIG_VALUE_PRESSED=0, BTN_SIG_VALUE_RELEASED=1} button_sig_value_t;


// Opazite lahko invertirajočo logiko: mikrokrmilnik uporablja "pull-up" upor in pritisk tipke
// potegne signal digitalnega vhoda na maso, torej na nizko stanje.







// ---------------- Public function prototypes ----------------



void KBD_init(void);
void KBD_scan(void);

buttons_enum_t KBD_get_pressed_key(void);
button_sig_value_t KBD_get_button_state(buttons_enum_t button);

void KBD_demo_toggle_LEDs_if_buttons_pressed(void);
void KBD_demo(void);

void KBD_flush(void); // vaja 13 


#endif /* INCLUDE_KBD_H_ */
