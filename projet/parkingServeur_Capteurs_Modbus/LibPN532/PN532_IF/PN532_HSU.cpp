////////////////////////////////////////////////////////////////////////////////
// pn532 hsu interface for mbed platform
// 
// by dotnfc@163.com
// 2016/09/10 18:16:00

#include "PN532_HSU.h"
#include "PN532_debug.h"


PN532_HSU::PN532_HSU(HardwareSerial &serial)
{
    _serial = &serial;
    command = 0;
}

void PN532_HSU::begin()
{
    _serial->baud(115200);
}

void PN532_HSU::wakeup()
{
    const uint8_t wakeup_cmd[] = { 0x55, 0x55, 0x00, 0x00, 0x00,     0x00, 0x00, 0xFF, 0x05, 0xFB,  0xD4, 0x14, 0x01, 0x00, 0x01,  0x16, 0x00 };
    _serial->write(wakeup_cmd, sizeof(wakeup_cmd));
    
    delay (50);
    
    /** dump serial buffer */
    if(_serial->readable()){
        DMSG("Dump sperial buffer: ");
    }
    while(_serial->readable()){
        uint8_t ret = _serial->getc();
        DMSG_HEX(ret);
    }
    DMSG("\n");
}

int8_t PN532_HSU::writeCommand(const uint8_t *header, uint8_t hlen, const uint8_t *body, uint8_t blen)
{
    /** dump serial buffer */
    if(_serial->readable()){
        DMSG("Dump sperial buffer: ");
    }
    while(_serial->readable()){
        uint8_t ret = _serial->getc();
        DMSG_HEX(ret);
    }
    DMSG("\n");
    
    command = header[0];
    
    _serial->write(PN532_PREAMBLE);
    _serial->write(PN532_STARTCODE1);
    _serial->write(PN532_STARTCODE2);
    
    uint8_t length = hlen + blen + 1;   // length of data field: TFI + DATA
    _serial->write(length);
    _serial->write(~length + 1);         // checksum of length
    
    _serial->write(PN532_HOSTTOPN532);
    uint8_t sum = PN532_HOSTTOPN532;    // sum of TFI + DATA

    DMSG("\nWrite: ");
    
    _serial->write(header, hlen);
    for (uint8_t i = 0; i < hlen; i++) {
        sum += header[i];

        DMSG_HEX(header[i]);
    }

    _serial->write(body, blen);
    for (uint8_t i = 0; i < blen; i++) {
        sum += body[i];

        DMSG_HEX(body[i]);
    }
    
    uint8_t checksum = ~sum + 1;            // checksum of TFI + DATA
    _serial->write(checksum);
    _serial->write(PN532_POSTAMBLE);

    delay (50);
    return readAckFrame();
}

int16_t PN532_HSU::readResponse(uint8_t buf[], uint8_t len, uint16_t timeout)
{
    uint8_t tmp[3];
    
    delay(100);
    
    DMSG("\nRead:  ");
    
    /** Frame Preamble and Start Code */
    if(receive(tmp, 3, timeout)<=0){
        return PN532_TIMEOUT;
    }
    if(0 != tmp[0] || 0!= tmp[1] || 0xFF != tmp[2]){
        DMSG("Preamble error");
        return PN532_INVALID_FRAME;
    }
    
    /** receive length and check */
    uint8_t length[2];
    if(receive(length, 2, timeout) <= 0){
        return PN532_TIMEOUT;
    }
    if( 0 != (uint8_t)(length[0] + length[1]) ){
        DMSG("Length error\n");
        return PN532_INVALID_FRAME;
    }
    length[0] -= 2;
    if( length[0] > len){
        return PN532_NO_SPACE;
    }
    
    /** receive command byte */
    uint8_t cmd = command + 1;               // response command
    if(receive(tmp, 2, timeout) <= 0){
        return PN532_TIMEOUT;
    }
    if( PN532_PN532TOHOST != tmp[0] || cmd != tmp[1]){
        DMSG("Command error\n");
        return PN532_INVALID_FRAME;
    }
    
    if(receive(buf, length[0], timeout) != length[0]){
        return PN532_TIMEOUT;
    }
    uint8_t sum = PN532_PN532TOHOST + cmd;
    for(uint8_t i=0; i<length[0]; i++){
        sum += buf[i];
    }
    
    /** checksum and postamble */
    if(receive(tmp, 2, timeout) <= 0){
        return PN532_TIMEOUT;
    }
    if( 0 != (uint8_t)(sum + tmp[0]) || 0 != tmp[1] ){
        DMSG("Checksum error\n");
        return PN532_INVALID_FRAME;
    }
    
    DMSG ("\n");
    return length[0];
}

int8_t PN532_HSU::readAckFrame()
{
    const uint8_t PN532_ACK[] = {0, 0, 0xFF, 0, 0xFF, 0};
    uint8_t ackBuf[sizeof(PN532_ACK)];
    
    delay(100);
    
    DMSG("\nAck: ");
    
    if( receive(ackBuf, sizeof(PN532_ACK), PN532_ACK_WAIT_TIME) <= 0 ){
        DMSG("Timeout\n");
        return PN532_TIMEOUT;
    }
    
    if( memcmp(ackBuf, PN532_ACK, sizeof(PN532_ACK)) ){
        DMSG("Invalid\n");
        return PN532_INVALID_ACK;
    }
    return 0;
}

/**
    @brief receive data .
    @param buf --> return value buffer.
           len --> length expect to receive.
           timeout --> time of reveiving
    @retval number of received bytes, 0 means no data received.
*/
int8_t PN532_HSU::receive(uint8_t *buf, int len, uint16_t timeout)
{
  int read_bytes = 0;
  int ret;
  Timer tmr_hsu;
  unsigned long start_millis;
    
  tmr_hsu.start();
  while (read_bytes < len) {
    start_millis = tmr_hsu.read_ms(); // millis();
    do {
      ret = _serial->getc();
      if (ret >= 0) {
        break;
     }
     delay(10);
    } while((timeout == 0) || ((tmr_hsu.read_ms() /* millis() */ - start_millis ) < timeout));
    
    if (ret < 0) {
        if(read_bytes){
            tmr_hsu.stop ();
            return read_bytes;
        }else{
            tmr_hsu.stop ();
            return PN532_TIMEOUT;
        }
    }
    buf[read_bytes] = (uint8_t)ret;
    DMSG_HEX(ret);
    read_bytes++;
  }
  
  tmr_hsu.stop ();
  return read_bytes;
}
