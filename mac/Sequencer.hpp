/* A sequencer with a different euclidean rhythm for each pattern and an internal sinewave LFO */

#ifndef Sequencer_hpp
#define Sequencer_hpp

#define NUM_SEQUENCES 8
#define NUM_STEP 16

class Sequencer {
    
    int CurrentStep, CurrentSeq, sampleRate;
    int Sequences [NUM_SEQUENCES] [NUM_STEP];
    int note[NUM_SEQUENCES];
    
    bool isSine;
    
    float frequency, counter, previewsOutput;
    
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
    
    CurrentStep++;
    if( CurrentStep >= NUM_STEP)
        CurrentStep = 0;
}

void Sequencer::advanceCounter(){
        
    float increment = frequency/(float)sampleRate;
    
    counter += isSine ? increment : increment*2.f;
    
    if(counter > 1.f){
        counter -= 1.f;
        advaceStep();
    }
}

void Sequencer::initSequencer(int _SampleRate){
    
    setSampleRate(_SampleRate);
    previewsOutput = 0.f;
    CurrentStep    = 0;
    CurrentSeq     = 0;
    frequency      = 1.f;
    counter        = 0.f;
    
    // Initializing each pattern with a different euclidean rhythm
    int num_Notes [NUM_SEQUENCES] = { 4, 6, 7, 9, 10, 11, 12, 13 };

    for (int i=0; i<NUM_SEQUENCES; i++)
        for(int j=0; j<NUM_STEP; j++){
        
            int a = (j * num_Notes[i]) % NUM_STEP;
            Sequences[i][j] = a < num_Notes[i] ? 1 : 0.001;
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
        else{
            previewsOutput -= 60/(float)sampleRate; // Fade out for the decrease of the signal
            output = previewsOutput> 0.001 ? previewsOutput : 0.001; // MAX(previewsOutput,0)
            
            previewsOutput = output;
        }
    }
        
    return output;
}

#endif /* Sequencer_hpp */
