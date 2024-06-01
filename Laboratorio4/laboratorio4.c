#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/spi.h>
#include "gfx.h"
#include "lcd-spi.h"
#include "sdram.h"
#include "clock.h"
#include <stdint.h>
#include <stdio.h>

// Registros del giroscopio
#define RNW (1 << 7) /* Escritura en 0 */
#define MNS (1 << 6) /* Multiples lecturas en 1 */
#define WHO_AM_I 0x0F
#define OUT_TEMP 0x26
#define STATUS_REG 0x27
#define CTRL_REG1 0x20
#define CTRL_REG2 0x20
#define CTRL_REG2_HPM1 (0 << 5)
#define CTRL_REG2_HPM2 (0 << 4)
#define CTRL_REG2_HPCF0 (1 << 0)
#define CTRL_REG2_HPCF1 (1 << 1)
#define CTRL_REG2_HPCF2 (1 << 2)
#define CTRL_REG2_HPCF3 (1 << 3)
#define CTRL_REG1_PD (1 << 3)
#define CTRL_REG1_XEN (1 << 1)
#define CTRL_REG1_YEN (1 << 0)
#define CTRL_REG1_ZEN (1 << 2)
#define CTRL_REG1_BW_SHIFT 4
#define CTRL_REG4 0x23
#define CTRL_REG4_FS_SHIFT 4
#define OUT_X_L 0x28
#define OUT_X_H 0x29
#define OUT_Y_L 0x2A
#define OUT_Y_H 0x2B
#define OUT_Z_L 0x2C
#define OUT_Z_H 0x2D
#define CONSOLE_UART USART1
// Llamado de funciones de la libreria console para convertir caracteres e imprimir en consola
void console_putc(char c);
char console_getc(int wait);
void console_puts(char *s);
int console_gets(char *s, int len);

float bateria = 0.95;
float temperatura = 25.00;
bool comunicacion = false;
static void clock_setup_enable(void)
{
	rcc_clock_setup_pll(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_84MHZ]);
	rcc_periph_clock_enable(RCC_SPI5);
	// Habilitacion de reloj de los puertos
	rcc_periph_clock_enable(RCC_GPIOF);
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_GPIOD);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOE);
	rcc_periph_clock_enable(RCC_USART1);
}

static void gyro_setup(void)
{
	// Configuracion de los GPIO's del Giroscopio

	gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO1);			 // Pin para CS
	gpio_mode_setup(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO7 | GPIO8 | GPIO9); // PIN SCK y MOSI
	gpio_set_af(GPIOF, GPIO_AF5, GPIO7 | GPIO8 | GPIO9);
	// Configuración SPI para giroscopio:

	spi_set_master_mode(SPI5);
	spi_set_baudrate_prescaler(SPI5, SPI_CR1_BR_FPCLK_DIV_8);
	spi_set_clock_polarity_0(SPI5);
	spi_set_clock_phase_0(SPI5);
	spi_set_full_duplex_mode(SPI5);
	spi_set_unidirectional_mode(SPI5);
	// spi_set_dff_8bit(SPI5);
	spi_enable_software_slave_management(SPI5);
	// spi_enable_ss_output(SPI1);
	spi_send_msb_first(SPI5);
	spi_set_nss_high(SPI5);
	spi_enable(SPI5);
}

static void usart_setup(void)
{
	// GPIO pin para trafico de información PA9 y PA10 para Tx y Rx respectivamente
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9 | GPIO10);
	// Pin USART1 TX
	gpio_set_af(GPIOA, GPIO_AF7, GPIO9 | GPIO10);
	// Configuración de USART:
	usart_set_baudrate(CONSOLE_UART, 115200);
	usart_set_databits(CONSOLE_UART, 8);
	usart_set_stopbits(CONSOLE_UART, USART_STOPBITS_1);
	usart_set_mode(CONSOLE_UART, USART_MODE_TX_RX);
	usart_set_parity(CONSOLE_UART, USART_PARITY_NONE);
	usart_set_flow_control(CONSOLE_UART, USART_FLOWCONTROL_NONE);
	usart_enable(CONSOLE_UART);
}

// Función para imprimir los valores de los girose en consola
static void print_int(uint32_t usart, int32_t valor)
{
	int i;
	int8_t nr_digits = 0;
	char buffer[25];
	if (valor < 0)
	{
		usart_send_blocking(usart, '-');
		valor = valor * -1;
	}
	if (valor == 0)
	{
		usart_send_blocking(usart, '0');
	}
	while (valor > 0)
	{
		buffer[nr_digits++] = "0123456789"[valor % 10];
		valor /= 10;
	}
	for (i = nr_digits - 1; i >= 0; i--)
	{
		usart_send_blocking(usart, buffer[i]);
	}
}

