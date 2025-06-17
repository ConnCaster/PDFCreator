#ifndef IBUILDER_H
#define IBUILDER_H

#include <json.hpp>

using json = nlohmann::json;

class IBuilder {
public:
    virtual ~IBuilder() = default;

    virtual void AddHeader() {};
    virtual void AddFooter() {};

    virtual void AddJSON(const json& header_fields) {};
    virtual void AddText(const std::string& text) {};
    virtual void AddTableRow(float font_size, const std::vector<std::string>& row_fields, const std::vector<std::string> &headers, const std::vector<float> &column_widths){};
    virtual void AddTableHeaders(float font_size, const std::vector<std::string>& headers, const std::vector<float> &column_widths) {};

    virtual IDocument* GetDocument() = 0;
};

#endif //IBUILDER_H
