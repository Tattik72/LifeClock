// ********************************************************
//
// LifeClock - a philosophical clock about life and luck 
//
// If you think your life depends only by your choises
// you are wrong. Luck have an enormous impact on every
// aspect of the life. This strange clock have a deadline of
// 36625 days, the amount of a 100 years life, including the
// 1 day bonus every 4 years the life give you in february.
//
// As the life speed percection does, the clock starts fast,
// then slow, the accelerate another time on its final
// dead stage. 
// 
// The life starts with a 500 points, about the 
// middle of if maximun of 999 points that represents you
// are living a "God" quality life, versus the 001 minimun that
// indicate you are experiencing a "Nightmare" life.
//
// Every single day of your life will be impacted by 5 factors:
//
// H : Happiness  : the force to be positive 
// A : Ambient    : the force of the ambient where do you live.
// L : Luck       : the force of the chaos
// T : Temperance : the force to react to chaos 
// E : Education  : the quality of the education you have got
//									or you are getting
//
// Every factor will have a positive or negative impact on the 
// quality of your life, and every type of impact will have
// a random duration. Every time the duration of a factor expires, 
// the value of the specific factor will be randomly recalculated,
// as for quality level then for duration time (in days). 
// 
// Level quality aand duration are calculated and highlighed
// in five steps and specific color, as for quality life
// and life remaining days:
//
//          Quality  | Quality life | Life remaining days
// Red    : <=-3		 | <201					| <=1/5 of MAXLIFE
// Violet : <0 - >-3 | >200 - <401	| <=2/5
// White  : 0			   | >400 - <601	| <=3/5
// Green  : >0 - <3	 | >600 - <801	| <=4/5
// Blue   : >=3			 | >800					| <=5/5 
//
// The clock screen is framed by a multicoloured leds line that
// change in full red color when the quality life is under 050, 
// indicating your about to dead, turning in blue when the
// the quality if over 950 indicating your life is fantastic.
//
// This clock doesn't have any type of control, as you on your
// life and restart when expires for a new change to have a
// better life. When the life counter reach zero there are some
// screen page to say something about how the life has going.
//
// You have to be lucky ora patient enought to see these screens,
// because the life expires without your permission.
//
// Good luck for a wonderful life!
//
// by michele.guerra72@gmail.com - www.tattik.it
// https://github.com/Tattik72/LifeClock
//
// v 1.0.0 (31.12.2024) : First version release
//
// ********************************************************


// Libraries needed (only one) 
#include <Adafruit_NeoPixel.h>

// Useful Define 
#define PIN        	2					// Pin used on the ESP01 to data out to WS2812 led strip
#define NUMPIXELS  	576				// Totale leds numbers of the 9x(8x8) matrix WS2812 led modules
#define LUMIN				3					// Brigthness of the leds (1-254). Use low level to have a comfortable vision
#define CBLACK			0,0,0			// RGB format of the colors. Black is to clear a pixel
#define CRED				L,0,0			// Red
#define CGREEN			0,L,0			// Greed
#define CBLUE				0,0,L			// Blue 
#define CYEL				L,L,0			// Yellow
#define CVIOL				L,0,L			// Violet
#define	CCYAN				0,L,L			// Cyan
#define	CWHITE			L,L,L			// White
#define CRAND				255,0,0 	// Trick to say to the color function to advance to next color
#define NOREFRESH		0					// Don't refresh the screen after update the data of the leds chain
#define REFRESH			1					// Refresh the screen after update the data of the leds chain
#define	COMPACT			2					// Use 3x5 font without space between charaters
#define	PXCLEAR			4					// Clear the screen after delay on the variuos screen effect functions
#define MAXLIFE		  600 			// Number of days of your life of 100 years (36625) with 1 bonus day every 4 years
#define FRAMETIME		1000			// Basic frame time 1 day = 1 second
#define INITSCORE		500				// Initial life score quality

// Global variables
uint8_t L = LUMIN; 							// L deve essere <255 per non interferire con la generazione del colore casuale
int 		intervallo = FRAMETIME; // delay di aggiornamento (1000)
int 		caso=0; 								// counter prossimo colore "casuale" in sequenza (1=red, 7=white) vedi define
long 		punteggio=INITSCORE; 		// Initial life score (range 1-999)
long 		vita=MAXLIFE; 					// Initial time life
long 		tempo=0; 								// Frame time passed
int 		tpx[NUMPIXELS]; 				// Array to pass from WS2812 coordinates to X,Y coordinates 
long 		life_span[5] = { 0, 0, 0, 0, 0};
long 		life_intens[5] = { 0, 0, 0, 0, 0};
int 		posFM[16] = {0,6,12,18, 144,150,156,162, 288,294,300,306, 432,438,444,450}; // posizioni caratteri (16) con font 5x5
int 		posFS[24] = {0,4,8,12,16,20, 144,148,152,156,160,164, 288,292,296,300,304,308, 432,436,440,444,448,452}; // posizioni caratteri (24) con font 3x5
long 		trend_bar[7] = {0,0,0,0,0,0,0};

