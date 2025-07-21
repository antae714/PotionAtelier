#include "../inc/SimpleLog.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <filesystem>

simple::Logger::Logger(const char* LogFolder) :
	workerThreads(&Logger::WriteLogFunc, this),
	logDir(LogFolder),
	db(GetLogFileName())
{
	ss_time.str().reserve(30);

	// �α� ���̺� ����
	db << "CREATE TABLE IF NOT EXISTS Logs ("
		"ID INTEGER PRIMARY KEY AUTOINCREMENT, "
		"Timestamp TEXT NOT NULL, "
		"LogType TEXT NOT NULL, "
		"Message TEXT NOT NULL"
		");";
}

simple::Logger::~Logger()
{
	isEndLogger = true;   //�ΰ� ���� Ȱ��ȭ
	cv.notify_one();	  //������ ����.
	workerThreads.join(); //������ ���� ���
}

void simple::Logger::WriteLog(const char* type, const char* message)
{
	//stringstream �ʱ�ȭ
	ss_time.str("");
	ss_time.clear();

	// ���� �ð� ���
	auto now = std::chrono::system_clock::now();
	std::time_t now_c = std::chrono::system_clock::to_time_t(now);
	std::tm local_time;
	localtime_s(&local_time, &now_c);
	ss_time << std::put_time(&local_time, "%Y-%m-%d %H:%M:%S");

	//�α� �ۼ�
	logData.TimeStamp = ss_time.str();
	logData.LogType = type;
	logData.Message = message;

	//�α� ��� ����. �����ÿ��� ��
	listMutex.lock();
	logList.emplace_back(logData);
	listMutex.unlock();

	cv.notify_one(); //�ۼ� ����
}

std::string simple::Logger::GetLogFileName()
{
	LogFileName = logDir;
	if (LogFileName.back() != '/' && LogFileName.back() != '\\')
	{
		LogFileName += '/';
	}
	LogFileName += "logs.db";

	//���� ���� ���� Ȯ�� �� ����.
	if (!std::filesystem::exists(logDir.c_str()))
	{
		std::filesystem::create_directories(logDir.c_str());
	}

	return LogFileName;
}

void simple::Logger::WriteLogFunc()
{
	while (!isEndLogger)
	{
		{
			std::unique_lock lock(cv_mutex);
			cv.wait(lock, [this] {return !logList.empty() || isEndLogger; });
		}
		std::ofstream outLog(LogFileName.c_str(), std::ios::app);

		while (!logList.empty())
		{
			if (!outLog)
			{
				break; //��õ�
			}

			LogData& front = logList.front();
			db << "INSERT INTO Logs (Timestamp, LogType, Message) VALUES (?, ?, ?);"
				<< front.TimeStamp
				<< front.LogType
				<< front.Message;

			//�����ÿ��� ��
			listMutex.lock();
			logList.pop_front();
			listMutex.unlock();
		}		
		outLog.close();
	}

	//���� �α� �ۼ�
	std::ofstream outLog(LogFileName.c_str(), std::ios::app);
	while(!logList.empty())
	{		
		LogData& front = logList.front();
		db << "INSERT INTO Logs (Timestamp, LogType, Message) VALUES (?, ?, ?);"
			<< front.TimeStamp
			<< front.LogType
			<< front.Message;
		logList.pop_front();		
	}
	outLog.close();
	return;
}
