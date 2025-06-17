#ifndef XMLCREATOR_H
#define XMLCREATOR_H

#include <iostream>
#include <json.hpp>
#include <tinyxml/tinyxml2.h>

#include "../../../../deps/pugixml-1.15/src/pugixml.hpp"
#include "interfaces/IDocument.h"
#include "interfaces/IBuilder.h"
#include "interfaces/IDirector.h"

using json = nlohmann::json;
using namespace tinyxml2;
using namespace pugi;

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

const std::vector<std::string> kHeaders_ = {
    "ID",
    "Тип события",
    "Журнал",
    "Время",
    "Результат",
    "Информация",
    "Объект",
    "Принтер",
    "Пользователь"
};

class XmlDocument : public IDocument {
private:
    xml_document doc_;
    xml_node spreadsheet_;

    bool in_table_ = false;
    xml_node current_table_;


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
        doc_.load_string(kXMLContent.data());
        spreadsheet_ = doc_.select_node("//office:spreadsheet").node();
    }

    void AddTableHeaders(float font_size, const std::vector<std::string>& headers, const std::vector<float>& column_widths) override {
        if (!in_table_) {
            in_table_ = true;
            current_table_ = spreadsheet_.append_child("table:table");
            current_table_.append_attribute("table:name").set_value("Sheet 1");

            xml_node row = current_table_.append_child("table:table-row");
            for (auto& header : headers) {
                xml_node cell = row.append_child("table:table-cell");
                cell.append_attribute("office:value-type").set_value("string");
                cell.append_child("text:p").text().set(header);
            }
        } else {
            throw std::runtime_error("Headers can be appended to existing table only once");
        }
    }

    void AddTableRow(float font_size, const std::vector<std::string>& row_fields, const std::vector<std::string>& headers, const std::vector<float>& column_widths) override {
        if (!in_table_) {
            throw std::runtime_error("Row in table can be appended to xml-document only if headers exists");
        }
        xml_node row = current_table_.append_child("table:table-row");
        for (auto& field : row_fields) {
            xml_node cell = row.append_child("table:table-cell");
            cell.append_attribute("office:value-type").set_value("string");
            cell.append_child("text:p").text().set(EscapeXml(field));
        }
    }

    void EndTable() {
        in_table_ = false;
        current_table_ = {};
    }

    ~XmlDocument() override = default;

    void AddJSON(const json& header_fields) override {
        if (!in_table_) {
            in_table_ = true;
            current_table_ = spreadsheet_.append_child("table:table");
            current_table_.append_attribute("table:name").set_value("Automatic testing of IPS functions");
            for (auto& header : header_fields) {
                xml_node row = current_table_.append_child("table:table-row");
                xml_node cell = row.append_child("table:table-cell");
                cell.append_attribute("office:value-type").set_value("string");
                cell.append_attribute("table:number-columns-spanned").set_value(4);
                cell.append_child("text:p").text().set(header.at("name").get<std::string>()+ ": " + header.at("value").get<std::string>());
            }
        } else {
            for (auto& header : header_fields) {
                xml_node row = current_table_.append_child("table:table-row");
                xml_node cell = row.append_child("table:table-cell");
                cell.append_attribute("office:value-type").set_value("string");
                cell.append_attribute("table:number-columns-spanned").set_value(4);
                cell.append_child("text:p").text().set(header.at("name").get<std::string>()+ ": " + header.at("value").get<std::string>());
            }
        }
    }

    void AddText(const std::string& text) override {}

    void SaveToFile(const std::string& file_path) override {
        doc_.save_file(file_path.c_str());
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
