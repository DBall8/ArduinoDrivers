/**
 * New Haven Display module driver, with 2 rows with 8 characters each
 * See documentionation here: http://www.newhavendisplay.com/specs/NHD-0208AZ-FL-YBW.pdf
 */
#ifndef DIPS082LCD_HPP
#define DIPS082LCD_HPP

#include "drivers/lcd/ILcd.hpp"
#include "drivers/dio/IDio.hpp"

namespace Lcd
{
    class Dips082Lcd : public ILcd
    {
        public:
            /**
             * Constructir for DIPS082 LCD driver
             * @param   pRsPin          Register select pin, 0 = command, 1 = data
             * @param   pRwPin          Read/write select pin, 0 = write, 1 = read
             * @param   pOePin          Operation enable pin, starts an operation on the fallin edge
             * @param   dataPinArray    Array of pins for data bus
             * @param   numDataPins     Number of pins in pin array, suggested to be 4 or 8
             */ 
            Dips082Lcd(dio::IDio* pRsPin,
                    dio::IDio* pRwPin, 
                    dio::IDio* pOePin, 
                    dio::IDio** dataPinArray,
                    uint8_t numDataPins);
            ~Dips082Lcd();

            /**
             * Initializes LCD to default state, clear screen with cursor at home
             * @return Successful
             */
            bool initialize() override;

            /**
             * Set's the location on the LCD of the cursor, 0 indexed
             * @param   row     Row to move cursor to, 0=top, 1=bottom
             * @param   column  Column to move cursor to, 0 indexed
             * @return successfull
             */
            bool setCursor(uint8_t row, uint8_t column) override;

            /**
             * Display text on the LCD. Starts at the cursor's location and
             * will loop around to the other row when the end of a row is reached
             * @param   string      String to display on the LCD
             * @param   strLength   Length of the string to display
             * @return successfull
             */
            bool display(const char* string, uint8_t strLength) override;

            /**
             * Display a character on the LCD at the cursor's location
             * @param   characterCode   Code of character to display, see https://www.lcd-module.de/eng/pdf/doma/dips082e.pdf
             * @return successfull
             */
            bool display(uint8_t characterCode) override;

            /**
             * Clear the display, whole display will be blank
             * @return successfull
             */
            bool clear() override;

        private:
            dio::IDio* pRsPin_;          // Register select pin, 0 = command, 1 = data
            dio::IDio* pRwPin_;          // Read/write select pin, 0 = write, 1 = read
            dio::IDio* pOePin_;          // Operation enable pin, starts an operation on the fallin edge
            dio::IDio** dataPinArray_;   // Array of pins used to send data
            uint8_t numDataPins_;        // Number of data pins, should be 4 or 8 typically

            uint8_t cursorCol_; // Cursor's current column, 0 indexed
            uint8_t cursorRow_; // Cursor's current row, 0 indexed

            /**
             * Send a command to the LCD
             * @param   command Command code to send
             * @return successfull
             */
            bool sendCommand(uint8_t command);

            /**
             * Send data to the LCD, either to DDRAM or CGRAM depending on which address was set last
             * @param   data    Data to send
             * @return successfull
             */
            bool sendData(uint8_t data);

            /**
             * Send message to LCD, uses either 4 or 8 bit mode depending on number of data pins
             * @param   data        Byte to send
             * @param   isCommand   If true, send as a command
             * @return successfull
             */
            bool send(uint8_t data, bool isCommand);

            /**
             * Place data on the data bits, and pulse the operation enable pin to send it
             * @param   data    Data to send
             */
            void pulseData(uint8_t data);

            /**
             * Sets the data pins to represent the value of the given byte
             * @param   data    Value to set the data pins to
             */
            void setDataBits(uint8_t data);

            /**
             * Run a wakeup sequence specified by https://www.newhavendisplay.com/specs/NHD-0208AZ-FL-YBW.pdf
             */
            void wakeup();
    };
}
#endif