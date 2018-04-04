/***** Scope.h *****/
#ifndef __Scope_H_INCLUDED__
#define __Scope_H_INCLUDED__ 

#include <ne10/NE10.h>
#include <vector>
#include <string>
#include <AuxTaskNonRT.h>
#include <AuxTaskRT.h>

#define FRAMES_STORED 4

#define TRIGGER_LOG_COUNT 16

/** 
 * \brief An oscilloscope which allows data to be visualised in a browser in real time.
 *
 * To use the scope, ensure the Bela IDE is running, and navigate to 
 * http://bela.local/scope
 */
class Scope{
    public:
        Scope();
		~Scope();
		
        /**
         * \brief Initialise the scope, setting the number of channels and the sample rate
         *
         * This function must be called once during setup. numChannels must be set
         * to the number of parameters passed in to log() or the channels may not be
         * displayed correctly. sampleRate must be the rate at which data is logged to
         * the scope (the rate at which log() is called) in Hz or the x-axis time values
         * displayed on the scope will be incorrect. numSliders is an optional parameter
         * which defaults to 0. Passing a non-zero value will create sliders in the 
         * scope's control panel whose values can be read with scope.getSliderValue().
         *
         * @param numChannels number of channels displayed by the scope.
         * @param sampleRate sample rate of the data passed in.
         * @param numSliders (optional) number of sliders to create (default is 0)
         */
        void setup(unsigned int numChannels, float sampleRate, int numSliders = 0);

        /** 
         * \brief Logs a frame of data to the scope.
         *
         * Pass one argument per channel (starting from the first), up to the
         * number of channels of the object.
         * Omitted values will be set to 0.
         */
        void log(float chn1, ...);

		/**
		 * \brief Logs a frame of data to the scope.
         *
         * Accepts a pointer to an array of floats representing each channel's value in
         * ascending order.
         *
         * @param values a pointer to an array containing numChannels values.
         */
        void log(const float* values);
        
        /** 
         * \brief Cause the scope to trigger when set to custom trigger mode.
         *
         * This method can be used to force the scope to trigger rather than relying on
         * the typical auto or normal trigger.
         */
        bool trigger();
        
        /** 
         * \brief Read the value of a slider
         *
         * If a slider was created in setup(), the value of that slider can be read at 
         * any time with this function. This allows values in your code to be adjusted in 
         * real-time from the scope's UI.
         * 
         * @param slider the index of the slider to read
         */
        float getSliderValue(int slider);
        
        /** 
         * \brief Check if the value of a slider has changed
         *
         * Check if the value of a slider has changed since the last time the value of that
         * slider was read using getSliderValue() 
         * 
         * @param slider the index of the slider to read
         */
        bool sliderChanged(int slider);
        
        /** 
         * \brief Initialise a slider's value and parameters
         *
         * This method can be used to set a slider's value and parameters, including
         * its minimum, maximum, step increment and name. This method is not safe to be
         * called on the audio thread and must be called in setup() only, and not render().
         * 
         * @param slider the index of the slider to set
         * @param min the minimum value of the slider
         * @param max the maximum value of the slider
         * @param step the step increment of the slider
         * @param value the initial value of the slider
         * @param name (optional) the name of the slider
         */
        void setSlider(int slider, float min, float max, float step, float value, std::string name = "Slider");
		
		/** 
         * \brief Initialise a slider's value and parameters
         *
         * This method can be used to set a slider's value and parameters, including
         * its minimum, maximum, step increment and name. This method is not safe to be
         * called on the audio thread and must be called in setup() only, and not render().
         * 
         * @param slider the index of the slider to set
         * @param min the minimum value of the slider
         * @param max the maximum value of the slider
         * @param step the step increment of the slider
         * @param value the initial value of the slider
         * @param name (optional) the name of the slider
         */
		void setTrigger(int mode, int channel, int dir, float level);
		
    private:

		void dealloc();
        void start();
        void stop();
        void triggerTimeDomain();
        void triggerFFT();
        bool triggered();
        bool prelog();
        void postlog();
        void setPlotMode();
        void doFFT();
        void setXParams();
        
		bool isUsingOutBuffer;
		bool isUsingBuffer;
		bool isResizing;
		
        // settings
        int numChannels;
        float sampleRate;
        int pixelWidth;
        int frameWidth;
        int plotMode;
        int triggerMode;
        int triggerChannel;
        int triggerDir;
        float triggerLevel;
        int xOffset;
        int xOffsetSamples;
        int upSampling;
        int downSampling;
        float holdOff;
        
        int logCount;
        
        int channelWidth;
        int downSampleCount;
        int holdOffSamples;
        
        // buffers
        std::vector<float> buffer;
        std::vector<float> outBuffer;
        
        // pointers
        int writePointer;
        int readPointer;
        int triggerPointer;
        int customTriggerPointer;
        
        // sliders
        int numSliders;
        struct ScopeSlider{
        	float value;
        	bool changed = false;
        };
        std::vector<ScopeSlider> sliders;
        
        // trigger status
        bool triggerPrimed;
        bool triggerCollecting;
        bool triggerWaiting;
        bool triggering;
        int triggerCount;
        int autoTriggerCount;
        bool started;
        bool settingUp;
        bool customTriggered;
        
        // FFT
        int FFTLength;
		int newFFTLength;
        float FFTScale;
		float FFTLogOffset;
        int pointerFFT;
        bool collectingFFT;
        float *windowFFT;
        int FFTXAxis;
        int FFTYAxis;
        
        ne10_fft_cpx_float32_t* inFFT;
        ne10_fft_cpx_float32_t* outFFT;
        ne10_fft_cfg_float32_t cfg;
        
        AuxTaskNonRT sendBufferTask;
        AuxTaskRT scopeTriggerTask;
        static void triggerTask(void* ptr);
		
		void setSetting(std::wstring setting, float value);
        friend struct ScopeControlHandler;
        
};

void scope_ws_setup(Scope* _scope);
void scope_ws_send(void* buf, int size);
void scope_ws_set_slider(int slider, float min, float max, float step, float value, std::string name);
void scope_ws_set_setting(std::wstring setting, float value);
void scope_ws_cleanup();

#endif
