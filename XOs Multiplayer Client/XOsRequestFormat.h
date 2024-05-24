#pragma once
#define HEADER_SIZE 3
#define JOIN_FAIL 0
#define USERNAME_LENGTH 15

enum XOsRequestType : char {
    JOIN,
    LIST,
    CHALLENGE,
    CHALLENGERS,
    MOVE,
    DISCONNECT
};

