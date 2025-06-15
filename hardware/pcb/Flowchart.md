# PCB Signal Flow Architecture

```mermaid
flowchart TD
    A[Power Input] -->|5.5x2.1mm Barrel Jack| B[Power Management]
    C[USB-C Port] -->|Programming & Debugging| D[ESP32-C3 MCU]
    B -->|3.3V Regulated Power| D
    B -->|5V Power| E[Peripheral Circuits]
    
    subgraph Power Management
        B[TPS63020 Buck-Boost<br>Input: 5-24V DC<br>Output: 3.3V@2A]
    end
    
    subgraph MCU Section
        D[ESP32-C3-MINI-1<br>160MHz RISC-V<br>4MB Flash<br>400KB SRAM]
        D --> F[Wi-Fi/BLE Radio]
    end
    
    subgraph Sensor Interfaces
        D -->|I²C0| G[BME680 Environmental<br>Temp/Humidity/Pressure]
        D -->|I²C0| H[BH1750 Light Sensor<br>1-65,535 Lux]
        D -->|I²C1| I[ADS1115 ADC<br>16-bit Resolution]
        I --> J[Soil Moisture Sensors<br>4x Analog Inputs]
    end
    
    subgraph Control Outputs
        D -->|GPIO PWM| K[MOSFET Drivers]
        K --> L[Water Pumps<br>12V @ 5A max]
        D -->|UART| M[LoRa Module<br>Optional Long-Range]
    end
    
    subgraph System Monitoring
        D -->|ADC| N[Voltage Sensing]
        D -->|ADC| O[Current Sensing]
        D --> P[Status LEDs<br>System Health]
    end
    
    Q[External Sensors] -->|JST PH Connectors| J
    R[Power Source] -->|Solar/Battery| A
    S[User Interface] -->|Boot/Reset Buttons| D
```

## Signal Flow Explanation

### 1. **Power Path**
```mermaid
flowchart LR
    A[External Power<br>5-24V DC] --> B
    B[Power Management IC] --> C[3.3V MCU Power]
    B --> D[5V Sensor Power]
    C --> E[Core Logic]
    D --> F[Analog Circuits]
```

### 2. **Sensor Data Acquisition**
```mermaid
flowchart TB
    A[Environmental Sensors] -->|I²C0| B[BME680]
    C[Light Sensor] -->|I²C0| B
    D[Soil Probes] -->|Analog| E[ADS1115 ADC]
    E -->|I²C1| F[ESP32-C3]
    F --> G[Data Processing]
```

### 3. **Irrigation Control**
```mermaid
flowchart LR
    A[Control Algorithm] --> B[PWM Generation]
    B --> C[MOSFET Drivers]
    C --> D[Water Pumps]
    D --> E[Irrigation System]
    F[Current Sensing] -->|Feedback| A
```

### 4. **Communication Flow**
```mermaid
flowchart RL
    A[Cloud Service] <-->|Wi-Fi| B[ESP32-C3]
    C[Local Network] <-->|MQTT| B
    D[Field Gateway] <-->|LoRa| B
    E[Mobile App] <-->|BLE| B
```

## Key Design Features

1. **Power Isolation Architecture**
   - Dedicated LDOs for analog/digital sections
   - Star-point grounding for noise reduction
   - TVS diodes on all external interfaces

2. **Signal Integrity Measures**
   - Controlled impedance for I²C lines (100Ω differential)
   - Separate analog/digital ground planes
   - Guard rings around high-impedance soil inputs

3. **Fault Protection Systems**
   ```mermaid
   flowchart TD
       A[Overvoltage] -->|TVS Diodes| B[Clamping]
       C[Overcurrent] -->|Polyfuses| D[Current Limiting]
       E[ESD] -->|Spark Gaps| F[Discharge Paths]
       G[Reverse Polarity] -->|Schottky Diode| H[Blocking]
   ```

4. **Manufacturing Test Points**
   | Test Point | Purpose | Expected Value |
   |------------|---------|----------------|
   | TP1 | 3.3V Power | 3.3V ±2% |
   | TP2 | I²C0 Clock | 100kHz Square Wave |
   | TP3 | Pump PWM | Variable Duty Cycle |
   | TP4 | Soil Sensor Input | 0-3.3V Analog |
   | TP5 | Wi-Fi Activity | RF Signal |

## Design Verification Flow
```mermaid
flowchart TB
    A[Schematic Capture] --> B[Layout Implementation]
    B --> C[DRC Verification]
    C --> D[Signal Integrity Analysis]
    D --> E[Thermal Simulation]
    E --> F[Prototype Fabrication]
    F --> G[Power-On Testing]
    G --> H[Functional Validation]
    H --> I[Environmental Testing]
    I --> J[Certification]
    J --> K[Mass Production]
```

## Revision History
| Version | Date       | Changes                     |
|---------|------------|----------------------------|
| v1.0    | 2023-10-20 | Initial Release            |
| v1.1    | 2023-11-05 | Added LoRa module footprint|
| v1.2    | 2023-11-18 | Improved thermal management|
