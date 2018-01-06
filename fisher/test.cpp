#include<iostream>
#include"fisher.hpp"

int main() {
	binary::bytes b;

	b << binary::endian::type::big << 'A';
	std::cout << b;
}