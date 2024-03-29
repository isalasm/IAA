#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;




/*-------------------------------Data Structures-------------------------------*/

struct problem_data {
    int J; //Number of Jobs
    int M; //Number of Machines
    std::vector<vector<int>> OM; //Operations Matrix OM[row][column]
    std::vector<vector<int>> CM; //Cost Matrix CM[row][column]
};


/*----------------------------Function Declaration----------------------------*/
problem_data getData(const char* file);
int job_compare(std::vector<int> v,  std::vector<vector<int>> time);
int job_compare_SPT(std::vector<int> v, std::vector<vector<int>> time);
int job_compare_LPT(std::vector<int> v, std::vector<vector<int>> time);
int job_compare_LRPT(std::vector<int> v, std::vector<vector<int>> time);
int job_compare_SRPT(std::vector<int> v, std::vector<vector<int>> time);
void greedy(problem_data data, const char* problem_name);
void printMatrix(std::vector<vector<int>> &M);
void printVector(vector<int> &M);
int elementNotInMatrix(std::vector<int> &M, int element);
void writeToFile(const char* problem_name, int time, std::vector<vector<int>> result);




/*------------------------------------Main------------------------------------*/

int main(int argc, char const *argv[])
{
  ofstream file;
  file.open("results.txt");
  file.close();

  if (argc>1)
  {
    string file_name = argv[1];
    cout << "nombre:  " << file_name << endl;
    char *file_name_c = &file_name[0u];
    problem_data data = getData(file_name_c);
    cout << "Jobs: " << data.J << endl;
    cout << "Machines: " << data.M << endl;
    printMatrix(data.OM);
    printMatrix(data.CM);
    //printMatrix(data.J, data.M, data.OM);
    //printMatrix(data.J, data.M, data.CM);
    char *instance_c = &file_name[0u];
    greedy(data, instance_c);
  }
  else{
    std::vector<string> instance = {"orb01", "orb02", "orb03", "orb04", "abz5", "abz6", "abz7", "abz8", "swv01", "swv02", "swv03", "swv11", "swv12", "swv13"};
    for (size_t i = 0; i < instance.size(); i++) {
      string file_name = "instancias/" + instance[i] + ".txt";
      cout << "nombre:  " << file_name << endl;
      char *file_name_c = &file_name[0u];
      problem_data data = getData(file_name_c);
    	cout << "Jobs: " << data.J << endl;
    	cout << "Machines: " << data.M << endl;
      printMatrix(data.OM);
      printMatrix(data.CM);
      //printMatrix(data.J, data.M, data.OM);
      //printMatrix(data.J, data.M, data.CM);
      char *instance_c = &instance[i][0u];
      greedy(data, instance_c);
    }
  }

	return 0;
}




/*---------------------------------Functions---------------------------------*/


problem_data getData(const char* file){
	ifstream inFile;
	problem_data data;

	//std::vector<int> PS(3); // Problem size: [N, D, S]
    inFile.open(file);
    if (!inFile) {
        cout << "No es posible abrir el archivo";
        exit(1); // terminate with error
    }
    char line[100];
    inFile.getline(line, 100);
    std::stringstream stream(line);
    //cout << line << endl;
    int J;
    int M;
    for (int i = 0; i < 2; ++i)
    {
	   int n;
	   stream >> n;
	   if(!stream)
	      break;
	   //cout << "Found integer: " << n << "\n";
	  if (i==0)
	  {
	  	J = n;
	  }
	  else{
	  	M = n;
	  }
    }
    //inFile.getline(line, 100);
    cout << J << endl;
    cout << M << endl;
    std::vector<vector<int>> OM; //Operation Matrix: OM[row][column]
    std::vector<vector<int>> CM; //Cost Matrix: CM[row][column]
    for (int i = 0; i < J; ++i)
    {
	    inFile.getline(line, 100);
	    //cout << line << endl;
	    std::stringstream stream2(line);
	    std::vector<int> OM_row;
	    std::vector<int> CM_row;
	    int j=0;
	    while (1)
	    {
		   int n;
		   stream2 >> n;
		   if(!stream2)
		      break;
		   //cout << "Found integer: " << n << "\n";
		  if (j%2==0)
		  {
		  	OM_row.push_back(n);
		  }
		  else{
		  	CM_row.push_back(n);
		  }
		   j++;
	    }
      OM.push_back(OM_row);
      CM.push_back(CM_row);
	  }
    inFile.close();
    cout << "OK " << "\n";
    data.J = J;
    data.M = M;
    cout << "OK2" << "\n";
    data.OM = OM;
    data.CM = CM;
    return data;
}


