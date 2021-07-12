#include <iostream>
#include <Windows.h>
#include <vector>

using namespace std;

int main()
{
  setlocale(LC_ALL, "chs");
  HKEY hKey;
  LSTATUS retCode;

  retCode = RegOpenKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\MyComputer\\NameSpace", NULL, KEY_ALL_ACCESS, &hKey);
  if (retCode != ERROR_SUCCESS)
  {
    printf("Error opening key\n");
    return 1;
  }

  // 打开成功后枚举所有子项
  wchar_t achClass[MAX_PATH] = L""; // 类名缓冲区
  DWORD cchClassName = MAX_PATH;    // 类字符串的大小
  DWORD cSubKeys = 0;               // 子键数
  DWORD cbMaxSubKey;                // 最长子密钥大小
  DWORD cchMaxClass;                // 最长的类串
  DWORD cValues;                    // 键值的数量

  retCode = RegQueryInfoKeyW(
    hKey,                  // key handle
    achClass,              // 类名缓冲区
    &cchClassName,         // 类字符串的大小
    NULL,                  // 预订的
    &cSubKeys,             // 子键数
    &cbMaxSubKey,          // 最长子密钥大小
    &cchMaxClass,          // 最长的类串
    &cValues,              // 此键的值数
    NULL,          // 最长值名称
    NULL,       // 最长值数据
    NULL, // 安全描述符
    NULL       // 上次写入时间
  );

  if (!cSubKeys) 
  {
    cout << "没有检查到可删除项\n";
    return 0;
  }

  cbMaxSubKey += sizeof(char);

  wchar_t* keyName = new wchar_t[cbMaxSubKey];
  DWORD keySize;

  vector<std::wstring> children;

  // 遍历子键
  for (int i = 0; i < cSubKeys; ++i)
  {
    ZeroMemory(keyName, cbMaxSubKey);
    keySize = cbMaxSubKey;

    // 获取子键名称
    retCode = RegEnumKeyExW(hKey, i, keyName, &keySize, NULL, NULL, NULL, NULL);
    if (retCode != ERROR_SUCCESS) {
      cout << "获取子键名称失败 " << retCode << endl;
      break;
    }

    wchar_t pvData[MAX_PATH];
    DWORD cbData = MAX_PATH;

    // 获取子键的默认数据
    retCode = RegGetValueW(hKey, keyName, NULL, RRF_RT_REG_SZ, NULL, pvData, &cbData);
    if (retCode != ERROR_SUCCESS) {
      cout << "获取子键的默认数据 " << retCode << endl;
      break;
    }

    printf("%d %ls\n", i + 1, pvData);
    children.push_back(keyName);
  }
  delete[] keyName;

  // 输入要删除的id
  cout << "\n输入要删除项的序号: ";
  int id;
  cin >> id;

  if (id < 0 || id > children.size())
    return 0;


  if (RegDeleteKeyExW(hKey, children.at(id - 1).data(), KEY_WOW64_32KEY, NULL) == ERROR_SUCCESS)
    cout << "删除成功\n";
  else 
    cout << "删除失败\n";

  RegCloseKey(hKey);
  return 0;
}