// Font 3x5 (small)
const uint16_t FS[43] = {
	0b0111111000111111, /* 48 0  */
	0b0010011111100001, /* 49 1  */
	0b0101111010111101, /* 50 2  */
	0b0101011010111111, /* 51 3  */
	0b0111000010011111, /* 52 4  */
	0b0111011010110111, /* 53 5  */
	0b0111111010110111, /* 54 6  */
	0b0100001000011111, /* 55 7  */
	0b0111111010111111, /* 56 8  */
	0b0111011010111111, /* 57 9  */
	0b0000000101000000, /* 58 :  */
	0b0000010010100000, /* 59 ;  */
	0b0001000101010001, /* 60 <  */
	0b0010100101001010, /* 61 =  */
	0b0100010101000100, /* 62 >  */
	0b0010001001101100, /* 63 ?  */
	0b0111111111111111, /* 64 @  */
	0b0011111010001111, /* 65 A  */
	0b0111111010101010, /* 66 B  */
	0b0011101000101010, /* 67 C  */
	0b0111111000101110, /* 68 D  */
	0b0111111010110101, /* 69 E  */
	0b0111111010010100, /* 70 F  */
	0b0011101000110111, /* 71 G  */
	0b0111110010011111, /* 72 H  */
	0b0100011111110001, /* 73 I  */
	0b0100101000111110, /* 74 J  */
	0b0111110101010001, /* 75 K  */
	0b0111110000100001, /* 76 L  */
	0b0111110100011111, /* 77 M  */
	0b0111111000011111, /* 78 N  */
	0b0011101000101110, /* 79 O  */
	0b0111111010001000, /* 80 P  */
	0b0011101000101111, /* 81 Q  */
	0b0111111010001011, /* 82 R  */
	0b0010011010110010, /* 83 S  */
	0b0100001111110000, /* 84 T  */
	0b0111110000111111, /* 85 U  */
	0b0111100000111110, /* 86 V  */
	0b0111110001011111, /* 87 W  */
	0b0110110010011011, /* 88 X  */
	0b0111000011111100, /* 89 Y  */
	0b0100111010111001  /* 90 Z  */
};

