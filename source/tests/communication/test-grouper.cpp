#include "communication/utils/time-group-maker.hpp"

#include "gtest/gtest.h"

TEST(TimePlannerTest, Operating)
{
    {
        TimePlanner<int> p;
        TimePlanner<int>::Task t(1, 10, 0, TimePlanningOptions(10, 5));
        p.add(t);
        const auto batch = p.get_batch(0);
        ASSERT_EQ(batch.tasks.size(), 1);
        ASSERT_EQ(p.get_batch(0).tasks.size(), 0);
    }
    {
        // Timeout test
        TimePlanner<int> p;
        TimePlanner<int>::Task t(1, 10, 0, TimePlanningOptions(10, 5, 1000, 100));
        p.add(t);
        const auto batch = p.get_batch(101);
        ASSERT_FALSE(p.has_task(10));
        ASSERT_EQ(batch.tasks.size(), 0);
    }
    {
        TimePlanner<int> p;
        p.add(TimePlanner<int>::Task(1, 10, 0, TimePlanningOptions(10, 5)));
        p.add(TimePlanner<int>::Task(2, 20, 1, TimePlanningOptions(10, 5)));
        p.add(TimePlanner<int>::Task(3, 30, 2, TimePlanningOptions(10, 5)));
        p.add(TimePlanner<int>::Task(4, 40, 2, TimePlanningOptions(10, 5)));

        ASSERT_EQ(p.get_batch(20).tasks.size(), 4);
        ASSERT_EQ(p.get_batch(20).tasks.size(), 0);
    }
    {
        TimePlanner<int> p;
        p.add(TimePlanner<int>::Task(1, 10, 0, TimePlanningOptions(10, 5)));
        p.add(TimePlanner<int>::Task(2, 20, 1, TimePlanningOptions(10, 5)));
        p.add(TimePlanner<int>::Task(3, 30, 2, TimePlanningOptions(10, 5)));
        p.add(TimePlanner<int>::Task(4, 40, 2, TimePlanningOptions(10, 5)));

        ASSERT_EQ(p.get_batch(3).tasks.size(), 4);
        ASSERT_EQ(p.get_batch(3).tasks.size(), 0);
        ASSERT_EQ(p.get_batch(3).tasks.size(), 0);

        ASSERT_EQ(p.get_batch(11).tasks.size(), 2);
        ASSERT_EQ(p.get_batch(11).tasks.size(), 0);

        ASSERT_EQ(p.get_batch(13).tasks.size(), 2);
        ASSERT_EQ(p.get_batch(13).tasks.size(), 0);
    }
    {
        TimePlanner<int> p;
        p.add(TimePlanner<int>::Task(1, 10, 0, TimePlanningOptions(10, 5)));
        p.add(TimePlanner<int>::Task(2, 20, 1, TimePlanningOptions(10, 5)));
        p.add(TimePlanner<int>::Task(3, 30, 2, TimePlanningOptions(10, 5)));
        p.add(TimePlanner<int>::Task(4, 40, 2, TimePlanningOptions(10, 5, 1))); // max cycles = 1

        ASSERT_EQ(p.get_batch(3).tasks.size(), 4);
        ASSERT_EQ(p.get_batch(3).tasks.size(), 0);
        ASSERT_EQ(p.get_batch(3).tasks.size(), 0);

        ASSERT_EQ(p.get_batch(11).tasks.size(), 2);
        ASSERT_EQ(p.get_batch(11).tasks.size(), 0);

        ASSERT_EQ(p.get_batch(13).tasks.size(), 1); // affects this
        ASSERT_EQ(p.get_batch(13).tasks.size(), 0);
    }
    {
        TimePlanner<int> p;
        p.add(TimePlanner<int>::Task(1, 10, 0, TimePlanningOptions(10, 5)));
        p.add(TimePlanner<int>::Task(2, 20, 1, TimePlanningOptions(10, 5)));
        p.add(TimePlanner<int>::Task(3, 30, 2, TimePlanningOptions(10, 5)));
        p.add(TimePlanner<int>::Task(4, 40, 2, TimePlanningOptions(10, 5)));

        ASSERT_EQ(p.get_batch(3, 2).tasks.size(), 2);
        ASSERT_EQ(p.get_batch(3, 2).tasks.size(), 2);
        ASSERT_EQ(p.get_batch(3).tasks.size(), 0);
        ASSERT_EQ(p.get_batch(3).tasks.size(), 0);

        ASSERT_EQ(p.get_batch(11, 1).tasks.size(), 1);
        ASSERT_EQ(p.get_batch(11, 1).tasks.size(), 1);
        ASSERT_EQ(p.get_batch(11).tasks.size(), 0);

        ASSERT_EQ(p.get_batch(13, 1).tasks.size(), 1);
        ASSERT_EQ(p.get_batch(13, 1).tasks.size(), 1);
        ASSERT_EQ(p.get_batch(13).tasks.size(), 0);
    }
    {
        TimePlanner<int> p;
        p.add(TimePlanner<int>::Task(1, 10, 0, TimePlanningOptions(10, 5)));
        p.add(TimePlanner<int>::Task(2, 20, 1, TimePlanningOptions(10, 5)));
        p.add(TimePlanner<int>::Task(3, 30, 2, TimePlanningOptions(10, 5)));
        p.add(TimePlanner<int>::Task(4, 40, 2, TimePlanningOptions(10, 5)));

        ASSERT_EQ(p.get_batch(3).tasks.size(), 4);
        ASSERT_EQ(p.get_batch(3).tasks.size(), 0);
        ASSERT_EQ(p.get_batch(3).tasks.size(), 0);

        p.remove(20);

        ASSERT_EQ(p.get_batch(11).tasks.size(), 1);
        ASSERT_EQ(p.get_batch(11).tasks.size(), 0);

        ASSERT_EQ(p.get_batch(13).tasks.size(), 2);
        ASSERT_EQ(p.get_batch(13).tasks.size(), 0);
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
