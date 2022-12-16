#include "WeirdDelay.hpp"
#include <stdlib.h>

#define MIN_DELAY_TIME 0.01
#define MAX_DELAY_TIME 2.5

AudioEffect* createEffectInstance(audioMasterCallback audioMaster)
{
    return new WeirdDelay(audioMaster);
}

//============================== WeirdDelay FUNCTIONS ==============================

WeirdDelay::WeirdDelay(audioMasterCallback audioMaster)
: AudioEffectX(audioMaster, 0, WDParam_NumParam)    // n program, n parameters
{
    setNumInputs(2);         // stereo in
    setNumOutputs(2);        // stereo out
    setUniqueID('GLDL');     // identify
    InitPlugin();
}

void WeirdDelay::setSampleRate (float sampleRate)
{
    AudioEffect::setSampleRate(sampleRate);
    DelayLine_1.initDelayLine( getSampleRate(), MAX_DELAY_TIME );

    DelayLine_2.initDelayLine(getSampleRate(), MAX_DELAY_TIME/2);
    
    LP1.InitFilter(getSampleRate(), 1000, FilterType_lp);
    LP2.InitFilter(getSampleRate(),  300, FilterType_lp);
    LP3.InitFilter(getSampleRate(), 1000, FilterType_lp);
    HP1.InitFilter(getSampleRate(),  200, FilterType_hp);
}

void WeirdDelay::InitPlugin()
{
    DelayTime = 0.7;
    FeedBack  = 0.4;
    Weirdness = 0;
    DryWet    = 0.5;
    
    DelayLine_1.initDelayLine( getSampleRate(), MAX_DELAY_TIME );

    DelayLine_2.initDelayLine(getSampleRate(), MAX_DELAY_TIME/2);

    LP1.InitFilter(getSampleRate(), 1000, FilterType_lp);
    LP2.InitFilter(getSampleRate(),  300, FilterType_lp);
    LP3.InitFilter(getSampleRate(), 1000, FilterType_lp);
    HP1.InitFilter(getSampleRate(),  100, FilterType_hp);
    
}

WeirdDelay::~WeirdDelay()
{
    
}

float WeirdDelay::denormDelayTime()
{
    if(DelayTime <= 0.5)
        return MIN_DELAY_TIME + DelayTime * 2 * (MAX_DELAY_TIME-MIN_DELAY_TIME);
    else
    {
        float bpm = getTimeInfo(kVstTempoValid)->tempo;

        if (DelayTime <= 0.583)
            return 120/bpm; // half note echo
        else if (DelayTime <= 0.666)
                return 40/bpm;  // triplets quarter note echo
        else if (DelayTime <= 0.750)
                return 60/bpm;  // quarter note echo
        else if (DelayTime <= 0.833)
                return 45/bpm;  // dotted eights note echo
        else if (DelayTime <= 0.916)
                return 30/bpm;  //  eights note echo
        else
            return 20/bpm;  // triplets eights note echo
    }
}

//============================== VST PROCESSING ==============================

