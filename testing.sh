#!/bin/sh

echo "*** BUS 0 ***************************"

echo "Testing MPU 0x69 Channel 1"
imu mpu 0 0x74/1/0x69 config i2c_bypass:on >/dev/null
i2c | grep "0x1c"
imu mpu 0 0x74/1/0x69 config i2c_bypass:off >/dev/null

echo "Testing ITG 0x68 Channel 1"
imu itg 0 0x74/1/0x68 config i2c_bypass:on >/dev/null
i2c | grep "0x1c"
imu itg 0 0x74/1/0x68 config i2c_bypass:off >/dev/null

echo "Testing ITG 0x69 Channel 2"
imu itg 0 0x74/1/0x68 config i2c_bypass:on >/dev/null
i2c | grep "0x1c"
imu itg 0 0x74/1/0x68 config i2c_bypass:on >/dev/null

echo "*** BUS 1 ***************************"

echo "Testing MPU 0x69 Channel 1"
imu mpu 1 0x74/1/0x69 config i2c_bypass:on >/dev/null
i2c | grep "0x1c"
imu mpu 1 0x74/1/0x69 config i2c_bypass:off >/dev/null

echo "Testing ITG 0x68 Channel 1"
imu itg 1 0x74/1/0x68 config i2c_bypass:on >/dev/null
i2c | grep "0x1c"
imu itg 1 0x74/1/0x68 config i2c_bypass:off >/dev/null

echo "Testing ITG 0x69 Channel 2"
imu itg 1 0x74/1/0x68 config i2c_bypass:on >/dev/null
i2c | grep "0x1c"
imu itg 1 0x74/1/0x68 config i2c_bypass:on >/dev/null

echo "...done."