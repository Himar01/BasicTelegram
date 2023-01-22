/*
    BasicTelegram.h - Library for creating Basic Telegram API requests
    Created by Himar Moreno Santana, January 15, 2023.
    Released into the public domain.
*/
#ifndef BasicTelegram_h
#define BasicTelegram_h

#include "WiFiClientSecure.h"
#include "time.h"
#include "string"

class TelegramClient
{
    public:
        void setup(char* token);
        bool sendMessage( int chatId, char* message);
    private:
        char* _token;
};

#endif