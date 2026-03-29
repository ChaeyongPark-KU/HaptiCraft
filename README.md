# HaptiCraft: A Modular Multimodal Haptic Controller for Immersive VR

**HaptiCraft** is a modular, reconfigurable handheld controller designed to replicate the physical appearance, mass distribution, and rich multimodal haptic properties of virtual objects. By assembling cubic modules with distinct functions, users can create a customized physical proxy tailored to specific VR interactions.

<img src="https://github.com/ChaeyongPark-KU/HaptiCraft/blob/main/img/teaser.png">

## Key Features

* **Modular Shape Approximation**: Uses unit cube modules ($48 \times 48 \times 48$ mm) to build controllers that mimic the shape of virtual counterparts.
* **Multimodal Feedback**: Supports five distinct active haptic stimuli: **Vibration**, **Impact**, **Thermal**, **Variable Inertia**, and **Variable Stiffness**.
* **Dynamic Property Matching**: Features a variable inertia module that shifts internal mass to match the Moment of Inertia (MOI) of virtual objects.
* **Integrated Authoring Tool**: Includes a graphical "What-You-See-Is-What-You-Get" (WYSIWYG) tool for designing both physical forms and complex haptic timelines.

## System Architecture

HaptiCraft is built on three foundational pillars:

1.  **Hardware**: 3D-printed (PLA) modules containing MCUs (Wemos D1 mini; ESP8266), actuators, and a secure mechanical connection mechanism.
2.  **Firmware**: ESP8266-based code managing assembly detection, wireless mesh synchronization, and real-time haptic command execution.
3.  **Software**: A Unity-based authoring tool that exports designs in the **HaptiCraft Description Format (.hdf)**, a JSON-based schema for easy integration into VR projects.

## Repository Structure

This repository is organized into three main components:

* **/Hardware**: Includes CAD files (STL) for module housings and circuit schematics for the haptic drivers.
* **/Firmware**: ESP8266 source code for the Wi-Fi mesh communication and haptic rendering protocols.
* **/Software**: The Unity source code for the HaptiCraft authoring tool.


## Citation
If you use HaptiCraft in your research, please cite our paper:
```
@article{Park2026HaptiCraft,
author = {Park, Chaeyong and Kim, Jeongwoo and Song, Yuk-Gwon and Kim, Sang-Youn and Choi, Seungmoon},
title = {HaptiCraft: A Modular Multimodal Haptic Controller for Immersive Virtual Reality Interactions},
journal = {IEEE transactions on visualization and computer graphics},
volume={XX},
number={X},
pages={XXXX--XXXX},
year={2026},
publisher={IEEE}
}
```

## Issues
If you encounter any issues, please feel free to contact us at chaeyong@korea.ac.kr.
