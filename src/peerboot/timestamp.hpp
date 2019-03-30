#pragma once

#include "../include/peerboot.hpp"

namespace pebo
{
    /**
     * Timestamp handling helper class.
     */
    class TimeStamp
    {
    public:
        // Epoch for peerboot timestamps, Jan 1 2019 UTC
        static const uint32_t Epoch = 1546300800;
        // Get current time
        static timestamp_t now();
        // Get age of given time
        static uint32_t age(timestamp_t time_in);
    };
}