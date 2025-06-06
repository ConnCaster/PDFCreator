#include "pdfcreator/pdfcreator.h"

#include <iostream>

const std::vector<std::string> IDocument::kHeaders_ = {
    "integrity_id",
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

void PDFDocument::AddJSON(const json& header_fields) {
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
        PrintTextWithWrap(text);
    }
}

void PDFDocument::AddText(const std::string& text) {
    if (text.empty()) return;

    if (cursor_.y < kMargin) {
        std::cout << "Создание новой страницы" << std::endl;
        try {
            AddNewPage();
        } catch (std::exception &e) {
            throw;
        }
    }

    PrintTextWithWrap(text);
}

void PDFDocument::PrintTextWithWrap(const std::string& text) {
    if (text.empty()) return;

    // Получаем доступную ширину для текста (ширина страницы минус левый и правый отступы)
    const HPDF_REAL page_width = HPDF_Page_GetWidth(page_);
    const HPDF_REAL available_width = page_width - 2 * kMargin;

    // Разбиваем текст на строки, которые помещаются в доступную ширину
    std::vector<std::string> lines;
    std::string current_line;
    std::string current_word;

    for (char ch : text) {
        if (ch == ' ' || ch == '\t' || ch == '\n') {
            // Для существующих разделителей обрабатываем накопленное слово
            if (!current_word.empty()) {
                ProcessWord(current_word, current_line, lines, available_width);
            }

            // Обрабатываем перевод строки отдельно,
            // чтобы завершить формирование текущей строки, так как встретили "перевод строки"
            if (ch == '\n') {
                if (!current_line.empty()) {
                    lines.push_back(current_line);
                    current_line.clear();
                } else {
                    lines.push_back("");
                }
            }
        } else {
            current_word += ch;
        }
    }

    // Добавляем последнее слово, если оно есть
    if (!current_word.empty()) {
        ProcessWord(current_word, current_line, lines, available_width);
    }

    // Добавляем оставшуюся строку
    if (!current_line.empty()) {
        lines.push_back(current_line);
    }

    // Печатаем все строки
    for (const auto& line : lines) {
        // Проверяем, нужно ли создать новую страницу
        if (cursor_.y < kMargin) {
            AddNewPage();
        }

        HPDF_Page_BeginText(page_);
        HPDF_Page_TextOut(page_, kStartPosX, cursor_.y, line.c_str());
        HPDF_Page_EndText(page_);
        cursor_.y -= kFontSize + kLineSpacing;
    }
}

