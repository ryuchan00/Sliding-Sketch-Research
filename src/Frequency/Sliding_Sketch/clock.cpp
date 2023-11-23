#include "clock.h"

#include <math.h>                                                               

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

void Recent_Counter::CM_Init(const unsigned char* str, int length, unsigned long long int num){
    unsigned int position;
    // 自信のインスタンスのメンバ変数clock_posが，速さ1か速さ1.1の領域のどちらを参照しているか判定する
    // 速さ1の領域を参照している場合は，Clock_Go(num * step)をcall
    // 速さ1.1の領域を参照している場合は，Clock_Go(num * step * 1.1)をcall
    Clock_Go(num * step);
    for(int i = 0;i < hash_number;++i){
        position = Hash(str, i, length) % row_length + i * row_length;
        std::cout << "i: " << i << " position: " << position << std::endl;
        // yesterdayかtodayかの判定
        if (position % 2 == 0) {
            counter[position].count[(cycle_num + (position < clock_pos)) % field_num] += 1;
        } else {
            // 飛んだケースは存在するか？
            counter[position].count[(cycle_num2 + (position < (int)clock_pos2)) % field_num] += 1;
        }
    }
}

void Recent_Counter::CU_Init(const unsigned char* str, int length, unsigned long long int num){
    // clock_posの初期値は0だった
    // row_lengthは固定
    int k = clock_pos / row_length;
    // ここでclock_posを更新している
    Clock_Go(num * step);
    // k * row_length は多分オフセット
    unsigned int position = Hash(str, k ,length) % row_length + k * row_length;
    // yesterdayかtodayかの判定
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
    // counterのclock_posを見て，今スケッチのどの場所を見ているか判定する
    // もし速度1のスケッチを参照している状態であればそのまま処理する
    // そうでなければ，numの値を1.1倍して，last_timeをnumに追従させる
    // そうすることによって，スケッチの更新速度を1.1として表現できる？
    // したがってColock_Goをcallするときの引数numを調整してあげる必要がある
    for(;last_time < num;++last_time){
        // 以下の2つの行を2倍してあげればいけるか？
        // counter[clock_pos2].count[(cycle_num2 + 1) % field_num] = 0;
        // clock_pos2 = (clock_pos2 + 1.1) % len;
        // clock_pos2がdouble or floatになったときにcounterの添え字intなので飛ぶ
        // 以下のような事が発生する
        // if (clock_pos == even) {このときに速度1.1のcounterを参照するので処理をしない}
        // counterのスケッチの速度による領域をevenとodd
        std::cout << "num: " << num << " clock_pos: " << clock_pos << " clock_pos2: " << clock_pos2 << std::endl;
        std::cout << "cycle_num: " << cycle_num << " cycle_num2: " << cycle_num2 << std::endl;

        if (clock_pos % 2 == 0) {
            counter[clock_pos].count[(cycle_num + 1) % field_num] = 0;
        }
        clock_pos = (clock_pos + 1) % len;
        if(clock_pos == 0){
            cycle_num = (cycle_num + 1) % field_num;
        }

        if ((int)clock_pos2 % 2 != 0) {
            counter[(int)clock_pos2].count[(cycle_num2 + 1) % field_num] = 0;
        }
        if ((int)(clock_pos2 - prev_clock_pos2) > 1) {
            int n = (int)(clock_pos2 - prev_clock_pos2);
            for(int i=0;i<n;i++) {
                int pos = prev_clock_pos2 + i + 1; 
                if ((int)pos % 2 != 0) {
                    counter[(int)pos].count[(cycle_num2 + 1) % field_num] = 0;
                }
            }
        }
        if ((prev_clock_pos2 > clock_pos2) && (prev_clock_pos2 - clock_pos2 + len > 1)) {
            int n = (int)(prev_clock_pos2 - clock_pos2 + len);
            for(int i=0;i<n;i++) {
                int pos = (int)(prev_clock_pos2 + i + 1) % len; 
                if ((int)pos % 2 != 0) {
                    counter[(int)pos].count[(cycle_num2 + 1) % field_num] = 0;
                }
            }
        }
        prev_clock_pos2 = clock_pos2;
        clock_pos2 = fmodf(clock_pos2 + 1.1, len);
        if((int)clock_pos2 == 0){
            cycle_num2 = (cycle_num2 + 1) % field_num;
        }
        // 0を飛ばした
        if ((prev_clock_pos2 > clock_pos2) && (prev_clock_pos2 - clock_pos2 + len > 1)) {
            cycle_num2 = (cycle_num2 + 1) % field_num;
        }
        
        for(int i=0;i<len;i++) {
            if (i % row_length == 0 && i != 0) {
                std::cout << std::endl;
            }
            std::cout << "(" << counter[i].count[0] << "," << counter[i].count[1] << ") ";
            
        }
                     std::cout <<  std::endl;
        std::cout << "-------------" << std::endl;  
        std::cout <<  std::endl;
    }
}
