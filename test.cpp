#include <thread>
#include <vector>
#include <chrono>
#include <iostream>

using namespace std;

void oogaBoogaPrint(int i, int & returnVal);

vector<int> starvations = {};

int main () {
    vector<thread> threads = {};
    cout<<" we want 0 1 2 3 4 order"<<endl;
    
    for (int i = 0; i < 5; i++) {
        threads.push_back(thread(oogaBoogaPrint, i, i));
    }
    for( auto & x : threads) {
        x.join();
    }
    cout<<"now that threads are all done continue and end"<<endl;
    for (auto & x : threads) {
        x.
    }
    return 0;
}

void oogaBoogaPrint(int i, int & returnVal) {
    cout<<i<<" is sleeping for "<<10-i<<" s"<<endl;
    this_thread::sleep_for(chrono::seconds(10-i));
    cout<<i<<endl;
    starvations. = i+1;
}