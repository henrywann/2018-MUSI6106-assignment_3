
#include <iostream>
#include <ctime>

#include "MUSI6106Config.h"

#include "AudioFileIf.h"
#include "Vibrato.h"

using std::cout;
using std::endl;

// local function declarations
void    showClInfo ();

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    std::string             sInputFilePath,                 //!< file paths
        sOutputFilePath;

    static const int        kBlockSize = 1024;

    clock_t                 time = 0;

    float                   **ppfAudioData = 0;
    float                   **ppfAudioOutData = 0;

    CAudioFileIf            *phAudioFile = 0;
    CAudioFileIf            *phAudioOutFile = 0;
    
    CAudioFileIf::FileSpec_t stFileSpec;
    CVibrato                 *pCVibrato = 0;
    int                      iNumChannels = 0;
    int                      iSampleRate = 44100;
    float                    fModWidth = 0.0025F;
    float                    fModFreq  = 10; //5-14Hz
    
    float                    fMaxDelayInSec = 0.02F; //5-10ms

    showClInfo();

    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    if (argc < 3)
    {
        cout << "Missing audio input path!";
        return -1;
    }
    else
    {
        sInputFilePath = argv[1];
        sOutputFilePath = argv[2];
    }

    //////////////////////////////////////////////////////////////////////////////
    // open the input wave file
    CAudioFileIf::create(phAudioFile);
    phAudioFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    if (!phAudioFile->isOpen())
    {
        cout << "Wave file open error!";
        return -1;
    }
    phAudioFile->getFileSpec(stFileSpec);
    
    // open the output wav file
    CAudioFileIf::create(phAudioOutFile);
    phAudioOutFile->openFile(sOutputFilePath, CAudioFileIf::kFileWrite, &stFileSpec);
    iNumChannels = stFileSpec.iNumChannels;
//    iSampleRate = stFileSpec.fSampleRateInHz;

//    //////////////////////////////////////////////////////////////////////////////
//    // open the output text file
//    hOutputFile.open(sOutputFilePath.c_str(), std::ios::out);
//    if (!hOutputFile.is_open())
//    {
//        cout << "Text file open error!";
//        return -1;
//    }
    
    //////////////////////////////////////////////////////////////////////////////
    // allocate memory
    ppfAudioData = new float*[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfAudioData[i] = new float[kBlockSize];
    
    ppfAudioOutData = new float*[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfAudioOutData[i] = new float[kBlockSize];
    
    CVibrato::create(pCVibrato);
    
    pCVibrato->init(fMaxDelayInSec, iSampleRate, iNumChannels, fModWidth, fModFreq);
    pCVibrato->setParam(CVibrato::VibratoParam_t::kParamModFreq, fModFreq);
    pCVibrato->setParam(CVibrato::VibratoParam_t::kParamWidth, fModWidth);

    time = clock();
    //////////////////////////////////////////////////////////////////////////////
    // get audio data and write it to the output file
    while (!phAudioFile->isEof())
    {
        long long iNumFrames = kBlockSize;
        phAudioFile->readData(ppfAudioData, iNumFrames);
        pCVibrato->process(ppfAudioData, ppfAudioOutData, iNumFrames);
        phAudioOutFile->writeData(ppfAudioOutData, iNumFrames);

        cout << "\r" << "reading and writing";

//        for (int i = 0; i < iNumFrames; i++)
//        {
//            for (int c = 0; c < stFileSpec.iNumChannels; c++)
//            {
//                hOutputFile << ppfAudioData[c][i] << "\t";
//            }
//            hOutputFile << endl;
//        }
    }

    cout << "\nreading/writing done in: \t" << (clock() - time)*1.F / CLOCKS_PER_SEC << " seconds." << endl;

    //////////////////////////////////////////////////////////////////////////////
    // clean-up
    CAudioFileIf::destroy(phAudioFile);
    CAudioFileIf::destroy(phAudioOutFile);

    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        delete[] ppfAudioData[i];
    delete[] ppfAudioData;
    ppfAudioData = 0;
    
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        delete[] ppfAudioOutData[i];
    delete ppfAudioOutData;
    ppfAudioOutData = 0;

    return 0;

}


void     showClInfo()
{
    cout << "GTCMT MUSI6106 Executable" << endl;
    cout << "(c) 2014-2018 by Alexander Lerch" << endl;
    cout  << endl;

    return;
}

