
int EPD_Init_4in37b() 
{
    EPD_Reset();

    //Soft-reset
    EPD_Send_1(0x00, 0x0e);
    EPD_WaitUntilIdle();

    //Input Temperature
    EPD_Send_1(0xe5, 0x19);
    //Active Temperature
    EPD_Send_1(0xe0, 0x02);
    //PSR
    EPD_Send_2(0x00, 0xcf, 0x8d);


    // EPD_SendCommand(0x04);//POWER_ON
    // EPD_WaitUntilIdle();
    
    // EPD_Send_2(0x00, 0x0F, 0x89);//PANEL_SETTING
    // EPD_Send_3(0x61, 0xB0, 0x01, 0xE0);//TCON_RESOLUTION
    // EPD_Send_1(0x50, 0x77);//VCOM_AND_DATA_INTERVAL_SETTING;

    EPD_SendCommand(0x10);//DATA_START_TRANSMISSION_1  
    delay(2);
    return 0;
}

void EPD_4IN37B_Show() 
{
    EPD_SendCommand(0x04);//POWER_ON
    // EPD_Send_1(0x04, 0x00);//POWER_ON
    EPD_WaitUntilIdle();

    // Refresh
    EPD_SendCommand(0x12);// DISPLAY_REFRESH
    delay(100);
    EPD_WaitUntilIdle();

    // Sleep
    EPD_SendCommand(0x02);// POWER_OFF
    EPD_WaitUntilIdle();
    // EPD_Send_1(0x07, 0xA5);// DEEP_SLEEP
}