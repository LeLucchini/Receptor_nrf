#include "mbed.h"
#include "nRF24L01P.h"
#include "MotorH.h"
#include <string.h>

// Initialize BufferedSerial object for PC communication
BufferedSerial pc(USBTX, USBRX); // USBTX and USBRX are default pins for serial communication

// nRF24L01+ module setup
nRF24L01P my_nrf24l01p(PTD2, PTD3, PTC5, PTD0, PTD5, PTA13);    // mosi, miso, sck, csn, ce, irq


motorH *motor = new motorH(); // classe de controle do motor

int main() {
 

    #define TRANSFER_SIZE 4

    char txData[TRANSFER_SIZE] = {0}, rxData[TRANSFER_SIZE] = {0};
    int txDataCnt = 0;
    int rxDataCnt = 0;

    // Initialize the nRF24L01+ module
    pc.write("Powering up the nRF24L01+...\r\n", 31);
    my_nrf24l01p.powerUp();

    // Display the setup of the nRF24L01+ chip
    char buffer[100];
    int len = sprintf(buffer, "nRF24L01+ Frequency    : %d MHz\r\n", my_nrf24l01p.getRfFrequency());
    pc.write(buffer, len);
    len = sprintf(buffer, "nRF24L01+ Output power : %d dBm\r\n", my_nrf24l01p.getRfOutputPower());
    pc.write(buffer, len);
    len = sprintf(buffer, "nRF24L01+ Data Rate    : %d kbps\r\n", my_nrf24l01p.getAirDataRate());
    pc.write(buffer, len);
    len = sprintf(buffer, "nRF24L01+ TX Address   : 0x%010llX\r\n", my_nrf24l01p.getTxAddress());
    pc.write(buffer, len);
    len = sprintf(buffer, "nRF24L01+ RX Address   : 0x%010llX\r\n", my_nrf24l01p.getRxAddress());
    pc.write(buffer, len);

    pc.write("Type keys to test transfers:\r\n  (transfers are grouped into 4 characters)\r\n", 75);

    my_nrf24l01p.setTransferSize(TRANSFER_SIZE);
    my_nrf24l01p.setReceiveMode();
    my_nrf24l01p.enable();

    
    motor->serial = &pc;
    motor->stop(); // inicia com os motores parados

    while (1) {
        // Check if data is available on the serial interface

        // if (pc.readable()) {
        //     char c;
        //     pc.read(&c, 1);
        //     txData[txDataCnt++] = c;

        //     // Transmit buffer full
        //     if (txDataCnt >= sizeof(txData)) {
        //         my_nrf24l01p.write(NRF24L01P_PIPE_P0, txData, txDataCnt);
        //         txDataCnt = 0;
        //     }
        // }

        // Check if data is available in the nRF24L01+
        if (my_nrf24l01p.readable()) {
            rxDataCnt = my_nrf24l01p.read(NRF24L01P_PIPE_P0, rxData, sizeof(rxData));
            pc.write(rxData, rxDataCnt);

            motor->execute(rxData); // executa o comando recebido na mensagem
        }
    }
}
