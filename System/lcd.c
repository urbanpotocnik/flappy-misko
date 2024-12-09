#include "lcd.h"
#include "SCI.h"



// ------------------- Privatna globalna spremenljivka ----------------------
UG_GUI gui;

// ------------------ Privatni prototipi funkcij ------------------------------
void UserPixelSetFunction(UG_S16 x, UG_S16 y, UG_COLOR c);
UG_RESULT _HW_FillFrame_(UG_S16 x, UG_S16 y, UG_S16 w, UG_S16 h, UG_COLOR c);

// ---------------- Osnovno upravljanje LCD zaslona preko ILI9341 vmesnika ----------------

/*!
 * @brief Nizkonivojska inicializacija zaslona
 * @internal
 */
static void LCD_IO_Init()
{
	LCD_RST_LOW();
	HAL_Delay(120);
	LCD_RST_HIGH();
	HAL_Delay(120);
}

/*!
 * @brief Strojno pospešen izris polnega pravokotnega polja
 * @param x x koordinata izhodišča
 * @param y y koordinata izhodišča
 * @param h višina polja
 * @param w širina polja
 * @param c podatek o barvi
 * @internal
 *
 * Funkcija izbere želeno območje, potem pa tolikokrat pošlje izbrano barvo,
 * kolikor slikovnih točk je potrebnih.
 */

void LCD_FillRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint16_t c)
{
	uint32_t max_count   = ILI9341_GetParam(LCD_AREA);     /* Št. vseh pikslov     */
	uint32_t pixel_count = ((w + 1) - x) * ((h + 1) - y);  /* Dejansko št. pikslov */

	if(pixel_count > max_count)
		pixel_count = max_count;

	// Izbor koordinat piksla
	ILI9341_SetDisplayWindow(x, y, w, h);

	ILI9341_SendRepeatedData(c, pixel_count);
	//while (pixel_count) {
	//	LCD_IO_SendData((uint16_t *)&c, LCD_IO_DATA_WRITE_CYCLES);
	//	pixel_count--;
	//}
}

/*!
 * @brief Glavna inicializacija zaslona
 *
 * Inicializacija nastavi privzet barvni prostor (RGB565), orientacijo zaslona, vklopi osvetlitev,
 * zapolni zaslon s črno barvo in nastavi bel tekst na črni podlagi.
 * @note Za inicializacijo posameznih strojnih enot mora poskrbeti uporabnik (npr. FMC)
 */
void LCD_Init()
{
	// Resetiraj V/I linijo zaslona
	LCD_IO_Init();

	// Inicializiraj krmilnik Ili9341 v barvnem prostoru RBG565 in brez zasuka slike
	ILI9341_Init(ILI9341_COLORSPACE_RBG565, ILI9341_MISKO_ROTATE_0);
	ILI9341_DisplayOn();

	LCD_ClearScreen();
	// Gornja funkcija se bo zaključila pred izbrisom celega zaslona, ker si FMC zapomni vse
	// ukaze in sporoči, da se je zaključil prenos, ne pa pošiljanje.
	// Brez zamika bo zaslon za trenutek utripnil belo. Prej:  HAL_Delay(25);
	ILI9341_WaitTransfer();

	// Inicializiramo še osvetlitev LCD zaslona.
	LCD_BKLT_init();
}

/*!
 * @brief Počisti zaslon (prebarvaj s črno barvo)
 */
void LCD_ClearScreen()
{
    LCD_FillRect(0, 0, ILI9341_GetParam(LCD_WIDTH), ILI9341_GetParam(LCD_HEIGHT), 0);
}


// ------------------ Testne demo funkcije ----------------

/*!
 * @brief Preprosta demonstracija delovanja LCD zaslona s pomočjo funkcij
 * za neposredno delo z LCD krmilnikom ILI9341.
 */
void LCD_demo_simple()
{
	// Izberemo si barvo. Sami moramo premisliti "kodo", ki kodira barvo,
	// saj nimamo na voljo nekakšne pred-definirane barvne lestvice,
	// iz katere bi lahko izbrali eno od barv.
	uint16_t color = 0xF800; 	// rdeča?

	// Nastavimo pozicijo in velikost okna, kjer bomo nato
	// izrisali obarvane piksle.
	ILI9341_SetDisplayWindow(0, 0, 319, 80);//320,240


	// In nato barvamo okno piksel za pikslom
	for (uint32_t i=0; i<320*240; i++)
	{
		// z uporabo preprostega ILI9341 ukaza.
		ILI9341_SendData(&color, 1);
	}
}

