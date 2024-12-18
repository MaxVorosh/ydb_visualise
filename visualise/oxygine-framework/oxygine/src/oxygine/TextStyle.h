#pragma once
#include "oxygine-include.h"
#include "math/Color.h"
#include <string>

namespace oxygine
{
    class TextStyle
    {
    public:
        enum HorizontalAlign
        {
            HALIGN_DEFAULT,
            HALIGN_LEFT,
            HALIGN_MIDDLE,
            HALIGN_RIGHT
        };

        enum VerticalAlign
        {
            VALIGN_DEFAULT,
            VALIGN_BASELINE,
            VALIGN_TOP,
            VALIGN_MIDDLE,
            VALIGN_BOTTOM
        };


        TextStyle(const ResFont* rs = 0): font(rs),
            hAlign(HALIGN_DEFAULT),
            vAlign(VALIGN_DEFAULT),
            linesOffset(0),
            kerning(0),
            multiline(false),
            fontSize(0),
            breakLongWords(false),
            baselineScale(1.0f),
            options(0) {}

        const ResFont* font;

        HorizontalAlign hAlign;
        VerticalAlign vAlign;

        int linesOffset;//vertical distance offset between lines
        int kerning;//horizontal distance
        bool multiline;
        int fontSize;
        bool breakLongWords;//works with multiline flag. breakLongWords = false doesn't allow to break too long words
        Color color;
        float baselineScale;//baseline distance multiplier
        size_t options;//additional flags could be used for generating custom glyphs

        TextStyle withFont(const ResFont* f) const { TextStyle st = *this; st.font = f; return st; }

        TextStyle alignTop() const { TextStyle st = *this; st.vAlign = VALIGN_TOP; return st; }
        TextStyle alignBottom() const { TextStyle st = *this; st.vAlign = VALIGN_BOTTOM; return st; }
        TextStyle alignBaseline() const { TextStyle st = *this; st.vAlign = VALIGN_BASELINE; return st; }
        TextStyle alignMiddleV() const { TextStyle st = *this; st.vAlign = VALIGN_MIDDLE; return st; }

        TextStyle alignLeft() const { TextStyle st = *this; st.hAlign = HALIGN_LEFT; return st; }
        TextStyle alignRight() const { TextStyle st = *this; st.hAlign = HALIGN_RIGHT; return st; }
        TextStyle alignMiddleH() const { TextStyle st = *this; st.hAlign = HALIGN_MIDDLE; return st; }

        TextStyle alignMiddle() const { TextStyle st = *this; st.vAlign = VALIGN_MIDDLE; st.hAlign = HALIGN_MIDDLE; return st; }

        TextStyle withHOffset(int offset) const { TextStyle st = *this; st.linesOffset = offset; return st; }
        TextStyle withBaselineScale(float s) const { TextStyle st = *this; st.baselineScale = s; return st; }

        TextStyle withKerning(int kerning) const { TextStyle st = *this; st.kerning = kerning; return st; }
        TextStyle withMultiline(bool multiline = true) const { TextStyle st = *this; st.multiline = multiline; return st; }
        TextStyle withColor(const Color& color) const { TextStyle st = *this; st.color = color; return st; }

        TextStyle withFontSize(int size) const { TextStyle st = *this; st.fontSize = size; return st; }

        TextStyle withOptions(size_t opt) const { TextStyle st = *this; st.options = opt; return st; }
    };

    std::string dumpStyle(const TextStyle& s, bool onlydiff);
}