#include "wideusb/communication/utils/time-group-maker.hpp"

#include "gtest/gtest.h"

TEST(TimePlannerTest, Operating)
{
    {
        auto tp = std::chrono::steady_clock::now();
        TimePlanner<int> p;
        TimePlanner<int>::Task t(1, 10, tp, TimePlanningOptions(10ms, 5ms));
        p.add(t);
        const auto batch = p.get_batch(tp);
        ASSERT_EQ(batch.tasks.size(), 1);
        ASSERT_EQ(p.get_batch(tp).tasks.size(), 0);
    }
    {
        auto tp = std::chrono::steady_clock::now();
        // Timeout test
        TimePlanner<int> p;
        TimePlanner<int>::Task t(1, 10, tp, TimePlanningOptions(10ms, 5ms, 1000, 100ms));
        p.add(t);
        const auto batch = p.get_batch(tp + 101ms);
        ASSERT_FALSE(p.has_task(10));
        ASSERT_EQ(batch.tasks.size(), 0);
    }
    {
        auto tp = std::chrono::steady_clock::now();
        TimePlanner<int> p;
        p.add(TimePlanner<int>::Task(1, 10, tp, TimePlanningOptions(10ms, 5ms)));
        p.add(TimePlanner<int>::Task(2, 20, tp + 1ms, TimePlanningOptions(10ms, 5ms)));
        p.add(TimePlanner<int>::Task(3, 30, tp + 2ms, TimePlanningOptions(10ms, 5ms)));
        p.add(TimePlanner<int>::Task(4, 40, tp + 2ms, TimePlanningOptions(10ms, 5ms)));

        ASSERT_EQ(p.get_batch(tp + 20ms).tasks.size(), 4);
        ASSERT_EQ(p.get_batch(tp + 20ms).tasks.size(), 0);
    }
    {
        auto tp = std::chrono::steady_clock::now();
        TimePlanner<int> p;
        p.add(TimePlanner<int>::Task(1, 10, tp, TimePlanningOptions(10ms, 5ms)));
        p.add(TimePlanner<int>::Task(2, 20, tp + 1ms, TimePlanningOptions(10ms, 5ms)));
        p.add(TimePlanner<int>::Task(3, 30, tp + 2ms, TimePlanningOptions(10ms, 5ms)));
        p.add(TimePlanner<int>::Task(4, 40, tp + 2ms, TimePlanningOptions(10ms, 5ms)));

        ASSERT_EQ(p.get_batch(tp + 3ms).tasks.size(), 4);
        ASSERT_EQ(p.get_batch(tp + 3ms).tasks.size(), 0);
        ASSERT_EQ(p.get_batch(tp + 3ms).tasks.size(), 0);

        ASSERT_EQ(p.get_batch(tp + 11ms).tasks.size(), 2);
        ASSERT_EQ(p.get_batch(tp + 11ms).tasks.size(), 0);

        ASSERT_EQ(p.get_batch(tp + 13ms).tasks.size(), 2);
        ASSERT_EQ(p.get_batch(tp + 13ms).tasks.size(), 0);
    }
    {
        auto tp = std::chrono::steady_clock::now();
        TimePlanner<int> p;
        p.add(TimePlanner<int>::Task(1, 10, tp, TimePlanningOptions(10ms, 5ms)));
        p.add(TimePlanner<int>::Task(2, 20, tp + 1ms, TimePlanningOptions(10ms, 5ms)));
        p.add(TimePlanner<int>::Task(3, 30, tp + 2ms, TimePlanningOptions(10ms, 5ms)));
        p.add(TimePlanner<int>::Task(4, 40, tp + 2ms, TimePlanningOptions(10ms, 5ms, 1))); // max cycles = 1

        ASSERT_EQ(p.get_batch(tp + 3ms).tasks.size(), 4);
        ASSERT_EQ(p.get_batch(tp + 3ms).tasks.size(), 0);
        ASSERT_EQ(p.get_batch(tp + 3ms).tasks.size(), 0);

        ASSERT_EQ(p.get_batch(tp + 11ms).tasks.size(), 2);
        ASSERT_EQ(p.get_batch(tp + 11ms).tasks.size(), 0);

        ASSERT_EQ(p.get_batch(tp + 13ms).tasks.size(), 1); // affects this
        ASSERT_EQ(p.get_batch(tp + 13ms).tasks.size(), 0);
    }
    {
        auto tp = std::chrono::steady_clock::now();
        TimePlanner<int> p;
        p.add(TimePlanner<int>::Task(1, 10, tp, TimePlanningOptions(10ms, 5ms)));
        p.add(TimePlanner<int>::Task(2, 20, tp + 1ms, TimePlanningOptions(10ms, 5ms)));
        p.add(TimePlanner<int>::Task(3, 30, tp + 2ms, TimePlanningOptions(10ms, 5ms)));
        p.add(TimePlanner<int>::Task(4, 40, tp + 2ms, TimePlanningOptions(10ms, 5ms)));

        ASSERT_EQ(p.get_batch(tp + 3ms, 2).tasks.size(), 2);
        ASSERT_EQ(p.get_batch(tp + 3ms, 2).tasks.size(), 2);
        ASSERT_EQ(p.get_batch(tp + 3ms).tasks.size(), 0);
        ASSERT_EQ(p.get_batch(tp + 3ms).tasks.size(), 0);

        ASSERT_EQ(p.get_batch(tp + 11ms, 1).tasks.size(), 1);
        ASSERT_EQ(p.get_batch(tp + 11ms, 1).tasks.size(), 1);
        ASSERT_EQ(p.get_batch(tp + 11ms).tasks.size(), 0);

        ASSERT_EQ(p.get_batch(tp + 13ms, 1).tasks.size(), 1);
        ASSERT_EQ(p.get_batch(tp + 13ms, 1).tasks.size(), 1);
        ASSERT_EQ(p.get_batch(tp + 13ms).tasks.size(), 0);
    }
    {
        auto tp = std::chrono::steady_clock::now();
        TimePlanner<int> p;
        p.add(TimePlanner<int>::Task(1, 10, tp, TimePlanningOptions(10ms, 5ms)));
        p.add(TimePlanner<int>::Task(2, 20, tp + 1ms, TimePlanningOptions(10ms, 5ms)));
        p.add(TimePlanner<int>::Task(3, 30, tp + 2ms, TimePlanningOptions(10ms, 5ms)));
        p.add(TimePlanner<int>::Task(4, 40, tp + 2ms, TimePlanningOptions(10ms, 5ms)));

        ASSERT_EQ(p.get_batch(tp + 3ms).tasks.size(), 4);
        ASSERT_EQ(p.get_batch(tp + 3ms).tasks.size(), 0);
        ASSERT_EQ(p.get_batch(tp + 3ms).tasks.size(), 0);

        p.remove(20);

        ASSERT_EQ(p.get_batch(tp + 11ms).tasks.size(), 1);
        ASSERT_EQ(p.get_batch(tp + 11ms).tasks.size(), 0);

        ASSERT_EQ(p.get_batch(tp + 13ms).tasks.size(), 2);
        ASSERT_EQ(p.get_batch(tp + 13ms).tasks.size(), 0);
    }

}

