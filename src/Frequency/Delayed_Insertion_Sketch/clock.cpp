#include "clock.h"

#include <math.h>                                                               
#include <algorithm>

struct Place{
    unsigned int serial;
    unsigned int pos;
};
bool operator < (Place a, Place b){
    return a.serial < b.serial;
}

Recent_Counter::Recent_Counter(int c, int l, int _row_length, int _hash_numberber, int _field_num, int element_count_step):
    Recent_Sketch(c,l,_row_length,_hash_numberber,_field_num)
    , element_count_step_(element_count_step) {
    counter = new Unit [l];
    counter_DE = new Unit2[l];
    field_num = _field_num;
    row_length = _row_length;
    for(int i = 0; i < l; i++){
        counter[i].count = new int[_field_num];
        counter[i].field_num = _field_num;
        memset(counter[i].count, 0, _field_num * sizeof(int));
    }
}

Recent_Counter::~Recent_Counter(){
    delete [] counter;
}

unsigned int Recent_Counter::DelayedInsertion_CM_Query(const unsigned char* str, int length) {
    int min_num = 0x7fffffff;
    std::string str2 = std::string((char*)str); // string型へ変換

    for(int i = 0;i < hash_number;i++) {
        min_num = min(counter[Hash(str, i, length) % row_length + i * row_length].count[0] + element_count_[str2], min_num);
        //std::cout << counter[Hash(str, i, length) % row_length + i * row_length].count[0] << std::endl;
    }
    return min_num;
}

void Recent_Counter::DelayedInsertion_CM_Init(const unsigned char* str, int length, unsigned long long int num) {
    Initilize_ElementCount(length, num * step);

    std::string str2 = std::string((char*)str); // string型へ変換

    if (element_count_.count(str2) > 0) {
        element_count_[str2]++;
        //std::cout << element_count_[str2] << std::endl;  
    } else {
        element_count_[str2] = 1;
        //std::cout << element_count_[str2] << std::endl;  
    }
    //std::cout << str << std::endl;
}

void Recent_Counter::Initilize_ElementCount(int length, unsigned long long int num) {
    unsigned int position;
    //std::cout << element_count_step_ << std::endl;
    for(;last_time < num;++last_time){
        //std::cout << num << ":" << last_time << std::endl;
        if (last_time % element_count_step_ == 0) {
            //std::cout << "hoge" << std::endl;
            for (int i = 0; i < hash_number; i++) {
                int frequency_confirmations[row_length] = {0};
                
                for (auto itr = element_count_.begin(); itr != element_count_.end(); itr++) {
                    // キャッシュの衝突を検知する
                    auto uchrs = reinterpret_cast<unsigned char*>(const_cast<char*>(itr->first.c_str()));

                    position = Hash(uchrs, i, length) % row_length;
                    frequency_confirmations[position] = max(frequency_confirmations[position], (*itr).second);
                }

                for (int j = 0; j < row_length; j++) {
                    if (frequency_confirmations[j] > 0) {
                        //std::cout << "hoge" << std::endl;
                        counter[j + i * row_length].count[0] = counter[j + i * row_length].count[0] + frequency_confirmations[j];
                    }
                }
                //std::cout << "end2" << std::endl;
            }
            element_count_.clear();
        }
    }
    //std::cout << "end3" << std::endl;
}
