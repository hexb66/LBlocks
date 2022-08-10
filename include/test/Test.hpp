#include <iostream>
#include <LBlocks/LBlocks.hpp>
#include <conio.h>
#include <Windows.h>
#include <LBlocks/LLog.hpp>

namespace lee
{
    namespace blocks
    {
        namespace pd_controller_test
        {
            // Simple observer block
            struct ObserverOutput{double Pos, Vel;};
            class ObserverBlock : public LBlock<double *, ObserverOutput>
            {
            private:
                double LastPos;
                double T;
                int InitFlag;

            public:
                ObserverBlock(double _T = 0.001) : LastPos(0), T(_T), InitFlag(0){};
                int run()
                {
                    if (InitFlag == 0)
                    {
                        this->LastPos = this->getMeasured();
                        this->DataOutput.Pos = this->getMeasured();
                        this->DataOutput.Vel = 0;
                        this->InitFlag = 1;
                    }
                    else
                    {
                        this->DataOutput.Pos = 0.2 * this->getMeasured() + 0.8 * this->LastPos;
                        this->DataOutput.Vel = (this->DataOutput.Pos - this->LastPos) / this->T;
                        this->LastPos = this->DataOutput.Pos;
                    }
                    return 0;
                };
                int print()
                {
                    std::cout << "Measured pos: " << this->getMeasured() << ", observed pos: " << this->DataOutput.Pos << ", vel: " << this->DataOutput.Vel << ", ";
                    return 0;
                };

                inline double &getMeasured() { return *this->DataInput; };
            };

            // Simple PD controller block
            struct PDInput{double *RefPos, *RealPos, *RealVel;};
            class PDBlock : public LBlock<PDInput, double>
            {
            private:
                double Kp, Kd;

            public:
                int init()
                {
                    this->Kp = 1000.0;
                    this->Kd = 100.0;
                    
                    return 0;
                };
                int run()
                {
                    this->DataOutput = -this->Kp * (*this->DataInput.RealPos - *this->DataInput.RefPos) - this->Kd * (*this->DataInput.RealVel);
                    
                    return 0;
                };
                int print()
                {
                    std::cout << "Control: " << this->DataOutput << ", ";
                    
                    return 0;
                };
            };

            // Simple mass system
            struct MassOutput{double Pos, Vel;};
            class MassBlock : public LBlock<double *, MassOutput>
            {
            private:
                double Mass, Acc, T;

            public:
                MassBlock(double _T = 0.001) : T(_T), Mass(10.0), Acc(0)
                {
                    this->DataOutput.Pos = 0.0;
                    this->DataOutput.Vel = 0.0;
                };
                int run()
                {
                    this->Acc = (*this->DataInput) / this->Mass;
                    this->DataOutput.Vel += this->Acc * this->T;
                    this->DataOutput.Pos += this->DataOutput.Vel * this->T;
                    return 0;
                };
                int print()
                {
                    std::cout << "Real pos: " << this->DataOutput.Pos << ", ";
                    return 0;
                }
            };

            // Whole system
            class Sys : public LBlock<double *, double *>
            {
            private:
                double Time, T;
                std::shared_ptr<MassBlock> pMassBlock;

            public:
                int init()
                {
                    this->Time = 0;
                    this->T = 0.001;

                    auto _p_ob_block = this->addBlock(new ObserverBlock(this->T));
                    auto _p_pd_block = this->addBlock<PDBlock>();
                    this->pMassBlock = this->addBlock(new MassBlock(this->T));

                    _p_ob_block->setInput(&this->getRealPos());
                    _p_pd_block->setInput({&this->getRefPos(), &_p_ob_block->getOutput().Pos, &_p_ob_block->getOutput().Vel});
                    this->pMassBlock->setInput(&_p_pd_block->getOutput());

                    for (auto i : this->SubBlockList)
                    {
                        i->init();
                    }
                    return 0;
                }

                int run()
                {
                    for (auto i : this->SubBlockList)
                    {
                        i->run();
                    }
                    this->Time += this->T;
                    this->getRealPos() = this->pMassBlock->getOutput().Pos;
                    return 0;
                };

                int print()
                {
                    std::cout << std::fixed << "Time = " << this->Time << "s, ";
                    for (auto i : this->SubBlockList)
                    {
                        i->print();
                    }
                    std::cout << std::endl;
                    return 0;
                };

                int log()
                {
                    return 0;
                };

                void setOutput(double *_RealPos) { this->DataOutput = _RealPos; };

                inline double &getRefPos() { return *this->DataInput; };
                inline double &getRealPos() { return *this->DataOutput; };
            };

            void test()
            {
                double RefPos = 1.0;
                double RealPos = 0.0;
                int Key = 0;
                Sys sys;
                LLog<double> Log;
                sys.setInput(&RefPos);
                sys.setOutput(&RealPos);
                sys.init();
                std::cout<<"start"<<std::endl;
                while(Key!=' ')
                {
                    Key = 0;
                    if(_kbhit())
                    {
                        Key = _getch();
                    }
                    sys.run();
                    sys.print();

                    Log.startLog();
                    Log.addLog(RefPos,  "Ref Position");
                    Log.addLog(RealPos, "Real Position");
                }
                Log.saveLog("test.dat");
            }
        } // namespace pd_controller_test
    } // namespace block
} // namespace lee