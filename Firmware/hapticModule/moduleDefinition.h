#ifndef _MODULEDEFINITION_H_
#define _MODULEDEFINITION_H_

/****
 *  Firmware for HaptiCraft (IEEE TVCG 2026)
 *  Author: Chaeyong Park
 *  Copyright (c): 2026. AXIS Laboratory, Korea University
 *  WEMOS D1 MINI (ESP8266)
 *  
 *  Module definition for the modular haptic system
 * 
 *  Types of modules:
 *  NORM: normal module for detecting connections (00-09)
 *  VIBERM: vibration with ERM actuators (10-19)
 *  THERM: thermal sensation (20-29)
 *  VMASS: variable moments (30-39)
 *  IMPACT: impact sensation (40-49)
 *  BUTTON: button for the input interaction (80-89)
 *  TOUCH: touch for the input interaction (90-99)
 *  VSTIFF: variable stiffness (60-69)
 */

#define VSTIFF

/* Before uploading to the mesh, you need to set the node name depending on the module type*/
String nodeName = "61";

#define MESH_PREFIX       "modularHaptics"
#define MESH_PASSWORD     "somethingSneaky"
#define MESH_PORT         5555

#define DIGIT_VIBERM  8
#define DIGIT_THERM   8
#define DIGIT_VMASS   6
#define DIGIT_IMPACT  5

#endif