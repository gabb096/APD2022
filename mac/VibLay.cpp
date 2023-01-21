#include "VibLay.hpp"
#include <stdlib.h>

#define MIN_DELAY_TIME 0.01
#define MAX_DELAY_TIME 2.5

#define NUM_PATTERN 8
#define MAX_EXTEND  20

AudioEffect* createEffectInstance(audioMasterCallback audioMaster)
{
    return new VibLay(audioMaster);
}

//============================== WeirdDelay FUNCTIONS ==============================

VibLay::VibLay(audioMasterCallback audioMaster)
: AudioEffectX(audioMaster, 0, VDParam_NumParam)    // n program, n parameters
{
    setNumInputs(2);         // stereo in
    setNumOutputs(2);        // stereo out
    setUniqueID('GLDL');     // identify
    InitPlugin();
}

void VibLay::setSampleRate (float sampleRate)
{
    AudioEffect::setSampleRate(sampleRate);
    init();
}

void VibLay::InitPlugin()
{
    DelayTime = 0.7;
    FeedBack  = 0.4;
    Rate      = 0;
    Pattern   = 0;
    DryWet    = 0.5;
    index     = 0;
    
    init();
}

void VibLay::init(){
    
    DelayLine.initDelayLine( getSampleRate(), MAX_DELAY_TIME );
    
    LP1.InitFilter(getSampleRate(), 200, FilterType_lp);
    
    LFO1.initLFO(getSampleRate(), 2, waveShape_sin);
    LFO2.initLFO(getSampleRate(), 2, waveShape_sin);
}

VibLay::~VibLay()
{
    
}

//============================== VST PROCESSING ==============================

void VibLay::processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames)
{
    // PROCESS SINGLE PRECISION
    float *inL = inputs[0]; // buffer input left
    float *inR = inputs[1]; // buffer input right
    
    float *outL = outputs[0]; // buffer output left
    float *outR = outputs[1]; // buffer output right
   
    int sizeofdelayline = denormParameters(VDParam_DelayTime) * getSampleRate();
    float signal_L = 0;
    float signal_R = 0;
    float lfo      = 0;

    for(int i=0; i<sampleFrames; ++i)
    {
        
        /* Delay Section */
        signal_L = DelayLine.BufferL[index];
        signal_R = DelayLine.BufferR[index];

        DelayLine.BufferL[ index ] = (inL[i] + signal_L) * FeedBack;
        DelayLine.BufferR[ index ] = (inR[i] + signal_R) * FeedBack;
        
        signal_L += inL[i];
        signal_R += inR[i];

        /* LFO Section*/
        lfo = LFO1.GetOutputValue() + LFO2.GetOutputValue();
        
        if (Pattern != 0)
            lfo = (lfo >= 0.75) ? 1 : 0;

        lfo = LP1.processSample(lfo);

        /* Modulation Section */
        signal_L *= lfo;
        signal_R *= lfo;

        /* Distortion section */
        signal_L = saturation(signal_L * 1.3);
        signal_R = saturation(signal_R * 1.3);

        /* OutputSignal -> Dry/Wet -> Clip Limiting*/
        outL[i] = clip(1.f, inL[i] * (1.f-DryWet) + signal_L * DryWet, -1.f, 1.f);
        outR[i] = clip(1.f, inR[i] * (1.f-DryWet) + signal_R * DryWet, -1.f, 1.f);
        
//        outL[i] = lfo;
//        outR[i]=  lfo;
        /* Update delay line and LFOs*/

        LFO1.AdvanceCounter();
        LFO2.AdvanceCounter();
        
        index ++;
        if( index >= sizeofdelayline )
            index -= sizeofdelayline;
        
    }
}

// ============================== VST INTERFACE ==============================

float VibLay::denormParameters(VstInt32 index)
{
    switch (index) {
        case VDParam_DelayTime:

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
            break;
        
        case VDParam_Pattern:
            return 1.0 + 0.1 * floor(Pattern * NUM_PATTERN);
            break;
            
        case VDParam_Rate:
            return denormParameters(VDParam_DelayTime) * Rate * MAX_EXTEND;
            break;
        
        default:
            break;
    }
}


