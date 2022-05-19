#include "bitstream.h"
#include <iostream>
#include <iomanip>
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
    uint8_t bits_taken = 0;
    uint8_t bits_placed = 0;
    while (bit_counter_ != bit_counter_end) {
        uint32_t byte_address = bit_counter_ >> 3;
        uint8_t position_within_byte = bit_counter_ - (byte_address << 3);
        uint8_t bits_to_take = std::min(static_cast<uint8_t>(8 - position_within_byte),
                                        static_cast<uint8_t>(bits - bits_taken));
        uint8_t target_byte = exi_data_[byte_address];
        uint8_t target_byte_shifted = (target_byte >> (8 - position_within_byte - bits_to_take));
        uint8_t bit_mask = (1 << bits_to_take) - 1;
        uint8_t partial_byte = target_byte_shifted & bit_mask;

        // putting the bits to output data
        uint8_t target_byte_address = bits_placed >> 3;
        uint8_t bits_left_in_byte = 8-(bits_placed - (target_byte_address << 3));

        if (bits_to_take > bits_left_in_byte) {
            data[target_byte_address] = (data[target_byte_address] << bits_left_in_byte) +
                    (partial_byte >> bits_left_in_byte);
            bit_mask = (1 << (bits_to_take-bits_left_in_byte)) - 1;
            data[target_byte_address+1] = partial_byte & bit_mask;
        } else {
            if (bits_left_in_byte != 8) {
                data[target_byte_address] = (data[target_byte_address] << bits_left_in_byte) +
                        partial_byte;
            } else {
                data[target_byte_address] = partial_byte;
            }
        }

        bits_placed += bits_to_take;
        bit_counter_ += bits_to_take;
        bits_taken += bits_to_take;
    }

    #ifndef NDEBUG
        std::cout << "getting " << std::dec << static_cast<int>(bits) << "bit(s) from position "
        << bit_counter_ - bits << " -> 0x";
        for (uint32_t byte=0; byte <= (bits-1 >> 3); byte++) {
            std::cout << std::setfill('0') << std::setw(2) << std::right << std::hex << static_cast<int>(data[byte]);
        }
        std::cout << std::endl;
    #endif
}

void add_n_bits(uint8_t bits, uint8_t * data) {}
