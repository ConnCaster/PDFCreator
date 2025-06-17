#ifndef XMLCREATOR_H
#define XMLCREATOR_H

#include <json.hpp>
#include <tinyxml/tinyxml2.h>

#include "interfaces/IDocument.h"
#include "interfaces/IBuilder.h"
#include "interfaces/IDirector.h"

using json = nlohmann::json;
using namespace tinyxml2;

const std::string mimetype_start = "application/vnd.oasis.opendocument.spreadsheet";
const std::string stylesxml_start = "<?xml version='1.0' encoding='UTF-8'?>\n<office:document-styles xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\" xmlns:table=\"urn:oasis:names:tc:opendocument:xmlns:table:1.0\" xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\" xmlns:manifest=\"urn:oasis:names:tc:opendocument:xmlns:manifest:1.0\" xmlns:meta=\"urn:oasis:names:tc:opendocument:xmlns:meta:1.0\" office:version=\"1.1\"><office:styles/><office:automatic-styles></office:automatic-styles></office:document-styles>";
const std::string contentxml_start = "<?xml version='1.0' encoding='UTF-8'?>\n<office:document-content xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\" xmlns:table=\"urn:oasis:names:tc:opendocument:xmlns:table:1.0\" xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\" xmlns:manifest=\"urn:oasis:names:tc:opendocument:xmlns:manifest:1.0\" xmlns:meta=\"urn:oasis:names:tc:opendocument:xmlns:meta:1.0\" office:version=\"1.1\"><office:automatic-styles/><office:body><office:spreadsheet><table:table table:name=\"Sheet 1\"><table:table-row><table:table-cell office:value-type=\"string\"><text:p>empty</text:p></table:table-cell><table:table-cell office:value-type=\"string\"><text:p>empty</text:p></table:table-cell><table:table-cell office:value-type=\"string\"><text:p>empty</text:p></table:table-cell></table:table-row></table:table></office:spreadsheet></office:body></office:document-content>";
const std::string metaxml_start = "<?xml version='1.0' encoding='UTF-8'?>\n<office:document-meta xmlns:table=\"urn:oasis:names:tc:opendocument:xmlns:table:1.0\" xmlns:manifest=\"urn:oasis:names:tc:opendocument:xmlns:manifest:1.0\" xmlns:presentation=\"urn:oasis:names:tc:opendocument:xmlns:presentation:1.0\" xmlns:meta=\"urn:oasis:names:tc:opendocument:xmlns:meta:1.0\" xmlns:style=\"urn:oasis:names:tc:opendocument:xmlns:style:1.0\" xmlns:chart=\"urn:oasis:names:tc:opendocument:xmlns:chart:1.0\" xmlns:draw=\"urn:oasis:names:tc:opendocument:xmlns:drawing:1.0\" xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\" xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\" office:version=\"1.1\"><office:meta><meta:generator>ODFPY/0.9.6</meta:generator></office:meta></office:document-meta>";
const std::string manifestxml_start = "<?xml version='1.0' encoding='UTF-8'?>\n<manifest:manifest xmlns:table=\"urn:oasis:names:tc:opendocument:xmlns:table:1.0\" xmlns:manifest=\"urn:oasis:names:tc:opendocument:xmlns:manifest:1.0\" xmlns:presentation=\"urn:oasis:names:tc:opendocument:xmlns:presentation:1.0\" xmlns:meta=\"urn:oasis:names:tc:opendocument:xmlns:meta:1.0\" xmlns:style=\"urn:oasis:names:tc:opendocument:xmlns:style:1.0\" xmlns:chart=\"urn:oasis:names:tc:opendocument:xmlns:chart:1.0\" xmlns:draw=\"urn:oasis:names:tc:opendocument:xmlns:drawing:1.0\" xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\" xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\"><manifest:file-entry manifest:media-type=\"application/vnd.oasis.opendocument.spreadsheet\" manifest:full-path=\"/\"/><manifest:file-entry manifest:media-type=\"text/xml\" manifest:full-path=\"styles.xml\"/><manifest:file-entry manifest:media-type=\"text/xml\" manifest:full-path=\"content.xml\"/><manifest:file-entry manifest:media-type=\"text/xml\" manifest:full-path=\"meta.xml\"/></manifest:manifest>";


constexpr std::string_view kXMLContent = "<?xml version='1.0' encoding='UTF-8'?>\n"
                                "<office:document-content xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\" xmlns:table=\"urn:oasis:names:tc:opendocument:xmlns:table:1.0\" xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\" xmlns:manifest=\"urn:oasis:names:tc:opendocument:xmlns:manifest:1.0\" xmlns:meta=\"urn:oasis:names:tc:opendocument:xmlns:meta:1.0\" office:version=\"1.1\">"
                                "<office:automatic-styles/>"

                                "<office:body>"
                                "   <office:spreadsheet>"

                                "   </office:spreadsheet>"
                                "</office:body>"
                                "</office:document-content>";

constexpr std::string_view kXMLTable =
                                "<table:table table:name=\"Sheet 1\">"
                                "   <table:table-row>"
                                "       <table:table-cell office:value-type=\"string\">"
                                "           <text:p>empty</text:p>"
                                "       </table:table-cell>"
                                "   </table:table-row>"
                                "</table:table>";

class XmlDocument : public IDocument {
private:
    XMLDocument doc_;
    XMLElement* current_parent_;
    XMLElement* current_table_ = nullptr;
    bool in_table_ = false;
    bool has_headers_ = false;

