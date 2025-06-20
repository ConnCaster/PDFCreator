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

const std::vector<std::string> TestODSDirector::kHeaders_ = {
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

#include "zip.h"

void OdsDocument::SaveToFile(const std::string& file_path) {
    const std::string mimetype_start =    "application/vnd.oasis.opendocument.spreadsheet";
    const std::string stylesxml_start =   "<?xml version='1.0' encoding='UTF-8'?>\n<office:document-styles xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\" xmlns:table=\"urn:oasis:names:tc:opendocument:xmlns:table:1.0\" xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\" xmlns:manifest=\"urn:oasis:names:tc:opendocument:xmlns:manifest:1.0\" xmlns:meta=\"urn:oasis:names:tc:opendocument:xmlns:meta:1.0\" office:version=\"1.1\"><office:styles/><office:automatic-styles></office:automatic-styles></office:document-styles>";
    const std::string metaxml_start =     "<?xml version='1.0' encoding='UTF-8'?>\n<office:document-meta xmlns:table=\"urn:oasis:names:tc:opendocument:xmlns:table:1.0\" xmlns:manifest=\"urn:oasis:names:tc:opendocument:xmlns:manifest:1.0\" xmlns:presentation=\"urn:oasis:names:tc:opendocument:xmlns:presentation:1.0\" xmlns:meta=\"urn:oasis:names:tc:opendocument:xmlns:meta:1.0\" xmlns:style=\"urn:oasis:names:tc:opendocument:xmlns:style:1.0\" xmlns:chart=\"urn:oasis:names:tc:opendocument:xmlns:chart:1.0\" xmlns:draw=\"urn:oasis:names:tc:opendocument:xmlns:drawing:1.0\" xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\" xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\" office:version=\"1.1\"><office:meta><meta:generator>ODFPY/0.9.6</meta:generator></office:meta></office:document-meta>";
    const std::string manifestxml_start = "<?xml version='1.0' encoding='UTF-8'?>\n<manifest:manifest xmlns:table=\"urn:oasis:names:tc:opendocument:xmlns:table:1.0\" xmlns:manifest=\"urn:oasis:names:tc:opendocument:xmlns:manifest:1.0\" xmlns:presentation=\"urn:oasis:names:tc:opendocument:xmlns:presentation:1.0\" xmlns:meta=\"urn:oasis:names:tc:opendocument:xmlns:meta:1.0\" xmlns:style=\"urn:oasis:names:tc:opendocument:xmlns:style:1.0\" xmlns:chart=\"urn:oasis:names:tc:opendocument:xmlns:chart:1.0\" xmlns:draw=\"urn:oasis:names:tc:opendocument:xmlns:drawing:1.0\" xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\" xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\"><manifest:file-entry manifest:media-type=\"application/vnd.oasis.opendocument.spreadsheet\" manifest:full-path=\"/\"/><manifest:file-entry manifest:media-type=\"text/xml\" manifest:full-path=\"styles.xml\"/><manifest:file-entry manifest:media-type=\"text/xml\" manifest:full-path=\"content.xml\"/><manifest:file-entry manifest:media-type=\"text/xml\" manifest:full-path=\"meta.xml\"/></manifest:manifest>";

    std::stringstream ss;
    doc_.print(ss);
    std::string xml_str = ss.str();

    zip_t *empty_archive = zip_open(file_path.data(), ZIP_CREATE | ZIP_TRUNCATE, nullptr);
    zip_source *source_mimetype = zip_source_buffer(empty_archive, mimetype_start.c_str(), mimetype_start.length(), 0);
    zip_source *source_stylesxml = zip_source_buffer(empty_archive, stylesxml_start.c_str(), stylesxml_start.length(), 0);
    zip_source *source_contentxml = zip_source_buffer(empty_archive, xml_str.c_str(), xml_str.length(), 0);
    zip_source *source_metaxml = zip_source_buffer(empty_archive, metaxml_start.c_str(), metaxml_start.length(), 0);
    zip_source *source_manifestxml = zip_source_buffer(empty_archive, manifestxml_start.c_str(), manifestxml_start.length(), 0);

    if(!source_mimetype || !source_stylesxml || !source_contentxml || !source_metaxml || !source_manifestxml) {
        if(source_mimetype) zip_source_free(source_mimetype);
        if(source_stylesxml) zip_source_free(source_stylesxml);
        if(source_contentxml) zip_source_free(source_contentxml);
        if(source_metaxml) zip_source_free(source_metaxml);
        if(source_manifestxml) zip_source_free(source_manifestxml);
        return;
    }

    zip_file_add(empty_archive, "mimetype", source_mimetype, ZIP_FL_ENC_UTF_8 | ZIP_FL_OVERWRITE);
    zip_file_add(empty_archive, "styles.xml", source_stylesxml, ZIP_FL_ENC_UTF_8 | ZIP_FL_OVERWRITE);
    zip_file_add(empty_archive, "content.xml", source_contentxml, ZIP_FL_ENC_UTF_8 | ZIP_FL_OVERWRITE);
    zip_file_add(empty_archive, "meta.xml", source_metaxml, ZIP_FL_ENC_UTF_8 | ZIP_FL_OVERWRITE);

    zip_dir_add(empty_archive, "META-INF", ZIP_FL_ENC_UTF_8);

    zip_file_add(empty_archive, "META-INF/manifest.xml", source_manifestxml, ZIP_FL_ENC_UTF_8 | ZIP_FL_OVERWRITE);

    zip_close(empty_archive);
    std::cout << "ZIP created successfully!" << std::endl;
}