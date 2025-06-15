# IP66-Rated Enclosure System (incomplete)

## 3D Model Specifications

### Main Enclosure
- **File:** `amis_enclosure_v1.stl`
- **Dimensions:** 150 x 100 x 60mm (LxWxH)
- **Material:** UV-Stabilized ASA Plastic
- **Print Settings:**
  - Layer Height: 0.2mm
  - Infill: 40% Gyroid
  - Walls: 3
  - Top/Bottom Layers: 6
- **IP66 Features:**
  - Gasket channel for 3mm silicone seal
  - IP67-rated cable glands (4x PG7)
  - Drainage channels
  - Overhang protection

### Sensor Capsule
- **File:** `sensor_capsule_v1.stl`
- **Dimensions:** Ø35mm x 120mm
- **Material:** PETG-CF (Carbon Fiber Reinforced)
- **Features:**
  - Snap-fit connector interface
  - Soil penetration tip (30° angle)
  - Internal moisture barrier
  - Cable strain relief

## Assembly Instructions

1. **Print Requirements:**
   - Main enclosure: 3D print with 100% infill base layer
   - Sensor capsules: Use watertight slicer settings

2. **Sealing Procedure:**
   ```bash
   # Apply conformal coating to PCB
   ./scripts/apply_conformal_coating.sh
   
   # Install silicone gasket
   cp gaskets/enclosure_gasket_v1.sil ./enclosure
   
   # Apply dielectric grease to cable glands
   ```

3. **Environmental Protection:**
   - Apply 3M™ Scotchkote™ Electrical Coating to PCB
   - Use Dow Corning® 732 sealant for seams
   - Install desiccant capsule in enclosure
