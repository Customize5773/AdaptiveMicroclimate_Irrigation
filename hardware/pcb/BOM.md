# Bill of Materials - Adaptive Microclimate Irrigation Controller PCB

| RefDes | Quantity | Component                     | Specification                              | Manufacturer | Part Number       | Unit Price | Notes |
|--------|----------|-------------------------------|--------------------------------------------|--------------|-------------------|------------|-------|
| U1     | 1        | MCU                           | ESP32-C3-MINI-1 (RISC-V, 160MHz, 4MB Flash) | Espressif    | ESP32-C3-MINI-1   | $3.20      |       |
| U2     | 1        | Environmental Sensor          | BME680 (Temp/Humidity/Pressure/VOC)        | Bosch        | BME680            | $6.80      |       |
| U3     | 1        | Light Sensor                  | BH1750FVI (I²C Digital Light Sensor)       | Rohm         | BH1750FVI         | $1.25      |       |
| U4     | 1        | Soil Sensor Interface         | ADS1115 (16-bit ADC)                       | Texas Inst.  | ADS1115IDGSR      | $2.30      |       |
| U5     | 1        | Power Management IC           | TPS63020 (Buck-Boost Converter)            | Texas Inst.  | TPS63020DSKR      | $2.80      |       |
| Q1-4   | 4        | MOSFET Transistor             | IRLB8721PbF (Logic Level)                  | Infineon     | IRLB8721PBF       | $0.85      | Pump drivers |
| J1     | 1        | Power Input Connector         | 5.5x2.1mm Barrel Jack                      | CUI          | PJ-102A           | $0.35      |       |
| J2-5   | 4        | Sensor Connectors             | 3-pin JST PH                               | JST          | B3B-PH-K-S        | $0.15      |       |
| J6     | 1        | USB-C Connector               | USB 2.0 Type-C                             | GCT          | USB4105-GF-A      | $0.60      |       |
| C1-6   | 6        | Ceramic Capacitor             | 10µF 25V X7R 0805                          | TDK          | C2012X7R1E106K125AB | $0.10      |       |
| R1-8   | 8        | Resistor                      | 10kΩ 1% 0805                               | Yageo        | RC0805FR-0710KL   | $0.02      |       |
| D1-4   | 4        | Protection Diode              | SMAJ5.0A (5V TVS)                          | Littelfuse   | SMAJ5.0A          | $0.25      |       |
| L1     | 1        | Power Inductor                | 4.7µH 3A Shielded                          | Coilcraft    | MSS7341-472MLB    | $0.90      |       |
| LED1-2 | 2        | Status LED                    | 0805 Green/Red                             | Lite-On      | LTST-C191KGKT     | $0.05      |       |
| SW1    | 1        | Reset Button                  | Tactile 6x6mm                              | C&K          | TS-1187A-B-A-B    | $0.30      |       |

**Total Estimated Cost (qty 100): $23.87/unit**

## PCB Specifications
- **Layers:** 4-layer stackup
- **Material:** FR-4 TG170
- **Finish:** ENIG (1µ gold)
- **Thickness:** 1.6mm
- **Copper Weight:** 1oz
- **Special Features:**
  - Conformal coating option
  - IP66-rated solder mask
  - Gold-plated edge connectors for sensor interfaces
  - Thermal relief for power components
