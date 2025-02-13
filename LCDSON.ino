//counter
#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11); // (TX, RX) DWIN HMI bağlantısı için

bool debugFlag = false;
uint8_t UARTReceive[50]; // Gelen verileri saklamak için buffer
uint8_t uartBuffCnt = 0;

uint8_t UARTReceivedebug[50]; // Gelen verileri saklamak için buffer
uint8_t uartBuffCntdebug = 0;

bool dataReceived = false;  // Tam bir veri paketi alındı mı?
uint8_t debugflag[10];

uint16_t counter = 0;  // Data Variables için sayaç
bool iconState = false; // İkon durumunu tutan değişken

void dwinPageControl(uint8_t PageID);
void UART_ICON(uint16_t add, uint8_t VALUE);
void dwinPtext(uint16_t add, uint16_t state);

void chartohex(uint8_t *str ,uint8_t sizet){
  
          for (uint8_t i = 0; i < sizet; i++) {
            if (str[i] < 0x10) {
                Serial.print("0");  
            }
            Serial.print(str[i], HEX);
            Serial.print(" ");
        }
        Serial.println(" ");
}

void UART_IT() {
    if (mySerial.available()>0) {
        uint8_t receivedByte = mySerial.read();
        
            Serial.print("bufcount ::   ");
            Serial.println(uartBuffCnt);
            UARTReceive[uartBuffCnt++] = receivedByte;
            UARTReceivedebug[uartBuffCntdebug++] = receivedByte;
            chartohex(UARTReceivedebug,uartBuffCntdebug);
        if(UARTReceive[uartBuffCnt-1] == 0x4f && UARTReceive[uartBuffCnt] == 0x4b){
             memset(UARTReceivedebug, 0, sizeof(UARTReceivedebug));
             uartBuffCntdebug=0;
             uartBuffCnt = 0;
              dataReceived = false;
              memset(UARTReceive, 0, sizeof(UARTReceive));
              return 0;
        }
        if (uartBuffCnt >= 6 && UARTReceive[0] == 0x5A && UARTReceive[1] == 0xA5) {
                dataReceived = true;  
                
        }

        // Data Variables için artırma işlemi (0x5A, 0xA5, ..., 0x20, 0x00, ...)
        if (uartBuffCnt > 6 && UARTReceive[uartBuffCnt-2] == 0x01 && UARTReceive[uartBuffCnt] == 0x00) {
            counter++;  // Değeri artır
            dwinPtext(0x3000, counter);  // Yeni değeri ekrana gönder
            Serial.print("Yeni Değer: ");
            Serial.println(counter);
            //debugFlag=true;
            iconState = !iconState;  // İkonu değiştir
            byte btnonoff=iconState ? 0x01 : 0x00;
            UART_ICON(0x1001, btnonoff);
            uartBuffCnt = 0;  // Buffer sıfırla

                         memset(UARTReceivedebug, 0, sizeof(UARTReceivedebug));
             uartBuffCntdebug=0;
             uartBuffCnt = 0;
              dataReceived = false;
              memset(UARTReceive, 0, sizeof(UARTReceive));
              return 0;
          
        }


        
        for (int i = 0; i < uartBuffCnt; i++) { 
   // if (UARTReceive[i] < 0x10) Serial.print("0"); 
    //Serial.print(UARTReceive[i], HEX);
    //Serial.print(" ");
}
     //   Serial.println(" ");
       // Serial.print("Yeni Değer: ");
      //  Serial.println(counter);
       

        
    }
}

void printReceivedData() {
    if (dataReceived) {
        Serial.print("Gelen veri: ");
        for (uint8_t i = 0; i < uartBuffCnt; i++) {
            if (UARTReceive[i] < 0x10) {
                Serial.print("0");  
            }
            Serial.print(UARTReceive[i], HEX);
            Serial.print(" ");
        }
       
        Serial.println();

        uartBuffCnt = 0;
        dataReceived = false;
        memset(UARTReceive, 0, sizeof(UARTReceive));
    }
}

void setup() {
    mySerial.begin(115200);
    Serial.begin(115200);
    dwinPtext(0x3000, counter); // Başlangıç değerini ekrana gönder
}

void loop() {
    UART_IT();
   // printReceivedData();

    if (debugFlag) {
        iconState = !iconState;  // İkonu değiştir
        byte btnonoff=iconState ? 0x01 : 0x00;
        UART_ICON(0x1001, btnonoff);
        Serial.print("ICON Durumu: ");
        Serial.println(iconState ? "AÇIK" : "KAPALI");
        debugFlag = false;
    }

    if (Serial.available()) {  
        char receivedChar = Serial.read();
        if (receivedChar == '1') { 
            debugFlag = true;
        }else{
             memset(UARTReceivedebug, 0, sizeof(UARTReceivedebug));
             uartBuffCntdebug=0;
             uartBuffCnt = 0;
              dataReceived = false;
              memset(UARTReceive, 0, sizeof(UARTReceive));
          
        }
    }
}

void dwinPageControl(uint8_t PageID) {
    uint8_t sendBuf[10] = {0x5A, 0xA5, 0x07, 0x82, 0x00, 0x84, 0x5A, 0x01, 0x10, PageID};
    mySerial.write(sendBuf, sizeof(sendBuf));
}

void UART_ICON(uint16_t add, uint8_t VALUE) {
    uint8_t sendBuf[8] = {0x5A, 0xA5, 0x05, 0x82, (uint8_t)(add >> 8), (uint8_t)add, 0x00, VALUE};
    mySerial.write(sendBuf, sizeof(sendBuf));
    Serial.print("ICON Güncellendi: ");  
    Serial.println(VALUE, HEX);
    
}

// Data Variables (0x3000) değerini artırma
void dwinPtext(uint16_t add, uint16_t state) {
    uint8_t lowadd = add;
    uint8_t upadd = add >> 8;
    uint8_t lowbyte = state;
    uint8_t upbyte = state >> 8;

    uint8_t sendBuf[8] = {0x5A, 0xA5, 0x05, 0x82, upadd, lowadd, upbyte, lowbyte};
    mySerial.write(sendBuf, sizeof(sendBuf));  
}
