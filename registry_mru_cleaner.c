#include <windows.h>
#include <stdio.h>

// Function prototypes
void DeleteMRUSubkeysRecursively(HKEY hKey, LPCSTR subkeyPath);
void DeleteSubkeys(HKEY hKey, LPCSTR subkeyPath);
void DeleteMRUListValue(HKEY hKey, LPCSTR subkeyPath);

// Function to delete subkeys and call the recursive delete function for deeper subkeys
void DeleteSubkeys(HKEY hKey, LPCSTR subkeyPath) {
    HKEY hSubKey;
    CHAR subkeyName[256];
    DWORD subkeyNameSize;
    DWORD index = 0;

    // Open the specified subkey
    if (RegOpenKeyExA(hKey, subkeyPath, 0, KEY_READ | KEY_WRITE, &hSubKey) == ERROR_SUCCESS) {
        printf("Opened registry key: %s\n", subkeyPath);

        while (1) {
            subkeyNameSize = sizeof(subkeyName);
            LONG result = RegEnumKeyExA(hSubKey, index, subkeyName, &subkeyNameSize, NULL, NULL, NULL, NULL);

            if (result == ERROR_NO_MORE_ITEMS) {
                break;  // No more subkeys
            } else if (result == ERROR_SUCCESS) {
                // Form the full subkey path
                CHAR fullSubkeyPath[512];
                snprintf(fullSubkeyPath, sizeof(fullSubkeyPath), "%s\\%s", subkeyPath, subkeyName);

                // Recursively delete the subkey and its subkeys
                DeleteMRUSubkeysRecursively(hKey, fullSubkeyPath);

                // Reset the subkey enumeration after deletion
                index = 0;
            } else {
                printf("Failed to enumerate subkeys.\n");
                break;
            }
        }

        // Close the subkey
        if (RegCloseKey(hSubKey) == ERROR_SUCCESS) {
            printf("Closed registry key: %s\n", subkeyPath);
        } else {
            printf("Failed to close registry key: %s\n", subkeyPath);
        }
    } else {
        printf("Failed to open registry key: %s\n", subkeyPath);
    }
}

// Function to recursively delete subkeys containing "MRU"
void DeleteMRUSubkeysRecursively(HKEY hKey, LPCSTR subkeyPath) {
    HKEY hSubKey;
    CHAR subkeyName[256];
    DWORD subkeyNameSize;
    DWORD index = 0;

    // Open the specified subkey
    if (RegOpenKeyExA(hKey, subkeyPath, 0, KEY_READ | KEY_WRITE, &hSubKey) == ERROR_SUCCESS) {
        printf("Opened subkey for recursive delete: %s\n", subkeyPath);

        while (1) {
            subkeyNameSize = sizeof(subkeyName);
            LONG result = RegEnumKeyExA(hSubKey, index, subkeyName, &subkeyNameSize, NULL, NULL, NULL, NULL);

            if (result == ERROR_NO_MORE_ITEMS) {
                break;  // No more subkeys
            } else if (result == ERROR_SUCCESS) {
                // Form the full subkey path
                CHAR fullSubkeyPath[512];
                snprintf(fullSubkeyPath, sizeof(fullSubkeyPath), "%s\\%s", subkeyPath, subkeyName);

                // Recursively delete the subkey and its subkeys
                DeleteMRUSubkeysRecursively(hKey, fullSubkeyPath);

                // Check if subkey name contains "MRU"
                if (strstr(subkeyName, "MRU") != NULL) {
                    // Delete the subkey
                    if (RegDeleteKeyA(hSubKey, subkeyName) == ERROR_SUCCESS) {
                        printf("Deleted subkey: %s\n", fullSubkeyPath);
                    } else {
                        printf("Failed to delete subkey: %s\n", fullSubkeyPath);
                    }

                    // Reset the subkey enumeration after deletion
                    index = 0;
                } else {
                    index++;  // Move to the next subkey if it doesn't contain "MRU"
                }
            } else {
                printf("Failed to enumerate subkeys.\n");
                break;
            }
        }

        // Delete MRUList value in the subkey
        DeleteMRUListValue(hKey, subkeyPath);

        // Close the subkey
        if (RegCloseKey(hSubKey) == ERROR_SUCCESS) {
            printf("Closed subkey: %s\n", subkeyPath);
        } else {
            printf("Failed to close subkey: %s\n", subkeyPath);
        }
    } else {
        printf("Failed to open registry key: %s\n", subkeyPath);
    }
}

// Function to delete the MRUList value in a specified subkey
void DeleteMRUListValue(HKEY hKey, LPCSTR subkeyPath) {
    HKEY hSubKey;

    // Open the specified subkey
    if (RegOpenKeyExA(hKey, subkeyPath, 0, KEY_READ | KEY_WRITE, &hSubKey) == ERROR_SUCCESS) {
        // Delete the MRUList value
        if (RegDeleteValueA(hSubKey, "MRUList") == ERROR_SUCCESS) {
            printf("Deleted MRUList value in subkey: %s\n", subkeyPath);
        } else {
            printf("Failed to delete MRUList value in subkey: %s\n", subkeyPath);
        }

        // Close the subkey
        if (RegCloseKey(hSubKey) == ERROR_SUCCESS) {
            printf("Closed registry key: %s\n", subkeyPath);
        } else {
            printf("Failed to close registry key: %s\n", subkeyPath);
        }
    } else {
        printf("Failed to open registry key: %s\n", subkeyPath);
    }
}

int main() {
    // Example: Delete subkeys containing "MRU" in the Software subkey under HKEY_CURRENT_USER
    DeleteSubkeys(HKEY_CURRENT_USER, "Software");

    // Additional: Delete RecentDocs and RunMRU and their MRUList values
    DeleteSubkeys(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RecentDocs");
    DeleteSubkeys(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RunMRU");

    // Notify system of registry change
    DWORD_PTR result;
    SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)"Environment", SMTO_ABORTIFHUNG, 5000, &result);
    printf("Notified system of registry change.\n");

    return 0;
}