// --------------------- Povezava LCD funkcij z uGUI knjižnico ----------------------

// ------ Definicija dveh temeljnih funkcij za risanje na zaslon -------

// Implementacija funkcije za izris enega samega piksla na zaslon.
void UserPixelSetFunction(UG_S16 x, UG_S16 y, UG_COLOR c)
{
	ILI9341_SetDisplayWindow(x, y, 1, 1);
	ILI9341_SendData((LCD_IO_Data_t *)&c, 1);
}

// Implementacija funkcije za izris pravokotnika na zaslon.
UG_RESULT _HW_FillFrame_(UG_S16 x, UG_S16 y, UG_S16 w, UG_S16 h, UG_COLOR c)
{
	LCD_FillRect(x, y, w, h, c);

	return UG_RESULT_OK;
}

// ------------ Inicializacija uGUI za delo z našim zaslonom -------------------

// Inicializacija uGUI knjižnice za delo z našim LCD zaslonom.
void LCD_uGUI_init(void)
{
	// Inicializacija uGUI knjižnice: registracija funkcije za izris enega piksla na zaslon,
	// specifikacija resolucije zaslona.
	UG_Init(&gui, UserPixelSetFunction, ILI9341_GetParam(LCD_WIDTH), ILI9341_GetParam(LCD_HEIGHT));

	// Nastavitev "default" fontov in barv za besedilo in ozadje.
	UG_FontSelect(&FONT_8X12);
	UG_SetForecolor(C_WHITE);
	UG_SetBackcolor(C_BLACK);

	// Registracija funkcij za izris pravokotnika.
	//UG_DriverRegister(DRIVER_FILL_FRAME, (void *)_HW_FillFrame_);
	//UG_DriverEnable(DRIVER_FILL_FRAME);
}

// ------------------ Testne demo funkcije ----------------

// Demonstracija izrisa grafike na zalon s pomočjo uGUI knjižnice.
void LCD_uGUI_demo_Misko3(void)
{

	UG_FillScreen(C_BLUE);

	uint16_t pozicija_y=135, pozicija_x=150;

	UG_FontSelect(&FONT_32X53);
	UG_SetForecolor(C_VIOLET);
	UG_PutString(pozicija_x-90,pozicija_y,"M");

	UG_SetForecolor(C_BLUE);
	UG_PutString(pozicija_x-60,pozicija_y,"i");

	UG_SetForecolor(C_CYAN);
	UG_PutString(pozicija_x-30,pozicija_y,"S");

	UG_SetForecolor(C_GREEN);
	UG_PutString(pozicija_x,pozicija_y,"K");

	UG_SetForecolor(C_YELLOW);
	UG_PutString(pozicija_x+30,pozicija_y,"o");

	UG_SetForecolor(C_RED);
	UG_PutString(pozicija_x+70,pozicija_y,"3");

	UG_SetForecolor(C_WHITE);
	UG_FontSelect(&FONT_16X26);
	UG_PutString(5,pozicija_y+50,"To mi deli, Borut!");

}

// ------------- Funkcionalnost povezana s senzorjem pritiska na zaslon ---------------

// ------------------- Demo testna funkcija ---------------

// Demonstracija detekcije pritiska na LCD zaslon. Funkcija je nastavljena tako,
// da s pomočjo "polling" pristopa zaznavamo pritisk na zaslon ter nato na to
// mesto izrišemo krogec s pomočjo uGUI knjižnice.
void LCD_TCH_demo(void)
{
	// Pomožni spremenljivki, za hranjenje koordinat pritiska.
	uint16_t x = 0, y = 0;


	// Poskusimo prebrati koordinate pritiska. Branje bo uspešno le,
	// če je trenutno prisoten pritisk na zaslon.
	if ( XPT2046_touch_get_coordinates_if_pressed(&x, &y) )
	{

		// in na to mesto pritiska narišemo krogec s pomočjo uGUI funkcij.
		UG_FillCircle(x,y, 2, C_GREEN);

		// Izpišemo lokacijo še preko SCI vmesnika.
		printf("x = %d\n", x);
		printf("y = %d\n", y);

	}

	// Dodamo nekoliko časovne zakasnitve pred morebitnim naslednjim
	// poizvedovanjem o lokaciji pritiska.
	HAL_Delay(30);
}
