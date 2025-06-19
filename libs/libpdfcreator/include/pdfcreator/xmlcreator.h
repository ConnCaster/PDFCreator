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

const std::string kMimetypeStart = "application/vnd.oasis.opendocument.spreadsheet";
const std::string kStylesXmlStart = "<?xml version='1.0' encoding='UTF-8'?>\n<office:document-styles xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\" xmlns:table=\"urn:oasis:names:tc:opendocument:xmlns:table:1.0\" xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\" xmlns:manifest=\"urn:oasis:names:tc:opendocument:xmlns:manifest:1.0\" xmlns:meta=\"urn:oasis:names:tc:opendocument:xmlns:meta:1.0\" office:version=\"1.1\"><office:styles/><office:automatic-styles></office:automatic-styles></office:document-styles>";
const std::string kContentXmlStart = "<?xml version='1.0' encoding='UTF-8'?>\n<office:document-content xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\" xmlns:table=\"urn:oasis:names:tc:opendocument:xmlns:table:1.0\" xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\" xmlns:manifest=\"urn:oasis:names:tc:opendocument:xmlns:manifest:1.0\" xmlns:meta=\"urn:oasis:names:tc:opendocument:xmlns:meta:1.0\" office:version=\"1.1\"><office:automatic-styles/><office:body><office:spreadsheet><table:table table:name=\"Sheet 1\"><table:table-row><table:table-cell office:value-type=\"string\"><text:p>empty</text:p></table:table-cell><table:table-cell office:value-type=\"string\"><text:p>empty</text:p></table:table-cell><table:table-cell office:value-type=\"string\"><text:p>empty</text:p></table:table-cell></table:table-row></table:table></office:spreadsheet></office:body></office:document-content>";
const std::string kMetaXmlStart = "<?xml version='1.0' encoding='UTF-8'?>\n<office:document-meta xmlns:table=\"urn:oasis:names:tc:opendocument:xmlns:table:1.0\" xmlns:manifest=\"urn:oasis:names:tc:opendocument:xmlns:manifest:1.0\" xmlns:presentation=\"urn:oasis:names:tc:opendocument:xmlns:presentation:1.0\" xmlns:meta=\"urn:oasis:names:tc:opendocument:xmlns:meta:1.0\" xmlns:style=\"urn:oasis:names:tc:opendocument:xmlns:style:1.0\" xmlns:chart=\"urn:oasis:names:tc:opendocument:xmlns:chart:1.0\" xmlns:draw=\"urn:oasis:names:tc:opendocument:xmlns:drawing:1.0\" xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\" xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\" office:version=\"1.1\"><office:meta><meta:generator>ODFPY/0.9.6</meta:generator></office:meta></office:document-meta>";
const std::string kManifestXmlStart = "<?xml version='1.0' encoding='UTF-8'?>\n<manifest:manifest xmlns:table=\"urn:oasis:names:tc:opendocument:xmlns:table:1.0\" xmlns:manifest=\"urn:oasis:names:tc:opendocument:xmlns:manifest:1.0\" xmlns:presentation=\"urn:oasis:names:tc:opendocument:xmlns:presentation:1.0\" xmlns:meta=\"urn:oasis:names:tc:opendocument:xmlns:meta:1.0\" xmlns:style=\"urn:oasis:names:tc:opendocument:xmlns:style:1.0\" xmlns:chart=\"urn:oasis:names:tc:opendocument:xmlns:chart:1.0\" xmlns:draw=\"urn:oasis:names:tc:opendocument:xmlns:drawing:1.0\" xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\" xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\"><manifest:file-entry manifest:media-type=\"application/vnd.oasis.opendocument.spreadsheet\" manifest:full-path=\"/\"/><manifest:file-entry manifest:media-type=\"text/xml\" manifest:full-path=\"styles.xml\"/><manifest:file-entry manifest:media-type=\"text/xml\" manifest:full-path=\"content.xml\"/><manifest:file-entry manifest:media-type=\"text/xml\" manifest:full-path=\"meta.xml\"/></manifest:manifest>";


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
public:
    XmlDocument() {
        doc_.load_string(kXMLContent.data());
        spreadsheet_ = doc_.select_node("//office:spreadsheet").node();
    }
    ~XmlDocument() override = default;

    void AddTableHeaders(float font_size, const std::vector<std::string>& headers, const std::vector<float>& column_widths) override;
    void AddTableRow(float font_size, const std::vector<std::string>& row_fields, const std::vector<std::string>& headers, const std::vector<float>& column_widths) override;
    void AddJSON(const json& header_fields) override;
    void AddText(const std::string& text) override {}
    void SaveToFile(const std::string& file_path) override;
    void EndTable();

private:
    // Вспомогательная функция для экранирования XML-специальных символов
    std::string EscapeXml(const std::string& input);
    std::string AddFontStyle(float font_size);

private:
    xml_document doc_;
    xml_node spreadsheet_;

    bool in_table_ = false;
    xml_node current_table_;
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
        try {
            builder_.AddTableHeaders(10, kHeaders_, {});

            builder_.AddTableRow(10, kHeaders_, {}, {});
            builder_.AddTableRow(10, kHeaders_, {}, {});
            builder_.AddTableRow(10, {
                R"({"key": "value"})",
                R"({"key": "value"})",
                R"({"key": "value"})",
                R"({"key": "value"})"
            }, {}, {});

            dynamic_cast<XmlDocument*>(builder_.GetDocument())->EndTable();
            builder_.AddJSON(json::parse(
                R"(
                [
                    {"name": "User-initiator", "value": "dlladmin"},
                    {"name": "Machine", "value": "x86_64"},
                    {"name": "Node name", "value": "astra.DL.LOCAL"},
                    {"name": "System name", "value": "Linux"},
                    {"name": "Version", "value": "#astra2+ci6 SMP PREEMPT_DYNAMIC Fri Oct  6 14:38:42 UTC 2023"},
                    {"name": "SZI version", "value": "5.8.109"},
                    {"name": "Test was started", "value": "04-06-2025@14:22:49"},
                    {"name": "Test was finished", "value": "04-06-2025@14:22:49"},
                    {"name": "Test done", "value": "15/23"},
                    {"name": "Status", "value": "interrupt"}
                ]
            )"));
            builder_.AddJSON(json::parse(
                R"(
                [
                    {"name": "User-initiator", "value": "dlladmin"},
                    {"name": "Machine", "value": "x86_64"},
                    {"name": "Node name", "value": "astra.DL.LOCAL"},
                    {"name": "System name", "value": "Linux"},
                    {"name": "Version", "value": "#astra2+ci6 SMP PREEMPT_DYNAMIC Fri Oct  6 14:38:42 UTC 2023"},
                    {"name": "SZI version", "value": "5.8.109"},
                    {"name": "Test was started", "value": "04-06-2025@14:22:49"},
                    {"name": "Test was finished", "value": "04-06-2025@14:22:49"},
                    {"name": "Test done", "value": "15/23"},
                    {"name": "Status", "value": "interrupt"}
                ]
            )"));
        } catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    };

    void SetBuilder(IBuilder& builder) override {
        builder_ = builder;
    };
private:
    IBuilder& builder_;
    static const std::vector<std::string> kHeaders_;
};

#endif //XMLCREATOR_H
