// function_objects.cpp : Defines the entry point for the application.
//


#include "function_object.hpp"

#include <iostream>
#include <string>

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

TEST_CASE("Testing copyable function objects", "[copyable]") {
	using namespace jl;

	int i = 0;
	auto l = [&i]() mutable -> void {++i; };

	l();

	REQUIRE(i == 1);

	auto j = make_function_object<void(void)>(l);
	j();

	REQUIRE(i == 2);

	auto k = make_function_object<void(void)>(std::move(l));
	k();

	REQUIRE(i == 3);

	j = std::move(k);

	j();

	REQUIRE(i == 4);

	auto h = [&i]() {--i; };
	auto t = make_function_object<void(void)>(h);

	t();

	REQUIRE(i == 3);

	std::swap(j, t);

	j();

	REQUIRE(i == 2);

	t();

	REQUIRE(i == 3);
}

TEST_CASE("Testing move only function objects.", "[move_only]") {
	using namespace jl;

	REQUIRE(sizeof(move_only::type_erased_callable<void(void)>) == 4 * sizeof(void*));

	size_t len = 100000;
	std::unique_ptr<int[]> big_arr = std::make_unique<int[]>(len);
	for (auto i = 0; i < len; ++i) {
		big_arr[i] = 0;
	}

	auto* peek = big_arr.get();

	REQUIRE(peek[100] == 0);

	auto p = make_function_object<void(void)>([big_arr = std::move(big_arr), len]() mutable -> void {
		for (size_t i = 0; i < len; ++i) {
			big_arr[i] += static_cast<int>(i);
		}
	});

	p();

	REQUIRE(peek[100] == 100);

	auto q = std::move(p);

	q();

	REQUIRE(peek[100] == 200);

	std::swap(p, q);

	p();

	REQUIRE(peek[100] == 300);

	REQUIRE_THROWS_AS(q(), std::runtime_error);
}

TEST_CASE("Testing copy only function objects.", "[copy_only]") {
	using namespace jl;

	int i = 0;
	auto l = [&i]() mutable -> void {++i; };

	l();

	REQUIRE(i == 1);

	auto j = make_function_object<void(void)>(l);
	j();

	REQUIRE(i == 2);

	auto k = make_function_object<void(void)>(std::move(l));
	k();

	REQUIRE(i == 3);

	j = std::move(k);

	j();

	REQUIRE(i == 4);

	auto h = [&i]() {--i; };
	auto t = make_function_object<void(void)>(h);

	t();

	REQUIRE(i == 3);

	std::swap(j, t);

	j();

	REQUIRE(i == 2);

	t();

	REQUIRE(i == 3);
}
