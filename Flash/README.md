## Overview  
This document explains the implementation of Flash memory read and write operations using STM32 HAL libraries. The code provides two functions:  

- `Flash_Write(uint8_t num1, uint8_t num2)`: Writes two 8-bit values into Flash memory.  
- `Flash_Read(uint8_t *num1, uint8_t *num2)`: Reads the stored values from Flash memory.  

---

## Code Breakdown  

### **Flash_Write Function**  
This function performs the following operations:  

1. **Unlock Flash Memory**: Enables write access using `HAL_FLASH_Unlock()`.  
2. **Erase the Flash Page**:  
   - Uses `FLASH_EraseInitTypeDef` to configure erase settings.  
   - Calls `HAL_FLASHEx_Erase()` to erase the page.  
3. **Write Data to Flash**:  
   - Uses `HAL_FLASH_Program()` to write two `uint8_t` values at specified addresses.  
   - Since STM32 Flash operates on `half-word` (16-bit) programming, each byte is stored separately with a 2-byte offset.  
4. **Lock Flash Memory**: Disables further write access with `HAL_FLASH_Lock()`.  

### **Flash_Read Function**  
This function reads the stored values from Flash memory:  

1. Accesses Flash memory using pointer dereferencing.  
2. Retrieves the values stored at `FLASH_PAGE_ADDR` and `FLASH_PAGE_ADDR + 2`.  

---

## Important Notes  

- **Flash Page Erase**:  
  - Flash memory must be erased before writing.  
  - Erasing sets all bits to `1`, so writing can only change `1` to `0`.  

- **Word Alignment**:  
  - The STM32 Flash programming unit is `half-word` (16-bit).  
  - Writing a single byte requires handling alignment properly.  

- **Memory Wear**:  
  - Flash memory has a limited number of erase/write cycles.  
  - Minimize unnecessary writes to extend Flash lifespan.  

---

## Potential Improvements  

- Use **EEPROM Emulation** if frequent write operations are required.  
- Store multiple values in a structured format using a dedicated Flash sector.  
- Implement error handling to check `HAL_FLASHEx_Erase()` and `HAL_FLASH_Program()` return statuses.  

---

## Conclusion  

This implementation provides a basic framework for Flash memory read and write operations in STM32. It is suitable for applications requiring non-volatile storage, such as configuration settings or calibration parameters.  