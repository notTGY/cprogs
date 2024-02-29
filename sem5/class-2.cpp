/*
Эту часть сдавать НЕ надо. Она будет вставлена ПЕРЕД Вашим кодом.
*/

#include <iostream>
#include <cstdlib>
#include <math.h>
using namespace std;

class Time{
  public:
   int hour;
   int min;
};

/*
А вот здесь начинается Ваша функция, которую Вы сдаёте:
*/

  int isEqual(Time a, int min){
// Здесь нужно написать код
  int res_min = min % 60;
  if (res_min != a.min) return 0;

  int res_hour = ((min - res_min) / 60) % 24;
  if (res_hour != a.hour) return 0;

  return 1;
}


/*
Здесь Ваша функция кончается. Для удобства отладки приведено то, что будет вставлено ПОСЛЕ  Вашего кода.
*/



int main() {
    Time a,newtime;
    int min;
    int res;
    
    // Получить a  из консоли 


    cin>>a.hour>>a.min;
   // Получить min  из консоли 

    cin>>min;
    res = isEqual(a,min);
    cout <<res<< endl;
}
