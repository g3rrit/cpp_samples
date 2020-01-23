#include <iostream>
#include <variant>
#include <memory>
#include <functional>

template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

/** TYPE FIXPOINT
 * 
 * template <class K>
 * using Var = std::variant<std::monostate, std::pair<int, K*>>;
 *
 * template <template<class> class K>
 * struct Fix : K<Fix<K>>
 * {
 *	using K<Fix>::K;
 * };
 * 
 * using List = Fix<Var>;
 */

template <class T, class K>
using upp = std::unique_ptr<std::pair<T, K>>;

template <class T>
struct List
{
	std::variant<std::monostate, upp<T, List<T>>> data;

	List(std::monostate m) 
		: data(m) {}
	List(upp<T, List<T>>&& l) 
		: data(std::in_place_index<1>, std::move(l)) {}
};

template <class T, class K>
auto fold(const List<T>& list,K m, std::function<K(K,T)> f) -> K
{
	return std::visit(overloaded {
		[&] (const std::monostate _) { return m; },
		[&] (const upp<K, List<T>>& p) { return fold(p->second, f(m, p->first), f); },
	}, list.data);
}

template <class T, class K>
auto cons(T t, K l) -> upp<T, List<T>>
{
	return std::make_unique<std::pair<T, List<T>>>(t, List<T> { std::move(l) });
}

auto nil() -> std::monostate
{
	return std::monostate();
}

auto main() -> int {

	List<int> list { cons(10, cons(15, cons(16, nil()))) };

	int a = fold<int, int>(list, 0, [](int l, int r) -> int { return l + r; });
	std::cout << a << std::endl;

	return 0;
}