/*
TEST(GrouperTest, Operating)
{
    {
        TimeGrouper<int> grouper;

        grouper.add_object(TimeGrouper<int>::DurationalObject(10, 20, 1));
        grouper.add_object(TimeGrouper<int>::DurationalObject(12, 19, 2));
        grouper.add_object(TimeGrouper<int>::DurationalObject(14, 25, 3));
        grouper.add_object(TimeGrouper<int>::DurationalObject(21, 30, 4));
        grouper.add_object(TimeGrouper<int>::DurationalObject(21, 22, 5));


        TimeGrouper<int>::TimeSlot slot1 = grouper.next_slot();
        TimeGrouper<int>::TimeSlot slot2 = grouper.next_slot();
        TimeGrouper<int>::TimeSlot slot3 = grouper.next_slot();

        ASSERT_EQ(slot1.objects.size(), 3);
        ASSERT_EQ(slot2.objects.size(), 2);
        ASSERT_EQ(slot3.objects.size(), 0);
    }
    {
        TimeGrouper<int> grouper;

        grouper.add_object(TimeGrouper<int>::DurationalObject(10, 20, 1));
        grouper.add_object(TimeGrouper<int>::DurationalObject(12, 19, 2));
        grouper.add_object(TimeGrouper<int>::DurationalObject(14, 25, 3));
        grouper.add_object(TimeGrouper<int>::DurationalObject(21, 30, 4));
        grouper.add_object(TimeGrouper<int>::DurationalObject(21, 22, 5));


        TimeGrouper<int>::TimeSlot slot1 = grouper.next_slot(2);
        TimeGrouper<int>::TimeSlot slot2 = grouper.next_slot(2);
        TimeGrouper<int>::TimeSlot slot3 = grouper.next_slot(2);

        ASSERT_EQ(slot1.objects.size(), 2);
        ASSERT_EQ(slot2.objects.size(), 2);
        ASSERT_EQ(slot3.objects.size(), 1);
    }
}
*/
