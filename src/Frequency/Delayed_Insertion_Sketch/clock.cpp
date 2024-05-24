#include "clock.h"

#include <math.h>                                                               
#include <algorithm>
#include <cstring>

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

    int target_index = GetTargetKeyIndex(str2);
    int correction_count = 0;
    
    //if (target_index != -1) {
        correction_count = element_count_2_.at(GetTargetKey(str));
    //}
    //std::cout << correction_count << std::endl;

    for(int i = 0;i < hash_number;i++) {
        min_num = min(counter[Hash(str, i, length) % row_length + i * row_length].count[0] + correction_count, min_num);
    }
    return min_num;
}

void Recent_Counter::DelayedInsertion_CM_Init(const unsigned char* str, int length, unsigned long long int num) {
    //std::cout << str << std::endl;
    Initilize_ElementCount(length, num);

    // std::string str2 = std::string((char*)str); // string型へ変換

    //int target_key = GetTargetKeyIndex(str2);
    //std::cout << target_key << std::endl;

    if (element_count_2_.count(GetTargetKey(str)) != 0) {
        element_count_2_.at(GetTargetKey(str))++;
    } else {
        element_count_2_.insert(std::make_pair(GetTargetKey(str), 1));
    }

    // if (element_count_2_[GetTargetKey(str)] == 0) {
    //     // not found
    //     //element_count_.at(str2) = 1;
    //     //Frequency freq;
    //     //freq.key = str2;
    //     //freq.count = 1;
    //     element_count_2_.push_back(freq);
    //     //  std::cout << "then" << std::endl;
    //     //std::cout << element_count_[str2] << std::endl;  
    // } else {
    //     element_count_2_.at(target_key).count = element_count_2_.at(target_key).count + 1;
    //     //  std::cout << "else" << std::endl;
    //     //std::cout << element_count_[str2] << std::endl;  
    // }
    // std::cout << element_count_2_.size() << std::endl;
    //std::cout << str << std::endl;
}

void Recent_Counter::Initilize_ElementCount(int length, unsigned long long int num) {
    unsigned int position;
    //std::cout << element_count_step_ << std::endl;
    int frequency_confirmations[row_length] = {0};

    // for(;last_time < num;++last_time){
        if (num % element_count_step_ == 0) {

            for (int i = 0; i < hash_number; i++) {
                frequency_confirmations[row_length] = {0};
                //Hash(str, i, length) % row_length + i * row_length
                
                //for (auto itr = element_count_.begin(); itr != element_count_.end(); itr++) {
                for( const auto& [key, value] : element_count_2_) {
                    // キャッシュの衝突を検知する
                    //auto target_key = reinterpret_cast<unsigned char*>(const_cast<char*>(itr->first.c_str()));
                    unsigned char a[DATA_LEN];
                    for(int i = 0; DATA_LEN > i; i++) {
                        // std::cout << key[i];
                        a[i] = key[i];
                    }
                    //  std::cout << a << std::endl;
                    position = Hash(a, i, length) % row_length;
                    frequency_confirmations[position] = max(frequency_confirmations[position], value);
                }

                for (int j = 0; j < row_length; j++) {
                    if (frequency_confirmations[j] > 0) {
                        counter[j + i * row_length].count[0] = counter[j + i * row_length].count[0] + frequency_confirmations[j];
                        // std::cout << counter[j + i * row_length].count[0] << std::endl;
                    }
                }
            }
            element_count_2_.clear();
        }
    // }
}

int Recent_Counter::GetTargetKeyIndex(const unsigned char* str) {
    int i = 0;

    std::string str2 = std::string((char*)str); // string型へ変換
    // std::string str3 = std::string(reinterpret_cast<const char*>(str));

    //for (Frequency freq : element_count_2_) {
        // std::string freq_key2 = std::string((char*)freq.key); // string型へ変換
        // std::string freq_key2 = std::string(reinterpret_cast<const char*>(freq.key));

        //std::cout << freq.key << ":" << str << std::endl;
        // if (freq.key == str) {
        // if (freq_key2 == str2) {
        // if (std::memcmp(str, freq.key, DATA_LEN) == 0) {
        // if (freq_key2 == str3) {
            // std::cout << freq.key << ":" << str << std::endl;
            // std::cout << "then" << std::endl;
            // return i;
        // }
        // i++;
        //std::cout << "hoge" << std::endl;
    //}
    return -1;
}

int Recent_Counter::GetTargetKeyIndex(std::string str) {
    int i = 0;

    // std::string str3 = std::string(reinterpret_cast<const char*>(str));

    // for (Frequency freq : element_count_2_) {
        // std::string freq_key2 = std::string((char*)freq.key); // string型へ変換
        // std::string freq_key2 = std::string(reinterpret_cast<const char*>(freq.key));

        //std::cout << freq.key << ":" << str << std::endl;
        // if (freq.key == str) {
        // if (freq_key2 == str2) {
        // if (str == freq.key) {
        // if (freq_key2 == str3) {
            // std::cout << freq.key << ":" << str << std::endl;
            // std::cout << "then" << std::endl;
            // return i;
        // }
        // i++;
        //std::cout << "hoge" << std::endl;
    // }
    return -1;
}

packet_str Recent_Counter::GetTargetKey(const unsigned char* str) {
    return {str[0],str[1],str[2],str[3],str[4],str[5],str[6],str[7]};
}