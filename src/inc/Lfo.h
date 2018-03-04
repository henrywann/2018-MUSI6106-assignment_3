#if !defined(__Lfo_hdr__)
#define __Lfo_hdr__

#define _USE_MATH_DEFINES
#include <math.h>

#include "ErrorDef.h"
#include "Synthesis.h"

class CLfo
{
public:
    CLfo(float fSampleRate, float fModFreq)
//    m_fSampleRate(fSampleRate),
//    m_fModFreq(fModFreq)
    {
        m_fSampleRate = fSampleRate;
        m_fModFreq = fModFreq;
        m_iLfoBufferLength = round(m_fSampleRate/m_fModFreq);
        m_pCRingBuffer = new CRingBuffer<float>(m_iLfoBufferLength);
        float fwaveTable[m_iLfoBufferLength];
        CSynthesis::generateSine(fwaveTable, m_fModFreq, m_fSampleRate, m_iLfoBufferLength);
        for (int i = 0; i< m_iLfoBufferLength; i++) {
            m_pCRingBuffer->putPostInc(fwaveTable[i]);
        }
    }
    
    ~CLfo() {
        if (m_pCRingBuffer) {
            delete m_pCRingBuffer;
        }
    }
    
    Error_t setLfoModFreq(float fmodFreq) {
        m_fModFreq = fmodFreq;
        return kNoError;
    }
    
    float getLfo() {
        return m_pCRingBuffer->getPostInc();
    }
    
private:
    float m_fSampleRate;
    float m_fModFreq;
    CRingBuffer<float>  *m_pCRingBuffer;
    int m_iLfoBufferLength;
};

#endif // __Lfo_hdr__
