#include "communication/json/identificator.hpp"

#include "gtest/gtest.h"


TEST(IdentificationTest, CorrectBuffer)
{
    IdentificatorJSON id;

    PBuffer buf = Buffer::create(100);

    uint8_t stub[100];

    id.put_message_id(*buf, MessageHeader(123, 0));
    buf->put(stub, sizeof(stub));
    id.put_message_id(*buf, MessageHeader(456, 1));

    BufferAccessor acc(buf);
    auto res1 = id.get_message_id(acc);
    ASSERT_TRUE(res1.has_value());
    ASSERT_EQ(res1->id, 123);

    acc.extract(stub, sizeof(stub));
    auto res2 = id.get_message_id(acc);
    ASSERT_TRUE(res2.has_value());
    ASSERT_EQ(res2->id, 456);

    auto res3 = id.get_message_id(acc);
    ASSERT_FALSE(res3.has_value());
}

