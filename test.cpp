#include<iostream>
#include<stdlib.h>
#include<chrono>
#include "include/asm-xml.h"

static char buffer[65536000];
static const int chunkSize = 1*1024*1024;
static const char* schemaFilename = "./data/employees-schema.xml";
static const char* xmlFilename = "./data/employees-big.xml";


AXElementClass* readClass(const char* filename, AXClassContext* classContext) {
  FILE*    f;
  size_t  size;
  
  f = fopen(filename, "rb");
  if( f == NULL ) {
    fprintf(stderr, "can't open schema '%s'\n", filename);
    return NULL;
  }
  size = fread(buffer, 1, 65535000, f);
  buffer[size] = 0;
  fclose(f);

  // Parse the string and build the class
  return ax_classFromString(buffer, classContext);
}


AXElement* readDocument(const char*     filename, AXParseContext* parseContext,AXElementClass* clazz) {
  FILE*    f;
  size_t  size;
  
  f = fopen(filename, "rb");
  if( f == NULL ) {
    fprintf(stderr, "can't open file '%s'\n", filename);
    return NULL;
  }
  size = fread(buffer, 1, 65535000, f);
  buffer[size] = 0;
  fclose(f);

  // Parse the string and build the class
  return ax_parse(parseContext, buffer, clazz, 1);
}

std::string getAXAttributeValue(AXAttribute* employee) {
  return std::string(employee->begin, employee->limit);
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    ax_initialize((void*)malloc, (void*)free);

    // Initialize a class context
    AXClassContext classContext;
    int res = ax_initializeClassParser(&classContext);
    if(res != 0) return 1;

    // schemaFilename is pointer to the schema structure
    AXElementClass* rootClass = readClass(schemaFilename, &classContext);
    if (rootClass == NULL) return 1;

    // Initialize a Parser context 
    AXParseContext parseContext;
    res = ax_initializeParser(&parseContext, chunkSize);
    if(res != 0) return 1;

    AXElement* root = readDocument(xmlFilename, &parseContext, rootClass);
    if(root == NULL) return 1;

    // get the first element or linkedList
    /*
    AXElement* employee = root->firstChild;

    while( employee ) {
        std::cout << "Id.........: " << getAXAttributeValue(&employee->attributes[0]) << std::endl;
        std::cout << "Manager....: " << getAXAttributeValue(&employee->attributes[1]) << std::endl;
        std::cout << "First Name.: " << getAXAttributeValue(&employee->attributes[2]) << std::endl;
        std::cout << "Last Name..: " << getAXAttributeValue(&employee->attributes[3]) << std::endl;
	std::cout << "Email......: " << getAXAttributeValue(&employee->attributes[4]) << std::endl;
	std::cout << "Postion....: " << getAXAttributeValue(&employee->attributes[5]) << std::endl;
	std::cout << std::endl;

        employee = employee->nextSibling;
    }
    */
    // cleanUp things from memory
    ax_releaseParser(&parseContext);
    ax_releaseClassParser(&classContext);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "\nExecution time: " << duration.count() << " seconds with AsmXml" << std::endl;
    return 0;
}
