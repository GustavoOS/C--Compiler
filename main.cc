/****************************************************/
/* File: main.c                                     */
/* Main program for TINY compiler                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/* Adapted by Gustavo O. Souza                      */
/****************************************************/
#include "globals.h"
#include <iostream>
#include "library.h"
/* set NO_PARSE to TRUE to get a scanner-only compiler */
#define NO_PARSE FALSE
/* set NO_ANALYZE to TRUE to get a parser-only compiler */
#define NO_ANALYZE FALSE

/* set NO_CODE to TRUE to get a compiler that does not
 * generate code
 */
#define NO_CODE TRUE

#include "util.h"
#if NO_PARSE
#include "scan.h"
#else
#include "parse.h"
#if !NO_ANALYZE
#include "analyze.h"
#if !_CODE_GENERATOR_
#include "codegenerator.h"
#include "binarygenerator.h"
#include "linker.h"
#endif
#endif
#endif

/* allocate global variables */
int lineno;
FILE *originalSource;
FILE *source;
FILE *listing;
FILE *code;

/* allocate and set tracing flags */
int EchoSource = FALSE;
int TraceScan = FALSE;
int TraceParse = FALSE;
bool TraceLink = false;

int Error = FALSE;

// Mark True for OS mode development
bool isOperatingSystem = false;
bool isBios = false;
bool isCompressedProgram = true;

int PROGRAM_OFFSET = 2060;

int main(int argc, char *argv[])
{
    TreeNode *syntaxTree;
    std::string pgm; /* source code file name */
    std::string outputFile = "output.txt";
    if (argc != 2 && argc != 3)
    {
        fprintf(stderr, "usage: %s <filename> <output>\n", argv[0]);
        exit(1);
    }

    if (argc == 3)
        outputFile = argv[2];

    pgm = std::string(argv[1]);
    if (pgm.find('.') == std::string::npos)
        pgm = pgm + ".c";
    originalSource = fopen(pgm.c_str(), "r");
    if (originalSource == NULL)
    {
        fprintf(stderr, "File %s not found\n", pgm.c_str());
        exit(1);
    }
    fclose(originalSource);
    listing = stdout; /* send listing to screen */
    LibraryIncluder *includer = new LibraryIncluder();
    if (isOperatingSystem || isBios)
        includer = dynamic_cast<LibraryIncluder *>(new OSLibraryIncluder());
    includer->setFileSource(pgm);
    source = includer->getFinalFile();
    lineno = -includer->library.size();
    // lineno = 0;
    fprintf(listing, "\nC- COMPILATION: %s\n", pgm.c_str());
#if NO_PARSE
    while (getToken() != ENDFILE)
        ;
#else
    syntaxTree = parse();
    if (TraceParse)
    {
        fprintf(listing, "\nSyntax tree:\n");
        printTree(syntaxTree);
    }
#if !NO_ANALYZE
    if (Error)
    {
        fclose(source);
        return 1;
    }

    if (TraceAnalyze)
        fprintf(listing, "\nBuilding Symbol Table...\n");
    buildSymtab(syntaxTree);

    if (Error)
    {
        fclose(source);
        return 1;
    }

    if (TraceAnalyze)
        fprintf(listing, "\nChecking for main Function...\n");
    mainVerify();

    if (Error)
    {
        fclose(source);
        return 1;
    }

    if (TraceAnalyze)
        fprintf(listing, "\nChecking Types...\n");
    typeCheck(syntaxTree);
    if (TraceAnalyze)
        fprintf(listing, "\nType Checking Finished\n");

    if (Error)
    {
        fclose(source);
        return 1;
    }

    CodeGenerator cg;
    cg.setTerminalDebug(TraceCode);
    cg.setMode(isBios, isOperatingSystem);
    if (TraceCode)
    {
    std::cout << "\nStarting code generation process\n";
    std::cout << "--------------------------------------\n\n\n";
    }
    cg.generate(syntaxTree);

    (new Linker())
        ->setDebugMode(!TraceLink)
        ->withCode(cg.getCode())
        ->withDestMap(cg.getDestMap())
        ->withOriginMap(cg.getOriginMap())
        ->withOffset(PROGRAM_OFFSET)
        ->link();

    BinaryGenerator *bGenerator =
        BinaryGeneratorFactory::generate(
            isCompressedProgram,
            isBios,
            isOperatingSystem);
    bGenerator->setFile(outputFile);
    bGenerator->setOffset(PROGRAM_OFFSET);
    bGenerator->run(cg.getCode());

#endif
#endif
    fclose(source);
    return 0;
}
