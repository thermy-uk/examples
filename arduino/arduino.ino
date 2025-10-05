#include <NimBLEDevice.h>
#include <AESLib.h>

const uint8_t KEY[16] = {0xA2, 0x36, 0x3C, 0xC2, 0x07, 0x23, 0x48, 0x97, 0x92, 0x2A, 0xEA, 0x86, 0x96, 0x85, 0x11, 0x5B};

// Unique Device ID (6-byte prefix)
const uint8_t EXPECTED_DEVICE_ID[6] = {0x74, 0x68, 0x65, 0x72, 0x6D, 0x79};

float g_temperature = 0;
float g_humidity = 0;
uint8_t g_batteryPercentage = 0;
static constexpr uint32_t scanTimeMs = 3 * 60 * 1000;  // 3-minute scan time.

AESLib aesLib;

bool aes128_dec_single_block(uint8_t *encryptedData, uint8_t *decryptedData, const uint8_t *key) {
  aesLib.set_paddingmode(paddingMode::ZeroLength);

  uint8_t iv[16] = {0};  // ECB mode does not use IV
  uint16_t decryptedLength = aesLib.decrypt(encryptedData, 16, decryptedData, key, 128, iv);

  if (decryptedLength < 5) {
    Serial.println("Decryption failed or invalid block size.");
    return false;
  }

  Serial.println("Decryption successful.");
  return true;
}

class scanCallbacks : public NimBLEScanCallbacks {
  void onScanEnd(const NimBLEScanResults &results, int reason) override {
    Serial.println("Scan ended. Restarting.");
    NimBLEDevice::getScan()->start(scanTimeMs, false, true);
  }

  void onResult(const NimBLEAdvertisedDevice *advertisedDevice) override {
    if (advertisedDevice->haveManufacturerData()) {
      std::string manufacturerData = advertisedDevice->getManufacturerData();
      uint8_t dataLength = manufacturerData.length();

      if (dataLength < 22) {
        // Serial.println("Invalid manufacturer data length, ignoring.");
        return;
      }

      // Extract expected 6-byte Device ID
      uint8_t deviceId[6];
      memcpy(deviceId, &manufacturerData[2], 6);

      // Compare with expected ID
      if (memcmp(deviceId, EXPECTED_DEVICE_ID, 6) != 0) {
        // Serial.println("Device ID mismatch, ignoring.");
        return;
      }

      Serial.println("Device matched. Extracting encrypted data...");

      // Extract 16 bytes of encrypted data
      uint8_t encryptedData[16];
      memcpy(encryptedData, &manufacturerData[8], 16);

      // Buffer for decrypted data
      uint8_t decryptedData[16];

      if (!aes128_dec_single_block(encryptedData, decryptedData, KEY)) {
        return;
      }

      Serial.print("Decrypted Data: ");
      for (int i = 0; i < 16; i++) {
        Serial.print(decryptedData[i], HEX);
        Serial.print(" ");
      }
      Serial.println();

      // Parse decrypted values
      uint16_t temp_raw = (decryptedData[0] << 8) | decryptedData[1];
      uint16_t humi_raw = (decryptedData[2] << 8) | decryptedData[3];
      uint8_t bat_raw = decryptedData[4];

      g_temperature = temp_raw / 100.0;
      g_humidity = humi_raw / 100.0;
      g_batteryPercentage = bat_raw;

      Serial.print("Parsed Data - Temperature: ");
      Serial.print(g_temperature);
      Serial.print("°C, Humidity: ");
      Serial.print(g_humidity);
      Serial.print("%, Battery: ");
      Serial.print(g_batteryPercentage);
      Serial.println("%");
    }
  }
} scanCallbacks;

NimBLEScan *pBLEScan;

void connectToThermometer() {
  NimBLEDevice::init("");
  NimBLEDevice::setPower(ESP_PWR_LVL_P9);
  pBLEScan = NimBLEDevice::getScan();
  pBLEScan->setScanCallbacks(&scanCallbacks, true);
  pBLEScan->setActiveScan(true);
  pBLEScan->setMaxResults(0);

  pBLEScan->start(scanTimeMs, false, true);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE Scanner...");
  connectToThermometer();
}

void loop() { delay(1000); }
