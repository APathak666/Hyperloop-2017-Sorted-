#include <FreeRTOS.h>
#include <Arduino.h>
#include <FlexCAN.h>
#include <task.h>

TaskHandle_t Ch = NULL;
void Check (void* args);

class CanListener1 : public CANListener 
{
public:
  bool frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller); //overrides the parent version so we can actually do something
};

CanListener1 canListener1;
CAN_message_t msg;


bool CanListener1::frameHandler(CAN_message_t &frame, int mailbox, uint8_t controller)
{
  digitalWrite(13, HIGH);
  delayMicroseconds(500000);
  digitalWrite(13, LOW);
  Serial1.println(frame.id);
  for (int i=0; i<8; i++)
    Serial1.println((char)frame.buf[i]);  
  return true;
  //xTaskNotifyFromISR(Ch, *((uint32_t*)frame.buf), eSetValueWithoutOverwrite, pdFALSE);
  xTaskNotify(Ch, *((uint32_t*)frame.buf), eSetValueWithoutOverwrite);
}
 
void Check (void* args)
{
  uint32_t myBuf;
  xTaskNotifyWait(0, 0, &myBuf, portMAX_DELAY);
  for (int i=0; i<4;i++)
    Serial1.println((uint8_t)myBuf + i);
}

int main()
{
    Serial1.begin(9600);
    xTaskCreate (Check, "Check", configMINIMAL_STACK_SIZE, NULL, 1, &Ch);
    vTaskStartScheduler();
    for (;;);
    return 0;
}