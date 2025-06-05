#include "pdfcreator/pdfcreator.h"

#include <iostream>

const std::vector<std::string> IDocument::kHeaders_ = {
    "integrity_idintegrity_idintegrity_id",
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

    // Настройка параметров страницы и курсора
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

void PDFDocument::AddHeader(const json &header_fields) {
    if (header_fields.empty()) return;

    for (const auto &field: header_fields) {
        if (cursor_.y < kMargin) {
            std::cout << "Создание новой страницы" << std::endl;
            try {
                AddNewPage();
            } catch (std::exception &e) {
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

void PDFDocument::SaveToFile(const std::string &file_path) {
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
    const char *font_name = HPDF_LoadTTFontFromFile(
        pdf_, "/home/user/dir/PDFCreator/fonts/JetBrainsMonoNL-Regular.ttf", HPDF_TRUE);
    HPDF_UseUTFEncodings(pdf_);
    font_ = HPDF_GetFont(pdf_, font_name, "UTF-8");
    if (!font_) {
        font_ = HPDF_GetFont(pdf_, kFont.data(), nullptr);
    }
    HPDF_Page_SetFontAndSize(page_, font_, kFontSize);
}

void PDFDocument::AddTable() {
    AddTableRow(kFontSizeTableRow);
    AddTableRow(8, {
        "абвгдеёжзийклмнопрстуфхцчшщъыьэюя",
        "1234567890",
        "АБВГДЕЁЖЗИЙКЛМОПРСТУФХЦЧШЩЪЫЬЭЮЯ",
        "0",
        "12345678901234567890123",
        "123456789012345",
        "123456789012345678901234567890123456",
        "№;%:&*()_+=-",
        "\"double_quotes\", \'single_quotes\'"
    });
    AddTableRow(8, {
        "abcdefghijklmnopqrstuvwxyz",
        "1234567890",
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
        "0",
        "{\"key_1\": \'value_!\'}, {\'key_2\': \"VALUE_@\"}, {\'key_3\': \"VALUE_#\"}, {\'key_4\': \"VALUE_$\"}",
        "123456789012345",
        "123456789012345678901234567890123456",
        "№;%:&*()_+=-",
        "\"double_quotes\", \'single_quotes\'"
    });
    AddTableRow(3, {
        "абвгдеёжзийклмнопрстуфхцчшщъыьэюя",
        "1234567890",
        "АБВГДЕЁЖЗИЙКЛМОПРСТУФХЦЧШЩЪЫЬЭЮЯ",
        "0",
        "12345678901234567890123",
        "123456789012345",
        "123456789012345678901234567890123456",
        "№;%:&*()_+=-",
        "\"double_quotes\", \'single_quotes\'"
    });
    AddTableRow(3, {
        "abcdefghijklmnopqrstuvwxyz",
        "1234567890",
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
        "0",
        "{\"key_1\": \'value_!\'}, {\'key_2\': \"VALUE_@\"}, {\'key_3\': \"VALUE_#\"}, {\'key_4\': \"VALUE_$\"}",
        "123456789012345",
        "123456789012345678901234567890123456",
        "№;%:&*()_+=-",
        "\"double_quotes\", \'single_quotes\'"
    });
    AddTableRow(19, {
       "абвгдеёжзийклмнопрстуфхцчшщъыьэюя",
       "1234567890",
       "АБВГДЕЁЖЗИЙКЛМОПРСТУФХЦЧШЩЪЫЬЭЮЯ",
       "0",
       "12345678901234567890123",
       "123456789012345",
       "123456789012345678901234567890123456",
       "№;%:&*()_+=-",
       "\"double_quotes\", \'single_quotes\'"
   });
    AddTableRow(13, {
        "abcdefghijklmnopqrstuvwxyz",
        "1234567890",
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
        "0",
        "{\"key_1\": \'value_!\'}, {\'key_2\': \"VALUE_@\"}, {\'key_3\': \"VALUE_#\"}, {\'key_4\': \"VALUE_$\"}",
        "123456789012345",
        "123456789012345678901234567890123456",
        "№;%:&*()_+=-",
        "\"double_quotes\", \'single_quotes\'"
    });
}

void PDFDocument::AddTableRow(HPDF_REAL font_size, const std::vector<std::string> &row_fields) {
    // Параметры таблицы:
    // ширина страницы
    HPDF_REAL page_width = HPDF_Page_GetWidth(page_);
    // ширина таблицы на странице (ширина страницы без левого и правого отступа от краев)
    HPDF_REAL table_width = page_width - 2 * kMargin;
    // ширина столбца в таблице
    // TODO: динамическая ширина столбца
    HPDF_REAL base_column_width = table_width / row_fields.size();
    // высота строки по умолчанию - размер шрифтра и еще полразмера сверху и снизу
    HPDF_REAL base_row_height = kFontSizeTableRow * 2;

    // 1. Предварительный расчет максимальной высоты строки.
    // Учитывает возможную необходимость переноса строки текста в рамках строки таблицы
    HPDF_REAL max_row_height = base_row_height;

    for (const auto &field: row_fields) {
        HPDF_REAL text_width = HPDF_Page_TextWidth(page_, field.c_str());
        int text_rows = 1;
        // если ширина текста в ячейке больше ширины ячейки за вычетом двух "заполнителей"
        if (text_width > (base_column_width - 2 * kLeftRightPadding)) {
            // то придется переносить текст на следующую строку (строка таблицы начнет вмещать 2 и более строк текста)
            // для этого сделаем высоту строки таблицы больше
            text_rows = ceil(text_width / (base_column_width - 2 * kLeftRightPadding));
            HPDF_REAL required_height = text_rows * (base_row_height - kFontSizeTableRow/2.0) + kFontSizeTableRow/2.0;
            max_row_height = std::max(max_row_height, required_height);
        }
        std::cout << "font: " << kFontSizeTableRow << " text_rows: " << text_rows << std::endl;
    }
    std::cout << "REQUIRED_H: " << max_row_height << " MAX_ROW_H: " << max_row_height << std::endl;
    std::cout << "===================" << std::endl;

    // 2. Проверка места на странице
    if (cursor_.y - max_row_height < kMargin) { //  kMargin + 2 * kLineSpacing
        try {
            AddNewPage();
            // После создания новой страницы сбрасываем курсор в верхнюю позицию
            cursor_.y = HPDF_Page_GetHeight(page_) - kStartPosY;

            // Если даже после создания страницы не хватает места - ошибка
            if (cursor_.y - max_row_height < kMargin) {
                throw std::runtime_error("Header row is too large for the page");
            }
        } catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to add new page: ") + e.what());
        }
    }

    // координата Y нижней границы строки с учетом рассчитанной максимальной высоты строки
    // (из текущей вертикальной координаты курсора вычитаем максимальную высоту строки)
    const float y_bottom_of_row = cursor_.y - max_row_height;

    // 3. Рисуем границы таблицы
    HPDF_Page_SetLineWidth(page_, kBorderWidth);

    // Горизонтальные линии
    HPDF_Page_MoveTo(page_, kStartPosX, cursor_.y);
    HPDF_Page_LineTo(page_, kStartPosX + table_width, cursor_.y); // на уровне курсора

    HPDF_Page_MoveTo(page_, kStartPosX, y_bottom_of_row);
    HPDF_Page_LineTo(page_, kStartPosX + table_width, y_bottom_of_row); // на уровне нижней границы строки

    // Вертикальные линии
    float x_pos_in_row = kStartPosX;
    // здесь и далее определяет положение курсора при работе в рамках строки по горизонтали
    for (size_t i = 0; i <= row_fields.size(); ++i) {
        HPDF_Page_MoveTo(page_, x_pos_in_row, cursor_.y);
        HPDF_Page_LineTo(page_, x_pos_in_row, y_bottom_of_row);
        if (i < row_fields.size()) x_pos_in_row += base_column_width;
    }
    HPDF_Page_Stroke(page_);

    // 4. Добавляем текст
    x_pos_in_row = kStartPosX;
    HPDF_Page_SetFontAndSize(page_, font_, kFontSizeTableRow);
    HPDF_Page_BeginText(page_);

    for (const auto &field: row_fields) {
        HPDF_REAL text_width = HPDF_Page_TextWidth(page_, field.c_str());

        if (text_width <= (base_column_width - 2 * kLeftRightPadding)) {
            // Однострочный текст
            HPDF_REAL text_x = x_pos_in_row + (base_column_width - text_width) / 2;
            HPDF_REAL text_y = cursor_.y - max_row_height / 2 - font_size / 3;
            HPDF_Page_TextOut(page_, text_x, text_y, field.c_str());
        } else {
            // Многострочный текст
            HPDF_REAL available_width = base_column_width - 2 * kLeftRightPadding;
            size_t chars_per_line = field.length() * (available_width / text_width);

            HPDF_REAL current_y = cursor_.y - kLeftRightPadding;
            size_t start_pos = 0;

            while (start_pos < field.length()) {
                size_t end_pos = std::min(start_pos + chars_per_line, field.length());
                std::string line = field.substr(start_pos, end_pos - start_pos);

                HPDF_REAL line_width = HPDF_Page_TextWidth(page_, line.c_str());
                HPDF_REAL text_x = x_pos_in_row + (base_column_width - line_width) / 2;

                HPDF_Page_TextOut(page_, text_x, current_y - font_size, line.c_str());
                current_y -= kFontSizeTableRow + kFontSizeTableRow/2.0;
                start_pos = end_pos;
            }
        }
        x_pos_in_row += base_column_width;
    }

    HPDF_Page_EndText(page_);

    // 5. Обновляем позицию курсора
    cursor_.y -= max_row_height;
}
