#include "GUI.h"

extern FILE* output;

//Overloaded function jsonStringAdder inserts values into the output.json file in the appropriate format.

// add string: "key":"value"
void jsonStringAdder(std::string key, std::string value)
{   
    std::string insert_this = "\""+key+"\""+":"+"\""+value+"\"";
    fwrite(&insert_this,1,insert_this.size(),output);
    return;
}
// add string: key space
void jsonStringAdder(char key, char space)
{   
    std::string insert_this;
    insert_this = insert_this+key+space;
    fwrite(&insert_this,1,2,output);
    return;
}
// add string: "key"
void jsonStringAdder(std::string key)
{   
    std::string insert_this = "\""+key+"\"";
    fwrite(&insert_this,1,insert_this.size(),output);
    return;
}
// add string: "key": value
void jsonStringAdder(std::string key, int value)
{   
    std::string insert_this = "\""+key+"\""+":"+std::to_string(value);
    fwrite(&insert_this,1,insert_this.size(),output);
    return;
}
// add string: number buffer
void jsonStringAdder(int key, char buffer)
{   
    std::string insert_this = std::to_string(key)+buffer;
    fwrite(&insert_this,1,insert_this.size(),output);
    return;
}
// add string: "key": "buffer"
void jsonStringAdder(uint32_t key, uint8_t buffer)
{   
    std::string insert_this = "\""+std::to_string(key)+"\""+":"+"\""+std::to_string(buffer)+"\"";
    fwrite(&insert_this,1,insert_this.size(),output);
    return;
}
// add string: "key": a[0]a[1]a[2]a[3]....a[31]
void jsonStringAdder(std::string key, int a[32])
{
    char str[]="00000000000000000000000000000000";
    for (int i=0; i<32; i++)
    {
        (a[31-i]==0)? str[i]='0': str[i]='1';
    }
    jsonStringAdder(key);
    jsonStringAdder(':','"');
    for (int i=0; i<32; i++)
    {
        jsonStringAdder(str[i],str[i+1]);
        i++;
    }
    jsonStringAdder('"',' ');
    return;
}