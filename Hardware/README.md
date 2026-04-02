# HaptiCraft Hardware

This folder contains the CAD files (STL) and electronic specifications for the **HaptiCraft** system. All haptic and utility modules are built around the [**ESP8266-based Wemos D1 Mini**](https://www.wemos.cc/en/latest/d1/d1_mini.html).

## Manufacturing & Assembly

* **3D Printing**: All housings are designed for modular assembly. For this research, parts were printed using a **Cubicon Single Plus 320C** with PLA filament. But, you can use any 3D printer for printing.
* **Module Faces**: Each STL file corresponds to a specific side of a cube, designed to be easily printed and assembled to house internal components.
* **Power & Connection**: Modules use [**magnetic pogo pins**](https://duertry.com/r/v2?u=bx&s1=&s2=&rti=false&d=https%3A%2F%2Fko.aliexpress.com%2Fitem%2F1005009452164878.html) at the center of each face to enable both structural bonding and power sharing (up to 12V, 10A) across the entire assembly.


## Module-Specific Hardware Descriptions
Follow the schematics provided in this directory for each specific module type:

### 1. Shape Module
The structural unit of the system. It consists of the 3D-printed housing, the MCU, and 6 Pogo pins for connectivity, with no active haptic actuators.

<p align="center">
   <img src="https://github.com/ChaeyongPark-KU/HaptiCraft/blob/main/img/schematic_shape_module.png" width="40%">
</p>

### 2. Vibration Module
Uses a motor driver to power two ERM motors for stable vibrotactile feedback.
* **ERM**: [D&J WITH Co., Ltd.; DVM1220](https://www.devicemart.co.kr/goods/view?no=1326747&srsltid=AfmBOooO0vIbSs27gQwI6GvXrJ4m8ziq-C5sojnKW-srApPcf6pz9BBs).
* **Motor Driver**: [Pololu; BD65496MUV](https://www.rohm.co.kr/products/motor-actuator-drivers/dc-brush-motor/bd65496muv-product#productDetail).

<p align="center">
   <img src="https://github.com/ChaeyongPark-KU/HaptiCraft/blob/main/img/schematic_vibration_module.png" width="65%">
</p>

### 3. Impact Module
Delivers short, strong impulses. Since it requires 5V, a step-up converter is integrated into the circuit.
* **Actuator**: [Acturonix; Digital Haptic Actuator](https://www.acturonix.com/).
* **Converter**: [WSTECH; DM06-V5](m.eleparts.co.kr/goods/view?no=4107103).

<p align="center">
   <img src="https://github.com/ChaeyongPark-KU/HaptiCraft/blob/main/img/schematic_impact_module.png" width="55%">
</p>

### 4. Thermal Module
Provides temperature feedback using a Peltier element. The assembly includes a step-up converter, motor driver, and a heatsink for effective thermal management.
* **Peltier**: [20×20 mm; SMG TES1-4903](https://www.devicemart.co.kr/goods/view?no=10894480).
* **Motor Driver**: [Pololu; BD65496MUV](https://www.rohm.co.kr/products/motor-actuator-drivers/dc-brush-motor/bd65496muv-product#productDetail).
* **Converter**: [WSTECH; DM13-V5](https://www.navimro.com/p/K92841578/?srsltid=AfmBOopcCdm5g3zLqHN5-aAA_jKeQ_9SND-a58UxNgDZ9oHb2oCQjKNd).

<p align="center">
   <img src="https://github.com/ChaeyongPark-KU/HaptiCraft/blob/main/img/schematic_thermal_module.png" width="60%">
</p>

### 5. Variable Inertia Module
Converts rotational motion into linear motion via a geared motor to shift the position of a **brass moving mass**. The moving mass can be weighted further using the uploaded weight-add files.
* **Gear Motor**: [N20; SZH-GNP095](https://www.icbanq.com/P008113428?srsltid=AfmBOoqd4h_dOW6KhbtmznncK5vQkRHJzLvjkogEKoTrAjK_nVLmlyeV).
* **Motor Driver**: [Pololu; BD65496MUV](https://www.rohm.co.kr/products/motor-actuator-drivers/dc-brush-motor/bd65496muv-product#productDetail).

<p align="center">
   <img src="https://github.com/ChaeyongPark-KU/HaptiCraft/blob/main/img/schematic_variable_inertia_module.png" width="60%">
</p>

### 6. Variable Stiffness Module
Adjusts surface softness using a custom variable-damping actuator. The internal circuit is based on the mechanism described in [[46]](https://journals.sagepub.com/doi/10.1177/1045389X13493359). All necessary CAD files for the spring-loaded structure are provided.

<p align="center">
   <img src="https://github.com/ChaeyongPark-KU/HaptiCraft/blob/main/img/schematic_variable_stiffness_module.png" width="60%">
</p>

### 7. Communication Module
A utility module containing only the MCU. It must be loaded with the `masterModule` firmware to act as the serial-to-mesh gateway.

### 8. Button Module
A 3D-printed housing embedded with a standard **mechanical keyboard switch** (linear switch) for user input.

<p align="center">
   <img src="https://github.com/ChaeyongPark-KU/HaptiCraft/blob/main/img/schematic_button_module.png" width="45%">
</p>

### 9. Touch Module
The top panel is printed using **conductive filament**. The circuit detects a touch when a finger bridges the filament paths, completing the electrical connection.

<p align="center">
   <img src="https://github.com/ChaeyongPark-KU/HaptiCraft/blob/main/img/schematic_touch_module.png" width="45%">
</p>

### 10. Power Module
Houses a **LiPo battery** (3.7V, 1200 mAh) and a charging circuit to provide untethered power to the entire HaptiCraft assembly.

<p align="center">
   <img src="https://github.com/ChaeyongPark-KU/HaptiCraft/blob/main/img/schematic_power_module.png" width="50%">
</p>

### 11. Tracking Module
Supports optical tracking (e.g., [Optitrack](https://optitrack.com/)). It is recommended to print various pillars and body types to create unique rigid-body marker configurations for VR tracking.

### 12. Connector Module
A 3D-printed part featuring four embossed poles that secure the mechanical connection between modules.