#include <iostream>
#include <ctime>
#include <vector>
#include <thread>
#include <cassert>
#include <atomic>

void
fill_matrix(int * mtx, size_t mtx_size)
{
  // std::srand(std::time(nullptr));
  std::srand(42);
  for (size_t row = 0; row < mtx_size; ++row)
  {
    int * row_start = mtx + row * mtx_size;
    for (size_t col = 0; col < mtx_size; ++col)
    {
      row_start[col] = std::rand();
    }
  }
}

struct RES
{
  int cnt;
  char payload[160];
};


void
calc_odds(size_t t, RES * thread_total_odds, int * from, int * to)
{
  size_t total_odds = 0;
  thread_total_odds[t].cnt = 0;

  for (int * p = from; p < to; ++p)
  {
    if (*p & 0x1)
    {
      ++thread_total_odds[t].cnt;
    }
  }
}

int
main(int argc, char **argv)
{
  const size_t MTX_SIZE = 10000;
  std::vector<int> mtx_arena(MTX_SIZE * MTX_SIZE);
  int * mtx = &mtx_arena.front();

  fill_matrix(mtx, MTX_SIZE);
  assert(argc == 2);
  const size_t NUM_THREADS = std::strtol(argv[1], nullptr, 10);
  // thread_total_odds.resize(NUM_THREADS);

  std::vector<std::thread> threads;

  auto t_start = std::chrono::high_resolution_clock::now();
  const size_t PART_SIZE = MTX_SIZE * MTX_SIZE / NUM_THREADS;
  RES thread_total_odds[100];

  for (size_t t = 0; t < NUM_THREADS; ++t)
  {
    int * from = mtx + PART_SIZE * t;
    int * to = (t == NUM_THREADS-1 ? mtx + MTX_SIZE * MTX_SIZE : from + PART_SIZE);
    threads.emplace_back(&calc_odds, t, thread_total_odds, from, to);
  }
  for (auto & t : threads)
  {
    t.join();
  }
  auto t_end = std::chrono::high_resolution_clock::now();
  double elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end-t_start).count();

  std::cout << "all threads are joined, elapsed time " << elapsed_time_ms << std::endl;

  size_t total_odds = 0;
  for (size_t t = 0; t < NUM_THREADS; ++t)
  {
    total_odds += thread_total_odds[t].cnt;
  }

  std::cout << total_odds << std::endl;
}
