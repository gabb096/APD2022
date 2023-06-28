#include <stdio.h>
#include "public.sdk/source/vst2.x/audioeffectx.h"
#include <memory>
#include "DelayLine.hpp"
#include "Distorsions.hpp"
#include "Filter.hpp"
#include "Sequencer.hpp"

#define NUM_PROGRAMS 3

using namespace std;

enum VibLayParam{
    VDParam_DelayTime,
    VDParam_FeedBack,
    VDParam_Rate,
    VDParam_Pattern,
    VDParam_Saturation,
    VDParam_DryWet,
    VDParam_NumParam,
    VDParam_PingPong
};

struct VibLayPreset {
    float Pset_DelayTime, Pset_FeedBack, Pset_Rate, Pset_Pattern, Pset_Saturation, Pset_DryWet;
    bool  PsetPingPong;
    char  nome[kVstMaxProgNameLen];
};


//====================================================================

class VibLay : public AudioEffectX{

    float DelayTime, FeedBack, Rate, Pattern, Saturation, DryWet;
    bool  pingPong;
    int   index;
    
    VibLayPreset presets[NUM_PROGRAMS];
    DelayLine DelayLine;
    Sequencer Seq;
    Filter LP;

    void InitPrograms();
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
    
    virtual bool getProgramNameIndexed (VstInt32 category, VstInt32 index, char* text) override;
    virtual void setProgram (VstInt32 program) override;

};
