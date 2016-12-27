

#pragma once


#include <vector>


template<class T> class LockFreeFifo
{
public:
    LockFreeFifo (unsigned int bufsz) : mReadIdx(0), mWriteIdx(0), mBuffer(bufsz)
    {
    }
    
    T get(void)
    {
        
        //	throw runtime_error ("lock free fifo underrun");
        unsigned int readIndex = mReadIdx.load(std::memory_order_relaxed);
        
        T result = mBuffer[readIndex];
        
        if ((readIndex + 1) >= mBuffer.size())
            readIndex = 0;
        else
            readIndex = readIndex + 1;
        
        mReadIdx = readIndex;
        
        return result;
    }
    
    void put(T element)
    {
        unsigned int newIdx = mWriteIdx.load(std::memory_order_relaxed) + 1;
        
        if (newIdx >= mBuffer.size())
            newIdx = 0;
        
        //throw runtime_error ("lock free fifo overrun");
        
        mBuffer[mWriteIdx] = element;
        
        mWriteIdx = newIdx;
    }
    
    bool canRead() const
    {
        return mReadIdx != mWriteIdx.load(std::memory_order_relaxed);
    }
    
    bool canWrite() const
    {
        unsigned int newIdx = mWriteIdx.load(std::memory_order_relaxed) + 1;
        
        if (newIdx >= mBuffer.size())
            newIdx = 0;
        
        
        return newIdx != mReadIdx.load(std::memory_order_relaxed);
    }
    
private:
    
    atomic_t mReadIdx;
    atomic_t mWriteIdx;
    std::vector<T> mBuffer;
};