// Font 5x5 (medium)
const uint32_t FM[128] = {
	0b00000000000000000001000000000000, /* 0   */
	0b00000000000001110010100111000000, /* 1   */
	0b00000001111110001100011000111111, /* 2   */
	0b00000000000100011001110111111111, /* 3   */
	0b00000001000011000111001111011111, /* 4   */
	0b00000001111101111001110001100001, /* 5   */
	0b00000001111111110111001100010000, /* 6   */
	0b00000000000100001000010000111111, /* 7   */
	0b00000001000010000100001000011111, /* 8   */
	0b00000001111100001000010000100001, /* 9   */
	0b00000001111110000100001000010000, /* 10   */
	0b00000000101010101100010101000100, /* 11   */
	0b00000000101011111111110111000100, /* 12   */
	0b00000001111110101111111000111111, /* 13   */
	0b00000001101111011000001101111011, /* 14   */
	0b00000001101101010000000101011011, /* 15   */
	0b00000001101110001000001000111011, /* 16   */
	0b00000001101110001001001000111011, /* 17   */
	0b00000000000011111001000010000000, /* 18   */
	0b00000000000000100011101000100000, /* 19   */
	0b00000000000000100001000010000000, /* 20   */
	0b00000000000011111010101000100000, /* 21   */
	0b00000000100001000111111111101110, /* 22   */
	0b00000000010010001101011000100100, /* 23   */
	0b00000001111100001101011000011111, /* 24   */
	0b00000000000011110010100111100000, /* 25   */
	0b00000001111110101111111010110101, /* 26   */
	0b00000000111000100011100101001010, /* 27   */
	0b00000000010011111011100101011011, /* 28   */
	0b00000000111010101111111111110101, /* 29   */
	0b00000000010001110111110111000100, /* 30   */
	0b00000000000010101000001010100000, /* 31   */
	0b00000000000000000000000000000000, /* 32 (space)  */
	0b00000000010000100001000000000100, /* 33 !  */
	0b00000000101001010000000000000000, /* 34 "  */
	0b00000000101011111010101111101010, /* 35 #  */
	0b00000001111110100111110010111111, /* 36 $  */
	0b00000001100111010001000101110011, /* 37 %  */
	0b00000001010110101101011010110101, /* 38 &  */
	0b00000000010000100000000000000000, /* 39 '  */
	0b00000000010001000010000100000100, /* 40 (  */
	0b00000000010000010000100001000100, /* 41 )  */
	0b00000001000101010111110101010001, /* 42 *  */
	0b00000000000000100011100010000000, /* 43 +  */
	0b00000000000000000000000010001000, /* 44 ,  */
	0b00000000000000000011100000000000, /* 45 -  */
	0b00000000000000000000000000000100, /* 46 .  */
	0b00000000000100010001000100010000, /* 47 /  */
	0b00000000111010011101011100101110, /* 48 0  */
	0b00000000010001100001000010001110, /* 49 1  */
	0b00000001111000001011101000011111, /* 50 2  */
	0b00000000111010001001101000101110, /* 51 3  */
	0b00000001010010100111110010000100, /* 52 4  */
	0b00000001111110000111100000111110, /* 53 5  */
	0b00000000111010000111101000101110, /* 54 6  */
	0b00000001111100001000100010000100, /* 55 7  */
	0b00000000111010001011101000101110, /* 56 8  */
	0b00000000111010001011110000111110, /* 57 9  */
	0b00000000000000100000000010000000, /* 58 :  */
	0b00000000000000100000000010001000, /* 59 ;  */
	0b00000000001101100100000110000011, /* 60 <  */
	0b00000000000001110000000111000000, /* 61 =  */
	0b00000001100000110000010011011000, /* 62 >  */
	0b00000000110010010001000000000100, /* 63 ?  */
	0b00000000111010001101101000001110, /* 64 @  */
	0b00000000010001010100011111110001, /* 65 A  */
	0b00000001111010001111101000111110, /* 66 B  */
	0b00000000111010001100001000101110, /* 67 C  */
	0b00000001111010001100011000111110, /* 68 D  */
	0b00000001111110000111101000011111, /* 69 E  */
	0b00000001111110000111101000010000, /* 70 F  */
	0b00000000111110000100111000101111, /* 71 G  */
	0b00000001000110001111111000110001, /* 72 H  */
	0b00000001111100100001000010011111, /* 73 I  */
	0b00000000011100001000011000101110, /* 74 J  */
	0b00000001000110010111001001010001, /* 75 K  */
	0b00000001000010000100001000011111, /* 76 L  */
	0b00000001000111011101011000110001, /* 77 M  */
	0b00000001000111001101011001110001, /* 78 N  */
	0b00000000111010001100011000101110, /* 79 O  */
	0b00000001111010001111101000010000, /* 80 P  */
	0b00000000111010001101011001101110, /* 81 Q  */
	0b00000001110010010111001001010001, /* 82 R  */
	0b00000000111110000011100000111110, /* 83 S  */
	0b00000001111110101001000010000100, /* 84 T  */
	0b00000001000110001100011000101110, /* 85 U  */
	0b00000001000110001010100101000100, /* 86 V  */
	0b00000001000110001101011101110001, /* 87 W  */
	0b00000001000101010001000101010001, /* 88 X  */
	0b00000001000110001010100010000100, /* 89 Y  */
	0b00000001111100010001000100011111, /* 90 Z  */
	0b00000000111001000010000100001110, /* 91 [  */
	0b00000001000001000001000001000001, /* 92 \  */
	0b00000000111000010000100001001110, /* 93 ]  */
	0b00000000010001010100010000000000, /* 94 ^  */
	0b00000001111110101111111010111111, /* 95 _  */
	0b00000001010101010101010101010101, /* 96 §  */
	0b00000001000010000100001000010000, /* 97 a  */
	0b00000000100001000010000100001000, /* 98 b  */
	0b00000000010000100001000010000100, /* 99 c  */
	0b00000000001000010000100001000010, /* 100 d  */
	0b00000000000100001000010000100001, /* 101 e  */
	0b00000001100011000110001100011000, /* 102 f  */
	0b00000001110011100111001110011100, /* 103 g  */
	0b00000001111011110111101111011110, /* 104 h  */
	0b00000001111111111111111111111111, /* 105 i  */
	0b00000000000000000000000000011111, /* 106 j  */
	0b00000000000000000000001111100000, /* 107 k  */
	0b00000000000000000111110000000000, /* 108 l  */
	0b00000000000011111000000000000000, /* 109 m  */
	0b00000001111100000000000000000000, /* 110 n  */
	0b00000000000000000000001111111111, /* 111 o  */
	0b00000000000000000111111111111111, /* 112 p  */
	0b00000000000011111111111111111111, /* 113 q  */
	0b00000000101000000100010111000000, /* 114 r  */
	0b00000000101000000000000111010001, /* 115 s  */
	0b00000000101000000000001111100000, /* 116 t  */
	0b00000000010001110101010010000100, /* 117 u  */
	0b00000000010000100101010111000100, /* 118 v  */
	0b00000000010001000111110100000100, /* 119 w  */
	0b00000000010000010111110001000100, /* 120 x  */
	0b00000000010001100111000110000100, /* 121 y  */
	0b00000000010000110001110011000100, /* 122 z  */
	0b00000000011000100011000010000110, /* 123 {  */
	0b00000000010000100001000010000100, /* 124 |  */
	0b00000000110000100001100010001100, /* 125 }  */
	0b00000000000001000101010001000000, /* 126 ~  */
	0b00000000000000000000000000000000  /* 127 (del)  */
};


// Create the object "pixels" that include the entire leds strip
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


