#define _USE_MATH_DEFINES

#include <cstdio>
#include <vector>
#include <omp.h>
#include <random>

typedef long double ld;

using namespace std;

#pragma region Прототипы функций

void monte_karlo();
void heat_equation();

#pragma endregion

int main(int argc, char** argv) {

	heat_equation();

	return 0;
}

#pragma region Теплопроводность

void print_plast(const int N, vector<vector<ld>> v) {
	for (auto i = 1; i <= N; i++) {
		for (auto k = 1; k <= N; k++)
			printf("%-10.4f", v[i][k]);
		printf("\n");
	}
}

void heat_equation() {
	omp_set_num_threads(8);

	const auto tau = 0.00001;
	const auto h = 0.1;
	const int N = 1 / h;
	int T = 1 / tau;

	vector<vector<ld>> u(N + 2, vector<ld>(N + 2, 0));
	auto u_prev = u;
	
	printf("Before:\n");
	print_plast(N, u);

	const auto start = omp_get_wtime();
	while (T--) {
		u_prev[1][1] = 1;
#pragma omp parallel for private(k) collapse(2)
		for (auto i = 1; i <= N; i++) {
			for (auto k = 1; k <= N; k++) {
				const auto sum = u_prev[i + 1][k] + u_prev[i][k + 1]
							   - 4 * u_prev[i][k]
							   + u_prev[i - 1][k] + u_prev[i][k - 1];

				u[i][k] = u_prev[i][k] + tau / (h * h) * sum;
			}
		}
		swap(u, u_prev);
	}

	printf("\nAfter:\n");
	print_plast(N, u_prev);

	printf("\ntime: %g\n", omp_get_wtime() - start);
}

#pragma endregion

#pragma region Монте Карло
double f(const double x) { return x * x; }

void monte_carlo() {
	const auto N = 1000000;

	// функция x*x
	const double a = 0;
	const double b = 1;
	const double y_min = 0;
	const double y_max = 1;

	// Рандом
	random_device rd;
	mt19937 gen(rd());
	const uniform_real_distribution<> rand_x(a, b);
	const uniform_real_distribution<> rand_y(y_min, y_max);


	// Количество потоков	
	omp_set_num_threads(8);

	// Первый тип синхронизации
	auto count = 0;
	auto start = omp_get_wtime();
#pragma omp parallel for reduction(+: count)
	for (auto i = 1; i <= N; i++) {
		const auto x = rand_x(gen);
		const auto y = rand_y(gen);

		if (x <= f(y))
			count++;
	}
	printf("//1//\nres: %f\n", (b - a) * (y_max - y_min) * count / N);
	printf("time: %f\n\n", omp_get_wtime() - start);

	// Второй тип синхронизации
	count = 0;
	start = omp_get_wtime();
#pragma omp parallel for
	for (auto i = 1; i <= N; i++) {
		const auto x = rand_x(gen);
		const auto y = rand_y(gen);
		if (y <= f(x))
#pragma omp atomic
			count++;
	}
	printf("//2//\nres: %f\n", (b - a) * (y_max - y_min) * count / N);
	printf("time: %f\n\n", omp_get_wtime() - start);
}
#pragma endregion 