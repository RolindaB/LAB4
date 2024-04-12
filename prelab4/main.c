//******************************************************************************
//Universidad del Valle de GUatemala
//IE023: Programación de Microcontroladores
//Autor: Astryd Rolinda Magaly Beb Caal
//Proyecto: lab5
//Hardware: ATMEGA328P
//Created: 4/04/2024

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>

uint8_t counter = 0;
volatile uint8_t adc_valueL = 0;
volatile uint8_t adc_valueH = 0;

const uint8_t segments[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};

void setup(void);
void initADC(void);
//void initTimer1(void);
void alarma(void);
int main(void)
{
	// Configuración
	setup();

	while (1)
	{
		ADCSRA |= (1<<ADSC);
		// Multiplexación de los displays de 7 segmentos
		PORTC |= (1 << PORTC4); // Enciende el segundo display
		PORTD = segments[adc_valueL]; // Muestra los dígitos menos significativos con el punto decimal
		_delay_ms(4); // Breve retardo para evitar parpadeos

		PORTC &= ~(1 << PORTC4); // Apaga el segundo display
		PORTC |= (1 << PORTC3); // Enciende el primer display
		PORTD = segments[adc_valueH]; // Muestra los dígitos más significativos
		_delay_ms(4); // Breve retardo para evitar parpadeos
		PORTC &= ~(1 << PORTC3); // Apaga el primer display

		// Multiplexación de las LEDs del contador
		PORTB |= (1 << PORTB2); // Enciende las LEDs del contador
		PORTD = counter;
		_delay_ms(5); // Breve retardo para evitar parpadeos
		PORTB &= ~(1 << PORTB2); // Apaga las LEDs del contador
		_delay_ms(5); // Breve retardo para evitar parpadeos
		alarma();
	}
	return 0;
}

void setup(void)
{
	// Configura el puerto D como salida para los displays de 7 segmentos
	DDRD = 0xFF; // Todos los pines de D como salida
	UCSR0B = 0;
	PORTD =0;
	// Configura los pines PC3 y PC4 como salida para la multiplexación de los displays
	DDRC |= (1 << DDC3) | (1 << DDC4);

	// Configura el pin PB2 como salida para la multiplexación de las LEDs del contador
	DDRB |= (1 << DDB2)|(1 << DDB1);

	// Configura los pines PC0 y PC1 como entrada y habilita las resistencias pull-up
	DDRC &= ~((1 << DDC0) | (1 << DDC1));
	PORTC |= (1 << PORTC0) | (1 << PORTC1);

	// Habilita las interrupciones por cambio de pin en el puerto C para los pines PC0 y PC1 (botones)
	PCICR |= (1 << PCIE1);
	PCMSK1 |= (1 << PCINT8) | (1 << PCINT9);
	
	initADC();
	//initTimer1();
	// Habilita interrupciones globales
	sei();
	PCIFR |= (1 << PCIF1);
}

void initADC(void)
{
	ADMUX = 6;
	// Selecciona ADC6 como entrada analógica (MUX2:0 = 0b110)
	ADMUX |= (1 << MUX2) | (1 << MUX1);
	// Configura referencia de voltaje AVCC con capacitor en AREF
	ADMUX |= (1 << REFS0);
	ADMUX &= ~(1<<REFS1);
	// Configura el ADC para operar en modo de 8 bits
	ADMUX |= (1 << ADLAR);
	ADCSRA = 0;
	// Habilita el ADC y configura la prescaler a 128 (Frecuencia de muestreo = F_CPU / 128)
	ADCSRA |= (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	ADCSRA |= (1 << ADEN); // habilitar interrupción
}

void alarma(){
	if (ADCH>counter)
	{
		PORTB |= (1<<PORTB1);
	}
	else
	{
		PORTB &= ~(1<<PORTB1);
	}
	
}

/*void initTimer1(void)
{
	TCCR1B |= (1 << CS12) | (1 << CS10);
	TCNT1 = 65340;
	TIMSK1 |= (1 << TOIE1);
}*/

ISR(PCINT1_vect){
	{
		if(((PINC) & (1<<0)) == 0){
			counter ++;
			if(counter >= 255){
				counter = 255;
			}
			
		}
		
		if(((PINC) & (1<<1)) == 0){
			counter --;
			if(counter <= 0){
				counter = 0;
			}
			
		}
		
	}
}

ISR(ADC_vect)
{
	//SE ALMACENA EL VALOR DE ADC Y SE BORRA EL VALOR DE LA BANDERA
	uint16_t adc_val16 = ADC;
	uint8_t adcValue = (adc_val16 >> 8);

	ADCSRA |= (1<<ADIF);
	
	//SE SEPARA EL VALOR DEL ADC EN 4 BITS CADA UNO
	adc_valueH = adcValue >> 4;
	adc_valueL = adcValue & 0x0F;
	ADCSRA |= (1<<ADSC);
	
}

/*ISR(TIMER1_OVF_vect)
{
	TCNT1 = 65340;
	ADCSRA |= (1 << ADSC);
	TIFR1 |= (1 << TOV1);
}*/
