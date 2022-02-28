#include <stdlib.h>
#include <string.h>

/* Scheduler include files. */
/* #include "asf.h" */
#include "FreeRTOS.h"
#include "task.h"
#include "status_codes.h"
#include "conf_uart_serial.h"
#include "twi.h"
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
#include "ext_flash.h"

void ext_flash_print_status(m25p_status_t status)
{
  switch (status)
    {
    case M25P_SUCCESS:
      printf("SUCCESS");
      break;
    case M25P_SECTOR_PROTECTED:
      printf("SECTOR_PROTECTED");
      break;
    case M25P_SECTOR_UNPROTECTED:
      printf("SECTOR_UNPROTECTED");
      break;
    case M25P_ERROR_INIT:
      printf("ERROR_INIT");
      break;
    case M25P_ERROR_NOT_FOUND:
      printf("ERROR_NOT_FOUND");
      break;
    case M25P_ERROR_WRITE:
      printf("ERROR_WRITE");
      break;
    case M25P_ERROR_BUSY:
      printf("ERROR_BUSY");
      break;
    case M25P_ERROR_PROTECTED:
      printf("ERROR_PROTECTED");
      break;
    case M25P_ERROR_SPI:
      printf("ERROR_SPI");
      break;
    case M25P_ERROR:
    default:
      printf("ERROR");
      break;
    }
}

m25p_status_t ext_flash_wait_for_ready()
{
  uint32_t timeout;
  uint8_t status;

  for (timeout = 100000; timeout; timeout--)
    {
      if (m25p_read_status_register(&status) == M25P_SUCCESS)
	if (!(status & M25P_SR_WIP))
	  return M25P_SUCCESS;
    }
  return M25P_ERROR;
}

m25p_status_t ext_flash_read_mac_address(uint8_t *mac_address)
{
  return m25p_read_data(MAC_ADDRESS_OFFSET, mac_address, 6);
}

m25p_status_t ext_flash_read_sn(char *sn, uint32_t sn_len)
{
  return m25p_read_data(SN_ASCII_OFFSET, (uint8_t *) sn, sn_len);
}

m25p_status_t ext_flash_write_mac_address(uint8_t *mac_address)
{
  m25p_status_t status;

  status = m25p_write_enable();
  if (status == M25P_SUCCESS)
    status = m25p_page_program(MAC_ADDRESS_OFFSET, mac_address, 6);
  m25p_write_disable();
  
  return status;  
}

m25p_status_t ext_flash_write_sn(char *sn, uint32_t sn_len)
{
  m25p_status_t status;

  status = m25p_write_enable();
  if (status == M25P_SUCCESS)
    status = m25p_page_program(SN_ASCII_OFFSET, sn,
			       sn_len > SN_ASCII_MAXLEN ?
			       SN_ASCII_MAXLEN : sn_len);
  m25p_write_disable();
  
  return status;  
}

m25p_status_t ext_flash_erase()
{
  m25p_status_t status;

  status = m25p_write_enable();
  if (status == M25P_SUCCESS)
    status = m25p_bulk_erase();
  m25p_write_disable();
  
  return status;  
}
