#include <string>
#include "stdlib.h"

#include "../includes/externs.h"
#include "../includes/waiter.h"
#include "../includes/PRINT.h"

using namespace std;

Waiter::Waiter(int id,std::string filename):id(id),myIO(filename){
}

Waiter::~Waiter()
{
}

//gets next Order(s) from file_IO
int Waiter::getNext(ORDER &anOrder){
	if (myIO.getNext(anOrder) == SUCCESS) {
		return SUCCESS;
	}
	return FAIL;
}

/*
	Function to simulate main waiter functionality (receive orders, notify bakers,
	etc... )
*/
void Waiter::beWaiter() {
	ORDER order; // Declare Order object
	int orderStatus = getNext(order); // Set order and get status of order.

	while(orderStatus == SUCCESS) {
		//PRINT5("Waiter ",id,": ","Order Status -> ", orderStatus)
		unique_lock<mutex> lck(mutex_order_inQ);
		order_in_Q.push(order); // 1) add order to queue
		//PRINT5("Waiter ",id,": ","Added Order ", order.order_number)
		orderStatus = getNext(order); // 3) get the next order
		cv_order_inQ.notify_all(); // 2) notify all listening/sleeping threads
	}
	unique_lock<mutex> lck(mutex_order_inQ);
	b_WaiterIsFinished = true; // No more orders, waiter is done. Set extern global to true.
	cv_order_inQ.notify_all(); // 2) notify all listening/sleeping threads
	//PRINT4("Waiter ",id,": ","Finished!")
}
