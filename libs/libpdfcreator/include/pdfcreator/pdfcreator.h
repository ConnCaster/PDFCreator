#ifndef PDF_CREATOR_H
#define PDF_CREATOR_H

#include <hpdf.h>
#include <json.hpp>

using json = nlohmann::json;

constexpr std::string_view kFont = "Times-Roman";  // шрифт по умолчанию
constexpr HPDF_REAL kFontSize = 10.0;              // размер шрифта документа
constexpr HPDF_REAL kFontSizeTableRow = 8.0;       // размер шрифта в строке таблицы
constexpr HPDF_REAL kLineSpacing = 15.0;           // межстрочный интервал

constexpr HPDF_REAL kStartPosX = 20;               // начальное положение курсора
constexpr HPDF_REAL kStartPosY = 20;               // начальное положение курсора
constexpr HPDF_REAL kMargin = 20;                  // размер воображаемой рамки страницы (одинаковый со всех сторон)

constexpr HPDF_REAL kBorderWidth = 0.5;            // толщина линии рамки таблицы
constexpr HPDF_REAL kLeftRightPadding = 4.0;       // "заполнитель" слева и справа текста, который не дает ему прилипнуть к рамке


class IDocument {
public:
    static const std::vector<std::string> kHeaders_;

    virtual ~IDocument() = default;

    virtual void AddHeader(const json& header_fields) {};
    virtual void AddBody() {};
    virtual void AddFooter() {};

    virtual void SaveToFile(const std::string& file_path) = 0;
};

class PDFDocument : public IDocument {
public:
    PDFDocument();

    void AddHeader(const json& header_fields) override;
    void AddTable();
    void SaveToFile(const std::string& file_path) override;

    ~PDFDocument() override;

private:
    void AddNewPage();
    void SetupFont();

    void AddTableRow(HPDF_REAL font_size, const std::vector<std::string>& row_fields = kHeaders_);

private:
    HPDF_Doc pdf_;
    HPDF_Page page_;
    HPDF_Font font_;

    struct Cursor {
        HPDF_REAL x = kStartPosX;
        HPDF_REAL y = kStartPosY;
    } cursor_;

};

#endif
