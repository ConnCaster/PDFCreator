#ifndef PDF_CREATOR_H
#define PDF_CREATOR_H

#include <hpdf.h>
#include <json.hpp>

using json = nlohmann::json;

constexpr std::string_view kFont = "Times-Roman";  // шрифт по умолчанию
constexpr HPDF_REAL kFontSize = 11.0;              // размер шрифта документа
constexpr HPDF_REAL kFontSizeTableRow = 7.0;       // размер шрифта в строке таблицы
constexpr HPDF_REAL kLineSpacing = 10.0;           // межстрочный интервал

constexpr HPDF_REAL kStartPosX = 20;               // начальное положение курсора
constexpr HPDF_REAL kStartPosY = 20;               // начальное положение курсора
constexpr HPDF_REAL kMargin = 20;                  // размер воображаемой рамки страницы (одинаковый со всех сторон)

constexpr HPDF_REAL kBorderWidth = 0.5;            // толщина линии рамки таблицы
constexpr HPDF_REAL kLeftRightPadding = 4.0;       // "заполнитель" слева и справа текста, который не дает ему прилипнуть к рамке


class IDocument {
public:
    static const std::vector<std::string> kHeaders_;

    virtual ~IDocument() = default;

    virtual void AddText(const json& header_fields) {};
    virtual void AddText(const std::string& text) {};
    virtual void AddTable() {};

    virtual void SaveToFile(const std::string& file_path) = 0;
};

class PDFDocument : public IDocument {
public:
    PDFDocument();

    void AddText(const json& header_fields) override;
    void AddText(const std::string& text) override;
    void AddTable();
    void SaveToFile(const std::string& file_path) override;

    ~PDFDocument() override;

#ifdef BUILD_GOOGLE_TEST
public:
#else
private:
#endif
    void AddNewPage();
    void AddTableRow(HPDF_REAL font_size, const std::vector<std::string>& row_fields = kHeaders_);

    void SetupFont();

    int CalcTextRowsInCell(const std::string& field_text, size_t chars_per_line);
    int CalcTextWidthInCell(HPDF_REAL cell_width, HPDF_REAL text_width, int symbols);
    HPDF_REAL CalcBaseColumnWidth(const std::vector<std::string> &row_fields);
    HPDF_REAL CalcMaxColumnHeight(HPDF_REAL base_row_height, HPDF_REAL base_column_width, HPDF_REAL font_size, const std::vector<std::string> &row_fields);
    void AddTextToTableRow(HPDF_REAL row_height, HPDF_REAL font_size, const std::vector<std::string> &row_fields);

    void PrintTextWithWrap(const std::string& text);
    void ProcessWord(std::string& word, std::string& current_line, std::vector<std::string>& lines, HPDF_REAL available_width);

#ifdef BUILD_GOOGLE_TEST
public:
#else
private:
#endif
    HPDF_Doc pdf_;
    HPDF_Page page_;
    HPDF_Font font_;

    struct Cursor {
        HPDF_REAL x = kStartPosX;
        HPDF_REAL y = kStartPosY;
    } cursor_;
};

#endif
