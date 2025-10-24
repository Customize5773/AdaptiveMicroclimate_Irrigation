# AdaptiveMicroclimate_Irrigation
LoRa-connected farm tech that auto-waters based on soil and weather conditions.

## TL;DR
- What: Low-power sensor nodes (C) that send soil/weather telemetry over LoRa to a Python gateway/server which decides when to water and issues valve commands.
- Who: Hobbyists, small farms, researchers building low-cost adaptive irrigation.
- Quick: Flash firmware → run gateway → configure thresholds/API token.

## Features
- Soil moisture, battery, optional temp/humidity telemetry.
- LoRa (SX127x family) radio comms: node → gateway → server.
- Server rule engine: soil threshold + weather/forecast + safety constraints.
- Power-efficient sleep mode for nodes.
- Simple JSON API and simulation helpers.

## Supported languages
- C (firmware) — ~77%
- Python (gateway/server) — ~23%

## Hardware (replace placeholders)
- MCU: (e.g., ESP32 / Arduino Pro Mini / STM32) — set in firmware config
- LoRa radio: SX1276/SX1278 (adjust freq, SF in firmware)
- Soil moisture sensor (analog or digital)
- Valve: solenoid + relay or valve driver
- Power: battery ± solar charge controller

## Quickstart — Firmware (device)
1. Edit board & radio settings:
   - Open firmware/config.h or firmware/boards/<board>.h and set MCU, pins, frequency, spreading factor, and node ID.
2. Build & flash (examples):
   - PlatformIO:
     ```
     cd firmware
     pio run -e <env> -t upload
     ```
   - Make / avr:
     ```
     cd firmware
     make BOARD=<board>
     make flash
     ```
3. Behavior:
   - Node wakes, reads sensors, sends compressed telemetry packet over LoRa, sleeps. It accepts simple control packets to open valve for N seconds.

## Quickstart — Gateway / Server (Python)
1. Create venv and install:
   ```bash
   python -m venv .venv
   # macOS / Linux
   source .venv/bin/activate
   # Windows (PowerShell)
   .\.venv\Scripts\Activate.ps1

   pip install -r gateway/requirements.txt
   ```

2. Example configuration (gateway/config.example.yaml)
   ```yaml
   server:
     host: 0.0.0.0
     port: 8000
     api_token: "replace-with-secure-token"

   lora:
     port: "/dev/ttyUSB0"   # serial port for sx127x gateway board (if using an attached concentrator)
     baudrate: 115200
     frequency: 915000000

   rules:
     soil_threshold: 30      # percent
     min_interval_sec: 3600  # minimum seconds between waterings per node
     max_valve_seconds: 300  # safety cap for commands
   ```

3. Run the server:
   ```bash
   cd gateway
   python server.py --config config.yaml
   ```

4. Docker (optional):
   - Build:
     ```bash
     docker build -t amc-irrigation:latest gateway/
     ```
   - Run:
     ```bash
     docker run -d \
       --device=/dev/ttyUSB0:/dev/ttyUSB0 \
       -p 8000:8000 \
       -v $(pwd)/gateway/config.yaml:/app/config.yaml \
       amc-irrigation:latest
     ```

## API
The gateway exposes a small JSON HTTP API for monitoring and manual commands.

- GET /api/v1/nodes
  - Returns list of known nodes and last-seen timestamps.

- GET /api/v1/nodes/{node_id}/telemetry
  - Returns latest telemetry for the node.

- POST /api/v1/nodes/{node_id}/valve
  - Body: { "duration": number } — seconds to open valve.
  - Header: Authorization: Bearer <api_token>
  - Response: { "status": "accepted", "scheduled_until": "<iso>" }

Security: always protect the API endpoint with a strong token and network-level restrictions.

## TypeScript examples
Below are short TypeScript examples showing how to query telemetry and issue a valve command.

- Install (Node):
  ```bash
  npm init -y
  npm install node-fetch@2
  npm install --save-dev @types/node-fetch
  ```

