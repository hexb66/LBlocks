#include <iostream>
#include <test/Test.hpp>
#include <test/TestPBlock.hpp>

int main()
{
    // lee::blocks::pd_controller_test::test();
    pblock_test::Block a;
    a.addBlock(new pblock_test::Block("SubBlock1"));
    a.addBlock(new pblock_test::Block("SubBlock2"));

    std::thread b([&a](){a.test();});
    b.join();

    std::cout<<"over"<<std::endl;
    return 0;
}