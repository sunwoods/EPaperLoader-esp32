
int EPD_10IN2B_Init() 
{
    EPD_Reset();
    // EPD_WaitUntilIdle(); 
        
    //960x640

    EPD_SendCommand(0x12);       //SWRESET
    EPD_WaitUntilIdle_high();         //waiting for the electronic paper IC to release the idle signal
    
    EPD_SendCommand(0x0C);  // Soft start setting
    EPD_SendData(0xAE);
    EPD_SendData(0xC7);
    EPD_SendData(0xC3);
    EPD_SendData(0xC0);
    EPD_SendData(0x80);   

    EPD_SendCommand(0x01);  // Set MUX as 639
    EPD_SendData(0x7F);
    EPD_SendData(0x02);
    EPD_SendData(0x00);

    EPD_SendCommand(0x11);  // Data entry mode
    // EPD_SendData(0x01);  // Y increment, X decrement
    EPD_SendData(0x03);   // Y increment, X increment
    
    EPD_SendCommand(0x44);
    EPD_SendData(0x00); // RAM x address start at 0
    EPD_SendData(0x00);
    EPD_SendData(0xBF); // RAM x address end at 3BFh -> 959
    EPD_SendData(0x03);

    EPD_SendCommand(0x45);
    // EPD_SendData(0x7F); // RAM y address start at 27Fh;
    // EPD_SendData(0x02);
    // EPD_SendData(0x00); // RAM y address end at 00h;
    // EPD_SendData(0x00);
    EPD_SendData(0x00); // 0 ->
    EPD_SendData(0x00);
    EPD_SendData(0x7F); // Height - 1
    EPD_SendData(0x02);

    EPD_SendCommand(0x3C); // VBD
    EPD_SendData(0x01); // LUT1, for white

    EPD_SendCommand(0x18);
    EPD_SendData(0X80);
    
    EPD_SendCommand(0x4E); 
    EPD_SendData(0x00);
    EPD_SendData(0x00);

    EPD_SendCommand(0x4F); 
    EPD_SendData(0x7F);
    EPD_SendData(0x02);

    EPD_SendCommand(0x24);   //write RAM for black(0)/white (1)
    delay(2);
    return 0;
}

static void EPD_10IN2B_Show(void)
{
    EPD_SendCommand(0x22); //Display Update Control
    EPD_SendData(0xF7);   

    EPD_SendCommand(0x20); //Activate Display Update Sequence
    EPD_WaitUntilIdle_high();

    EPD_SendCommand(0x10); //enter deep sleep
    EPD_SendData(0x01); 
    delay(100);
}
