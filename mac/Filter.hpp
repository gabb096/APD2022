#include <stdio.h>
#include <math.h>

#define FILTER_ORDER 2

enum FilterType{
    FilterType_lp,
    FilterType_hp
};

class Filter{
    
    float a0, a1, b1, a2, b2;       // coefficienti del filtro
    float memIn[FILTER_ORDER];      // linea di ritardo per l'ingresso
    float memOut[FILTER_ORDER];     // linea di ritardo per l'uscita
    // È comodo avere il valore FILTER_ORDER perché possiamo cambiare dinamicamente il comportamento del plugin
    int sampleRate, counter;
    float freq, Q; // rispettivamente frequenza di taglio, risonanza
    
    FilterType type;
    
    void ComputeCoeff();
    
    
    public:
    Filter();
    void InitFilter(int _sampleRate, float _freq, FilterType _type);
    void setSampleRate(int _SampleRate);
    void setFreq(float _freq);
    void setQ(float _Q);
    void setType(FilterType _type);
    void Process(float* outL, float* outR, int sampleFrame);
    float processSample(float xn);

};

// ===========================================================================================

Filter::Filter()
{
    a0 = a1 = b1 = a2 = b2=0;
    
    for(int i=0; i<FILTER_ORDER; ++i)
    {
        memIn[i]  = 0;
        memOut[i] = 0;
    }
    
    counter = 0;
}

void Filter::InitFilter(int _sampleRate, float _freq, FilterType _type)
{
    sampleRate = _sampleRate;
    freq = _freq;
    type = _type;
    Q = 0.707;
    
    ComputeCoeff();

}

void Filter::setSampleRate(int _SampleRate)
{
    sampleRate = _SampleRate;
    ComputeCoeff();

}
void Filter::setFreq(float _freq)
{
    freq = _freq;
    ComputeCoeff();

}
void Filter::setQ(float _Q)
{
    Q = _Q;
    ComputeCoeff();

}
void Filter::setType(FilterType _type)
{
    type = _type;
    ComputeCoeff();
}

float Filter::processSample(float xn)
{
    float xn2 = memIn[counter];
    int counter_n1 = counter-1;
    
    if(counter_n1<0)
        counter_n1 += FILTER_ORDER;
    
    float xn1 = memIn[counter_n1];
    memIn[counter] = xn;

    float yn2 = memOut[counter];
    
    if(counter_n1<0)
        counter_n1 += FILTER_ORDER;
    
    float yn1 = memOut[counter_n1];
    
    float yn = a0*xn + a1*xn1 + a2*xn2 - b1*yn1 - b2*yn2;
    memOut[counter]=yn;
    counter= ++counter % FILTER_ORDER;
    
    return yn;
    
}

void Filter::Process(float* outL, float* outR, int sampleFrame)
{
    for(int i=0; i<sampleFrame;++i)
    {
        outL[i]=processSample(outL[i]);
        outR[i]=processSample(outR[i]);
    }
}

void Filter::ComputeCoeff()
{
    /* NOTA nelle slide la formula è 2*M_PI*freq/(float)sampleRate ma empiricamente si
     vede che la frequenza di taglio si trova al doppio di quella impostata */
    double teta = M_PI*freq/(double)sampleRate;
    double d = 1.0/Q;
    double betaNum = 1.0-((d/2.0)*sin(teta));
    double betaDen = 1.0+((d/2.0)*sin(teta));
    double beta = 0.5*(betaNum/betaDen);
    double gamma = (0.5+beta)*cos(teta);
    
    b1 = -2.0*gamma;
    b2 = 2.0*beta;
    
    /*I seguenti coefficienti variano in base al tipo di filtro*/
    
    switch (type) {
        case FilterType_lp:
            
            a0 = (0.5+beta-gamma)/2.0;
            a1 = 0.5+beta-gamma;
            break;
            
        case FilterType_hp:
            
            a0 = (0.5+beta+gamma)/2.0;
            a1 = -(0.5+beta+gamma);
            break;
            
        default:
            break;
    }
    
    a2 = a0;
}
