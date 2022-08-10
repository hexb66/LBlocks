#pragma once
#include <LBlocks/PBlock.hpp>
#include <chrono>

namespace pblock_test{

struct Input{};
struct Output{};

class Block:public lee::blocks::PBlock<Input,Output>
{
protected:
    int k;
    int spinOnce()
    {
        k++;
        return 0;
    };
public:
    Block()
    {
        this->BlockName = "TestParallelBlock";
        this->setWaitCount(0);
        this->setWaitTimeMs(1);
        this->k = 0;
    };

    void test()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        for (int i = 0; i < 10; i++)
        {
            this->run();
            this->print();
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    };

    int print()
    {
        std::cout<<this->Msg<<", k="<<this->k<<std::endl;
        return 0;
    }
}; 

}