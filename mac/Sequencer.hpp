/* A sequencer with a different euclidean rhythm for each pattern and an internal sinewave LFO */

#ifndef Sequencer_hpp
#define Sequencer_hpp

#define NUM_SEQUENCES 8
#define NUM_STEP 16

class Sequencer {
    
    int previewsStep, CurrentStep, CurrentSeq, sampleRate;
    int Sequences [NUM_SEQUENCES] [NUM_STEP];
    int note[NUM_SEQUENCES];
    int p1, p2;
    
    bool isSine, pingPong;
    
    float frequency, counter, previewsOutput, previewsOutput1, previewsOutput2;
    
public:
    
    Sequencer();
    ~Sequencer();
    
    void setCurrentStep(int _newStep);
    void setIsSine(bool _isSine);
    void setSampleRate(int _SampleRate);
    void setFrequenccy(float _freq);
    void setSequence(int _sequence);
    
    void advaceStep();
    void advanceCounter();
    
    void initSequencer(int _SampleRate);
    
    float output();
    float ping();
    float pong();

};

Sequencer::Sequencer()  {    }
Sequencer::~Sequencer() {    }

void Sequencer::setCurrentStep(int _newStep)            { CurrentStep = _newStep;         }
void Sequencer::setIsSine(bool _isSine)                 { isSine = _isSine;               }
void Sequencer::setSampleRate(int _SampleRate)          { sampleRate = _SampleRate;       }
void Sequencer::setFrequenccy(float _freq)              { frequency  = _freq;             }

void Sequencer::setSequence(int _sequence){
    
    if (_sequence == 0)
        setIsSine(true);
    else{
        setIsSine(false);
        CurrentSeq = (_sequence - 1) % NUM_SEQUENCES;
    }
}

void Sequencer::advaceStep(){
    
    previewsStep = CurrentStep;
    CurrentStep++;
    
    if( CurrentStep >= NUM_STEP)
        CurrentStep = 0;
    
    if( ( Sequences[CurrentSeq][CurrentStep] - Sequences[CurrentSeq][previewsStep] ) == 1){
        p1 = (p1+1)%2;
        p2 = (p2+1)%2;
    }
}

void Sequencer::advanceCounter(){
        
    float increment = frequency/(float)sampleRate;
    
    counter += isSine ? increment : increment*2.f;
    
    if(counter > 1.f) {
        counter -= 1.f;
        advaceStep();
    }
}

void Sequencer::initSequencer(int _SampleRate){
    
    setSampleRate(_SampleRate);
    previewsOutput  = 0.f;
    previewsOutput1 = 0.f;
    previewsOutput2 = 0.f;
    previewsStep   = NUM_STEP-1;
    CurrentStep    = 0;
    CurrentSeq     = 0;
    frequency      = 1.f;
    counter        = 0.f;
    pingPong       = false;
    p1             = 0;
    p2             = 1;
    
    // Initializing each pattern with a different euclidean rhythm
    int num_Notes [NUM_SEQUENCES] = { 4, 6, 7, 9, 10, 11, 12, 13 };

    for (int i=0; i<NUM_SEQUENCES; i++)
        for(int j=0; j<NUM_STEP; j++){
        
            int a = (j * num_Notes[i]) % NUM_STEP;
            Sequences[i][j] = a < num_Notes[i] ? 1 : 0;
        }
}

float Sequencer::output(){
    
    float output = 0;
    
    if(isSine) // approximated using sin(pi*x) = 4x(1-x) in [0,1]
        output = 4.f * counter * (1.f - counter);
    else {
                
        if(Sequences[CurrentSeq][CurrentStep]){
            previewsOutput += 60/(float)sampleRate; // Fade in for the rise of the signal
            output = previewsOutput < 1.f ? previewsOutput : 1.f; // MIN(previewsOutput,1)
            
            previewsOutput = output;
        }
        else {
            previewsOutput -= 60/(float)sampleRate; // Fade out for the decrease of the signal
            output = previewsOutput> 0.001 ? previewsOutput : 0.001; // MAX(previewsOutput,0)
            
            previewsOutput = output;
        }
    }
        
    return output;
}

float Sequencer::ping(){

    float output = 0;
    
    if(isSine) // approximated using sin(pi*x) = 4x(1-x) in [0,1]
        output = 4.f * counter * (1.f - counter);
    else {
                
        if(Sequences[CurrentSeq][CurrentStep] * p1){
            previewsOutput1 += 60/(float)sampleRate; // Fade in for the rise of the signal
            output = previewsOutput1 < 1.f ? previewsOutput1 : 1.f; // MIN(previewsOutput,1)
            
            previewsOutput1 = output;
        }
        else{
            previewsOutput1 -= 60/(float)sampleRate; // Fade out for the decrease of the signal
            output = previewsOutput1> 0.001 ? previewsOutput1 : 0.001; // MAX(previewsOutput,0)
            
            previewsOutput1 = output;
        }
    }
        
    return output;
}

float Sequencer::pong(){
    
    float output = 0;
    
    if(isSine) // approximated using sin(pi*x) = 4x(1-x) in [0,1]
        output = 4.f * counter * (1.f - counter);
    else {
                
        if(Sequences[CurrentSeq][CurrentStep] * p2){
            previewsOutput2 += 60/(float)sampleRate; // Fade in for the rise of the signal
            output = previewsOutput2 < 1.f ? previewsOutput2 : 1.f; // MIN(previewsOutput,1)
            
            previewsOutput2 = output;
        }
        else{
            previewsOutput2 -= 60/(float)sampleRate; // Fade out for the decrease of the signal
            output = previewsOutput2> 0.001 ? previewsOutput2 : 0.001; // MAX(previewsOutput,0)
            
            previewsOutput2 = output;
        }
    }
        
    return output;
}

#endif /* Sequencer_hpp */
