import noble from '@abandonware/noble';
import crypto from 'node:crypto';

const DEBUG = Boolean(process.env.DEBUG);
const debug = (message) => DEBUG && console.log(message);

/**
 * Key that is used to decrypt the data. It will change from device to device.
 */
const KEY = Buffer.from('A2363CC207234897922AEA869685115B', 'hex');

/**
 * Unique device ID we expect in the first 6 bytes of manufacturer data.
 * It will change from device to device.
 */
const EXPECTED_DEVICE_ID = Buffer.from([0x74, 0x68, 0x65, 0x72, 0x6D, 0x79]);

/**
 * AES-128-ECB decryption function.
 */
function aes128Decrypt(encryptedData, key) {
    try {
        const decipher = crypto.createDecipheriv('aes-128-ecb', key, null);
        decipher.setAutoPadding(false); // No padding used in the firmware
        return Buffer.concat([decipher.update(encryptedData), decipher.final()]);
    } catch (error) {
        console.error('Decryption error:', error.message);
        return null;
    }
}

/**
 * Parse decrypted BLE advertisement data.
 */
function parseDecryptedData(data) {
    if (!data || data.length < 5) {
        console.error('Invalid decrypted data.');
        return null;
    }

    const temperatureRaw = data.readUInt16BE(0); // First 2 bytes
    const humidityRaw = data.readUInt16BE(2); // Next 2 bytes
    const batteryRaw = data.readUInt8(4); // Fifth byte

    const temperature = temperatureRaw / 100.0;
    const humidity = humidityRaw / 100.0;
    const batteryPercentage = batteryRaw;

    console.log(`Parsed Data - Temperature: ${temperature}°C, Humidity: ${humidity}%, Battery: ${batteryPercentage}%`);
}

noble.on('stateChange', async (state) => {
    if (state === 'poweredOn') {
        console.log('Starting scan...');
        await noble.startScanningAsync([], true);
    } else {
        console.log(`State: ${state}`);
        await noble.stopScanningAsync();
    }
});

noble.on('discover', (peripheral) => {
    const manufacturerData = peripheral.advertisement.manufacturerData;

    if (!manufacturerData || manufacturerData.length < 22) {
        return; // Ignore devices without proper manufacturer data
    }

    /**
     * Skip first 2 bytes as this is company identifier
     * Read next 6 bytes as this is our
     */
    const deviceId = manufacturerData.slice(2, 8);

    debug({ deviceId })

    if (!deviceId.equals(EXPECTED_DEVICE_ID)) {
        return; // Ignore devices that don't match our unique ID
    }

    debug(`Device detected with expected ID: ${deviceId.toString('hex')}`);

    // Extract the remaining bytes (AES encrypted data)
    const encryptedData = manufacturerData.slice(8);

    if (encryptedData.length !== 16) {
        debug(`Skipping device ${peripheral.id} - Encrypted data size incorrect.`);
        return;
    }

    const decryptedData = aes128Decrypt(encryptedData, KEY);

    if (decryptedData) {
        console.log(`Decrypted Data: ${decryptedData.toString('hex')}`);
        parseDecryptedData(decryptedData);
    }
});

noble.on('error', (error) => {
    console.error('Noble error:', error.message);
});