int main(void)
{
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 1716

	int main()
	{
		///////////////////////////////////////////
		//             ABRIENDO SOCKET           //
		///////////////////////////////////////////
		int sockfd, newsockfd;
		struct sockaddr_in addr, cliaddr;
		socklen_t clilen;
		char buffer[1024];

		// Crear un socket
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0)
		{
			perror("socket");
			exit(1);
		}

		// Configurar el socket
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = INADDR_ANY;
		addr.sin_port = htons(PORT);

		// Asociar el socket a una dirección local
		if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		{
			perror("bind");
			exit(1);
		}

		// Escuchar en el socket
		if (listen(sockfd, 5) < 0)
		{
			perror("listen");
			exit(1);
		}

		// Aceptar una conexión
		clilen = sizeof(cliaddr);
		newsockfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen);
		if (newsockfd < 0)
		{
			perror("accept");
			exit(1);
		}

		// Configuracion de la bateria externa
		gpio_mode_setup(GPIOE, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO4);
		gpio_clear(GPIOE, GPIO4);
		// Configuracion del boton
		gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO0);
		// Configuracion del led
		gpio_mode_setup(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13 | GPIO14);

		clock_setup();
		sdram_init();
		clock_setup_enable();
		usart_setup();
		gyro_setup();
		lcd_spi_init();

		// Configuración inicial del giroscopio:
		gpio_clear(GPIOC, GPIO1);
		spi_send(SPI5, CTRL_REG1);
		spi_read(SPI5);
		// Habilitación de Ejes
		spi_send(SPI5, CTRL_REG1_PD | CTRL_REG1_XEN |
						   CTRL_REG1_YEN | CTRL_REG1_ZEN |
						   (3 << CTRL_REG1_BW_SHIFT));
		spi_read(SPI5);
		gpio_set(GPIOC, GPIO1);

		gpio_clear(GPIOC, GPIO1);
		// Prescaler
		spi_send(SPI5, CTRL_REG2);
		spi_read(SPI5);
		spi_send(SPI5, ~CTRL_REG2_HPM1 | CTRL_REG2_HPM2 | CTRL_REG2_HPCF0 | CTRL_REG2_HPCF1 | CTRL_REG2_HPCF2 | CTRL_REG2_HPCF3);
		spi_read(SPI5);
		gpio_set(GPIOC, GPIO1);
		gpio_clear(GPIOC, GPIO1);
		spi_send(SPI5, CTRL_REG4);
		spi_read(SPI5);
		spi_send(SPI5, (1 << CTRL_REG4_FS_SHIFT));
		spi_read(SPI5);
		// Configuración inicial de la pantalla:
		gfx_init(lcd_draw_pixel, 240, 320);
		gfx_fillScreen(LCD_GREY);
		gfx_fillRoundRect(10, 10, 220, 220, 5, LCD_WHITE);
		gfx_drawRoundRect(10, 10, 220, 220, 5, LCD_RED);
		gfx_setTextSize(2);
		gfx_setCursor(15, 25);
		gfx_puts("MONITOR");
		gfx_setTextSize(2);
		gfx_setCursor(15, 60);
		gfx_puts("DE PENDIENTES");
		gfx_setCursor(15, 80);
		lcd_show_frame(); // permite mostrar diseño en pantalla
		// Variables para obtencion de datos, conversion e impresion en pantalla
		int16_t giro_x, giro_y, giro_z;
		char giro_x_char[15], giro_y_char[15], giro_z_char[15];
		int boton_com_enable;
		boton_com_enable = 0;
		while (1)
		{

			// Configuración de Pantalla Principal:

			gfx_fillScreen(LCD_GREY);
			gfx_fillRoundRect(10, 10, 220, 220, 5, LCD_WHITE);
			gfx_drawRoundRect(10, 10, 220, 220, 5, LCD_RED);
			gfx_setCursor(150, 18.5);
			gfx_drawRoundRect(190, 15, 30, 15, 4, LCD_BLACK);
			gfx_drawRect(220, 18.9, 2, 7, LCD_BLACK);
			gfx_setTextSize(1.5);
			gfx_setCursor(15, 40);
			gfx_puts("Monitor de pendientes");
			gfx_setTextSize(0.9);

			// Toma de datos del giroscopio:

			// Eje x:
			gpio_clear(GPIOC, GPIO1);	   // poner pin en bajo
			spi_send(SPI5, OUT_X_L | RNW); // enviar comando para lectura
			spi_read(SPI5);				   // Lectura del bus SPI
			spi_send(SPI5, 0);			   // Inicia la transferencia de datos del giroscopio al microcontrolador.
			giro_x = spi_read(SPI5);	   // Lectura y almacenamiento del valor del giro
			gpio_set(GPIOC, GPIO1);		   // poner pin en alto, termina comunicacion temporal

			gpio_clear(GPIOC, GPIO1);	   // Inicia el ciclo nuevamente
			spi_send(SPI5, OUT_X_H | RNW); // enviar comando para lectura
			spi_read(SPI5);				   // Lectura del bus SPI
			spi_send(SPI5, 0);			   // Inicia la transferencia de datos del giroscopio al microcontrolador.
			giro_x |= spi_read(SPI5) << 8; // Se lee el dato obtenido y se combina con el anterior obtenido
			gfx_setTextSize(1);			   // Tamaño del texto
			gfx_setCursor(15, 80);
			gfx_puts("Eje x: ");
			sprintf(giro_x_char, "%d", giro_x); // Conversion a caracteres del valor obtenido del giro y almacenamiento en giro_x_char
			gfx_puts(giro_x_char);
			gpio_set(GPIOC, GPIO1); // Pone pin en alto, fin de la comunicacion

			// Eje y:
			gpio_clear(GPIOC, GPIO1);
			spi_send(SPI5, OUT_Y_L | RNW);
			spi_read(SPI5);
			spi_send(SPI5, 0);
			giro_y = spi_read(SPI5);
			gpio_set(GPIOC, GPIO1);

			gpio_clear(GPIOC, GPIO1);
			spi_send(SPI5, OUT_Y_H | RNW);
			spi_read(SPI5);
			spi_send(SPI5, 0);
			giro_y |= spi_read(SPI5) << 8;
			gfx_setCursor(15, 100);
			gfx_puts("Eje y: ");
			sprintf(giro_y_char, "%d", giro_y);
			gfx_puts(giro_y_char);
			gpio_set(GPIOC, GPIO1);

			// Eje z:
			gpio_clear(GPIOC, GPIO1);
			spi_send(SPI5, OUT_Z_L | RNW);
			spi_read(SPI5);
			spi_send(SPI5, 0);
			giro_z = spi_read(SPI5);
			gpio_set(GPIOC, GPIO1);

			gpio_clear(GPIOC, GPIO1);
			spi_send(SPI5, OUT_Z_H | RNW);
			spi_read(SPI5);
			spi_send(SPI5, 0);
			giro_z |= spi_read(SPI5) << 8;
			gfx_setCursor(15, 120);
			gfx_puts("Eje z: ");
			sprintf(giro_z_char, "%d", giro_z);
			gfx_puts(giro_z_char);
			gfx_setTextSize(1.5);
			gfx_setCursor(15, 140);
			gfx_puts("Comunicacion: ");
			gpio_get(GPIOA, GPIO0);
			// Funcionamiento del boton y habilitación de USART:
			if (gpio_get(GPIOA, GPIO0) & (boton_com_enable == 0))
			{
				boton_com_enable = 1;
			}
			else if (gpio_get(GPIOA, GPIO0) & (boton_com_enable == 1))
			{
				boton_com_enable = 0;
			}
			if (boton_com_enable == 1)
			{
				gfx_puts("ON");
				gpio_toggle(GPIOG, GPIO13);
				// Eje x:
				print_int(CONSOLE_UART, giro_x);
				console_puts(" \n");
				// Eje y:
				print_int(CONSOLE_UART, giro_y);
				console_puts(" \n");
				// Eje z:
				print_int(CONSOLE_UART, giro_z);
				console_puts(" \n");
				gpio_set(GPIOC, GPIO1);
			}
			else if (boton_com_enable == 0)
			{
				gfx_puts("OFF");
				gpio_clear(GPIOG, GPIO13);
			}
			gfx_puts("Temperatura:");
			lcd_show_frame();
			// Enviar un mensaje
			sprintf(message, "%f,%f,%f,%f,%f,%s", giro_x, giro_y, giro_z, bateria, temperatura, comunicacion ? "True\r\n" : "False\r\n");
			int n = send(newsockfd, message, strlen(message), 0);
			if (n < 0)
			{
				perror("send");
				exit(1);
			}
		}
		///////////////////////////////////////////
		//             CERRANDO SOCKET           //
		///////////////////////////////////////////
		close(newsockfd);
		close(sockfd);
		return 0;
	}
