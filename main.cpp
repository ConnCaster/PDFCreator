#include "pdfcreator/pdfcreator.h"
#include "pdfcreator/xmlcreator.h"

int main() {

    ODSBuilder builder;
    TestODSDirector director{builder};
    director.CreateDocument();
    IDocument* document =  builder.GetDocument();
    document->SaveToFile("/home/user/dir/programming/confident/PDFCreator/out.ods");

    // XMLBuilder builder;
    // TestXMLDirector test_xml_director{builder};
    // test_xml_director.CreateDocument();
    // IDocument* xml_document =  builder.GetDocument();
    // xml_document->SaveToFile("/home/user/dir/programming/confident/PDFCreator/out.xml");

    // PDFBuilder builder;
    // TestPDFDirector test_pdf_director{builder};
    // test_pdf_director.CreateDocument();
    // IDocument* pdf_document =  builder.GetDocument();
    // pdf_document->SaveToFile("/home/user/dir/programming/confident/PDFCreator/out.pdf");

    return 0;
}
