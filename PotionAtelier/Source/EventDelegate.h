#pragma once
#include <functional>
#include <vector>
#include <concepts>

/* ����
std::function�� ��ȭ�����̶�� �����ϸ� ����.
Ŭ������ �ܺο��� �Լ��� ���
-> ����� �Լ��� ���ϴ� Ÿ�ֿ̹� ȣ���� �� �ְ� �ӽ÷� ������ ����.
-> ���ϴ� Ÿ�ֿ̹� ������ ���� �Լ� Call

class A
{
	DECLARE_DELEGATE(Event, int)			// Type�� Event��� �̸����� ����, �μ��� int.

	Event event;		// ������ ������ ������ ������ ���

public:
	Event& GetEvent() { return event; }		// �ٱ����� ������ �� �ִ� Get�Լ�
};

int main()
{
	A a;

	// �Լ� ����� ������ �����µ� -> ���ϴ� �Լ��� Bind
	a.GetEvent().Bind([](int a){ return a; });

	// ���ϴ� Ÿ�ֿ̹� ����ߴ� �Լ��� �μ��� �Բ� ȣ��
	a.Execute(5);
};



Object::Awake() ���� Component���� ������ �Լ��� Ŭ������ ���
-> ����� �Լ��� ���ϴ� Ÿ�ֿ̹� ȣ���� �� �ְ� �ӽ÷� ������ ����.
-> ���ϴ� Ÿ�ֿ̹� ������ ���� �Լ� Call

* ����
class FooComponent
{
	DECLARE_EVENT(Event, FooComponent, int, float)		// Type�� Event��� �̸����� ����, ������ FooComponent, �μ��� int�� float 2���� ����.

	Event event;		// ������ ������ ������ ������ ���

public:
	Event& GetEvent() { return event; }		// �ٱ����� ������ �� �ִ� Get�Լ�
};


class BarComponent
{
	void BarFunction(int a, float b);		// ����� �Լ��� �ٷ� �̳༮
};


class Object
{
public:
	void Awake()
	{
		auto& foo = &AddComponent<FooComponent>();
		auto& bar = &AddComponent<BarComponent>();

		foo.GetEvent().AddMember(&BarComponent::BarFunction, *bar);		// ����� �Լ������ͷ� �־��ְ�, Instance �����͵� �Բ� ���
	}
};

���� FooComponent�� event�� �Լ��� ����� �Ϸ�.
���ϴ� ���� FooComponent������ eventȣ���ϸ� ��.

event.BroadCast(10, 5.f);
event.BroadCast(3, 0.f);

*/

#define DECLARE_DELEGATE(type_name, ...) \
	using type_name = Event::EventDelegate<void(__VA_ARGS__)>;

#define DECLARE_DELEGATE_RETURNVALUE(type_name, return_value, ...) \
	using type_name = Event::EventDelegate<return_value(__VA_ARGS__)>;

#define DECLARE_MULTICAST_DELEGATE(type_name, ...) \
	using type_name = Event::MultiCastDelegate<__VA_ARGS__>; \

#define DECLARE_EVENT(type_name, owner, ...) \
	class type_name : public Event::MultiCastDelegate<__VA_ARGS__> \
	{ \
	private: \
		friend class owner; \
		using MultiCastDelegate::Broadcast; \
		using MultiCastDelegate::RemoveAll; \
		using MultiCastDelegate::Remove; \
	};


// ImGuiGizmo���� Delegate��� �̸��� ���ļ�...
namespace Event
{
	template<typename...>
	class EventDelegate;

	template<typename ReturnType, typename... Args>
	class EventDelegate<ReturnType(Args...)>
	{
		using DelegateType = std::function<ReturnType(Args...)>;

	public:
		EventDelegate() = default;
		EventDelegate(EventDelegate const&) = default;
		EventDelegate(EventDelegate&&) noexcept = default;
		~EventDelegate() = default;
		EventDelegate& operator=(EventDelegate const&) = default;
		EventDelegate& operator=(EventDelegate&&) noexcept = default;

		template<typename F> requires std::is_constructible_v<DelegateType, F>
		void Bind(F&& callable)
		{
			callback = callable;
		}

