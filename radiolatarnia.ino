/*  RADIOLATARNIA BY INIT000


*/
//=================================================KONFIGURACJA RADIOLATARNI=================================================================================
String ramkaCW = "DE SQ1JGX SK AR^";// tutaj nie zmiensiz ramki cw bo nie XD WPM=10 jako coś
String ramkaRTTY = "\nDe SQ1JGX, SQ1JGX, SQ1JGX This is a beacon 20m QRG SWINOUJSCIE, POLAND QTH Loc JO73CV Programmed by INIT000 Kacper\n"; //wiadomość RTTY
double qrg = 27100000; // podaj częstotliwość w Hz
double rttySPEED = 50; //podaj prędkość RTTY max 300BAUD
double shift = 350; //podaj shift dla RTTY


//===========================DDS AD9850 konfig=================================================
#define W_CLK 2       // Pin 2 - Sygnał zegarowy magistrali komunikacyjnej (W_CLK)
#define FQ_UD 3       // Pin 3 - Zmiana częstotliwości (FQ)
#define DATA 4           // Pin 4 - Linia danych interfejsu szeregowego (DATA)
#define RESET 5       // Pin 5 - Reset modułu (RESET)

#define pulseHigh(pin) {digitalWrite(pin, HIGH); digitalWrite(pin, LOW); } // funkcja puls ustawia na moment stan wysoki
//==========================DDS AD9850 koniec konfigu=========================================

int x;


//=======================================================================================DDS AD985===========================================================================
// przesyłanie bajtu, bit po bicie do modułu AD9850 przez linię DATA
void tfr_byte(byte data)
{
  for (int i = 0; i < 8; i++, data >>= 1) {
    digitalWrite(DATA, data & 0x01);
    pulseHigh(W_CLK);   // po każdym bicie sygnał CLK ustawiany w stan wysoki
  }
}
// częstotliwość obliczona według wzoru na 8 stronie dokumentacji = */2^32
void sendFrequency(double frequency) {
  int32_t freq = frequency * 4294967295 / 125000000; // zegar 125 MHz na AD9850
  for (int b = 0; b < 4; b++, freq >>= 8) {
    tfr_byte(freq & 0xFF);
  }
  tfr_byte(0x000);   // bajt kontrolny, same zera "0" dla modułu AD9850
  pulseHigh(FQ_UD);  // aktualizacja częstotliwości
}
//====================================================================================DDS AD9850 koniec=========================================================================

char datastring[150];

void setup() {

  //piny dds ad9850
  pinMode(FQ_UD, OUTPUT);
  pinMode(W_CLK, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(RESET, OUTPUT);
  //koniec pinów dds ad9850
}

void loop()
{
  //----------------------------------------------------------------------------------------RAMKA MORSE-------------------------------------------------------------------------
  //De


  ramkaRTTY.toCharArray(datastring, 140);

  rtty_txstring (datastring);
  delay(2000);



}
//--------------------------------------------------------------------------------------MODULOWANIE RTTY--------------------------------------------------------------------------------------------------------
void rtty_txstring (char * string)
{
 
  /* Simple function to sent a char at a time to 
    ** rtty_txbyte function. 
    ** NB Each char is one byte (8 Bits)
    */
 
  char c;
 
  c = *string++;
 
  while ( c != '\0')
  {
    rtty_txbyte (c);
    c = *string++;
  }
}
 
 
void rtty_txbyte (char c)
{
  /* Simple function to sent each bit of a char to 
    ** rtty_txbit function. 
    ** NB The bits are sent Least Significant Bit first
    **
    ** All chars should be preceded with a 0 and 
    ** proceded with a 1. 0 = Start bit; 1 = Stop bit
    **
    */
 
  int i;
 
  rtty_txbit (0); // Start bit
 
  // Send bits for for char LSB first 
 
  for (i=0;i<7;i++) // Change this here 7 or 8 for ASCII-7 / ASCII-8
  {
    if (c & 1) rtty_txbit(1); 
 
    else rtty_txbit(0); 
 
    c = c >> 1;
 
  }
 
  rtty_txbit (1); // Stop bit
  rtty_txbit (1); // Stop bit
}
 
void rtty_txbit (int bit)
{
  
   if(bit)
 {
sendFrequency(qrg + shift);
 }
 else
 {
  sendFrequency(qrg);
  }

  _delay_ms(1000/rttySPEED); 
 
 
}
