/* Low Frequency Oscillator with 2 waveshapes, the output goes from */
#include <math.h>
#include "public.sdk/source/vst2.x/audioeffectx.h"

#ifndef LowFreqOsc_hpp
#define LowFreqOsc_hpp

enum waveShape{
    waveShape_sin,
    waveShape_rmp,
    waveShape_tri,
    waveShape_sqr
};

class LowFreqOsc{
    
    float freq, counter, simmetry;
    int sampleRate, isBipolar;
    
    waveShape shape;
        
public:
    LowFreqOsc();
    ~LowFreqOsc();
    
    void SetSampleRate(int _SampleRate);
    void SetFrequenccy(float _freq);
    void SetShape(waveShape _shape);
    void SetIsBipolar(int _bipolar);
    void setCounter(float _position);
    
    float getFrequency();
    float GetOutputValue();
    
    void AdvanceCounter();
    void initLFO(int _sampleRate, float _frequency, waveShape _shape);
    
};

LowFreqOsc::LowFreqOsc(){
    counter = 0;
    sampleRate = 1;
    freq = 0;
    simmetry = 0.5;
    isBipolar = 0;
    shape = waveShape_sin;
}

LowFreqOsc::~LowFreqOsc()                           {                           }
void LowFreqOsc::SetSampleRate(int _SampleRate)     { sampleRate = _SampleRate; }
void LowFreqOsc::SetFrequenccy(float _freq)         { freq = _freq;             }
void LowFreqOsc::SetShape(waveShape _shape)         { shape = _shape;           }
void LowFreqOsc::SetIsBipolar(int _bipolar)         { isBipolar = _bipolar;     }
void LowFreqOsc::setCounter(float _position)        { counter = _position;      }

float LowFreqOsc::getFrequency()                    { return freq;               }

float LowFreqOsc::GetOutputValue(){
    
    float output = 0;
    
    switch(shape)
    {
        case waveShape_sin: // approximated using sin(pi*x) = 4x(1-x) in [0,1]
            // output = sin(counter*M_2_PI);
            if( isBipolar )
                output = (counter*2-1) <= 0.0 ? (4.f * counter * (1.f + counter)) : (4.f * counter * (1.f - counter)) ;
            else
                output = 4.f * counter * (1.f - counter);
            break;
            
        case waveShape_tri:
            if( isBipolar )
                output = (counter*2-1) <= 0.0 ? (-2.f*counter + 1.f) : (2.f*counter - 1.f) ;
            else
                output = counter <= 0.5 ? (-2.f*counter + 1.f) : (2.f*counter - 1.f) ;
            break;
            
        case waveShape_sqr:
                output = counter >= simmetry ? 1.f : ((float)isBipolar) * -1;
            break;
            
        default:// ramp
            if( isBipolar )
                output = counter*2.f -1.f;
            else
                output = counter;
            break;
    }
    
    return output;
}

void LowFreqOsc::AdvanceCounter(){
    
    float increment = freq/(float)sampleRate;
    counter += increment;
    
    if(counter > 1.f)
        counter -= 1.f;
}

void LowFreqOsc::initLFO(int _sampleRate, float _frequency, waveShape _shape ){
    counter = 0;
    sampleRate = _sampleRate;
    freq = _frequency;
    shape = _shape;
}
#endif /* LowFreqOsc_hpp */
