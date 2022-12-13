//
//  WeirdDelay.hpp
//  GabrieleLeva
//
//  Created by Gabriele on 07/12/22.
//

#include <stdio.h>
#include "public.sdk/source/vst2.x/audioeffectx.h"
#include <memory>
#include "DelayLine.hpp"
#include "DSP.hpp"
#include "Filter.hpp"

using namespace std;


enum WeirdDelayParam{
    WDParam_DelayTime,
    WDParam_FeedBack,
    WDParam_Weirdness,
    WDParam_DryWet,
    WDParam_NumParam
};

//=========================================================================================================

class WeirdDelay : public AudioEffectX
{

    float DelayTime, FeedBack, Weirdness, DryWet;
    
    DelayLine DelayLine1, DelayLine2;
    
    Filter LP1, LP2, LP3, HP1;
    
    void InitPlugin();

public:

    WeirdDelay(audioMasterCallback audioMaster);
    ~WeirdDelay();
    
    virtual void processReplacing(float** inputs, float** outputs, VstInt32 sampleFrames) override;
    //virtual void processDoubleReplacing(double** inputs, double** outputs, VstInt32 sampleFrames) override;

    virtual void setParameter (VstInt32 index, float value) override;
    virtual float getParameter (VstInt32 index) override;
    virtual void getParameterLabel (VstInt32 index, char* label) override;
    virtual void getParameterDisplay (VstInt32 index, char* text) override;
    virtual void getParameterName (VstInt32 index, char* text) override;
    virtual bool getEffectName(char* name) override;
    virtual bool getVendorString(char* name) override;
    virtual bool getProductString(char* name) override;
};
