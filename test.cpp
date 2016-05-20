#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
using namespace std;
namespace randtest
{
	int test_1()
	{
	    int list[7] = {2,3,5,10,20,30,30};
	    int num = 0, total = 0;
	    num = rand()%100; //Éú³É·¶Î§[0, 99)
	    for(int c = 0; c < 7; c++)
	    {
		total += list[c];
		if(num < total)
		{
		    //cout << "the last id is>>" << c << " rate is >> " << list[c] << endl;
		    return c;
		}
	    }
	}
     
        void test()
        {
            int list[7] = {0};
            for(int c =0; c < 1000; c++)
            {
                ++list[test_1()];
            }
            for(int k =0; k < 7; ++k)
            {
                 cout << list[k] << endl;

            }
        }
}

namespace lastVec
{
void test()
{
    vector<int> vec;
    //vec.push_back(1);
    
    vector<int>::iterator p = vec.begin();

    for(p; p != vec.end(); ++p)
    {
        if((p + 1) == vec.end())
        {
            cout << "now get the end! data is " << *p <<endl;
        }

    }
}


}
int main(int argc, char **argv)
{
    srand(time(NULL));
//    randtest::test();
    lastVec::test();
}
