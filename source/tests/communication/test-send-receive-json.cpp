#include "communication/sender.hpp"
#include "communication/receiver.hpp"

#include "communication/json/streamificator.hpp"
#include "communication/json/identificator.hpp"
#include "communication/json/messages-core.hpp"

#include "gtest/gtest.h"

class SendReceive : public ::testing::Test
{
protected:
    void SetUp() override {
        status_request_property = std::make_shared<Property<StatusRequest>>();
        streamificator = std::make_shared<StreamificatorJSON>();
        destreamificator = std::make_shared<DestreamificatorJSON>();
        identificator = std::make_shared<IdentificatorJSON>();

        sender = std::make_shared<MessageTransmitter>(streamificator, identificator);
        sender->add_serializer(std::make_shared<StatusRequestSerializer>());

        receiver = std::make_shared<MessageListener>(destreamificator, identificator);
        receiver->add(std::make_shared<StatusRequestSerializer>());
        receiver->add(status_request_property);

    }

    std::shared_ptr<IStreamificator> streamificator;
    std::shared_ptr<IDestreamificator> destreamificator;
    std::shared_ptr<IMessageIdentificator> identificator;

    std::shared_ptr<MessageListener> receiver;
    std::shared_ptr<MessageTransmitter> sender;

    std::shared_ptr<Property<StatusRequest>> status_request_property;

    RingBuffer ring_buffer{100};
};

TEST_F(SendReceive, SingleMessageCore)
{
    StatusRequest r;
    r.do_blink = true;

    (*status_request_property)->do_blink = true;

    ASSERT_TRUE(sender->put_message(ring_buffer, r));

    ASSERT_TRUE(receiver->receive_one(ring_buffer));

    ASSERT_EQ((*status_request_property)->do_blink, r.do_blink);
}
