//  DSP.hpp
//  GabrieleLeva
//
//  Created by Gabriele on 12/12/22.
//

#ifndef DSP_hpp
#define DSP_hpp

float saturation(float input) // based on the approximation of tanh(x)
{
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

float wavefolding(float preGain, float sample, int iterations)
{
    
    sample *= preGain;
    
    for(int i=0; i<iterations; ++i)
    {
        if(sample > 1)
            sample = 2 - sample;
        else if(sample < -1)
            sample = -2 - sample;
            
    }

    return sample;
}

float clip(float gain, float sample, float negative, float positive)
{
    sample *= gain;
    
    if(sample > positive)
        sample = positive;
    else if(sample < negative)
        sample = negative;

    return sample;
}


#endif /* DSP_hpp */
