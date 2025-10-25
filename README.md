# 🧹 Registry Editor MRU Cleaner

A Windows system utility written in C that recursively scans and deletes **MRU (Most Recently Used)** entries from the Windows Registry. This tool helps clean up privacy-sensitive traces like `RecentDocs`, `RunMRU`, and other MRUList entries.

## 🚀 Features

- Recursively deletes registry subkeys containing `"MRU"` from specified paths.
- Removes `MRUList` values from registry entries.
- Notifies the system of registry changes using `SendMessageTimeout`.
- Targets keys like:
  - `HKEY_CURRENT_USER\Software`
  - `HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\RecentDocs`
  - `HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\RunMRU`

## 🛠️ How It Works

The program uses Windows API functions like:
- `RegOpenKeyExA`
- `RegEnumKeyExA`
- `RegDeleteKeyA`
- `RegDeleteValueA`
- `SendMessageTimeout`

It performs recursive deletion of subkeys and values that match `"MRU"` and resets enumeration after each deletion to ensure completeness.

## 📦 Build Instructions

### Requirements
- Windows OS
- C Compiler (e.g., MinGW, MSVC)

### Compile with GCC (MinGW)
```bash
gcc src/registry_mru_cleaner.c -o RegistryMRUCleaner.exe