- Get telemetry (TypeScript):
  ```typescript
  // filename: getTelemetry.ts
  import fetch from "node-fetch";

  type Telemetry = {
    node_id: string;
    moisture: number; // percent
    battery_mv: number;
    temp_c?: number;
    humidity?: number;
    ts: string; // ISO
  };

  async function getLatest(nodeId: string, token: string): Promise<Telemetry> {
    const res = await fetch(`http://localhost:8000/api/v1/nodes/${nodeId}/telemetry`, {
      headers: { Authorization: `Bearer ${token}` },
    });
    if (!res.ok) throw new Error(`HTTP ${res.status}`);
    return (await res.json()) as Telemetry;
  }

  // usage
  (async () => {
    const telemetry = await getLatest("node-01", "replace-with-token");
    console.log("Latest telemetry:", telemetry);
  })();
  ```

- Open valve (TypeScript):
  ```typescript
  // filename: openValve.ts
  import fetch from "node-fetch";

  async function openValve(nodeId: string, durationSec: number, token: string) {
    const res = await fetch(`http://localhost:8000/api/v1/nodes/${nodeId}/valve`, {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
        Authorization: `Bearer ${token}`,
      },
      body: JSON.stringify({ duration: durationSec }),
    });
    if (!res.ok) {
      const text = await res.text();
      throw new Error(`Failed to open valve: ${res.status} ${text}`);
    }
    return await res.json();
  }

  // usage
  (async () => {
    const resp = await openValve("node-01", 60, "replace-with-token");
    console.log("Command response:", resp);
  })();
  ```

## Simulation helpers
There are simple scripts to emulate node telemetry for testing (see gateway/simulate/).
- Example:
  ```bash
  # send a fake telemetry packet for node-01
  python gateway/simulate/send_telemetry.py --node node-01 --moisture 25 --battery 3700
  ```
- Use simulation to validate server rules, web UI, and API clients without hardware.

## Packet format (high-level)
Nodes send compact binary packets over LoRa. The gateway decodes and stores telemetry as JSON:
- node_id (1 byte or ASCII ID)
- flags (sensors present)
- moisture (1 byte, 0-100)
- battery_mv (2 bytes)
- optional temp / humidity fields
- sequence / timestamp (optional)

See firmware/README or firmware/protocol.md for the exact encoding used by your build.

## Rule engine (server)
The server evaluates simple rules:
- If soil moisture < soil_threshold AND last_watering > min_interval_sec → schedule valve open.
- Safety checks:
  - Never command more than max_valve_seconds in a single command.
  - Respect blackout windows (e.g., don't water during freeze).
  - Rate-limit automatic commands per node.

You can extend rules by modifying gateway/rules.py (or equivalent) to integrate weather APIs, ET0, or soil-water balance models.

## Power & reliability tips
- Use deep-sleep and long intervals to maximize battery life.
- Report battery voltage and implement low-battery behavior (e.g., less frequent telemetry).
- Use hardware-level watchdog on MCU for reliability.
- Add validation on server for malformed packets and replay protection (sequence numbers).

## Troubleshooting
- Gateway can't see nodes:
  - Verify LoRa frequency/spreading factor match between node and gateway.
  - Check serial device permissions (udev rules on Linux).
- Valve commands appear accepted but valve doesn't open:
  - Confirm relay wiring and power supply to solenoid.
  - Check node logs for received control packet and any safety override.
- Telemetry values out of expected range:
  - Calibrate sensors and add scaling in firmware/config.

## Contributing
Contributions welcome. Suggested workflow:
1. Fork the repo.
2. Create a feature branch.
3. Add tests or simulation steps for new behavior.
4. Open a PR describing the change.

Please include hardware tested against (board, radio, sensors) for firmware changes.

## Project structure (high-level)
- firmware/ — device code (C)
- gateway/ — Python gateway + server, simulation tooling
- docs/ — protocol docs, hardware wiring diagrams (if present)
- examples/ — wiring and deployment examples

## Next steps / TODO
- Add UI for scheduling and visualizing moisture trends.
- Add OAuth / RBAC to API for teams.
- Add more precise irrigation models (weather + crop coefficients).

## License
Add a LICENSE file to this repo with your preferred license (MIT or similar). If you haven't chosen one yet, consider MIT for permissive reuse.

## Acknowledgements
Inspired by hobbyist LoRa projects and community irrigation controllers. Please attribute and share improvements.

If you want, I can:
- Add an example config that wires to a popular LoRa concentrator (e.g., RPi + LoRa HAT).
- Draft a minimal systemd service file and Docker Compose example for automated deployment.
- Generate TypeScript client library types & helper functions to interact with the API.
