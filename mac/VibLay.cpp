#include "VibLay.hpp"
#include <stdlib.h>

#define MIN_DELAY_TIME 0.01
#define MAX_DELAY_TIME 2.5

AudioEffect* createEffectInstance(audioMasterCallback audioMaster){
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

void VibLay::setSampleRate (float sampleRate){
    AudioEffect::setSampleRate(sampleRate);
    init();
}

void VibLay::InitPlugin(){
    DelayTime  = 0.7;
    FeedBack   = 0.4;
    Rate       = 0.2;
    Pattern    = 0.f;
    Saturation = 0.2;
    DryWet     = 0.5;
    index      = 0;
    
    init();
}

void VibLay::init(){
    
    DelayLine.initDelayLine( getSampleRate(), MAX_DELAY_TIME );
    
    Seq.initSequencer(getSampleRate());
    Seq.setFrequenccy(denormParameters(VDParam_Rate));
    Seq.setSequence(denormParameters(VDParam_Pattern));
    
    LP.InitFilter(getSampleRate(), 5000, FilterType_lp);
}

VibLay::~VibLay() {   }

//============================== VST PROCESSING ==============================

void VibLay::processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames){
    
    // PROCESS SINGLE PRECISION
    float *inL = inputs[0]; // buffer input left
    float *inR = inputs[1]; // buffer input right
    
    float *outL = outputs[0]; // buffer output left
    float *outR = outputs[1]; // buffer output right
   
    int sizeofdelayline = denormParameters(VDParam_DelayTime) * getSampleRate();
    float signal_L = 0;
    float signal_R = 0;

    for(int i=0; i<sampleFrames; ++i)
    {
        /* Delay Section */
        if(FeedBack !=0){
            signal_L = DelayLine.BufferL[index];
            signal_R = DelayLine.BufferR[index];

            DelayLine.BufferL[ index ] = (inL[i] + signal_L) * FeedBack;
            DelayLine.BufferR[ index ] = (inR[i] + signal_R) * FeedBack;
            
            signal_L += inL[i];
            signal_R += inR[i];
        }
        
        /* Sequencer Output*/
        if(Pattern != 0){
            signal_L *= Seq.output();
            signal_R *= Seq.output();
        }
        

        /* Distortion & Low Pass Filter*/
        if(Saturation != 0){
            float S = 1.f + 20 * Saturation;

            signal_L = saturation( signal_L * S);
            signal_R = saturation( signal_R * S);

            signal_L += wavefolding( 2.f, signal_L) * 0.06;
            signal_R += wavefolding( 2.f, signal_R) * 0.06;

            signal_L *= 0.4; // ~4 dB Pad
            signal_R *= 0.4; // ~4 dB Pad

            LP.processSample(signal_L);
            LP.processSample(signal_R);
        }
        
        /* OutputSignal -> Dry/Wet -> Clip Limiting*/
        outL[i] = clip(1.f, inL[i] * (1.f-DryWet) + signal_L * DryWet, -0.8, 0.95);
        outR[i] = clip(1.f, inR[i] * (1.f-DryWet) + signal_R * DryWet, -0.8, 0.95);
        
        /* Update Delay Line and Sequencer*/
        index ++;
        if( index >= sizeofdelayline )
            index -= sizeofdelayline;
        
        Seq.advanceCounter();
        
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
            return Pattern == 0 ? 1 : (int) (1 + Pattern * NUM_SEQUENCES);
            break;
            
        case VDParam_Rate:{
            float  k = 1.f/NUM_STEP;
            return k + Rate * (NUM_STEP - k);
            }
            break;
        
        default:
            return 0;
            break;
    }
}

void VibLay::setParameter (VstInt32 index, float value)
{
    switch (index) {
        case VDParam_DelayTime:
            DelayTime = value;
            break;
        
        case VDParam_Rate:
            Rate = value;
            Seq.setFrequenccy(denormParameters(VDParam_Rate));
            break;
            
        case VDParam_Pattern:
            Pattern = value;
            Seq.setSequence((int)(denormParameters(VDParam_Pattern)-1));
            break;
            
        case VDParam_FeedBack:
            FeedBack = value;
            break;
            
        case VDParam_Saturation:
            Saturation = value;
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
            
        case VDParam_Saturation:
            toReturn = Saturation;
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
            vst_strncpy(label, "Hz", kVstMaxParamStrLen);
            break;
            
        case VDParam_DelayTime:
            vst_strncpy(label, "S", kVstMaxParamStrLen);
            break;
            
        case VDParam_Saturation:
            vst_strncpy(label, "%", kVstMaxParamStrLen);
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
            if(FeedBack == 0)
                vst_strncpy(text, "Del Off",  kVstMaxParamStrLen);
            else
            int2string( FeedBack*100, text, kVstMaxParamStrLen);
            break;
            
        case VDParam_Pattern:
            if(Pattern == 0)
                vst_strncpy(text, "Var Off",  kVstMaxParamStrLen);
            else
                int2string( denormParameters(VDParam_Pattern), text, kVstMaxParamStrLen);
            break;
            
        case VDParam_Rate:
            float2string( denormParameters(VDParam_Rate), text, kVstMaxParamStrLen);
            break;
            
        case VDParam_DelayTime:
            
            if(DelayTime <= 0.5)
                float2string( denormParameters(VDParam_DelayTime), text, kVstMaxParamStrLen);
            else if (DelayTime <= 0.583)
                vst_strncpy(text, "1/2",  kVstMaxParamStrLen);
            else if (DelayTime <= 0.666)
                vst_strncpy(text, "1/4T", kVstMaxParamStrLen);  // triplets quarter note echo
            else if (DelayTime <= 0.750)
                vst_strncpy(text, "1/4",  kVstMaxParamStrLen);  // quarter note echo
            else if (DelayTime <= 0.833)
                vst_strncpy(text, "1/8D", kVstMaxParamStrLen);  // dotted eights note echo
            else if (DelayTime <= 0.916)
                vst_strncpy(text, "1/8",  kVstMaxParamStrLen);  //  eights note echo
            else
                vst_strncpy(text, "1/8T", kVstMaxParamStrLen);  // triplets eights note echo
        break;
            
        case VDParam_Saturation:
            if(Saturation == 0)
                vst_strncpy(text, "Sat Off",  kVstMaxParamStrLen);
            else
            int2string( Saturation*100, text, kVstMaxParamStrLen);
            break;
            
        case VDParam_DryWet:
            if(DryWet == 0)
                vst_strncpy(text, "Dry", kVstMaxParamStrLen);
            else if(DryWet == 1)
                vst_strncpy(text, "Wet", kVstMaxParamStrLen);
            else
                int2string( DryWet*100, text, kVstMaxParamStrLen);
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
            vst_strncpy(text, "Var[x]", kVstMaxParamStrLen);
            break;
            
        case VDParam_Saturation:
            vst_strncpy(text, "Saturn", kVstMaxParamStrLen);
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

