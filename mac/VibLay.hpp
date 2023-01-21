#include <stdio.h>
#include "public.sdk/source/vst2.x/audioeffectx.h"
#include <memory>
#include "DelayLine.hpp"
#include "Distorsions.hpp"
#include "Filter.hpp"
#include "LowFreqOsc.hpp"


using namespace std;


enum VibLayParam{
    VDParam_DelayTime,
    VDParam_FeedBack,
    VDParam_Rate,
    VDParam_Pattern,
    VDParam_DryWet,
    VDParam_NumParam
};

//====================================================================

class VibLay : public AudioEffectX{

    float DelayTime, FeedBack, Rate, Pattern, DryWet;
    int index;
        
    DelayLine DelayLine;
    
    Filter LP1;
    
    LowFreqOsc LFO1, LFO2;
    
    void InitPlugin();
    
    float denormParameters(VstInt32 index);

public:

    VibLay(audioMasterCallback audioMaster);
    ~VibLay();
    void init();
    
    virtual void processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames) override;
    //virtual void processDoubleReplacing(double** inputs, double** outputs, VstInt32 sampleFrames) override;
    
    virtual void setSampleRate (float sampleRate) override;
    virtual void setParameter (VstInt32 index, float value) override;
    virtual float getParameter (VstInt32 index) override;
    virtual void getParameterLabel (VstInt32 index, char* label) override;
    virtual void getParameterDisplay (VstInt32 index, char* text) override;
    virtual void getParameterName (VstInt32 index, char* text) override;
    virtual bool getEffectName(char* name) override;
    virtual bool getVendorString(char* name) override;
    virtual bool getProductString(char* name) override;
};