void PDFDocument::ProcessWord(std::string& word, std::string& current_line,
                            std::vector<std::string>& lines, HPDF_REAL available_width) {
    // Проверяем, помещается ли слово в текущую строку
    std::string test_line = current_line.empty() ? word : current_line + " " + word;
    HPDF_REAL text_width = HPDF_Page_TextWidth(page_, test_line.c_str());

    if (text_width <= available_width) {
        current_line = test_line;
    } else {
        // Если слово слишком длинное и не помещается даже в пустую строку
        if (current_line.empty()) {
            // Разбиваем слово посимвольно
            for (char ch : word) {
                std::string single_char(1, ch);
                HPDF_REAL char_width = HPDF_Page_TextWidth(page_, single_char.c_str());

                if (char_width > available_width) {
                    // Если даже один символ не помещается - пропускаем
                    continue;
                }

                test_line = current_line + single_char;
                text_width = HPDF_Page_TextWidth(page_, test_line.c_str());

                if (text_width <= available_width) {
                    current_line = test_line;
                } else {
                    lines.push_back(current_line);
                    current_line = single_char;
                }
            }
            word.clear();
        } else {
            // Переносим текущую строку и начинаем новую с этого слова
            lines.push_back(current_line);
            current_line = word;
        }
    }
    word.clear();
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
    AddTableRow(14, {
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
    AddTableRow(14, {
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
    AddTableRow(11, {
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
    AddTableRow(11, {
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
        AddTableRow(18, {
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
    AddTableRow(18, {
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

/*
 *  Расчет количества символов, которые поместятся в строку внутри ячейки с учетом ширины ячейки и шрифта
 *  cell_width - ширина ячейки в "пикселях"
 *  text_width - ширина текста в "пикселях" (учитывает шрифт текста, ведь он влияет на ширину символа)
 *  symbols    - количество символов в штуках
 */
int PDFDocument::CalcTextWidthInCell(HPDF_REAL cell_width, HPDF_REAL text_width, int symbols) {
    return symbols * ((cell_width - 2 * kLeftRightPadding) / text_width);
}

/*
 *  Расчет количества строк, которые займет текст в ячейке с заданной шириной
 *  field_text     - непосредственно текст, который нужно запихнуть в ячейку
 *  chars_per_line - количество символов, которые поместятся в строку внутри ячейки с учетом ширины ячейки и шрифта
 */
int PDFDocument::CalcTextRowsInCell(const std::string& field_text, size_t chars_per_line) {
    size_t start_pos = 0;
    int counter = 0;
    while (start_pos < field_text.length()) {
        size_t end_pos = std::min(start_pos + chars_per_line, field_text.length());
        std::string line = field_text.substr(start_pos, end_pos - start_pos);
        start_pos = end_pos;
        counter++;
    }
    return counter;
}

/*
 *  Расчет базовой ширины ячейки таблицы при условии, что все ячейки имеют одинаковую ширину
 *  row_fields - вектор строк с текстом каждой ячейки в строке
 */
HPDF_REAL PDFDocument::CalcBaseColumnWidth(const std::vector<std::string> &row_fields) {
    return (HPDF_Page_GetWidth(page_) - 2 * kMargin) / row_fields.size();
}

/*
 *  Расчет максимальной высоты ячейки таблицы, в которую поместятся все строки с учетом возможных переносов
 *  base_row_height - базовая высота ячейки таблицы (высота шрифта + две половины высоты шрифта)
 *  base_column_width - базовая ширина ячейки таблицы (высота шрифта + две половины высоты шрифта)
 *  font_size - размер шрифта
 *  row_fields - вектор строк с текстом каждой ячейки в строке
 */
HPDF_REAL PDFDocument::CalcMaxColumnHeight(HPDF_REAL base_row_height, HPDF_REAL base_column_width, HPDF_REAL font_size, const std::vector<std::string> &row_fields) {
    HPDF_REAL max_row_height = base_row_height;

    for (const auto &field: row_fields) {
        HPDF_REAL text_width = HPDF_Page_TextWidth(page_, field.c_str());
        int text_rows_counter = 1;
        // если ширина текста в ячейке больше ширины ячейки за вычетом двух "заполнителей"
        if (text_width > (base_column_width - 2 * kLeftRightPadding)) {
            // то придется переносить текст на следующую строку (строка таблицы начнет вмещать 2 и более строк текста)
            // для этого сделаем высоту строки таблицы больше
            text_rows_counter = CalcTextRowsInCell(field, CalcTextWidthInCell(base_column_width, text_width, field.length()));    //ceil(text_width / (base_column_width - 2 * kLeftRightPadding));
            HPDF_REAL required_height = text_rows_counter * (base_row_height - font_size/2.0) + font_size/2.0;
            max_row_height = std::max(max_row_height, required_height);
        }
    }
    return max_row_height;
}

void PDFDocument::AddTableRow(HPDF_REAL font_size, const std::vector<std::string> &row_fields) {
    HPDF_Page_SetFontAndSize(page_, font_, font_size);

    // Параметры таблицы:
    // ширина страницы
    HPDF_REAL page_width = HPDF_Page_GetWidth(page_);
    // ширина таблицы на странице (ширина страницы без левого и правого отступа от краев)
    HPDF_REAL table_width = page_width - 2 * kMargin;
    // ширина столбца в таблице
    // TODO: динамическая ширина столбца
    HPDF_REAL base_column_width = CalcBaseColumnWidth(row_fields);
    // высота строки по умолчанию - размер шрифтра и еще полразмера сверху и снизу
    HPDF_REAL base_row_height = font_size * 2;

    // 1. Предварительный расчет максимальной высоты строки таблицы.
    // Учитывает возможную необходимость переноса строки текста в рамках ячейки таблицы
    HPDF_REAL max_row_height = CalcMaxColumnHeight(base_row_height, base_column_width, font_size, row_fields);

    // 2. Проверка места на странице
    if (cursor_.y - max_row_height < kMargin) { //  kMargin + 2 * kLineSpacing
        try {
            AddNewPage();
            HPDF_Page_SetFontAndSize(page_, font_, font_size);
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

    // y_bottom_of_row - координата Y нижней границы строки с учетом рассчитанной максимальной высоты строки
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
    AddTextToTableRow(max_row_height, font_size, row_fields);

    // 5. Обновляем позицию курсора
    cursor_.y = y_bottom_of_row;
}

void PDFDocument::AddTextToTableRow(HPDF_REAL row_height, HPDF_REAL font_size, const std::vector<std::string> &row_fields) {
    float x_pos_in_row = kStartPosX;
    HPDF_Page_BeginText(page_);
    HPDF_REAL base_column_width = CalcBaseColumnWidth(row_fields);

    for (const auto &field: row_fields) {
        HPDF_REAL text_width = HPDF_Page_TextWidth(page_, field.c_str());

        if (text_width <= (base_column_width - 2 * kLeftRightPadding)) {
            // Однострочный текст
            HPDF_REAL text_x = x_pos_in_row + kLeftRightPadding; /*+ (base_column_width - text_width) / 2*/;
            // для вертикальной позиции выбираем положение по середнине
            HPDF_REAL text_y = cursor_.y - row_height / 2 - font_size / 3;
            HPDF_Page_TextOut(page_, text_x, text_y, field.c_str());
        } else {
            // Многострочный текст
            HPDF_REAL available_width_of_cell = base_column_width - 2 * kLeftRightPadding;
            size_t chars_per_line = field.length() * (available_width_of_cell / text_width);

            HPDF_REAL current_y = cursor_.y - kLeftRightPadding;
            size_t start_pos = 0;
            while (start_pos < field.length()) {
                size_t end_pos = std::min(start_pos + chars_per_line, field.length());
                std::string line = field.substr(start_pos, end_pos - start_pos);

                HPDF_REAL line_width = HPDF_Page_TextWidth(page_, line.c_str());
                HPDF_REAL text_x = x_pos_in_row + kLeftRightPadding; /*(base_column_width - line_width) / 2*/;

                HPDF_Page_TextOut(page_, text_x, current_y - font_size, line.c_str());
                current_y -= font_size + font_size/2.0;
                start_pos = end_pos;
            }
        }
        x_pos_in_row += base_column_width;
    }

    HPDF_Page_EndText(page_);
}