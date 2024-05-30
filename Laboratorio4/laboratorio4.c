// Librerias para dar soporte al algoritmo
#include <stdint.h>
#include "clock.h"
#include "console.h"
#include "sdram.h"
#include "lcd-spi.h"
#include "gfx.h"


int main(void)
{
	// Inicializacion de la configuracion mediante librerias
	clock_setup();
	console_setup(115200);
	sdram_init();
	lcd_spi_init();
	while (1) {
		gfx_init(lcd_draw_pixel, 240, 320);
		gfx_fillScreen(LCD_GREY);
		gfx_fillRoundRect(10, 10, 220, 220, 5, LCD_WHITE);
		gfx_drawRoundRect(10, 10, 220, 220, 5, LCD_RED);
		//gfx_fillCircle(20, 250, 10, LCD_RED);
		//gfx_fillCircle(120, 250, 10, LCD_GREEN);
		//gfx_fillCircle(220, 250, 10, LCD_BLUE);
		
		// Impresion en pantalla del titulo y los valores de los ejes del giroscopio, y temperatura
		gfx_setTextSize(1);
		gfx_setCursor(15, 25);
		gfx_puts("MONITOR-PENDIENTES");
		gfx_setTextSize(1);
		gfx_setCursor(15, 49);
		gfx_puts("Eje X:");
		gfx_setCursor(15, 60);
		gfx_puts("Eje Y:");
		gfx_setTextSize(1);
		gfx_setCursor(15, 70);
		gfx_puts("Eje Z:");
		gfx_setTextSize(1);
		gfx_setCursor(15, 80);
		gfx_puts("Temperatura:");
		lcd_show_frame();
	}
}