void WeirdDelay::processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames)
{
    // PROCESS SINGLE PRECISION
    
    float *inL = inputs[0]; // buffer input left
    float *inR = inputs[1]; // buffer input right
    
    float *outL = outputs[0]; // buffer output left
    float *outR = outputs[1]; // buffer output right
   
    int sizeofdelayline1 = denormDelayTime() * getSampleRate();
    int sizeofdelayline2 = MAX_DELAY_TIME/2  * getSampleRate();

    
    
    for(int i=0; i<sampleFrames; ++i)
    {

        /* Main_Delay_Output -> LowPass */
        float Signal_L = DelayLine_1.BufferL[DelayLine_1.counter];
        Signal_L = LP1.processSample(Signal_L);

        float Signal_R = DelayLine_1.BufferR[DelayLine_1.counter];
        Signal_R = LP1.processSample(Signal_R);
        
        /* Second_Delay_Output -> LowPass -> Wavefolding */
        float SecSignal_L = DelayLine_2.BufferL[DelayLine_2.readHead];
        SecSignal_L = wavefolding(1, LP2.processSample(SecSignal_L), 1 );

        float SecSignal_R = DelayLine_2.BufferR[DelayLine_2.readHead];
        SecSignal_R = wavefolding(1, LP2.processSample(SecSignal_R), 1 );

        /* (Main_Delay_Output * Feedback + Direct_Input) -> Saturation -> Main_Delay_Input */
        Signal_L = saturation( inL[i] + (Signal_L * FeedBack) );
        Signal_R = saturation( inR[i] + (Signal_R * FeedBack) );

        DelayLine_1.BufferL[DelayLine_1.counter] = Signal_L;
        DelayLine_1.BufferR[DelayLine_1.counter] = Signal_R;
        
        /* Main_Delay_Input + Second_Delay_Output -> Second_Delay_Input */
        DelayLine_2.BufferL[DelayLine_2.writeHead] = (Signal_L + SecSignal_L) * 0.85;
        DelayLine_2.BufferR[DelayLine_2.writeHead] = (Signal_R + SecSignal_R) * 0.85;

        /* Second_Delay_Output -> LowPass * Weirdness */
        SecSignal_L = LP3.processSample(SecSignal_L) * Weirdness;
        SecSignal_R = LP3.processSample(SecSignal_R) * Weirdness;

        /* Summing stage */
        Signal_L += SecSignal_L;
        Signal_R += SecSignal_R;

        /* Main_Signal -> HighPass -> Saturation */
        Signal_L = saturation(HP1.processSample(Signal_L));
        Signal_R = saturation(HP1.processSample(Signal_R));
    
        /* Dry/Wet Direct_Input and Processed_Signal */
        outL[i] = inL[i]*(1-DryWet) + Signal_L*DryWet;
        outR[i] = inR[i]*(1-DryWet) + Signal_R*DryWet;
    
        
        /* Update delay lines */

//        DelayLine_1.counter ++;

        if( ++DelayLine_1.counter >= sizeofdelayline1)
            DelayLine_1.counter = 0;
        
        DelayLine_2.writeHead ++;
        DelayLine_2.readHead  --;
        
        if(DelayLine_2.writeHead >= sizeofdelayline2)
            DelayLine_2.writeHead = 0;
        
        if (DelayLine_2.readHead <= 0)
            DelayLine_2.readHead = sizeofdelayline2;

    }
}

// ============================== VST INTERFACE ==============================

void WeirdDelay::setParameter (VstInt32 index, float value)
{
    switch (index) {
        case WDParam_DelayTime:
            DelayTime = value;
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
            float2string(FeedBack*100, text, kVstMaxParamStrLen);
            break;
            
        case WDParam_Weirdness:
            float2string(Weirdness*100, text, kVstMaxParamStrLen);
            break;
            
        case WDParam_DelayTime:
            
            if(DelayTime <= 0.5)
                float2string(denormDelayTime(), text, kVstMaxParamStrLen);
            else if (DelayTime <= 0.583)
                vst_strncpy(text, "1/2", kVstMaxParamStrLen);
            else if (DelayTime <= 0.666)
                vst_strncpy(text, "1/4T", kVstMaxParamStrLen);  // triplets quarter note echo
            else if (DelayTime <= 0.750)
                vst_strncpy(text, "1/4", kVstMaxParamStrLen);  // quarter note echo
            else if (DelayTime <= 0.833)
                vst_strncpy(text, "1/8D", kVstMaxParamStrLen);  // dotted eights note echo
            else if (DelayTime <= 0.916)
                vst_strncpy(text, "1/8", kVstMaxParamStrLen);  //  eights note echo
            else
                vst_strncpy(text, "1/8T", kVstMaxParamStrLen);  // triplets eights note echo
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

