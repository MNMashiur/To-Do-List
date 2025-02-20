#include <iostream>
#include <queue>
#include <cstring>
#include <algorithm>
#include <fstream>

#define MAX_TASKS 50

using namespace std;

struct Task
{
    int id;
    char name[50];
    int priority;
    int duration;
    bool completed;
};

Task tasks[MAX_TASKS];
int taskCount = 0;
const char* FILENAME = "Tasks.txt";

// Function to find a task by ID
int findTaskById(int id)
{
    for (int i = 0; i < taskCount; i++)
    {
        if (tasks[i].id == id) return i;
    }
    return -1;
}

void saveTasksToFile()
{
    ofstream file(FILENAME, ios::trunc);
    if (!file)
    {
        cerr << "Error opening file for writing.\n";
        return;
    }

    for (int i = 0; i < taskCount; i++)
    {
        file << "Task ID : "<<tasks[i].id << "\t" <<"Task Name : "<< tasks[i].name << "\t"<<"Task Priority : "<< tasks[i].priority << "\t" <<"Task Duration : "<< tasks[i].duration << "\n";
    }

    file.close();
}

// Function to load tasks from file
void loadTasksFromFile()
{
    ifstream file(FILENAME);
    if (!file)
    {
        cerr << "No task file found. Starting fresh.\n";
        return;
    }

    taskCount = 0;
    while (file)
    {
        Task task;
        char completed;
        file >> task.id;
        file.ignore();
        file.getline(task.name, 50, '|');
        file >> task.priority;
        file.ignore();
        file >> task.duration;
        file.ignore();
        file >> completed;
        file.ignore();

        if (file)
        {
            task.completed = (completed == '1');
            tasks[taskCount++] = task;
        }
    }

    file.close();
}

// ADD TASK: Add a new task
void addTask(int id, const char* name, int priority, int duration)
{
    tasks[taskCount].id = id;
    strcpy(tasks[taskCount].name, name);
    tasks[taskCount].priority = priority;
    tasks[taskCount].duration = duration;
    tasks[taskCount].completed = false;

    taskCount++;
    saveTasksToFile();
    cout << "Task added successfully!\n";
}

//Modify an existing task
void updateTask(int id)
{
    int index = findTaskById(id);
    if (index == -1)
    {
        cout << "Task not found.\n";
        return;
    }

    cout << "Updating Task ID: " << id << "\n";
    cout << "Enter new Task Name (current: " << tasks[index].name << "): ";
    cin.ignore();
    cin.getline(tasks[index].name, 50);

    cout << "Enter new Priority (current: " << tasks[index].priority << "): ";
    cin >> tasks[index].priority;

    cout << "Enter new Duration (current: " << tasks[index].duration << "): ";
    cin >> tasks[index].duration;
    saveTasksToFile();
    cout << "Task updated successfully!\n";
}

// DELETE a task
void deleteTask(int id)
{
    int index = findTaskById(id);
    if (index == -1)
    {
        cout << "Task not found.\n";
        return;
    }

    for (int i = index; i < taskCount - 1; i++)
    {
        tasks[i] = tasks[i + 1];
    }
    taskCount--;
    saveTasksToFile();
    cout << "Task deleted successfully!\n";
}

// DISPLAY all tasks
void displayTasks()
{
    if (taskCount == 0)
    {
        cout << "No tasks available.\n";
        return;
    }

    cout << "Current Tasks:\n";
    for (int i = 0; i < taskCount; i++)
    {
        cout << "ID: " << tasks[i].id << ", Name: " << tasks[i].name
             << ", Priority: " << tasks[i].priority
             << ", Duration: " << tasks[i].duration << "\n";
    }
}

// TOPOLOGICAL SORT: Find task execution order
void topologicalSort()
{
    vector<Task> sortedTasks(tasks, tasks + taskCount);

    sort(sortedTasks.begin(), sortedTasks.end(), [](Task &a, Task &b)
    {
        if (a.priority == b.priority)
            return a.duration < b.duration;
        return a.priority > b.priority;
    });

    cout << "Task Order (based on Priority and Duration):\n";
    for (Task &task : sortedTasks)
    {
        cout << task.name << " (ID: " << task.id << "), Priority: " << task.priority
             << ", Duration: " << task.duration << "\n";
    }
}