void printMatrix(std::vector<vector<int>> &M){
  for(std::vector<vector<int>>::size_type x=0;x<M.size();x++)  // loop 3 times for three lines
    {
      for(std::vector<int>::size_type y=0;y<M[x].size();y++)  // loop for the three elements on the line
      {
        cout<<M[x][y]<<" | " << flush;  // display the current element out of the array
      }
      cout<<endl;  // when the inner loop is done, go to a new line
    }
}

void printVector(std::vector<int> &M){
    for(std::vector<int>::size_type y=0;y<M.size();y++)  // loop for the three elements on the line
    {
      cout<<M[y]<<" | " << endl;  // display the current element out of the array
    }
}

int elementNotInMatrix(std::vector<int> &M, int element){
      for(std::vector<int>::size_type y=0;y<M.size();y++)  // loop for the three elements on the line
      {
        if (M[y] == element) {
          return 0;
        }
      }
    return 1;
}


void greedy(problem_data data, const char* problem_name){
  std::vector<vector<int>> r_jobs = data.OM; // remaining jobs
  std::vector<vector<int>> r_times = data.CM; // remaining times
  std::vector<int> machines_o; //machines operation
  std::vector<int> t_machines; //machines remaining time

  std::vector<vector<int>> finalOrder(data.M); //machines queues

  int total_time = 0;
  for (int i = 0; i < data.M; i++) {
    t_machines.push_back(0);
    machines_o.push_back(-1);
  }
  while (1) {
    std::vector<vector<int>> q_machines(data.M); //machines queues
    for (int i = 0; i < data.M; i++) {
      q_machines[i].push_back(-1);
    }
    for (int i = 0; i < data.J; i++) {
      if (r_jobs[i].size() >= 1) {
        if (elementNotInMatrix(machines_o, i)) {
          int m = r_jobs[i][0];
          if (q_machines[m][0]==-1) {
            q_machines[m][0] = i;
          }
          else{
            q_machines[m].push_back(i);
          }
        }
      }

    }
    cout << "Jobs remaining" << endl;
    printMatrix(r_jobs);
    cout << "Machines queues" << endl;
    printMatrix(q_machines);
    for (int i = 0; i < data.M; i++) {
      if (machines_o[i] == -1) {
        if (q_machines[i].size() > 1) {
          int selected_job = job_compare_SPT(q_machines[i], r_times);
          //int selected_job = job_compare_LPT(q_machines[i], r_times);
          //int selected_job = job_compare_SRPT(q_machines[i], r_times);
          //int selected_job = job_compare_LRPT(q_machines[i], r_times);
          //q_machines[i].erase(q_machines[i].begin(), q_machines[i].begin() + q_machines[i].size());
          machines_o[i] = selected_job;
          t_machines[i] = r_times[selected_job][0];
          //r_jobs[selected_job].erase(r_jobs[selected_job].begin());
          //r_times[selected_job].erase(r_times[selected_job].begin());
        }
        else{
          if (q_machines[i][0] != -1) {
            machines_o[i] = q_machines[i][0];
            t_machines[i] = r_times[q_machines[i][0]][0];
          }
        }
      }
    }
    int end = true;
    for (int i = 0; i < data.M; i++) {
      if (machines_o[i] !=-1) {
        end = false;
      }
    }
    if (end) {
      cout << "FinaleOrder" << endl;
      printMatrix(finalOrder);
      cout << "Total time: " << total_time << endl;
      writeToFile(problem_name, total_time, finalOrder);
      break;
    }
    ////cout << "Machines queues" << endl;
    ////printMatrix(q_machines);
    //cout << "Machines time" << endl;
    //printVector(t_machines);
    int lt = 1000000000; //lowest time
    for (int i = 0; i < data.M; i++) {
      if (t_machines[i] != 0 && t_machines[i] < lt) {
        lt = t_machines[i];
      }
    }
    total_time = total_time + lt;
    for (int i = 0; i < data.M; i++) {
      if (t_machines[i] != 0 ) {
        t_machines[i] = t_machines[i] - lt;
      }
    }
    //cout << "Machines time" << endl;
    //printVector(t_machines);
    for (int i = 0; i < data.M; i++) {
      if (t_machines[i] == 0) {
        if (machines_o[i] !=-1) {
          r_jobs[machines_o[i]].erase(r_jobs[machines_o[i]].begin());
          r_times[machines_o[i]].erase(r_times[machines_o[i]].begin());
          finalOrder[i].push_back(machines_o[i]);
          machines_o[i] = -1;
        }
      }
    }

    //cout << "FinaleOrder" << endl;
    //printMatrix(finalOrder);
  }
}

