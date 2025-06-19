#include "pdfcreator/xmlcreator.h"

const std::vector<std::string> TestXMLDirector::kHeaders_ = {
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


void XmlDocument::AddTableHeaders(float font_size, const std::vector<std::string> &headers,
                                  const std::vector<float> &column_widths) {
    if (!in_table_) {
        in_table_ = true;
        current_table_ = spreadsheet_.append_child("table:table");
        current_table_.append_attribute("table:name").set_value("Sheet 1");

        std::string font_style_name = AddFontStyle(font_size);

        xml_node row = current_table_.append_child("table:table-row");
        for (auto &header: headers) {
            xml_node cell = row.append_child("table:table-cell");
            cell.append_attribute("office:value-type").set_value("string");
            // Применяем стиль к тексту
            xml_node p = cell.append_child("text:p");
            p.append_attribute("text:style-name").set_value(font_style_name.c_str());
            p.text().set(header.c_str());
        }
    } else {
        throw std::runtime_error("Headers can be appended to existing table only once");
    }
}

void XmlDocument::AddTableRow(float font_size, const std::vector<std::string> &row_fields,
                              const std::vector<std::string> &headers,
                              const std::vector<float> &column_widths) {
    if (!in_table_) {
        throw std::runtime_error("Row in table can be appended to xml-document only if headers exists");
    }
    xml_node row = current_table_.append_child("table:table-row");
    for (auto &field: row_fields) {
        xml_node cell = row.append_child("table:table-cell");
        cell.append_attribute("office:value-type").set_value("string");
        cell.append_child("text:p").text().set(EscapeXml(field));
    }
}

void XmlDocument::AddJSON(const json &header_fields) {
    if (!in_table_) {
        in_table_ = true;
        current_table_ = spreadsheet_.append_child("table:table");
        current_table_.append_attribute("table:name").set_value("Automatic testing of IPS functions");
        for (auto &header: header_fields) {
            xml_node row = current_table_.append_child("table:table-row");
            xml_node cell = row.append_child("table:table-cell");
            cell.append_attribute("office:value-type").set_value("string");
            cell.append_attribute("table:number-columns-spanned").set_value(4);
            cell.append_child("text:p").text().set(
                header.at("name").get<std::string>() + ": " + header.at("value").get<std::string>());
        }
    } else {
        for (auto &header: header_fields) {
            xml_node row = current_table_.append_child("table:table-row");
            xml_node cell = row.append_child("table:table-cell");
            cell.append_attribute("office:value-type").set_value("string");
            cell.append_attribute("table:number-columns-spanned").set_value(4);
            cell.append_child("text:p").text().set(
                header.at("name").get<std::string>() + ": " + header.at("value").get<std::string>());
        }
    }
}

void XmlDocument::SaveToFile(const std::string& file_path) {
    doc_.save_file(file_path.c_str());
}

void XmlDocument::EndTable() {
    in_table_ = false;
    current_table_ = {};
}

std::string XmlDocument::EscapeXml(const std::string& input) {
    std::string output;
    output.reserve(input.size());

    for (char c : input) {
        switch (c) {
            case '&':  output += "&amp;";  break;
            case '<':  output += "&lt;";   break;
            case '>':  output += "&gt;";   break;
            case '"':  output += "&quot;"; break;
            case '\'': output += "&apos;"; break;
            default:   output += c;        break;
        }
    }

    return output;
}

std::string XmlDocument::AddFontStyle(float font_size) {
    // Добавляем стиль текста в automatic-styles
    xml_node automatic_styles = doc_.child("office:document-content").child("office:automatic-styles");
    if (!automatic_styles) {
        automatic_styles = doc_.child("office:document-content").prepend_child("office:automatic-styles");
    }

    // Создаем уникальное имя стиля на основе размера шрифта
    std::string style_name = "font_size_" + std::to_string(static_cast<int>(font_size));

    xml_node text_style = automatic_styles.append_child("style:style");
    text_style.append_attribute("style:name").set_value(style_name.c_str());
    text_style.append_attribute("style:family").set_value("text");

    xml_node text_properties = text_style.append_child("style:text-properties");
    text_properties.append_attribute("fo:font-size").set_value((std::to_string(font_size) + "pt").c_str());
    return style_name;
}