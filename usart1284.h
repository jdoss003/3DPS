

#ifndef USART1284_H
#define USART1284_H

#ifdef __cplusplus
extern "C" {
#endif

void USART_init(unsigned char usartNum);
void USART_initBaud(unsigned char usartNum, unsigned short baudRate);
unsigned char USART_isSendReady(unsigned char usartNum);
unsigned char USART_hasTransmitted(unsigned char usartNum);
unsigned char USART_hasReceived(unsigned char usartNum);
void USART_send(unsigned char toSend, unsigned char usartNum);
unsigned char USART_receive(unsigned char usartNum);

void USART_autoRecieve(unsigned char b, unsigned char usartNum);
unsigned char USART_hasLine(unsigned char usartNum);
char* USART_getLine(unsigned char usartNum);
void USART_clearBuf(unsigned char usartNum);

void USART_sendLine(char* l, unsigned char usartNum);
unsigned char USART_hasTransmittedLine(unsigned char usartNum);

#ifdef __cplusplus
}
#endif

#endif //USART1284_H
