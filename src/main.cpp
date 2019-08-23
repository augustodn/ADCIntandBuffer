/**
 * Description of project
 *
 */
#include "Arduino.h"
#include "RingBuf.h"   // https://github.com/Locoduino/RingBuffer

#if !defined LED_BUILTIN
#define LED_BUILTIN 13
#endif

#define ADC_CH 2
#define U_INT_L 65535

struct data_t {

    unsigned int id=0;
    unsigned int adc[ADC_CH];

} data, data_r;
RingBuf<data_t, 150> dataBuffer;
byte adc_ch = 0;

void setup()
{

    // initialize LED digital pin as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    //Intialize Serial
    Serial.begin(500000);
    // Initialize encoder

    cli(); // stop global interrupts

    // Setup Timer 1 Interruption
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;
    OCR1A = 624;  //Compare register
    TCCR1B |= (1 << WGM12);
    TCCR1B |= (1 << CS12);
    TIMSK1 |= (1 << OCIE1A); //Enable Timer 1 interrupt

    // Setup ADC Registers
    ADMUX = B01000000;  // Ext Ref, Select Channel 0
    ADCSRB = B00000000; // Clear ADCSRCB Register
    ADCSRA = B10001111; // Turn on ADC, Enable Interrpts, Presc = 128
	
    sei(); // enable global interrupts


}


ISR(TIMER1_COMPA_vect) 
{
 
  // Initialize ADC every tick on the Timer1 interrupt
  ADMUX &= B11110000;   // Begin selecting ADC CH 0
  ADCSRA |= B01000000;  // Initialize conversion
}

ISR(ADC_vect)
{
  
   if (adc_ch < ADC_CH )
   {
   
     // Read first ADCL and then ADCH, otherwise
     // ADC registers are not updated anymore
     data.adc[adc_ch] = ADCL | (ADCH << 8);
     adc_ch = adc_ch + 1;
     
     // Select next ADC CH
     ADMUX = (0xF0 & ADMUX) |  (0x0F & adc_ch);
     // Initialize conversion
     ADCSRA |= B01000000;
     
  }
  else  {
     // End of ADC Acquisitions per channel
     // until next timer interruption
     adc_ch = 0;
     dataBuffer.push(data);
     if (data.id < U_INT_L){
         data.id = data.id + 1;
     }
     else data.id = 0;
  }
}


void loop(){

    if ((Serial.available() > 0)){ 
        byte val_ch = Serial.read();
        if (val_ch == 0x31){ // character '1'
            while (dataBuffer.pop(data_r)){
                Serial.write((char *) &data_r.id, 2);
                Serial.write((char *) &data_r.adc[1], 2);
                Serial.write((char *) &data_r.adc[0], 2);
	        Serial.write(0x7c);
            }
        }
    }
    //delay(50);
}
