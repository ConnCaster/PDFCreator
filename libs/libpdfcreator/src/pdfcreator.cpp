#include "pdfcreator/pdfcreator.h"

#include <iostream>

const std::vector<std::string> IDocument::kHeaders_ = {
    "integrityid",
    "type_id",
    "journal_id",
    "time",
    "result",
    "info",
    "object",
    "printer",
    "user_name"
};

PDFDocument::PDFDocument() {
    pdf_ = HPDF_New(nullptr, nullptr);
    if (!pdf_) {
        throw std::runtime_error("Error creating pdf document");
    }

    // Настройка параметров страницы
    page_ = HPDF_AddPage(pdf_);
    if (!page_) {
        throw std::runtime_error("Error creating new page in pdf");
    }
    HPDF_Page_SetSize(page_, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);
    cursor_.y = HPDF_Page_GetHeight(page_) - kStartPosY;

    // Настройка шрифта
    SetupFont();
}

PDFDocument::~PDFDocument() {
    HPDF_Free(pdf_);
}

void PDFDocument::AddHeader(const json& header_fields) {
    if (header_fields.empty()) return;

    for (const auto& field : header_fields) {
        if (cursor_.y < kMargin) {
            std::cout << "Создание новой страницы" << std::endl;
            try {
                AddNewPage();
            } catch (std::exception& e) {
                throw;
            }
        }

        std::string text = field.at("name").get<std::string>() + ": " + field.at("value").get<std::string>();

        HPDF_Page_BeginText(page_);
        HPDF_Page_TextOut(page_, kStartPosX, cursor_.y, text.c_str());
        HPDF_Page_EndText(page_);
        cursor_.y -= kLineSpacing;
        std::cout << "Current Y: " << cursor_.y << ", Page height: " << HPDF_Page_GetHeight(page_) << std::endl;
    }
}

void PDFDocument::SaveToFile(const std::string& file_path) {
    HPDF_SaveToFile(pdf_, file_path.data());
}

void PDFDocument::AddNewPage() {
    page_ = HPDF_AddPage(pdf_);
    if (!page_) {
        throw std::runtime_error("Error creating new page in pdf");
    }
    HPDF_Page_SetSize(page_, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);
    cursor_.y = HPDF_Page_GetHeight(page_) - kMargin;
    HPDF_Page_SetFontAndSize(page_, font_, kFontSize);
}

void PDFDocument::SetupFont() {
    const char* font_name = HPDF_LoadTTFontFromFile(pdf_, "/home/user/dir/PDFCreator/fonts/JetBrainsMonoNL-Regular.ttf", HPDF_TRUE);
    HPDF_UseUTFEncodings(pdf_);
    font_ = HPDF_GetFont(pdf_, font_name, "UTF-8");
    if (!font_) {
        font_ = HPDF_GetFont(pdf_, kFont.data(), nullptr);
    }
    HPDF_Page_SetFontAndSize(page_, font_, kFontSize);
}