/****
 *  Firmware for HaptiCraft (IEEE TVCG 2026)
 *  Author: Chaeyong Park
 *  Copyright (c): 2026. AXIS Laboratory, Korea University
 *  WEMOS D1 MINI (ESP8266)
 *
 *  Haptic modules
 * 
 *  Vibration (ERM) parameters: VABBCCDD (10~19)
 *  A: Envelope (0:flat, 1:increase, 2:decrease, 3:in-decrease, 4:de-increase)
 *  BB: Intensity (00-10)
 *  CC: Duration (x100ms; 00-99)
 *  DD: Initial delay (x100ms; 00-99)
 *  Example: V1101005 (flat, full intensity, 1second, after 0.5s)
 *
 *  Thermal Parameters: TABBCCDD (20~29)
 *  A: Direction (0: cool; 1: hot)
 *  BB: Intensity (00-10)
 *  CC: Duration (x100ms; 00-99)
 *  DD: Initial delay (x100ms; 00-99)
 *  Example: T1105010 (hot, full intensity, 5seconds, after 1s)
 *
 *  Variable Interia Parameters: MABBCC (30~39)
 *  A: Direction (0: upward, 1: downward)
 *  BB: Distnace (00-99: mm)
 *  CC: Initial delay (x100ms; 00-99)
 *  Example: M01005 (10mm movement to upward direction, after 0.5s)
 * 
 *  Impact Parameters: PAABB (40~49)
 *  AA: Initial delay (x100ms; 00-99)
 *  BB: Initial delay (x100ms; 00-99)
 *  Example: P10 (Impact rendering after 1s)
 *  
 *  Touch (50~59)
 *  
 *  Variable Stiffness Parameters: SAABBCC (60-69)
 *  AA: Intensity (00-10)
 *  BB: Duration (00-99)
 *  CC: Initial Delay (x100ms; 00-99)
 *  Example: S100500 (Full intensity, 5s, after 0.5s)
 *  
 *  Vibration (LRA) parameters: LAABBCC (90~99) *Not implemented yet
 *  AA: Intensity (00-10)
 *  BB: Frequency (Hz x 10; 00-99)
 *  CC: Duration (ms x 100; 00-99)
 *  Example: L101010
 *  
 *  I: Information
 *  D: Check for delete
 *  H: Handshake for checking
 */

#include "painlessMesh.h"
#include "moduleDefinition.h"
#include "waveform.h"
#include <Vector.h>

#define ELEMENT_COUNT_MAX 20

Scheduler userScheduler;
painlessMesh mesh;
bool isOn = false;
uint32_t masterID;

enum PHASE {
  PHASE_DELAY,
  PHASE_HAPTICGEN
};

void connectionChecker();
Task taskConnectionChecker(500 * TASK_MILLISECOND, TASK_FOREVER, &connectionChecker); 

