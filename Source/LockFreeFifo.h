
/*
 NUI3 - C++ cross-platform GUI framework for OpenGL based applications
 Copyright (C) 2002-2003 Sebastien Metrot
 */

#pragma once
#include "SFZCommon.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		LockFreeFifo
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T> class LockFreeFifo : NonCopyable
{
public:
    LockFreeFifo (unsigned int bufsz) : mReadIdx(0), mWriteIdx(0), mBuffer(bufsz)
    {
    }
    
    T get(void)
    {
        //	throw runtime_error ("lock free fifo underrun");
        
        T result = mBuffer[mReadIdx];
        
        if ((mReadIdx + 1) >= mBuffer.size())
            mReadIdx = 0;
        else
            mReadIdx = mReadIdx + 1;
        
        return result;
    }
    
    void put(T element)
    {
        unsigned int newIdx;
        
        if ((mWriteIdx + 1) >= mBuffer.size())
            newIdx = 0;
        else
            newIdx = mWriteIdx + 1;
        
        //throw runtime_error ("lock free fifo overrun");
        
        mBuffer[mWriteIdx] = element;
        
        mWriteIdx = newIdx;
    }
    
    bool canRead() const
    {
        return mReadIdx != mWriteIdx;
    }
    
    bool canWrite() const
    {
        unsigned int newIdx;
        
        if ((mWriteIdx + 1) >= mBuffer.size())
            newIdx = 0;
        else
            newIdx = mWriteIdx + 1;
        
        return newIdx != mReadIdx;
    }
    
private:
    
    atomic_t mReadIdx;
    atomic_t mWriteIdx;
    std::vector<T> mBuffer;
};

