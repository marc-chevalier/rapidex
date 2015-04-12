#pragma once
#include <string>

class MessageException : public std::exception
{
public:
    MessageException(std::string message) noexcept;
    ~MessageException() noexcept;
    std::string getMessage() const;
private:
    std::string msg;
};

class ParseError : public MessageException
{
public:
    ParseError(std::string message) noexcept : MessageException(message) {};
};
