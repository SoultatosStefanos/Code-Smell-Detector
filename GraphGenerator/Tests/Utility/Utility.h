// Contains testing utilities.
// Soultatos Stefanos 2022

#pragma once

#include <string>
#include <libgen.h>
#include <cassert>
#include <random>
#include "DependenciesMining.h"

namespace tests::utility {

	template <typename Seed = std::random_device>
	auto Rng() -> auto&
	{
		thread_local static std::mt19937 generator{Seed{}()};
		return generator;
	}

	// Uniform random function, seeded by thread local generator.
	template <typename T>
	T URandom(T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max())
	{
		static_assert(std::is_integral_v<T> or std::is_floating_point_v<T>);

		using Distribution = std::conditional_t<std::is_integral_v<T>,
										std::uniform_int_distribution<T>,
										std::uniform_real_distribution<T>>;

		Distribution dist{min, max};
		return static_cast<T>(dist(Rng()));
	}

	std::string ResolvePath(std::string from, const std::string& to); // Useful for out-of-source builds.

	template <typename T, typename Function>
	void Repeat(T times, Function f) {
		static_assert(std::is_integral_v<T> or std::is_floating_point_v<T>);

		for (T i = 0; i < times; ++i)
			f();
	}

	bool AreEqual(const dependenciesMining::Symbol& lhs, const dependenciesMining::Symbol& rhs);
	bool AreEqual(const dependenciesMining::SymbolTable& lhs, const dependenciesMining::SymbolTable& rhs);

} // tests::utility

#define RESOLVE_PATH(to) tests::utility::ResolvePath(__FILE__, to)