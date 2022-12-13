////
////  DelayLine.cpp
////  GabrieleLeva
////
////  Created by Gabriele on 07/12/22.
////
//
//#include "DelayLine.hpp"
//
//DelayLine::DelayLine()
//{
//    counter = 0;
//   
//    BufferL = NULL;
//    BufferR = NULL;
//    
//}
//
//DelayLine::~DelayLine()
//{
//    
//}
//
//void DelayLine::initDelayLine(int sample_rate, int del_max_delay){
//    
//    counter = 0;
//
//    int sizeOfDelayTime = del_max_delay * sample_rate * sizeof(float);
//    
//    if(NULL != BufferL)
//        free(BufferL);
//
//    if(NULL != BufferR)
//        free(BufferR);
//    
//    /* Inizialize buffers to 0 */
//    BufferL = (float*) malloc(sizeOfDelayTime);
//    memset(BufferL, 0, sizeOfDelayTime);
//    
//    BufferR = (float*) malloc(sizeOfDelayTime);
//    memset(BufferR, 0, sizeOfDelayTime);
//}
