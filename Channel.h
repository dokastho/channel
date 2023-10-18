#ifndef CHANNEL
#define CHANNEL

#include <queue>
#include <mutex>
#include <condition_variable>

template <class T>
class Channel
{
private:
    std::queue<T> items;
    std::mutex __l;
    std::condition_variable cv;

public:
    T get()
    {
        std::unique_lock<std::mutex> l(__l);
        while (items.empty())
        {
            cv.wait(l);
        }

        T item = items.front();
        items.pop();
        l.unlock();
        return item;
    }

    void add(T item)
    {
        std::unique_lock<std::mutex> l(__l);
        items.push(item);
        cv.notify_one();
        l.unlock();
    }

    size_t size()
    {
        std::unique_lock<std::mutex> l(__l);
        size_t s = items.size();
        l.unlock();
        return s;
    }

    bool empty()
    {
        std::unique_lock<std::mutex> l(__l);
        bool b = items.empty();
        l.unlock();
        return b;
    }
};

template <class T>
class priority_channel
{
    struct priority_item
    {
        uint8_t rank;
        T datum;
    };

    struct priority_comp
    {
        bool operator()(priority_item &lhs, priority_item &rhs)
        {
            return lhs.rank < rhs.rank;
        }
    };

    std::priority_queue<priority_item, std::vector<T>, priority_comp> items;
    std::mutex __l;
    std::condition_variable cv;

public:
    T get()
    {
        std::unique_lock<std::mutex> l(__l);
        while (items.empty())
        {
            cv.wait(l);
        }

        T item = items.front();
        items.pop();
        l.unlock();
        return item;
    }

    void add(T item, uint8_t priority)
    {
        std::unique_lock<std::mutex> l(__l);
        items.push(priority_item{item, priority});
        cv.notify_one();
        l.unlock();
    }

    size_t size()
    {
        std::unique_lock<std::mutex> l(__l);
        size_t s = items.size();
        l.unlock();
        return s;
    }

    bool empty()
    {
        std::unique_lock<std::mutex> l(__l);
        bool b = items.empty();
        l.unlock();
        return b;
    }
};
#endif
