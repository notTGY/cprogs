/*
  В начале программы директивы препроцессора
*/
// Начало мантры

#include <iostream>
#include <cstdlib>
#include <math.h>
using namespace std;
// Конец директив и выбора пространства имён

class Time{
  public:
   int hour;
   int min;
};

Time add(Time a, int min){
  Time res;
// Здесь нужно написать код
  int total_min = a.min + min;
  res.min = total_min % 60;
  int overflow_hour = (total_min - res.min) / 60;
  res.hour = (overflow_hour + a.hour) % 24;

  return res;
}


int main() {
    Time a,newtime;
    int min;
    
    // Получить a  из консоли 


    cin>>a.hour>>a.min;
   // Получить min  из консоли 

    cin>>min;
    newtime = add(a,min);
    cout << newtime.hour << ":" << newtime.min << endl;
}
