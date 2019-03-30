#include "timestamp.hpp"

#include <chrono>

using namespace pebo;

const uint32_t TimeStamp::Epoch;

timestamp_t TimeStamp::now()
{
    auto now = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::seconds>(now).count() - Epoch;
}

uint32_t TimeStamp::age(timestamp_t time_in)
{
    return (uint32_t)(now() - time_in);
}

