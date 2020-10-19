#include "Dips082Lcd.hpp"
#include "drivers/timer/Delay.hpp"
#include "utilities/print/Print.hpp"

using namespace Dio;

namespace Lcd
{
    // LCD dimensions
    const static uint8_t DISP_WIDTH_CHAR = 8;
    const static uint8_t DISP_HEIGHT_CHAR = 2;
    const static uint8_t NUM_DISP_CHARS = DISP_WIDTH_CHAR * DISP_HEIGHT_CHAR;

    // Commands for display setup
    const static uint8_t SETUP_8BIT_COMMAND = 0x38; // Sets LCD to 8bit control, 2 lines, 5/8 characters
    const static uint8_t SETUP_4BIT_COMMAND = 0x28; // Sets LCD to 4bit control, 2 lines, 5/8 characters


    // Display on/off commands
    const static uint8_t DISPLAY_OFF = 0x08;             // Display off
    const static uint8_t DISPLAY_ON = 0x0c;              // Display on, cursor off
    const static uint8_t DISPLAY_ON_CURSOR_ON = 0x0e;    // Display on, cursor on
    const static uint8_t DISPLAY_ON_CURSOR_BLINK = 0x0f; // Display on, cursor blinking

    // Clear display
    const static uint8_t CLEAR_DISPLAY = 0x1;

    // Set cursor/display movement on write
    const static uint8_t CURSOR_MOVES_LEFT = 0x04;
    const static uint8_t CURSOR_MOVES_RIGHT = 0x06;
    const static uint8_t DISPLAY_MOVES_LEFT = 0x05;
    const static uint8_t DISPLAY_MOVES_RIGHT = 0x07;

    // Move cursor display without writing
    const static uint8_t MOVE_LEFT = 0x10;
    const static uint8_t MOVE_RIGHT = 0x14;
    const static uint8_t SHIFT_LEFT = 0x18;
    const static uint8_t SHIFT_RIGHT = 0x1c;

    // Cursor position commands
    const static uint8_t RETURN_HOME = 0x02;        // Return cursor to home position
    const static uint8_t DDRAM_ADDRESS_MASK = 0x80; // Mask to add to DDRAM address
                                                    // for sending DDRAM address command

    const static uint8_t CGRAM_ADDRESS_MASK = 0x60; // Mask to add to CGRAM address
                                                    // for sending CGRAM address command

    const static uint8_t SECOND_ROW_ADDRESS = 0x40;

    // Commands for LCD wakeup
    const static uint8_t FOUR_BIT_WAKEUP = 0x03;
    const static uint8_t FOUR_BIT_INITIALIZATION = 0x02;
    const static uint8_t EIGHT_BIT_INITIALIZATION = 0x30;

    Dips082Lcd::Dips082Lcd(Dio::IDio* pRsPin,
                           Dio::IDio* pRwPin, 
                           Dio::IDio* pOePin, 
                           Dio::IDio** dataPinArray,
                           uint8_t numDataPins) :
        pRsPin_(pRsPin),
        pRwPin_(pRwPin),
        pOePin_(pOePin),
        dataPinArray_(dataPinArray),
        numDataPins_(numDataPins),
        cursorCol_(0),
        cursorRow_(0)
    {
    }
    
    Dips082Lcd::~Dips082Lcd()
    {

    }

    bool Dips082Lcd::setCursor(uint8_t row, uint8_t column)
    {
        if ((column >= DISP_WIDTH_CHAR) ||
            (row >= DISP_HEIGHT_CHAR))
        {
            // Out of range
            return false;
        }

        // Store new cursor location
        cursorCol_ = column;
        cursorRow_ = row;

        // Calculate DDRAM address
        uint8_t address = column + (row * SECOND_ROW_ADDRESS);

        // Set DDRAM address
        bool success = sendCommand(DDRAM_ADDRESS_MASK | address);
        DELAY_MICROSECONDS(50);

        return success;
    }

