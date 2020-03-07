/*
 *     07-acel.ino
 *         Manejo del acelerometro MPU6050
 *         Usa interfase I2C
 *         Detalle ver hoja de datos del circuito integrado
 */

#include <Arduino.h>

#include <Wire.h>

/*
 *     Definiciones
 */

/*
    Defined in platformio.ini

    MPU6050_ADDRESS     I2C address
    SCL_PIN             I2C Clock pin 
    SDA_PIN             I2C Data pin
    SERIAL_BAUD         Serial baud rate
*/

// Factores de escala con respecto a fondo de  escala provistos en la hoja de datos
const uint16_t AccelScaleFactor = 16384;
const uint16_t GyroScaleFactor = 131;

// Direcciones de registros de configuracion del MPU6050
const uint8_t MPU6050_REGISTER_SMPLRT_DIV = 0x19;
const uint8_t MPU6050_REGISTER_USER_CTRL = 0x6A;
const uint8_t MPU6050_REGISTER_PWR_MGMT_1 = 0x6B;
const uint8_t MPU6050_REGISTER_PWR_MGMT_2 = 0x6C;
const uint8_t MPU6050_REGISTER_CONFIG = 0x1A;
const uint8_t MPU6050_REGISTER_GYRO_CONFIG = 0x1B;
const uint8_t MPU6050_REGISTER_ACCEL_CONFIG = 0x1C;
const uint8_t MPU6050_REGISTER_FIFO_EN = 0x23;
const uint8_t MPU6050_REGISTER_INT_ENABLE = 0x38;
const uint8_t MPU6050_REGISTER_ACCEL_XOUT_H = 0x3B;
const uint8_t MPU6050_REGISTER_SIGNAL_PATH_RESET = 0x68;

//     Variables para lectura de valores directos del chip


int16_t AccelX, AccelY, AccelZ, Temperature, GyroX, GyroY, GyroZ;

void
I2C_Write(uint8_t deviceAddress, uint8_t regAddress, uint8_t data)
{
    Wire.beginTransmission(deviceAddress);
    Wire.write(regAddress);
    Wire.write(data);
    Wire.endTransmission();
}

void
MPU6050_Init(void)
{
    delay(150);
    Serial.println("MPU6050_Init");
    I2C_Write(MPU6050_ADDRESS, MPU6050_REGISTER_SMPLRT_DIV, 0x07);
    I2C_Write(MPU6050_ADDRESS, MPU6050_REGISTER_PWR_MGMT_1, 0x01);
    I2C_Write(MPU6050_ADDRESS, MPU6050_REGISTER_PWR_MGMT_2, 0x00);
    I2C_Write(MPU6050_ADDRESS, MPU6050_REGISTER_CONFIG, 0x00);
    I2C_Write(MPU6050_ADDRESS, MPU6050_REGISTER_GYRO_CONFIG, 0x00);  // set +/-250 degree/second full scale
    I2C_Write(MPU6050_ADDRESS, MPU6050_REGISTER_ACCEL_CONFIG, 0x00); // set +/- 2g full scale
    I2C_Write(MPU6050_ADDRESS, MPU6050_REGISTER_FIFO_EN, 0x00);
    I2C_Write(MPU6050_ADDRESS, MPU6050_REGISTER_INT_ENABLE, 0x01);
    I2C_Write(MPU6050_ADDRESS, MPU6050_REGISTER_SIGNAL_PATH_RESET, 0x00);
    I2C_Write(MPU6050_ADDRESS, MPU6050_REGISTER_USER_CTRL, 0x00);
}

// read all 14 register
void
Read_RawValue(uint8_t deviceAddress, uint8_t regAddress)
{
    Wire.beginTransmission(deviceAddress);
    Wire.write(regAddress);
    Wire.endTransmission();
    Wire.requestFrom(deviceAddress, 14U);

    AccelX = (((int16_t)Wire.read() << 8) | Wire.read());
    AccelY = (((int16_t)Wire.read() << 8) | Wire.read());
    AccelZ = (((int16_t)Wire.read() << 8) | Wire.read());
    Temperature = (((int16_t)Wire.read() << 8) | Wire.read());
    GyroX = (((int16_t)Wire.read() << 8) | Wire.read());
    GyroY = (((int16_t)Wire.read() << 8) | Wire.read());
    GyroZ = (((int16_t)Wire.read() << 8) | Wire.read());
}

void
setup(void)
{
    Serial.begin(SERIAL_BAUD);
    Serial.println();
    Serial.println("Serial ready");
    Wire.begin(SDA_PIN, SCL_PIN); // Inicializacion de I2C
    MPU6050_Init();       //    Inicializacion del chip
}

void
loop(void)
{
    double Ax, Ay, Az, T, Gx, Gy, Gz;

    Read_RawValue(MPU6050_ADDRESS, MPU6050_REGISTER_ACCEL_XOUT_H);

    // divide each with their sensitivity scale factor
    Ax = (double)AccelX / AccelScaleFactor;
    Ay = (double)AccelY / AccelScaleFactor;
    Az = (double)AccelZ / AccelScaleFactor;
    T = (double)Temperature / 340 + 36.53; // temperature formula
    Gx = (double)GyroX / GyroScaleFactor;
    Gy = (double)GyroY / GyroScaleFactor;
    Gz = (double)GyroZ / GyroScaleFactor;

    Serial.print("Ax: ");
    Serial.print(Ax);
    Serial.print(" Ay: ");
    Serial.print(Ay);
    Serial.print(" Az: ");
    Serial.print(Az);
    Serial.print(" T: ");
    Serial.print(T);
    Serial.print(" Gx: ");
    Serial.print(Gx);
    Serial.print(" Gy: ");
    Serial.print(Gy);
    Serial.print(" Gz: ");
    Serial.println(Gz);

    delay(100);
}

