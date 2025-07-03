#ifndef GUI_H_
#define GUI_H_
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <string.h>


extern FILE* output;

//Overloaded function jsonStringAdder inserts values into the output.json file in the appropriate format.

// add string: "key":"value"
void jsonStringAdder(std::string key, std::string value);
// add string: key space
void jsonStringAdder(char key, char space);
// add string: "key"
void jsonStringAdder(std::string key);
// add string: "key": value
void jsonStringAdder(std::string key, int value);
// add string: number buffer
void jsonStringAdder(int key, char buffer);
// add string: "key": "buffer"
void jsonStringAdder(uint32_t key, uint8_t buffer);
// add string: "key": a[0]a[1]a[2]a[3]....a[31]
void jsonStringAdder(std::string key, int a[32]);

#endif