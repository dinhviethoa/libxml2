/*
 * testSAX.c : a small tester program for parsing using the SAX API.
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 */

#include "libxml.h"

#include <string.h>
#include <stdarg.h>

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif


#include <libxml/xmlreader.h>

int debug = 0;
int dump = 0;
int noent = 0;
int count = 0;

static void usage(const char *progname) {
    printf("Usage : %s [options] XMLfiles ...\n", progname);
    printf("\tParse the XML files using the xmlTextReader API\n");
    printf("\tand output the result of the parsing\n");
    exit(1);
}
static int elem, attrs;

static void processNode(xmlTextReaderPtr reader) {
    int type;

    type = xmlTextReaderNodeType(reader);
    if (count) {
	if (type == 1) {
	    elem++;
	    attrs += xmlTextReaderAttributeCount(reader);
	}
    } else {
	xmlChar *name = xmlTextReaderName(reader);
	if (name != NULL) {
	    printf("%s : %d", name, xmlTextReaderNodeType(reader));
	    xmlFree(name);
	} else {
	    printf("NULL: %d", xmlTextReaderNodeType(reader));
	}
	if (xmlTextReaderIsEmptyElement(reader))
	    printf(" empty");
	printf("\n");
    }
}

static void handleFile(const char *filename) {
    xmlTextReaderPtr reader;
    int ret;

    if (count) {
	elem = 0;
	attrs = 0;
    }

    reader = xmlNewTextReaderFilename(filename);
    if (reader != NULL) {
	/*
	 * Process all nodes in sequence
	 */
	ret = xmlTextReaderRead(reader);
	while (ret == 1) {
	    processNode(reader);
	    ret = xmlTextReaderRead(reader);
	}

	/*
	 * Done, cleanup and status
	 */
	xmlFreeTextReader(reader);
	if (ret != 0) {
	    printf("%s : failed to parse\n", filename);
	} else if (count)
	    printf("%s : %d elements, %d attributes\n", filename, elem, attrs);
    } else {
	fprintf(stderr, "Unable to open %s\n", filename);
    }
}

int main(int argc, char **argv) {
    int i;
    int files = 0;

    if (argc <= 1) {
	usage(argv[0]);
	return(1);
    }
    LIBXML_TEST_VERSION
    for (i = 1; i < argc ; i++) {
	if ((!strcmp(argv[i], "-debug")) || (!strcmp(argv[i], "--debug")))
	    debug++;
	else if ((!strcmp(argv[i], "-dump")) || (!strcmp(argv[i], "--dump")))
	    dump++;
	else if ((!strcmp(argv[i], "-count")) || (!strcmp(argv[i], "--count")))
	    count++;
	else if ((!strcmp(argv[i], "-noent")) ||
	         (!strcmp(argv[i], "--noent")))
	    noent++;
    }
    if (noent != 0) xmlSubstituteEntitiesDefault(1);
    for (i = 1; i < argc ; i++) {
	if (argv[i][0] != '-') {
	    handleFile(argv[i]);
	    files ++;
	}
    }
    xmlCleanupParser();
    xmlMemoryDump();

    return(0);
}