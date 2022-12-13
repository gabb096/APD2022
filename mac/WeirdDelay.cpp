#include "WeirdDelay.hpp"
#include <stdlib.h>

#define MIN_DELAY_TIME 0.001
#define MAX_DELAY_TIME 2.5

AudioEffect* createEffectInstance(audioMasterCallback audioMaster)
{
    return new WeirdDelay(audioMaster);
}

//========= WeirdDelay FUNCTIONS =======================================================================================

WeirdDelay::WeirdDelay(audioMasterCallback audioMaster)
: AudioEffectX(audioMaster, 0, WDParam_NumParam)    // n program, n parameters
{
    setNumInputs(2);         // stereo in
    setNumOutputs(2);        // stereo out
    setUniqueID('GLDL');     // identify
    InitPlugin();
}

void WeirdDelay::InitPlugin()
{
    DelayTime = 0.1;
    FeedBack = 0;
    Weirdness = 0;
    DryWet = 0;
    
    DelayLine1.initDelayLine(getSampleRate(), MAX_DELAY_TIME );
    DelayLine2.initDelayLine(getSampleRate(), MAX_DELAY_TIME*2 );

    LP1.InitFilter(getSampleRate(), 1000, FilterType_lp);
    LP2.InitFilter(getSampleRate(), 1000, FilterType_lp);
    LP3.InitFilter(getSampleRate(), 1000, FilterType_lp);
    HP1.InitFilter(getSampleRate(),  100, FilterType_hp);
    
}

WeirdDelay::~WeirdDelay()
{
    
}

//========= VST PROCESSING =======================================================================================

void WeirdDelay::processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames)
{
    // PROCESS SINGLE PRECISION
    
    float *inL = inputs[0]; // buffer input left
    float *inR = inputs[1]; // buffer input right
    
    float *outL = outputs[0]; // buffer output left
    float *outR = outputs[1]; // buffer output right
        
    int delayLineSize = DelayTime*getSampleRate();
    
    for(int i=0; i<sampleFrames; ++i)
    {
        outL[i] = inL[i];
        outR[i] = inR[i];
        
        float oldestSampleL = DelayLine1.BufferL[DelayLine1.counter];
        float oldestSampleR = DelayLine1.BufferR[DelayLine1.counter];
        
        LP1.processSample(oldestSampleL);
        LP1.processSample(oldestSampleR);
        
        DelayLine1.BufferL[DelayLine1.counter] = saturation( inL[i] + (oldestSampleL * FeedBack) );
        DelayLine1.BufferR[DelayLine1.counter] = saturation( inR[i] + (oldestSampleR * FeedBack) );
        
        /* TO-DO  [4] Compute the "weird" algorithm / delay read */

        /* TO-DO  [5.1] filter weird Line output with a 1 pole LowPass @ f_cut = 1/DelayTime */
        
        /* TO-DO  [5.2] wavefold the [5.1] signal and multiply by Weirdness */

        /* TO-DO  [6] saturate the sum of Main delay outuput and [5.2] signals */
        
        /* TO-DO  [7] saturate the output of [7] */

        /* TO-DO  [8] filter the output of [7] with a 1 pole HighPass @ 100 Hz */
        
        /* TO-DO  [9] Dry/Wet direct signal and [8] */
        
        outL[i] = oldestSampleL;
        outR[i] = oldestSampleR;
        
        DelayLine1.counter += 1;
        
        if(DelayLine1.counter > delayLineSize)
            DelayLine1.counter = 0;
        
        // Output stage
        outL[i] = inL[i]*(1-DryWet) + outL[i]*DryWet;
        outR[i] = inR[i]*(1-DryWet) + outL[i]*DryWet;
        
    }
}

//========= VST INTERFACE =======================================================================================

void WeirdDelay::setParameter (VstInt32 index, float value)
{
    switch (index) {
        case WDParam_DelayTime:
            DelayTime = MIN_DELAY_TIME + value * (MAX_DELAY_TIME-MIN_DELAY_TIME);
            break;
        
        case WDParam_FeedBack:
            FeedBack = value;
            break;
        
        case WDParam_Weirdness:
            Weirdness = value;
            break;
        
        case WDParam_DryWet:
            DryWet = value;
            break;

        default:
            break;
    }
}
float WeirdDelay::getParameter (VstInt32 index)
{
    float toReturn = 0;
    
    switch (index) {
        case WDParam_FeedBack:
            toReturn = FeedBack;
            break;
            
        case WDParam_Weirdness:
            toReturn = Weirdness;
            break;
            
        case WDParam_DelayTime:
            toReturn = DelayTime;
            break;
            
        case WDParam_DryWet:
            toReturn = DryWet;
            break;
            
        default:
            break;
    }
    
    return toReturn;
}
void WeirdDelay::getParameterLabel (VstInt32 index, char* label)
{
    switch (index) {
        case WDParam_FeedBack:
            vst_strncpy(label, "%", kVstMaxParamStrLen);
            break;
            
        case WDParam_Weirdness:
            vst_strncpy(label, "%", kVstMaxParamStrLen);
            break;
            
        case WDParam_DelayTime:
            vst_strncpy(label, "S", kVstMaxParamStrLen);
            break;
            
        case WDParam_DryWet:
            vst_strncpy(label, "%", kVstMaxParamStrLen);
            break;
            
        default:
            AudioEffect::getParameterLabel(index, label); // nel caso base chiamo la funzione del SDK
            break;
    }

}
void WeirdDelay::getParameterDisplay (VstInt32 index, char* text)
{
    switch (index) {
        case WDParam_FeedBack:
            float2string(FeedBack, text, kVstMaxParamStrLen);
            break;
            
        case WDParam_Weirdness:
            float2string(Weirdness*100, text, kVstMaxParamStrLen);
            break;
            
        case WDParam_DelayTime:
            float2string(DelayTime, text, kVstMaxParamStrLen);
            break;
            
        case WDParam_DryWet:
            if(DryWet == 0)
                vst_strncpy(text, "Dry", kVstMaxParamStrLen);
            else if(DryWet==1)
                vst_strncpy(text, "Wet", kVstMaxParamStrLen); 
            else
                float2string(DryWet*100, text, kVstMaxParamStrLen);
            break;
            
        default:
            break;
            
    }
}
void WeirdDelay::getParameterName (VstInt32 index, char* text)
{
    switch (index) {
        case WDParam_DelayTime:
            vst_strncpy(text, "Time", kVstMaxParamStrLen);
            break;
            
        case WDParam_FeedBack:
            vst_strncpy(text, "FeedBack", kVstMaxParamStrLen);
            break;
            
        case WDParam_Weirdness:
            vst_strncpy(text, "Weirdness", kVstMaxParamStrLen);
            break;
            
        case WDParam_DryWet:
            vst_strncpy(text, "DryWet", kVstMaxParamStrLen);
            break;
            
        default:
            break;
    }
}

bool WeirdDelay::getEffectName(char *name){
    
    vst_strncpy(name, "WeirdDelay", kVstMaxEffectNameLen);
    return true;
}

bool WeirdDelay::getVendorString(char *name){
    
    vst_strncpy(name, "MIS Gabriele Leva", kVstMaxVendorStrLen);
    return true;
}

bool WeirdDelay::getProductString(char *name){
 
    vst_strncpy(name, "WDLY", kVstMaxProductStrLen);
    return true;
}

