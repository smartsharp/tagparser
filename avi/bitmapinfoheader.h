#ifndef TAG_PARSER_BITMAPINFOHEADER_H
#define TAG_PARSER_BITMAPINFOHEADER_H

#include "../global.h"

#include <c++utilities/conversion/types.h>

namespace IoUtilities {
class BinaryReader;
}

namespace TagParser {

class TAG_PARSER_EXPORT BitmapInfoHeader {
public:
    BitmapInfoHeader();

    void parse(IoUtilities::BinaryReader &reader);

    uint32 size;
    uint32 width;
    uint32 height;
    uint16 planes;
    uint16 bitCount;
    uint32 compression;
    uint32 imageSize;
    uint32 horizontalResolution;
    uint32 verticalResolution;
    uint32 clrUsed;
    uint32 clrImportant;
};

} // namespace TagParser

#endif // TAG_PARSER_BITMAPINFOHEADER_H
