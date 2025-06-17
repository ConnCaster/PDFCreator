#include "pdfcreator/pdfcreator.h"
#include "pdfcreator/xmlcreator.h"

int main() {

    XMLBuilder builder;
    TestXMLDirector test_xml_director{builder};
    test_xml_director.CreateDocument();

    IDocument* xml_document =  builder.GetDocument();
    try {
        xml_document->AddTableHeaders(10, kHeaders_, {});

        xml_document->AddTableRow(10, kHeaders_, {}, {});
        xml_document->AddTableRow(10, kHeaders_, {}, {});

        dynamic_cast<XmlDocument*>(xml_document)->EndTable();
        xml_document->AddTableRow(10, kHeaders_, {}, {});

    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }


    xml_document->SaveToFile("/home/user/dir/PDFCreator/out.xml");

    // PDFBuilder builder;
    // TestPDFDirector test_pdf_director{builder};
    // test_pdf_director.CreateDocument();
    // IDocument* pdf_document =  builder.GetDocument();
    // pdf_document->SaveToFile("/home/user/dir/programming/confident/PDFCreator/out.pdf");

    return 0;
}
