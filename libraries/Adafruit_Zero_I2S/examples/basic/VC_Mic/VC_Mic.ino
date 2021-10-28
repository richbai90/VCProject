    #include <Arduino.h>
    #include <Adafruit_ZeroI2S.h>
    
    /*
     * From variants/metro_m4/variant.h
     * #define PIN_I2S_SDO  (8u)
     * #define PIN_I2S_SDI  (1u)
     * #define PIN_I2S_SCK  (3u)
     * #define PIN_I2S_FS   (9u)
     * #define PIN_I2S_MCK  (2u)
     */
    
    /*
     * Circuit Adafruit I2S MEMS Microphone Breakout (SPH0645LM4H):
     * SEL unconnected or connected to GND for left channel,
     *  connected to 3.3V for right channel
     * LRCL connected to PIN_I2S_FS (9)
     * DOUT connected to PIN_I2S_SDI (1)
     * BCLK connected to PIN_I2S_SCK (3)
     * GND connected GND
     * 3V connected to 3.3V
     */
    
    int32_t left;
    int32_t right;
    #define SAMPLERATE_HZ 22000
    #define DECIMATION    64
    
    Adafruit_ZeroI2S i2s;
    // a windowed sinc filter for 44 khz, 64 samples
uint16_t sincfilter[DECIMATION] = {0, 2, 9, 21, 39, 63, 94, 132, 179, 236, 302, 379, 467, 565, 674, 792, 920, 1055, 1196, 1341, 1487, 1633, 1776, 1913, 2042, 2159, 2263, 2352, 2422, 2474, 2506, 2516, 2506, 2474, 2422, 2352, 2263, 2159, 2042, 1913, 1776, 1633, 1487, 1341, 1196, 1055, 920, 792, 674, 565, 467, 379, 302, 236, 179, 132, 94, 63, 39, 21, 9, 2, 0, 0};

// a manual loop-unroller!
#define ADAPDM_REPEAT_LOOP_16(X) X X X X X X X X X X X X X X X X
    
    void setup()
    {
      Serial.begin(115200);
      while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
      }
     
      /* begin I2S on the default pins. 24 bit depth at
       * 44100 samples per second
       */
      i2s.begin(I2S_24_BIT, SAMPLERATE_HZ * DECIMATION / 16);
      i2s.enableRx();
    }
    
    void loop()
    {

      uint16_t runningsum = 0;
      uint16_t *sinc_ptr = sincfilter;
       i2s.read(&left, &right);
      uint16_t left1 = left;

  for (uint8_t samplenum=0; samplenum < (DECIMATION/16) ; samplenum++) {
     uint16_t sample = left1 & 0xFFFF;    // we read 16 bits at a time, by default the low half

     ADAPDM_REPEAT_LOOP_16(      // manually unroll loop: for (int8_t b=0; b<16; b++) 
       {
         // start at the LSB which is the 'first' bit to come down the line, chronologically 
         // (Note we had to set I2S_SERCTRL_BITREV to get this to work, but saves us time!)
         if (sample & 0x1) {
           runningsum += *sinc_ptr;     // do the convolution
         }
         sinc_ptr++;
         sample >>= 1;
      }
    )
  }
    runningsum /= 64 ;

    Serial.println(runningsum);
    
    }
