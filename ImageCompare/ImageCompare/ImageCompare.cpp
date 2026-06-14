#include <iostream>
#include "stb_image.h"

#include <windows.h>

class image
{
public:
    char* filename;
    int w, h, chan;
    unsigned char* pxls;

    int load(char* in_filename)
    {
        filename = in_filename;

        printf("Attempting to load %s using stbi_load()    ", filename);

//        stbi_set_flip_vertically_on_load(true);
        pxls = stbi_load(filename, &w, &h, &chan, 0);
        if (pxls)
        {
            printf("Load success! width: %d height: %d channels: %d\n", w, h, chan);
            return 0;
        }
        else
        {
            printf("Load Failed!\n");
            return 1;
        }
        return 1;
    }
};

int dimension_test(image left, image right)
{

    if (left.w != right.w)
    {
        printf("Failed preliminary, Width does not match\n");
        return -1;
    }
    if (left.h != right.h)
    {
        printf("Failed preliminary, Height does not match\n");
        return -1;
    }
    if (left.chan != right.chan)
    {
        printf("Failed preliminary, Channel count does not match\n");
        return -1;
    }
    if (left.chan != 3 || right.chan != 3)
    {
        printf("Failed preliminary, Channel count needs to be 3\n");
        return -1;
    }
    return 0;
}

int savebitmap(const char* filename, float* pixelBuffer, int w, int h)
{
    char* charBuffer = (char*)malloc(sizeof(unsigned char) * w * h * 3);

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            charBuffer[(y * w * 3) + (x * 3) + 0] = pixelBuffer[(((h - 1) - y) * w * 3) + (x * 3) + 2];
            charBuffer[(y * w * 3) + (x * 3) + 1] = pixelBuffer[(((h - 1) - y) * w * 3) + (x * 3) + 1];
            charBuffer[(y * w * 3) + (x * 3) + 2] = pixelBuffer[(((h - 1) - y) * w * 3) + (x * 3) + 0];
        }
    }

    BITMAPINFOHEADER infoHdr;
    infoHdr.biSize = 40;
    infoHdr.biWidth = w;
    infoHdr.biHeight = h;
    infoHdr.biPlanes = 1;
    infoHdr.biBitCount = 24;
    infoHdr.biCompression = 0;
    infoHdr.biSizeImage = sizeof(unsigned char) * w * h * 3;
    infoHdr.biXPelsPerMeter = 0;
    infoHdr.biYPelsPerMeter = 0;
    infoHdr.biClrUsed = 0;
    infoHdr.biClrImportant = 0;

    BITMAPFILEHEADER fileHdr;
    fileHdr.bfType = 19778;
    fileHdr.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (sizeof(unsigned char) * w * h * 3);
    fileHdr.bfReserved1 = 0;
    fileHdr.bfReserved2 = 0;
    fileHdr.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    FILE* bitmapFile;

    errno_t err = fopen_s(&bitmapFile, filename, "wb");
    if (err != 0 || bitmapFile == NULL)
    {
        printf("savebitmap - open failed for %s\n", filename);
        return NULL;
    }

    fwrite(&fileHdr, sizeof(BITMAPFILEHEADER), 1, bitmapFile);

    fwrite(&infoHdr, sizeof(BITMAPINFOHEADER), 1, bitmapFile);

    fseek(bitmapFile, fileHdr.bfOffBits, SEEK_SET);

    int nBytes = infoHdr.biWidth * infoHdr.biHeight * 3;

    fwrite(charBuffer, sizeof(unsigned char), nBytes, bitmapFile);

    fclose(bitmapFile);

    free(charBuffer);

    printf("savebitmap - saved %s w=%d h=%d bits=%d\n", filename, infoHdr.biWidth, infoHdr.biHeight, infoHdr.biBitCount);
    return 0;
}

struct stats
{
    int area;
    int match_num;
    double match_perc;
};

stats get_percentage_match(image left, image right)
{
    stats st;
    int H = left.h, W = left.w, C = left.chan;

    st.area = H * W;
    st.match_num = 0;
    st.match_perc = 0.f;

    float* diff_pixels = (float*)malloc(H * W * C * sizeof(float));

    for (int row = 0; row < H; row++)
    {
        float percf = (float)row / (float)left.h;
        int perci = percf * 100;
        std::clog << "\rScanlines done: " << perci << "%" << ' ' << std::flush;

        for (int col = 0; col < W; col++)
        {
            int p = (row * (W * C)) + (col * C);

            unsigned char l_r = left.pxls[p + 0];
            unsigned char r_r = right.pxls[p + 0];

            unsigned char l_g = left.pxls[p + 1];
            unsigned char r_g = right.pxls[p + 1];

            unsigned char l_b = left.pxls[p + 2];
            unsigned char r_b = right.pxls[p + 2];

           // printf("x: %d y: %d pixel: %d left: %d %d %d right: %d %d %d      ", col, row, p, l_r, l_g, l_b, r_r, r_g, r_b);
            char d_r = abs(l_r - r_r);
            char d_g = abs(l_g - r_g);
            char d_b = abs(l_b - r_b);

            //allow a 1pc error to account for potential floating point errors
            if (d_r < 2.56 && d_g < 2.56 && d_b < 2.56)
            {
                // printf("MATCH\n");
                st.match_num++;

                diff_pixels[p + 0] = 0;
                diff_pixels[p + 1] = 0;
                diff_pixels[p + 2] = 0;
            }
            else
            {
               // printf("NO_MATCH\n");

                diff_pixels[p + 0] = 255;
                diff_pixels[p + 1] = 0;
                diff_pixels[p + 2] = 255;
            }
        }
    }

    std::clog << "\rFinished comparison.           \n";

    savebitmap("diff_output.bmp", diff_pixels, W, H);
    free(diff_pixels);

    st.match_perc = (double)st.match_num / (double)(st.area);

    return st;
}


int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        printf("usage: file1 file2\n");
        exit(1);
    }

    image left, right;

    left.load(argv[1]);
    right.load(argv[2]);

    int ret = dimension_test(left, right);

    if (ret != 0)
    {
        printf("Try again with matching dimensions\n");
        exit(1);
    }
    else
    {
        printf("Initial test of image dimensions passed!\n");
    }

    printf("Comparing colour data of %s and %s\n", left.filename, right.filename);

    stats computed_stats = get_percentage_match(left, right);

    printf("\n\ntotal pixel count: %d \nmatching pixel count: %d \npercent of matched pixels %.8f\n", computed_stats.area, computed_stats.match_num, computed_stats.match_perc);
}