		template<typename T>
		void BindMember(ReturnType(T::* mem_pfn)(Args...), T& instance)
		{
			callback = [&instance, mem_pfn](Args&&... args) mutable -> ReturnType {return (instance.*mem_pfn)(std::forward<Args>(args)...); };
		}

		void UnBind()
		{
			callback = nullptr;
		}

		ReturnType Execute(Args&&... args)
		{
			return callback(std::forward<Args>(args)...);
		}

		bool IsBound() const { return callback != nullptr; }

	private:
		DelegateType callback = nullptr;
	};

	class DelegateHandle
	{
	public:
		DelegateHandle() : id(INVALID_ID) {}
		explicit DelegateHandle(int) : id(GenerateID()) {}
		~DelegateHandle() noexcept = default;
		DelegateHandle(DelegateHandle const&) = default;
		DelegateHandle(DelegateHandle&& that) noexcept : id(that.id)
		{
			that.Reset();
		}
		DelegateHandle& operator=(DelegateHandle const&) = default;
		DelegateHandle& operator=(DelegateHandle&& that) noexcept
		{
			id = that.id;
			that.Reset();
			return *this;
		}

		operator bool() const
		{
			return IsValid();
		}

		bool operator==(DelegateHandle const& that) const
		{
			return id == that.id;
		}

		bool operator<(DelegateHandle const& that) const
		{
			return id < that.id;
		}

		bool IsValid() const
		{
			return id != INVALID_ID;
		}

		void Reset()
		{
			id = INVALID_ID;
		}

	private:
		size_t id;

	private:

		inline static constexpr size_t INVALID_ID = size_t(-1);
		static size_t GenerateID()
		{
			static size_t current_id = 0;
			return current_id++;
		}
	};

	template<typename... Args>
	class MultiCastDelegate
	{
		using DelegateType = std::function<void(Args...)>;
		using HandleDelegatePair = std::pair<DelegateHandle, DelegateType>;

	public:
		MultiCastDelegate() = default;
		MultiCastDelegate(MultiCastDelegate const&) = default;
		MultiCastDelegate(MultiCastDelegate&&) noexcept = default;
		~MultiCastDelegate() = default;
		MultiCastDelegate& operator=(MultiCastDelegate const&) = default;
		MultiCastDelegate& operator=(MultiCastDelegate&&) noexcept = default;

		template<typename F> requires std::is_constructible_v<DelegateType, F>
		[[maybe_unused]] DelegateHandle Add(F&& callable)
		{
			delegate_array.emplace_back(DelegateHandle(0), std::forward<F>(callable));
			return delegate_array.back().first;
		}

		template<typename T>
		[[maybe_unused]] DelegateHandle AddMember(void(T::* mem_pfn)(Args...), T& instance)
		{
			delegate_array.emplace_back(DelegateHandle(0), [&instance, mem_pfn](Args&&... args) mutable -> void {return (instance.*mem_pfn)(std::forward<Args>(args)...); });
			return delegate_array.back().first;
		}

		template<typename F> requires std::is_constructible_v<DelegateType, F>
		[[nodiscard]] DelegateHandle operator+=(F&& callable) noexcept
		{
			return Add(std::forward<F>(callable));
		}

		[[maybe_unused]] bool operator-=(DelegateHandle& handle)
		{
			return Remove(handle);
		}

		[[maybe_unused]] bool Remove(DelegateHandle& handle)
		{
			if (handle.IsValid())
			{
				for (size_t i = 0; i < delegate_array.size(); ++i)
				{
					if (delegate_array[i].Handle == handle)
					{
						std::swap(delegate_array[i], delegate_array.back());
						delegate_array.pop_back();
						handle.Reset();
						return true;
					}
				}
			}
			return false;
		}

		void RemoveAll()
		{
			delegate_array.clear();
		}

		void Broadcast(Args... args)
		{
			for (size_t i = 0; i < delegate_array.size(); ++i)
			{
				if (delegate_array[i].first.IsValid()) delegate_array[i].second(std::forward<Args>(args)...);
			}
		}

		bool IsHandleBound(DelegateHandle const& handle) const
		{
			if (handle.IsValid())
			{
				for (size_t i = 0; i < delegate_array.size(); ++i)
				{
					if (delegate_array[i].Handle == handle) return true;
				}
			}
			return false;
		}

	private:
		std::vector<HandleDelegatePair> delegate_array;

	};
}