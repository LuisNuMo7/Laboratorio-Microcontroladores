// Librerias de apoyo al algoritmo
#include <avr/io.h>
#include <avr/interrupt.h>
// Definicion de constantes para el boton y los leds
#define LED_VERDE_VEHICULO PB0
#define LED_AMARILLO_VEHICULO PB1
#define LED_ROJO_VEHICULO PB2
#define LED_VERDE_PEATON_I PB3
#define LED_ROJO_PEATON_I PB4
#define LED_VERDE_PEATON_D PB5
#define LED_ROJO_PEATON_D PB6
#define BOTON_PEATON PB7

volatile uint8_t estado = 0;
volatile uint8_t contador = 0;

ISR(INT1_vect) {
	if (estado == 1 || estado == 3) {
		// Si el semáforo peatonal está en verde, reinicia el contador
		contador = 0;
	}
}

// Interrupcion para el contador mediante timer1
ISR(TIMER1_OVF_vect) {
	contador++;
}
// Ya que una de forma de realizar la maquina de estados 
// es mediante llamado a funciones, se encienden y apagan
// los leds mediante funciones
void luz_verde_vehiculo() {
	PORTB |= (1 << LED_VERDE_VEHICULO);
}

void luz_amarilla_vehiculo() {
	PORTB |= (1 << LED_AMARILLO_VEHICULO);
}

void luz_roja_vehiculo() {
	PORTB |= (1 << LED_ROJO_VEHICULO);
}

void luz_verde_peaton_izquierda() {
	PORTB |= (1 << LED_VERDE_PEATON_I);
}

void luz_roja_peaton_izquierda() {
	PORTB |= (1 << LED_ROJO_PEATON_I);
}

void luz_verde_peaton_derecha() {
	PORTB |= (1 << LED_VERDE_PEATON_D);
}

void luz_roja_peaton_derecha() {
	PORTB |= (1 << LED_ROJO_PEATON_D);
}

void apagar_luces() {
	PORTB &= ~((1 << LED_VERDE_VEHICULO) | (1 << LED_AMARILLO_VEHICULO) | (1 << LED_ROJO_VEHICULO) |
	(1 << LED_VERDE_PEATON_I) | (1 << LED_ROJO_PEATON_I) |
	(1 << LED_VERDE_PEATON_D) | (1 << LED_ROJO_PEATON_D));
}

int main() {
	//inicializar();
	DDRB |= (1 << LED_VERDE_VEHICULO) | (1 << LED_AMARILLO_VEHICULO) | (1 << LED_ROJO_VEHICULO) |
	(1 << LED_VERDE_PEATON_I) | (1 << LED_ROJO_PEATON_I) |
	(1 << LED_VERDE_PEATON_D) | (1 << LED_ROJO_PEATON_D);

	// Inicia la máquina de estados en el estado inicial
	estado = 0;

	// Configura el timer1
	TCCR1B |= (1 << CS12) | (1 << CS10); // Prescaler 1024
	TIMSK |= (1 << TOIE1); // Habilita la interrupción de overflow del Timer1
	sei(); // Habilita las interrupciones globales
	DDRB &= ~(1 << BOTON_PEATON); // Configura el pin como entrada
	PORTB |= (1 << BOTON_PEATON); // Habilita la resistencia de pull-up interna

	// Configura la interrupción para que se active en flanco de bajada del pin del botón
	MCUCR |= (1 << ISC11) | (0 << ISC10); // Configura para activarse en flanco de bajada
	GIMSK |= (1 << INT1); // Habilita la interrupción INT1
	//init_interrupt();

	while (1) {
		switch (estado) {
			case 0:
			// En este estado los vehiculos tienen el derecho de paso
			luz_verde_vehiculo();
			luz_roja_peaton_izquierda();
			luz_roja_peaton_derecha();
			if (contador >= 10) {
				contador = 0;
				estado = 1;
			}
			break;
			case 1:
			// En este estado se apagan las luces y solo queda la luz amarilla de precaucion
			apagar_luces();
			luz_amarilla_vehiculo();
			if (contador >= 2) {
				contador = 0;
				estado = 2;
			}
			break;
			case 2:
			// En este estado el peaton tiene el derecho a paso
			apagar_luces();
			luz_verde_peaton_izquierda();
			luz_verde_peaton_derecha();
			if (contador >= 5) {
				contador = 0;
				estado = 0;
			}
			break;
		}
	}

	return 0;
}



