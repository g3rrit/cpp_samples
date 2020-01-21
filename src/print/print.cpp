#include <iostream>

auto fprint(std::ostream &os) { return; }

template <class T, class ... Ts>
auto fprint(std::ostream &os, const T& arg, const Ts&... args)
{
	std::cout << arg << " ";
	fprint(os, args...);
}

template <class ... Ts>
auto fprintln(std::ostream &os, const Ts&... args) 
{
	fprint(os, args...);
	os << std::endl;
}

template <class ... Ts>
auto print(const Ts&... args) 
{
	fprint(std::cout, args...);
}

template <class ... Ts>
auto println(const Ts&... args) 
{
	fprintln(std::cout, args...);
}
