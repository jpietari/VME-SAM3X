#include <stdlib.h>
#include <string.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "status_codes.h"
#include "conf_uart_serial.h"
#include "twi.h"
#include "sam_twi/twi_master.h"
/* Demo file headers. */
#include "partest.h"
#include "ethernet.h"
#include "netif/etharp.h"
#include "flash.h"
#include "sysclk.h"
#include "ioport.h"
#include "uart_serial.h"
#include "stdio_serial.h"
#include <compiler.h>
#include "led.h"
#include "conf_board.h"

#include "evm300.h"

void fpga_twi_init(void)
{
  twi_options_t opt =
    {
      .speed = 400000,
      .chip = 0x30
    };

  gpio_configure_pin(TWI0_DATA_GPIO, TWI0_DATA_FLAGS);
  gpio_configure_pin(TWI0_CLK_GPIO, TWI0_CLK_FLAGS);

  twi_master_setup(TWI0, &opt);
}

void fpga_test_write(void)
{
  const uint8_t test_pattern[] = {0x31, 0x00, 0xaa, 0x55, 0x48};

  twi_packet_t packet_write;

  packet_write.addr[0] = 0x55;
  packet_write.addr[1] = 0xaa;
  packet_write.addr[2] = 0x48;
  packet_write.addr_length = 24;
  packet_write.chip = 0x30;
  packet_write.buffer = (void *) test_pattern;
  packet_write.length = sizeof(test_pattern);

  while (twi_master_write(TWI0, &packet_write) != TWI_SUCCESS);
}

void fpga_write_command(uint8_t command)
{
  twi_packet_t packet_write;
  uint8_t buffer[1];

  buffer[0] = command;
  packet_write.addr_length = 0;
  packet_write.chip = 0x30;
  packet_write.buffer = (void *) buffer;
  packet_write.length = 1;

  while (twi_master_write(TWI0, &packet_write) != TWI_SUCCESS);
}

void fpga_write_opb_address(uint32_t address)
{
  twi_packet_t packet_write;
  uint8_t buffer[5];

  buffer[0] = 0x31;
  buffer[1] = (address >> 24) & 0x00ff;
  buffer[2] = (address >> 16) & 0x00ff;
  buffer[3] = (address >> 8) & 0x00ff;
  buffer[4] = (address) & 0x00ff;
  /*  packet_write.addr[0] = 0x31; */
  packet_write.addr_length = 0;
  packet_write.chip = 0x30;
  packet_write.buffer = (void *) buffer;
  packet_write.length = 5;

  while (twi_master_write(TWI0, &packet_write) != TWI_SUCCESS);
}

uint32_t fpga_read_data()
{
  twi_packet_t packet_read;
  uint8_t buffer[5];
  uint32_t     data;

  packet_read.addr_length = 0;
  packet_read.chip = 0x30;
  packet_read.buffer = (void *) buffer;
  packet_read.length = 5;

  while (twi_master_read(TWI0, &packet_read) != TWI_SUCCESS);
  
  data = (((uint32_t) buffer[1]) << 24) |
    (((uint32_t) buffer[2]) << 16) |
    (((uint32_t) buffer[3]) << 8) |
    ((uint32_t) buffer[4]);

  return data;
}


uint32_t fpga_read_byte(uint32_t address)
{
  uint32_t data;

  fpga_write_opb_address(address);
  fpga_write_command(0x04);
  return ((fpga_read_data() >> 24) & 0x00ff);  
}

uint32_t fpga_read_short(uint32_t address)
{
  uint32_t data;

  fpga_write_opb_address(address);
  fpga_write_command(0x14);
  return ((fpga_read_data() >> 16) & 0x00ffff);
}

uint32_t fpga_read_long(uint32_t address)
{
  uint32_t data;

  fpga_write_opb_address(address);
  fpga_write_command(0x34);
  return fpga_read_data();  
}

void fpga_write_byte(uint32_t address, uint32_t data)
{
  twi_packet_t packet_write;
  uint8_t buffer[5];

  fpga_write_opb_address(address);
  buffer[0] = 0x03;
  buffer[1] = (data) & 0x00ff;
  packet_write.addr_length = 0;
  packet_write.chip = 0x30;
  packet_write.buffer = (void *) buffer;
  packet_write.length = 2;

  while (twi_master_write(TWI0, &packet_write) != TWI_SUCCESS);
}

void fpga_write_short(uint32_t address, uint32_t data)
{
  twi_packet_t packet_write;
  uint8_t buffer[5];

  fpga_write_opb_address(address);
  buffer[0] = 0x13;
  buffer[1] = (data >> 8) & 0x00ff;
  buffer[2] = (data) & 0x00ff;
  packet_write.addr_length = 0;
  packet_write.chip = 0x30;
  packet_write.buffer = (void *) buffer;
  packet_write.length = 3;

  while (twi_master_write(TWI0, &packet_write) != TWI_SUCCESS);
}

void fpga_write_long(uint32_t address, uint32_t data)
{
  twi_packet_t packet_write;
  uint8_t buffer[5];

  fpga_write_opb_address(address);
  buffer[0] = 0x33;
  buffer[1] = (data >> 24) & 0x00ff;
  buffer[2] = (data >> 16) & 0x00ff;
  buffer[3] = (data >> 8) & 0x00ff;
  buffer[4] = (data) & 0x00ff;
  packet_write.addr_length = 0;
  packet_write.chip = 0x30;
  packet_write.buffer = (void *) buffer;
  packet_write.length = 5;

  while (twi_master_write(TWI0, &packet_write) != TWI_SUCCESS);
}
