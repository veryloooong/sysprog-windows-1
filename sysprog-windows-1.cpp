// sysprog-windows-1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <Windows.h>
#include <Psapi.h>
#include <tchar.h>
#include <stdlib.h>

struct ProcessDetails {
  DWORD process_id;
  DWORD parent_process_id;
  char process_name[MAX_PATH];
  char parent_process_name[MAX_PATH];
};

void print_process_details(DWORD process_id) {  
  TCHAR process_name[MAX_PATH] = TEXT("<unknown>");  
   
  HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, process_id);  
  if (process != NULL) {  
   HMODULE module;  
   DWORD needed;  
   
   if (EnumProcessModulesEx(process, &module, sizeof(module), &needed, LIST_MODULES_ALL)) {  
     GetModuleBaseName(process, module, process_name, sizeof(process_name) / sizeof(TCHAR));  
   }  
  }  
   
  _tprintf(TEXT("%u\t0\t%ls\t%ls\n"), process_id, process_name, TEXT("<todo>"));  
  
  if (process != NULL)
    CloseHandle(process);  
}

int _cmp_function(const void* a, const void* b) {  
  int int_a = *(const int*)a;  
  int int_b = *(const int*)b;  
  return (int_a > int_b) - (int_a < int_b);  
}

int main() {
  DWORD processes[2048], needed, processes_count;

  if (!EnumProcesses(processes, sizeof(processes), &needed)) return 1;

  processes_count = needed / sizeof(DWORD);
  qsort(processes, processes_count, sizeof(DWORD), _cmp_function);

  _tprintf(TEXT("PID\tPPID\tName\tParentName\n"));

  for (unsigned int i = 0; i < processes_count; i++) {
    if (processes[i] != 0) {
      print_process_details(processes[i]);
    }
  }

  return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
