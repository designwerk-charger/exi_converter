#include "bitstream.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

BitStream::BitStream(uint8_t * input_data, size_t length_byte)
    : exi_data_(input_data, input_data+length_byte), bit_counter_(0), num_bits_(length_byte << 3) {
    if (length_byte < 1) {
        throw std::invalid_argument("Bitstream can not be initialized with size < 1.");
    }
}

BitStream::BitStream(const std::vector<uint8_t> & input_data)
        : exi_data_(input_data), bit_counter_(0),
        num_bits_(input_data.size() << 3) {
    if (input_data.size() < 1) {
        throw std::invalid_argument("Bitstream can not be initialized with size < 1.");
    }
}

BitStream::BitStream()
        : exi_data_(), bit_counter_(0),
          num_bits_(0) { }

// Todo: This function should be replaced by the get_max_4bytes
void BitStream::get_next_n_bits(uint32_t bitsrequested, uint8_t * data) {
    if (data == nullptr)
        throw std::invalid_argument("output data argument is NULL!");
    if (bitsrequested == 0)
        throw std::invalid_argument("requested number of bits can not be 0!");
    if (bitsrequested > num_bits_)
        throw std::range_error("Requested number of bits is not available!");

    uint32_t bits_taken = 0;
    while (bits_taken < bitsrequested) {
        uint32_t bits_to_take = std::min(bitsrequested-bits_taken, 32u);
        uint32_t d;
        switch ((bits_to_take-1)>>3) {
            case 0:
                data[bits_taken>>3] = get_max_4bytes(bits_to_take);
                break;
            case 1:
                reinterpret_cast<uint16_t *>(data)[bits_taken>>4] = __builtin_bswap16(get_max_4bytes(bits_to_take));
                break;
            case 2:
                d = get_max_4bytes(bits_to_take);
                data[(bits_taken>>5)+0] = reinterpret_cast<uint8_t *>(&d)[2];
                data[(bits_taken>>5)+1] = reinterpret_cast<uint8_t *>(&d)[1];
                data[(bits_taken>>5)+2] = reinterpret_cast<uint8_t *>(&d)[0];
                break;
            default:
                reinterpret_cast<uint32_t *>(data)[bits_taken>>5] = __builtin_bswap32(get_max_4bytes(bits_to_take));
                break;
        }
        bits_taken += bits_to_take;
    }

    #ifndef NDEBUG
        std::cout << "\tgetting " << std::dec << static_cast<int>(bitsrequested) << "bit(s) from position "
                  << bit_counter_ - bitsrequested << " -> 0x";
        for (uint32_t byte=0; byte <= (bitsrequested - 1 >> 3); byte++) {
            std::cout << std::setfill('0') << std::setw(2) << std::right << std::hex << static_cast<int>(data[byte]);
        }
        if (bitsrequested <= 8) {
            std::cout << std::dec << " (" << static_cast<int>(data[0]) << ")";
        }
        std::cout << "↴" << std::endl;
    #endif
}

uint32_t BitStream::get_max_4bytes(uint8_t bitsrequested) {
    uint32_t address_32bit = bit_counter_ >> 5;
    uint32_t bit_available_for_takeout = 32 - (bit_counter_ - (address_32bit << 5));

    uint32_t value1 = __builtin_bswap32(reinterpret_cast<uint32_t *>(exi_data_.data())[address_32bit]);

    if (bitsrequested <= bit_available_for_takeout) {
        // single word access is sufficient
        bit_counter_ += bitsrequested;
        uint32_t shift = bit_available_for_takeout-bitsrequested;
        uint32_t bitmask = (1 << bitsrequested) - 1;
        if (bitsrequested == 32) {
            bitmask = 0xFFFFFFFF;
        }
        return (value1 >> shift) & bitmask;
    } else {
        uint32_t value2 = __builtin_bswap32(reinterpret_cast<uint32_t *>(exi_data_.data())[address_32bit+1]);
        uint32_t remaining_bits = bitsrequested - bit_available_for_takeout;
        value1 = value1 & ((1 << bit_available_for_takeout) - 1);
        value2 = (value2 >> (32-remaining_bits)) & ((1 << remaining_bits) - 1);
        bit_counter_ += bitsrequested;
        return (value1 << remaining_bits) + value2;
    }
}

void BitStream::add_max_8bits(uint8_t data, uint8_t num_bits) {
    if (num_bits == 0)
        throw std::invalid_argument("Added number of bits can not be 0!");
    if (num_bits > 8)
        throw std::range_error("Number of bits must be < 8");

    uint8_t tmp_byte;
    uint8_t addr_exi_byte;
    uint8_t bit_pos_in_byte;
    uint8_t bits_left_in_byte;

    addr_exi_byte = bit_counter_ >> 3;
    bit_pos_in_byte = bit_counter_ - (addr_exi_byte << 3);
    bits_left_in_byte = 8 - bit_pos_in_byte;

    if (bits_left_in_byte < num_bits) {
        tmp_byte = data >> bit_pos_in_byte;
        exi_data_[addr_exi_byte] = exi_data_[addr_exi_byte] | tmp_byte;
        tmp_byte = data << (8 - bit_pos_in_byte);
        exi_data_.push_back(tmp_byte);
    } else {
        tmp_byte = data << (8 - num_bits - bit_pos_in_byte);
        if (bit_pos_in_byte == 0) {
            exi_data_.push_back(0x00);
        }
        tmp_byte = tmp_byte | exi_data_.at(addr_exi_byte);
        exi_data_.at(addr_exi_byte) = tmp_byte;
    }
    bit_counter_ += num_bits;

    #ifndef NDEBUG
        std::cout << "\tsetting " << std::dec << static_cast<int>(num_bits) << "bit(s) from position "
                  << bit_counter_ - num_bits << " -> 0x";
        std::cout << std::endl;
    #endif
}

void BitStream::add_bytes(const uint8_t * data, uint8_t num_bytes) {
    for (uint32_t byte=0; byte < (num_bytes); byte++) {
        add_max_8bits(data[byte], 8);
    }
}

std::vector<uint8_t> BitStream::get_exi_data() {
    return exi_data_;
}
