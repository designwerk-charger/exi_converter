#pragma once

#include "base/input_string_stream.h"
#include "gmock/gmock.h"

class MockInputStringStream : public InputStringStream {
public:
    MockInputStringStream() : InputStringStream() {}  // NOLINT

    // std::string get_item_and_move_to_next();
    MOCK_METHOD(std::string , get_item_and_move_to_next, (), (override));
};
