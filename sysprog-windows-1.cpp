#include <Windows.h>

#include <TlHelp32.h>
#include <cstdio>
#include <cstdlib>
#include <tchar.h>
#include <unordered_map>

void print_err(TCHAR const *where) {
  DWORD code = GetLastError();
  ::_tprintf(TEXT("%s failed with error code %lu\n"), where, code);
}

typedef struct _ProcessDetail {
  DWORD pid;
  DWORD ppid;
  TCHAR pname[MAX_PATH];
  TCHAR ppname[MAX_PATH];
} ProcessDetail;

int _cmp_function(const void *a, const void *b) { 
  DWORD pid_a = ((ProcessDetail*)a)->pid;
  DWORD pid_b = ((ProcessDetail*)b)->pid;
  return (pid_a > pid_b) - (pid_b > pid_a);
}

BOOL GetProcessesList(ProcessDetail *processes, DWORD max_count,
                      DWORD *processes_count) {
  HANDLE snapshot;
  PROCESSENTRY32 pe32;

  snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (snapshot == INVALID_HANDLE_VALUE) {
    print_err(TEXT("CreateToolhelp32Snapshot"));
    return FALSE;
  }

  pe32.dwSize = sizeof(PROCESSENTRY32);

  if (!Process32First(snapshot, &pe32)) {
    print_err(TEXT("Process32First"));
    CloseHandle(snapshot);
    return FALSE;
  }

  DWORD i = 0;
  do {
    processes[i].pid = pe32.th32ProcessID;
    processes[i].ppid = pe32.th32ParentProcessID;
    CopyMemory(processes[i].pname, pe32.szExeFile, MAX_PATH * sizeof(TCHAR));

    if (i++ == max_count)
      break;
  } while (Process32Next(snapshot, &pe32));

  *processes_count = i;
  CloseHandle(snapshot);

  return TRUE;
}

void SortProcessesList(ProcessDetail* processes, DWORD processes_count) {
  ::qsort(processes, processes_count, sizeof(ProcessDetail), _cmp_function);

  // assign names to ppid
  DWORD i;
  std::unordered_map<DWORD, TCHAR *> names;

  TCHAR const* not_found = "[not found]";
  for (i = 0; i < processes_count; i++) {
    names[processes[i].pid] = processes[i].pname;
  }
  for (i = 0; i < processes_count; i++) {
    if (names.contains(processes[i].ppid)) {
      CopyMemory(processes[i].ppname, names[processes[i].ppid], MAX_PATH * sizeof(TCHAR));
    } else {
      CopyMemory(processes[i].ppname, not_found, MAX_PATH * sizeof(TCHAR));
    }
  }
}

void PrintProcessesList(ProcessDetail *processes, DWORD processes_count) {
  for (DWORD i = 0; i < processes_count; i++) {
    ::_tprintf(TEXT("%lu\t%lu\t%s\t\t%s\n"), processes[i].pid,
               processes[i].ppid, processes[i].pname, processes[i].ppname);
  }
}

int main() {
  ::_tprintf(TEXT("PID\tPPID\tName\t\tParentName\n"));
  const DWORD max_count = 2048;
  DWORD processes_count = 0;
  ProcessDetail *processes = (ProcessDetail *) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, max_count * sizeof(ProcessDetail));
  if (processes == NULL) {
    print_err(TEXT("HeapAlloc"));
    return 1;
  }

  if (!GetProcessesList(processes, max_count, &processes_count)) {
    return 1;
  }

  SortProcessesList(processes, processes_count);

  PrintProcessesList(processes, processes_count);

  if (HeapFree(GetProcessHeap(), 0, processes) == FALSE) {
    print_err(TEXT("HeapFree"));

    return 1;
  }

  return 0;
}
