
#include "MCP4561.h"
#include <mbed.h>

I2C i2c(p30, p31);
  //<<constructor>> 
  MCP4561::MCP4561(char dev_address) // define the device adress for this instance
  {
  dev_ADDR= dev_address;
  };
  //<<destructor>>
  MCP4561::~MCP4561(){/*nothing to destruct*/};
  
uint8_t MCP4561::write(char mem_addr, uint16_t setValue) // mem_addr is 00-0F, setvalue is 0-257
{
////if you set the volatile output register, the same value is put to the non-volatile register
	if(setValue <0)
	{
	setValue =0;
	}
	if(setValue >257)
	{
	setValue =257;
	}
	if(mem_addr <0x00 || mem_addr >0x0F )
	{
	return 0;
	}

	
	char cmd_byte = 0x00, data_byte = 0x00;   
	cmd_byte = ((mem_addr << 4) & 0xF0) | (((setValue & 0x01FF) >> 8)&0x03);   //  top 4 is now address   2 command   2 data (D9,D8)
	data_byte = ((setValue) & (0xff));           // is now D7-D0
	i2c.start();
	char cmd[2];                                      // device address                 0101110x
    cmd[0] = cmd_byte;								  // sends command byte             AAAACCDD
	cmd[1] = data_byte;								  // sends potentiometer value byte DDDDDDDD  (D7-D0)
	i2c.write(dev_ADDR, cmd, 2);
	i2c.stop();

  if (mem_addr == WIPER_0_NON_VOLATILE || mem_addr == WIPER_1_NON_VOLATILE) 
  {
		//delay(10); // EEPROM takes 5 - 10 ms to write ( datasheet page 12 )
	  wait_ms(10);
  }
  else {
	  //delay(2); // NV memory is faster
	  wait_ms(2);
  }

//give unit time to apply the value to non volatile register
  uint8_t set_reading = read(mem_addr);
  if (set_reading == setValue)
  {
  return 1; // it has accepted our setting ( EEPROM reflects what we set it to )
  }
  return 0;
}


uint16_t MCP4561::read(char mem_addr)// mem addr 0x00 - 0x0f ( 0-16 )
{
	if(mem_addr <0x00 || mem_addr >0x0F )
	{
	return 0x0FFF; // return something that is out of the expected bounds to signify an error
	}
   char cmd_byte = 0x0F;
  
   cmd_byte = (mem_addr<<4) | 0x0C ; 
   i2c.start();
   char cmd[2];
   cmd[0] = cmd_byte;
   i2c.write(dev_ADDR, cmd, 1);
   i2c.stop();
   i2c.start();
   i2c.read(dev_ADDR, cmd, 2);
   uint16_t tmp = (cmd[0] << 8) | cmd[1];
   i2c.stop();
   return tmp;
}


uint8_t MCP4561::openCircuit(char mem_addr)// disconnect teh internal connectiuon in the pot, creating an open circuit
{
	
	if (mem_addr <0x00 || mem_addr >0x0F)
	{
		return 0;
	}

return write(mem_addr,0x0088);//returns 1 if the operation was succesful, 0 otherwise
}

uint8_t MCP4561::enableOutput(char mem_addr)// disconnect the internal connectiuon in the pot, creating an open circuit
	
{
	if (mem_addr <0x00 || mem_addr >0x0F)
	{
		return 0;
	}
return write(mem_addr,0x00FF);
//returns 1 if the operation was succesful, 0 otherwise
}
