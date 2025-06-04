#ifndef PDF_CREATOR_H
#define PDF_CREATOR_H

#include <hpdf.h>
#include <json.hpp>

using json = nlohmann::json;

constexpr std::string_view kFont = "Times-Roman";   // шрифт по умолчанию
constexpr HPDF_REAL kFontSize = 14.0;               // размер шрифта
constexpr HPDF_REAL kLineSpacing = 15.0;            // межстрочный интерва

constexpr HPDF_REAL kStartPosX = 20;
constexpr HPDF_REAL kStartPosY = 20;
constexpr HPDF_REAL kMargin = 20;                   // размер воображаемой рамки страницы (одинаковый со всех сторон)

class IDocument {
public:
    static const std::vector<std::string> kHeaders_;

public:
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
    void SaveToFile(const std::string& file_path) override;

    ~PDFDocument() override;

private:
    void AddNewPage();
    void SetupFont();

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
