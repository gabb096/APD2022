//
//  DelayLine.hpp
//  GabrieleLeva
//
//  Created by Gabriele on 07/12/22.
//
#ifndef DelayLine_hpp
#define DelayLine_hpp

#include <memory>
#include <stdio.h>

class DelayLine{
    
   
public:
    
    int counter, sizeOfDelayTime;
    
    float* BufferL;
    float* BufferR;
    
    
    DelayLine();
    ~DelayLine();
    
    void initDelayLine(int sample_rate, int del_max_delay);
    
};

DelayLine::DelayLine()
{
    counter = 0;
    sizeOfDelayTime = 0;
    
    BufferL = NULL;
    BufferR = NULL;
    
}

DelayLine::~DelayLine()
{
    
}

void DelayLine::initDelayLine(int sample_rate, int del_max_delay){
    
    counter = 0;
    
    sizeOfDelayTime = del_max_delay * sample_rate * sizeof(float);
    
    if(NULL != BufferL)
        free(BufferL);

    if(NULL != BufferR)
        free(BufferR);
    
    /* Inizialize buffers to 0 */
    BufferL = (float*) malloc(sizeOfDelayTime);
    memset(BufferL, 0, sizeOfDelayTime);
    
    BufferR = (float*) malloc(sizeOfDelayTime);
    memset(BufferR, 0, sizeOfDelayTime);
}

#endif  /* DelayLine_hpp */
