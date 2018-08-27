#ifndef FASTFUNCTION_HPP
#define FASTFUNCTION_HPP

#include <functional>

template<typename Signature, std::size_t Size = 64>
class FastFunction;

template<typename R, typename ...Args, std::size_t Size>
class FastFunction<R(Args...), Size>
{
public:

	using Target = typename std::aligned_storage<Size, 8ULL>::type;
	using Invoker = R(*)(Target&, Args&&...);
	using Destroyer = void(*)(Target&);

	/*
	* Default constructs an empty function call wrapper.
	*/
	FastFunction() noexcept :
		invoker_(nullptr),
		destroyer_(nullptr)
	{
	}

	/*
	* Creates an empty function call wrapper.
	*/
	FastFunction(std::nullptr_t) noexcept :
		invoker_(nullptr),
		destroyer_(nullptr)
	{
	}

	/*
	* Copy constructor
	*/
	FastFunction(const FastFunction& x)
	{
		if (static_cast<bool>(x))
		{
			new (&target_) Target(x.target_);

			invoker_ = x.invoker_;
			destroyer_ = x.destroyer_;
		}
	}

	/*
	* Move constructor
	*/
	FastFunction(FastFunction&& x) noexcept
	{
		x.swap(*this);
	}

	/*
	* Constructor that builds a function that targets a copy of the incoming function object.
	*/
	template<typename Callable,
		typename RC = std::result_of_t<Callable&(Args...)>,
		typename = std::enable_if_t< 
			(std::is_void_v<R> || std::is_same_v<RC, R> || std::is_convertible_v<RC, R>) &&
			!std::is_same_v<FastFunction, Callable> &&
			(alignof(Callable) <= alignof(Target)) &&
			(sizeof(Callable) <= sizeof(Target))>>
	FastFunction(Callable f)
	{
		new (&target_) Callable(f);
		invoker_ = &invoke<Callable>;
		destroyer_ = &destroy<Callable>;
	}

	/*
	* Copy-assignment operator
	*/
	FastFunction& operator=(const FastFunction& x)
	{
		FastFunction(x).swap(*this);
		return *this;
	}

	/*
	* Move-assignment operator
	*/
	FastFunction& operator=(FastFunction&& x) noexcept
	{
		FastFunction(std::move(x)).swap(*this);
		return *this;
	}

	/*
	* Assignment to null
	*/
	FastFunction& operator=(std::nullptr_t) noexcept
	{
		if (invoker_)
		{
			destroyer_(target_);
			invoker_ = nullptr;
			destroyer_ = nullptr;
		}

		return *this;
	}

	/*
	* Assignment to new target.
	*/
	template<typename Callable,	typename RC = std::result_of_t<Callable&(Args...)>>
	typename std::enable_if_t<(std::is_void_v<R> || std::is_same_v<RC, R> || std::is_convertible_v<RC, R>), FastFunction&>
	operator=(Callable&& f)
	{
		FastFunction(std::forward<Callable>(f)).swap(*this);
		return *this;
	}

	/*
	* Assignment to new target (reference wrapper overload).
	*/
	template<typename Callable>
	FastFunction& operator=(std::reference_wrapper<Callable> f)
	{
		FastFunction(f).swap(*this);
		return *this;
	}

	/*
	* Destructor
	*/
	~FastFunction()
	{
		if (destroyer_)
			destroyer_(target_);
	}

	/*
	 *  Determine if the this wrapper has a target.
	*/
	explicit operator bool() const noexcept
	{
		return !!invoker_;
	}

	/*
	* Invokes the target function with arguments.
	*/
	R operator()(Args&&... args)
	{
		return invoker_(target_, std::forward<Args>(args)...);
	}

private:

	void swap(FastFunction& x) noexcept
	{
		std::swap(target_, x.target_);
		std::swap(invoker_, x.invoker_);
		std::swap(destroyer_, x.destroyer_);
	}

	template<typename Callable>
	static R invoke(Target& target, Args&&... args)
	{
		Callable& f = *reinterpret_cast<Callable*>(&target);
		return f(std::forward<Args>(args)...);
	}

	template<typename Callable>
	static void destroy(Target& dest)
	{
		Callable& f = *reinterpret_cast<Callable*>(&dest);
		f.~Callable();
	}

	Target target_;
	Invoker invoker_;
	Destroyer destroyer_;
};

#endif