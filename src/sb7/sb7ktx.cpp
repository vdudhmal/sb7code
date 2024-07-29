/*
 * Copyright � 2012-2015 Graham Sellers
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "sb7ktx.h"

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS 1
#endif /* _MSC_VER */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

// #include <sb7.h>

#include "GL/gl3w.h"

namespace sb7
{

namespace ktx
{

namespace file
{

static const unsigned char identifier[] =
{
    0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A
};

// Function to get the next available filename with increasing numbers
std::string getNextFilename(const std::string& prefix) {
    static int counter = 0;
    std::string filename;
    std::ifstream file;

    //do {
        std::ostringstream oss;
        oss << prefix << counter << ".bmp";
        filename = oss.str();
        //file.open(filename);
        //if (file.is_open()) {
        //    file.close();
        //}
        counter++;
    //} while (file.is_open());

    return filename;
}

// Function to save the bitmap to a file
bool SaveBitmapToFile(HBITMAP hBitmap, const char* filePath) {
    BITMAP bmp;
    BITMAPFILEHEADER bmpFileHeader;
    BITMAPINFOHEADER bmpInfoHeader;
    DWORD dwBmpSize;
    HANDLE hDIB;
    char* lpbitmap;
    DWORD dwSizeofDIB;
    HANDLE hFile;
    DWORD dwBytesWritten = 0;

    // Get the bitmap details
    GetObject(hBitmap, sizeof(BITMAP), &bmp);

    // Define the BMP file header
    bmpFileHeader.bfType = 0x4D42; // 'BM'
    bmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bmpFileHeader.bfReserved1 = 0;
    bmpFileHeader.bfReserved2 = 0;

    // Define the BMP info header
    bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfoHeader.biWidth = bmp.bmWidth;
    bmpInfoHeader.biHeight = bmp.bmHeight;
    bmpInfoHeader.biPlanes = 1;
    bmpInfoHeader.biBitCount = bmp.bmBitsPixel;
    bmpInfoHeader.biCompression = BI_RGB;
    bmpInfoHeader.biSizeImage = 0;
    bmpInfoHeader.biXPelsPerMeter = 0;
    bmpInfoHeader.biYPelsPerMeter = 0;
    bmpInfoHeader.biClrUsed = 0;
    bmpInfoHeader.biClrImportant = 0;

    // Calculate the BMP size
    dwBmpSize = ((bmp.bmWidth * bmpInfoHeader.biBitCount + 31) / 32) * 4 * bmp.bmHeight;

    // Create a device-independent bitmap
    hDIB = GlobalAlloc(GHND, dwBmpSize);
    lpbitmap = (char*)GlobalLock(hDIB);

    // Get the bitmap bits
    GetDIBits(GetDC(NULL), hBitmap, 0, (UINT)bmp.bmHeight, lpbitmap, (BITMAPINFO*)&bmpInfoHeader, DIB_RGB_COLORS);

    // Open the file to save the bitmap
    hFile = CreateFile(filePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    // Write the file header
    WriteFile(hFile, &bmpFileHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);

    // Write the info header
    WriteFile(hFile, &bmpInfoHeader, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);

    // Write the bitmap bits
    WriteFile(hFile, lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

    // Unlock and free the DIB
    GlobalUnlock(hDIB);
    GlobalFree(hDIB);

    // Close the file handle
    CloseHandle(hFile);

    return true;
}

void saveBitmap(const header& h, unsigned char* data) {
    int bitsPerPixel = 24;
    switch (h.glbaseinternalformat)
    {
    case GL_RED:    bitsPerPixel = 8;
        break;
    case GL_RG:     bitsPerPixel = 16;
        break;
    case GL_BGR:
    case GL_RGB:    bitsPerPixel = 24;
        break;
    case GL_BGRA:
    case GL_RGBA:   bitsPerPixel = 32;
        break;
    default:
        std::cerr << "Failed to create bitmap." << std::endl;
        break;
    }
    // Create the bitmap
    HBITMAP hBitmap = CreateBitmap(h.pixelwidth, h.pixelheight, 1, bitsPerPixel, data);
    if (!hBitmap) {
        std::cerr << "Failed to create bitmap." << std::endl;
    }

    // Generate the next available filename
    std::string filename = getNextFilename("bmps/image_");

    // Save the bitmap to a file
    if (!SaveBitmapToFile(hBitmap, filename.c_str())) {
        std::cerr << "Failed to save bitmap to file." << std::endl;
    }
    else {
        std::cout << "Bitmap saved to " << filename << std::endl;
    }

    // Clean up
    DeleteObject(hBitmap);
}
// Function to save bitmap to a file
void SaveBitmapCpp(int width, int height, unsigned char* pixelData) {
    // Calculate the size of the bitmap file headers
    int fileHeaderSize = 14;
    int infoHeaderSize = 40;
    int pixelDataSize = width * height * 3;

    // Create the file header
    unsigned char fileHeader[] = {
        0x42, 0x4D,             // Signature 'BM'
        0, 0, 0, 0,             // Image file size in bytes
        0, 0, 0, 0,             // Reserved
        54, 0, 0, 0             // Start of pixel array (54 bytes)
    };

    // Fill in the size
    int fileSize = fileHeaderSize + infoHeaderSize + pixelDataSize;
    fileHeader[2] = (unsigned char)(fileSize);
    fileHeader[3] = (unsigned char)(fileSize >> 8);
    fileHeader[4] = (unsigned char)(fileSize >> 16);
    fileHeader[5] = (unsigned char)(fileSize >> 24);

    // Create the info header
    unsigned char infoHeader[] = {
        40, 0, 0, 0,            // Header size (40 bytes)
        0, 0, 0, 0,             // Image width
        0, 0, 0, 0,             // Image height
        1, 0,                   // Number of color planes
        24, 0,                  // Bits per pixel (24)
        0, 0, 0, 0,             // Compression (none)
        0, 0, 0, 0,             // Image size (no compression)
        0, 0, 0, 0,             // Horizontal resolution (pixels per meter)
        0, 0, 0, 0,             // Vertical resolution (pixels per meter)
        0, 0, 0, 0,             // Colors in color table (none)
        0, 0, 0, 0              // Important color count (all colors are important)
    };

    // Fill in the width and height
    infoHeader[4] = (unsigned char)(width);
    infoHeader[5] = (unsigned char)(width >> 8);
    infoHeader[6] = (unsigned char)(width >> 16);
    infoHeader[7] = (unsigned char)(width >> 24);

    infoHeader[8] = (unsigned char)(height);
    infoHeader[9] = (unsigned char)(height >> 8);
    infoHeader[10] = (unsigned char)(height >> 16);
    infoHeader[11] = (unsigned char)(height >> 24);

    // Generate the next available filename
    std::string filename = getNextFilename("bmps/image_");

    // Write headers and pixel data to the file
    std::ofstream outFile(filename.c_str(), std::ios::out | std::ios::binary);
    outFile.write(reinterpret_cast<char*>(fileHeader), fileHeaderSize);
    outFile.write(reinterpret_cast<char*>(infoHeader), infoHeaderSize);
    outFile.write(reinterpret_cast<char*>(pixelData), pixelDataSize);
    outFile.close();
}

static const unsigned int swap32(const unsigned int u32)
{
    union
    {
        unsigned int u32;
        unsigned char u8[4];
    } a, b;

    a.u32 = u32;
    b.u8[0] = a.u8[3];
    b.u8[1] = a.u8[2];
    b.u8[2] = a.u8[1];
    b.u8[3] = a.u8[0];

    return b.u32;
}

static const unsigned short swap16(const unsigned short u16)
{
    union
    {
        unsigned short u16;
        unsigned char u8[2];
    } a, b;

    a.u16 = u16;
    b.u8[0] = a.u8[1];
    b.u8[1] = a.u8[0];

    return b.u16;
}

static unsigned int calculate_stride(const header& h, unsigned int width, unsigned int pad = 4)
{
    unsigned int channels = 0;

    switch (h.glbaseinternalformat)
    {
        case GL_RED:    channels = 1;
            break;
        case GL_RG:     channels = 2;
            break;
        case GL_BGR:
        case GL_RGB:    channels = 3;
            break;
        case GL_BGRA:
        case GL_RGBA:   channels = 4;
            break;
    }

    unsigned int stride = h.gltypesize * channels * width;

    stride = (stride + (pad - 1)) & ~(pad - 1);

    return stride;
}

static unsigned int calculate_face_size(const header& h)
{
    unsigned int stride = calculate_stride(h, h.pixelwidth);

    return stride * h.pixelheight;
}

extern
unsigned int load(const char * filename, unsigned int tex)
{
    FILE * fp;
    GLuint temp = 0;
    GLuint retval = 0;
    header h;
    size_t data_start, data_end;
    unsigned char * data;
    GLenum target = GL_NONE;

    fp = fopen(filename, "rb");

    if (!fp)
        return 0;

    if (fread(&h, sizeof(h), 1, fp) != 1)
        goto fail_read;

    if (memcmp(h.identifier, identifier, sizeof(identifier)) != 0)
        goto fail_header;

    if (h.endianness == 0x04030201)
    {
        // No swap needed
    }
    else if (h.endianness == 0x01020304)
    {
        // Swap needed
        h.endianness            = swap32(h.endianness);
        h.gltype                = swap32(h.gltype);
        h.gltypesize            = swap32(h.gltypesize);
        h.glformat              = swap32(h.glformat);
        h.glinternalformat      = swap32(h.glinternalformat);
        h.glbaseinternalformat  = swap32(h.glbaseinternalformat);
        h.pixelwidth            = swap32(h.pixelwidth);
        h.pixelheight           = swap32(h.pixelheight);
        h.pixeldepth            = swap32(h.pixeldepth);
        h.arrayelements         = swap32(h.arrayelements);
        h.faces                 = swap32(h.faces);
        h.miplevels             = swap32(h.miplevels);
        h.keypairbytes          = swap32(h.keypairbytes);
    }
    else
    {
        goto fail_header;
    }

    // Guess target (texture type)
    if (h.pixelheight == 0)
    {
        if (h.arrayelements == 0)
        {
            target = GL_TEXTURE_1D;
        }
        else
        {
            target = GL_TEXTURE_1D_ARRAY;
        }
    }
    else if (h.pixeldepth == 0)
    {
        if (h.arrayelements == 0)
        {
            if (h.faces == 0)
            {
                target = GL_TEXTURE_2D;
            }
            else
            {
                target = GL_TEXTURE_CUBE_MAP;
            }
        }
        else
        {
            if (h.faces == 0)
            {
                target = GL_TEXTURE_2D_ARRAY;
            }
            else
            {
                target = GL_TEXTURE_CUBE_MAP_ARRAY;
            }
        }
    }
    else
    {
        target = GL_TEXTURE_3D;
    }

    // Check for insanity...
    if (target == GL_NONE ||                                    // Couldn't figure out target
        (h.pixelwidth == 0) ||                                  // Texture has no width???
        (h.pixelheight == 0 && h.pixeldepth != 0))              // Texture has depth but no height???
    {
        goto fail_header;
    }

    temp = tex;
    if (tex == 0)
    {
        glGenTextures(1, &tex);
    }

    glBindTexture(target, tex);

    data_start = ftell(fp) + h.keypairbytes;
    fseek(fp, 0, SEEK_END);
    data_end = ftell(fp);
    fseek(fp, data_start, SEEK_SET);

    data = new unsigned char [data_end - data_start];
    memset(data, 0, data_end - data_start);

    fread(data, 1, data_end - data_start, fp);

    if (h.miplevels == 0)
    {
        h.miplevels = 1;
    }

    switch (target)
    {
        case GL_TEXTURE_1D:
            glTexStorage1D(GL_TEXTURE_1D, h.miplevels, h.glinternalformat, h.pixelwidth);
            glTexSubImage1D(GL_TEXTURE_1D, 0, 0, h.pixelwidth, h.glformat, h.glinternalformat, data);
            break;
        case GL_TEXTURE_2D:
            // glTexImage2D(GL_TEXTURE_2D, 0, h.glinternalformat, h.pixelwidth, h.pixelheight, 0, h.glformat, h.gltype, data);
            if (h.gltype == GL_NONE)
            {
                glCompressedTexImage2D(GL_TEXTURE_2D, 0, h.glinternalformat, h.pixelwidth, h.pixelheight, 0, 420 * 380 / 2, data);
                SaveBitmapCpp(h.pixelwidth, h.pixelheight, data);
                saveBitmap(h, data);
            }
            else
            {
                glTexStorage2D(GL_TEXTURE_2D, h.miplevels, h.glinternalformat, h.pixelwidth, h.pixelheight);
                SaveBitmapCpp(h.pixelwidth, h.pixelheight, data);
                saveBitmap(h, data);
                {
                    unsigned char * ptr = data;
                    unsigned int height = h.pixelheight;
                    unsigned int width = h.pixelwidth;
                    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                    for (unsigned int i = 0; i < h.miplevels; i++)
                    {
                        glTexSubImage2D(GL_TEXTURE_2D, i, 0, 0, width, height, h.glformat, h.gltype, ptr);
                        ptr += height * calculate_stride(h, width, 1);
                        height >>= 1;
                        width >>= 1;
                        if (!height)
                            height = 1;
                        if (!width)
                            width = 1;
                    }
                }
            }
            break;
        case GL_TEXTURE_3D:
            glTexStorage3D(GL_TEXTURE_3D, h.miplevels, h.glinternalformat, h.pixelwidth, h.pixelheight, h.pixeldepth);
            glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, h.pixelwidth, h.pixelheight, h.pixeldepth, h.glformat, h.gltype, data);
            break;
        case GL_TEXTURE_1D_ARRAY:
            glTexStorage2D(GL_TEXTURE_1D_ARRAY, h.miplevels, h.glinternalformat, h.pixelwidth, h.arrayelements);
            glTexSubImage2D(GL_TEXTURE_1D_ARRAY, 0, 0, 0, h.pixelwidth, h.arrayelements, h.glformat, h.gltype, data);
            break;
        case GL_TEXTURE_2D_ARRAY:
            glTexStorage3D(GL_TEXTURE_2D_ARRAY, h.miplevels, h.glinternalformat, h.pixelwidth, h.pixelheight, h.arrayelements);
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, h.pixelwidth, h.pixelheight, h.arrayelements, h.glformat, h.gltype, data);
            {
                unsigned int face_size = calculate_face_size(h);
                for (unsigned int i = 0; i < h.arrayelements; i++)
                {
                    SaveBitmapCpp(h.pixelwidth, h.pixelheight, data + face_size * i);
                    saveBitmap(h, data + face_size * i);
                }
            }
            break;
        case GL_TEXTURE_CUBE_MAP:
            glTexStorage2D(GL_TEXTURE_CUBE_MAP, h.miplevels, h.glinternalformat, h.pixelwidth, h.pixelheight);
            // glTexSubImage3D(GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0, h.pixelwidth, h.pixelheight, h.faces, h.glformat, h.gltype, data);
            {
                unsigned int face_size = calculate_face_size(h);
                for (unsigned int i = 0; i < h.faces; i++)
                {
                    glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, h.pixelwidth, h.pixelheight, h.glformat, h.gltype, data + face_size * i);
                    SaveBitmapCpp(h.pixelwidth, h.pixelheight, data + face_size * i);
                    saveBitmap(h, data + face_size * i);
                }
            }
            break;
        case GL_TEXTURE_CUBE_MAP_ARRAY:
            glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, h.miplevels, h.glinternalformat, h.pixelwidth, h.pixelheight, h.arrayelements);
            glTexSubImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, 0, 0, 0, h.pixelwidth, h.pixelheight, h.faces * h.arrayelements, h.glformat, h.gltype, data);
            {
                unsigned int face_size = calculate_face_size(h);
                for (unsigned int i = 0; i < h.faces * h.arrayelements; i++)
                {
                    SaveBitmapCpp(h.pixelwidth, h.pixelheight, data + face_size * i);
                    saveBitmap(h, data + face_size * i);
                }
            }
            break;
        default:                                               // Should never happen
            goto fail_target;
    }

    if (h.miplevels == 1)
    {
        glGenerateMipmap(target);
    }

    retval = tex;

fail_target:
    delete [] data;

fail_header:;
fail_read:;
    fclose(fp);

    return retval;
}

bool save(const char * filename, unsigned int target, unsigned int tex)
{
    header h;

    memset(&h, 0, sizeof(h));
    memcpy(h.identifier, identifier, sizeof(identifier));
    h.endianness = 0x04030201;

    glBindTexture(target, tex);

    glGetTexLevelParameteriv(target, 0, GL_TEXTURE_WIDTH, (GLint *)&h.pixelwidth);
    glGetTexLevelParameteriv(target, 0, GL_TEXTURE_HEIGHT, (GLint *)&h.pixelheight);
    glGetTexLevelParameteriv(target, 0, GL_TEXTURE_DEPTH, (GLint *)&h.pixeldepth);

    return true;
}

}

}

}
