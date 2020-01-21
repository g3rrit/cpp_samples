#include <iostream>
#include <variant>

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

using List = std::variant<
	std::monostate,
	std::pair<int, void*>
>;

auto main() -> int {

	List list = std::make_pair(10, nullptr);

	std::visit(overloaded {
		[&](const std::monostate _) { std::cout << "monostate"; },
		[&](const std::pair<int, void*>& cons) { std::cout << cons.first; },
	}, list);

	return 0;
}
