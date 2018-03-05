#if !defined(__Vibrato_hdr__)
#define __Vibrato_hdr__

#include "ErrorDef.h"

// forward declaration
class CLfo;
template <class T>
class CRingBuffer;

/*
 * Parameters are stored in an enum,VibratoParam_t. We want the client to have init, reset, setParam, getParam,
 * and process functionalities to the class CVibrato. Other variables and useful classes like CRingBuffer and CLfo
 * are stored in private. 
 */

class CVibrato
{
public:
    enum VibratoParam_t
    {
        kParamModFreq,                     //!< Modulation Frequency
        kParamDelay,                    //!< delay in seconds for specification of comb width
        kParamWidth,                    //!< Modulation width
        
        kNumVibratoParams
    };
    
    static Error_t create(CVibrato*& cpCVibrato);
    
    static Error_t destroy (CVibrato*& pCVibrato);
    
    
//    CVibrato (int iMaxDelayInFrames, int iNumChannels);
//    virtual ~CVibrato ();
    
    Error_t resetInstance ();
    
    Error_t init (float iMaxDelayInSec, float fSampleRateInHz, int iNumChannels, float fModWidth, float fModFreq);
    
    Error_t reset ();
    
    Error_t setParam (VibratoParam_t eParam, float fParamValue);
    
    float   getParam (VibratoParam_t eParam) const;
    
    Error_t process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames);
//    CVibrato ();
//    ~CVibrato ();
private:
    bool            m_bIsInitialized;   //!< internal bool to check whether the init function has been called
    float           m_fSampleRate;      //!< audio sample rate in Hz
    
    CRingBuffer<float> **m_ppCRingBuffer;
    float   m_afParam[CVibrato::kNumVibratoParams];
    CLfo    *m_pCLfo;
    int     m_iNumChannels;
    CVibrato();
    ~CVibrato();

};

#endif // #if !defined(__Vibrato_hdr__)
