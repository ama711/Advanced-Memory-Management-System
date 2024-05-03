/*
    --------------------------------------------
    Running compiler and Hardware details:
    IDE: VScode x64-1.84.0
    Compiler: mingw-w64-x86_64-gcc GNU compiler collection 
              (C,C++,OpenMP) for MinGw-w64
    IDE extenstion: C/C++ Extension Pack
    Operating System: Windows 11 Home x64 (64 bits)
*/


// include libraries
#include <iostream>
#include <vector>
#include <string>
using namespace std;

// struct to represent a process as memory block
struct MemoryBlock
{
    int start_address;
    int end_address;
    string process_name;
};

// class memory manager to habdle memory operations
class MemoryManager
{

    // private members and methods of the class (method headers)
private:
    vector<MemoryBlock> memory; // memory vector to store processes
    int total_size;

    void mergeHoles();
    int findAvailableHole(const string &process_name, int size, char strategy);

    // public members and methods of the class
public:
    MemoryManager(int total_size);

    void requestMemory(const string &process_name, int size, char strategy);
    void releaseMemory(const string &process_name);
    void compactMemory();
    void reportMemoryStatus();
};

// methods of the class:

// Constructor of the class, take total_size variable as input
MemoryManager::MemoryManager(int total_size) : total_size(total_size)
{
    // first insert a block with max size provided by user
    // to initialize memory vector
    MemoryBlock block;
    block.start_address = 0;
    block.end_address = total_size - 1;
    block.process_name = "Unused";
    memory.push_back(block);
}

// Meothod to merge adjacent holes
void MemoryManager::mergeHoles()
{
    // if 2 adjacent holes exist in the memory, merge them into one holes
    // starting from the index of the first one, and erase the second that
    // was merged
    for (size_t i = 0; i < memory.size() - 1; ++i)
    {
        if (memory[i].process_name == "Unused" && memory[i + 1].process_name == "Unused")
        {
            memory[i].end_address = memory[i + 1].end_address;
            memory.erase(memory.begin() + i + 1);
            --i; // if a block is ereased, check from same index 
                 // next iteration as memory is updated 
        }
    }
}

// Meothod to allocate a hole to a process
int MemoryManager::findAvailableHole(const string &process_name, int size, char strategy)
{
    // initialize availableSize and index to allocate process
    int hole_index = -1;
    int available_size = 0;

    // check requested method of allocation and implement it

    // first fit algorithm
    if (strategy == 'F')
    {
        for (size_t i = 0; i < memory.size(); ++i)
        {
            if (memory[i].process_name == "Unused" 
                && memory[i].end_address - memory[i].start_address + 1 >= size)
            {
                hole_index = i;
                break;
            }
        }
    }

    // best fit algorithm
    else if (strategy == 'B')
    {
        int best_fit_index = -1;
        int best_fit_size = total_size + 1;

        for (size_t i = 0; i < memory.size(); ++i)
        {
            if (memory[i].process_name == "Unused" 
                && memory[i].end_address - memory[i].start_address + 1 >= size)
            {
                int hole_size = memory[i].end_address - memory[i].start_address + 1;
                if (hole_size < best_fit_size)
                {
                    best_fit_index = i;
                    best_fit_size = hole_size;
                }
            }
        }

        hole_index = best_fit_index;
    }

    // worst fit algorithm
    else if (strategy == 'W')
    {
        int worst_fit_index = -1;
        int worst_fit_size = -1;

        for (size_t i = 0; i < memory.size(); ++i)
        {
            if (memory[i].process_name == "Unused" 
                && memory[i].end_address - memory[i].start_address + 1 >= size)
            {
                int hole_size = memory[i].end_address - memory[i].start_address + 1;
                if (hole_size > worst_fit_size)
                {
                    worst_fit_index = i;
                    worst_fit_size = hole_size;
                }
            }
        }

        hole_index = worst_fit_index;
    }

    // check if an available hole was found, create a block to represent the process
    // and allocate it into the memory
    if (hole_index != -1)
    {
        available_size = memory[hole_index].end_address - memory[hole_index].start_address + 1;
        if (available_size > size)
        {
            // initialize memory block (process) details and insert into memory 
            MemoryBlock new_block;
            new_block.start_address = memory[hole_index].start_address + size;
            new_block.end_address = memory[hole_index].end_address;
            new_block.process_name = "Unused";
            memory[hole_index].end_address = memory[hole_index].start_address + size - 1;
            memory.insert(memory.begin() + hole_index + 1, new_block);
        }
        memory[hole_index].process_name = process_name;
    }

    return hole_index;
}

