#pragma once
#include <string>
#include <Windows.h>

namespace WinUtility
{
    /* 
      ������ â�� ���� ��θ� ������ �����ɴϴ�. 
      �Ű������� ���ͷ� ������ Ȯ���� �Դϴ�.
    */
    std::wstring GetOpenFilePath(const wchar_t* fileType = L"*", const wchar_t* defaultFileName = nullptr);
 
    /*
      ������ â�� ���� ��θ� ������ �����ɴϴ�.
      �Ű������� ���ͷ� ������ Ȯ���� �Դϴ�.
    */
    std::wstring GetSaveAsFilePath(const wchar_t* fileType = L"*", const wchar_t* defaultFileName = nullptr);

    /*
      �޽��� �ڽ��� ���� Ȯ���� ���� �޽��� �ڽ��� ���ϴ�.
      ����ڰ� �� ������ true�� ��ȯ�ϰ�, �ƴϿ� ������ false�� ��ȯ�մϴ�.
      * title : �޽��� �ڽ��� ����
      * text : �˸�â�� �� �ȳ���
    */
    bool ShowConfirmationDialog(const wchar_t* title, const wchar_t* text);
};