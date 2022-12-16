//
//  DelayLine_V2.h
//  GabrieleLeva
//
//  Created by Gabriele on 15/12/22.
//

#ifndef DelayLine_V2_hpp
#define DelayLine_V2_hpp

#include <memory>
#include <stdio.h>

class DelayLine_V2{
    
   
public:
    
    int writeHead, readHead, size_Of_Delay_in_Samples;
    
    float* BufferL;
    float* BufferR;
    
    
    DelayLine_V2();
    ~DelayLine_V2();
    
    void initDelayLine(int sample_rate, float max_delay);
   
//    void writeSample(int channel, float sample);
//    float readSample(int channel);
};

DelayLine_V2::DelayLine_V2()
{

    writeHead = 0;
    readHead  = 0;
    size_Of_Delay_in_Samples = 48000;
    
    BufferL = NULL;
    BufferR = NULL;
    
}

DelayLine_V2::~DelayLine_V2()
{
    
}

void DelayLine_V2::initDelayLine(int sample_rate, float max_delay){
    
    writeHead = 0;
    readHead  = 0;
    
    size_Of_Delay_in_Samples = max_delay * sample_rate * sizeof(float);
    
    if(NULL != BufferL)
        free(BufferL);

    if(NULL != BufferR)
        free(BufferR);
    
    /* Inizialize buffers to 0 */
    BufferL = (float*) malloc(size_Of_Delay_in_Samples);
    memset(BufferL, 0, size_Of_Delay_in_Samples);
    
    BufferR = (float*) malloc(size_Of_Delay_in_Samples);
    memset(BufferR, 0, size_Of_Delay_in_Samples);
}

//void DelayLine_V2::writeSample(int channel, float sample)
//{
//    writeHead = writeHead % size_Of_Delay_in_Samples;
//
//    if(channel == 1)
//        BufferR[writeHead] = sample;
//    else
//        BufferL[writeHead] = sample;
//}
//
//float DelayLine_V2::readSample(int channel)
//{
//    readHead = readHead % size_Of_Delay_in_Samples;
//
//    if(channel == 1)
//        return BufferR[readHead];
//    else
//        return BufferL[readHead];
//}

#endif /* DelayLine_V2_hpp */