// COIN CHANGE GREEDY: Allocate time to tasks based on priority
void timeAllocationGreedy(int totalTime)
{
    Task temp[MAX_TASKS];
    memcpy(temp, tasks, sizeof(tasks));

    sort(temp, temp + taskCount, [](Task &a, Task &b)
    {
        return a.priority > b.priority;
    });

    int currentTime = 0;
    cout << "Selected Tasks for Time Allocation:\n";

    for (int i = 0; i < taskCount; i++)
    {
        if (currentTime + temp[i].duration <= totalTime)
        {
            currentTime += temp[i].duration;
            temp[i].completed = true;
            cout << "Task: " << temp[i].name << ", Priority: " << temp[i].priority << ", Duration: " << temp[i].duration << "\n";
        }
    }

    cout << "Total Time Allocated: " << currentTime << "/" << totalTime << "\n";
}

// EFFICIENT TASK MATCHING USING KMP
void computePrefixArray(const char* pattern, int prefix[], int patternLength)
{
    int j = 0;
    prefix[0] = 0;

    for (int i = 1; i < patternLength; i++)
    {
        if (pattern[i] == pattern[j])
        {
            j++;
            prefix[i] = j;
        }
        else
        {
            if (j != 0)
            {
                j = prefix[j - 1];
                i--;
            }
            else
            {
                prefix[i] = 0;
            }
        }
    }
}
//SEARCHING by name
void searchTasksByName(const char* keyword)
{
    int keywordLength = strlen(keyword);
    int prefix[keywordLength];

    computePrefixArray(keyword, prefix, keywordLength);

    cout << "Matching Tasks:\n";
    bool found = false;

    for (int i = 0; i < taskCount; i++)
    {
        const char* taskName = tasks[i].name;
        int taskNameLength = strlen(taskName);
        int j = 0;
        for (int k = 0; k < taskNameLength; k++)
        {
            if (keyword[j] == taskName[k])
            {
                j++;
                if (j == keywordLength)
                {
                    cout << "ID: " << tasks[i].id << ", Name: " << tasks[i].name
                         << ", Priority: " << tasks[i].priority
                         << ", Duration: " << tasks[i].duration << "\n";
                    found = true;
                    break;
                }
            }
            else
            {
                if (j != 0)
                {
                    j = prefix[j - 1];
                    k--;
                }
            }
        }
    }

    if (!found)
    {
        cout << "No matching tasks found.\n";
    }
}

// MAIN FUNCTION
int main()
{
    loadTasksFromFile();
    int choice;

    do
    {
        cout << "\n========== To-Do List Menu ==========" << endl;
        cout << "1. Add Task\n";
        cout << "2. Update Task\n";
        cout << "3. Delete Task\n";
        cout << "4. Display Tasks\n";
        cout << "5. Find Task Order (Topological Sort)\n";
        cout << "6. Search Tasks by Name\n";
        cout << "7. Time Allocation (Greedy)\n";
        cout << "8. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
        {
            int id, priority, duration;
            char name[50];

            cout << "Enter Task ID: ";
            cin >> id;
            cin.ignore();
            cout << "Enter Task Name: ";
            cin.getline(name, 50);
            cout << "Enter Task Priority: ";
            cin >> priority;
            cout << "Enter Task Duration: ";
            cin >> duration;

            addTask(id, name, priority, duration);
            break;
        }
        case 2:
        {
            int id;
            cout << "Enter Task ID to Update: ";
            cin >> id;
            updateTask(id);
            break;
        }
        case 3:
        {
            int id;
            cout << "Enter Task ID to Delete: ";
            cin >> id;
            deleteTask(id);
            break;
        }
        case 4:
            displayTasks();
            break;
        case 5:
            topologicalSort();
            break;
        case 6:
        {
            char keyword[50];
            cout << "Enter keyword to search: ";
            cin.ignore();
            cin.getline(keyword, 50);
            searchTasksByName(keyword);
            break;
        }
        case 7:
        {
            int totalTime;
            cout << "Enter Available Time: ";
            cin >> totalTime;
            timeAllocationGreedy(totalTime);
            break;
        }
        case 8:
            cout << "Exiting... Thank you!\n";
            break;
        default:
            cout << "Invalid choice! Please try again.\n";
        }
    }
    while (choice != 8);

    return 0;
}
