// wave.h


#ifndef __WAVE_H__ // do not include more than once
#define __WAVE_H__
// Timer0A_Init interrupt frequency
#define F16HZ (80000000/16)		//bus clock /sound hertz

#include <stdint.h>

//notes (frequencies)
#define A 440
#define B 494
#define D 588
#define G 392
#define G2 98
#define C3 131
#define D3 147
#define E3 156//normal:165
#define F3 175
#define G3 196
#define A3 220
#define B3 233//normal:247
#define C4 262
#define D4 294
#define E4 311//normal:330
#define F4 349
#define G4 392
#define A4 440
#define B4 466//494
#define C5 523
#define D5 587
#define E5 622//659
#define F5 698
#define G5 784
#define A5 880
#define B5 932//988
#define C6 1047
#define D6 1175
#define E6 1244//1319
#define F6 1397
#define G6 1568
#define A6 1760
#define B6 1865//1976
#define C7 2093
 
//note length
#define Whole 16
#define Half 8
#define Quarter 4
#define Eighth 2
#define Sixteenth 1

// Represents a song that will be played.
typedef struct {
	uint16_t pitch;    	//the note to be played (frequency)
	uint16_t length;    //how long note is held
} Note;

uint16_t getSound(uint32_t instrument);
	int Current_Note(void);
//create instance of Note structure and array index
Note song;
uint32_t song_index = 0;
extern Note mary_lamb[];

Note little_lamb[] = {
	// BAGABBBAAABDD
	{B, Quarter},
	{A, Quarter},
	{G, Quarter},
	{A, Quarter},
	{B, Quarter},
	{B, Quarter},
	{B, Half},
	{A, Quarter},
	{A, Quarter},
	{A, Half},
	{B, Quarter},
	{D, Quarter},
	{D, Half},
	
	// BAGABBBBAABAG

	{B, Quarter},
	{A, Quarter},
	{G, Quarter},
	{A, Quarter},
	{B, Quarter},
	{B, Quarter},
	{B, Quarter},
	{B, Quarter},
	{A, Quarter},
	{A, Quarter},
	{B, Quarter},
	{A, Quarter},
	{G, Whole},
	
	// BAGABBBAAABDD

	{B, Quarter},
	{A, Quarter},
	{G, Quarter},
	{A, Quarter},
	{B, Quarter},
	{B, Quarter},
	{B, Half},
	{A, Quarter},
	{A, Quarter},
	{A, Half},
	{B, Quarter},
	{D, Quarter},
	{D, Half},
	
	// BAGABBBBAABAGD

	{B, Quarter},
	{A, Quarter},
	{G, Quarter},
	{A, Quarter},
	{B, Quarter},
	{B, Quarter},
	{B, Quarter},
	{B, Quarter},
	{A, Quarter},
	{A, Quarter},
	{B, Quarter},
	{A, Quarter},
	{G, Quarter},
	{D, Quarter}
};


Note dfp[] = {
	

	{F3, Eighth},
	{E3, Eighth},
	{D3, Eighth},
	{C3, Eighth},
	{B3, Eighth},
	{A3, Eighth},
	{G2, Quarter},

	{A3, Quarter},
	{A3, Quarter},
	{A3, Quarter},
	{A3, Eighth},
	{B3, Eighth},

	{C3, Quarter},
	{C3, Quarter},
	{C3, Quarter},
	{C3, Eighth},
	{D3, Eighth},

	{E3, Quarter},
	{E3, Quarter},
	{E3, Quarter},
	{E3, Quarter},

	{F3, Eighth},
	{E3, Eighth},
	{D3, Eighth},
	{C3, Eighth},
	{B3, Eighth},
	{A3, Eighth},
	{G2, Quarter},
};

#endif // __TIMERSINTS_H__