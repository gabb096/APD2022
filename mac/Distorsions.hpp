#ifndef Distorsions_hpp
#define Distorsions_hpp

#include <math.h>

class SlewLimiter{
    
    float slewOut, slewMin, slewMax;
    float sampleRate;
        
public:
    SlewLimiter();
    void setSampleRate(float _sampleRate);
    float processSample(float rise, float fall,float sample);
};

SlewLimiter::SlewLimiter(){
    
    sampleRate = 441000;
    slewMin = 0.1f;    // Minimum slope in volts per second
    slewMax = 1000.f; // Maximum slope in volts per second
    slewOut = 0;
}

void SlewLimiter::setSampleRate(float _sampleRate){
    sampleRate = _sampleRate;
}

float SlewLimiter::processSample(float rise, float fall,float sample){

    float Ts = 1/sampleRate;

    float slewRise = slewMax * Ts * pow(slewMin/slewMax, rise);
    float slewFall = slewMax * Ts * pow(slewMin/slewMax, fall);

    float input = sample;
  
   if(input > slewOut)                  // rise limiting
       if(input < slewOut+slewRise)     // min(input, out+slewRise)
           slewOut = input;
       else
           slewOut += slewRise;
   else                             // fall limiting
       if(input < slewOut+slewFall)     // max(input, out-slewFall)
           slewOut += slewFall;
       else
           slewOut = input;
    
    return slewOut;
}

float saturation(float input){ // based on the approximation of tanh(x)

    float sample = input;
    
    if(sample < -3 )
        return -1;
    
    else if(sample > 3 )
        return 1;
    
    else
    {
        float num = sample * (27.f + sample * sample);
        float den = 27.f + 9.f * sample * sample;
        
        return num/den;
    }
}

float wavefolding(float preGain, float sample){
    
    sample *= preGain;
    
    int iter = sample >= 0.0 ? ceil(sample) : -1*floor(sample);
    
    for(int i=0; i < iter; i++)
    {
        if(sample > 1)
            sample = 2 - sample;
        else if(sample < -1)
            sample = -2 - sample;
    }

    return sample;
}

float sinefolding(float gain, float depth, float freq, float input){
    
    /* Sine fast approx based on https://gist.github.com/geraldyeo/988116 */
    
    float sin;
    float sample  = input*freq;
    
    while(sample < -3.14159265)
        sample += 6.28318531;
    
    while(sample >  3.14159265)
        sample -= 6.28318531;

    // 1.27323954 = 4/pi, 0.405284735 = (2/pi)^2
    if (sample < 0.0)
        sin = sample * (1.27323954 + 0.405284735 * sample);
    else
        sin = sample * (1.27323954 - 0.405284735 * sample);
    
    return gain * input + depth * sin;
}

float clip(float gain, float sample, float negative, float positive){
    
    sample *= gain;
    
    if(sample > positive)
        sample = positive;
    else if(sample < negative)
        sample = negative;

    return sample;
}

#endif /* Distorsions_hpp */
