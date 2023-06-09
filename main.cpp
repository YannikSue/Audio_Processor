#include <stdlib.h>
#include <stdio.h>
#include <portaudio.h>
#include <cstring>

#define SAMPLE_RATE 48000.000000
#define FRAMES_PER_BUFFER 512

static void checkErr(PaError err)
{

    if (err != paNoError)
    {
        printf("PortAudio Error: %s\n", Pa_GetErrorText(err));
    }
}

static inline float max(float a, float b)
{

    return a > b ? a : b;
}

static inline float absolute(float a)
{
    return a > 0 ? a : -a;
}

static int paTestCallback(const void *inputBufffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *timeinfo, PaStreamCallbackFlags statusFlags, void *userData)
{

    float *input = (float *)inputBufffer;
    (void)outputBuffer;

    // '█`, ``, ``

    int displaySize = 100;
    printf("\r");

    float volume_left = 0;
    float volume_right = 0;

    for (unsigned long i = 0; i < framesPerBuffer * 2; i += 2)
    {

        volume_left = max(volume_left, absolute(input[i]));
        volume_right = max(volume_right, absolute(input[i + 1]));
    }

    for (int i = 0; i < displaySize; i++)
    {

        float barProportaion = i / (float)displaySize;
        if(barProportaion <= volume_left && barProportaion <=volume_right)
            printf("█");
        else if(barProportaion <= volume_left){
            printf("▀");
        }
        else if(barProportaion <= volume_right)
            printf("▄");
        else{
            printf(" ");
        }
    }

    fflush(stdout);

    return EXIT_SUCCESS;
}

int main()
{
    PaError err;

    // Initialize the PA Instance
    err = Pa_Initialize();
    checkErr(err);

    // Retrieves the current number of available audio devices
    int numDevices = Pa_GetDeviceCount();
    if (numDevices < 0)
    {

        printf("Error getting device count!");
        return EXIT_FAILURE;
    }
    else if (numDevices == 0)
    {

        printf("There are no available audio devices on this machine!");
        return EXIT_SUCCESS;
    }

    // Iterates through available devices and prints out basic information about the devices
    const PaDeviceInfo *deviceInfo;

    for (int i = 0; i < numDevices; i++)
    {
        deviceInfo = Pa_GetDeviceInfo(i);
        printf("Device %d: \n", i);
        printf("\t name: %s \n", deviceInfo->name);
        printf("\t maxInputChannels: %d\n", deviceInfo->maxInputChannels);
        printf("\t maxOutputChannels: %d\n", deviceInfo->maxOutputChannels);
        printf("\t defaultSampleRate: %f\n", deviceInfo->defaultSampleRate);
    }

    // should be selectable by the user later on
    int device = 6;
    printf("You selected following device: %s", Pa_GetDeviceInfo(device)->name);

    // Initializes the streams and sets the memory to zero to preventing unwanted side effects
    PaStreamParameters inputParameters;
    PaStreamParameters outputParameters;

    memset(&inputParameters, 0, sizeof(inputParameters));
    inputParameters.channelCount = 1;
    inputParameters.device = 9;
    inputParameters.hostApiSpecificStreamInfo = NULL;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(device)->defaultLowInputLatency;

    memset(&outputParameters, 0, sizeof(outputParameters));
    outputParameters.channelCount = 2;
    outputParameters.device = device;
    outputParameters.hostApiSpecificStreamInfo = NULL;
    outputParameters.sampleFormat = paFloat32;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(device)->defaultLowInputLatency;

    // Opens the actual in and output stream
    PaStream *stream;
    err = Pa_OpenStream(
        &stream,
        &inputParameters,
        &outputParameters,
        SAMPLE_RATE,
        FRAMES_PER_BUFFER,
        paNoFlag,
        paTestCallback,
        NULL);

    checkErr(err);

    err = Pa_StartStream(stream);
    checkErr(err);

    Pa_Sleep(10 * 1000);

    err = Pa_StopStream(stream);
    checkErr(err);

    err = Pa_CloseStream(stream);
    checkErr(err);

    // Exit the function safely
    err = Pa_Terminate();
    checkErr(err);

    return EXIT_SUCCESS;
}