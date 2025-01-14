#include <bluefruit.h>

// 0x004C is Apple
#define MANUFACTURER_ID   0x004C

uint8_t Uuid[16] = {
  0x01, 0x00, 0x00, 0x02, // Identificador del tag
  0x05, 0x06, 0x07, 0x08, // Bytes de relleno
  0x01, 0x01, 0x01, 0x01, // Activo fijo (1001 en hexadecimal) y bytes de relleno
  0x01, 0x01, 0x01, 0x01  // Relleno adicional para completar 16 bytes
};

BLEBeacon beacon(Uuid, 0x0102, 0x0304, -80);

void setup()
{
  Serial.begin(115200);
  Bluefruit.Advertising.stop();
  Bluefruit.Advertising.clearData();
  Bluefruit.begin();

  // off Blue LED for lowest power consumption
  Bluefruit.autoConnLed(false);
  Bluefruit.setTxPower(-16);    // Check bluefruit.h for supported values

  // Manufacturer ID is required for Manufacturer Specific Data
  beacon.setManufacturer(MANUFACTURER_ID);

  // Setup the advertising packet
  startAdv();
  //Serial.println("Broadcasting beacon, open your beacon app to test");

  suspendLoop();
}

void startAdv(void)
{
  Bluefruit.Advertising.setBeacon(beacon);

  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.setName("EB.EKG"); // Nombre de la etiqueta
  Bluefruit.ScanResponse.addName();

  //Bluefruit.Advertising.setType(BLE_GAP_ADV_TYPE_ADV_NONCONN_IND);
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(320, 1600);   //intervalo de tiempo para la publicidad 
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds
}

void loop()
{
}
