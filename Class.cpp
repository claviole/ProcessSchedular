#include "Class.h"
/*
Christian Laviolette
CS30200
2/13/2023


THIS PROGRAM IS DESIGNED TO SIMULATE A PROCESS SCHEDULER
*/

using namespace std;


// counter for number of REALTIME processes that have been completed
int completedRealTime = 0;

//counter for number of INTERACTIVE process that have been completed
int completedInteractive = 0;

//counter for number of disk accesses
int diskAccesses = 0;

///total time for disk access (includiing waiting time in the disk queue)
double diskAccessTime = 0;

//time elapsed since the start of the first process
int elapsedTime = 0;

int ttyRequest = 0;






double cpuUtilizationTime = 0;

double missedDeadline = 0;






//Function to read input file that has a process type
vector < pair < string, int >>
readInput(string fileName)
{
	vector < pair < string, int >>data;
	string processClass;
	int processArrivalTime;

	ifstream file("input.txt");
	if (!file)
	{
		cout << "Unable to open file " << fileName << endl;
		return data;
	}

	while (file >> processClass >> processArrivalTime)
	{
		data.emplace_back(processClass, processArrivalTime);
	}

	file.close();
	return data;
}



// create a class to represent a "process" 
class Process {
public:
	string processClass;
	int sequenceNumber;
	int processArrivalTime;
	int timeAlive;
	int deadline;
	string currentStatus;
	vector < pair < string, int >>resourceRequest;

	Process(string processClass, int deadline, int sequenceNumber, int processArrivalTime, string currentStatus, vector < pair < string, int >>resourceRequest) {
		this->processClass = processClass;
		this->sequenceNumber = sequenceNumber;
		this->processArrivalTime = processArrivalTime;
		this->currentStatus = currentStatus;
		this->resourceRequest = resourceRequest;
		this->deadline = deadline;
	}

	Process() {
		this->processClass = "";
		this->sequenceNumber = 0;
		this->processArrivalTime = 0;
		this->currentStatus = "";
		this->resourceRequest = {};
		this->deadline = 0;
	}
};

// a function to add a resource request to a process
void addResourceRequest(Process& p, string resourceType, int resourceAmount) {
	p.resourceRequest.emplace_back(resourceType, resourceAmount);
}

void addProcessToQueue(queue<Process>& processQueue, Process p) {
	processQueue.push(p);
}



// a function to remove a process from a queue 
void removeProcessFromQueue(queue<Process>& processQueue) {
	processQueue.pop();
}


//function to move the first item of a queue to the back of the queue
void moveProcessToBack(queue<Process>& processQueue) {
	Process p = processQueue.front();
	processQueue.pop();
	processQueue.push(p);
}
//function to remove the first resource request from a process
void removeResourceRequest(Process& p) {
	p.resourceRequest.erase(p.resourceRequest.begin());
}



