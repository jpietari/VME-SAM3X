#define MAC_ADDRESS_OFFSET   (0x00FC0000)
#define SN_ASCII_OFFSET      (0x00FC0006)
#define SN_ASCII_MAXLEN      26

void ext_flash_print_status(m25p_status_t status);
m25p_status_t ext_flash_read_mac_address(uint8_t *mac_address);
m25p_status_t ext_flash_read_sn(char *sn, uint32_t sn_len);
m25p_status_t ext_flash_write_mac_address(uint8_t *mac_address);
m25p_status_t ext_flash_write_sn(char *sn, uint32_t sn_len);
m25p_status_t ext_flash_erase();
m25p_status_t ext_flash_wait_for_ready();
