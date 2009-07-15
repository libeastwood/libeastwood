#ifndef EASTWOOD_FONT_H
#define EASTWOOD_FONT_H

#include <string>

#define MIN_FONT_SIZE 6
#define MAX_FONT_SIZE 24
#define FONTS (MAX_FONT_SIZE - MIN_FONT_SIZE)

struct FNTHeader
{
    uint16_t fsize;    /* Size of the file              */
    uint16_t unknown1; /* Unknown entry (always 0x0500) */
    uint16_t unknown2; /* Unknown entry (always 0x000e) */
    uint16_t unknown3; /* Unknown entry (always 0x0014) */
    uint16_t wpos;     /* Offset of char. widths array  (abs. from beg. of file) */
    uint16_t cdata;    /* Offset of char. graphics data (abs. from beg. of file) */
    uint16_t hpos;     /* Offset of char. heights array (abs. from beg. of file) */
    uint16_t unknown4; /* Unknown entry (always 0x1012) */
    uint8_t unknown5; // dunk- had to add this to get nchars read correctly 
    uint8_t nchars;   /* Number of characters in font minus 1*/ // dunk- the doc says uint16_t 
    uint8_t height;   /* Font height                   */
    uint8_t maxw;     /* Max. character width          */
};

struct FNTCharacter
{
    uint8_t width;
    uint8_t height;
    uint8_t y_offset;
    uint8_t *bitmap;
};

class Font 
{
    public:
        Font(FNTCharacter *characters, FNTHeader *header);
        ~Font();

        void extents(std::string text, uint16_t& w, uint16_t& h);
        void render(std::string text, SDL_Surface *image, int x, int y, uint8_t paloff);


    private:
        FNTHeader *m_header;
        FNTCharacter *m_characters;
        uint16_t m_nchars;
};

#endif // EASTWOOD_FONT_H
