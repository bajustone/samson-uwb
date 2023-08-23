#include <SPI.h>
#include "DW1000Ranging.h"

#define ANCHOR_ADD "63:17:5B:D5:A9:9A:E2:9A"

#define SPI_SCK PA5
#define SPI_MISO PA6
#define SPI_MOSI PA7
#define DW_CS PA4
#define TX_CLK_PIN PB9
#define TX_DATA_PIN PB8
#define EN_SEND_DATA PB5
#define CHAR_BIT 8

// connection pins
const uint8_t PIN_RST = PB12; // reset pin
const uint8_t PIN_IRQ = PB0; // irq pin
const uint8_t PIN_SS = PA4;   // spi select pin




void setup()
{
    Serial.begin(57600);
    // delay(2000);
    //init the configuration
    // SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    SPI.begin();
    DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ); //Reset, CS, IRQ pin
    DW1000Ranging.attachNewRange(newRange);
    DW1000Ranging.attachBlinkDevice(newBlink);
    DW1000Ranging.attachInactiveDevice(inactiveDevice);
    DW1000Ranging.attachNewDevice(newDevice);
    // delay(1000);
    DW1000Ranging.startAsAnchor(ANCHOR_ADD, DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
    
    
    pinMode(TX_CLK_PIN, OUTPUT);
    pinMode(TX_DATA_PIN, OUTPUT);
    pinMode(EN_SEND_DATA, OUTPUT);
    
    digitalWrite(TX_CLK_PIN, LOW);
    digitalWrite(TX_DATA_PIN, LOW);
    delay(1000);
    // Serial.println("Ready...");  
}

void loop()
{
  DW1000Ranging.loop();

}
void newDevice(DW1000Device *device)
{

    Serial.print("A:");
    // Serial.println(device->getShortAddress(), HEX);

    // add_link(uwb_data, device->getShortAddress());
}

void newRange() {}

void newBlink(DW1000Device *device) {
  
}

void inactiveDevice(DW1000Device *device) {}