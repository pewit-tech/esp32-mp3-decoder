# What is this?

**This is work in progress!**

This is a port of ESP8266_MP3_DECODER to the Espressif ESP32 chip. Portions of this file come from the Espressif [ESP8266_MP3_DECODER](https://github.com/espressif/ESP8266_MP3_DECODER) project.

For more details check the Pewit Ltd. [ESP32 I2S project page](https://www.pewit.co.uk/esp32/i2s/).

# I2S MP3 webradio streaming example

This is an example of how to use the I2S module inside the ESP32 to output
sound. In this case, it is used to output decoded MP3 data (actually, more 
accurately: MPEG2 layer III data): the code described here basically is a 
webradio streamer which can connect to an Icecast server, take the MP3 data 
the server sends out, decode it and output it over the I2S bus to a DAC. The
MP3 decoder has been tested for bitrates up to 320KBit/s and sample
rates of up to 48KHz.

## Configuration options, building

All high-level options can be configured in mp3/user/playerconfig.h. Edit 
that file to set up your access point and a webradio stream or other source of
MP3 data served over HTTP.

## Needed hardware

If you want to have nice, high-quality buffered audio output, you will need to
connect one IC I2S codec. The IC is 
optional, but you will get stuttering and low-quality sound if you leave it
off.

For the I2S codec, pick whatever chip or board works for you; this code was 
written using a MAX98357A chip, but other I2S boards and chips will probably
work as well. The connections to make here are:

```
ESP pin   - I2S signal
----------------------
GPIO17   - DATA
GPIO18   - LRCK
GPIO19   - BCLK
```

Also, don't forget to hook up any supply voltages and grounds needed.

## Running without the I2S DAC

To not use an I2S DAC chip, please edit mp3/user/playerconfig.h and
define PWM_HACK. This uses some code to abuse the I2S module as a
5-bit PWM generator. You can now connect an amplifier to the I2S
data pin (GPIO17) of the ESP module. Connecting a speaker 
directly may also work but is not advised: the GPIOs of the ESP
are not meant to drive inductive loads directly.

## Sound quality

In the default configuration, this code will output 16-bit mono audio.
Stereo audio is possible but hasn't been implemented yet.Furthermore, the
ESP can decode a pretty wide range of bitrates: 96KBit to 320KBit
MP3s have been tested and seem to work fine. Sound quality in general
is not bad, but the scaling and clock adaption algorithms can use
some improvement if the decoder needs to output real high-quality
sound: Patches are welcome.

## About the FreeRTOS SDK used

The MP3 example is a very memory-sensitive piece of code: the MP3 decoder
uses almost all the RAM and the needed buffers for input and output
buffering take up the rest: when using no external SPI RAM, only a few 
bytes of memory are left. The SDK libs that come with this example are
libraries that have been optimized for memory usage and are known to work.

## Technical details on this implementation

The biggest part of this code consists of a modified version of libmad,
a fixed-point mp3 decoder. The specific version we use here has already
been modified by NXP to use less memory 
(source: www.nxp.com/documents/application_note/AN10583.pdf) and has been
massaged by Espressif to store as much constants in flash as possible in
order to decrease RAM use even more. The MP3 decoder is fed from a FIFO
realized in RAM. This RAM is filled from a network
socket in a separate thread.

On the output side, the MP3 samples are fed into the I2S subsystem using 
DMA. The I2S DMA basically consists of a circular buffer consisting of
a number of smaller buffers. As soon as the DMA is done emptying one of
the smaller buffers into the I2S subsystem, it will fire an interrupt. This
interrupt will put the buffer address in a queue.

When the MP3 decoder has a bunch of samples ready, it will pop a buffer 
off this queue and put the samples in it until it is full, then take the
next buffer etc. The MP3 decoder generally is faster than the I2S output,
so at a certain moment there will be no free buffers left. The queue
system of FreeRTOS will suspend the mp3 decoding task when that
happens, allowing the ESP8266 to attend to other tasks.

# License/legal

The Espressif support code is licensed under the MIT license, rest of the functions are licensed under Apache license. All 
trademarks, service marks, trade names and product names appearing in these files are the property of 
their respective owner(s).