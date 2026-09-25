#include <iostream>  
#include <vector> 

enum class Side { Buy, Sell } ; 

struct Order { 
    int id ; 
    Side side ; 
    int price ; 
    int quantity ; 
} ; 

int main() { 
    // test cases 
    std::vector<Order> orders { 
        Order{ 1, Side::Sell, 10050, 100 } , 
        Order{ 2, Side::Sell, 10060, 200 } ,
        Order{ 3, Side::Buy, 10000, 150 } , 
    } ; 

    for( const Order& order : orders ) { 

        std::cout << "#" << order.id << " " ; 
        if( order.side == Side::Buy ) { 
        std::cout << "BUY" ;
        } else { 
            std::cout << "SELL" ; 
        }
        
        std::cout << " " << order.quantity << " @ " << order.price << '\n' ; 
    }
}