int job_compare(std::vector<int> v,  std::vector<vector<int>> time){
  int num = v[0];
  return num;
}


int job_compare_SPT(std::vector<int> v, std::vector<vector<int>> time){
  std::vector<int> job_time;
  for (std::vector<int>::size_type i = 0; i < v.size(); i++) {
    int t = time[v[i]][0];
    job_time.push_back(t);
  }
  int max_time = 1000000;
  int max_pos = -1;
  for (std::vector<int>::size_type i = 0; i < job_time.size(); i++) {
    if (job_time[i] < max_time) {
      max_time = job_time[i];
      max_pos = i;
    }
  }
  return v[max_pos];
}


int job_compare_LPT(std::vector<int> v, std::vector<vector<int>> time){
  std::vector<int> job_time;
  for (std::vector<int>::size_type i = 0; i < v.size(); i++) {
    int t = time[v[i]][0];
    job_time.push_back(t);
  }
  int max_time = -1;
  int max_pos = -1;
  for (std::vector<int>::size_type i = 0; i < job_time.size(); i++) {
    if (job_time[i] > max_time) {
      max_time = job_time[i];
      max_pos = i;
    }
  }
  return v[max_pos];
}


int job_compare_LRPT(std::vector<int> v, std::vector<vector<int>> time){
  std::vector<int> job_time;
  for (std::vector<int>::size_type i = 0; i < v.size(); i++) {
    int t = 0;
    for (std::vector<int>::size_type j = 0; j < time[v[i]].size(); j++) {
      t = t + time[v[i]][j];
    }
    job_time.push_back(t);
  }
  int max_time = -1;
  int max_pos = -1;
  for (std::vector<int>::size_type i = 0; i < job_time.size(); i++) {
    if (job_time[i] > max_time) {
      max_time = job_time[i];
      max_pos = i;
    }
  }
  return v[max_pos];
}


int job_compare_SRPT(std::vector<int> v, std::vector<vector<int>> time){
  std::vector<int> job_time;
  for (std::vector<int>::size_type i = 0; i < v.size(); i++) {
    int t = 0;
    for (std::vector<int>::size_type j = 0; j < time[v[i]].size(); j++) {
      t = t + time[v[i]][j];
    }
    job_time.push_back(t);
  }
  int max_time = 1000000;
  int max_pos = -1;
  for (std::vector<int>::size_type i = 0; i < job_time.size(); i++) {
    if (job_time[i] < max_time) {
      max_time = job_time[i];
      max_pos = i;
    }
  }
  return v[max_pos];
}

void writeToFile(const char* problem_name, int time, std::vector<vector<int>> result){
  ofstream file;
  file.open("results.txt", std::ios_base::app);
  file << problem_name;
  file << "\n  \n";
  file << time;
  file << "\n  \n";
  for(std::vector<vector<int>>::size_type x=0;x<result.size();x++)  // loop 3 times for three lines
    {
      for(std::vector<int>::size_type y=0;y<result[x].size();y++)  // loop for the three elements on the line
      {
        file << result[x][y];
        file << " ";
      }
      file << "\n";
    }
  file << "----------------------------------------------------------------\n";
  file.close();
}
