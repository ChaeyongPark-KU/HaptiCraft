# **HaptiCraft Authoring Tool**

HaptiCraft authoring tool is a Unity-based tool designed for creating modular haptic controllers and designing custom haptic feedback patterns. This tool supports the entire workflow from virtual assembly to physical implementation and haptic signal synchronization.

<p align="center">
<img src="https://github.com/ChaeyongPark-KU/HaptiCraft/blob/main/img/authoring0.png" width="60%">
</p>

## **Preparation**

### **Hardware Requirements**

- **Communication Module:** Connect a communication module to your PC via USB.
- **Serial Port:** Identify the COM port number (Windows) assigned to the communication module.

### **Software Requirements**

- **Unity Version:** Tested and verified on **Unity 2022.3 (LTS)**.
- **Package:** Import the provided `.unitypackage` into your Unity project.

### **Project Configuration**

1. Go to **Project Settings \> Player \> Configuration**.
2. Set **Api Compatibility Level** to **.NET Framework**.
3. Open the main scene: Assets/Scenes/HaptiCraft.unity.
4. In the **Hierarchy**, find Authoring Utils \-\> Serial Manager.
5. Set the **Port Number** in the Inspector to match your communication module's serial port.

## **Basic Controls**

<p align="center">

| **Action**            | **Input**           |
| --------------------- | ------------------- |
| **Rotate Controller** | Left Click \+ Drag  |
| **Move Controller**   | Wheel Click \+ Drag |
| **Zoom In / Out**     | Wheel Scroll        |
</p>

## **Authoring Workflow**

The tool consists of three main phases to guide you through the creation process.

### **1\. Controller Design Phase**

This is where you design the physical structure of your virtual modular controller.

<p align="center">
<img src="https://github.com/ChaeyongPark-KU/HaptiCraft/blob/main/img/authoring1.png"  width="60%">
</p>

- **Add Modules:** Click the module buttons (top-left) to spawn a module at the bottom-center of the screen.
- **Connect Modules:** Select a specific face of the new module and then the face on the existing controller you wish to connect to.
- **Rotate:** Use the **Arrow Keys** to rotate the module before finalizing.
- **Confirm:** Press the **Return (Enter)** key to finalize the connection.
- **Next:** Once the controller is finished, click the **Next** button (top-right).

### **2\. Haptic Assembly Phase**

Design and edit haptic signals for the assembled controller.

<p align="center">
<img src="https://github.com/ChaeyongPark-KU/HaptiCraft/blob/main/img/authoring2_1.png" width="60%">
</p>

- **Module Selection:** Left-click on any haptic-capable module to create a stimulus.
- **Edit Haptic Effects:** Use the **Haptic Layer** (right) and the **Timeline/Haptic Track** (bottom) to adjust the timing, duration, and intensity of the feedback.
  

### **3\. Implementation Phase**

This phase helps you assemble the physical hardware corresponding to your virtual design.

- **Assembly Guide:** Starting from `Power module`, the virtual module to be connected next will **blink**.
- **Detection:** Once `Communication Module` recognizes the physical connection, the next module in the sequence will start blinking.
- **Skip:** You can skip this phase and proceed to the next by clicking the **Next** button without connecting physical modules.
  

#### **Supported Modules:**

- **Vibration Module:** Envelope, Intensity, Duration, Delay.
- **Thermal Module:** Mode (Cold / Hot), Intensity, Duration, Delay.
- **Inertia Module:** Direction, Duration.
- **Impact Module:** Repetition, Delay.
- **Stiffness Module:** _(Currently disabled)_
- **Button Module:** Click detection.
- **Touch Module:** Touch detection.

## **Save / Load**

You can save or load your controller designs and haptic patterns at any time.

- Enter a filename in the **Input Field** at the top of the screen.
- Click **Save** to store the current configuration or **Load** to restore a previous one.
