#ifndef TIMEGROUPMAKER_HPP
#define TIMEGROUPMAKER_HPP

#include <vector>
#include <map>
#include <cstdint>

struct TimePlanningOptions
{
    TimePlanningOptions(uint32_t interval = 1000, uint32_t duration = 100, uint32_t cycles_count = 0, uint32_t timeout = 0) :
        duration(duration), interval(interval), cycles_count(cycles_count), timeout(timeout)
    { }

    TimePlanningOptions& operator=(const TimePlanningOptions&) = default;

    uint32_t duration;
    uint32_t interval;
    uint32_t cycles_count;
    uint32_t timeout;
};

template<typename T>
class TimePlanner
{
public:
    class Task
    {
        friend class TimePlanner;
    public:
        Task() = default;

        Task(T data, uint32_t id, uint32_t current_time, const TimePlanningOptions& time_planning_options) :
            m_id(id), m_time_planning_options(time_planning_options), m_data(data), m_creation_time(current_time)
        {
        }

        Task& operator=(const Task&) = default;

    private:
        uint32_t m_id = 0;

        TimePlanningOptions m_time_planning_options;
        T m_data;

        uint32_t m_creation_time = 0;
        uint32_t m_cycle_done = 0;
        uint32_t m_repetitions = 0;
    };

    struct Batch
    {
        std::vector<T> tasks;
    };

    void add(const Task& task)
    {
        m_active_tasks[task.m_id] = task;
    }

    void remove(uint32_t id)
    {
        auto it = m_active_tasks.find(id);
        if (it != m_active_tasks.end())
            m_active_tasks.erase(it);
    }

    bool has_task(uint32_t id)
    {
        auto it = m_active_tasks.find(id);
        return it != m_active_tasks.end();
    }

    Batch get_batch(uint32_t time, std::size_t task_limit = 0)
    {
        TimePlanner::Batch result;

        for (auto it = m_active_tasks.begin(); it != m_active_tasks.end(); )
        {
            if (task_limit != 0 && result.tasks.size() == task_limit)
                break;

            Task& task = it->second;

            uint32_t lifetime = time - task.m_creation_time;

            // Remove tasks
            if (
                    // Remove task if timeouted
                    (task.m_time_planning_options.timeout != 0 && lifetime >= task.m_time_planning_options.timeout)
                    // or if all cycles are done AND timeout == 0
                    || (task.m_time_planning_options.timeout == 0 && task.m_time_planning_options.cycles_count != 0 && task.m_repetitions >= task.m_time_planning_options.cycles_count)
                )
            {
                auto jt = std::next(it);
                m_active_tasks.erase(it);
                it = jt; continue;
            }

            // Skip task with cycles limit
            if (task.m_time_planning_options.cycles_count != 0 && task.m_repetitions >= task.m_time_planning_options.cycles_count)
            {
                it++; continue;
            }

            if (task.m_cycle_done == 0)
            {
                // Task was not used yet
                result.tasks.push_back(task.m_data);
                task.m_cycle_done = 1;
                task.m_repetitions++;
                it++; continue;
            }


            uint32_t cycle_duration = (task.m_time_planning_options.duration + task.m_time_planning_options.interval);
            uint32_t time_in_cycle = lifetime % cycle_duration;
            uint32_t cycle_number = lifetime / cycle_duration + 1;

            if (time_in_cycle >= task.m_time_planning_options.interval && task.m_cycle_done <= cycle_number)
            {
                result.tasks.push_back(task.m_data);
                task.m_cycle_done = cycle_number + 1;
                task.m_repetitions++;
            }
            it++;
        }
        return result;
    }


private:
    std::map<uint32_t, Task> m_active_tasks;
    int m_last_task;
    uint32_t m_last_time = 0;
};

/*
template<typename T>
class TimeGrouper
{
public:

    struct DurationalObject
    {
        DurationalObject(uint32_t time_from, uint32_t time_to, const T data) :
            time_from(time_from), time_to(time_to), data(data)
        {
        }
        uint32_t time_from;
        uint32_t time_to;
        T data;
    };

    struct TimeSlot
    {
        uint32_t time_from = 0;
        uint32_t time_to = 0;
        std::vector<DurationalObject> objects;
    };

    TimeGrouper()  :
        m_objects(compare_ptrs)
    {
    }
    void add_object(const DurationalObject& obj)
    {
        m_objects.insert(obj);
    }

    TimeSlot next_slot(uint32_t limit = std::numeric_limits<uint32_t>::max())
    {
        TimeSlot result;
        auto it = m_objects.begin();
        if (it == m_objects.end())
            return result;

        uint32_t time_from = it->time_from;
        uint32_t time_to = it->time_to;

        result.objects.push_back(*it);
        m_objects.erase(it);

        uint32_t count = 1;

        while (!m_objects.empty())
        {
            if (count == limit)
                break;
            auto it = m_objects.begin();
            if (it->time_from > time_to)
                break;

            result.objects.push_back(*it);
            time_to = std::min(time_to, it->time_to);
            time_from = std::max(time_from, it->time_from);
            m_objects.erase(it);
            count++;
        }
        result.time_from = time_from;
        result.time_to = time_to;
        return result;
    }

private:
    static bool compare_ptrs(const DurationalObject& p1, const DurationalObject& p2)
    {
        return p1.time_from < p2.time_from;
    }

    std::multiset<DurationalObject, decltype(compare_ptrs)*> m_objects;
}; */

#endif // TIMEGROUPMAKER_HPP
