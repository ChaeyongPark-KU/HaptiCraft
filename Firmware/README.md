# HaptiCraft Firmware

This folder contains the ESP8266-based firmware for the **HaptiCraft** system. The system operates on a **Master-Slave architecture** using a Wi-Fi mesh network to deliver synchronized, multimodal haptic feedback.

## System Architecture

The firmware is divided into two primary roles:

### 1. Master Module (`/masterModule`)
The Master Module (referred to as the **Communication Module** in the paper) serves as the central bridge between the PC and the haptic controller.
* **Serial Gateway**: Receives high-level haptic commands from the PC via Serial communication (115200 baud).
* **Mesh Management**: Manages a list of all physically connected haptic modules, assigning names and tracking their connection status.
* **Connection Checker**: Periodically broadcasts check signals ("D") to verify if modules are still attached; it automatically removes timed-out nodes from the registry.
* **Command Broadcasting**: Parses PC commands and broadcasts them to the entire Wi-Fi mesh.

### 2. Haptic Modules (`/hapticModule`)
Every individual haptic module runs the same unified firmware, with its specific behavior defined by a module-type constant.
* **Multimodal Execution**: Supports Vibration (ERM), Thermal, Variable Inertia, Impact, and Variable Stiffness.Each module contains its own scheduler to handle timing-critical tasks like PWM generation for waveforms.
* **Input Detection**: Input modules (Touch/Button) detect user interaction and send status updates ("CO" for contact on, "CF" for contact off) back to the Master.


## Communication Flow
<p align="center">
   <img src="https://github.com/ChaeyongPark-KU/HaptiCraft/blob/main/img/implementation_protocol.png" width="60%">
</p>

Refer to the diagram below for the data transmission path:

1.  **PC &rarr; Master**: Sends encoded strings (e.g., `$11V101000`) via Wire.
2.  **Master &rarr; Mesh**: Broadcasts the command wirelessly to all nodes.
3.  **Haptic Module**: If the `nodeName` matches the prefix in the command, the module parses the parameters and triggers the actuator.

## Configuration & Upload

### 1. Prerequisites
* **Hardware**: Wemos D1 Mini (ESP8266).
* **Libraries**: 
    * `painlessMesh` (for Wi-Fi Mesh networking).
    * `ArduinoVector` (for node management).
    * `ArduinoJson` (required by painlessMesh).

### 2. Defining Module Types
Before uploading to a **Haptic Module**, you must define its identity in `moduleDefinition.h`:

```cpp
// moduleDefinition.h
#define VIBERM // Uncomment the specific module type (VIBERM, THERM, IMPACT, etc.)
String nodeName = "11"; // Set a unique 2-digit ID for the module
```

Here is the updated **Command Format** section for your `README.md`. This version specifically reflects the broadcasting logic found in your `masterModule.ino` and the parsing behavior in `hapticModule.ino`.

---

### 3. Command Format & Broadcasting logic

HaptiCraft uses a **Global Broadcast** strategy to ensure synchronized multimodal effects. The Master Module receives a concatenated command string from the PC and sends it to all nodes simultaneously.

Here is the updated **Command Format & Broadcasting Logic** section for your `README.md`. This version clarifies that the Master Module acts as a transparent relay for the aggregated command string sent by the PC[cite: 919, 922].

---

### 3. Command Format & Broadcasting Logic

HaptiCraft utilizes a **Global Broadcast** strategy to ensure synchronized multimodal effects across multiple modules.

#### **Transmission Protocol**
1.  **PC → Master**: The PC generates a single aggregated string containing all haptic instructions for the intended modules. This string starts with the `$` prefix and appends information for every module to be operated.
2.  **Master → Mesh**: The Master Module identifies the `$` header from the serial input and **broadcasts the command exactly as received** to the entire Wi-Fi mesh network using `mesh.sendBroadcast(inString)`.
3.  **Module-Side Parsing**:
    * Every Haptic Module in the mesh receives the full broadcasted string.
    * Each module parses the string to find segments that begin with its own unique `nodeName` (e.g., "11").
    * If a match is found, the module extracts the specific haptic parameters, stores them in a local command buffer, and executes the rendering task.

#### **Command Syntax**
The aggregated string sent by the PC follows this structure: `$[ModuleID][Command],[ModuleID][Command]...`

| Module Type | Haptic Command (Example) | Description |
| :--- | :--- | :--- |
| **Vibration** | `11V[Env][Int][Dur][Del]` | `11V1101005`: Flat, max intensity, 1s duration, 0.5s delay. |
| **Thermal** | `21T[Dir][Int][Dur][Del]` | `21T1105010`: Hot, max intensity, 5s duration, 1s delay. |
| **Variable Inertia** | `31M[Dir][Dist][Del]` | `31M01005`: Move 10mm upward, 0.5s delay. |
| **Variable Impact** | `41P[Del][Count]` | `41P1001`: Trigger 1 impact after 1s delay. |
| **Variable Stiffness** | `61S[Int][Dur][Del]` | `61S100500`: Max stiffness, 5s duration, 0.5s delay. |

* **Example PC Command**: `$11E1101005,41P1001`
    * *Master Module*: Broadcasts the entire string to the mesh.
    * *Module 11*: Extracts `E1101005` and starts vibration.
    * *Module 41*: Extracts `P1001` and triggers an impact.
