#include <infos/kernel/sched.h>
#include <infos/kernel/thread.h>
#include <infos/kernel/log.h>
#include <infos/util/list.h>
#include <infos/util/lock.h>

using namespace infos::kernel;
using namespace infos::util;

// Define the number of priority levels
#define NUM_PRIORITIES 4

/**
 * A Multiple Queue priority scheduling algorithm
 */
class MultipleQueuePriorityScheduler : public SchedulingAlgorithm
{
public:
    /**
     * Returns the friendly name of the algorithm, for debugging and selection purposes.
     */
    const char* name() const override { return "mq"; }

    /**
     * Called during scheduler initialisation.
     */
    void init()
    {
        // Meh
    }

    /**
     * Called when a scheduling entity becomes eligible for running.
     * @param entity
     */
    void add_to_runqueue(SchedulingEntity& entity) override
    {
        // Determine the priority level of the entity
        int priority = entity->priority();
        if (priority >= NUM_PRIORITIES) {
            priority = NUM_PRIORITIES - 1;
        }
        if (priority = 0) {
            runqueue0.append(&entity);
        }
        if (priority = 1) {
            runqueue1.append(&entity);
        }
        if (priority = 2) {
            runqueue2.append(&entity);
        }
        if (priority = 3) {
            runqueue3.append(&entity);
        }

        // Add the entity to the appropriate run queue
    }

    /**
     * Called when a scheduling entity is no longer eligible for running.
     * @param entity
     */
    void remove_from_runqueue(SchedulingEntity& entity) override
    {
        // Determine the priority level of the entity
        int priority = entity->priority();
        if (priority >= NUM_PRIORITIES) {
            priority = NUM_PRIORITIES - 1;
        }
        if (priority = 0) {
            runqueue0.remove(&entity);
        }
        if (priority = 1) {
            runqueue1.remove(&entity);
        }
        if (priority = 2) {
            runqueue2.remove(&entity);
        }
        if (priority = 3) {
            runqueue3.remove(&entity);
        }

        // Remove the entity from the appropriate run queue

    }

    /**
     * Called every time a scheduling event occurs, to cause the next eligible entity
     * to be chosen.  The next eligible entity might actually be the same entity, if
     * e.g. its timeslice has not expired.
     */
    SchedulingEntity *pick_next_entity() override
    {
        
        SchedulingEntity *next = NULL;

        // Select the highest priority non-empty run queue
        if (runqueue0.count() > 0) {
            next = &runqueue0.first();
            runqueue0.remove(next);
            runqueue0.append(next);
            goto label1;
        }
        if (runqueue1.count() > 0) {
            SchedulingEntity *next = &runqueue1.first();
            runqueue1.remove(*next);
            runqueue1.append(next);
            goto label1;
        }
        if (runqueue2.count() > 0) {
            SchedulingEntity *next = &runqueue2.first();
            runqueue2.remove(*next);
            runqueue2.append(next);
            goto label1;
        }
        if (runqueue3.count() > 0) {
            SchedulingEntity *next = &runqueue3.first();
            runqueue3.remove(*next);
            runqueue3.append(next);
            goto label1;
           
        }
        label1:
        return next;

    }

private:
    // The run queues for each priority level
    List<SchedulingEntity*> runqueue0;
    List<SchedulingEntity*> runqueue1;
    List<SchedulingEntity*> runqueue2;
    List<SchedulingEntity*> runqueue3;
};

// Register the scheduler
RegisterScheduler(MultipleQueuePriorityScheduler);
/* --- DO NOT CHANGE ANYTHING BELOW THIS LINE --- */











    /**
     * Called every time a scheduling event occurs, to cause the next eligible entity
     * to be chosen.  The next eligible entity might actually be the same entity, if
     * e.g. its timeslice has not expired.
     
    SchedulingEntity *pick_next_entity() override
    /*{
		if (runqueue.count() == 0) return NULL;
		if (runqueue.count() == 1) return runqueue.first();


        for (const auto& entity : runqueue) {
            if (entity->priority() == SchedulingEntityPriority::REALTIME) {
                return entity;
            }
        }

        // TODO: Implement me!
    }

    {
        for (auto& queue : runqueue) {
            if (!queue.empty()) {
                auto entity = queue.front();
                queue.pop_front();
                return entity;
            }
        }
        return nullptr;
    }

    void enqueue_entity(SchedulingEntity *entity) override
    {
        auto priority = static_cast<int>(entity->priority());
        runqueue[priority].push_back(entity);
    }
private:
	List<SchedulingEntity *> runqueue;
};






/*
List.h
This is a C++ header file implementing a singly linked list data structure. The file defines several nested structs and classes for the list, including ListNode, ListIterator, and List.

ListNode is a simple struct representing a node in the list. It contains a pointer to the next node in the list and a data element.

ListIterator is an iterator for the list. It contains a pointer to the current node in the list and provides methods to increment the iterator and access the data element.

List is the main class representing the list data structure. It contains a pointer to the first node in the list, a count of the number of elements in the list, and methods for manipulating the list such as append, remove, dequeue, enqueue, push, pop, first, last, at, count, empty, clear, begin, and end.

Overall, this header file provides a basic implementation of a singly linked list data structure in C++.

*/


