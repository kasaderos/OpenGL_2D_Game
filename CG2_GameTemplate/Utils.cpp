#include "Utils.h"

bool OneOf(char symbol, const char *set)
{
    if (symbol == 0)
        return true;
    for (; *set; ++set)
        if (symbol == *set)
            return true;
    return false;
}

stl::vector<char *> SplitText(char *text, size_t length, const char *splitters, const char *ignoreBlockEnders)
{
    stl::vector<char *> result;

    bool newWordBegin = true;
    bool ignore = false;
    for (size_t pos = 0; pos < length; ++pos)
    {
        bool ignoreSymbol = OneOf(text[pos], ignoreBlockEnders);
        if (ignoreSymbol)
            ignore = !ignore;

        if (OneOf(text[pos], splitters) && (!ignore || ignore && ignoreSymbol))
        {
            text[pos] = '\0';
            newWordBegin = true;
        }
        else if (newWordBegin)
        {
            result.push_back(text + pos);
            newWordBegin = false;
        }
    }

    return result;
}