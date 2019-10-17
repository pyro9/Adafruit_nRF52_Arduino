/*
 * SPI Master library for nRF5x.
 * Copyright (c) 2015 Arduino LLC
 * Copyright (c) 2016 Sandeep Mistry All right reserved.
 * Copyright (c) 2019 Ha Thach for Adafruit Industries. All right reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

// Due to nRF52832 Errata with SPIM, we use the legacy SPI for 832 variant
// https://infocenter.nordicsemi.com/topic/errata_nRF52832_Rev2/ERR/nRF52832/Rev2/latest/anomaly_832_58.html
#ifdef NRF52832_XXAA

#include "SPI.h"
#include <Arduino.h>
#include <wiring_private.h>
#include <assert.h>


const SPISettings DEFAULT_SPI_SETTINGS = SPISettings();

SPIClass::SPIClass(NRF_SPIM_Type *p_spi, uint8_t uc_pinMISO, uint8_t uc_pinSCK, uint8_t uc_pinMOSI)
{
  initialized = false;
  assert(p_spi != NULL);

  _p_spi = (NRF_SPI_Type*) p_spi;

// not used by 832
//  _spim.p_reg = p_spi;
//  _spim.drv_inst_idx = 0;

  // pins
  _uc_pinMiso = g_ADigitalPinMap[uc_pinMISO];
  _uc_pinSCK = g_ADigitalPinMap[uc_pinSCK];
  _uc_pinMosi = g_ADigitalPinMap[uc_pinMOSI];

  _dataMode = SPI_MODE0;
  _bitOrder = SPI_CONFIG_ORDER_MsbFirst;
}

void SPIClass::begin()
{
  if (initialized) return;
  initialized = true;

  _p_spi->PSELSCK  = _uc_pinSCK;
  _p_spi->PSELMOSI = _uc_pinMosi;
  _p_spi->PSELMISO = _uc_pinMiso;

  beginTransaction(DEFAULT_SPI_SETTINGS);
}

void SPIClass::beginTransaction(SPISettings settings)
{
  _p_spi->ENABLE = (SPI_ENABLE_ENABLE_Disabled << SPI_ENABLE_ENABLE_Pos);

  this->_bitOrder = (settings.bitOrder == MSBFIRST ? SPI_CONFIG_ORDER_MsbFirst : SPI_CONFIG_ORDER_LsbFirst);
  uint32_t config = this->_bitOrder;

  switch (settings.dataMode) {
    default:
    case SPI_MODE0:
      config |= (SPI_CONFIG_CPOL_ActiveHigh << SPI_CONFIG_CPOL_Pos);
      config |= (SPI_CONFIG_CPHA_Leading    << SPI_CONFIG_CPHA_Pos);
      break;

    case SPI_MODE1:
      config |= (SPI_CONFIG_CPOL_ActiveHigh << SPI_CONFIG_CPOL_Pos);
      config |= (SPI_CONFIG_CPHA_Trailing   << SPI_CONFIG_CPHA_Pos);
      break;

    case SPI_MODE2:
      config |= (SPI_CONFIG_CPOL_ActiveLow  << SPI_CONFIG_CPOL_Pos);
      config |= (SPI_CONFIG_CPHA_Leading    << SPI_CONFIG_CPHA_Pos);
      break;

    case SPI_MODE3:
      config |= (SPI_CONFIG_CPOL_ActiveLow  << SPI_CONFIG_CPOL_Pos);
      config |= (SPI_CONFIG_CPHA_Trailing   << SPI_CONFIG_CPHA_Pos);
      break;
  }

  _p_spi->CONFIG = config;
  setClockDivider(F_CPU / settings.clockFreq);

  _p_spi->ENABLE = (SPI_ENABLE_ENABLE_Enabled << SPI_ENABLE_ENABLE_Pos);
}

void SPIClass::end()
{
  _p_spi->ENABLE = (SPI_ENABLE_ENABLE_Disabled << SPI_ENABLE_ENABLE_Pos);

  initialized = false;
}

void SPIClass::usingInterrupt(int /*interruptNumber*/)
{
}

//void SPIClass::beginTransaction(SPISettings settings)
//{
//  config(settings);
//}

void SPIClass::endTransaction(void)
{
  _p_spi->ENABLE = (SPI_ENABLE_ENABLE_Disabled << SPI_ENABLE_ENABLE_Pos);
}

