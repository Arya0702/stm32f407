#include "main.h"
#include "cmsis_os.h"
#include "MI1640.h"
#include "usb_host.h"
#include "usbh_cdc.h"

extern USBH_HandleTypeDef hUsbHostFS;

void Send_Camera_Command(const char* cmd_content) 
{
    char tx[64];
    uint16_t checksum = 0;

    for (int i = 0; i < strlen(cmd_content); i++) 
    {
        checksum += (uint8_t)cmd_content[i];
    }

    snprintf(tx, sizeof(tx), "   #%s%04X", cmd_content, checksum);

    USBH_CDC_Transmit(&hUsbHostFS, (uint8_t*)tx, strlen(tx));
}


void Start_Camera_Stream(void) 
{
    Send_Camera_Command("000CWREGB102");
}






