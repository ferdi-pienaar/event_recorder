/*
 *
 */
#pragma once

#include <array>
#include <time.h> // struct timespec

constexpr unsigned NUM_ENTRIES = 3;
constexpr unsigned NUM_STAMPS_PER_ENTRY = 2;

using Double_stamp = std::array<timespec, NUM_STAMPS_PER_ENTRY>;