void SPIClass::setBitOrder(BitOrder order)
{
  this->_bitOrder = (order == MSBFIRST ? SPI_CONFIG_ORDER_MsbFirst : SPI_CONFIG_ORDER_LsbFirst);

  uint32_t config = this->_bitOrder;

  switch (this->_dataMode) {
    default:
    case SPI_MODE0:
      config |= (SPI_CONFIG_CPOL_ActiveHigh << SPI_CONFIG_CPOL_Pos);
      config |= (SPI_CONFIG_CPHA_Leading    << SPI_CONFIG_CPHA_Pos);
      break;

    case SPI_MODE1:
      config |= (SPI_CONFIG_CPOL_ActiveHigh << SPI_CONFIG_CPOL_Pos);
      config |= (SPI_CONFIG_CPHA_Trailing   << SPI_CONFIG_CPHA_Pos);
      break;

    case SPI_MODE2:
      config |= (SPI_CONFIG_CPOL_ActiveLow  << SPI_CONFIG_CPOL_Pos);
      config |= (SPI_CONFIG_CPHA_Leading    << SPI_CONFIG_CPHA_Pos);
      break;

    case SPI_MODE3:
      config |= (SPI_CONFIG_CPOL_ActiveLow  << SPI_CONFIG_CPOL_Pos);
      config |= (SPI_CONFIG_CPHA_Trailing   << SPI_CONFIG_CPHA_Pos);
      break;
  }

  _p_spi->CONFIG = config;
}

void SPIClass::setDataMode(uint8_t mode)
{
  this->_dataMode = mode;

  uint32_t config = this->_bitOrder;

  switch (this->_dataMode) {
    default:
    case SPI_MODE0:
      config |= (SPI_CONFIG_CPOL_ActiveHigh << SPI_CONFIG_CPOL_Pos);
      config |= (SPI_CONFIG_CPHA_Leading    << SPI_CONFIG_CPHA_Pos);
      break;

    case SPI_MODE1:
      config |= (SPI_CONFIG_CPOL_ActiveHigh << SPI_CONFIG_CPOL_Pos);
      config |= (SPI_CONFIG_CPHA_Trailing   << SPI_CONFIG_CPHA_Pos);
      break;

    case SPI_MODE2:
      config |= (SPI_CONFIG_CPOL_ActiveLow  << SPI_CONFIG_CPOL_Pos);
      config |= (SPI_CONFIG_CPHA_Leading    << SPI_CONFIG_CPHA_Pos);
      break;

    case SPI_MODE3:
      config |= (SPI_CONFIG_CPOL_ActiveLow  << SPI_CONFIG_CPOL_Pos);
      config |= (SPI_CONFIG_CPHA_Trailing   << SPI_CONFIG_CPHA_Pos);
      break;
  }

  _p_spi->CONFIG = config;
}

void SPIClass::setClockDivider(uint32_t div)
{
  uint32_t clockFreq;

  // Adafruit Note: nrf52 run at 64MHz
  if (div >= SPI_CLOCK_DIV512) {
    clockFreq = SPI_FREQUENCY_FREQUENCY_K125;
  } else if (div >= SPI_CLOCK_DIV256) {
    clockFreq = SPI_FREQUENCY_FREQUENCY_K250;
  } else if (div >= SPI_CLOCK_DIV128) {
    clockFreq = SPI_FREQUENCY_FREQUENCY_K500;
  } else if (div >= SPI_CLOCK_DIV64) {
    clockFreq = SPI_FREQUENCY_FREQUENCY_M1;
  } else if (div >= SPI_CLOCK_DIV32) {
    clockFreq = SPI_FREQUENCY_FREQUENCY_M2;
  } else if (div >= SPI_CLOCK_DIV16) {
    clockFreq = SPI_FREQUENCY_FREQUENCY_M4;
  } else {
    clockFreq = SPI_FREQUENCY_FREQUENCY_M8;
  }

  _p_spi->FREQUENCY = clockFreq;
}

byte SPIClass::transfer(uint8_t data)
{
  _p_spi->TXD = data;

  while(!_p_spi->EVENTS_READY);

  data = _p_spi->RXD;

  _p_spi->EVENTS_READY = 0x0UL;

  return data;
}

uint16_t SPIClass::transfer16(uint16_t data) {
  union { uint16_t val; struct { uint8_t lsb; uint8_t msb; }; } t;

  t.val = data;

  if (_bitOrder == SPI_CONFIG_ORDER_LsbFirst) {
    t.lsb = transfer(t.lsb);
    t.msb = transfer(t.msb);
  } else {
    t.msb = transfer(t.msb);
    t.lsb = transfer(t.lsb);
  }

  return t.val;
}

void SPIClass::attachInterrupt() {
  // Should be enableInterrupt()
}

void SPIClass::detachInterrupt() {
  // Should be disableInterrupt()
}

// SPIM0, SPIM1 are configured as I2C
SPIClass SPI(NRF_SPIM2,  PIN_SPI_MISO,  PIN_SPI_SCK,  PIN_SPI_MOSI);

#endif // NRF52832_XXAA