void connectionChecker() {
  if (isOn) {
    digitalWrite(LED_BUILTIN, LOW);
  } else {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  isOn = !isOn;
}

/* Vibration (ERM) */
#ifdef VIBERM
struct sVibrationERM {
  int pinPS;
  int pinDIR;
  int pinEN;
  int pinPWM;
  int envelope;
  int intensity;
  int duration;
  int initialDelay;
  PHASE phase;
  int cnt;
  String storage_array[ELEMENT_COUNT_MAX];
  Vector<String> commands;
  int commandIndex;
  short *waveform;
};
struct sVibrationERM hapticParams;

/* Haptic rendering function for ERM vibration */
void hapticRendering();
Task taskHapticRendering(10 * TASK_MILLISECOND, TASK_FOREVER, &hapticRendering); 

/* Reset haptic parameters for ERM vibration */
void resetHapticParams(struct sVibrationERM *_hapticParams) {
  Serial.println("Reset Vibration Parameters");
  _hapticParams->envelope = 0;
  _hapticParams->intensity = 0;
  _hapticParams->duration = 0;
  _hapticParams->initialDelay = 0;
  _hapticParams->phase = PHASE_DELAY;
  _hapticParams->cnt = 0;
  _hapticParams->waveform = NULL;

  pinMode(_hapticParams->pinPS, OUTPUT);
  pinMode(_hapticParams->pinEN, OUTPUT);
  pinMode(_hapticParams->pinDIR, OUTPUT);
  pinMode(_hapticParams->pinPWM, OUTPUT);
  digitalWrite(_hapticParams->pinPS, LOW);
  digitalWrite(_hapticParams->pinPWM, LOW);
  digitalWrite(_hapticParams->pinDIR, LOW);
  analogWrite(_hapticParams->pinEN, 0);
  pinMode(_hapticParams->pinPS, INPUT);
  pinMode(_hapticParams->pinEN, INPUT);
  pinMode(_hapticParams->pinDIR, INPUT);
  pinMode(_hapticParams->pinPWM, INPUT);
}

/* Reset the commands vector */
void resetCommandsVector(struct sVibrationERM *_hapticParams) {
  _hapticParams->commands.clear();
  _hapticParams->commandIndex = 0;
}

/* Haptic rendering function for ERM vibration */
void hapticRendering() {
  if (hapticParams.phase == PHASE_DELAY) {
    if (hapticParams.cnt == 0) {
      bool isCorrect;
      isCorrect = hapticCommandParser(hapticParams.commands[hapticParams.commandIndex]);
      if (!isCorrect) {
        resetHapticParams(&hapticParams);
        resetCommandsVector(&hapticParams);
        taskHapticRendering.disable();
        Serial.println("Wrong input command");
        return;
      }
    }
    if (hapticParams.cnt == hapticParams.initialDelay) {
      hapticParams.phase = PHASE_HAPTICGEN;
      hapticParams.cnt = 0;
    } else {
      hapticParams.cnt++;
    }
  }
  if (hapticParams.phase == PHASE_HAPTICGEN) {
    if (hapticParams.cnt == 0) {
      pinMode(hapticParams.pinPS, OUTPUT);
      pinMode(hapticParams.pinEN, OUTPUT);
      pinMode(hapticParams.pinDIR, OUTPUT);
      pinMode(hapticParams.pinPWM, OUTPUT);

      if (hapticParams.envelope == 0) { // flat
        hapticParams.waveform = wFlat;
      } else if (hapticParams.envelope == 1) { // Increase
        hapticParams.waveform = wIncrease;
      } else if (hapticParams.envelope == 2) { // Decrease
        hapticParams.waveform = wDecrease;
      } else if (hapticParams.envelope == 3) { // Decrease
        hapticParams.waveform = wIncreaseDecrease;
      }

      int arrNum = (hapticParams.cnt + 1) * WAVENUM / hapticParams.duration;
      int _intensity = (int)((float)hapticParams.intensity/10 * hapticParams.waveform[arrNum] + MINIMUNVALUE);
      digitalWrite(hapticParams.pinPS, HIGH);
      digitalWrite(hapticParams.pinPWM, HIGH);
      digitalWrite(hapticParams.pinDIR, LOW);
      
      analogWrite(hapticParams.pinEN, _intensity);
      hapticParams.cnt++;
    } else if (hapticParams.cnt >= hapticParams.duration) {
      resetHapticParams(&hapticParams);
      taskHapticRendering.disable();
      if (hapticParams.commandIndex < hapticParams.commands.size() - 1) {
        hapticParams.commandIndex++;
        taskHapticRendering.enable();
      } else {
        resetCommandsVector(&hapticParams);
        Serial.println("Vibration End");
      }
    } else {
      int arrNum = (hapticParams.cnt + 1) * WAVENUM / hapticParams.duration;
      int _intensity = (int)((float)hapticParams.intensity/10 * hapticParams.waveform[arrNum] + MINIMUNVALUE);
      analogWrite(hapticParams.pinEN, _intensity);
      hapticParams.cnt++;
    }
  }
}
#endif

/* Vibration (LRA) 
 * But, not implemented yet
 */
#ifdef VIBLRA
struct sVibrationLRA {
};
struct sVibrationLRA hapticParams;
#endif

/* Thermal Sensation */
#ifdef THERM
struct sThermal {
  int pinPS;
  int pinDIR;
  int pinEN;
  int pinPWM;
  int direction;
  int envelope;
  int intensity;
  int duration;
  int initialDelay;
  PHASE phase;
  int cnt;
  String storage_array[ELEMENT_COUNT_MAX];
  Vector<String> commands;
  int commandIndex;
  short *waveform;
};
struct sThermal hapticParams;

/* Haptic rendering function for thermal sensation */
void hapticRendering();
Task taskHapticRendering(100 * TASK_MILLISECOND, TASK_FOREVER, &hapticRendering); 

/* Reset haptic parameters for thermal sensation */
void resetHapticParams(struct sThermal *_hapticParams) {
  Serial.println("Reset Thermal Parameters");
  _hapticParams->envelope = 0;
  _hapticParams->direction = 0;
  _hapticParams->intensity = 0;
  _hapticParams->duration = 0;
  _hapticParams->cnt = 0;
  _hapticParams->initialDelay = 0;
  _hapticParams->phase = PHASE_DELAY;
  _hapticParams->cnt = 0;
  _hapticParams->waveform = NULL;

  pinMode(_hapticParams->pinPS, OUTPUT);
  pinMode(_hapticParams->pinEN, OUTPUT);
  pinMode(_hapticParams->pinDIR, OUTPUT);
  pinMode(_hapticParams->pinPWM, OUTPUT);
  digitalWrite(_hapticParams->pinPS, LOW);
  digitalWrite(_hapticParams->pinPWM, LOW);
  digitalWrite(_hapticParams->pinDIR, LOW);
  analogWrite(_hapticParams->pinEN, 0);
  pinMode(_hapticParams->pinPS, INPUT);
  pinMode(_hapticParams->pinEN, INPUT);
  pinMode(_hapticParams->pinDIR, INPUT);
  pinMode(_hapticParams->pinPWM, INPUT);
}

/* Reset the commands vector */
void resetCommandsVector(struct sThermal *_hapticParams) {
  _hapticParams->commands.clear();
  _hapticParams->commandIndex = 0;
}

/* Haptic rendering function for thermal sensation */
void hapticRendering() {
  if (hapticParams.phase == PHASE_DELAY) {
    if (hapticParams.cnt == 0) {
      bool isCorrect;
      isCorrect = hapticCommandParser(hapticParams.commands[hapticParams.commandIndex]);
      if (!isCorrect) {
        resetHapticParams(&hapticParams);
        resetCommandsVector(&hapticParams);
        taskHapticRendering.disable();
        Serial.println("Wrong input command");
        return;
      }
    }
    if (hapticParams.cnt == hapticParams.initialDelay) {
      hapticParams.phase = PHASE_HAPTICGEN;
      hapticParams.cnt = 0;
    } else {
      hapticParams.cnt++;
    }
  }
  if (hapticParams.phase == PHASE_HAPTICGEN) {
    if (hapticParams.cnt == 0) {
      pinMode(hapticParams.pinPS, OUTPUT);
      pinMode(hapticParams.pinEN, OUTPUT);
      pinMode(hapticParams.pinDIR, OUTPUT);
      pinMode(hapticParams.pinPWM, OUTPUT);

      int _intensity = hapticParams.intensity * 20;
      digitalWrite(hapticParams.pinPS, HIGH);
      digitalWrite(hapticParams.pinPWM, HIGH);
      if (hapticParams.envelope == 0) {  // !CHANGE!
        digitalWrite(hapticParams.pinDIR, LOW); // 22 - HIGH, 21 - LOW
      } else if (hapticParams.envelope == 1) {
        digitalWrite(hapticParams.pinDIR, HIGH);
      }
      analogWrite(hapticParams.pinEN, _intensity);
      hapticParams.cnt++;
    } else if (hapticParams.cnt >= hapticParams.duration) {
      resetHapticParams(&hapticParams);
      taskHapticRendering.disable();
      if (hapticParams.commandIndex < hapticParams.commands.size() - 1) {
        hapticParams.commandIndex++;
        taskHapticRendering.enable();
      } else {
        resetCommandsVector(&hapticParams);
        Serial.println("Thermal Sensation End");
      }
    } else {
      int _intensity = hapticParams.intensity * 20;
      analogWrite(hapticParams.pinEN, _intensity);

      hapticParams.cnt++;
    }
  }
}
#endif

/* Variable Inertia */
#ifdef VMASS
struct sVariableMass {
  int pinPS;
  int pinDIR;
  int pinEN;
  int pinPWM;
  int direction;
  int distance;
  int initialDelay;
  int cnt;
  PHASE phase;
  String storage_array[ELEMENT_COUNT_MAX];
  Vector<String> commands;
  int commandIndex;
};
struct sVariableMass hapticParams;;

/* Haptic rendering function for variable inertia */
void hapticRendering();
Task taskHapticRendering(100 * TASK_MILLISECOND, TASK_FOREVER, &hapticRendering);

/* Reset haptic parameters for variable inertia */
void resetHapticParams(struct sVariableMass *_hapticParams) {
  Serial.println("Reset Variable Mass Parameters");
  _hapticParams->direction = 0;
  _hapticParams->distance = 0;
  _hapticParams->cnt = 0;
  _hapticParams->initialDelay = 0;
  _hapticParams->phase = PHASE_DELAY;

  pinMode(_hapticParams->pinPS, OUTPUT);
  pinMode(_hapticParams->pinEN, OUTPUT);
  pinMode(_hapticParams->pinDIR, OUTPUT);
  pinMode(_hapticParams->pinPWM, OUTPUT);
  digitalWrite(_hapticParams->pinPS, LOW);
  digitalWrite(_hapticParams->pinPWM, LOW);
  digitalWrite(_hapticParams->pinDIR, LOW);
  analogWrite(_hapticParams->pinEN, 0);
  pinMode(_hapticParams->pinPS, INPUT);
  pinMode(_hapticParams->pinEN, INPUT);
  pinMode(_hapticParams->pinDIR, INPUT);
  pinMode(_hapticParams->pinPWM, INPUT);
}

/* Reset the commands vector */
void resetCommandsVector(struct sVariableMass *_hapticParams) {
  _hapticParams->commands.clear();
  _hapticParams->commandIndex = 0;
}

/* Haptic rendering function for variable inertia */
void hapticRendering() {
  if (hapticParams.phase == PHASE_DELAY) {
    if (hapticParams.cnt == 0) {
      bool isCorrect;
      isCorrect = hapticCommandParser(hapticParams.commands[hapticParams.commandIndex]);
      if (!isCorrect) {
        resetHapticParams(&hapticParams);
        resetCommandsVector(&hapticParams);
        taskHapticRendering.disable();
        return;
      }
    }
    if (hapticParams.cnt == hapticParams.initialDelay) {
      hapticParams.phase = PHASE_HAPTICGEN;
      hapticParams.cnt = 0;
    } else {
      hapticParams.cnt++;
    }
  }
  if (hapticParams.phase == PHASE_HAPTICGEN) {
    if (hapticParams.cnt == 0) {
      pinMode(hapticParams.pinPS, OUTPUT);
      pinMode(hapticParams.pinEN, OUTPUT);
      pinMode(hapticParams.pinDIR, OUTPUT);
      pinMode(hapticParams.pinPWM, OUTPUT);

      digitalWrite(hapticParams.pinPS, HIGH);
      digitalWrite(hapticParams.pinPWM, HIGH);
      if (hapticParams.direction == 0 || hapticParams.direction == 2) {
        digitalWrite(hapticParams.pinDIR, LOW);
      } else {
        digitalWrite(hapticParams.pinDIR, HIGH);
      }
      analogWrite(hapticParams.pinEN, 255);

      hapticParams.cnt++;
    } else if (hapticParams.cnt >= hapticParams.distance) {
      resetHapticParams(&hapticParams);
      taskHapticRendering.disable();
      if (hapticParams.commandIndex < hapticParams.commands.size() - 1) {
        hapticParams.commandIndex++;
        taskHapticRendering.enable();
      } else {
        resetCommandsVector(&hapticParams);
        Serial.println("Vmass End");
      }
    } else {
      hapticParams.cnt++;
      if (hapticParams.direction == 2) { // Initialization
        int voltage_ = analogRead(A0);
        Serial.println(voltage_);
        if (voltage_ > 20) {
          resetHapticParams(&hapticParams);
          resetCommandsVector(&hapticParams);
          taskHapticRendering.disable();
          Serial.println("Vmass End");
        }
      }
    }
  }
}
#endif

/* Impact Sensation */
#ifdef IMPACT
struct sImpact {
  int pin;
  int isGen;
  int cnt;
  int duration;
  int initialDelay;
  int impCnt;
  PHASE phase;
  bool direction = true;
  String storage_array[ELEMENT_COUNT_MAX];
  Vector<String> commands;
  int commandIndex;
};
struct sImpact hapticParams;

const int impactDuration = 5;
/* Haptic rendering function for impact sensation */
void hapticRendering();
Task taskHapticRendering(10 * TASK_MILLISECOND, TASK_FOREVER, &hapticRendering); 

/* Reset haptic parameters for impact sensation */
void resetHapticParams(struct sImpact *_hapticParams) {
  Serial.println("Reset Impact Parameters");
  _hapticParams->isGen = false;
  _hapticParams->cnt = false;
  _hapticParams->initialDelay = 0;
  _hapticParams->impCnt = 0;
  _hapticParams->phase = PHASE_DELAY;
}

/* Reset the commands vector */
void resetCommandsVector(struct sImpact *_hapticParams) {
  _hapticParams->commands.clear();
  _hapticParams->commandIndex = 0;
}

/* Haptic rendering function for impact sensation */
void hapticRendering() {
  if (hapticParams.phase == PHASE_DELAY) {
    if (hapticParams.cnt == 0) {
      hapticCommandParser(hapticParams.commands[hapticParams.commandIndex]);
    }
    if (hapticParams.cnt == hapticParams.initialDelay*10) {
      hapticParams.phase = PHASE_HAPTICGEN;
      hapticParams.cnt = 0;
    } else {
      hapticParams.cnt++;
    }
  }
  if (hapticParams.phase == PHASE_HAPTICGEN) {
    if (hapticParams.cnt == 0) {
      pinMode(hapticParams.pin, OUTPUT);
      if (hapticParams.direction) {
        digitalWrite(hapticParams.pin, HIGH);
      } else {
        digitalWrite(hapticParams.pin, LOW);
      }
      hapticParams.cnt++;
    } else if (hapticParams.cnt == impactDuration) {
      pinMode(hapticParams.pin, INPUT);
      hapticParams.cnt++;
    } else if (hapticParams.cnt == impactDuration + 1) {
      resetHapticParams(&hapticParams);
      hapticParams.direction = !hapticParams.direction;
      taskHapticRendering.disable();
      if (hapticParams.commandIndex < hapticParams.commands.size() - 1) {
        hapticParams.commandIndex++;
        taskHapticRendering.enable();
      } else {
        resetCommandsVector(&hapticParams);
        Serial.println("Impact End");
      }
    } else {
      hapticParams.cnt++;
    }
  }
}
#endif

/* Variable Stiffness */
#ifdef VSTIFF
struct sVariableStiff {
  int pinPS;
  int pinDIR;
  int pinEN;
  int pinPWM;
  int envelope;
  int intensity;
  int duration;
  int initialDelay;
  PHASE phase;
  int cnt;
  String storage_array[ELEMENT_COUNT_MAX];
  Vector<String> commands;
  int commandIndex;
};
struct sVariableStiff hapticParams;

/* Haptic rendering function for variable stiffness */
void hapticRendering();
Task taskHapticRendering(100 * TASK_MILLISECOND, TASK_FOREVER, &hapticRendering); 

/* Reset haptic parameters for variable stiffness */
void resetHapticParams(struct sVariableStiff *_hapticParams) {
  Serial.println("Reset VStiff Parameters");
  _hapticParams->envelope = 0;
  _hapticParams->intensity = 0;
  _hapticParams->duration = 0;
  _hapticParams->cnt = 0;
  _hapticParams->initialDelay = 0;
  _hapticParams->phase = PHASE_DELAY;
  _hapticParams->cnt = 0;

  pinMode(_hapticParams->pinPS, OUTPUT);
  pinMode(_hapticParams->pinEN, OUTPUT);
  pinMode(_hapticParams->pinDIR, OUTPUT);
  pinMode(_hapticParams->pinPWM, OUTPUT);
  digitalWrite(_hapticParams->pinPS, LOW);
  digitalWrite(_hapticParams->pinPWM, LOW);
  digitalWrite(_hapticParams->pinDIR, LOW);
  analogWrite(_hapticParams->pinEN, 0);
  pinMode(_hapticParams->pinPS, INPUT);
  pinMode(_hapticParams->pinEN, INPUT);
  pinMode(_hapticParams->pinDIR, INPUT);
  pinMode(_hapticParams->pinPWM, INPUT);
}

/* Reset the commands vector */
void resetCommandsVector(struct sVariableStiff *_hapticParams) {
  _hapticParams->commands.clear();
  _hapticParams->commandIndex = 0;
}

/* Haptic rendering function for variable stiffness */
void hapticRendering() {
  if (hapticParams.phase == PHASE_DELAY) {
    if (hapticParams.cnt == 0) {
      bool isCorrect;
      isCorrect = hapticCommandParser(hapticParams.commands[hapticParams.commandIndex]);
      if (!isCorrect) {
        resetHapticParams(&hapticParams);
        resetCommandsVector(&hapticParams);
        taskHapticRendering.disable();
        Serial.println("Wrong input command");
        return;
      }
    }
    if (hapticParams.cnt == hapticParams.initialDelay) {
      hapticParams.phase = PHASE_HAPTICGEN;
      hapticParams.cnt = 0;
    } else {
      hapticParams.cnt++;
    }
  }
  if (hapticParams.phase == PHASE_HAPTICGEN) {
    if (hapticParams.cnt == 0) {
      pinMode(hapticParams.pinPS, OUTPUT);
      pinMode(hapticParams.pinEN, OUTPUT);
      pinMode(hapticParams.pinDIR, OUTPUT);
      pinMode(hapticParams.pinPWM, OUTPUT);

      int _intensity = hapticParams.intensity * 20;
      digitalWrite(hapticParams.pinPS, HIGH);
      digitalWrite(hapticParams.pinPWM, HIGH);
      digitalWrite(hapticParams.pinDIR, HIGH);
      analogWrite(hapticParams.pinEN, _intensity);
      hapticParams.cnt++;
    } else if (hapticParams.cnt >= 50) {
      resetHapticParams(&hapticParams);
      taskHapticRendering.disable();
      if (hapticParams.commandIndex < hapticParams.commands.size() - 1) {
        hapticParams.commandIndex++;
        taskHapticRendering.enable();
      } else {
        resetCommandsVector(&hapticParams);
        Serial.println("Vstiff End");
      }
    } else {
      hapticParams.cnt++;
    }
  }
}
#endif

/* Touch */
#ifdef TOUCH
int touchPin;
int capThreshold = 8;
bool isTouched = false;

/* Check capacitance for touch detection */
void checkCapacitacne();
Task taskReadCapacitance(100 * TASK_MILLISECOND, TASK_FOREVER, &checkCapacitacne); // every 0.25 seconds

/* Check capacitance for touch detection */
void checkCapacitacne() {
  int cap = analogRead(touchPin);
  Serial.println(cap);

  if (!isTouched && cap >= capThreshold) {
    isTouched = true;
    mesh.sendBroadcast("CO"); // Contact ON
  } else if (isTouched && cap < capThreshold) {
    isTouched = false;
    mesh.sendBroadcast("CF"); // Contact OFF
  }
}
#endif

/* Button */
#ifdef BUTTON
int buttonPin;
int buttonThreshold = 100;
bool isPressed = false;

/* Check button press status */
void checkButtonPress();
Task taskCheckButtonPress(50 * TASK_MILLISECOND, TASK_FOREVER, &checkButtonPress); // every 0.25 seconds

/* Check button press status */
void checkButtonPress() {
  int _button = analogRead(buttonPin);
  

  if (!isPressed && _button >= buttonThreshold) {
    isPressed = true;
    mesh.sendBroadcast("CO"); // Contact ON
  } else if (isPressed && _button < buttonThreshold) {
    isPressed = false;
    mesh.sendBroadcast("CF"); // Contact OFF
  }
}
#endif

/* Initialize variables for haptic parameters and commands */
void initVariables() {
  #if defined(VIBERM) || defined(THERM) || defined(VMASS) || defined(IMPACT) || defined(VSTIFF)
  hapticParams.commands.setStorage(hapticParams.storage_array);
  resetHapticParams(&hapticParams);
  resetCommandsVector(&hapticParams);
  #endif
  #ifdef IMPACT
  hapticParams.direction = false;
  hapticParams.commands.push_back("P00");
  hapticParams.commands.push_back("P00");
  taskHapticRendering.enable();
  #endif
}

/* Parse the received command and set haptic parameters accordingly */
bool hapticCommandParser(String _command) {
  String type = _command.substring(0, 1);
  Serial.println(_command);

  #ifdef VIBERM
  if (type == "E") {
    hapticParams.envelope = _command.substring(1, 2).toInt();
    hapticParams.intensity = _command.substring(2, 4).toInt();
    hapticParams.duration = _command.substring(4, 6).toInt() * 10;
    hapticParams.initialDelay = _command.substring(6, 8).toInt() * 10;

    /* Debug */
    Serial.print(hapticParams.envelope);
    Serial.print(" ");
    Serial.print(hapticParams.intensity);
    Serial.print(" ");
    Serial.print(hapticParams.duration * 10);
    Serial.print("ms ");
    Serial.print(hapticParams.initialDelay * 10);
    Serial.println("ms");

    if (hapticParams.envelope > 3 || hapticParams.intensity > 10 || hapticParams.duration == 0) {
      return false;
    } else {
      return true;
    }
  }
  #endif
  #ifdef THERM
  if (type == "T") {
    hapticParams.envelope = _command.substring(1, 2).toInt();
    hapticParams.intensity = _command.substring(2, 4).toInt();
    hapticParams.duration = _command.substring(4, 6).toInt();
    hapticParams.initialDelay = _command.substring(6, 8).toInt();

    
    /* Debug */
    if (hapticParams.envelope == 0) { // !CHANGE!
      Serial.print("Cool ");
    }
    else {
      Serial.print("Hot ");
    }
    Serial.print(hapticParams.intensity);
    Serial.print(" ");
    Serial.print(hapticParams.duration * 100);
    Serial.print("ms ");
    Serial.print(hapticParams.initialDelay * 100);
    Serial.println("ms");

    if (hapticParams.envelope > 1 || hapticParams.intensity > 10 || hapticParams.duration == 0) {
      return false;
    } else {
      return true;
    }
  }
  #endif
  #ifdef VMASS
  if (type == "M") {
    hapticParams.direction = _command.substring(1, 2).toInt();
    hapticParams.distance = _command.substring(2, 4).toInt();
    hapticParams.initialDelay = _command.substring(4, 6).toInt();

    /* Debug */
    if (hapticParams.direction == 0) {
      Serial.print("Upward ");
    }
    else {
      Serial.print("Downward ");
    }
    Serial.print(hapticParams.distance);
    Serial.print("mm ");
    Serial.print(hapticParams.initialDelay * 100);
    Serial.println("ms");

    if (hapticParams.distance > 80) {
      return false;
    } else {
      return true;
    }
  }
  #endif
  #ifdef IMPACT
  if (type == "P") {
    hapticParams.initialDelay = _command.substring(1, 3).toInt();
    hapticParams.impCnt = _command.substring(3, 5).toInt();

    if (hapticParams.impCnt != 0) {
      for (int i = 0; i < hapticParams.impCnt - 1; i++) {
        // hapticParams.commands.push_back(_command.substring(0, 3) + "00");
        hapticParams.commands.push_back("P0000");
      }
    }
    /* Debug */
    Serial.print("Impact Generation ");
    Serial.print(hapticParams.initialDelay * 100);
    Serial.println("ms");

    return true;
  }
  #endif
  #ifdef VSTIFF
  if (type == "S") {
    hapticParams.intensity = _command.substring(1, 3).toInt();
    hapticParams.initialDelay = _command.substring(3, 5).toInt() * 10;

    /* Debug */
    Serial.print(hapticParams.intensity);
    Serial.print(" ");
    Serial.print(hapticParams.initialDelay * 10);
    Serial.println("ms");

    if (hapticParams.intensity > 10) {
      return false;
    } else {
      return true;
    }
  }
  #endif
}

/* Callback function for received messages */
void receivedCallback(uint32_t _nodeID, String &msg) {
  String command = msg.substring(0, 1);

  if (command == "D") {
    mesh.sendBroadcast("H"); // Handshaking for check the node
  } else if (command == "I") {
    mesh.sendBroadcast("N" + nodeName);
    Serial.println("PLEASE GIVE ME YOUR NAME FOR REGISTRATION");
    
    /* Connection */
    #if defined(NORM)
    analogWrite(LED_BUILTIN, 192);
    #else
    analogWrite(LED_BUILTIN, 128);
    #endif
    taskConnectionChecker.disable();
    masterID = _nodeID;
  } else if (command == "E" || command == "T" || command == "M" || command == "P" || command == "S") {
    hapticCommandParser(msg);
  } else if (command == "Q") {
    initVariables();
  } else if (command == "W") {
    int _resolution = msg.substring(1, 5).toInt();
    analogWriteFreq(_resolution);
    
    Serial.print("Change PWM resolution: ");
    Serial.println(_resolution);
  } else if (command == "$") {
    #if defined(VIBERM) || defined(THERM) || defined(VMASS) || defined(IMPACT) || defined(VSTIFF)
    String parsingCommand;

    int prevIndex = 0;
    int curIndex;
    bool isLast = false;
    bool isClear = false;

    while(1) {
      if (prevIndex == 0) {
        curIndex = msg.indexOf(",", prevIndex);
      } else {
        curIndex = msg.indexOf(",", prevIndex + 1);
      }
      
      if (curIndex == -1) {
        curIndex = msg.length();
        isLast = true;
      }
      
      parsingCommand = msg.substring(prevIndex + 1, curIndex);
      Serial.println(parsingCommand);

      if (parsingCommand.substring(0, 2) == nodeName) {
        if (!isClear) {
          taskHapticRendering.disable();
          resetHapticParams(&hapticParams);
          resetCommandsVector(&hapticParams);
          isClear = true;
        }
        hapticParams.commands.push_back(parsingCommand.substring(2, parsingCommand.length()));
      }
      if (isLast) {
        break;
      }
      prevIndex = curIndex;
    }

    if (hapticParams.commands.size() > 0) {
      taskHapticRendering.enable();
    }
    #endif
  } 
}

/* Callback function for new connections */
void newConnectionCallback(uint32_t _nodeID) {
    Serial.printf("NEW CONNECTION! ID: %u\n", _nodeID);
}

/* Callback function for dropped connections */
void droppedConnectionCallback(uint32_t _nodeID) {
  Serial.printf("DROPPED CONNECTIONS %u\n", _nodeID);
  if (_nodeID == masterID) {
    taskConnectionChecker.enable();
  }
}

/* Setup function */
void setup() {
  Serial.begin(115200);
  Serial.setTimeout(5);
  pinMode(LED_BUILTIN, OUTPUT);

  /* Setup pins */
  #if defined(VIBERM) || defined(THERM) || defined(VMASS) || defined(VSTIFF)
  hapticParams.pinPS = D1;
  hapticParams.pinEN = D5;
  hapticParams.pinDIR = D6;
  hapticParams.pinPWM = D8;
  userScheduler.addTask(taskHapticRendering);
  #endif

  #ifdef VIBERM
  analogWriteFreq(5000);
  #endif
  #ifdef THERM
  analogWriteFreq(100);
  #endif
  #ifdef VMASS
  analogWriteFreq(500);
  #endif
  #ifdef VSTIFF
  analogWriteFreq(100);
  #endif
  #ifdef IMPACT
  hapticParams.pin = D1;
  userScheduler.addTask(taskHapticRendering);
  #endif
  #ifdef TOUCH
  touchPin = A0;
  userScheduler.addTask(taskReadCapacitance);
  taskReadCapacitance.enable();
  #endif
  #ifdef BUTTON
  buttonPin = A0;
  userScheduler.addTask(taskCheckButtonPress);
  taskCheckButtonPress.enable();
  #endif
  initVariables();
  userScheduler.addTask(taskConnectionChecker);

  mesh.setDebugMsgTypes(ERROR | STARTUP);  // set before init() so that you can see startup messages

  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onDroppedConnection(&droppedConnectionCallback);

  taskConnectionChecker.enable();
}

void loop() {
  mesh.update();
}
