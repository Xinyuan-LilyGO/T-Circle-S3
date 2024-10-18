#include "Arduino_GC9D01N.h"
#include "SPI.h"

Arduino_GC9D01N::Arduino_GC9D01N(
    Arduino_DataBus *bus, int8_t rst, uint8_t r,
    bool ips, int16_t w, int16_t h,
    uint8_t col_offset1, uint8_t row_offset1, uint8_t col_offset2, uint8_t row_offset2)
    : Arduino_TFT(bus, rst, r, ips, w, h, col_offset1, row_offset1, col_offset2, row_offset2)
{
}

bool Arduino_GC9D01N::begin(int32_t speed)
{
    _override_datamode = SPI_MODE0; // always use SPI_MODE0

    return Arduino_TFT::begin(speed);
}

void Arduino_GC9D01N::writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h)
{
    if ((x != _currentX) || (w != _currentW) || (y != _currentY) || (h != _currentH))
    {
        _bus->writeC8D16D16(GC9D01N_CASET, x + _xStart, x + w - 1 + _xStart);
        _bus->writeC8D16D16(GC9D01N_RASET, y + _yStart, y + h - 1 + _yStart);

        _currentX = x;
        _currentY = y;
        _currentW = w;
        _currentH = h;
    }

    _bus->writeCommand(GC9D01N_RAMWR); // write to RAM
}

/**************************************************************************/
/*!
    @brief   Set origin of (0,0) and orientation of TFT display
    @param   m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void Arduino_GC9D01N::setRotation(uint8_t r)
{
    Arduino_TFT::setRotation(r);
    switch (_rotation)
    {
    case 1:
        r = GC9D01N_MADCTL_MY | GC9D01N_MADCTL_MV;
        break;
    case 2:
        r = GC9D01N_MADCTL_MX | GC9D01N_MADCTL_MY;
        break;
    case 3:
        r = GC9D01N_MADCTL_MX | GC9D01N_MADCTL_MV;
        break;
    default: // case 0:
        r = GC9D01N_MADCTL_RGB;
        break;
    }
    _bus->beginWrite();
    _bus->writeCommand(GC9D01N_MADCTL);
    _bus->write(r);
    _bus->endWrite();
}

void Arduino_GC9D01N::invertDisplay(bool i)
{
    _bus->sendCommand(_ips ? (i ? GC9D01N_INVOFF : GC9D01N_INVON) : (i ? GC9D01N_INVON : GC9D01N_INVOFF));
}

void Arduino_GC9D01N::displayOn(void)
{
    _bus->sendCommand(GC9D01N_SLPOUT);
    delay(GC9D01N_SLPOUT_DELAY);
}

void Arduino_GC9D01N::displayOff(void)
{
    _bus->sendCommand(GC9D01N_SLPIN);
    delay(GC9D01N_SLPIN_DELAY);
}

void Arduino_GC9D01N::tftInit()
{
    if (_rst != GFX_NOT_DEFINED)
    {
        pinMode(_rst, OUTPUT);
        digitalWrite(_rst, HIGH);
        delay(200);
        digitalWrite(_rst, LOW);
        delay(GC9D01N_RST_DELAY);
        digitalWrite(_rst, HIGH);
        delay(GC9D01N_RST_DELAY);
    }
    else
    {
        // Software Rest
    }

    _bus->batchOperation(gc9d01n_init_operations, sizeof(gc9d01n_init_operations));

    invertDisplay(false);
}
