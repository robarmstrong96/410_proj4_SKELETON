#include <mutex>
#include <math.h>

#include "../includes/externs.h"
#include "../includes/baker.h"
#include "../includes/PRINT.h"

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
		//PRINT6("\tOrder #", anOrder.order_number, " Current Box Size = ", newBox.size(), " Ctr at ", i);
		//logger.log();
		//PRINT4("\tOrder #", anOrder.order_number, " Total Donut Order Size: ", anOrder.number_donuts);
		if (newBox.size() >= 12 || i == anOrder.number_donuts) {
			//PRINT4("\t\t Order #", anOrder.order_number, "Box Size is: ", newBox.size());
			anOrder.boxes.push_back(newBox);
			newBox.clear();
		}
		DONUT newDonut;
		newBox.addDonut(newDonut);
	}
}

void Baker::beBaker() {
	while(!b_WaiterIsFinished || !order_in_Q.empty()) {
		//PRINT4("Waiter not finished? ", !b_WaiterIsFinished, " Order Queue  Not Empty? ", !order_in_Q.empty())
		//PRINT2("First Conditional is ", (!b_WaiterIsFinished || !order_in_Q.empty() ) )
		{
			unique_lock<mutex> lck(mutex_order_inQ);
			//PRINT2("Second Conditional is ", ( order_in_Q.empty() || !b_WaiterIsFinished ) )
			//PRINT4("Baker ", id,": ","Waiting...")
			while(order_in_Q.empty() || !b_WaiterIsFinished) { // Waits for baker to add items
				cv_order_inQ.wait(lck);
			}
			//PRINT4("Baker ", id,": ","Done Waiting!")
		}

		ORDER order;
		//PRINT4("Baker ", id,": ","Getting order queue")

		{
			lock_guard<mutex> lck(mutex_order_inQ);
			order = order_in_Q.front();
			order_in_Q.pop();
			//PRINT6("Baker ", id,": ","Popped queue [", order.order_number, "]")
		}

		//PRINT6("Baker ", id,": ","Boxing order ", order.order_number, "]")
		bake_and_box(order);

		{
			//PRINT4("Baker ", id,": ","Adding Out Order...")
			lock_guard<mutex> lck(mutex_order_outQ);
			order_out_Vector.push_back(order);
			//PRINT4("Baker ", id,": ","Added Out Order...")
		}

	}
	//PRINT3("Baker ", id," is done")
}
