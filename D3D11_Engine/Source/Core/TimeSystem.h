#pragma once
#include <Core/TSingleton.h>
#include <windows.h>
#include <cmath>
#include <functional>

class QPCTime : public TSingleton<QPCTime>
{
	friend class TSingleton;
private:
	QPCTime();
	~QPCTime() = default; 

public:
	void UpdateTime();
	void UpdateDelayedInvoker();
	const int GetFrameRate() const
	{
		if (deltaTime_ms == 0) return 0;

		return static_cast<int>(std::ceil(((1000.0f / deltaTime_ms) * 1000.f) / 1000.f));
	}

	float GetDeltaTime_ms(bool isScale = true) const;
	float GetDeltaTime(bool isScale = true) const { return (GetDeltaTime_ms(isScale) / 1000.f);}

	/* DeltaTime (���� : sec). timeScale ���� �޽��ϴ�.*/
	float GetPropertyDeltaTime() const { return deltaTime_ms * timeScale / 1000.f; }
	/* DeltaTime (���� : sec). timeScale ���� �޽��ϴ�.*/
	__declspec(property(get = GetPropertyDeltaTime)) float DeltaTime;

	/* DeltaTime (���� : ms). timeScale ���� �޽��ϴ�.*/
	float GetPropertyDeltaTime_ms() const { return deltaTime_ms * timeScale; }
	/* DeltaTime (���� : ms). timeScale ���� �޽��ϴ�.*/
	__declspec(property(get = GetPropertyDeltaTime_ms)) float DeltaTime_ms;

	int FixedUpdatePerSec = 50;
	float GetFixedDelta() const { return 1.f / (float)FixedUpdatePerSec ; }

	float timeScale;

public:
	/// <summary>
	/// ���޹��� �Լ��� ������ �ð� ���Ŀ� ȣ�����ݴϴ�. 
	/// </summary>
	/// <param name="funtion :">������ ���� ȣ��� �Լ�</param>
	/// <param name="delayTime :">������ �ð� (0.f �̸� ���� �����ӿ� ȣ��.)</param>
	/// <param name="affectedTimeScale :">timeScale ���� ���� (�⺻�� = true)</param>
	void DelayedInvok(const std::function<void()>& func, float delayTime, bool affectedTimeScale = true);

	//������� �Լ��� �ʱ�ȭ
	void ClearInvokFunc() { delayFunctionsVec.clear(); }
private:
	LARGE_INTEGER previousTime;
	LARGE_INTEGER currentTime;
	LARGE_INTEGER frequency;

	float deltaTime_ms = 0;

	//tuple<�Լ�, ������ �ð�, Ÿ�ӽ����� ����, ���� �ð�, ���� ����>
	using DelayFunctionTuple = std::tuple<std::function<void()>, float, bool, float, bool>;
	std::vector<DelayFunctionTuple> delayFunctionsVec;
};

namespace TimeSystem
{
	extern QPCTime& Time;
}
