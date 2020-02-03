#include <rfm02.h>

 struct rfm02Settings{
   inline static uint8_t *const SCKPort = reinterpret_cast<uint8_t *const>(0x25); // &PORTB;
   inline static uint8_t *const SCKDDRx = reinterpret_cast<uint8_t *const>(0x24); // &DDRB;
   static const uint8_t SCKBit = 0;

   inline static uint8_t *const MOSIPort = reinterpret_cast<uint8_t *const>(0x2B); // &PORTD;
   inline static uint8_t* const MOSIDDRx = reinterpret_cast<uint8_t *const>(0x2A); // &DDRD;
   static const uint8_t MOSIBit = 7;
 };

using Rfm02 = Rfm02_T<rfm02Settings>;

const uint32_t RADIO_BEEP_DURATION_MS = 400;
uint32_t radioBeepDurationMS = 0;
uint8_t RadioSwitcher = 0;
uint32_t timestamp_1H    = millis();
uint32_t timestamp_125KH = micros();

void setup() {
  Rfm02::init();
}

void loop() {
 
 if (millis()-timestamp_1H > 1000){ // 1H timer ISP emulation
   timestamp_1H = millis(); 
   switch (++RadioSwitcher){
    case 2:
     Rfm02::enablePowerAmplifier();
    break;
    case 3 ... 10:
     Rfm02::setOutputPowerLevel( RadioSwitcher-3 );
     radioBeepDurationMS = RADIO_BEEP_DURATION_MS;
    break;
    case 11:
     Rfm02::disablePowerAmplifier(); // saves battery and gives some static noise
    break;
    case 13: // increase it to extend pause before next 8 beeps
     RadioSwitcher = 0;
    break;
  }
 }
 
 if (micros()-timestamp_125KH > 8){ // 125KH timer ISP emulation
  timestamp_125KH = micros();
  if (radioBeepDurationMS){
    // that argument needs to generate different beep tones for every 
    // power level (0..7)
    Rfm02::flipCrystalLoadCapacitor((radioBeepDurationMS--)%(RadioSwitcher ));
  }
 }

}