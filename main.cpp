#include <iostream>
#include <mbase/common.h>
#include <mbase/allocator.h>
#include <mbase/string.h>
#include <unordered_map>
#include <algorithm>

using namespace mbase;

int main() {
    
    mbase::character_sequence abc;
    abc = "arabasd";
    mbase::character_sequence aaa = "arabsasd";
    
    aaa += " buda yenii";

    getchar();
    return 0;
}