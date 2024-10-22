
#pragma once

#include <atomic>
#include <tuple>

#include "ozz/types.hpp"

namespace OZZ_CORE::utils{

template<typename T>
class xpipe
{
public:
    struct slot_t
    {   
        inline char* cdata() noexcept
        { return arr; }

         inline T* data() noexcept
         { return reinterpret_cast<T*>(arr); }

        inline uint64_t seq() noexcept
        { return sequence; }

        slot_t() noexcept
        { memset(arr, 0, sizeof(T)); }

    private:
        explicit slot_t(T* data) noexcept
        { memcpy(arr, data, sizeof(T)); }

    private:
        uint64_t sequence{0};
        char  arr[sizeof(T)];

        friend class xpipe;
    };

public:

    /** Constructor
     * \param size Number of nodes.
     */
    explicit xpipe(size_t size) noexcept;
    
    /// Destructor
    ~xpipe() noexcept;

    /// Deallocates nodes
    void destroy() noexcept;

    void notify_all() noexcept;

    std::tuple<T*, uint64_t> push(const T& arg) noexcept;

    inline slot_t* head()
    { return &memblk[sequence.load(std::memory_order_relaxed) % mem_size]; } 

    inline slot_t* tail() noexcept
    { return &memblk[mem_size - 1]; }

    inline slot_t* next(uint64_t seq) const noexcept
    { return &memblk[(seq + 1) % mem_size]; }

    inline const slot_t* slot(uint64_t seq) const noexcept;

    void commit(slot_t* slot, uint64_t seq) noexcept
    { slot->sequence = seq + 1; }

    inline size_t size() const noexcept
    { return mem_size; }

    inline long long diff() noexcept
    { return sequence - mem_size; }

    void wait() noexcept
    { 
        signal.wait(false);
        signal.clear();
    }

private:

    std::tuple<slot_t*, uint64_t> memseq() noexcept;

private:
    
    const size_t              mem_size;
    slot_t*                   memblk;
    std::atomic_flag          signal;
    std::atomic<uint64_t>     sequence{0};///< Number of valid data.
    std::atomic_bool          destroyed{false};
public:
    
class receiver
{
public:
    /**
     * Constructs ringlist cursor
     * @param xpipe instance
     * @details Can be called only from ringlist instance
     */
    receiver(xpipe<T>& instance) noexcept
        : ring(instance)
        , last(instance.diff() < 0 ? instance.tail() : instance.head())
    {}
    
    ~receiver() noexcept = default;

    void sign(bool fl) noexcept
    { ring.sign(fl); }

    void notify_all() noexcept
    { ring.notify_all(); }

    void wait() noexcept
    { ring.wait(); }   

    /**
     * Acquires a next available slot from the RING
     * @return pointer to user data in a slot memory
     */
     T* recv() noexcept
    {
        slot_t* slot = ring.next(last->seq());
        if(last->seq() < slot->seq())
        {
            last = slot;
            return last->data();
        }
        
        return nullptr;
    }

protected:
    
    /// Ring instance
    xpipe<T>& ring;

    /// Last head memory cell sequence
    typename xpipe<T>::slot_t* last;
};

friend class receiver;
};

template <typename T>
xpipe<T>::xpipe(size_t size) noexcept
    : mem_size(size)
    , memblk(new (std::nothrow) slot_t[size])
{}

template <typename T>
xpipe<T>::~xpipe() noexcept
{ destroy(); }

template <typename T>
std::tuple<typename xpipe<T>::slot_t*, uint64_t> xpipe<T>::memseq() noexcept
{
    uint64_t msq_entry = sequence.fetch_add(1, std::memory_order_relaxed);
    return std::make_tuple(&memblk[(msq_entry + 1) % mem_size], msq_entry);
}

template <typename T>
std::tuple<T*, uint64_t> xpipe<T>::push(const T& arg) noexcept
{
    auto [node, seq] = memseq();
    char* ptr = node->cdata();
    memcpy(ptr, &arg, sizeof(T));
    commit(node, seq);
    return std::make_tuple(node->data(), seq);
}    

template <typename T>
inline const xpipe<T>::slot_t* xpipe<T>::slot(uint64_t seq) const noexcept
{
    slot_t* inst = &memblk[seq % mem_size];
    if(seq != inst->sequence)
        return nullptr;
    return inst;
}

template <typename T>
void xpipe<T>::destroy() noexcept
{
    if(!destroyed.exchange(true))
    {
        sequence = 0;
        delete[] memblk;
    }
}

template <typename T>
void xpipe<T>::notify_all() noexcept
{
    signal.test_and_set();
    signal.notify_all();
}

}

