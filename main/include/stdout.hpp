#ifndef UART_STREAMBUF_HPP
#define UART_STREAMBUF_HPP

#include <streambuf>
#include <ostream>
#include "usart.h"  // Pour accéder à huartx

class UARTStreamBuf : public std::streambuf {
protected:
    virtual int_type overflow(int_type c) override {
        if (c != EOF) {
            uint8_t ch = c;
            HAL_UART_Transmit(&huart3, &ch, 1, HAL_MAX_DELAY); // Change huart3 selon ton UART
        }
        return c;
    }
};

#endif // UART_STREAMBUF_HPP
