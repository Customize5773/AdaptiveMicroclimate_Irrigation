# Adaptive Microclimate Irrigation System (AMIS) Architecture

## System Overview
```mermaid
graph TD
    A[Sensors] --> B[Data Acquisition]
    B --> C[Environmental Modeling]
    C --> D[Decision Engine]
    D --> E[Irrigation Control]
    F[Weather API] --> C
    G[User Interface] --> D
    E --> H[Actuators]
    B --> I[Data Logging]
    I --> J[Cloud Storage]
```

## Core Design Principles
1. **Environmental Responsiveness**
   - Real-time microclimate monitoring
   - Predictive weather integration
   - Soil-plant-atmosphere continuum modeling

2. **Energy Efficiency**
   - Solar-optimized power architecture
   - Sleep modes (95% duty cycle reduction)
   - Adaptive sampling rates

3. **Scalability**
   - Modular sensor/actuator interfaces
   - Distributed edge computing
   - Hybrid connectivity (LoRa/Wi-Fi/BLE)

4. **Resilience**
   - IP66-rated enclosure
   - Fault-tolerant sensor networks
   - Watchdog-protected firmware

## Hardware Architecture
```mermaid
flowchart LR
    subgraph Sensing Layer
        A[BME680] -->|I²C| B[ESP32-C3]
        C[BH1750] -->|I²C| B
        D[Soil Probes] -->|Analog| E[ADS1115 ADC]
        E -->|I²C| B
    end
    
    subgraph Control Layer
        B -->|PWM| F[MOSFET Drivers]
        F --> G[Water Pumps]
        B -->|UART| H[LoRa Module]
    end
    
    subgraph Power System
        I[Solar Panel] --> J[MPPT Charger]
        J --> K[LiFePO₄ Battery]
        K --> L[Buck-Boost Converter]
        L -->|3.3V| B
    end
```

## Software Architecture
```mermaid
flowchart TB
    subgraph Sensor Fusion
        A[Raw Data] --> B[Temperature Compensation]
        B --> C[Environmental Calibration]
        C --> D[Microclimate Model]
    end
    
    subgraph Decision Core
        E[Soil VWC] --> F[Water Deficit Calc]
        G[Weather Forecast] --> F
        H[Plant Profile] --> F
        F --> I[Irrigation Schedule]
    end
    
    subgraph Connectivity
        J[MQTT Broker] <--> K[Cloud Integration]
        L[HTTP REST] --> M[Weather APIs]
        N[BLE GATT] --> O[Mobile App]
    end
```

## Key Design Choices

### 1. RISC-V MCU Selection
- **ESP32-C3** over ARM alternatives:
  - 40% lower power consumption
  - Built-in IEEE 802.11b/g/n Wi-Fi
  - Secure boot + flash encryption
  - 30% cost reduction

### 2. Hybrid Connectivity
| Protocol | Use Case | Range | Power |
|----------|----------|-------|-------|
| Wi-Fi    | Cloud sync | Medium | High |
| LoRaWAN  | Field mesh | Long | Low |
| BLE      | Local config | Short | Very Low |

### 3. Predictive Watering Model
```math
ET_c = K_c \times \left[ \frac{0.408\Delta(R_n - G) + \gamma\frac{900}{T+273}u_2(e_s - e_a)}{\Delta + \gamma(1 + 0.34u_2)} \right]
```
*FAO Penman-Monteith equation implementation*

### 4. Fault Tolerance Mechanisms
- Sensor health monitoring
- Pump current sensing
- Watchdog timer (HW + SW)
- Brown-out recovery
- Fail-safe dry contacts

## Data Flow
```mermaid
sequenceDiagram
    participant Sensor as Environmental Sensors
    participant MCU as ESP32-C3
    participant Cloud as Cloud Service
    participant Pump as Irrigation Actuator
    
    loop Every 15 minutes
        Sensor->>MCU: Raw measurements
        MCU->>MCU: Apply calibration
        MCU->>MCU: Run climate model
        MCU->>Cloud: Sync sensor data
        Cloud->>MCU: Weather forecast
        MCU->>MCU: Compute water deficit
        alt Watering needed
            MCU->>Pump: Activate valve
            Pump->>MCU: Confirm operation
            MCU->>Cloud: Log irrigation event
        end
    end
```