void VibLay::setParameter (VstInt32 index, float value)
{
    switch (index) {
        case VDParam_DelayTime:
            DelayTime = value;

            LFO1.SetFrequenccy( denormParameters(VDParam_Rate) );
            LFO2.SetFrequenccy( LFO1.getFrequency() * denormParameters(VDParam_Pattern));
            break;
        
        case VDParam_Rate:
            Rate = value;
            LFO1.SetFrequenccy( denormParameters(VDParam_Rate) );
            LFO2.SetFrequenccy( LFO1.getFrequency() * denormParameters(VDParam_Pattern));
            break;
            
        case VDParam_Pattern:
            Pattern = value;
            LFO2.SetFrequenccy( LFO1.getFrequency() * denormParameters(VDParam_Pattern));
            break;
            
        case VDParam_FeedBack:
            FeedBack = value;
            break;
        
        case VDParam_DryWet:
            DryWet = value;
            break;

        default:
            break;
    }
}

float VibLay::getParameter (VstInt32 index)
{
    float toReturn = 0;
    
    switch (index) {
        case VDParam_FeedBack:
            toReturn = FeedBack;
            break;
            
        case VDParam_Rate:
            toReturn = Rate;
            break;
            
        case VDParam_Pattern:
            toReturn = Pattern;
            break;
            
        case VDParam_DelayTime:
            toReturn = DelayTime;
            break;
            
        case VDParam_DryWet:
            toReturn = DryWet;
            break;
            
        default:
            break;
    }
    
    return toReturn;
}

void VibLay::getParameterLabel (VstInt32 index, char* label)
{
    switch (index) {
        case VDParam_FeedBack:
            vst_strncpy(label, "%", kVstMaxParamStrLen);
            break;
            
        case VDParam_Rate:
            vst_strncpy(label, "Hz", kVstMaxParamStrLen);
            break;
            
        case VDParam_Pattern:
            vst_strncpy(label, "P", kVstMaxParamStrLen);
            break;
            
        case VDParam_DelayTime:
            vst_strncpy(label, "S", kVstMaxParamStrLen);
            break;
            
        case VDParam_DryWet:
            vst_strncpy(label, "%", kVstMaxParamStrLen);
            break;
            
        default:
            AudioEffect::getParameterLabel(index, label); // nel caso base chiamo la funzione del SDK
            break;
    }
}

void VibLay::getParameterDisplay (VstInt32 index, char* text)
{
    switch (index) {
        case VDParam_FeedBack:
            int2string(FeedBack*100, text, kVstMaxParamStrLen);
            break;
            
        case VDParam_Pattern:
            int2string(Pattern * NUM_PATTERN + 1, text, kVstMaxParamStrLen);
            break;

            
        case VDParam_Rate:
            float2string(denormParameters(VDParam_Rate), text, kVstMaxParamStrLen);
            break;
            
        case VDParam_DelayTime:
            
            if(DelayTime <= 0.5)
                float2string(denormParameters(VDParam_DelayTime), text, kVstMaxParamStrLen);
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
            
        case VDParam_DryWet:
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

void VibLay::getParameterName (VstInt32 index, char* text)
{
    switch (index) {
        case VDParam_DelayTime:
            vst_strncpy(text, "Time", kVstMaxParamStrLen);
            break;
            
        case VDParam_FeedBack:
            vst_strncpy(text, "FeedBack", kVstMaxParamStrLen);
            break;
            
        case VDParam_Rate:
            vst_strncpy(text, "Rate", kVstMaxParamStrLen);
            break;
            
        case VDParam_Pattern:
            vst_strncpy(text, "Pattern", kVstMaxParamStrLen);
            break;
            
        case VDParam_DryWet:
            vst_strncpy(text, "DryWet", kVstMaxParamStrLen);
            break;
            
        default:
            break;
    }
}

bool VibLay::getEffectName(char *name){
    
    vst_strncpy(name, "Viblay", kVstMaxEffectNameLen);
    return true;
}

bool VibLay::getVendorString(char *name){
    
    vst_strncpy(name, "MIS Gabriele Leva", kVstMaxVendorStrLen);
    return true;
}

bool VibLay::getProductString(char *name){
 
    vst_strncpy(name, "VBL", kVstMaxProductStrLen);
    return true;
}