    bool Dips082Lcd::display(const char* string, uint8_t strLength)
    {
        bool success = true;
        for (uint8_t i=0; i<strLength; i++)
        {
            // Send each character
            success &= sendData(string[i]);
            DELAY_MICROSECONDS(50);

            // Increment cursor position
            cursorCol_++;

            // Loop around to other row if gone past end of row
            if (cursorCol_ >= DISP_WIDTH_CHAR)
            {
                uint8_t newRow = (cursorRow_ == 1) ? 0 : 1;
                success &= setCursor(newRow, 0);
            }
        }

        return success;
    }

    bool Dips082Lcd::display(uint8_t characterCode)
    {
        // Send character
        bool success = sendData(characterCode);
        DELAY_MICROSECONDS(50);

        // Increment cursor position, and loop around if needed
        cursorCol_++;
        if (cursorCol_ >= DISP_WIDTH_CHAR)
        {
            uint8_t newRow = (cursorRow_ == 1) ? 0 : 1;
            success &= setCursor(newRow, 0);
        }

        return success;
    }

    bool Dips082Lcd::clear()
    {
        // Send clear screen command
        bool success = sendCommand(CLEAR_DISPLAY);
        DELAY(2);
        return success;
    }

    bool Dips082Lcd::sendCommand(uint8_t command)
    {
        return send(command, true);
    }

    bool Dips082Lcd::sendData(uint8_t data)
    {
        return send(data, false);
    }

    bool Dips082Lcd::send(uint8_t data, bool isCommand)
    {
        // Set register select pin LOW if command, HIGH if data
        Level rsPinLevel = (isCommand) ? Level::L_LOW : Level::L_HIGH;

        pRsPin_->set(rsPinLevel);    // Select data or command
        pRwPin_->set(Level::L_LOW);  // Select write

        if (numDataPins_ <= 4)
        {
            // 4 bit mode, pulse each half data byte 

            // High order bits first
            pulseData(data >> 4);
            DELAY(1);
        }

        // Send low order bits for 4 bit mode, or full data for 8 bit mode
        pulseData(data);
        return true;
    }

    void Dips082Lcd::pulseData(uint8_t data)
    {
        // Set the data to pulse
        setDataBits(data);

        // Pulse data pin high for at least 300ns
        pOePin_->set(Level::L_HIGH);
        DELAY(1);
        pOePin_->set(Level::L_LOW);
    }

    void Dips082Lcd::setDataBits(uint8_t data)
    {
        for (uint8_t i=0; i<numDataPins_; i++)
        {
            // Set each pin to the value of the corresponing bit
            IDio* pPin = dataPinArray_[i];
            Level pinLevel = (data & (1 << i)) ? L_HIGH : L_LOW;
            pPin->set(pinLevel);
        }
    }

    void Dips082Lcd::wakeup()
    {
        if (numDataPins_ <= 4)
        {
            setDataBits(FOUR_BIT_WAKEUP);
            DELAY(5);

            pRsPin_->set(Level::L_LOW);  // Select command
            pRwPin_->set(Level::L_LOW);  // Select write

            pulseData(FOUR_BIT_WAKEUP);
            DELAY(1);
            pulseData(FOUR_BIT_WAKEUP);
            DELAY(1);
            pulseData(FOUR_BIT_WAKEUP);
            DELAY(1);
            pulseData(FOUR_BIT_INITIALIZATION);
            DELAY(1);
        }
        else
        {
            sendCommand(EIGHT_BIT_INITIALIZATION);
            DELAY(10);
            sendCommand(EIGHT_BIT_INITIALIZATION);
            DELAY(1);
            sendCommand(EIGHT_BIT_INITIALIZATION);
            DELAY(1);
        }
    }

    bool Dips082Lcd::initialize()
    {
        // Wait at least 40ms after power up
        DELAY(100);

        // Wakeup procedure as suggested
        wakeup();

        // Send commands to get to initial state, delay 1ms after each to allow each to execute
        uint8_t setupCommand = (numDataPins_ > 4) ? SETUP_8BIT_COMMAND : SETUP_4BIT_COMMAND;
        sendCommand(setupCommand); // Set correcting LCD settings (byte length, num rows, character size)
        DELAY(1);
        sendCommand(DISPLAY_ON);
        DELAY(1);
        sendCommand(CURSOR_MOVES_RIGHT);
        DELAY(1);
        sendCommand(RETURN_HOME);
        DELAY(2);
        clear();

        return true;
    }
}