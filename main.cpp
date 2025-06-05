#include <iostream>

#include "pdfcreator/pdfcreator.h"

int main() {

    PDFDocument pdf_document{};
    /*pdf_document.AddHeader( json::parse(
        R"(
            [
                {"name": "Document", "value": "Annual Report"},
                {"name": "Author", "value": "Джон Доу"},
                {"name": "Дата", "value": "2023-05-15"},
                {"name": "1 Касперский Endpoint Security", "value": "2023-05-15 @ 10//20\\30;:"},
                {"name": "Дата", "value": "2023-05-15"},
                {"name": "2 Касперский Endpoint Security", "value": "2023-05-15 @ 10//20\\30;:"},
                {"name": "Дата", "value": "2023-05-15"},
                {"name": "3 Касперский Endpoint Security", "value": "2023-05-15 @ 10//20\\30;:"},
                {"name": "Дата", "value": "2023-05-15"},
                {"name": "4 Касперский Endpoint Security", "value": "2023-05-15 @ 10//20\\30;:"},
                {"name": "Дата", "value": "2023-05-15"},
                {"name": "5 Касперский Endpoint Security", "value": "2023-05-15 @ 10//20\\30;:"},
                {"name": "Дата", "value": "2023-05-15"},
                {"name": "6 Касперский Endpoint Security", "value": "2023-05-15 @ 10//20\\30;:"},
                {"name": "Дата", "value": "2023-05-15"},
                {"name": "7 Касперский Endpoint Security", "value": "2023-05-15 @ 10//20\\30;:"},
                {"name": "Дата", "value": "2023-05-15"},
                {"name": "8 Касперский Endpoint Security", "value": "2023-05-15 @ 10//20\\30;:"},
                {"name": "Дата", "value": "2023-05-15"},
                {"name": "9 Касперский Endpoint Security", "value": "2023-05-15 @ 10//20\\30;:"},
                {"name": "Дата", "value": "2023-05-15"},
                {"name": "9 Касперский Endpoint Security", "value": "2023-05-15 @ 10//20\\30;:"},
                {"name": "Дата", "value": "2023-05-15"},
                {"name": "9 Касперский Endpoint Security", "value": "2023-05-15 @ 10//20\\30;:"},
                {"name": "Дата", "value": "2023-05-15"}
            ]
        )"));*/
    pdf_document.AddTable();
    pdf_document.SaveToFile("/home/user/dir/PDFCreator/out.pdf");

    return 0;
}