// Init
void setup()
{
	
	// Debug
	Serial.begin(115200); delay(100); Serial.println();
	
	// Set pseudo-random generator
	//randomSeed(analogRead(A0));
	
	// Coordinate da WS2812 (0-575) a coordinate x,y (0-23,0-23)
	for (int k=0; k<8; k++) for (int i=0; i<8; i++)
	{
		tpx[i+k*24    ]=    k*8+i; tpx[i+k*24+  8]= 64+k*8+i; tpx[i+k*24+ 16]=128+k*8+i; 
		tpx[i+k*24+192]=192+k*8+i; tpx[i+k*24+200]=256+k*8+i; tpx[i+k*24+208]=320+k*8+i; 
		tpx[i+k*24+384]=384+k*8+i; tpx[i+k*24+392]=448+k*8+i; tpx[i+k*24+400]=512+k*8+i; 
	}
	
	// Test accensione
	pixels.begin(); pixels.clear(); line(0,0, 6,0, CRAND, REFRESH+PXCLEAR, 2000);
	
	// Presentazione
	box(0,0,23,23, 0, CRAND, REFRESH, 1000);
	// 
	// Schermate varie
	// 
	// 
	// 
	
	
	// Avvio contatore periodico
	tempo=millis();
}


// Ciclo principale 
void loop() 
{
	// Frame temporizzato
	if  (millis()-tempo>=intervallo)
	{
		// Visualizza contatore vita allineato a destra con il colore degli andamenti: BLUE appena iniziata ... RED al termine
		if (vita<=MAXLIFE/5*5) { scrivi_mini_xy(2,2, "00000", CBLUE, NOREFRESH, 0);	scrivi_mini_xy(2+(vita<10000 ? 4:0)+(vita<1000 ? 4:0)+(vita<100 ? 4:0)+(vita<10 ? 4:0), 2, String(vita), CBLUE, NOREFRESH, 0); }
		if (vita<=MAXLIFE/5*4) { scrivi_mini_xy(2,2, "00000", CGREEN, NOREFRESH, 0);	scrivi_mini_xy(2+(vita<10000 ? 4:0)+(vita<1000 ? 4:0)+(vita<100 ? 4:0)+(vita<10 ? 4:0), 2, String(vita), CGREEN, NOREFRESH, 0); }
		if (vita<=MAXLIFE/5*3) { scrivi_mini_xy(2,2, "00000", CWHITE, NOREFRESH, 0);	scrivi_mini_xy(2+(vita<10000 ? 4:0)+(vita<1000 ? 4:0)+(vita<100 ? 4:0)+(vita<10 ? 4:0), 2, String(vita), CWHITE, NOREFRESH, 0); }
		if (vita<=MAXLIFE/5*2) { scrivi_mini_xy(2,2, "00000", CVIOL, NOREFRESH, 0);	scrivi_mini_xy(2+(vita<10000 ? 4:0)+(vita<1000 ? 4:0)+(vita<100 ? 4:0)+(vita<10 ? 4:0), 2, String(vita), CVIOL, NOREFRESH, 0); }
		if (vita<=MAXLIFE/5*1) { scrivi_mini_xy(2,2, "00000", CRED, NOREFRESH, 0);	scrivi_mini_xy(2+(vita<10000 ? 4:0)+(vita<1000 ? 4:0)+(vita<100 ? 4:0)+(vita<10 ? 4:0), 2, String(vita), CRED, NOREFRESH, 0); }
		
		// Visualizza punteggio allineato a destra colorato a seconda del livello vita
		if (punteggio<201) { scrivi_mini_xy(10,8, "000", CRED, NOREFRESH,0); scrivi_mini_xy(10+(punteggio<100 ? 4:0)+(punteggio<10 ? 4:0), 8, String(punteggio), CRED, NOREFRESH, 0); }
		if (punteggio>200 && punteggio<401) { scrivi_mini_xy(10,8, "000", CVIOL, NOREFRESH,0); scrivi_mini_xy(10+(punteggio<100 ? 4:0)+(punteggio<10 ? 4:0), 8, String(punteggio), CVIOL, NOREFRESH, 0); }
		if (punteggio>400 && punteggio<601) { scrivi_mini_xy(10,8, "000", CWHITE, NOREFRESH,0); scrivi_mini_xy(10+(punteggio<100 ? 4:0)+(punteggio<10 ? 4:0), 8, String(punteggio), CWHITE, NOREFRESH, 0); }
		if (punteggio>600 && punteggio<801) { scrivi_mini_xy(10,8, "000", CGREEN, NOREFRESH,0); scrivi_mini_xy(10+(punteggio<100 ? 4:0)+(punteggio<10 ? 4:0), 8, String(punteggio), CGREEN, NOREFRESH, 0); }
		if (punteggio>800) { scrivi_mini_xy(10,8, "000", CBLUE, NOREFRESH,0); scrivi_mini_xy(10+(punteggio<100 ? 4:0)+(punteggio<10 ? 4:0), 8, String(punteggio), CBLUE, NOREFRESH, 0); }
		
		// Cornice multicolore standard
		box(0,0,23,23, 0, CRAND, NOREFRESH, 0); 
		if (punteggio<=50)  box(0,0,23,23, 0, CRED, NOREFRESH, 0); // Cornice ROSSA: quasi morto
		if (punteggio>=950) box(0,0,23,23, 0, CBLUE, NOREFRESH, 0); // Cornice BLU: quasi un dio
	
		// Calcolo casuale dei fattori vita "c'est la vie"
		// H [0] = Happiness  (felicita' : 0-999 > infelice-felice [nonostante gli altri fattori vita])
		// A [1] = Action     (azione    : 0-999 > immobile-attivo)
		// L [2] = Luck       (fortuna   : 0-999 > sfortuna-fortuna)
		// T [3] = Temperance (carattere : 0-999 > debole-forte)
		// E [4] = Education  (istruzione: 0-999 > essenziale-estesa)
		for (uint8_t i=0; i<5; i++)
		{
			if (life_span[i]==0)
			{
				// Se esaurita la fase, nuova durata del fattore vita...
				life_span[i]=random(0,101); // reale 0-100
				
				// ... e nuova intensita' del fattore vita
				// random (0,999) = 0-998 > -499+random = -499 - +499
				// /100 = tutti i fattori di intensita' saranno compresi tra +4 e -4
				life_intens[i]=(-499+random(0,999))/100; 
			}
			else
			{	
				// Altrimenti toglie tempo alla fase
				life_span[i]--;
			}
		}
		
		// Calcolo nuovo punteggio vita
		punteggio=punteggio+trend_bar[0];
		if (punteggio<1) punteggio=1; // quasi morto
		if (punteggio>999) punteggio=999; // dio
		
		// Visualizza trend
		box(2,8, 8,12, 1, CBLACK, NOREFRESH, 0); // Pulisce l'area del trend
	
		// Calcola dimensione della barra attuale. Max oscillazione del trend = +-4*5 = +-20 rispetto a +-2 delle barre grafiche
		trend_bar[0]=life_intens[0]+life_intens[1]+life_intens[2]+life_intens[3]+life_intens[4];
		
		// Visualizza le barre di trend a scorrimento orizzontale
		for (uint8_t i=0; i<7; i++)
		{
			if (trend_bar[i]>= 14) line(8-i,10, 8-i,8, CBLUE, NOREFRESH, 0);
			if (trend_bar[i]>=  7 && trend_bar[i]< 14) line(8-i,10, 8-i,8, CGREEN, NOREFRESH, 0);
			if (trend_bar[i]>   0 && trend_bar[i]<  7) line(8-i,10, 8-i,9, CGREEN, NOREFRESH, 0);
			if (trend_bar[i]==  0) p(8-i,10, CWHITE, NOREFRESH, 0);
			if (trend_bar[i]<   0 && trend_bar[i]> -7) line(8-i,10, 8-i,11, CRED, NOREFRESH, 0);
			if (trend_bar[i]<= -7 && trend_bar[i]>-14) line(8-i,10, 8-i,12, CRED, NOREFRESH, 0);
			if (trend_bar[i]<=-14) line(8-i,10, 8-i,12, CVIOL, NOREFRESH, 0);
			//Serial.print(i); Serial.print("  "); Serial.println(trend_bar[i]);
		}
		
		// Scorre le barre di trend per effetto scorrimento orizzontale
		for (uint8_t i=6; i>0; i--)	trend_bar[i]=trend_bar[i-1];

		// Colora lettere dei fattori vita
		scrivi_mini_xy(2,17, "HALTE", CWHITE, NOREFRESH, 0);
		if (life_intens[0]>=3) scrivi_mini_xy(2,17, "H", CBLUE, NOREFRESH, 0);
		if (life_intens[0]>0 && life_intens[0]<3) scrivi_mini_xy(2,17, "H", CGREEN, NOREFRESH, 0);
		if (life_intens[0]<0 && life_intens[0]>-3) scrivi_mini_xy(2,17, "H", CRED, NOREFRESH, 0);
		if (life_intens[0]<=-3) scrivi_mini_xy(2,17, "H", CVIOL, NOREFRESH, 0);
		
		if (life_intens[1]>=3) scrivi_mini_xy(6,17, "A", CBLUE, NOREFRESH, 0);
		if (life_intens[1]>0 && life_intens[1]<3) scrivi_mini_xy(6,17, "A", CGREEN, NOREFRESH, 0);
		if (life_intens[1]<0 && life_intens[1]>-3) scrivi_mini_xy(6,17, "A", CRED, NOREFRESH, 0);
		if (life_intens[1]<=-3) scrivi_mini_xy(6,17, "A", CVIOL, NOREFRESH, 0);

		if (life_intens[2]>=3) scrivi_mini_xy(10,17, "L", CBLUE, NOREFRESH, 0);
		if (life_intens[2]>0 && life_intens[2]<3) scrivi_mini_xy(10,17, "L", CGREEN, NOREFRESH, 0);
		if (life_intens[2]<0 && life_intens[2]>-3) scrivi_mini_xy(10,17, "L", CRED, NOREFRESH, 0);
		if (life_intens[2]<=-3) scrivi_mini_xy(10,17, "L", CVIOL, NOREFRESH, 0);

		if (life_intens[3]>=3) scrivi_mini_xy(14,17, "T", CBLUE, NOREFRESH, 0);
		if (life_intens[3]>0 && life_intens[3]<3) scrivi_mini_xy(14,17, "T", CGREEN, NOREFRESH, 0);
		if (life_intens[3]<0 && life_intens[3]>-3) scrivi_mini_xy(14,17, "T", CRED, NOREFRESH, 0);
		if (life_intens[3]<=-3) scrivi_mini_xy(14,17, "T", CVIOL, NOREFRESH, 0);

		if (life_intens[4]>=3) scrivi_mini_xy(18,17, "E", CBLUE, NOREFRESH, 0);
		if (life_intens[4]>0 && life_intens[4]<3) scrivi_mini_xy(18,17, "E", CGREEN, NOREFRESH, 0);
		if (life_intens[4]<0 && life_intens[4]>-3) scrivi_mini_xy(18,17, "E", CRED, NOREFRESH, 0);
		if (life_intens[4]<=-3) scrivi_mini_xy(18,17, "E", CVIOL, NOREFRESH, 0);
		
		// Colora i riquadri dei tempi dei fattori vita dello stesso colore dei fattori vita leggendo il colore
		// di un pixel del carattere (per T uso un pixel più a destra di +1)
		box(2,14, 20,15, 0, CBLACK, NOREFRESH, 0); // Pulisco l'area dei riquadri
		for (uint8_t i=0; i<5; i++)
		{
			if (life_span[i]>80) pixels.setPixelColor(tpx[4+4*i+14*24], pixels.getPixelColor(tpx[2+(i==3?1:0)+i*4+19*24]));
			if (life_span[i]>64) pixels.setPixelColor(tpx[3+4*i+14*24], pixels.getPixelColor(tpx[2+(i==3?1:0)+i*4+19*24]));
			if (life_span[i]>48) pixels.setPixelColor(tpx[2+4*i+14*24], pixels.getPixelColor(tpx[2+(i==3?1:0)+i*4+19*24]));
			if (life_span[i]>32) pixels.setPixelColor(tpx[4+4*i+15*24], pixels.getPixelColor(tpx[2+(i==3?1:0)+i*4+19*24]));
			if (life_span[i]>16) pixels.setPixelColor(tpx[3+4*i+15*24], pixels.getPixelColor(tpx[2+(i==3?1:0)+i*4+19*24]));
			if (life_span[i]> 0) pixels.setPixelColor(tpx[2+4*i+15*24], pixels.getPixelColor(tpx[2+(i==3?1:0)+i*4+19*24]));
		}
		
		// Visualizza la matrice aggiornata
		pixels.show(); delay(5);

		// Calcolo nuovo punteggio vita
		punteggio=punteggio+trend_bar[0];
		if (punteggio<1) punteggio=1; // quasi morto
		if (punteggio>999) punteggio=999; // dio
		
		// Imposto l'intervallo di frame per dare la percezione della velocità del tempo
		// con cui scorre la vita. Divido l'intervallo in 10 sezioni 
		// tempo di frame = intervallo..
		// 1 :  0- 9 fanciullezza    : /10*1   : CBLUE
		// 2 : 10-19 adolescenza     : /10*3   : CBLUE
		// 3 : 20-29 prima maturità  : /10*4   : CGREEN
		// 4 : 30-39 maturità        : /10*7   : CGREEN
		// 5 : 40-49 piena maturità  : /10*10  : CWHITE
		// 6 : 50-59 piena maturità  : /10*10  : CWHITE
		// 7 : 60-69 ultra maturità  : /10*7   : CVIOL
		// 8 : 70-79 terza età       : /10*4   : CVIOL
		// 9 : 80-89 vecchiaia       : /10*3   : CRED
		//10 : 90-99 ultra vecchiaia : /10*1   : CRED
		intervallo=FRAMETIME;
		if (vita<MAXLIFE/10*1 || vita>MAXLIFE/10*9) intervallo=FRAMETIME/10*1;
		else if (vita<MAXLIFE/10*2 || vita>MAXLIFE/10*8) intervallo=FRAMETIME/10*3;
		else if (vita<MAXLIFE/10*3 || vita>MAXLIFE/10*7) intervallo=FRAMETIME/10*4;
		else if (vita<MAXLIFE/10*4 || vita>MAXLIFE/10*6) intervallo=FRAMETIME/10*7;

		//Serial.print(vita); Serial.print("  "); Serial.println(intervallo);
		// Il tempo trascorre ad ogni intervallo tic-toc-tic-toc...
		vita--;	
		
		// Fine vita
		if (vita==0)
		{
			// Schermate varie fine ciclo vita
			// ***
			// ***
			
			// Nuovo inizio
			vita=MAXLIFE; punteggio=INITSCORE;
		}	
		
		// Aggiorna il tempo di frame
		tempo=millis();
	}
}



