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

size_t total_odds = 0;

void
calc_odds(int * from, int * to)
{
  for (int * p = from; p < to; ++p)
  {
    if (*p & 0x1)
    {
      ++total_odds;
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

  std::vector<std::thread> threads;

  auto t_start = std::chrono::high_resolution_clock::now();
  const size_t PART_SIZE = MTX_SIZE * MTX_SIZE / NUM_THREADS;

  for (size_t t = 0; t < NUM_THREADS; ++t)
  {
    int * from = mtx + PART_SIZE * t;
    int * to = (t == NUM_THREADS-1 ? mtx + MTX_SIZE * MTX_SIZE : from + PART_SIZE);
    threads.emplace_back(&calc_odds, from, to);
  }
  for (auto & t : threads)
  {
    t.join();
  }
  auto t_end = std::chrono::high_resolution_clock::now();
  double elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end-t_start).count();

  std::cout << "all threads are joined, elapsed time " << elapsed_time_ms << std::endl;

  std::cout << total_odds << std::endl;
}
