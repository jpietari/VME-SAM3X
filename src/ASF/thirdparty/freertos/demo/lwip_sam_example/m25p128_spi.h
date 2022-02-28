typedef enum m25p_status {
  M25P_SUCCESS = 0,
  M25P_SECTOR_PROTECTED,
  M25P_SECTOR_UNPROTECTED,
  M25P_ERROR_INIT,
  M25P_ERROR_NOT_FOUND,
  M25P_ERROR_WRITE,
  M25P_ERROR_BUSY,
  M25P_ERROR_PROTECTED,
  M25P_ERROR_SPI,
  M25P_ERROR
} m25p_status_t;

#define M25P_WREN      (0x06)
#define M25P_WRDI      (0x04)
#define M25P_RDID      (0x9F)
#define M25P_RDSR      (0x05)
#define M25P_SR_WIP    (0x01)
#define M25P_WRSR      (0x01)
#define M25P_READ      (0x03)
#define M25P_FAST_READ (0x0B)
#define M25P_PP        (0x02)
#define M25P_SE        (0xD8)
#define M25P_BE        (0xC7)

void m25p_spi_init(void);
m25p_status_t m25p_write_enable(void);
m25p_status_t m25p_write_disable(void);
m25p_status_t m25p_read_dev_id(uint32_t *dev_id);
m25p_status_t m25p_read_status_register(uint8_t *status);
m25p_status_t m25p_write_status_register(uint8_t status);
m25p_status_t m25p_read_data(uint32_t address, uint8_t *data, uint32_t len);
m25p_status_t m25p_page_program(uint32_t address, uint8_t *data, uint32_t len);
m25p_status_t m25p_sector_erase(uint32_t address);
m25p_status_t m25p_bulk_erase(void);
