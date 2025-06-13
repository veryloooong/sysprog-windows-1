#include <Windows.h>

#include <TlHelp32.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

void print_err(TCHAR const *where) {
  DWORD code = GetLastError();
  _tprintf(TEXT("%s failed error code %lu\n"), where, code);
}

int _cmp_function(const void *a, const void *b) { return 0; }

typedef struct _ProcessDetail {
  DWORD pid;
  DWORD ppid;
  TCHAR pname[MAX_PATH];
  TCHAR ppname[MAX_PATH];
} ProcessDetail;

BOOL GetProcessesList(ProcessDetail *processes, DWORD count) {
  HANDLE snapshot;
  HANDLE process;
  PROCESSENTRY32 pe32;
  DWORD priority;

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

  do {
    // TODO
  } while (Process32Next(snapshot, &pe32));

  CloseHandle(snapshot);

  return TRUE;
}

int main() {
  _tprintf(TEXT("PID\tPPID\tName\tParentName\n"));
  const DWORD cnt = 2048;
  ProcessDetail processes[cnt];

  if (!GetProcessesList(processes, cnt)) {
    return 1;
  }

  return 0;
}
