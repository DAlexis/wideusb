#include "gtest/gtest.h"

#include "message-receive.hpp"
#include "message-send.hpp"

#include "ring-buffer.h"

#include "json/header-json.hpp"
#include "json/msg-core-json.hpp"
/*
TEST(SendReceiveTest, SingleMessage)
{
    MessageReceiver receiver(std::make_shared<HeaderDeserializerJSON>());

    StatusRequest status_request;
    StatusResponse status_response;

    receiver.add_message_parser(
                "core.status_request",
                std::make_shared<JSONDeserializer<StatusRequest>>(
                    [&status_request](const StatusRequest& val) { status_request = val; }
                    )
                );

    receiver.add_message_parser(
                "core.status_response",
                std::make_shared<JSONDeserializer<StatusResponse>>(
                    [&status_response](const StatusResponse& val) { status_response = val; }
                    )
                );

    DataSender sender(std::make_shared<HeaderSerializerJSON>());
    StatusRequest status_request_to_send;
    status_request_to_send.do_blink = true;

    StatusResponse status_response_to_send;
    status_response_to_send.alive = true;
    status_response_to_send.free_mem = 123;
    status_response_to_send.system_ticks = 321;

    sender.push("core.status_request", JSONSerializer<StatusRequest>(), &status_request_to_send);
    sender.push("core.status_response", JSONSerializer<StatusResponse>(), &status_response_to_send);

    RingBuffer ring_buffer;
    std::vector<uint8_t> contents(200);
    ring_buffer_init(&ring_buffer, contents.data(), contents.size());

    while (!sender.empty())
    {
        *sender.next_message() >> ring_buffer;
    }

    while (ring_buffer_data_size(&ring_buffer) != 0)
    {
        ASSERT_NO_THROW(receiver.try_receive(ring_buffer));
    }

    ASSERT_EQ(status_request.do_blink, status_request_to_send.do_blink);
    ASSERT_EQ(status_response.alive, status_response_to_send.alive);
    ASSERT_EQ(status_response.free_mem, status_response_to_send.free_mem);
    ASSERT_EQ(status_response.system_ticks, status_response_to_send.system_ticks);

    // Creating message

}
*/
