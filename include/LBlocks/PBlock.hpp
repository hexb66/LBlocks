// PBlock.hpp
// With parallel running function
// -- Version 0.0, Jun. 11, 2022
// Copyright (C) 2021-2022, Qingqing Li (liqingmuguang@163.com). 
//
// This software may be modified and distributed under the terms
// of the MIT license.  See the LICENSE file for details.

#pragma once
#include "LBlocks.hpp"
#include <thread>
#include <mutex>
namespace lee{namespace blocks{

template<typename INPUT_TYPE, typename OUTPUT_TYPE>
class PBlock:public LBlock<INPUT_TYPE, OUTPUT_TYPE>
{
protected:
    std::string Msg;
    std::thread Thread;
    std::timed_mutex Ping, Pong;
    int WaitCount;
    int Count;
    bool SubThreadRunFlag;
    int MaxWaitTimeMs;
    
    virtual int spinOnce() = 0;
    virtual void input(){};
    virtual void output(){};

    void closeSubThread()
    {
        this->SubThreadRunFlag = false;
        this->Ping.unlock();
        this->Pong.unlock();
    };

public:
    PBlock():Count(0), WaitCount(0), SubThreadRunFlag(true), MaxWaitTimeMs(5)
    {
        this->Ping.lock();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        this->Thread = std::thread([this](){
            std::cout<<"Open sub thread: "<<this->BlockName<<std::endl;
            while(1)
            {
                this->Pong.unlock();
                this->Ping.lock();
                if(!this->SubThreadRunFlag)
                {
                    this->Ping.unlock();
                    break;
                }
                this->spinOnce();
                this->Ping.unlock();
                this->Pong.lock();
            }
            std::cout<<"Close sub thread: "<<this->BlockName<<std::endl;
        });
        this->Thread.detach();
    };

    ~PBlock()
    {
        this->closeSubThread();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    };

    int run()
    {
        if (this->WaitCount == 0)
        {
            this->input     ();
            this->spinOnce  ();
            this->output    ();
            this->Msg = "Output OK!";
            return -1;
        }
        if (this->Count == 0)
        {
            this->input();
            this->Pong.lock();
            this->Ping.unlock();
            this->Msg = "Wait Res  ";
            // Next line should be replaced by better way to give enough time to trigger the sub thread.
            // std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
        if(this->Count++>=this->WaitCount)
        {
            if(this->Ping.try_lock_for(std::chrono::microseconds(this->MaxWaitTimeMs)))
            {
                this->Pong.unlock();
                this->Count = 0;
                this->Msg = "Output OK!";
                this->output();
            }
            else
            {
                this->Msg = "OVER TIME ";
            }
        }
        return 0;
    };

    inline void setWaitCount(const int &Counts){this->WaitCount = Counts;};
    inline void setWaitTimeMs(const int &TimeMs){this->MaxWaitTimeMs = TimeMs;};
};
}}