    // Вспомогательная функция для экранирования XML-специальных символов
    std::string EscapeXml(const std::string& input) {
        std::string output;
        output.reserve(input.size());

        for (char c : input) {
            switch (c) {
                case '&':  output += "&amp;";  break;
                case '<':  output += "&lt;";   break;
                case '>':  output += "&gt;";   break;
                case '"':  output += "&quot;"; break;
                case '\'': output += "&apos;"; break;
                default:   output += c;       break;
            }
        }

        return output;
    }

public:
    XmlDocument() {
        // Создаем декларацию XML и корневой элемент
        // doc_.InsertEndChild(doc_.NewDeclaration());
        // current_parent_ = doc_.NewElement("document");
        // doc_.InsertEndChild(current_parent_);
        doc_.Parse(kXMLContent.data());
    }

    ~XmlDocument() override = default;

    void AddJSON(const json& header_fields) override {
        // Добавляем JSON-данные как отдельный элемент
        XMLElement* jsonElement = doc_.NewElement("metadata");
        current_parent_->InsertEndChild(jsonElement);

        for (auto& [key, value] : header_fields.items()) {
            XMLElement* field = doc_.NewElement(key.c_str());
            if (value.is_string()) {
                field->SetText(EscapeXml(value.get<std::string>()).c_str());
            } else {
                field->SetText(EscapeXml(value.dump()).c_str());
            }
            jsonElement->InsertEndChild(field);
        }
    }

    void AddText(const std::string& text) override {
        if (in_table_) {
            // Если внутри таблицы, добавляем текст в текущую ячейку
            XMLElement* cell = doc_.NewElement("cell");
            cell->SetText(EscapeXml(text).c_str());
            current_parent_->InsertEndChild(cell);
        } else {
            // Обычный текстовый параграф
            XMLElement* paragraph = doc_.NewElement("paragraph");
            paragraph->SetText(EscapeXml(text).c_str());
            current_parent_->InsertEndChild(paragraph);
        }
    }

    void AddTableHeaders(float font_size, const std::vector<std::string>& headers, const std::vector<float>& column_widths) override {
        // Создаем элемент таблицы, если его еще нет
        if (!in_table_) {
            current_table_ = doc_.NewElement("table");
            current_parent_->InsertEndChild(current_table_);
            in_table_ = true;
            has_headers_ = true;
        }

        // Добавляем строку заголовков
        XMLElement* headerRow = doc_.NewElement("header_row");
        headerRow->SetAttribute("font_size", font_size);
        current_table_->InsertEndChild(headerRow);

        // Добавляем ячейки заголовков
        for (size_t i = 0; i < headers.size(); ++i) {
            XMLElement* headerCell = doc_.NewElement("header_cell");
            if (i < column_widths.size()) {
                headerCell->SetAttribute("width", column_widths[i]);
            }
            headerCell->SetText(EscapeXml(headers[i]).c_str());
            headerRow->InsertEndChild(headerCell);
        }

        // Устанавливаем текущий родитель для последующих строк
        current_parent_ = current_table_;
    }

    void AddTableRow(float font_size, const std::vector<std::string>& row_fields, const std::vector<std::string>& headers, const std::vector<float>& column_widths) override {
        // Если таблица еще не создана, создаем ее
        if (!in_table_) {
            current_table_ = doc_.NewElement("table");
            current_parent_->InsertEndChild(current_table_);
            in_table_ = true;
        }

        // Если есть заголовки, но они еще не добавлены, добавляем их
        if (!headers.empty() && !has_headers_) {
            AddTableHeaders(font_size, headers, column_widths);
        }

        // Добавляем строку данных
        XMLElement* row = doc_.NewElement("row");
        row->SetAttribute("font_size", font_size);
        current_table_->InsertEndChild(row);

        // Добавляем ячейки данных
        for (size_t i = 0; i < row_fields.size(); ++i) {
            XMLElement* cell = doc_.NewElement("cell");
            if (i < column_widths.size()) {
                cell->SetAttribute("width", column_widths[i]);
            }
            cell->SetText(EscapeXml(row_fields[i]).c_str());
            row->InsertEndChild(cell);
        }
    }

    void SaveToFile(const std::string& file_path) override {
        auto res = doc_.SaveFile(file_path.c_str());
    }
};

class XMLBuilder : public IBuilder {
public:
    XMLBuilder() = default;
    ~XMLBuilder() override = default;

    void AddJSON(const json& header_fields) override {
        document_.AddJSON(header_fields);
    };

    void AddText(const std::string& text) override {
        document_.AddText(text);
    };

    void AddTableRow(float font_size, const std::vector<std::string>& row_fields, const std::vector<std::string> &headers, const std::vector<float> &column_widths) override {
        document_.AddTableRow(font_size, row_fields, headers, column_widths);
    }

    void AddTableHeaders(float font_size, const std::vector<std::string>& headers, const std::vector<float> &column_widths) override {
        document_.AddTableHeaders(font_size, headers, column_widths);
    };

    IDocument* GetDocument() override {
        return &document_;
    };
private:
    XmlDocument document_;
};

class TestXMLDirector : public IDirector {
public:
    TestXMLDirector(IBuilder& builder)
        : builder_(builder)
    {};
    ~TestXMLDirector() override = default;

    void CreateDocument() override {
        // builder_.AddText(std::string("HELLO"));
    };

    void SetBuilder(IBuilder& builder) override {
        builder_ = builder;
    };
private:
    IBuilder& builder_;
    static const std::vector<std::string> kHeaders_;
};

#endif //XMLCREATOR_H
