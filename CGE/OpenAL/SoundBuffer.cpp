#include "SoundBuffer.h"
#include "../Tools.h"
#include "../Memory.h"

#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisenc.h>
#include <vorbis/vorbisfile.h>

namespace CGE
{
    static Heap AudioHeap("Audio");

    SoundBuffer::SoundBuffer(const char* inFile) : mHandle(0)
    {
        // These just silence a few warnings. :P
        (void)OV_CALLBACKS_DEFAULT;
        (void)OV_CALLBACKS_NOCLOSE;
        (void)OV_CALLBACKS_STREAMONLY;
        (void)OV_CALLBACKS_STREAMONLY_NOCLOSE;

        if (isAudioActive())
        {
            alGenBuffers(1, &mHandle);
            loadFile(inFile);
        }
    }

    SoundBuffer::~SoundBuffer()
    {
        if (mHandle) alDeleteBuffers(1, &mHandle);
    }

    void SoundBuffer::loadFile(const char* inFile)
    {
        if (!inFile || !*inFile) return;

        const char* lastDot = NULL;
        size_t length = 0;
        for (const char* i = inFile; *i; ++i)
        {
            ++length;
            if (*i == '.') lastDot = i;
        }

        if (lastDot)
        {
            const char* extension = lastDot + 1;

            if (caseInsensitiveEquals(extension, "ogg"))
                loadOgg(inFile);
            else if (caseInsensitiveEquals(extension, "wav"))
                loadWav(inFile);
        }
        else
        {
            std::cerr << "found no audio file extension :(\n";
        }
    }

    void SoundBuffer::bindToSource(ALuint inSource) const
    {
        alSourcei(inSource, AL_BUFFER, mHandle);
    }

    void SoundBuffer::loadOgg(const char* inFile)
    {
        FILE* f = fopen(inFile, "rb");
        if (!f)
        {
            std::cerr << "failed to open audio file: " << inFile << '\n';
            return;
        }

        OggVorbis_File ovf;
        int result = ov_open(f, &ovf, NULL, 0);
        if (result < 0)
        {
            // We only need to close the file if and only if ov_open fails.
            // http://xiph.org/vorbis/doc/vorbisfile/ov_open.html
            fclose(f);
            std::cerr << "failed to open ogg file: " << inFile << '\n';
            return;
        }

        vorbis_info* vorbisInfo = ov_info(&ovf, -1);
        //vorbis_comment* vorbisComment = ov_comment(&ovf, -1);

        ALenum format = vorbisInfo->channels == 1 ? AL_FORMAT_MONO16
            : AL_FORMAT_STEREO16;

        const size_t BufferSize = 4096 * 64;
        void* chunk = allocate(BufferSize, AudioHeap);
        char* data = reinterpret_cast<char*>(chunk);

        int got = 0;
        int bytes = BufferSize;
        while (bytes > 0)
        {
            int section;
            result = ov_read(&ovf, data + got, bytes, 0, 2, 1, &section);

            if (result <= 0) break;

            bytes -= result;
            got += result;
        }

        alBufferData(mHandle, format, data, got, vorbisInfo->rate);

        release(chunk);
    }

    void SoundBuffer::loadWav(const char* inFile)
    {
        std::ifstream fin;
        fin.open(inFile, std::ifstream::binary);
        if (!fin)
        {
            std::cerr << "failed to open wav file: " << inFile << '\n';
            return;
        }

        std::cerr << "loading " << inFile << '\n';

        fin.seekg(0, std::ios_base::end);
        size_t size = fin.tellg();
        fin.seekg(0, std::ios_base::beg);

        if (size > 44)
        {
            char* buffer = (char*)allocate(size * sizeof(char), AudioHeap);
            fin.read(buffer, size);

            if (!memcmp(buffer, "RIFF", 4)
                && !memcmp(buffer + 8, "WAVEfmt ", 8))
            {
                std::cerr << "allegedly a valid wav file!\n";

                short format;
                readBytes(buffer + 20, format);

                short channels;
                readBytes(buffer + 22, channels);

                int sampleRate;
                readBytes(buffer + 24, sampleRate);

                int byteRate;
                readBytes(buffer + 28, byteRate);

                short bitsPerSample;
                readBytes(buffer + 34, bitsPerSample);

                std::cerr << "format == " << format
                    << "\nchannels == " << channels
                    << "\nsample rate == " << sampleRate
                    << "\nbyte rate == " << byteRate
                    << "\nbits per sample == " << bitsPerSample
                    << '\n';

                size_t offset = 0;
                if (bitsPerSample != 16)
                {
                    short excess;
                    readBytes(buffer + 36, excess);
                    offset = excess;
                }

                if (memcmp(buffer + 36 + offset, "data", 4))
                {
                    std::cerr << "invalid WAV format: missing 'data' marker\n";
                }
                else
                {
                    int dataChunkSize;
                    readBytes(buffer + 40 + offset, dataChunkSize);

                    if (dataChunkSize > 0)
                    {
                        std::cerr << "data chunk size == " << dataChunkSize
                            << '\n';

                        // I have to double the sample rate to make the sound
                        // play at regular speed. I don't know why. Online
                        // documentation makes no mention of needing such an
                        // adjustment.
                        alBufferData(mHandle, format == 1 ? AL_FORMAT_MONO16
                            : AL_FORMAT_STEREO16, buffer + 44 + offset,
                            dataChunkSize, sampleRate * 2);
                    }
                    else
                    {
                        std::cerr << "invalid WAV format: "
                            "bad data chunk size\n";
                    }
                }
            }
            else
            {
                std::cerr << "invalid WAV format: missing 'RIFF' header\n";
            }

            release(buffer);
        }
        else
        {
            std::cerr << "bad wav file size: " << size << '\n';
        }

        fin.close();
    }
}
