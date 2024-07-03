//
//    FILE: ADG729.cpp
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.0
//    DATE: 2024-07-03
// PURPOSE: Arduino Library for I2C ADG729 matrix switch. 2x4 Multiplexer.
//     URL: https://github.com/RobTillaart/ADG729


#include "ADG729.h"


ADG729::ADG729(uint8_t deviceAddress, TwoWire *wire)
{
  _address  = deviceAddress;
  _wire     = wire;
  _mask     = 0x00;
  _resetPin = -1;
  _forced   = false;
  _error    = ADG729_OK;
  _channels = 8;
}


bool ADG729::begin(uint8_t mask)
{
  if (! isConnected()) return false;
  setChannelMask(mask);
  return true;
}


bool ADG729::isConnected()
{
  _wire->beginTransmission(_address);
  return ( _wire->endTransmission() == 0);
}


uint8_t ADG729::channelCount()
{
  return _channels;
}


bool ADG729::enableChannel(uint8_t channel)
{
  if (channel >= _channels) return false;
  return setChannelMask(_mask | (0x01 << channel));
}


bool ADG729::disableChannel(uint8_t channel)
{
  if (channel >= _channels) return false;
  return setChannelMask(_mask & ~(0x01 << channel));
}


bool ADG729::selectChannel(uint8_t channel)
{
  if (channel >= _channels) return false;
  return setChannelMask(0x01 << channel);
}


bool ADG729::isEnabled(uint8_t channel)
{
  if (channel >= _channels) return false;
  return (_mask & (0x01 << channel));
}


bool ADG729::disableAllChannels()
{
  return setChannelMask(0x00);
}


bool ADG729::setChannelMask(uint8_t mask)
{
  if ((_mask == mask) && (not _forced)) return true;
  _mask = mask;
  _wire->beginTransmission(_address);
  _wire->write(_mask);
  _error = _wire->endTransmission();
  return (_error == 0);
}


uint8_t ADG729::getChannelMask()
{
  if (_forced)  //  read from device.
  {
    _wire->requestFrom(_address, (uint8_t)1);
    _mask = _wire->read();
  }
  return _mask;
}


void ADG729::setResetPin(uint8_t resetPin)
{
  _resetPin = resetPin;
  pinMode(_resetPin, OUTPUT);
  digitalWrite(_resetPin, HIGH);  //  page 3 HIGH is normal operation
}


void ADG729::reset()
{
  if (_resetPin != -1)
  {
    digitalWrite(_resetPin, LOW);
    delayMicroseconds(1);
    digitalWrite(_resetPin, HIGH);
  }
}


void ADG729::setForced(bool forced)
{
  _forced = forced;
}


bool ADG729::getForced()
{
  return _forced;
}


int ADG729::getError()
{
  int error = _error;
  _error = ADG729_OK;
  return error;
}


//  -- END OF FILE --

