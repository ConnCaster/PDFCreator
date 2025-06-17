#include "pdfcreator/pdfcreator.h"
#include "pdfcreator/xmlcreator.h"

int main() {

    XMLBuilder builder;
    TestXMLDirector test_xml_director{builder};
    test_xml_director.CreateDocument();

    IDocument* xml_document =  builder.GetDocument();
    try {
        xml_document->AddTableHeaders(-1, kHeaders_, {});

        xml_document->AddTableRow(-1, kHeaders_, {}, {});
        xml_document->AddTableRow(-1, kHeaders_, {}, {});
        xml_document->AddTableRow(-1, {
            R"({"key": "value"})",
            R"({"key": "value"})",
            R"({"key": "value"})",
            R"({"key": "value"})"
        }, {}, {});

        dynamic_cast<XmlDocument*>(xml_document)->EndTable();
        xml_document->AddJSON(json::parse(
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
        xml_document->AddJSON(json::parse(
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

        dynamic_cast<XmlDocument*>(xml_document)->EndTable();
        xml_document->AddTableRow(-1, kHeaders_, {}, {});

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
