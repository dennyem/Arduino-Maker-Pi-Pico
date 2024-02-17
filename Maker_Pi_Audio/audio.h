#ifndef AUDIO_H //AUDIO_H
#define AUDIO_H //AUDIO_H

#include "pico/stdlib.h"
#include <stdio.h>
#include "pico/time.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "hardware/pll.h"
#include "hardware/clocks.h"
#include "hardware/structs/pll.h"
#include "hardware/structs/clocks.h"
#include <Arduino.h>
#include "SdFat.h"
 
typedef struct {
  uint32_t ChunkID;
  uint32_t ChunkSize;
  uint32_t Format;
  /// fmt
  uint32_t Subchunk1ID;
  uint32_t Subchunk1Size;
  uint16_t AudioFormat;
  uint16_t NumChannels;
  uint32_t SampleRate;
  uint32_t ByteRate;
  uint16_t BlockAlign;
  uint16_t BitsPerSample;
  /// data
  uint32_t Subchunk2ID;
  uint32_t Subchunk2Size;
} wavHeader;           //__attribute__((packed)) wavHeader;

#define AUDIO_STOPPED 0
#define AUDIO_PLAY_MONO 1
#define AUDIO_PLAY_STEREO 2
#define AUDIO_PAUSE_MONO 3
#define AUDIO_PAUSE_STEREO 4

extern unsigned int sliceAL,sliceAR,chanAL,chanAR;
extern unsigned int freqdiv;
extern volatile const char* nowData;
extern volatile const char* nextData;
extern volatile unsigned int nowSize, nextSize, nowPtr;
extern unsigned int audioState;

extern unsigned int audioSetRate(unsigned int s); //returns actual rate
extern unsigned int audioBytes();  //number of bytes left to play
extern unsigned int audioSpace(); //is there space to Queue?
extern unsigned int audioQueue(const char* b, unsigned int s); //add to queue
extern unsigned int audioPlay(unsigned int s);  //takes 1/2 mono/stereo, will resume current state if paused
extern void audioStop(); //stop and put everything in an empty state
extern unsigned int audioPause();
extern void audioPWMINT();
extern void audioInit();

/*** Function Prototypes ***/
void playSound(SdFs *sd, const char * filePath);
void getWavData(SdFs *sd, const char * filePath);
void printfHeader(wavHeader * header);
void printfU32String(uint32_t array);
void swapEndiannes(uint32_t *value);


#endif  //AUDIO_H
