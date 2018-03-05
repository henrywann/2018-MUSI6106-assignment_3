#include "MUSI6106Config.h"

#ifdef WITH_TESTS
#include <cassert>
#include <cstdio>
#include <algorithm>
#include <iostream>

#include "UnitTest++.h"

#include "Synthesis.h"
#include "Vector.h"
#include "ErrorDef.h"

#include "Vibrato.h"

SUITE(Vibrato)
{
    struct VibratoData
    {
        VibratoData()
        {
            // setup
            // e.g., allocate a vibrato object and test signal (newly created for each test case)
            iNumChannels = 2;
            iSampleRateInHz = 44100;
            iSigLenth = 4000;
            iBlockSize = 1024;
            fMaxDelayInSec = 0.2f;
            
            ppTestOutput = new float*[iNumChannels];
            ppTestInput = new float*[iNumChannels];
            for (int i = 0; i < iNumChannels; i++) {
                
                *(ppTestInput + i) = new float [iSigLenth];
                *(ppTestOutput + i) = new float [iSigLenth];
            }
            
            CVibrato::create(pcVibrato);
            
            
            pcVibrato->init(fMaxDelayInSec, iSampleRateInHz, iNumChannels, fWidth, fModFreq);
            pcVibrato->setParam(CVibrato::kParamWidth, fWidth);
            pcVibrato->setParam(CVibrato::VibratoParam_t::kParamModFreq, fModFreq);
        }
        
        ~VibratoData()
        {
            // teardown
            // e.g., deallocate the vibrato object and test signal
            for (int i = 0; i < iNumChannels; i++) {
                delete [] ppTestOutput[i];
                delete [] ppTestInput[i];
            }
            delete[] ppTestOutput;
            delete[] ppTestInput;
        }
        
        // e.g., a reusable process() function
        void vibratoProcess(float modfreq, float modwidth) {
            pcVibrato->setParam(CVibrato::VibratoParam_t::kParamWidth, modwidth);
            pcVibrato->setParam(CVibrato::VibratoParam_t::kParamModFreq, modfreq);
            VibratoData::pcVibrato->process(ppTestInput, ppTestOutput, iSigLenth);
        }
        
        // e.g., a member vibrato object to be reused in each test
        CVibrato* pcVibrato;
        int iSampleRateInHz, iSigLenth, iNumChannels, iBlockSize;
        float fMaxDelayInSec, fWidth, fModFreq;
        float **ppTestInput = 0;
        float **ppTestOutput = 0;
        
        void resetBuffer() {
            for (int i = 0; i < iNumChannels; i++) {
                for (int j = 0; j < iSigLenth; j++) {
                    ppTestOutput[i][j] = 0;
                }
            }
        }
    };
    
    TEST_FIXTURE(VibratoData, ZeroModAmp) {
        
        // generate simple sine wave
        for (int i = 0; i < iNumChannels; i++) {
            CSynthesis::generateSine(ppTestInput[i], 100, iSampleRateInHz, iSigLenth);
        }
        resetBuffer();
        vibratoProcess(5, 0);

        for (int i = 0; i < iNumChannels; i++) {
            for (int j = static_cast<int>(round(fMaxDelayInSec*iSampleRateInHz)); j < iSigLenth; j++) {
                CHECK_CLOSE(ppTestInput[i][j- static_cast<int>(round((fMaxDelayInSec*iSampleRateInHz)))], ppTestOutput[i][j], 1e-3F);
            }
        }
    }
    
    TEST_FIXTURE(VibratoData, DCSignal) {

        // generate a DC signal
        for (int i = 0; i < iNumChannels; i++) {
            CSynthesis::generateDc(ppTestInput[i], iSigLenth);
        }

        resetBuffer();
        vibratoProcess(5, 0.005);

        float **ppTestOutputTemp = ppTestOutput;
        vibratoProcess(10, 0.0025);

        for (int i = 0; i < iNumChannels; i++) {
            CHECK_ARRAY_CLOSE(ppTestOutputTemp[i], ppTestOutput[i], iSigLenth, 1e-3F);
        }
    }
    
    //    TEST_FIXTURE(VibratoData, VaryingBlockSize) {
    //
    //    }

    TEST_FIXTURE(VibratoData, ZeroInput) {
        
        // generate 0 input
        for (int i = 0; i < iNumChannels; i++) {
            for (int j = 0; j < iSigLenth; j++) {
                ppTestInput[i][j] = 0;
            }
        }
        resetBuffer();
        vibratoProcess(5, 0.005);
        
        for (int i = 0; i < iNumChannels; i++) {
            CHECK_ARRAY_CLOSE(ppTestInput[i], ppTestOutput[i], iSigLenth, 1e-3);
        }
    }
    
    TEST_FIXTURE(VibratoData, myTest) {
        
    }

}

#endif //WITH_TESTS

