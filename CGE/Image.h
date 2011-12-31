#ifndef IMAGE_H
#define IMAGE_H

#include "Graphics.h"
#include "Font.h"

namespace CGE
{
    class Image
    {
        public:
            Image();
            Image(const char* inFile);
            Image(int inWidth, int inHeight);
            Image(const Image& inImage);
            ~Image();

            inline bool isValid() const { return !!mData; }
            void powersOfTwoRectangleFrom(const Image& inImage);
            void loadFile(const char* inFile);
            void loadText(const Font& inFont, const char* inText,
                Uint8 inRed = 255, Uint8 inGreen = 255, Uint8 inBlue = 255);

            void blitOnto(Image& inImage) const;
            void loadIntoTexture(GLenum inTarget = GL_TEXTURE_2D) const;
            void setAsWindowIcon() const;
            int width() const;
            int height() const;

        private:
            void destroy();
            void findFormat();

            Surface mData;
            int mColors;
            GLenum mFormat;
    };
}

#endif