//create processSchedular
void processScheduler(queue<Process>& processQueue)
{


	// create a queue for ready interactive processes
	queue<Process> interactiveQueue;
	// create a queue for ready real time processes
	queue<Process> realTimeQueue;

	while (!processQueue.empty())
	{
		for (int i = 0; i < processQueue.size(); i++)
		{

			if (processQueue.front().processClass == "REAL-TIME")
			{
				processQueue.front().currentStatus = "WAITING";

				addProcessToQueue(realTimeQueue, processQueue.front());

				removeProcessFromQueue(processQueue);
			}
			else if (processQueue.front().processClass == "INTERACTIVE")
			{
				processQueue.front().currentStatus = "WAITING";

				addProcessToQueue(interactiveQueue, processQueue.front());

				removeProcessFromQueue(processQueue);
			}
			else
			{
				removeProcessFromQueue(processQueue);
			}


		}





	}


	//simulate process execution


	//complete realtime process as they have top priority
	while (!realTimeQueue.empty())
	{
		for (int i = 0; i < realTimeQueue.size(); i++)
		{
			if (elapsedTime < realTimeQueue.front().processArrivalTime)
			{
				elapsedTime++;
			}
			else
			{
				realTimeQueue.front().currentStatus = "RUNNING";
				cout << realTimeQueue.front().sequenceNumber << " " << realTimeQueue.front().processClass << " " << realTimeQueue.front().currentStatus << endl;
				for (int j = 0; j < realTimeQueue.front().resourceRequest.size();)
				{
					if (realTimeQueue.front().resourceRequest[j].first == "CPU")
					{
						cpuUtilizationTime += realTimeQueue.front().resourceRequest[j].second;
						elapsedTime += realTimeQueue.front().resourceRequest[j].second;
						realTimeQueue.front().timeAlive += realTimeQueue.front().resourceRequest[j].second;
						removeResourceRequest(realTimeQueue.front());
						if (realTimeQueue.front().timeAlive >= realTimeQueue.front().deadline)
						{
							missedDeadline++;
							realTimeQueue.front().currentStatus = "TERMINATED";
							cout << realTimeQueue.front().sequenceNumber << " " << realTimeQueue.front().currentStatus << endl;
							removeProcessFromQueue(realTimeQueue);
							break;
						}
						else
						{
							realTimeQueue.front().currentStatus = "WAITING";
							cout << realTimeQueue.front().sequenceNumber << " " << realTimeQueue.front().processClass << " " << realTimeQueue.front().currentStatus << endl;
							moveProcessToBack(realTimeQueue);
							break;
						}
					}
					else if (realTimeQueue.front().resourceRequest[j].first == "DISK")
					{
						diskAccesses++;
						diskAccessTime += realTimeQueue.front().resourceRequest[j].second;
						elapsedTime += realTimeQueue.front().resourceRequest[j].second;
						realTimeQueue.front().timeAlive += realTimeQueue.front().resourceRequest[j].second;
						removeResourceRequest(realTimeQueue.front());
						if (realTimeQueue.front().timeAlive >= realTimeQueue.front().deadline)
						{
							missedDeadline++;
							realTimeQueue.front().currentStatus = "TERMINATED";
							cout << realTimeQueue.front().sequenceNumber << " " << realTimeQueue.front().processClass << " " << realTimeQueue.front().currentStatus << endl;
							removeProcessFromQueue(realTimeQueue);
							break;
						}
						else
						{
							realTimeQueue.front().currentStatus = "WAITING";
							cout << realTimeQueue.front().sequenceNumber << " " << realTimeQueue.front().processClass << " " << realTimeQueue.front().currentStatus << endl;
							break;
						}
					}
					else if (realTimeQueue.front().resourceRequest[j].first == "TTY")
					{
						ttyRequest++;
						elapsedTime += realTimeQueue.front().resourceRequest[j].second;
						removeResourceRequest(realTimeQueue.front());
						realTimeQueue.front().timeAlive += realTimeQueue.front().resourceRequest[j].second;
					}
				}
				completedRealTime++;
				removeProcessFromQueue(realTimeQueue);
			}

		}

	}
	//Complete interactive processes
	while (!interactiveQueue.empty())
	{
		for (int i = 0; i < interactiveQueue.size(); i++)
		{
			if (elapsedTime < interactiveQueue.front().processArrivalTime)
			{
				elapsedTime++;
			}
			else
			{
				interactiveQueue.front().currentStatus = "RUNNING";
				cout << interactiveQueue.front().sequenceNumber << " " << interactiveQueue.front().processClass << " " << interactiveQueue.front().currentStatus << endl;
				for (int j = 0; j < interactiveQueue.front().resourceRequest.size(); j++)
				{
					if (interactiveQueue.front().resourceRequest[j].first == "CPU")
					{
						cpuUtilizationTime += interactiveQueue.front().resourceRequest[j].second;
						elapsedTime += interactiveQueue.front().resourceRequest[j].second;
						interactiveQueue.front().timeAlive += interactiveQueue.front().resourceRequest[j].second;
						removeResourceRequest(interactiveQueue.front());

					}
					else if (interactiveQueue.front().resourceRequest[j].first == "DISK")
					{
						diskAccesses++;
						diskAccessTime += interactiveQueue.front().resourceRequest[j].second;
						elapsedTime += interactiveQueue.front().resourceRequest[j].second;
						interactiveQueue.front().timeAlive += interactiveQueue.front().resourceRequest[j].second;
						removeResourceRequest(interactiveQueue.front());

					}
					else if (interactiveQueue.front().resourceRequest[j].first == "TTY")
					{
						ttyRequest++;
						elapsedTime += interactiveQueue.front().resourceRequest[j].second;
						removeResourceRequest(interactiveQueue.front());
						interactiveQueue.front().timeAlive += interactiveQueue.front().resourceRequest[j].second;
					}
				}
				completedInteractive++;
				removeProcessFromQueue(interactiveQueue);
			}
		}
	}

















	// print the statistics
	cout << "CPU utilization time: " << cpuUtilizationTime / elapsedTime << endl;
	cout << "Disk accesses: " << diskAccesses << endl;
	cout << "Disk access time: " << diskAccessTime << endl;
	cout << "Completed real-time processes: " << completedRealTime << endl;
	cout << "Disk Utilization time: " << diskAccessTime / elapsedTime << endl;
	cout << "Completed interactive processes: " << completedInteractive << endl;
	cout << "Missed deadlines: " << missedDeadline << endl;
	cout << "Percent of Processes that missed their deadline: " << missedDeadline / (completedRealTime + missedDeadline) << "%" << endl;
	cout << "Average disk time : " << diskAccessTime / diskAccesses << endl;
	cout << "Total time elapsed since start of first process : " << elapsedTime << endl;

}

int main() {

	// read the input file a
	vector < pair < string, int >>data = readInput("input.txt");

	// construct a waiting Queue                  
	queue<Process> processQueue;
	Process currentprocess;
	int sequence_counter = 0;
	for (int i = 1; i < data.size(); i++)
	{
		//add process into queue and start new current process
		if ((data[i].first == "INTERACTIVE" || data[i].first == "REAL-TIME") && i != 0)
		{
			addProcessToQueue(processQueue, currentprocess);
			currentprocess = Process();
			currentprocess.processClass = data[i].first;
			currentprocess.processArrivalTime = data[i].second;
			currentprocess.sequenceNumber = sequence_counter;
			sequence_counter++;
			currentprocess.currentStatus = "WAITING";

		}
		//add deadline to process
		else if ((data[i].first == "DEADLINE"))
		{
			currentprocess.deadline = data[i].second;
		}
		//add resource request
		else {
			addResourceRequest(currentprocess, data[i].first, data[i].second);
		}


	}
	addProcessToQueue(processQueue, currentprocess);
	processScheduler(processQueue);




}