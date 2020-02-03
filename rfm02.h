// ****************************************************************
// * NSEL pull down (Chip select (active low))
// * CLK not connected (it's clock out for MCU (1 MHz-10 MHz))
// * FSK pull up (FSK data input)
// * NIRQ not connected (Interrupts request output? active low)
// ****************************************************************

#ifndef RFM02_H_
#define RFM02_H_

template<class T> class Rfm02_T{ // inspired by Slon
   static const uint16_t powerManagementCommand = 0xC000;
   static const uint8_t disableOutputOfCLKpin   = 0;
   static const uint8_t _enablePowerAmplifier    = 3; // rewrite
   static const uint8_t enableSynthesizer       = 4;
   static const uint8_t enableCrystalOscillator = 5;
	
   static void semiCommand(uint8_t value);
   static void command(uint16_t value);
  public:
   static void init();
   static void setOutputPowerLevel(uint8_t power){
   	command(0xB000|((power & 7)<< 8)); //0xB000..0xB700
   }
   static void enablePowerAmplifier(); 
   static void disablePowerAmplifier();
   static void flipCrystalLoadCapacitor(bool value);

};

template<class T> void Rfm02_T<T>::semiCommand(uint8_t value){
	for (uint8_t i=0 ; i<8 ; i++){
		_delay_us(2);
		if(value & 0b10000000){
			*T::MOSIPort |= (1<<T::MOSIBit);
			}else{
			*T::MOSIPort &= ~(1<<T::MOSIBit);
		}
		value <<= 1;
		_delay_us(2);
		*T::SCKPort |= (1<<T::SCKBit);
		_delay_us(2);
		*T::SCKPort &= ~(1<<T::SCKBit);
		_delay_us(2);
	}
}

template<class T> void Rfm02_T<T>::command(uint16_t value){
  semiCommand(value >> 8);   // high byte
  semiCommand(value & 0xFF); // low byte
}

template<class T> void Rfm02_T<T>::init(){
  *T::SCKDDRx  |= (1<<T::SCKBit);  // Output
  *T::SCKPort  &= ~(1<<T::SCKBit); // clear
	
  *T::MOSIDDRx |= (1<<T::MOSIBit);  // Output
  *T::MOSIPort &= ~(1<<T::MOSIBit); // clear
	
  command(0xCC00); // Read Status
  command(0x8B10); // Configuration Settings (433MHz, +/- 60kHz)
  command(0xA640); // Freq Settings 434Mhz (it's almost certain)
  command(0xC823); // DataRate 9.6kbps
  command(0xD240); // bug? D040->D240 ?HalfDataRate? PLL settings
  command(0xD040); // bug? D040->D240 ?HalfDataRate?
  command(0xC220); // LowBatt+enable bit sync
  enablePowerAmplifier(); // 0xC001 Power management, turn everything off
  setOutputPowerLevel(7);
}

template<class T> void Rfm02_T<T>::enablePowerAmplifier(){
  command(powerManagementCommand|(1<<disableOutputOfCLKpin)|
  (1<<_enablePowerAmplifier)|(1<<enableSynthesizer)|
  (1<<enableCrystalOscillator));	
}

template<class T> void Rfm02_T<T>::disablePowerAmplifier(){
  command(powerManagementCommand|(1<<disableOutputOfCLKpin));	
}

template<class T> void Rfm02_T<T>::flipCrystalLoadCapacitor(bool value){
  // X0 (select crystal load capacitor) 15.5pF -> 16.0pF
  command(value ? 0b1000101111100000 : 0b1000101111110000);
}
#endif