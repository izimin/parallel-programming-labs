#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#define MPI_ROOT_PROCESS 0
#include <cstdio>
#include <mpi.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <random>
#include <climits>
#include <cmath>

using namespace std;

#pragma region Прототипы функций
void send_one_message();
void average_send_time();
void average_ssend_time();
void average_bsend_time();
void average_isend_time();
void process_circle();
void dead_end();
void monte_carlo();
void fact();
void collective_operation(); 
void communicators();
#pragma endregion

int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);

#pragma region сданные лабы 
	// Одно сообщение
	//send_one_message();

	// Среднее время работы
	// average_send_time();
	// average_ssend_time();
	// average_bsend_time();
	// average_isend_time();

	// Эстафета
	// process_circle();

	// Тупик
	// dead_end();

	// Монте Карло
	// monte_carlo();

	// Факториал 
	// fact();

	// Коллективная операция Scatterv
	// collective_operation();
#pragma endregion 

	communicators();

	MPI_Finalize();

	return 0;
}

#pragma region Отправка ровно одного сообщения
void send_one_message() {
	int size, rank;

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0) {
		char* buf_send;

		string message = "Hello from " + to_string(rank) + " process";
		buf_send = new char[message.size()+1];
		strcpy(buf_send, message.c_str());

		MPI_Send(buf_send, message.size()+1, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
		printf("I am %d, I sent message \"%s\"\n", rank, buf_send);
	} else if (rank == 1) {
		char* buf_recv;

		MPI_Status status;
		MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
		int length = status.count;
		buf_recv = new char[length];
		MPI_Recv(buf_recv, length, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
		printf("I am %d, I received message \"%s\"\n", rank, buf_recv);
	}
}
#pragma endregion

#pragma region Тупик
void dead_end() {
	int size, rank;

	int send_msg = 1;
	int recv_msg = 0;


	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);


	if (rank == 0) {
		MPI_Recv(&recv_msg, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
		MPI_Send(&send_msg, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		printf("I am %d, recv: %d", rank, recv_msg);
	}
	else if (rank == 1) {
		MPI_Send(&send_msg, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		MPI_Recv(&recv_msg, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
		printf("I am %d, recv: %d", rank, recv_msg);
	}
}
#pragma endregion 

#pragma region Среднее время работы операции MPI_Send
void average_send_time() {
	int size, rank;
	double start;

	int send_msg = 1;
	int recv_msg = 0;

	const auto count_iteration = 100000;

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0) {
		start = MPI_Wtime();

		for (auto i = 0; i < count_iteration; i++) {
			MPI_Send(&send_msg, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		}

		printf("Average time Send %d messages: %g\n", count_iteration, (MPI_Wtime() - start) / (double)count_iteration);
	}
	else if (rank == 1) {
		for (auto i = 0; i < count_iteration; i++) {
			MPI_Recv(&recv_msg, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
		}
	}
}
#pragma endregion 

#pragma region Среднее время работы операции MPI_Ssend
void average_ssend_time() {
	int size, rank;

	auto send_msg = 1;
	auto recv_msg = 0;

	const auto count_iteration = 100000;

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0) {
		auto start = MPI_Wtime();

		for (auto i = 0; i < count_iteration; i++) {
			MPI_Ssend(&send_msg, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		}

		printf("Average time Ssend %d messages: %g\n", count_iteration, (MPI_Wtime() - start) / (double)count_iteration);
	}
	else if (rank == 1) {
		for (auto i = 0; i < count_iteration; i++) {
			MPI_Recv(&recv_msg, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
		}
	}
}
#pragma endregion 

#pragma region Среднее время работы операции MPI_Bsend
void average_bsend_time() {
	int size, rank;

	auto send_msg = 1;
	auto recv_msg = 0;

	const auto count_iteration = 100000;

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0) {
		const auto start = MPI_Wtime();
		int buffer_size = count_iteration * sizeof(int) + MPI_BSEND_OVERHEAD;

		const auto buffer = new int[buffer_size];
		MPI_Buffer_attach(buffer, buffer_size);

		for (auto i = 0; i < count_iteration; i++) {
			MPI_Bsend(&send_msg, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		}

		MPI_Buffer_detach(buffer, &buffer_size);
		delete buffer;
		printf("Average time Bsend %d messages: %g\n", count_iteration, (MPI_Wtime() - start) / static_cast<double>(count_iteration));
	} else if (rank == 1) {
		for (auto i = 0; i < count_iteration; i++) {
			MPI_Recv(&recv_msg, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
		}
	}
}
#pragma endregion 

#pragma region Среднее время работы операции MPI_Isend
void average_isend_time() {
	int size, rank;

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	auto send_msg = 1;

	const auto count_iteration = 100000;

	const auto r = new MPI_Request[count_iteration];
	const auto s = new MPI_Status[count_iteration];

	int* buff = new int[count_iteration];

	if (rank == 0) {
		const auto start = MPI_Wtime();
		for (auto i = 0; i < count_iteration; i++) {
			MPI_Isend(&send_msg, 1, MPI_INT, rank + 1, i, MPI_COMM_WORLD, &r[i]);
		}
		MPI_Waitall(count_iteration, r, s);

		printf("Average time Isend %d messages: %g\n", count_iteration, (MPI_Wtime() - start) / double(count_iteration));
	} else if (rank == 1) {
		for (auto i = 0; i < count_iteration; i++) {
			MPI_Irecv(&buff[i], 1, MPI_INT, rank - 1, i, MPI_COMM_WORLD, &r[i]);
		}
		MPI_Waitall(count_iteration, r, s);
	}
}
#pragma endregion

#pragma region Сообщения по кругу
void process_circle()
{
	int size, rank;
	const auto count_process = 5;
	const auto count_cycle = 2;
	auto msg_send = 1, msg_recv = 0;

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int next = rank == count_process - 1 ? 0 : rank + 1;
	int prev = rank == 0 ? count_process - 1 : rank - 1;

	if (rank == 0) {
		MPI_Send(&msg_send, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
		printf("%d\tto \t %d, \t cycle = %d \t msg = %d\t!!! FIRST !!!\n", rank, next, 1, msg_send);
	}

	for (int i = 0; i < count_cycle; i++) {
		MPI_Recv(&msg_recv, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
		printf("%d\tfrom \t %d, \t cycle = %d \t msg = %d\t%s\n", rank, prev, rank == 0 ? i+2 : i+1, msg_recv, i == count_cycle - 1 && rank == 0 ? "!!! LAST !!!" : "");
		msg_send = msg_recv * 2;
		if (rank == 0 && i == count_cycle-1) break;
		MPI_Send(&msg_send, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
		printf("%d\tto \t %d, \t cycle = %d \t msg = %d\n", rank, next, rank == 0 ? i + 2 : i+1, msg_send);
	}
}
#pragma endregion 

#pragma region Монте Карло (MPI)
double random_double(const double left, const double right) {
	srand(time(nullptr));
	random_device rd;
	default_random_engine re(rd());
	const uniform_real_distribution<double> unif(left, right);
	return unif(re);
}

double func(const double x) {
	return x*x;
}

void monte_carlo() {
	int size, rank;

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	const auto count_points = 100000;

	const auto a = 1.;
	const auto b = 2.;

	if (rank == 0) {
		auto sum = 0.;
		const auto start = MPI_Wtime();
		for (auto i = 0; i < size - 1; i++) {
			auto sum_i = 0.;

			// Принимаем от любого процесса
			MPI_Recv(&sum_i, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			sum += sum_i;
		}

		printf("time: %g\n", MPI_Wtime() - start);
		printf("all points count: %d\n", count_points);
		printf("ans: %f\n", sum * (b - a) / count_points);
	} else {
		double sum = 0;
		auto proc_points = count_points / (size - 1);

		for (auto i = 0; i < proc_points; i++) {
			auto x = random_double(a, b);
			auto y = random_double(1, 4);
			if (y <= func(x)) {
				sum ++;
			}
		}

		// Отправляем 0-му процессу
		MPI_Send(&sum, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	}
}
#pragma endregion 

#pragma region Факториал
void fact()
{
	int rank, size, n = 16;
	double res = 0, sendBuf, recvBuf, buf4;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	/**** 1 stage ****/

	// 0 - 1*2
	// 1 - 3*4
	// 2 - 5*6
	// 3 - 7*8
	// 4 - 9*10
	// 5 - 11*12
	// 6 - 13*14
	// 7 - 15*16
	auto cur = (rank + 1) * n / size - 1;
	res = cur * (cur + 1);
	cur++;

	if (rank == 0) {
		cout << "2! = " << res << endl;
	}

	// 4 - 9*10 to 1 for 4th stage
	if (rank == 4) {
		MPI_Ssend(&res, 1, MPI_DOUBLE, 1, 1, MPI_COMM_WORLD);
	}

	// 1 - 9*10 from 4
	if (rank == 1) {
		MPI_Recv(&buf4, 1, MPI_DOUBLE, 4, 1, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
	}

	/*** 2 stage ***/

	// 0 - 1*2 to 1
	// 2 - 5*6 to 3,  5*6 to 1
	// 4 - 9*10 to 5
	// 6 - 13*14 to 7,   13*14 to 5
	if (rank % 2 == 0) {
		sendBuf = res;
		MPI_Ssend(&sendBuf, 1, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD);
		cur++;
		if (rank % 4 == 2) {
			MPI_Ssend(&sendBuf, 1, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD);
		}

		res *= cur;
		cur += 2;
	}

	// 1 - 1*2 from 0, 5*6 from 2
	// 3 - 5*6 from 2,  
	// 5 - 9*10 from 4,   13*14 from 6
	// 7 - 13*14 from 6 
	else {
		MPI_Recv(&recvBuf, 1, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
		res *= recvBuf;
		if (rank % 4 == 1) {
			MPI_Recv(&recvBuf, 1, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
		}
	}

	if (rank < 2) {
		cout << rank + 3 << "! = " << res << endl;
	}

	// 4 - 9*10*11 to 2
	if (rank == 4) {
		MPI_Ssend(&res, 1, MPI_DOUBLE, 2, 1, MPI_COMM_WORLD);
	}
	if (rank == 2) {
		MPI_Recv(&buf4, 1, MPI_DOUBLE, 4, 1, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
	}

	/*  3 stage  */

	// 0..3 - 1*2*3*4 from 1
	if (rank < 4) {
		if (rank == 1) {
			for (auto i = 0; i < 4; i++) {
				if (i != 1) {
					MPI_Ssend(&res, 1, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
				}
			}
		}
		else {
			MPI_Recv(&recvBuf, 1, MPI_DOUBLE, 1, 1, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
		}
	}

	// 4..7 - 9*10*11*12 from 5
	else {
		if (rank == 5) {
			for (auto i = 4; i < size; i++) {
				if (i != 5) {
					MPI_Ssend(&res, 1, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
				}
			}
		}
		else {
			MPI_Recv(&recvBuf, 1, MPI_DOUBLE, 5, 1, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
		}
	}

	// 5 - 9*10*11*12 to 3
	if (rank == 5) {
		MPI_Ssend(&res, 1, MPI_DOUBLE, 3, 1, MPI_COMM_WORLD);
	}
	if (rank == 3) {
		MPI_Recv(&buf4, 1, MPI_DOUBLE, 5, 1, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
	}

	// 0 and 4 - got 1*2*3*4 and 9*10*11*12, mult cur
	if (rank % 4 == 0) {
		res = recvBuf * cur;
	}

	// остальные процессы умножают свой результат на 1*2*3*4 или на 9*10*11*12 соответственно
	else {
		res *= recvBuf;
	}

	// вывод промежуточных результатов
	if (rank < 4) {
		cout << rank + 5 << "! = " << res << endl;
	}

	/*  4 stage  */
	cur += 4;

	if (rank == 3) {
		recvBuf = res;
	}

	// 8! from 3
	MPI_Bcast(&recvBuf, 1, MPI_DOUBLE, 3, MPI_COMM_WORLD);

	// 0 - 8! * 9
	if (rank == 0) {
		res = recvBuf * cur;
	}
	// 1 - 8! * 9 * 10, 
	// 2 - 8! * 9 * 10 * 11,
	// 3 - 8! * 9 * 10 * 11 * 12
	else if (rank > 0 && rank < 4) {
		res = recvBuf * buf4;
	}
	else {
		res *= recvBuf;
	}

	cout << rank + 9 << "! = " << setprecision(15) << res << endl;
}
#pragma endregion 

#pragma region Коллективная операция MPI_Scatterv
void print_array(int rank, int count, int* buf) {
	printf("%d proc: ", rank);
	for (auto i = 0; i < count; i++) {
		printf("%d ", buf[i]);
	}
	printf("\n");
}

void print_array2(int rank, int count, int* buf) {
	printf("%d proc (2): ", rank);
	for (auto i = 0; i < count; i++) {
		printf("%d ", buf[i]);
	}
	printf("\n");
}

void collective_operation() {
	//MPI_Scatterv - Разбивает буфер по частям (не обязательно равным) на все процессы в коммуникаторе
	
	int size, rank;
	double time = 0, start = 0;

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	const auto count_numbers = 24;

	const auto sendbuf = new int[count_numbers];
	for (auto i = 0; i < count_numbers; i++) {
		sendbuf[i] = i + 1;
	}

	const auto sendcounts = new int[size];
	const auto displs = new int[size];

	const auto recvbuf = new int[count_numbers];

	for (auto i = 0; i < size; i++) {
		sendcounts[i] = i + 1;
		displs[i] = i + rank;
	}

	if (rank == MPI_ROOT_PROCESS) {
		start = MPI_Wtime();

		MPI_Scatterv(sendbuf, sendcounts, displs, MPI_INT, recvbuf, sendcounts[rank],
			MPI_INT, MPI_ROOT_PROCESS, MPI_COMM_WORLD);

		printf("time: %g\n", MPI_Wtime() - start);

		print_array(rank, sendcounts[rank], recvbuf);

	} else {
		MPI_Scatterv(nullptr, nullptr, nullptr, MPI_INT, recvbuf, sendcounts[rank],
			MPI_INT, MPI_ROOT_PROCESS, MPI_COMM_WORLD);

		print_array(rank, sendcounts[rank], recvbuf);
	}

	// Двуточечными
	if (rank == MPI_ROOT_PROCESS) {
		start = MPI_Wtime();

		for (auto i = 1; i < size; i++) {
			MPI_Send(sendbuf + displs[i], sendcounts[i], MPI_INT, i, 0, MPI_COMM_WORLD);
		}

		printf("time(2): %g\n", MPI_Wtime() - start);
	} else {
		MPI_Recv(recvbuf, sendcounts[rank], MPI_INT, MPI_ROOT_PROCESS, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
		print_array2(rank, sendcounts[rank], recvbuf);
	}
}
#pragma endregion 

#pragma region Коммуникаторы 
struct val_rank {
	double val;
	int   rank;
};

void MPIAPI mult(const int* in_param, int* out_param, const int* len, MPI_Datatype* datatype) {
	for (auto i = 0; i < *len; i++) {
		out_param[i] *= in_param[i];
	}
}

void print_array(val_rank arr[], const int n) {
	printf("[ ");
	for (auto i = 0; i < n; i++)
		printf("%.2f ", arr[i].val);
	printf("]\n");
}

void communicators() {
	MPI_Group group1, group2, group_world;
	MPI_Comm comm1;
	MPI_Comm comm2;

	int size, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Comm_group(MPI_COMM_WORLD, &group_world);

	const auto n = 4;

	int ranks_group1[] = { 0, 1, 2, 3 };
	MPI_Group_incl(group_world, n, ranks_group1, &group1);
	MPI_Comm_create(MPI_COMM_WORLD, group1, &comm1);

	int ranks_group2[] = { 4, 5, 6, 7 };
	MPI_Group_incl(group_world, n, ranks_group2, &group2);
	MPI_Comm_create(MPI_COMM_WORLD, group2, &comm2);

	// 1-й коммуникатор (scan + my_op)
	if (comm1 != MPI_COMM_NULL) {
		MPI_Op my_op;

		auto res = 1;
		auto send_data = rank > 0 ? rank + 1 : 1;

		MPI_Op_create(reinterpret_cast<MPI_User_function*>(mult), true, &my_op);
		MPI_Scan(&send_data, &res, 1, MPI_INT, my_op, comm1);
		printf("Comm 1, rank = %d ans: %d", rank, res);
	}

	// 2-й коммуникатор (reduce + numlock)
	if (comm2 != MPI_COMM_NULL) {
		MPI_Comm_rank(comm2, &rank);

		val_rank in[n], out[n];

		for (auto& i : in) {
			i.val = random_double(-10, 10);
			i.rank = rank;
		}

		cout << "Comm 2, rank = " << rank << ", data: ";
		print_array(in, n);

		MPI_Reduce(in, out, n, MPI_DOUBLE_INT, MPI_MINLOC, MPI_ROOT_PROCESS, comm2);
		if (rank == MPI_ROOT_PROCESS) {
			for (auto& i : out) {
				printf("Comm 2, ans: rank = %d, val = %.2f\n", i.rank, i.val);
			}
		}
	}
}
#pragma endregion 

#pragma region Сортировка
int procNumber = 0, procCount = 0;

int* countByProcesses;
int* partsStart;

int* part;
int tag = 0;

void diffParts(int n) {
	for (int i = 0; i < procCount; i++)
		partsStart[i] = (n * i) / procCount;
	for (int i = 0; i < procCount - 1; i++)
		countByProcesses[i] = partsStart[i + 1] - partsStart[i];
	countByProcesses[procCount - 1] = n - partsStart[procCount - 1];
}

void merge2Procs(int i, int j) {
	tag++;
	int currentSize = countByProcesses[procNumber];
	if (j >= procCount)
		return;
	if (j == procNumber) {
		cout << i << " " << j << endl;
		MPI_Send(part, countByProcesses[procNumber], MPI_INT, i, tag, MPI_COMM_WORLD);
		MPI_Recv(part, countByProcesses[procNumber], MPI_INT, i, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	else if (i == procNumber) {
		int anotherSize = countByProcesses[j];
		int* dataFromAnother = new int[anotherSize];
		int* mergeBuffer = new int[anotherSize + currentSize];
		MPI_Recv(dataFromAnother, anotherSize, MPI_INT, j, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		merge(part, part + currentSize, dataFromAnother, dataFromAnother + anotherSize, mergeBuffer);
		for (int c = 0; c < currentSize; c++) part[c] = mergeBuffer[c];
		MPI_Send(mergeBuffer + currentSize, anotherSize, MPI_INT, j, tag, MPI_COMM_WORLD);
	}
}

void batcherSplit(int l, int r, int step) {
	if (l + 2 * step >= r) {
		merge2Procs(l, l + step);
		return;
	}
	batcherSplit(l, r - step, 2 * step);
	batcherSplit(l + step, r, 2 * step);

	for (int i = l + step; i + 2 * step < r; i += 2 * step)
		merge2Procs(i, i + step);
}

void batcherSort(int l, int r)
{
	int m = (r + l) / 2;
	if (m - l > 1)
		batcherSort(l, m);
	if (r - m > 1)
		batcherSort(m, r);
	batcherSplit(l, r, 1);
}

void batcher(int n) {
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &procNumber);
	MPI_Comm_size(MPI_COMM_WORLD, &procCount);
	int n = 16;

	int* a = new int[n];
	countByProcesses = new int[procCount];
	partsStart = new int[procCount];
	diffParts(n);

	int currentCount = countByProcesses[procNumber];
	part = new int[currentCount];
	string before = "BEFORE proc: " + to_string(procNumber) + "\t";
	for (int i = 0; i < currentCount; i++) {
		part[i] = (procNumber * RAND_MAX + rand()) % 100;
		before += to_string(part[i]) + " ";
	}
	cout << before << endl;

	sort(part, part + currentCount);
	batcherSort(0, procCount);
	MPI_Barrier(MPI_COMM_WORLD);


	MPI_Gatherv(part, currentCount, MPI_INT, a, countByProcesses, partsStart, MPI_INT, 0, MPI_COMM_WORLD);

	if (procNumber == 0) {
		cout << "result:\n";
		for (int i = 0; i < n; ++i)
			cout << to_string(a[i]) + " ";
		cout << endl;
	}
}
#pragma endregion 