// -------------------------------------------------------------------------------------------------
// Funzioni


uint32_t ColoreCasuale()
{
	caso++; if (caso==8) caso=1;
	
	// Shift e Or inutili lasciati per chiarezza
	if (caso==1) return ((uint32_t)L << 16) | ((uint32_t)0 << 8) | 0;
	if (caso==2) return ((uint32_t)0 << 16) | ((uint32_t)L << 8) | 0;
	if (caso==3) return ((uint32_t)0 << 16) | ((uint32_t)0 << 8) | L;
	if (caso==4) return ((uint32_t)L << 16) | ((uint32_t)L << 8) | 0;
	if (caso==5) return ((uint32_t)L << 16) | ((uint32_t)0 << 8) | L;
	if (caso==6) return ((uint32_t)0 << 16) | ((uint32_t)L << 8) | L;
	if (caso==7) return ((uint32_t)L << 16) | ((uint32_t)L << 8) | L;
	
	return 0;
}


// x,y = coordinate | r,g,b = colore | s = 0: no refresh, = 1: refresh | d = ritardo
void p(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t s, int d)
{
	pixels.setPixelColor(tpx[x+y*24], (r==255 ? ColoreCasuale() : ((uint32_t)r << 16) | ((uint32_t)g << 8) | b));
	
	if (bitRead(s,0)==1) { pixels.show(); delay(5); } // sotto 5ms ESP8266 si resetta se molte chiamate a show ripetute !
	if (d>0) delay(d);
	if (bitRead(s,2)==1) pixels.clear();
}


