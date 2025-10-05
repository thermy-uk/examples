# Examples

This directory contains example code for discovering and reading data from a Thermy BLE thermometer. The device broadcasts temperature and humidity over Bluetooth Low Energy (BLE) in the advertisement Manufacturer Specific Data, so apps and services can read the measurements in real time without a GATT connection.

## What this is

- **Device**: A BLE thermometer that transmits both temperature and humidity.
- **Goal**: Provide small, focused examples showing how to scan for the device and read its measurements.
- **Audience**: Integrators, developers, and customers who want to build their own apps, services, or dashboards using Thermy devices.

## Repository layout

- `arduino/`: Minimal firmware example showing how the device advertises and encodes temperature and humidity in Manufacturer Specific Data (for reference and bring‑up).
- `nodejs/`: Node.js example that scans for the Thermy device and parses temperature and humidity from BLE advertisement Manufacturer Specific Data (no GATT connection required).

Additional languages and platforms will be added over time (see Roadmap).

## Data model (high‑level)

The Thermy device embeds readings directly in the advertisement's **Manufacturer Specific Data**. No GATT characteristics are used for measurement reads.

- **Payload size**: 8 bytes total
- **Fields**: temperature and humidity (encoded as compact numeric values)

Exact byte layout and scaling are documented within each example and may evolve with firmware updates.

## Quick start

### Node.js example

Requirements:

- Node.js 18+ (or a recent LTS)
- BLE support on your machine (Bluetooth enabled)

Steps:

1. Open a terminal in `examples/nodejs/`.
2. Install dependencies:

   ```bash
   npm install
   ```

3. Run the example:

   ```bash
   node index.js
   ```

4. Follow the on‑screen prompts/logs. The script will scan for the Thermy device and parse temperature and humidity from advertisement manufacturer data.

Notes:

- On some platforms, you may need additional permissions to access BLE (for example, enabling experimental BLE flags or running from a terminal session with Bluetooth access).

### Arduino firmware (reference)

Requirements:

- A supported Arduino/ESP board with BLE capability
- Toolchain for your board and `make` if you want to use the provided `Makefile`

Steps (generic):

1. Review `arduino/arduino.ino` to understand how the device advertises and publishes temperature and humidity.
2. Build and upload using your preferred IDE/toolchain, or from `arduino/` using the provided `Makefile` if it matches your board setup.
3. Once flashed, the device will begin advertising; use the Node.js example (or a BLE scanner) to verify readings.

## Roadmap

Planned examples:

- Python
- Go
- Rust
- Swift (iOS/macOS)
- Kotlin (Android)
- Web Bluetooth (browser)

If you need a specific language sooner, please open an issue to upvote it.

## Support and contributions

- Issues and feature requests: please open an issue in this repository.
- Contributions: PRs that improve clarity, add platforms/languages, or fix bugs are welcome.

## License

This examples directory follows the license of the main project. See the repository root for details.
