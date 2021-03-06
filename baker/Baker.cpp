#include <mutex>
#include <math.h>

#include "../includes/externs.h"
#include "../includes/baker.h"
#include "../includes/PRINT.h"
#include "../includes/logger.h"

using namespace std;

Baker::Baker(int id):id(id)
{
}

Baker::~Baker()
{
}

void Baker::bake_and_box(ORDER &anOrder) {
	Box newBox;
	for (int i = 0; i <= anOrder.number_donuts; i++) {
		if (newBox.size() >= 12 || i == anOrder.number_donuts) {
			anOrder.boxes.push_back(newBox);
			newBox.clear();
		}
		DONUT newDonut;
		newBox.addDonut(newDonut);
	}
	//PRINT2("Done with order number ", anOrder.order_number)
}

void Baker::beBaker() {
	while(!b_WaiterIsFinished || !order_in_Q.empty()) {

		// Bakers wait for notification from conditional
		{
			unique_lock<mutex> lck(mutex_order_inQ);
			while( !b_WaiterIsFinished  || (!b_WaiterIsFinished && order_in_Q.empty()) ) { // Waits for waiter to add items
				//PRINT4("\tPrint Status: ", !b_WaiterIsFinished, " or ",  ( !b_WaiterIsFinished && order_in_Q.empty() ) )
				//PRINT7("Baker ", id, " is waiting... <Queue Size is ", order_in_Q.size(), ", Waiter Done? -> ", b_WaiterIsFinished, ">")
				cv_order_inQ.wait(lck);
				//PRINT3("Baker ", id, " is back to work!")
			}
		}

		ORDER order;
		bool empty = true;

		{ // Lock for bakers to receive order from order in queue
		lock_guard<mutex> lck(mutex_order_inQ);
			if (!order_in_Q.empty()) {
				{
					empty = false;
					order = order_in_Q.front();
					order_in_Q.pop();
				}
			}
		}

		if (!empty) { // If current thread/baker was able to get an order (non empty queue)

			//PRINT4("Baker ", id, " is boxing order number ", order.order_number)
			bake_and_box(order);

			{ // Baker has baked and boxed the order and is now adding it to the out queue
				lock_guard<mutex> lck(mutex_order_outQ);
				order_out_Vector.push_back(order);
			}

		}
	}
}
