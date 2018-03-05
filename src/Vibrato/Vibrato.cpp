
// standard headers

// project headers
#include "MUSI6106Config.h"

#include "ErrorDef.h"

#include "RingBuffer.h"

#include "Lfo.h"
#include "Vibrato.h"

#include <iostream>


CVibrato::CVibrato():
    m_pCLfo(0),
    m_ppCRingBuffer(0),
    m_bIsInitialized(false),
    m_fSampleRate(0),
    m_iNumChannels(0)
{
//    // this never hurts
//    this->reset ();
}


CVibrato::~CVibrato ()
{
        for (int i = 0; i < m_iNumChannels; i++) {
            delete m_ppCRingBuffer[i];
        }
    delete [] m_ppCRingBuffer;
    
}

Error_t CVibrato::create(CVibrato*& pCVibrato) {
    pCVibrato = new CVibrato();
    return kNoError;
}

Error_t CVibrato::destroy(CVibrato*& pCVibrato) {
    if (pCVibrato) {
        delete[] pCVibrato;
    }
    return kNoError;
}

Error_t CVibrato::init(float iMaxDelayInSec, float fSampleRateInHz, int iNumChannels, float fModWidth, float fModFreq) {
    if(iMaxDelayInSec<fModWidth) {
        std::cout << "Invalid Input, max delay must be greater than mod width" << std::endl;
        exit(-1);
    }
    
    int iMaxDelayInSample = floor(iMaxDelayInSec*fSampleRateInHz);
    m_fSampleRate = fSampleRateInHz;
    m_iNumChannels = iNumChannels;
    m_afParam[VibratoParam_t::kParamWidth] = fModWidth;
    m_afParam[VibratoParam_t::kParamModFreq] = fModFreq;
    m_ppCRingBuffer = new CRingBuffer<float>*[iNumChannels];
    for (int i = 0; i < iNumChannels; i++) {
        m_ppCRingBuffer[i]  = new CRingBuffer<float>(iMaxDelayInSample*2+1);
        m_ppCRingBuffer[i]->setReadIdx(0);
        m_ppCRingBuffer[i]->setWriteIdx(m_afParam[VibratoParam_t::kParamModFreq]*m_fSampleRate);
    }
    m_pCLfo = new CLfo(m_fSampleRate, fModFreq);
    m_bIsInitialized = true;
    
    return kNoError;
}

Error_t CVibrato::setParam(CVibrato::VibratoParam_t eParam, float fParamValue) {
    switch(eParam) {
        case VibratoParam_t::kParamModFreq:
            m_afParam[eParam] = fParamValue;
            m_pCLfo->setLfoModFreq(fParamValue);
            break;
        case VibratoParam_t::kParamWidth:
            m_afParam[eParam] = fParamValue;
            break;
        default:
            return kNoError;
    }
    return kNoError;
}

float CVibrato::getParam(CVibrato::VibratoParam_t eParam) const {
    return m_afParam[eParam];
}

Error_t CVibrato::reset() {
    for (int i = 0; i<m_iNumChannels; i++) {
        m_ppCRingBuffer[i]->reset();
    }
    setParam(kParamWidth, 0.0025f);
    setParam(kParamModFreq, 5);
    m_bIsInitialized    = false;
    
    delete m_pCLfo;
    m_pCLfo = 0;
    return kNoError;
}


Error_t CVibrato::process(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames)
{
    for (int i = 0; i < m_iNumChannels; i++) {
        for (int j = 0; j < iNumberOfFrames; j++) {
            m_ppCRingBuffer[i]->putPostInc(ppfInputBuffer[i][j]);
            ppfOutputBuffer[i][j] = m_ppCRingBuffer[i]->get(m_afParam[VibratoParam_t::kParamWidth] * m_pCLfo->getLfo() * m_fSampleRate );
            m_ppCRingBuffer[i]->getPostInc();
        }
    }
    return kNoError;
}




