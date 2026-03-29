/****
 *  Firmware for HaptiCraft (IEEE TVCG 2026)
 *  Author: Chaeyong Park
 *  Copyright (c): 2026. AXIS Laboratory, Korea University
 *  WEMOS D1 MINI (ESP8266)
 *  
 *  Pre-defined waveforms for Haptic rendering
 *  - The waveforms are defined for ERM vibration, but you can define your own waveforms for other types of haptic rendering.
 *  - The waveforms are defined with 50 data points, and the values are between 0 and 200 for the intensity of the vibration.
 *  - You can adjust the values and the number of data points according to your needs.
 * 
 *  The waveforms are defined as follows:
 *  1) Flat: The intensity is constant throughout the duration of the haptic feedback.
 *  2) Increase: The intensity gradually increases from the minimum value to the maximum value.
 *  3) Decrease: The intensity gradually decreases from the maximum value to the minimum value.
 *  4) Increase-Decrease: The intensity gradually increases from the minimum value to the maximum value and then gradually decreases back to the minimum value.
 */
 
#ifndef _WAVEFORMS_H_
#define _WAVEFORMS_H_

#include "moduleDefinition.h"
#define WAVENUM 50
#define MINIMUNVALUE 30

#ifdef VIBERM
/* Define the waveforms for ERM vibration */
short wFlat[WAVENUM + 1] = {200,200,200,200,200,200,200,200,200,200,
                            200,200,200,200,200,200,200,200,200,200,
                            200,200,200,200,200,200,200,200,200,200,
                            200,200,200,200,200,200,200,200,200,200,
                            200,200,200,200,200,200,200,200,200,200,
                            200};

short wIncrease[WAVENUM + 1] = {0,4,8,12,16,20,24,28,32,36,
                                40,44,48,52,56,60,64,68,72,76,
                                80,84,88,92,96,100,104,108,112,116,
                                120,124,128,132,136,140,144,148,152,156,
                                160,164,168,172,176,180,184,188,192,196,
                                200};

short wDecrease[WAVENUM + 1] = {200,196,192,188,184,180,176,172,168,164,
                                160,156,152,148,144,140,136,132,128,124,
                                120,116,112,108,104,100,96,92,88,84,
                                80,76,72,68,64,60,56,52,48,44,
                                40,36,32,28,24,20,16,12,8,4,
                                0};

short wIncreaseDecrease[WAVENUM + 1] = {0,8,16,24,32,40,48,56,64,72,
                                        80,88,96,104,112,120,128,136,144,152,
                                        160,168,176,184,192,200,192,184,176,168,
                                        160,152,144,136,128,120,112,104,96,88,
                                        80,72,64,56,48,40,32,24,16,8,
                                        0};
#endif

#ifdef VIBERMLEGACY
short wFlat[WAVENUM + 1] = {255,255,255,255,255,255,255,255,255,255,
                        255,255,255,255,255,255,255,255,255,255,
                        255,255,255,255,255,255,255,255,255,255,
                        255,255,255,255,255,255,255,255,255,255,
                        255,255,255,255,255,255,255,255,255,255,
                        255,255,255,255,255,255,255,255,255,255,
                        255,255,255,255,255,255,255,255,255,255,
                        255,255,255,255,255,255,255,255,255,255,
                        255,255,255,255,255,255,255,255,255,255,
                        255,255,255,255,255,255,255,255,255,255,
                        255};

short wIncrease[WAVENUM + 1] = {0,3,5,8,10,13,15,18,20,23,
                            26,28,31,33,36,38,41,43,46,48,
                            51,54,56,59,61,64,66,69,71,74,
                            77,79,82,84,87,89,92,94,97,99,
                            102,105,107,110,112,115,117,120,122,125,
                            128,130,133,135,138,140,143,145,148,150,
                            153,156,158,161,163,166,168,171,173,176,
                            179,181,184,186,189,191,194,196,199,201,
                            204,207,209,212,214,217,219,222,224,227,
                            230,232,235,237,240,242,245,247,250,252,
                            252};

short wDecrease[WAVENUM + 1] = {252,250,247,245,242,240,237,235,232,230,
                            227,224,222,219,217,214,212,209,207,204,
                            201,199,196,194,191,189,186,184,181,179,
                            176,173,171,168,166,163,161,158,156,153,
                            150,148,145,143,140,138,135,133,130,128,
                            125,122,120,117,115,112,110,107,105,102,
                            99,97,94,92,89,87,84,82,79,77,
                            74,71,69,66,64,61,59,56,54,51,
                            48,46,43,41,38,36,33,31,28,26,
                            23,20,18,15,13,10,8,5,3,0,
                            0};

short wIncreaseDecrease[WAVENUM + 1] = {5,10,15,20,26,31,36,41,46,51,
                                        56,61,66,71,77,82,87,92,97,102,
                                        107,112,117,122,128,133,138,143,148,153,
                                        158,163,168,173,179,184,189,194,199,204,
                                        209,214,219,224,230,235,240,245,250,255,
                                        255,250,245,240,235,230,224,219,214,209,
                                        204,199,194,189,184,179,173,168,163,158,
                                        153,148,143,138,133,128,122,117,112,107,
                                        102,97,92,87,82,77,71,66,61,56,
                                        51,46,41,36,31,26,20,15,10,5,
                                        5};


#endif

#endif
