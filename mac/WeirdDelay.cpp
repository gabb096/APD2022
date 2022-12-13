#include "WeirdDelay.hpp"
#include <stdlib.h>

#define MIN_DELAY_TIME 0.001
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

void WeirdDelay::InitPlugin()
{
    DelayTime = 0.25;
    FeedBack  = 0.3;
    Weirdness = 0;
    DryWet    = 0.5;
    
    DelayLine1.initDelayLine( getSampleRate(), MAX_DELAY_TIME );
    
    DelayLine2.initDelayLine( getSampleRate(), MAX_DELAY_TIME );
    DelayLine2.counter = 0;
    
    LP1.InitFilter(getSampleRate(), 1000, FilterType_lp);
    LP2.InitFilter(getSampleRate(),  300, FilterType_lp);
    LP3.InitFilter(getSampleRate(), 1000, FilterType_lp);
    HP1.InitFilter(getSampleRate(),  100, FilterType_hp);
    
}

WeirdDelay::~WeirdDelay()
{
    
}

//============================== VST PROCESSING ==============================

void WeirdDelay::processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames)
{
    // PROCESS SINGLE PRECISION
    
    float *inL = inputs[0]; // buffer input left
    float *inR = inputs[1]; // buffer input right
    
    float *outL = outputs[0]; // buffer output left
    float *outR = outputs[1]; // buffer output right
        
    DelayLine1.sizeOfDelayTime = DelayTime*getSampleRate();
    DelayLine2.sizeOfDelayTime = MAX_DELAY_TIME*getSampleRate()*2;

    for(int i=0; i<sampleFrames; ++i)
    {
//        outL[i] = inL[i];
//        outR[i] = inR[i];
// ============================== Main Delay Output + LowPass ==============================
        float Signal_L = DelayLine1.BufferL[DelayLine1.counter];
        Signal_L = LP1.processSample(Signal_L);

        float Signal_R = DelayLine1.BufferR[DelayLine1.counter];
        Signal_R = LP1.processSample(Signal_R);
        
// ============================== Secondary Delay Output + LowPass + Wavefolding ==============================
        float secondSignal_L = DelayLine2.BufferL[DelayLine2.counter];
        secondSignal_L = LP2.processSample(secondSignal_L);
        secondSignal_L = wavefolding(1.2, secondSignal_L, 1);

        float secondSignal_R = DelayLine2.BufferR[DelayLine2.counter];
        secondSignal_R = LP2.processSample(secondSignal_R);
        secondSignal_R = wavefolding(1.2, secondSignal_R, 1);

// Main_Delay_Output * Feedback + Direct_Input and saturation of their sum became the input of the Main_Delay_Line
        Signal_L = saturation( inL[i] + (Signal_L * FeedBack) );
        Signal_R = saturation( inR[i] + (Signal_R * FeedBack) );
        
        DelayLine1.BufferL[DelayLine1.counter] = Signal_L;
        DelayLine1.BufferR[DelayLine1.counter] = Signal_R;
        
// ============================== INVERTING LEFT AND RIGHT SIGNALS OF SECONDSIGNAL ==============================
        DelayLine2.BufferL[DelayLine2.counter] = (Signal_L + secondSignal_R)/2;
        DelayLine2.BufferR[DelayLine2.counter] = (Signal_R + secondSignal_L)/2;

// ============================== MainSignal + processed second signal ==============================
        Signal_L += LP3.processSample(secondSignal_L)*Weirdness;
        Signal_R += LP3.processSample(secondSignal_R)*Weirdness;
//
        
// ============================== Saturation of Main_Signal after HighPass @ 100Hz ==============================
        Signal_L = wavefolding(1.5, HP1.processSample(Signal_L) * (1+Weirdness), 1 );
        Signal_R = wavefolding(1.5, HP1.processSample(Signal_R) * (1+Weirdness), 1 );
        
// ============================== Update delay lines ==============================

        DelayLine1.counter += 1;
       
        if(DelayLine1.counter > DelayLine1.sizeOfDelayTime)
            DelayLine1.counter = 0;
        
        DelayLine2.counter += 1;

        if(DelayLine2.counter > DelayLine2.sizeOfDelayTime)
            DelayLine2.counter = 0;
        
// ============================== Dry/Wet Direct_Input and Processed_Signal ==============================
        outL[i] = inL[i]*(1-DryWet) + Signal_L*DryWet;
        outR[i] = inR[i]*(1-DryWet) + Signal_R*DryWet;
        
    }
}

// ============================== VST INTERFACE ==============================

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
            float2string(FeedBack*100, text, kVstMaxParamStrLen);
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

