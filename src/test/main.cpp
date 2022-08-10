#include <iostream>
#include <test/Test.hpp>
#include <test/TestPBlock.hpp>

int main()
{
    // lee::blocks::pd_controller_test::test();
    pblock_test::Block a;
    a.addBlock<pblock_test::Block>();

    std::thread b([&a](){a.test();});
    b.join();

    std::cout<<"over"<<std::endl;
    return 0;
}