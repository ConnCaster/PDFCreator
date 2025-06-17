#ifndef IDOCUMENT_H
#define IDOCUMENT_H

#include <json.hpp>

using json = nlohmann::json;

class IDocument {
public:
    virtual ~IDocument() = default;

    virtual void AddJSON(const json& header_fields) = 0;
    virtual void AddText(const std::string& text) = 0;
    virtual void AddTableRow(float font_size, const std::vector<std::string> &row_fields, const std::vector<std::string> &headers, const std::vector<float> &column_widths) = 0;
    virtual void AddTableHeaders(float font_size, const std::vector<std::string>& headers, const std::vector<float> &column_widths) = 0;
    virtual void SaveToFile(const std::string& file_path) = 0;
};

#endif //IDOCUMENT_H
