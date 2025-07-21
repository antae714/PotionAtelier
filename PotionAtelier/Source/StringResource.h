#pragma once
#include <map>
#include <string>
#include "Core/TSingleton.h"
#include <fstream>
#include <filesystem>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
// Memo
// 

class StringResource : public TSingleton<StringResource>
{
	const std::wstring resource_path{ L"./Resource/Strings.txt" };
	std::map<std::wstring, std::wstring> strings;

public:
	StringResource();

public:
	static std::wstring GetTutorialText(std::wstring key)
	{
		auto& instance = GetInstance();
		if (instance.strings.find(key) != instance.strings.end())
		{
			return instance.strings[key];
		}
		else
		{
			return L"";
		}
	}
};


struct CSVFildData
{
	int offset;
	std::function<void(std::string_view value, void* buffer)> parser;
};
#include <cstddef>  // offsetof�� ���ǵ� ���
/**
 * @brief 
 * 
 *  ��� ����
 *  ���� , "" �ִ°� ó���� ����....
 *  
 * 
struct Test 
{
	int ID;
	std::string Name;
};
	CSVReader reader;
	reader.header["ID"] = { offsetof(Test, ID), [](std::string_view value, void* buffer) { *static_cast<int*>(buffer) = std::stoi(value.data()); } };
	reader.header["Name"] = { offsetof(Test, Name), [](std::string_view value, void* buffer) { *static_cast<std::string*>(buffer) = value.data(); } };

	std::vector<Test> tests;
	std::stringstream sstream;
	sstream <<
R"R(ID,Name
1,t
2,tt
44,asd)R";
	reader.Read(sstream, tests);
 */

struct CSVReader 
{
	std::map<std::string, CSVFildData> header;


    template<class T>
    void Read(std::istream& sstream, std::vector<T>& container)
    {
        std::string line;
        // 1. ù ��° ��(��� ��) �б�
        if (!std::getline(sstream, line))
        {
            std::cerr << "File is empty or header cannot be read.\n";
            return;
        }

        // CSV�� ��� ��ū �и� (�ܼ� ',' ������ ����)
        std::vector<std::string> headerTokens;
        {
            std::istringstream iss(line);
            std::string token;
            while (std::getline(iss, token, ','))
            {
                headerTokens.push_back(token);
            }
        }

        // ��� �࿡ ���� �÷���� CSVReader::header�� ��ϵ� �ʵ���� ��Ī�Ͽ�,
        // �� �ʵ��� CSV ���� �� �ε����� ���.
        // (���⼭�� ������ �ӽ� ���� ���)
        std::unordered_map<std::string, int> fieldColumnIndices;
        for (int i = 0; i < static_cast<int>(headerTokens.size()); ++i)
        {
            // headerTokens[i]�� �츮�� ó���� �ʵ忡 ��ϵǾ� �ִ��� Ȯ��.
            auto it = header.find(headerTokens[i]);
            if (it != header.end())
            {
                fieldColumnIndices[headerTokens[i]] = i;
            }
        }

        // 2. ������ �� ó��: �� �ึ�� T ��ü ���� ��, �� ��� �ʵ忡 ���� �Ҵ�.
        while (std::getline(sstream, line))
        {
            // CSV ������ �� ���� ','�� �����Ͽ� ��ūȭ.
            std::vector<std::string> tokens;
            {
                std::istringstream iss(line);
                std::string token;
                while (std::getline(iss, token, ','))
                {
                    tokens.push_back(token);
                }
            }

            T row{}; // ���ο� T ��ü (�⺻ ����)

            // CSVReader::header�� ��ϵ� �� �ʵ忡 ���� ���� ����.
            // fieldColumnIndices���� CSV ������ �� �ʵ���� �� ��° �÷��� ��ġ�ϴ����� ����Ǿ� ����.
            for (const auto& [fieldName, colIndex] : fieldColumnIndices)
            {
                // CSVFildData ����: T ��ü �� ��� �����¿� ���� �������, �׸��� �Ľ� �Լ��� ��������.
                const CSVFildData& fieldData = header[fieldName];

                // ������ �࿡�� �ش� �÷��� ���� �����ϴ��� üũ.
                if (colIndex < static_cast<int>(tokens.size()))
                {
                    // T ��ü�� base �ּҿ�, �������� ���� �ش� ����� ������ ���.
                    void* fieldPtr = reinterpret_cast<void*>(reinterpret_cast<char*>(&row) + fieldData.offset);
                    // �ļ� �Լ� ȣ��: ���ڿ� ���� ������ Ÿ������ ��ȯ �� T ��ü�� �ش� ����� ����.
                    fieldData.parser(tokens[colIndex], fieldPtr);
                }
                else
                {
                    std::cerr << "Column index " << colIndex << " out of range in line: " << line << "\n";
                }
            }

            // �ϼ��� ��(row)�� �����̳ʿ� �߰�.
            container.push_back(std::move(row));
        }

    }
    template<class T>
    void Read(const std::filesystem::path& path, std::vector<T>& container)
    {
        std::ifstream ifs(path);
        if (ifs.is_open())
        {
            Read(ifs, container);
            ifs.close();
        }
    }
};