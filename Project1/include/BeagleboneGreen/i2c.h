#ifndef I2C_H_
#define I2C_H_
#include <stdint.h>
/**
 * @brief 
 * 
 * @return int 
 */
int i2c_init(void);
/**
 * @brief 
 * 
 */
void i2c_close(void);
/**
 * @brief i2c write to register of slave device 
 * 
 * @param slave_addr 
 * @param reg 
 * @param data 
 * @return int 
 */
int i2c_write(uint8_t slave_addr, uint8_t reg, uint8_t data);
/**
 * @brief i2c read from register of slave device 
 * 
 * @param slave_addr 
 * @param reg 
 * @param result 
 * @return int 
 */
int i2c_read(uint8_t slave_addr, uint8_t reg, uint8_t *result);
#endif