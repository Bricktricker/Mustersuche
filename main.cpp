#include <cstring>
#include <array>
#include <cassert>
#include <random>
#include <benchmark/benchmark.h>

// Länge des generierten Textes
#define TEXT_LENGTH 8000000

//Wird aufgerufen, wenn das Muster im Text gefunden wurde
// Platzhaltermethode
static void hit(const size_t start, const size_t end) {
	benchmark::DoNotOptimize(start);
	benchmark::DoNotOptimize(end);
}

static void horspool(const std::vector<uint8_t>& P, const std::vector<uint8_t>& T) {
	const size_t m = P.size();
	const size_t n = T.size();
	assert(m < 255);

	// shifts berechnen
	std::array<uint8_t, 256> shifts;
	std::fill(begin(shifts), end(shifts), static_cast<uint8_t>(m));
	for (size_t i = 0; i < P.size() - 1; i++) {
		shifts[P[i]] = static_cast<uint8_t>(m - 1 - i);
	}

	uint8_t last_P = P[m - 1];
	size_t last = m - 1;

	while (true) {
		while (last < n && T[last] != last_P) {
			last += shifts[T[last]];
		}
		if (last >= n) {
			break;
		}
		if (memcmp(T.data() + last - m + 1, P.data(), m - 1) == 0) { //Rest des Textes überprüfen
			hit(last - m + 1, last + 1);
		}
		last += shifts[last_P];
	}
}

static void shift_and(const std::vector<uint8_t>& P, const std::vector<uint8_t>& T) {
	const size_t m = P.size();
	const size_t n = T.size();
	const uint64_t accept = 1ULL << (m - 1ULL);
	assert(m <= 64);

	//masken erstellen
	std::array<uint64_t, 256> masks;
	std::fill(begin(masks), end(masks), 0);
	for (size_t i = 0; i < m; i++) {
		const uint8_t c = P[i];
		masks[c] |= (1ULL << i);
	}

	uint64_t A = 0;
	for (size_t i = 0; i < n; i++) {
		A = ((A << 1) | 1) & masks[T[i]];
		if (A & accept) {
			hit(i - m + 1, i + 1);
		}
	}
}

static void bndm(const std::vector<uint8_t>& P, const std::vector<uint8_t>& T) {
	const size_t m = P.size();
	const size_t n = T.size();
	const uint64_t full = (1ULL << m) - 1;
	const uint64_t accept = 1ULL << (m - 1ULL);
	assert(m <= 64);

	//masken erstellen
	std::array<uint64_t, 256> masks;
	std::fill(begin(masks), end(masks), 0);
	for (size_t i = 0; i < m; i++) {
		const uint8_t c = P[m - i - 1];
		masks[c] |= (1ULL << i);
	}

	size_t last = m;
	while (last <= n) {
		uint64_t A = full;
		size_t lastsuffix = 0;
		size_t j = 1;
		while (A) {
			A &= masks[T[last - j]];
			if (A & accept) {
				if (j == m) {
					hit(last - m, last);
					break;
				}else{
					lastsuffix = j;
				}
			}
			j += 1;
			A <<= 1;
		}
		last += m - lastsuffix;
	}
}

///////////////////// Benchmark Methoden ////////////////////////////

// füllt T mit TEXT_LENGTH Anzahl zufälliger bytes
// wählt ein zufälliges 'patternLength' langes Muster aus T und speichert es in P
static void initStr(const size_t patternLength, std::vector<uint8_t>* P, std::vector<uint8_t>* T) {
	std::mt19937 rng(5489u); // seed
	std::uniform_int_distribution<> dist(1, 255); // zufälliges byte im Bereich [1, 255]
	T->reserve(TEXT_LENGTH);
	for (size_t i = 0; i < TEXT_LENGTH; i++) {
		T->push_back(dist(rng));
	}

	const size_t pStart = std::uniform_int_distribution<size_t>(0U, T->size() - patternLength - 1)(rng);
	P->assign(T->data() + pStart, T->data() + pStart + patternLength);
}

static void shiftBench(benchmark::State& state) {
	std::vector<uint8_t> P, T;
	initStr(state.range(0), &P, &T);
	for (auto _ : state) {
		shift_and(P, T);
	}
}

static void horspoolBench(benchmark::State& state) {
	std::vector<uint8_t> P, T;
	initStr(state.range(0), &P, &T);
	for (auto _ : state) {
		horspool(P, T);
	}
}

static void bndmBench(benchmark::State& state) {
	std::vector<uint8_t> P, T;
	initStr(state.range(0), &P, &T);
	for (auto _ : state) {
		bndm(P, T);
	}
}

BENCHMARK(shiftBench)->DenseRange(2, 64, 2);
BENCHMARK(horspoolBench)->DenseRange(2, 64, 2);
BENCHMARK(bndmBench)->DenseRange(2, 64, 2);

BENCHMARK_MAIN();
