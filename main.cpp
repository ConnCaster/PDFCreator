#include "pdfcreator/pdfcreator.h"
#include "pdfcreator/xmlcreator.h"

int main() {

    ODSBuilder builder;
    TestODSDirector director{builder};
    director.CreateDocument();
    IDocument* document =  builder.GetDocument();
    document->SaveToFile("/home/user/dir/PDFCreator/out.ods");

    // PDFBuilder builder;
    // TestPDFDirector test_pdf_director{builder};
    // test_pdf_director.CreateDocument();
    // IDocument* pdf_document =  builder.GetDocument();
    // pdf_document->SaveToFile("/home/user/dir/programming/confident/PDFCreator/out.pdf");

    return 0;
}
