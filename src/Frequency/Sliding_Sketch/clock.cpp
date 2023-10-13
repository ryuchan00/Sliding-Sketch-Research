#include "clock.h"

struct Place{
    unsigned int serial;
    unsigned int pos;
};
bool operator < (Place a, Place b){
    return a.serial < b.serial;
}

int Recent_Sketch::Mid(int *num){
    if(hash_number & 1){
        return max(num[hash_number >> 1], 0);
    }
    else{
        return max(0, (num[hash_number >> 1] + num[(hash_number >> 1) - 1]) >> 1);
    }
}

Recent_Counter::Recent_Counter(int c, int l, int _row_length, int _hash_numberber, int _field_num):
    Recent_Sketch(c,l,_row_length,_hash_numberber,_field_num){
    counter = new Unit [l];
    field_num = _field_num;
    for(int i = 0; i < l; i++){
        counter[i].count = new int[_field_num];
        counter[i].field_num = _field_num;
        memset(counter[i].count, 0, _field_num * sizeof(int));
    }
}

Recent_Counter::~Recent_Counter(){
    delete [] counter;
}

void Recent_Counter::CM_Init(const unsigned char* str, int length, unsigned long long int num){
    unsigned int position;
    Clock_Go(num * step);
    for(int i = 0;i < hash_number;++i){
        position = Hash(str, i, length) % row_length + i * row_length;
        // yesterdayかtodayかの判定か
        counter[position].count[(cycle_num + (position < clock_pos)) % field_num] += 1;
    }
}

void Recent_Counter::CU_Init(const unsigned char* str, int length, unsigned long long int num){
    // clock_posの初期値は0だった
    // row_lengthは固定
    int k = clock_pos / row_length;
    // ここでclock_posを更新している
    Clock_Go(num * step);
    // k * row_length は多分オフセットかな
    unsigned int position = Hash(str, k ,length) % row_length + k * row_length;
    // yesterdayかtodayかの判定か，違うかな？
    if(position < clock_pos){
        k = (k + 1) % hash_number;
        position = Hash(str, k ,length) % row_length + k * row_length;
    }

    unsigned int height = counter[position].count[(cycle_num + (position < clock_pos)) % field_num];
    counter[position].count[(cycle_num + (position < clock_pos)) % field_num] += 1;

    for(int i = (k + 1) % hash_number;i != k;i = (i + 1) % hash_number){
        position = Hash(str, i ,length) % row_length + i * row_length;
        if(counter[position].count[(cycle_num + (position < clock_pos)) % field_num] <= height){
            height = counter[position].count[(cycle_num + (position < clock_pos)) % field_num];
            counter[position].count[(cycle_num + (position < clock_pos)) % field_num] += 1;
        }
    }
}

unsigned int Recent_Counter::Query(const unsigned char* str, int length){
    unsigned int min_num = 0x7fffffff;

    for(int i = 0;i < hash_number;++i)
        min_num = min(counter[Hash(str, i, length) % row_length + i * row_length].Total(), min_num);

    return min_num;
}

static int Count_Hash[2] = {-1, 1};
void Recent_Counter::CO_Init(const unsigned char *str, int length, unsigned long long num){
    unsigned int position;
    Clock_Go(num * step);
    for(int i = 0;i < hash_number;++i){
        position = Hash(str, i, length) % row_length + i * row_length;
        counter[position].count[(cycle_num + (position < clock_pos)) % field_num] +=
                Count_Hash[(str[length - 1] + position) & 1];
    }
}


int Recent_Counter::CO_Query(const unsigned char *str, int length){
    int* n = new int[hash_number];
    memset(n, 0, hash_number * sizeof(int));

    for(int i = 0;i < hash_number;++i)
    {
        unsigned int position = Hash(str, i, length) % row_length + i * row_length;
        /*
        if(clock_pos >= position){
            n[i] = (counter[position].Total() * Count_Hash[(str[length - 1] + position) & 1]) * row_length * hash_number / (row_length * hash_number + clock_pos - position);
        }else{
            n[i] = (counter[position].Total() * Count_Hash[(str[length - 1] + position) & 1]) * row_length * hash_number / (row_length * hash_number * 2 - (position - clock_pos));

        }*/
        n[i] = counter[position].Total() * Count_Hash[(str[length - 1] + position) & 1] ;
        
    }

    std::sort(n, n + hash_number);

    return Mid(n);
}

void Recent_Counter::Clock_Go(unsigned long long int num){
    // coutで細かい値を見ていく
    // 最初はスケッチの値を小さくするなどして工夫してみる
    
    std::cout << "last_time" << last_time << std::endl;
    for(;last_time < num;++last_time){
        int display_flag = 0;
        // std::cout << "clock_pos= " << clock_pos << std::endl;
        if (counter[clock_pos].count[0] != 0 && counter[clock_pos].count[1] != 0) {
            std::cout << counter[clock_pos].count[0] << " " << counter[clock_pos].count[1] << " => ";
            display_flag = 1;
        }
        counter[clock_pos].count[(cycle_num + 1) % field_num] = 0;
        if (display_flag == 1) {
            std::cout << counter[clock_pos].count[0] << " " << counter[clock_pos].count[1] << std::endl;
        }
        clock_pos = (clock_pos + 1) % len;
        if(clock_pos == 0){
            // field_numのどちらがTodayですか？
            cycle_num = (cycle_num + 1) % field_num;
        }
    }
    // std::cout << "len,field_num:";
    // std::cout << std::endl;
    // std::cout << len << " ";
    // std::cout << field_num;
    // std::cout << std::endl;
    // for(int i = 0;i < field_num;++i){
        // for(int j = 0;j < field_num;++j){
            // std::cout << counter[i].count[j] << " ";
        // }
        // std::cout << counter[clock_pos].count[i] << std::endl;
    // }
    
}
