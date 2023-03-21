#include <iostream>
using namespace std;

bool check(int arr, int arr2, int size, int array[]){
    if(size < 2) return false;
    else{
        if(arr == arr2) return true;
    }
    return check(array[size-1], array[size-2], size-1, array);
}

bool adjArray(int* arr, int size){
    return check(arr[size], arr[size], size, arr);
}

int main(){
    int arr[] = {10,1,2,3,4,5,5,7,8,1};
    int* ptr = arr;
    int size = 10;

    bool ret = adjArray(ptr, size);
    if(ret = true) cout << "true" <<endl;
    else cout << "false" <<endl;

}