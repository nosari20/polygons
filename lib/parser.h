#ifndef PARSER_H
#define PARSER_H

#include <QList>
#include <string>
#include <vector>


#include "DGtal/base/Common.h"
#include "DGtal/base/BasicTypes.h"
#include "DGtal/helpers/StdDefs.h"


class Parser
{
public:
    Parser();
    static std::vector<DGtal::Z2i::Point> parseSDPFile(std::string url);
    static std::vector<DGtal::Z2i::Point> parseInlineSDPFile(std::string  url);
    static bool writeSDPFile(std::vector<DGtal::Z2i::Point> points, std::string url, std::string separator = " ");
    static std::vector<int> parseDominantPointFile(std::string url);

};

#endif // PARSER_H
