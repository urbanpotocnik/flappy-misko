//! @file    lcd.h
//! @brief   Grafični gonilnik za platformo MiŠKo3
//! @author  nbert, februar-marec 2022
#ifndef LCD_H_
#define LCD_H_

// LCD modul združuje vse preostale module, ki implementirajo
// polno uporabnost LCD zaslona in jo nadgradijo z grafično knjižnico:
//
// 		- upravljanje LCD zaslona s pomočjo ILI9341 vmesnika (lcd_ili9341.h)
//		- upravljanje osvetlitve LCD zaslona (lcd_backlight.h)
//		- vključitev grafične knjižnice za delo z zaslonom (ugui.h)
//		- upravljanje s senzorjem dotika zaslona (angl. touch screen, XPT2046_touch.h)
//
#include "lcd_ili9341.h"
#include "lcd_backlight.h"
#include "ugui.h"
#include "XPT2046_touch.h"

// -------------- User-defined parameters ----------------
//
// Tu uporabnik modula specificira parametre za delo z LCD zaslonom.

// POZOR: za uspešno uporabo modula je potrebno predhodno poskrbeti
// za ustrezno nizko-nivojsko inicializacijo digitalnih GPIO izhoda
// ter FMC vmesnika za komunikacijo z LCD zaslonom. Za to v našem primeru
// poskrbi CubeMX z avtomatsko generirano kodo.

// Specificiramo, kateri pin se uporablja za LCD_RST signal ("LCD reset").
// Uporabimo pred-definirane makroje na enak način, kot smo to naredili pri
// vaji z LEDicami.
#define LCD_RESET_GPIO_PORT  GPIOD
#define LCD_RESET_GPIO_PIN   GPIO_PIN_3

// --------------------------------------------------------

// Makro funkcije za krmiljenje LCD reset signala.
#define LCD_RST_LOW()        WRITE_REG(LCD_RESET_GPIO_PORT->BRR, LCD_RESET_GPIO_PIN)
#define LCD_RST_HIGH()       WRITE_REG(LCD_RESET_GPIO_PORT->BSRR, LCD_RESET_GPIO_PIN)

// ---- Prototipi funkcij ----
void LCD_Init();
void LCD_ClearScreen();
void LCD_FillRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint16_t c);
void LCD_demo_simple();
void LCD_uGUI_init(void);
void LCD_uGUI_demo_Misko3(void);
void LCD_TCH_demo(void);

#endif /* LCD_H_ */
