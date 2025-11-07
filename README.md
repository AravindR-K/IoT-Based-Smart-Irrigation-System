# IoT-Based Smart Irrigation System

**Simulation & demo files:** [Google Drive folder](https://drive.google.com/drive/folders/1xR5tYACOy1FFiejmJZFEArbnSXCe8Itk?usp=sharing)

## Project Overview  
This project implements a smart irrigation controller using the STM32F4 series MCU. It monitors temperature and soil moisture via analog sensors, applies multi-condition logic, and controls irrigation accordingly. Real-time status is communicated via USART and indicated by onboard LEDs.

## Technologies Used  
- Platform: STM32F4 (STM32F401xx)  
- Language: Embedded C (Keil uVision, CMSIS)  
- Key Peripherals:  
  - ADC (Analog-to-Digital Converter) for LM35 temperature sensor and soil moisture sensor  
  - USART1 (PA9 TX) for serial output to terminal  
  - NVIC + EXTI (external interrupt) for manual override / stop irrigation  
  - GPIO for LED indicators (PC13 & PC14)  
- Version Control: Git & GitHub  
- Simulation / Demo Files: Link above

## Features  
- Reads analog temperature (LM35) via ADC channel PA2 → Channel 2  
- Reads analog soil moisture via ADC channel PA3 → Channel 3  
- Evaluates four conditions based on temperature & moisture to decide if irrigation is needed:  
  1. Temp high & Moisture high → *No irrigation*  
  2. Temp high & Moisture low → *No irrigation*  
  3. Temp low & Moisture high → *No irrigation*  
  4. Temp low & Moisture low → *Irrigation needed*  
- LED indicators:  
  - **PC14**: Green LED → ON when irrigation is needed  
  - **PC13**: Red LED → ON when irrigation is **not** needed  
- USART1 serial output prints status messages for each condition → viewable in terminal (e.g., Docklight)  
- External interrupt (button on PA0) triggers “Irrigation Stopped” message and turns off irrigation LED

## Hardware Setup  
| Sensor | MCU Pin | Notes |
|--------|---------|--------|
| LM35 temperature sensor | PA2 (ADC1_CH2) | VCC: 5V or 3.3V, GND: GND |
| Soil moisture sensor (analog) | PA3 (ADC1_CH3) | VCC: 5V, GND: GND |
| LED indicator green | PC14 | Set as output |
| LED indicator red | PC13 | Set as output |
| Manual stop switch | PA0 | Input with pull-up, triggers EXTI0 |

## Build & Upload Instructions  
1. Open the project in Keil uVision.  
2. Build the project.  
3. Load to STM32 board via ST-Link (or your preferred programmer).  
4. Open your serial terminal (baud: 115200, 8-N-1) to view status messages.  
5. Observe LEDs changing based on sensor conditions and button press.

## Repository Structure  
IoT-Smart-Irrigation-System/

│
├── main.c                      # Core application code  
├── Embedded Project.uvprojx    # Keil uVision project file  
├── .gitignore                  # Excludes build/temp files  
├── README.md                   # Project documentation  
└── RTE/                        # Keil Runtime Environment files (ignored)

