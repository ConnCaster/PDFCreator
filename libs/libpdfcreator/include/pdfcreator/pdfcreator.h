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

    virtual void AddJSON(const json& header_fields) = 0;
    virtual void AddText(const std::string& text) = 0;
    virtual void AddTable() = 0;

    virtual void SaveToFile(const std::string& file_path) = 0;
};

class PDFDocument : public IDocument {
public:
    PDFDocument();

    void AddJSON(const json& header_fields) override;
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

class IBuilder {
public:
    virtual ~IBuilder() = default;

    virtual void AddHeader() {};
    virtual void AddFooter() {};

    virtual void AddJSON(const json& header_fields) {};
    virtual void AddText(const std::string& text) {};
    virtual void AddTable() {};

    virtual IDocument* GetDocument() = 0;
};

class PDFBuilder : public IBuilder {
public:
    PDFBuilder() = default;
    ~PDFBuilder() override = default;

    void AddJSON(const json& header_fields) override {
        document_.AddJSON(header_fields);
    };

    void AddText(const std::string& text) override {
        document_.AddText(text);
    };

    void AddTable() override {
        document_.AddTable();
    };

    IDocument* GetDocument() override {
        return &document_;
    };
private:
    PDFDocument document_;
};

class IDirector {
public:
    virtual ~IDirector() = default;

    virtual void CreateDocument() = 0;
    virtual void SetBuilder(IBuilder& builder) = 0;
};

class PDFDirector : public IDirector {
public:
    PDFDirector(IBuilder& builder)
        : builder_(builder)
    {};
    ~PDFDirector() override = default;

    void CreateDocument() override {
        builder_.AddText("text");
        builder_.AddTable();
    };

    void SetBuilder(IBuilder& builder) override {
        builder_ = builder;
    };
private:
    IBuilder& builder_;
};

class TestPDFDirector : public IDirector {
public:
    TestPDFDirector(IBuilder& builder)
        : builder_(builder)
    {};
    ~TestPDFDirector() override = default;

    void CreateDocument() override {
        builder_.AddJSON( json::parse(
        R"(
            [
                {"name": "Document", "value": "Annual Report"},
                {"name": "Author", "value": "Джон Доу"},
                {"name": "Касперский Endpoint Security", "value": "2023-05-15 @ 10//20\\30;:"},
                {"name": "Дата", "value": "2023-05-15"}
            ]
        )"));
        builder_.AddText(std::string("\n"));
        builder_.AddText(std::string("\n"));
        builder_.AddText(std::string("\n"));

        builder_.AddJSON( json::parse(
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
    };

    void SetBuilder(IBuilder& builder) override {
        builder_ = builder;
    };
private:
    IBuilder& builder_;
};

#endif