// x,y = coordinate | f= 0: no fill 1: fill | r,g,b = colore | s = 0: no refresh, = 1: refresh | d = ritardo
void box(int x0, int y0, int x1, int y1, uint8_t f, uint8_t r, uint8_t g, uint8_t b, uint8_t s, int d)
{
	line(x0,y0, x1,y0, r,g,b, 0,0);	line(x1,y0, x1,y1, r,g,b, 0,0);
	line(x1,y1, x0,y1, r,g,b, 0,0);	line(x0,y1, x0,y0, r,g,b, 0,0);
	
	// box fill
	if (f==1) for (int i=x0+1; i<x1; i++) line(i,y0+1, i,y1-1, r,g,b, 0,0);
	
	if (bitRead(s,0)==1) { pixels.show(); delay(5); } // sotto 5ms ESP8266 si resetta se molte chiamate a show ripetute !
	if (d>0) delay(d);
	if (bitRead(s,2)==1) pixels.clear();
}


// Bresenham Line Algorithm
// https://gist.github.com/bert/1085538
void line(int x0, int y0, int x1, int y1, uint8_t r, uint8_t g, uint8_t b, uint8_t s, int d)
{
	int dx =  abs (x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = -abs (y1 - y0), sy = y0 < y1 ? 1 : -1; 
  int err = dx + dy, e2; 
 
  for (;;)
	{
    pixels.setPixelColor(tpx[x0+y0*24], (r==255 ? ColoreCasuale() : ((uint32_t)r << 16) | ((uint32_t)g << 8) | b));
    if (x0 == x1 && y0 == y1) break;
    e2 = 2 * err;
    if (e2 >= dy) { err += dy; x0 += sx; }
    if (e2 <= dx) { err += dx; y0 += sy; }
  }
	
	if (bitRead(s,0)==1) { pixels.show(); delay(5); } // sotto 5ms ESP8266 si resetta se molte chiamate a show ripetute !
	if (d>0) delay(d);
	if (bitRead(s,2)==1) pixels.clear();
}


// cerchio
void circle(int xm, int ym, int ra, uint8_t r, uint8_t g, uint8_t b, uint8_t s, int d)
{
	int x = -ra, y = 0, err = 2-2*ra; 

	do 
	{
		pixels.setPixelColor(tpx[(xm-x)+(ym+y)*24], (r==255 ? ColoreCasuale() : ((uint32_t)r << 16) | ((uint32_t)g << 8) | b));
		pixels.setPixelColor(tpx[(xm-y)+(ym-x)*24], (r==255 ? ColoreCasuale() : ((uint32_t)r << 16) | ((uint32_t)g << 8) | b));
		pixels.setPixelColor(tpx[(xm+x)+(ym-y)*24], (r==255 ? ColoreCasuale() : ((uint32_t)r << 16) | ((uint32_t)g << 8) | b));
		pixels.setPixelColor(tpx[(xm+y)+(ym+x)*24], (r==255 ? ColoreCasuale() : ((uint32_t)r << 16) | ((uint32_t)g << 8) | b));
		
		ra = err;
		if (ra >  x) err += ++x*2+1;
		if (ra <= y) err += ++y*2+1;
	} while (x < 0);

	if (bitRead(s,0)==1) { pixels.show(); delay(5); } // sotto 5ms ESP8266 si resetta se molte chiamate a show ripetute !
	if (d>0) delay(d);
	if (bitRead(s,2)==1) pixels.clear();
}


// Scrive con font 5x5
// p=posizione assoluta 0-15 (0-3 / 4-7 / 8-11 / 12-15) | v=stringa | 
// r,g,b= colore | s = 0: refresh, = 1 refresh | d = ritardo
void scrivi(uint8_t p, String v, uint8_t r, uint8_t g, uint8_t b, uint8_t s, int d)
{
	for (uint8_t i=0; i<v.length(); i++)
  {
		if (byte(v[i])>31 && byte(v[i])<128 && (p+i)<16)
		{
			uint32_t f = FM[(v[i])]; 
			uint32_t c = (r==255 ? ColoreCasuale() : ((uint32_t)r << 16) | ((uint32_t)g << 8) | b);
			
			for (uint8_t j=0; j<5; j++) 
			{
				for (uint8_t k=0; k<5; k++) 
				{
					if (bitRead(f, j+k*5)==1) pixels.setPixelColor(tpx[posFM[p+i]+(4-j)+(4-k)*24], c); 
						else pixels.setPixelColor(tpx[posFM[p+i]+(4-j)+(4-k)*24], pixels.Color(0, 0, 0));
				}
			}
		}
  }

	if (bitRead(s,0)==1) { pixels.show(); delay(5); } // sotto 5ms ESP8266 si resetta se molte chiamate a show ripetute !
	if (d>0) delay(d);
	if (bitRead(s,2)==1) pixels.clear();
}


// Scrive con font 3x5 (set ascii ridotto)
// p=posizione assoluta 0-23 (0-5 / 6-11 / 12-17 / 18-23) | v=stringa | 
// r,g,b= colore | s = 0: refresh, = 1 refresh | d = ritardo
void scrivi_mini(uint8_t p, String v, uint8_t r, uint8_t g, uint8_t b, uint8_t s, int d)
{
	for (uint8_t i=0; i<v.length(); i++)
  {
		if (byte(v[i])>47 && byte(v[i])<91 && (p+i)<24)
		{
			uint16_t f = FS[(v[i]-48)];  
			uint32_t c = (r==255 ? ColoreCasuale() : ((uint32_t)r << 16) | ((uint32_t)g << 8) | b);
			
			for (uint8_t j=0; j<5; j++) 
			{
				for (uint8_t k=0; k<3; k++) 
				{
					if (bitRead(f, j+k*5)==1) pixels.setPixelColor(tpx[posFS[p+i]+(2-k)+(4-j)*24], c); 
						else pixels.setPixelColor(tpx[posFS[p+i]+(2-k)+(4-j)*24], pixels.Color(0, 0, 0));
				}
			}
		}
  }

	if (bitRead(s,0)==1) { pixels.show(); delay(5); } // sotto 5ms ESP8266 si resetta se molte chiamate a show ripetute !
	if (d>0) delay(d);
	if (bitRead(s,2)==1) pixels.clear();
}


// Scrive con font 3x5 (set ascii ridotto)
// x,y =coordinate assolute 0-23 (0-5 / 6-11 / 12-17 / 18-23)
// se s=2 font compresso senza spazio tra caratteri
// | v=stringa | // r,g,b= colore | s = bit0: 0= no refresh, 1= refresh, bit1: 0=font normale, 1=font compresso
// | d = ritardo
void scrivi_mini_xy(uint8_t x, uint8_t y, String v, uint8_t r, uint8_t g, uint8_t b, uint8_t s, int d)
{
	uint8_t av=4; if (bitRead(s,1)==1) av=3; // se bit 1 di s ? settato allora font compresso
	
	for (uint8_t i=0; i<v.length(); i++)
  {
		if (byte(v[i])>47 && byte(v[i])<91)
		{
			uint16_t f = FS[(v[i]-48)];  
			uint32_t c = (r==255 ? ColoreCasuale() : ((uint32_t)r << 16) | ((uint32_t)g << 8) | b);
			
			for (uint8_t j=0; j<5; j++) 
			{
				for (uint8_t k=0; k<3; k++) 
				{
					if ((x+i*av+2-k)<24 && (y+4-j)<24)
					{
						if (bitRead(f, j+k*5)==1) pixels.setPixelColor(tpx[x+i*av+(2-k)+(y+4-j)*24], c); 
						else pixels.setPixelColor(tpx[x+i*av+(2-k)+(y+4-j)*24], pixels.Color(0, 0, 0));
					}
				}
			}
		}
  }

	if (bitRead(s,0)==1) { pixels.show(); delay(5); } // sotto 5ms ESP8266 si resetta se molte chiamate a show ripetute !
	if (d>0) delay(d);
	if (bitRead(s,2)==1) pixels.clear();
}



// end