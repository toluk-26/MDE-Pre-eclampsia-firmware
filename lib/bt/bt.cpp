/**
 * @file bt.cpp
 * @author Tolu Kolade
 * @brief ble device functions and starts services
 */

#include "bt.hpp"
#include "bletis.hpp"

// BLE Services
BLEDfu bledfu; // OTA DFU service
BLEDis bledis; // device information
BLEBas blebas; // battery info
BLETis bletis; // time sync

void PESBt::init() {
    this->ble_config(); // configure BLE

    bledfu.begin(); // start DFU service

    this->dev_info_config(); // start bledis which is dev information service

    blebas.begin();    // Battery Service
    blebas.write(255); // TODO: implement and remove

    bletis.begin(); // Start Time Service

    // Set up and start advertising
    this->startAdv();
}

void PESBt::disconnect() {
    Bluefruit.Advertising.restartOnDisconnect(false);
    uint16_t connections = Bluefruit.connected();
    for (uint16_t conn = 0; conn < connections; conn++) {
        Bluefruit.disconnect(conn);
    }
    Bluefruit.Advertising.stop();
    Bluefruit.autoConnLed(false);
    Serial.println("STATUS: Disconnected bluetooth service");
}

void PESBt::startAdv(void) {
    // Advertising packet
    Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
    Bluefruit.Advertising.addTxPower();

    // Secondary Scan Response packet (optional)
    // Since there is no room for 'Name' in Advertising packet
    Bluefruit.ScanResponse.addName();

    /* Start Advertising
     * - Enable auto advertising if disconnected
     * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
     * - Timeout for fast mode is 30 seconds
     * - Start(timeout) with timeout = 0 will advertise forever (until
     * connected)
     *
     * For recommended advertising interval
     * https://developer.apple.com/library/content/qa/qa1931/_index.html
     */
    Bluefruit.Advertising.restartOnDisconnect(true);
    Bluefruit.Advertising.setInterval(32, 244); // in unit of 0.625 ms
    Bluefruit.Advertising.setFastTimeout(30); // number of seconds in fast mode
    Bluefruit.Advertising.start(
        0); // 0 = Don't stop advertising after n seconds

    Serial.println("STATUS: advertising...");
}

// callback invoked when central connects
void PESBt::connect_callback(uint16_t conn_handle) {
    // Get the reference to current connection
    BLEConnection *connection = Bluefruit.Connection(conn_handle);

    char central_name[32] = {0};
    connection->getPeerName(central_name, sizeof(central_name));

    Serial.printf("STATUS: Connected to %s\n", central_name);
}

/**
 * Callback invoked when a connection is dropped
 * @param conn_handle connection where this event happens
 * @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
 */
void PESBt::disconnect_callback(uint16_t conn_handle, uint8_t reason) {
    (void)conn_handle;
    (void)reason;

    Serial.print("STATUS: Disconnected, reason = 0x");
    Serial.println(reason, HEX);
}

void PESBt::ble_config() {
    // Setup the BLE LED to be enabled on CONNECT
    // Note: This is actually the default behavior, but provided
    // here in case you want to control this LED manually via PIN 19
    Bluefruit.autoConnLed(true);

    // Config the peripheral connection with maximum bandwidth
    // more SRAM required by SoftDevice
    // Note: All config***() function must be called before begin()
    Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);

    Bluefruit.begin();
    // create ble name from last 4 characters of dev id
    String uid = getMcuUniqueID();
    String s = "PES-" + uid.substring(12, 16);
    Bluefruit.setName(s.c_str());

    Bluefruit.setTxPower(4); // Check bluefruit.h for supported values
    Bluefruit.Periph.setConnectCallback(PESBt::connect_callback);
    Bluefruit.Periph.setDisconnectCallback(PESBt::disconnect_callback);
}

void PESBt::dev_info_config() {
    // Configure and Start Device Information Service
    bledis.setManufacturer("S26-09");
    bledis.setModel("Preeclampsia Screener"); // TODO: change to something more
                                              // meaningful? numeral?
    bledis.setSoftwareRev(SOFTWARE_REVISION);
    bledis.setHardwareRev(HARDWARE_REVISION);
    bledis.begin();
}