/*
 * prelab4.c
 *
 * Created: 4/04/2024 08:00:21
 * Author : asrol
 */ 
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>

uint8_t counter = 0;
void setup(void);

int main(void)
{
	//loop
	setup();

	while (1)
	{
		PORTD = counter;
	}
}

void setup(void){
	cli();
	DDRD = 0xFF; // Configurar todos los pines del puerto B como salida
	PORTD &= ~(1 <<PORTD0); // limpiar
	UCSR0B =0;
	
	DDRC &= ~((1 << PC0)|(1 << PC1)); // Configurar pin PC0 (botón incremento) como entrada yConfigurar pin PC1 (botón decremento) como entrada
	PORTC |= (1 << PORTC0) | (1 << PORTC1); // Activar pull-up para los botones
	
	PCMSK1 |= (1 << PCINT8) | (1 << PCINT9); // Habilitar interrupciones para pines PC0 y PC1
	PCICR |= (1 << PCIE1); // Habilitar interrupciones por cambio de pin en el puerto C
	sei();//habilitar interrupciones
}


ISR(PCINT1_vect) {
	// Verifica si el botón de incremento fue presionado
	if (PINC == 0b00000001) {
		if (counter < 255) { // Asegura que el contador no supere 255
		counter++; // Incrementa el contador
		 }
	}

	// Verifica si el botón de decremento fue presionado
	else if (PINC == 0b00000010) {
		 if (counter > 0) { // Asegura que el contador no sea negativo
		counter--; // Decrementa el contador
		  }
	}
	else{
		counter =counter;
	}
}
