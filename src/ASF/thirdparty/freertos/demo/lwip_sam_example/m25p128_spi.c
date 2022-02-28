#include <stdlib.h>
#include <string.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "status_codes.h"
#include "conf_uart_serial.h"
#include "twi.h"
#include "spi.h"
#include "spi_master.h"
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
#include "m25p128_spi.h"

void m25p_spi_init()
{
  struct spi_device m25p_spi = {
    .id = 0
  };

  gpio_configure_pin(SPI0_MISO_GPIO, SPI0_MISO_FLAGS);
  gpio_configure_pin(SPI0_MOSI_GPIO, SPI0_MOSI_FLAGS);
  gpio_configure_pin(SPI0_SPCK_GPIO, SPI0_SPCK_FLAGS);
  gpio_configure_pin(SPI0_NPCS0_GPIO, SPI0_NPCS0_FLAGS);

  spi_master_init(SPI0);
  spi_master_setup_device(SPI0, &m25p_spi, SPI_MODE_0, 12000000, 0);
  spi_enable(SPI0);
}

m25p_status_t m25p_write_enable()
{
  struct spi_device m25p_spi = {
    .id = 0
  };
  uint8_t command = M25P_WREN;
  m25p_status_t stat;

  spi_select_device(SPI0, &m25p_spi);

  stat = spi_write_packet(SPI0, &command, 1);

  spi_deselect_device(SPI0, &m25p_spi);

  return stat;
}

m25p_status_t m25p_write_disable()
{
  struct spi_device m25p_spi = {
    .id = 0
  };
  uint8_t command = M25P_WRDI;
  m25p_status_t stat;

  spi_select_device(SPI0, &m25p_spi);

  stat = spi_write_packet(SPI0, &command, 1);

  spi_deselect_device(SPI0, &m25p_spi);

  return stat;
}

m25p_status_t m25p_read_dev_id(uint32_t *dev_id)
{
  struct spi_device m25p_spi = {
    .id = 0
  };
  uint8_t command = M25P_RDID;
  uint8_t *id = (uint8_t *) dev_id;
  m25p_status_t stat;

  spi_select_device(SPI0, &m25p_spi);

  if ((stat = spi_write_packet(SPI0, &command, 1)) == M25P_SUCCESS)
    {
      stat = spi_read_packet(SPI0, &id[0], 3);
    }
  /* Arrange uint32_t bytes, little endian */
  id[3] = id[0];
  id[0] = id[2];
  id[2] = id[3];
  id[3] = 0; 

  spi_deselect_device(SPI0, &m25p_spi);

  return stat;
}

m25p_status_t m25p_read_status_register(uint8_t *status)
{
  struct spi_device m25p_spi = {
    .id = 0
  };
  uint8_t command = M25P_RDSR;
  m25p_status_t stat;

  spi_select_device(SPI0, &m25p_spi);

  if ((stat = spi_write_packet(SPI0, &command, 1)) == M25P_SUCCESS)
    stat = spi_read_packet(SPI0, status, 1);

  spi_deselect_device(SPI0, &m25p_spi);

  return stat;
}

m25p_status_t m25p_write_status_register(uint8_t status)
{
  struct spi_device m25p_spi = {
    .id = 0
  };
  uint8_t command[2];
  m25p_status_t stat;

  command[0] = M25P_WRSR;
  command[1] = status;

  spi_select_device(SPI0, &m25p_spi);

  stat = spi_write_packet(SPI0, command, 2);

  spi_deselect_device(SPI0, &m25p_spi);

  return stat;
}

m25p_status_t m25p_read_data(uint32_t address, uint8_t *data, uint32_t len)
{
  struct spi_device m25p_spi = {
    .id = 0
  };
  uint8_t command[4];
  m25p_status_t stat;

  command[0] = M25P_READ;
  command[1] = (address >> 16) & 0x00ff;;
  command[2] = (address >> 8) & 0x00ff;;
  command[3] = address & 0x00ff;;

  spi_select_device(SPI0, &m25p_spi);

  if ((stat = spi_write_packet(SPI0, command, 4)) == M25P_SUCCESS)
    stat = spi_read_packet(SPI0, data, len);

  spi_deselect_device(SPI0, &m25p_spi);

  return stat;
}

m25p_status_t m25p_page_program(uint32_t address, uint8_t *data, uint32_t len)
{
  struct spi_device m25p_spi = {
    .id = 0
  };
  uint8_t command[4];
  m25p_status_t stat;

  command[0] = M25P_PP;
  command[1] = (address >> 16) & 0x00ff;;
  command[2] = (address >> 8) & 0x00ff;;
  command[3] = address & 0x00ff;;

  spi_select_device(SPI0, &m25p_spi);

  if ((stat = spi_write_packet(SPI0, command, 4)) == M25P_SUCCESS)
    stat = spi_write_packet(SPI0, data, len);

  spi_deselect_device(SPI0, &m25p_spi);

  return stat;
}

m25p_status_t m25p_sector_erase(uint32_t address)
{
  struct spi_device m25p_spi = {
    .id = 0
  };
  uint8_t command[4];
  m25p_status_t stat;

  command[0] = M25P_SE;
  command[1] = (address >> 16) & 0x00ff;;
  command[2] = (address >> 8) & 0x00ff;;
  command[3] = address & 0x00ff;;

  spi_select_device(SPI0, &m25p_spi);

  stat = spi_write_packet(SPI0, command, 4);

  spi_deselect_device(SPI0, &m25p_spi);

  return stat;
}

m25p_status_t m25p_bulk_erase()
{
  struct spi_device m25p_spi = {
    .id = 0
  };
  uint8_t command = M25P_BE;
  m25p_status_t stat;

  spi_select_device(SPI0, &m25p_spi);

  stat = spi_write_packet(SPI0, &command, 1);

  spi_deselect_device(SPI0, &m25p_spi);

  return stat;
}
