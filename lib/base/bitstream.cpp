#include "bitstream.h"
#include <iostream>
#include <algorithm>


// TODO(LLR): Implement and Test
BitStream::BitStream(uint8_t * input_data, size_t length)
    : exi_data_(input_data), length_(length), bit_counter_(0), num_bits_(length << 3) {
    if (length < 1) {
        throw std::invalid_argument("Bitstream can not be initialized with size < 1.");
    }
}

void BitStream::get_next_n_bits(uint8_t bits, uint8_t * data) {
    if (data == nullptr)
        throw std::invalid_argument("output data argument is NULL!");
    if (bits == 0)
        throw std::invalid_argument("requested number of bits can not be 0!");
    if (bits > num_bits_)
        throw std::range_error("Requested number of bits is not available!");

    uint32_t bit_counter_end = bit_counter_ + bits;
    uint32_t bits_taken = 0;
    while (bit_counter_ != bit_counter_end) {
        uint32_t byte_address = bit_counter_ >> 3;
        uint8_t bits_to_take = std::min(static_cast<uint8_t>(8 - (bit_counter_ - (byte_address << 3))), bits);
        uint8_t partial_byte = exi_data_[byte_address] >> (8 - bits_to_take) & ((1 << bits_to_take) - 1);
        data[bits_taken >> 3] = partial_byte;
        bit_counter_ += bits_to_take;
        bits_taken += bits_to_take;
    }
    return;

    uint8_t addr_offset;
    uint32_t subset;
    uint32_t value;
    addr_offset = bit_counter_ >> 5;  // divide by 32
    subset = exi_data_[addr_offset];
    value = subset >> (32 - bits - (bit_counter_ - (addr_offset << 3)));
    value = ((1 << bits) - 1) & value;
}

void add_n_bits(uint8_t bits, uint8_t * data) {}