// Method to recive hole request for a process from main and call the findAvailableHole() 
void MemoryManager::requestMemory(const string &process_name, int size, char strategy)
{
    // take memory request from main and call the (findAvailableHole) method
    // then display the result whether not allocated or display the allocation details
    int hole_index = findAvailableHole(process_name, size, strategy);
    if (hole_index == -1)
    {
        cout << "Not enough memory available to allocate " << size 
             << " bytes for process " << process_name << "\n";
    }
    else
    {
        cout << "Allocated " << size << " bytes for process " << process_name 
             << " in the memory block from address "
             << memory[hole_index].start_address 
             << " to " << memory[hole_index].end_address << "\n";
    }
}

// Method to relese hole allocated to process by name
void MemoryManager::releaseMemory(const string &process_name)
{
    // if a processe exits and is chosen by the user to be released from memory
    // mark it as unusede and call the mergeHoles() algorithm to combine it with
    // adjacent holes if they exist
    for (size_t i = 0; i < memory.size(); ++i)
    {
        if (memory[i].process_name == process_name)
        {
            memory[i].process_name = "Unused";

            // Merge adjacent unused holes
            mergeHoles();

            cout << "Released memory allocated to process " << process_name << "\n";
            return;
        }
    }

    // message if no such process exist
    cout << "No memory allocated to process " << process_name << "\n";
}

// Method to compact memory
void MemoryManager::compactMemory()
{

    // this algorithm is used to compact memory into 1 hole and processes all are adjacent
    // first check if memory has 1 hole (begin, middle, or end)
    // if yes: it is already compacted and no need to continue, 
    // else:
    // reallocate all processe to the start of the memory contiguasily 
    // and the hole will be in the bottom of the memory vector

    // make sure that holes are correct
    mergeHoles();

    // count number of holes in the memory
    int holeCount;
    for (size_t i = 0; i < memory.size(); ++i)
    {
        if (memory[i].process_name == "Unused")
        {
            holeCount++;
            if (holeCount == 2)
            {
                break;
            }
        }
    }

    // if holes are 1 or no holes exist return
    if (holeCount < 2)
    {
        cout << "Memory is already compacted\n";
        return;
    }

    // declare vectors to store names and sizes of processes currently
    // present in memory to realLocate them later contigously
    vector<string> names;
    vector<int> sizes;

    // loop over current memory to extract current proccess information
    for (size_t i = 0; i < memory.size(); ++i)
    {
        if (memory[i].process_name != "Unused")
        {

            // store current process size
            int size = memory[i].end_address - memory[i].start_address + 1;
            sizes.push_back(size);

            // store current process name
            names.push_back(memory[i].process_name);
        }
    }

    // erase old memory content to free memory space
    for (size_t i = 0; i < memory.size(); ++i)
    {
        memory.erase(memory.begin() + i);
    }

    // creat new vector as a new empty memory
    vector<MemoryBlock> newMem;
    MemoryBlock block;
    block.start_address = 0;
    block.end_address = total_size - 1;
    block.process_name = "Unused";
    newMem.push_back(block);

    // assign the new memory to the current memory of the class
    memory = newMem;

    // reassign processes to correct locations using first fit so they become contigous
    // new location of each process will be printed during reallocation process
    for (size_t i = 0; i < names.size(); ++i)
    {
        MemoryManager::requestMemory(names[i], sizes[i], 'F');
    }
}

// method for printing current memory status 
void MemoryManager::reportMemoryStatus()
{
    // method to give status of memory, it reports allocated and non-allocated parts
    // accoding to each block name in the memory
    for (const auto &block : memory)
    {
        cout << "Addresses [" << block.start_address << ":" << block.end_address << "] ";
        if (block.process_name == "Unused")
        {
            cout << "Unused\n";
        }
        else
        {
            cout << "Process " << block.process_name << "\n";
        }
    }
}

// main method
int main()
{

    // read total memory size from user
    int total_size;
    cout << "enter memory size, should be larger than 0: ";
    cin >> total_size;

    // if memory size < 1 stop the program
    if (total_size < 1)
    {
        cout << "size is invalid, try the program next time" << endl;
        return 0;
    }
    
    // create memory manager object
    MemoryManager memoryManager(total_size);

    // read commands from user and execute them while the program is still running
    string command;
    while (true)
    {
        cout << "allocator> ";
        cin >> command;

        if (command == "RQ")
        {
            string process_name;
            int size;
            char strategy;
            cin >> process_name >> size >> strategy;
            
            // if process size is < 0 stop the program, invalid input
            // if memory size < 1 stop the program
            if (size < 1)
            {
                cout << "process size is invalid, try the program next time" << endl;
                return 0;
            }

            memoryManager.requestMemory(process_name, size, strategy);
        }
        else if (command == "RL")
        {
            string process_name;
            cin >> process_name;
            memoryManager.releaseMemory(process_name);
        }
        else if (command == "C")
        {
            memoryManager.compactMemory();
        }
        else if (command == "STAT")
        {
            memoryManager.reportMemoryStatus();
        }
        else if (command == "X")
        {
            break;
        }
        else
        {
            cout << "Invalid command\n";
        }
    }
    return 0;
}